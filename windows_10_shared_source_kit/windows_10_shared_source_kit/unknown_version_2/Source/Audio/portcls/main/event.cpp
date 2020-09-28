nd loop aL, i0 breakp p0.x block";
string PS_325 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_326_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_326_Desc = "ps_2_sw : dp4 is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string PS_326 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_327_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_327_Desc = "ps_2_sw : dp4 is NOT allowed in a if p0.x nop else and loop aL, i0 break_ge c0.x, c0.y block";
string PS_327 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_328_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_328_Desc = "ps_2_sw : dp4 is allowed in a if p0.x nop else and rep i0 break_ge c0.x, c0.y block";
string PS_328 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_329_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_329_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and if b0 block";
string PS_329 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_330_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_330_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and if b0 nop else block";
string PS_330 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_331_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_331_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and loop aL, i0 block";
string PS_331 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_332_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_332_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and rep i0 block";
string PS_332 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_333_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_333_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and if_ne c0.x, c0.y block";
string PS_333 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if_ne c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_334_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_334_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and if_ne c0.x, c0.y nop else block";
string PS_334 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if_ne c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_335_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_335_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and if p0.x block";
string PS_335 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_336_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_336_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_336 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_337_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_337_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_337 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_338_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_338_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and rep i0 break block";
string PS_338 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_339_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_339_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_339 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_340_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_340_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_340 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_341_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_341_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and loop aL, i0 break_ge c0.x, c0.y block";
string PS_341 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_342_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_342_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break and rep i0 break_ge c0.x, c0.y block";
string PS_342 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_343_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_343_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break and if b0 block";
string PS_343 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_344_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_344_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break and if b0 nop else block";
string PS_344 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_345_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_345_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 break and loop aL, i0 block";
string PS_345 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_346_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_346_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break and rep i0 block";
string PS_346 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_347_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_347_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break and if_ne c0.x, c0.y block";
string PS_347 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if_ne c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_348_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_348_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break and if_ne c0.x, c0.y nop else block";
string PS_348 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if_ne c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_349_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_349_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break and if p0.x block";
string PS_349 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_350_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_350_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break and if p0.x nop else block";
string PS_350 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_351_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_351_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 break and loop aL, i0 break block";
string PS_351 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_352_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_352_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break and rep i0 break block";
string PS_352 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_353_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_353_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string PS_353 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_354_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_354_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break and rep i0 breakp p0.x block";
string PS_354 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_355_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_355_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 break and loop aL, i0 break_ge c0.x, c0.y block";
string PS_355 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_356_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_356_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break and rep i0 break_ge c0.x, c0.y block";
string PS_356 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_357_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_357_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and if b0 block";
string PS_357 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_358_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_358_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string PS_358 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_359_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_359_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string PS_359 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_360_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_360_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string PS_360 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_361_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_361_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and if_gt c0.x, c0.y block";
string PS_361 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if_gt c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_362_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_362_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and if_gt c0.x, c0.y nop else block";
string PS_362 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if_gt c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_363_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_363_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string PS_363 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_364_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_364_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string PS_364 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_365_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_365_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string PS_365 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_366_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_366_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string PS_366 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_367_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_367_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_367 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_368_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_368_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_368 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_369_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_369_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_ge c0.x, c0.y block";
string PS_369 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_370_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_370_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break_ge c0.x, c0.y block";
string PS_370 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_371_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_371_Desc = "ps_2_sw : dp4 is allowed in a rep i0 breakp p0.x and if b0 block";
string PS_371 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_372_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_372_Desc = "ps_2_sw : dp4 is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string PS_372 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_373_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_373_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string PS_373 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_374_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_374_Desc = "ps_2_sw : dp4 is allowed in a rep i0 breakp p0.x and rep i0 block";
string PS_374 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_375_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_375_Desc = "ps_2_sw : dp4 is allowed in a rep i0 breakp p0.x and if_eq c0.x, c0.y block";
string PS_375 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if_eq c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_376_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_376_Desc = "ps_2_sw : dp4 is allowed in a rep i0 breakp p0.x and if_eq c0.x, c0.y nop else block";
string PS_376 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if_eq c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_377_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_377_Desc = "ps_2_sw : dp4 is allowed in a rep i0 breakp p0.x and if p0.x block";
string PS_377 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_378_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_378_Desc = "ps_2_sw : dp4 is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string PS_378 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_379_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_379_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string PS_379 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_380_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_380_Desc = "ps_2_sw : dp4 is allowed in a rep i0 breakp p0.x and rep i0 break block";
string PS_380 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_381_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_381_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_381 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_382_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_382_Desc = "ps_2_sw : dp4 is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_382 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_383_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_383_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break_ge c0.x, c0.y block";
string PS_383 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_384_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_384_Desc = "ps_2_sw : dp4 is allowed in a rep i0 breakp p0.x and rep i0 break_ge c0.x, c0.y block";
string PS_384 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_385_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_385_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and if b0 block";
string PS_385 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_386_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_386_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and if b0 nop else block";
string PS_386 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_387_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_387_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 block";
string PS_387 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_388_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_388_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 block";
string PS_388 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_389_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_389_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and if_eq c0.x, c0.y block";
string PS_389 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_390_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_390_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and if_eq c0.x, c0.y nop else block";
string PS_390 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if_eq c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_391_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_391_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and if p0.x block";
string PS_391 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_392_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_392_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and if p0.x nop else block";
string PS_392 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_393_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_393_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 break block";
string PS_393 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_394_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_394_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 break block";
string PS_394 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_395_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_395_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_395 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_396_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_396_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 breakp p0.x block";
string PS_396 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_397_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_397_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 break_ge c0.x, c0.y block";
string PS_397 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_398_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_398_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 break_ge c0.x, c0.y block";
string PS_398 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_399_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_399_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break_ge c0.x, c0.y and if b0 block";
string PS_399 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_400_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_400_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break_ge c0.x, c0.y and if b0 nop else block";
string PS_400 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_401_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_401_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 block";
string PS_401 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_402_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_402_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 block";
string PS_402 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_403_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_403_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break_ge c0.x, c0.y and if_lt c0.x, c0.y block";
string PS_403 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_404_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_404_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break_ge c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string PS_404 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_405_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_405_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break_ge c0.x, c0.y and if p0.x block";
string PS_405 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_406_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_406_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break_ge c0.x, c0.y and if p0.x nop else block";
string PS_406 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_407_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_407_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 break block";
string PS_407 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_408_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_408_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 break block";
string PS_408 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_409_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_409_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_409 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_410_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_410_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 breakp p0.x block";
string PS_410 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_411_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_411_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 break_ge c0.x, c0.y block";
string PS_411 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_412_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_412_Desc = "ps_2_sw : dp4 is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 break_ge c0.x, c0.y block";
string PS_412 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";


/*****************************************************************************
 * event.cpp - event support
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#include "private.h"





/*****************************************************************************
 * Functions
 */

#pragma code_seg("PAGE")

/*****************************************************************************
 * CheckEvent
 *****************************************************************************
 * Private function that checks the event property if it contains a node ID
 * and returns the node ID.
 *
 * Irp            the KS IOCTL
 * NodesCount     count of nodes on the filter
 * IsNodeEvent    set to TRUE if event is directed at a node, otherwise FALSE
 * NodeId         set to the node ID of the property
 */
NTSTATUS CheckEvent (IN PIRP Irp, IN ULONG NodesCount, OUT PBOOLEAN IsNodeEvent,
    OUT PULONG NodeId)
{
    PIO_STACK_LOCATION IrpStack;
    ULONG InputBufferLength;
    ULONG OutputBufferLength;
    ULONG AlignedBufferLength;

    PAGED_CODE();

    ASSERT (Irp);
    ASSERT (NodeId);
    ASSERT (IsNodeEvent);

    //
    // ---- CUT/PASTE snipped from KspEnableEvent -----
    // This code touches the event property and extracts the node ID (if present).
    //
    *IsNodeEvent = FALSE;

    //
    // Determine the offsets to both the Event and EventData parameters based
    // on the lengths of the DeviceIoControl parameters. A single allocation is
    // used to buffer both parameters. The EventData (or results on a support
    // query) is stored first, and the Event is stored second, on
    // FILE_QUAD_ALIGNMENT.
    //
    IrpStack = IoGetCurrentIrpStackLocation(Irp);
    InputBufferLength = IrpStack->Parameters.DeviceIoControl.InputBufferLength;
    OutputBufferLength = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    AlignedBufferLength = (OutputBufferLength + FILE_QUAD_ALIGNMENT) & ~FILE_QUAD_ALIGNMENT;

    //
    // Determine if the parameters have already been buffered by a previous
    // call to this function.
    //
    if (!Irp->AssociatedIrp.SystemBuffer) {
        //
        // Initially just check for the minimal event parameter length. The
        // actual minimal length will be validated when the event item is found.
        // Also ensure that the output and input buffer lengths are not set so
        // large as to overflow when aligned or added.
        //
        if ((InputBufferLength < sizeof(KSEVENT)) || (AlignedBufferLength < OutputBufferLength) || (AlignedBufferLength + InputBufferLength < AlignedBufferLength)) {
            return STATUS_INVALID_BUFFER_SIZE;
        }

        // Allocate a local buffer for the event
        PKSEVENT Event = (PKSEVENT)ExAllocatePoolWithTag(NonPagedPoolNx, InputBufferLength, 'tEcP');
        if(NULL == Event) {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        __try {
            //
            // Validate the pointers if the client is not trusted.
            //
            if (Irp->RequestorMode != KernelMode) {
                ProbeForRead(IrpStack->Parameters.DeviceIoControl.Type3InputBuffer, InputBufferLength, sizeof(BYTE));
            }

            //
            // From now on we look at the KSEVENT structure and extract the node ID
            // if KSEVENT_TYPE_TOPOLOGY is set. This is done within the try-except
            // because this is a potentially untrusted buffer.
            //

            // 
            // Start by making a local copy
            // 
            RtlCopyMemory(PVOID(Event), PVOID(IrpStack->Parameters.DeviceIoControl.Type3InputBuffer), InputBufferLength );

            if (Event->Flags & KSEVENT_TYPE_TOPOLOGY) {
                //
                // Input buffer must include the node ID.
                //
                PKSE_NODE nodeEvent = (PKSE_NODE) Event;
                if (InputBufferLength < sizeof(*nodeEvent)) {
                    ExFreePoolWithTag(Event, 'tEcP');
                    return STATUS_INVALID_BUFFER_SIZE;
                }
                if (nodeEvent->NodeId >= NodesCount) {
                    ExFreePoolWithTag(Event, 'tEcP');
                    return STATUS_INVALID_DEVICE_REQUEST;
                }
                *NodeId = nodeEvent->NodeId;
                *IsNodeEvent = TRUE;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            ExFreePoolWithTag(Event, 'tEcP');
            return GetExceptionCode();
        }

        ExFreePoolWithTag(Event, 'tEcP');
    }
    else
    {
        //
        // Retrieve a pointer to the Event, which is on FILE_LONG_ALIGNMENT within
        // the SystemBuffer, after any EventData.
        //
        PKSEVENT Event = (PKSEVENT)((PUCHAR)Irp->AssociatedIrp.SystemBuffer + AlignedBufferLength);

        //
        // Extract the node ID if present.
        //
        if (Event->Flags & KSEVENT_TYPE_TOPOLOGY) {
            //
            // Input buffer must include the node ID.
            //
            PKSE_NODE nodeEvent = (PKSE_NODE) Event;
            if (InputBufferLength < sizeof(*nodeEvent)) {
                return STATUS_INVALID_BUFFER_SIZE;
            }
            if (nodeEvent->NodeId >= NodesCount) {
                return STATUS_INVALID_DEVICE_REQUEST;
            }
            *NodeId = nodeEvent->NodeId;
            *IsNodeEvent = TRUE;
        }
    }

    return STATUS_SUCCESS;
}

/*****************************************************************************
 * PcHandleEnableEventWithTable()
 *****************************************************************************
 * Uses an event table to handle a KS enable event IOCTL.
 * This function uses the event table stored in pContext if the event is
 * directed at the filter/pin, otherwise uses the NodeEventTables if the IOCTL
 * was directed at a node.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcHandleEnableEventWithTable
(
    IN      PIRP                    pIrp,
    IN      PEVENT_CONTEXT          pContext,
    IN      ULONG                   NodeCount,
    IN      PEVENT_TABLE            NodeEventTables
)
{
    PAGED_CODE();

    ASSERT (pIrp);
    ASSERT (pContext);

    BOOLEAN IsNodeEvent;
    ULONG   NodeId;

    _DbgPrintF(DEBUGLVL_BLAB,("PcHandleEnableEventWithTable"));

    pIrp->Tail.Overlay.DriverContext[3] = pContext;

    NTSTATUS ntStatus = CheckEvent (pIrp, NodeCount, &IsNodeEvent, &NodeId);
    if (NT_SUCCESS(ntStatus))
    {
        if (IsNodeEvent)
        {
            ASSERT (NodeEventTables);
            pContext->ulEventSetCount = NodeEventTables[NodeId].EventSetCount;
            pContext->pEventSets = NodeEventTables[NodeId].EventSets;
        }

        ntStatus = KsEnableEvent (pIrp,
                           pContext->ulEventSetCount,
                           pContext->pEventSets,
                           NULL,
                           KSEVENTS_NONE,
                           NULL );
    }

    return ntStatus;

}

/*****************************************************************************
 * PcHandleDisableEventWithTable()
 *****************************************************************************
 * Uses an event table to handle a KS disable event IOCTL.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcHandleDisableEventWithTable
(
    IN      PIRP                    pIrp,
    IN      PEVENT_CONTEXT          pContext
)
{
    PAGED_CODE();

    ASSERT(pIrp);
    ASSERT(pContext);

    _DbgPrintF(DEBUGLVL_VERBOSE,("PcHandleDisableEventWithTable"));

    pIrp->Tail.Overlay.DriverContext[3] = pContext;

    return KsDisableEvent( pIrp,
                           &(pContext->pEventList->List),
                           KSEVENTS_SPINLOCK,
                           &(pContext->pEventList->ListLock) );
}

/*****************************************************************************
 * EventItemAddHandler()
 *****************************************************************************
 * KS-sytle event handler that handles Adds using the
 * PCEVENT_ITEM mechanism. Note that filter and pin events in the port do not
 * utilize this AddHandler, only events exposed by the miniport.
 */
NTSTATUS
EventItemAddHandler
(
    IN PIRP                     pIrp,
    IN PKSEVENTDATA             pEventData,
    IN PKSEVENT_ENTRY           pEventEntry
)
{
    PAGED_CODE();

    ASSERT(pIrp);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    _DbgPrintF(DEBUGLVL_VERBOSE,("EventItemAddHandler"));

    // get the IRP stack location
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation( pIrp );

    // get the event context
    PEVENT_CONTEXT pContext = PEVENT_CONTEXT(pIrp->Tail.Overlay.DriverContext[3]);

    //
    // Setup event request structure
    //
    PPCEVENT_REQUEST pPcEventRequest = new(NonPagedPoolNx,'rEcP') PCEVENT_REQUEST;

    if( !pPcEventRequest )
    {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }
    else
    {
        ASSERT(pIrp->AssociatedIrp.SystemBuffer);

        // IOCTL_KS_ENABLE_EVENT is a METHOD_NEITHER CTL_CODE, so the input buffer is
        // at  Parameters.DeviceIoControl.Type3InputBuffer with size Parameters.DeviceIoControl.InputBufferLength 
        // ... and the output buffer is at  Irp->UserBuffer with size  and Parameters.DeviceIoControl.OutputBufferLength 
        PKSEVENT  pKse = (PKSEVENT)irpSp->Parameters.DeviceIoControl.Type3InputBuffer;
        ULONG     cbKse = irpSp->Parameters.DeviceIoControl.InputBufferLength;

        if (cbKse == sizeof(KSE_NODE))
        {
            // Based on the size, the KSEVENT structure is actually a KSE_NODE or KSE_PIN.
            // These two structures are identical except for the ULONG context field (NodeId or PinId)
            // and can be used interchangeably.  
            // PCEVENT_REQUEST has a NodeId field, but not PinId field.  We will use this field to hold both
            // PinIds and NodeIds and use the KSEVENT_TYPE_TOPOLOGY flag to indicate the interpretation.
            pPcEventRequest->Node = ((PKSE_NODE)pKse)->NodeId;
        }

        //
        // Copy target information from the context structure (node taken care of above)
        //
        pPcEventRequest->MajorTarget    = pContext->pPropertyContext->pUnknownMajorTarget;
        pPcEventRequest->MinorTarget    = pContext->pPropertyContext->pUnknownMinorTarget;
        pPcEventRequest->EventItem      = NULL;

        // get the filter descriptor
        PPCFILTER_DESCRIPTOR pPcFilterDescriptor = pContext->pPropertyContext->pPcFilterDescriptor;

        if ((pKse->Flags & KSEVENT_TYPE_TOPOLOGY) == 0)
        {
            if( !pPcEventRequest->MinorTarget )
            {
                //
                // FILTER EVENT
                //

                if( ( pPcFilterDescriptor ) &&
          