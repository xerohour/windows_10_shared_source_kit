-1, -1, 0, -1, 0.f };
string PS_330_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and if b0 block";
string PS_330 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if b0 "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_331_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_331_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and if b0 nop else block";
string PS_331 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_332_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_332_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and loop aL, i0 block";
string PS_332 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_333_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_333_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and rep i0 block";
string PS_333 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_334_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_334_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and if_le c0.x, c0.y block";
string PS_334 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_335_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_335_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and if_le c0.x, c0.y nop else block";
string PS_335 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if_le c0.x, c0.y nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_336_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_336_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and if p0.x block";
string PS_336 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if p0.x "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_337_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_337_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_337 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_338_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_338_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_338 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_339_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_339_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and rep i0 break block";
string PS_339 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_340_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_340_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_340 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_341_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_341_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_341 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_342_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_342_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and loop aL, i0 break_le c0.x, c0.y block";
string PS_342 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_343_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_343_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break and rep i0 break_le c0.x, c0.y block";
string PS_343 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_344_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_344_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break and if b0 block";
string PS_344 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if b0 "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_345_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_345_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break and if b0 nop else block";
string PS_345 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if b0 nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_346_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_346_Desc = "ps_2_sw : setp_gt is NOT allowed in a rep i0 break and loop aL, i0 block";
string PS_346 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_347_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_347_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break and rep i0 block";
string PS_347 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_348_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_348_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break and if_le c0.x, c0.y block";
string PS_348 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_349_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_349_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break and if_le c0.x, c0.y nop else block";
string PS_349 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if_le c0.x, c0.y nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_350_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_350_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break and if p0.x block";
string PS_350 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if p0.x "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_351_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_351_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break and if p0.x nop else block";
string PS_351 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if p0.x nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_352_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_352_Desc = "ps_2_sw : setp_gt is NOT allowed in a rep i0 break and loop aL, i0 break block";
string PS_352 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_353_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_353_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break and rep i0 break block";
string PS_353 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 break "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_354_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_354_Desc = "ps_2_sw : setp_gt is NOT allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string PS_354 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_355_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_355_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break and rep i0 breakp p0.x block";
string PS_355 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_356_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_356_Desc = "ps_2_sw : setp_gt is NOT allowed in a rep i0 break and loop aL, i0 break_le c0.x, c0.y block";
string PS_356 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_357_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_357_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break and rep i0 break_le c0.x, c0.y block";
string PS_357 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_358_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_358_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and if b0 block";
string PS_358 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_359_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_359_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string PS_359 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_360_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_360_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string PS_360 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_361_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_361_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string PS_361 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_362_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_362_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and if_le c0.x, c0.y block";
string PS_362 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_363_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_363_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and if_le c0.x, c0.y nop else block";
string PS_363 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if_le c0.x, c0.y nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_364_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_364_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string PS_364 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_365_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_365_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string PS_365 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_366_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_366_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string PS_366 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_367_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_367_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string PS_367 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_368_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_368_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_368 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_369_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_369_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_369 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_370_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_370_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_le c0.x, c0.y block";
string PS_370 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_371_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_371_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break_le c0.x, c0.y block";
string PS_371 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_372_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_372_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 breakp p0.x and if b0 block";
string PS_372 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_373_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_373_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string PS_373 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_374_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_374_Desc = "ps_2_sw : setp_gt is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string PS_374 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_375_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_375_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 breakp p0.x and rep i0 block";
string PS_375 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_376_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_376_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 breakp p0.x and if_le c0.x, c0.y block";
string PS_376 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_377_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_377_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 breakp p0.x and if_le c0.x, c0.y nop else block";
string PS_377 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if_le c0.x, c0.y nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_378_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_378_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 breakp p0.x and if p0.x block";
string PS_378 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_379_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_379_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string PS_379 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_380_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_380_Desc = "ps_2_sw : setp_gt is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string PS_380 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_381_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_381_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 breakp p0.x and rep i0 break block";
string PS_381 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_382_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_382_Desc = "ps_2_sw : setp_gt is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_382 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_383_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_383_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_383 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_384_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_384_Desc = "ps_2_sw : setp_gt is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break_le c0.x, c0.y block";
string PS_384 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_385_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_385_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 breakp p0.x and rep i0 break_le c0.x, c0.y block";
string PS_385 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_386_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_386_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and if b0 block";
string PS_386 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if b0 "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_387_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_387_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and if b0 nop else block";
string PS_387 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if b0 nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_388_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_388_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 block";
string PS_388 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_389_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_389_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 block";
string PS_389 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_390_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_390_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and if_le c0.x, c0.y block";
string PS_390 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_391_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_391_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and if_le c0.x, c0.y nop else block";
string PS_391 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if_le c0.x, c0.y nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_392_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_392_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and if p0.x block";
string PS_392 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if p0.x "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_393_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_393_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and if p0.x nop else block";
string PS_393 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if p0.x nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_394_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_394_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 break block";
string PS_394 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_395_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_395_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 break block";
string PS_395 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 break "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_396_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_396_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_396 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_397_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_397_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 breakp p0.x block";
string PS_397 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_398_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_398_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 break_le c0.x, c0.y block";
string PS_398 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_399_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_399_Desc = "ps_2_sw : setp_gt is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 break_le c0.x, c0.y block";
string PS_399 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_400_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_400_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break_le c0.x, c0.y and if b0 block";
string PS_400 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	if b0 "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_401_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_401_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break_le c0.x, c0.y and if b0 nop else block";
string PS_401 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	if b0 nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_402_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_402_Desc = "ps_2_sw : setp_gt is NOT allowed in a rep i0 break_le c0.x, c0.y and loop aL, i0 block";
string PS_402 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	loop aL, i0 "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_403_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_403_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break_le c0.x, c0.y and rep i0 block";
string PS_403 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_404_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_404_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break_le c0.x, c0.y and if_le c0.x, c0.y block";
string PS_404 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	if_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_405_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_405_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break_le c0.x, c0.y and if_le c0.x, c0.y nop else block";
string PS_405 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	if_le c0.x, c0.y nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_406_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_406_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break_le c0.x, c0.y and if p0.x block";
string PS_406 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	if p0.x "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_407_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_407_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break_le c0.x, c0.y and if p0.x nop else block";
string PS_407 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	if p0.x nop else "
	"		setp_gt p0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_408_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_408_Desc = "ps_2_sw : setp_gt is NOT allowed in a rep i0 break_le c0.x, c0.y and loop aL, i0 break block";
string PS_408 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	loop aL, i0 break "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_409_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_409_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break_le c0.x, c0.y and rep i0 break block";
string PS_409 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_410_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_410_Desc = "ps_2_sw : setp_gt is NOT allowed in a rep i0 break_le c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_410 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_411_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_411_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break_le c0.x, c0.y and rep i0 breakp p0.x block";
string PS_411 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_412_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_412_Desc = "ps_2_sw : setp_gt is NOT allowed in a rep i0 break_le c0.x, c0.y and loop aL, i0 break_le c0.x, c0.y block";
string PS_412 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_413_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_413_Desc = "ps_2_sw : setp_gt is allowed in a rep i0 break_le c0.x, c0.y and rep i0 break_le c0.x, c0.y block";
string PS_413 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endrep "
	"endrep ";



// Auto-Generated pch file from the following files:
// cnsurface.cpp, CSurface6.cpp, CSurface7.cpp, CSurface.cpp, CSurface8.cpp, CSurface9.cpp, CVolume.cpp, CVolume8.cpp, CVolume9.cpp

#define STRICT
#include "CSurface.h"		// referenced 9 times
#include "CTextures.h"		// referenced 4 times
#include "CD3DX.h"		// referenced 4 times
#include <float.h>
//#include "d3dx9.h"		// referenced 2 times
//#include "d3d.h"		// referenced 2 times
//#include <d3dx8.h>		// referenced 2 times
!include ..\..\conf.mk

TARGETNAME=DynamicResources

SOURCES= \
		..\DynamicResources.rc \
		..\EntryPoint.cpp \
		..\DynamicVB.cpp \
		..\StaticDynamicVB.cpp \
		..\DynamicTexture.cpp \

SOURCES=$(SOURCES) ..\kitContent.kml

/*++

Copyright (c) Microsoft Corporation

Module Name:

    config.c

Abstract:

    This module will build the loader's configuration tree for an EFI
    boot application.

Environment:

    Boot Environment.

Revision History:

--*/

// ------------------------------------------------------------------- Includes

#include "osloader.h"
#include <ntacpi.h>

// ---------------------------------------------------------------- Definitions

#define MACHINE_IDENTIFIER_X86  "AT/AT COMPATIBLE"
#define MACHINE_IDENTIFIER_ARM  "ARM processor family"
#define MACHINE_IDENTIFIER_UNKNOWN "Unknown Machine Type"

// ----------------------------------------------------------------- Prototypes

NTSTATUS
OslpInitializeControllerNode (
    __in CONFIGURATION_CLASS Class,
    __in CONFIGURATION_TYPE Type,
    __in ULONG AffinityMask,
    __in_bcount_opt(IdentifierLength) PCHAR IdentifierString,
    __in size_t IdentifierLength,
    __deref_out PCONFIGURATION_COMPONENT_DATA *ControllerNode
    );

NTSTATUS
OslpCleanupControllerNode (
    __inout PCONFIGURATION_COMPONENT_DATA ControllerNode
    );

VOID
OslpCollectInt13DriveParameters (
    __in ULONG DiskCount,
    __out_ecount(DiskCount) PCM_INT13_DRIVE_PARAMETER ParameterBuffer,
    __in_ecount(DiskCount) PDEVICE_ID DiskIdList
    );

NTSTATUS
OslpBuildDiskControllerTree (
    __deref_out PCONFIGURATION_COMPONENT_DATA *DiskRootEntry,
    __in PGENERIC_BUFFER DiskIdBuffer
    );

NTSTATUS
OslpBuildAcpiBiosNode (
    __deref_out PCONFIGURATION_COMPONENT_DATA *AcpiEntry
    );

NTSTATUS
OslpBuildKeyboardControllerNode (
    __deref_out PCONFIGURATION_COMPONENT_DATA *KeyboardEntry
    );

NTSTATUS
OslpCreateFloppyPeripheralEntry (
    __in DEVICE_ID DeviceId,
    __deref_out PCONFIGURATION_COMPONENT_DATA *FloppyPeripheralEntry
    );

NTSTATUS
OslpCreateDiskPeripheralEntry (
    __in DEVICE_ID DeviceId,
    __in ULONG EnumNumber,
    __deref_out PCONFIGURATION_COMPONENT_DATA *DiskPeripheralEntry
    );

NTSTATUS
OslpBuildSystemNode (
    __deref_out PCONFIGURATION_COMPONENT_DATA *SystemEntry,
    __in PGENERIC_BUFFER DiskIdBuffer
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
OslLegacyHardwareDetection (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine will build a configuration tree for the loader block.

    It is the goal of this routine to build a minimal configuration tree.
    The structure of that tree is as follows:

        - System Class

            - ISA Adapter

                - Disk Information

            - ACPI (RSDT Address).

Arguments:

    LoaderBlock - A pointer to the argument structure that will eventually
        be passed into the kernel.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if an allocation fails.
    STATUS_INVALID_PARAMETER if the firmware configuration tree is malformed.

--*/

{

    PCONFIGURATION_COMPONENT_DATA ConfigurationRoot;
    PCONFIGURATION_COMPONENT_DATA CurrentEntry;
    GENERIC_BUFFER DiskIdBuffer;
    BOOT_ENVIRONMENT_DEVICE EnumerateDeviceId;
    PCONFIGURATION_COMPONENT_DATA ParentEntry;
    PCONFIGURATION_COMPONENT_DATA SiblingEntry;
    NTSTATUS Status;

    ConfigurationRoot = NULL;

    //
    // Enumerate the hard disks present in the machine.  The list of hard
    // disks is used at multiple points during configuration tree construction
    // and thus must be computed before configuration tree construction
    // begins.
    //

    RtlZeroMemory(&EnumerateDeviceId, sizeof(BOOT_ENVIRONMENT_DEVICE));
    EnumerateDeviceId.DeviceType = DEVICE_BLOCK_IO;
    EnumerateDeviceId.Size = sizeof(BOOT_ENVIRONMENT_DEVICE);
    EnumerateDeviceId.u.BlockIo.BlockIoType = BLOCK_IO_HARD_DISK;
    DiskIdBuffer.Buffer = NULL;
    DiskIdBuffer.BufferSize = 0;
    Status = BlDeviceEnumerateDeviceClass(&EnumerateDeviceId,
                                          &DiskIdBuffer,
                                          TRUE);

    if (!NT_SUCCESS(Status)) {
        goto OslLegacyHardwareDetectionEnd;
    }

    //
    // The root of the configuration tree is a system class node.
    //

    Status = OslpBuildSystemNode(&ConfigurationRoot, &DiskIdBuffer);
    if (!NT_SUCCESS(Status)) {
        goto OslLegacyHardwareDetectionEnd;
    }

    ParentEntry = ConfigurationRoot;

    //
    // Create an ISA adapter node.
    //

    Status = OslpInitializeControllerNode(
                                AdapterClass,
                                MultiFunctionAdapter,
                                0xffffffff,
                                "ISA",
                                sizeof("ISA"),
                                &CurrentEntry);

    if (!NT_SUCCESS(Status)) {
        goto OslLegacyHardwareDetectionEnd;
    }

    //
    // This node should be inserted into the tree as a child of the
    // root.
    //

    CurrentEntry->Parent = ParentEntry;
    ParentEntry->Child   = CurrentEntry;
    ParentEntry = CurrentEntry;  // ParentEntry = ISA Adapter.

    //
    // Collect Disk Geometry information.
    //

    Status = OslpBuildDiskControllerTree(&CurrentEntry, &DiskIdBuffer);
    if (!NT_SUCCESS(Status)) {
        goto OslLegacyHardwareDetectionEnd;
    }

    //
    // Insert the disk information as a child to the ISA Adapter.
    //

    CurrentEntry->Parent = ParentEntry;
    ParentEntry->Child = CurrentEntry;

    //
    // Collect keyboard controller information.
    //

    SiblingEntry = CurrentEntry;
    Status = OslpBuildKeyboardControllerNode(&CurrentEntry);
    if (!NT_SUCCESS(Status)) {
        goto OslLegacyHardwareDetectionEnd;
    }

    //
    // Insert keyboard node as a child to the ISA Adapter and as a
    // sibling of the disk node.
    //

    CurrentEntry->Parent = SiblingEntry->Parent;
    SiblingEntry->Sibling = CurrentEntry;

    //
    // Create ACPI Bios node
    //

    Status = OslpBuildAcpiBiosNode(&CurrentEntry);
    if (NT_SUCCESS(Status)) {

        //
        // Adjust pointers to be relative to the System Class Node to insert
        // the ACPI Bios node.
        //

        SiblingEntry = ParentEntry;
        ParentEntry = ParentEntry->Parent;

        //
        // Insert ACPI node as a child of the system class and a sibling to
        // the ISA Adapter Node.
        //

        CurrentEntry->Parent = ParentEntry;
        SiblingEntry->Sibling = CurrentEntry;

    } else {
        goto OslLegacyHardwareDetectionEnd;
    }

    //
    // We were successful in creating a configuration tree.  Pass this to
    // the kernel via the loader block.
    //

    LoaderBlock->ConfigurationRoot = ConfigurationRoot;

OslLegacyHardwareDetectionEnd:
    if (DiskIdBuffer.BufferSize != 0) {
        BlMmFreeHeap(DiskIdBuffer.Buffer);
    }

    if (!NT_SUCCESS(Status)) {

        //
        // In the event of an error, the configuration tree should be
        // freed.
        //

        OslpCleanupControllerNode(ConfigurationRoot);
    }

    return Status;
}

NTSTATUS
OslpBuildSystemNode (
    __deref_out PCONFIGURATION_COMPONENT_DATA *SystemEntry,
    __in PGENERIC_BUFFER DiskIdBuffer
    )

/*++

Routine Description:

    This routine will build configuration tree entries for the root
    system node.  It handles node initialization and is also responsible
    for populating the relevant ConfigurationData for the system node.

Arguments:

    SystemEntry - On output, contains a pointer to the root of the
        configuration tree.

    DiskIdBuffer - Supplies a generic buffer listing the device IDs for all
        hard disks enumerated by the boot environment.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if an allocation fails.
    STATUS_INVALID_PARAMETER if the firmware configuration tree is malformed.

--*/

{

    PCONFIGURATION_COMPONENT Component;
    ULONG ConfigurationDataLength;
    PCONFIGURATION_COMPONENT_DATA ConfigurationRoot;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor;
    ULONG DiskCount;
    ULONG DiskInformationLength;
    PCHAR MachineType;
    PCM_PARTIAL_RESOURCE_LIST ResourceList;
    NTSTATUS Status;
    PVOID Target;
    PCM_ROM_BLOCK VideoRomBlock;
    ULONG VideoRomBlockCount;
    BOOLEAN VideoRomBlockPresent;

    VideoRomBlock = NULL;

    //
    // Root of configuration tree is a system class node.
    //

    ConfigurationRoot = NULL;
#pragma prefast(suppress: 6285, "logical-or of constants is by design")
    if ((BOOT_APPLICATION_MACHINE_TYPE == IMAGE_FILE_MACHINE_I386) ||
        (BOOT_APPLICATION_MACHINE_TYPE == IMAGE_FILE_MACHINE_AMD64)) {

        MachineType = MACHINE_IDENTIFIER_X86;

#pragma prefast(suppress: 6285, "logical-or of constants is by design")
    } else if ((BOOT_APPLICATION_MACHINE_TYPE == IMAGE_FILE_MACHINE_ARMNT) ||
               (BOOT_APPLICATION_MACHINE_TYPE == IMAGE_FILE_MACHINE_ARM64)) {
        MachineType = MACHINE_IDENTIFIER_ARM;

    } else {

        ASSERT(FALSE);

        MachineType = MACHINE_IDENTIFIER_UNKNOWN;
    }

    Status = OslpInitializeControllerNode(
                        SystemClass,
                        MaximumType,
                        0,
                        MachineType,
                        strlen(MachineType)+1,
                        &ConfigurationRoot);

    if (!NT_SUCCESS(Status)) {
        goto BuildSystemNodeEnd;
    }

    Component = &ConfigurationRoot->ComponentEntry;

    //
    // The ConfigurationData for the root entry is a :
    //
    //         CM_PARTIAL_RESOURCE_LIST header
    //         CM_PARTIAL_RESOURCE_DESCRIPTOR for disk parameter table.
    //         CM_PARTIAL_RESOURCE_DESCRIPTOR for Video Rom blocks.
    //
    // The CM_PARTIAL_RESOURCE_LIST header contains a single partial resource
    // descriptor structure, so extra space is needed for only one of the two
    // resource descriptors.
    //

    //
    // Code in disk.sys consumes disk parameters encoded into the
    // configuration data element attached to the system node.  In addition,
    // the firmware specific implementation must be queried to find out if
    // video rom blocks are present, as these are needed to allow VGA and 16
    // bit VDM bios emulation to function properly.  After this information
    // has been collected, the appropriately sized packed structure can be
    // constructed and linked to the system node.
    //

    ConfigurationDataLength = sizeof(CM_PARTIAL_RESOURCE_LIST) +
                              sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR);

    DiskCount = DiskIdBuffer->DataSize / sizeof(DEVICE_ID);
    DiskInformationLength = DiskCount * sizeof(CM_INT13_DRIVE_PARAMETER);
    ConfigurationDataLength += DiskInformationLength;
    VideoRomBlockPresent = FALSE;
    Status = OslFwGetVideoBiosData(&VideoRomBlock, &VideoRomBlockCount);
    if (NT_SUCCESS(Status)) {

        //
        // Video BIOS data is present on this system.  Roll the size of the
        // video BIOS rom block information into the total size of the system
        // node configuration data element.
        //

        ConfigurationDataLength += sizeof(CM_ROM_BLOCK) * VideoRomBlockCount;
        VideoRomBlockPresent = TRUE;
    }

    //
    // Allocate space for the descriptor and fill it in.
    //

    ResourceList = BlMmAllocateHeap(ConfigurationDataLength);
    if (ResourceList == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BuildSystemNodeEnd;
    }

    Component->ConfigurationDataLength = ConfigurationDataLength;
    ConfigurationRoot->ConfigurationData = ResourceList;
    RtlZeroMemory(ResourceList, ConfigurationDataLength);
    ResourceList->Count = 2;
    Descriptor = &ResourceList->PartialDescriptors[0];
    Descriptor->Type = CmResourceTypeDeviceSpecific;
    Descriptor->u.DeviceSpecificData.DataSize = DiskInformationLength;

    //
    // Copy the disk information, which may be zero bytes in length, into the
    // resource list immediately following the initial descriptor.
    //

    Target = (PVOID)(Descriptor + 1);
    if (DiskInformationLength != 0) {
        OslpCollectInt13DriveParameters(DiskCount,
                                        Target,
                                        DiskIdBuffer->Buffer);
    }

    //
    // If video ROM block information has been retrieved from the BIOS, make
    // sure to copy it in after the second resource descriptor.
    //
    // N.B. The location of the second resource descriptor is variable
    //      depending upon the amount of preceding disk information.  All
  