reak_eq c0.x, c0.y block";
string PS_308 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_309_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_309_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break_lt c0.x, c0.y block";
string PS_309 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_310_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_310_Desc = "ps_2_x : abs is allowed in a rep i0 breakp p0.x and if b0 block";
string PS_310 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_311_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_311_Desc = "ps_2_x : abs is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string PS_311 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_312_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_312_Desc = "ps_2_x : abs is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string PS_312 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_313_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_313_Desc = "ps_2_x : abs is allowed in a rep i0 breakp p0.x and rep i0 block";
string PS_313 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_314_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_314_Desc = "ps_2_x : abs is allowed in a rep i0 breakp p0.x and if_eq c0.x, c0.y block";
string PS_314 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_315_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_315_Desc = "ps_2_x : abs is allowed in a rep i0 breakp p0.x and if_eq c0.x, c0.y nop else block";
string PS_315 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_316_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_316_Desc = "ps_2_x : abs is allowed in a rep i0 breakp p0.x and if p0.x block";
string PS_316 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_317_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_317_Desc = "ps_2_x : abs is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string PS_317 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_318_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_318_Desc = "ps_2_x : abs is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string PS_318 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_319_Criteria = { true, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_319_Desc = "ps_2_x : abs is allowed in a rep i0 breakp p0.x and rep i0 break block";
string PS_319 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_320_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_320_Desc = "ps_2_x : abs is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_320 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_321_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_321_Desc = "ps_2_x : abs is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_321 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_322_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_322_Desc = "ps_2_x : abs is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string PS_322 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_323_Criteria = { true, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_323_Desc = "ps_2_x : abs is allowed in a rep i0 breakp p0.x and rep i0 break_lt c0.x, c0.y block";
string PS_323 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_324_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_324_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if b0 block";
string PS_324 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_325_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_325_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if b0 nop else block";
string PS_325 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_326_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_326_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 block";
string PS_326 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_327_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_327_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 block";
string PS_327 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_328_Criteria = { false, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_328_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if_eq c0.x, c0.y block";
string PS_328 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_329_Criteria = { false, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_329_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if_eq c0.x, c0.y nop else block";
string PS_329 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_330_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_330_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if p0.x block";
string PS_330 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_331_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_331_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if p0.x nop else block";
string PS_331 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_332_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_332_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 break block";
string PS_332 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_333_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_333_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 break block";
string PS_333 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_334_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_334_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_334 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_335_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_335_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 breakp p0.x block";
string PS_335 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_336_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_336_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string PS_336 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_337_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_337_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 break_lt c0.x, c0.y block";
string PS_337 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_338_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_338_Desc = "ps_2_x : abs is allowed in a rep i0 break_lt c0.x, c0.y and if b0 block";
string PS_338 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_339_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_339_Desc = "ps_2_x : abs is allowed in a rep i0 break_lt c0.x, c0.y and if b0 nop else block";
string PS_339 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_340_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_340_Desc = "ps_2_x : abs is NOT allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 block";
string PS_340 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_341_Criteria = { true, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_341_Desc = "ps_2_x : abs is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 block";
string PS_341 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_342_Criteria = { true, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_342_Desc = "ps_2_x : abs is allowed in a rep i0 break_lt c0.x, c0.y and if_eq c0.x, c0.y block";
string PS_342 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_343_Criteria = { true, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_343_Desc = "ps_2_x : abs is allowed in a rep i0 break_lt c0.x, c0.y and if_eq c0.x, c0.y nop else block";
string PS_343 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_344_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_344_Desc = "ps_2_x : abs is allowed in a rep i0 break_lt c0.x, c0.y and if p0.x block";
string PS_344 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_345_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_345_Desc = "ps_2_x : abs is allowed in a rep i0 break_lt c0.x, c0.y and if p0.x nop else block";
string PS_345 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_346_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_346_Desc = "ps_2_x : abs is NOT allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 break block";
string PS_346 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_347_Criteria = { true, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_347_Desc = "ps_2_x : abs is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 break block";
string PS_347 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_348_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_348_Desc = "ps_2_x : abs is NOT allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_348 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_349_Criteria = { true, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_349_Desc = "ps_2_x : abs is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 breakp p0.x block";
string PS_349 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_350_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_350_Desc = "ps_2_x : abs is NOT allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string PS_350 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_351_Criteria = { true, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_351_Desc = "ps_2_x : abs is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 break_lt c0.x, c0.y block";
string PS_351 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";


#include "Filter.h"

#define D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT (D3D10_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT)
#define MAX_DEVICES (32)

static const c_ValidD3DCreateFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_SWITCH_TO_REF | D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;

static
HRESULT
CreateTempDevice(ID3D11Device* pExisting, ID3D11Device** pNewDevice)
{
	HRESULT hr = E_UNEXPECTED;
	*pNewDevice = NULL;
	IDXGIDevice* pDXGI = NULL;
	IDXGIAdapter* pAdapter = NULL;

	hr = pExisting->QueryInterface(__uuidof(IDXGIDevice), (void**)(&pDXGI));
	if (FAILED(hr) || pDXGI == NULL)
		goto func_end;
	hr = pDXGI->GetAdapter(&pAdapter);
	const UINT createFlags = c_ValidD3DCreateFlags & pExisting->GetCreationFlags();
	ID3D11DeviceContext *pDevContext = NULL;
	g_App.CreateDevice(pAdapter, g_App.m_D3DOptions.DriverType, createFlags, g_App.m_D3DOptions.FeatureLevel, pNewDevice, &pDevContext);
	SAFE_RELEASE(pDevContext);

func_end:
	SAFE_RELEASE(pDXGI);
	SAFE_RELEASE(pAdapter);
	return hr;
}

static bool isComparisonFilter(D3D11_FILTER filter)
{
	switch(filter)
	{
	default:
		return false;
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_ANISOTROPIC:
		return true;
	}
}

static D3D11_FILTER NextFilterValue(D3D11_FILTER input, BOOL bIncludeMinMax)
{
	if( !bIncludeMinMax )
	{
		switch (input)
		{
		default:
			assert (!"Unexpected Filter Value");
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case D3D11_FILTER_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		case D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		case D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		case D3D11_FILTER_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_ANISOTROPIC;
		case D3D11_FILTER_ANISOTROPIC:
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_ANISOTROPIC;
		case D3D11_FILTER_COMPARISON_ANISOTROPIC:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		}
	}
	else
	{
		switch (input)
		{
		default:
			assert (!"Unexpected Filter Value");
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case D3D11_FILTER_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		case D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		case D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		case D3D11_FILTER_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_ANISOTROPIC;
		case D3D11_FILTER_ANISOTROPIC:
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_ANISOTROPIC;
		case D3D11_FILTER_COMPARISON_ANISOTROPIC:
			return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
		case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
		case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
		case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
		case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_MINIMUM_ANISOTROPIC;
		case D3D11_FILTER_MINIMUM_ANISOTROPIC:
			return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
		case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
		case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
		case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
		case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_MAXIMUM_ANISOTROPIC;
		case D3D11_FILTER_MAXIMUM_ANISOTROPIC:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		}
	}
}

TEST_RESULT
ID3D11SamplerStateTest::Setup()
{
	m_pDevice = g_App.GetDevice();
	return RESULT_PASS;
}

void
ID3D11SamplerStateTest::Cleanup()
{

}

void 
ID3D11SamplerStateTest::InitTestParameters()
{
	int nPrimitiveW = g_App.Width();
	int nPrimitiveH = g_App.Height();

	g_App.m_RenderTargetOptions.SwapChainDesc.Width = nPrimitiveW*2;
	g_App.m_RenderTargetOptions.SwapChainDesc.Height = nPrimitiveH;
	g_App.m_RenderTargetOptions.DepthTextureDesc.Width = nPrimitiveW*2;
	g_App.m_RenderTargetOptions.DepthTextureDesc.Height = nPrimitiveH;
	g_App.m_RenderTargetOptions.NoDepthStencils = true;

	AddParameter( _T("NumDevices"), &m_numDevices );
	testfactor::RFactor rNumDevices = AddParameterValue( _T("NumDevices"), 16 );

	SetRootTestFactor(rNumDevices);
}

TEST_RESULT
ID3D11SamplerStateTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = E_UNEXPECTED;

	assert(m_numDevices <= MAX_DEVICES);

	ID3D11SamplerState** pStates[MAX_DEVICES] = {NULL};
	ID3D11Device* pDevices[MAX_DEVICES] = {NULL};
	D3D11_SAMPLER_DESC defaultStateDesc, stateDesc;
	ZeroStructMem(defaultStateDesc);
	// defaults first
	defaultStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	defaultStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	defaultStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	defaultStateDesc.BorderColor[0] = 1.0f;
	defaultStateDesc.BorderColor[1] = 1.0f;
	defaultStateDesc.BorderColor[2] = 1.0f;
	defaultStateDesc.BorderColor[3] = 1.0f;
	defaultStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	defaultStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	defaultStateDesc.MaxAnisotropy = 1;
	defaultStateDesc.MaxLOD = FLT_MAX;
	defaultStateDesc.MinLOD = -FLT_MAX;
	defaultStateDesc.MipLODBias = 0;
	// then incremental values
	stateDesc = defaultStateDesc;

	for (int nDevice = 0; nDevice < m_numDevices; ++nDevice)
	{
		hr = CreateTempDevice(m_pDevice, &pDevices[nDevice]);
		if (FAILED(hr))
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Failed to create another device. device#=%d, hr=%s\n"), nDevice+1, D3DHResultToString(hr).c_str());
			goto testDone;
		}
		D3D11_FEATURE_DATA_D3D11_OPTIONS1 d3d11Options1;
		BOOL bSupportsMinMaxFiltering = FALSE;

		if (g_App.GetDriverVersion() >= KMT_DRIVERVERSION_WDDM_1_3)
		{
			if( FAILED( hr = m_pDevice->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS1, &d3d11Options1, sizeof(d3d11Options1) ) ) )
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T( "CheckFeatureSupport() failed. HR = %s." ), D3DHResultToString(hr).c_str());
				goto testDone;
			}
			bSupportsMinMaxFiltering = d3d11Options1.MinMaxFiltering;
		}

		pStates[nDevice] = new ID3D11SamplerState*[D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT+1];
		ZeroMemory(pStates[nDevice], sizeof(ID3D11SamplerState*)*(D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT+1));
		hr = pDevices[nDevice]->CreateSamplerState(&defaultStateDesc, &pStates[nDevice][0]);
		//for (UINT n = 1; n <= D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT; ++n)
		for (UINT n = 1; n < D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT; ++n)
		{
			(UINT32&)(stateDesc.AddressU) += 1;
			if (stateDesc.AddressU > D3D11_TEXTURE_ADDRESS_MIRROR_ONCE)
			{
				stateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				(UINT32&)(stateDesc.AddressV) += 1;
				if (stateDesc.AddressV > D3D11_TEXTURE_ADDRESS_MIRROR_ONCE)
				{
					stateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
					(UINT32&)(stateDesc.AddressW) += 1;
					if (stateDesc.AddressW > D3D11_TEXTURE_ADDRESS_MIRROR_ONCE)
					{
						stateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
						stateDesc.Filter = NextFilterValue(stateDesc.Filter,bSupportsMinMaxFiltering);
						if (stateDesc.Filter == D3D11_FILTER_MIN_MAG_MIP_POINT)
						{
							(UINT32&)(stateDesc.MaxAnisotropy) += 1;
							if (stateDesc.MaxAnisotropy > 16)
							{
								stateDesc.MaxAnisotropy = 1;
								if (isComparisonFilter(stateDesc.Filter))
								{
									stateDesc.MaxLOD = FLT_MAX;
									(UINT32&)(stateDesc.ComparisonFunc) += 1;
									if (stateDesc.ComparisonFunc > D3D11_COMPARISON_ALWAYS)
									{
										stateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
									}
								}
								else
								{
									stateDesc.MaxLOD /= 2;
									if (stateDesc.MaxLOD < 0.5)
									{
										stateDesc.MaxLOD = FLT_MAX;
									}
								}
							}
						}
					}
				}
			}
			hr = pDevices[nDevice]->CreateSamplerState(&stateDesc, &pStates[nDevice][n]);
			if (FAILED(hr))
			{
				if (n < D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT 
					|| hr != D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS)
				{
					tRes = RESULT_FAIL;
					WriteToLog( _T("Failed on call to CreateSamplerState.\n"));
					WriteToLog( _T("Device#=%d, StateNum=%d, TotalStates=%d, hr=%s\n"), nDevice+1, n, nDevice*4096+n, D3DHResultToString(hr).c_str() );
					goto testDone;
				}
			}
			else if (n >= D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT)
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T("Too many unique objects were created without failure.\n"));
				WriteToLog( _T("Device#=%d, StateNum=%d, TotalStates=%d, hr=%s\n"), nDevice+1, n, nDevice*4096+n, D3DHResultToString(hr).c_str() );
				goto testDone;
			}
		}
	}

testDone:
	for (int nDevice = 0; nDevice < m_numDevices; ++nDevice)
	{
		if (pStates[nDevice] != NULL)
		{
			for (UINT n = 0; n <= D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT; ++n)
			{
				SAFE_RELEASE(pStates[nDevice][n]);
			}
			delete[] pStates[nDevice];
		}
		SAFE_RELEASE(pDevices[nDevice]);
	}

	return tRes;
}
//
//  VBSwap.h
//

#ifndef __VBSwap_H__
#define __VBSwap_H__

#define TESTDXVER 0x0800

#include <d3d8.h>
#include <CShapes.h>
// temporary
//class CMesh;

typedef struct tagFVFVERTEX
{
	float x, y, z;
	float nx, ny, nz;
	float tu, tv;
}FVFVERTEX, *PFVFVERTEX;

#define FVFVERTEXHANDLE	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

//-----------------------------------------------------------------------------

typedef struct _DEV_CONTEXT
{
	CDevice* pDevice;
	CTexture* pTexture;
	CTexture* pDynamicTexture;
	CVertexBuffer* pVB;
	CIndexBuffer* pIB;
}DEV_CONTEXT;

//-----------------------------------------------------------------------------
//
//  The VBSwap specification
//
//-----------------------------------------------------------------------------

class CVBSwapTest : public CD3DTest
{
public:
	CVBSwapTest();
    bool SortVertexBuffer(PVOID pVBDataDest,  PVOID pVBDataSrc, WORD *pwIBDataSrc, UINT uIndexCount, UINT uVertexSize);
	bool SetDefaultRenderStates();
	bool SetDefaultMaterials();
	bool SetDefaultMatrices();
	bool SetDefaultLights();
	UINT TestInitialize();
	bool ExecuteTest(UINT nTest);
	bool TestTerminate();
private:
//	CMD2Mesh*       m_pMesh;
//	CMesh*          m_pMesh;
    CShapes         m_Shape[10];
    PFVFVERTEX      m_pfvfTriSorted[10];

    
	bool            m_bDynamicTextures;
	bool			m_bHandleExceptions;
	DEV_CONTEXT		m_SrcContext;
	bool			Draw(UINT n, DEV_CONTEXT* pContext);
	UINT			InitResources(DEV_CONTEXT* pContext);
	bool            CopyTexture( CTexture* pDst, CTexture* pSrc );

};

#if 0
//-----------------------------------------------------------------------------
// Some mesh loading stuff...
//-----------------------------------------------------------------------------


#define MESH_KEYFRAMES 32

#define MESH_FVF D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0)

typedef struct _MESH_VERTEX
{
	float x,y,z;
	float nx,ny,nz;
	float u,v;
}MESH_VERTEX;


class CMesh
{
protected:
	int m_iNumKeyFrames;
	int m_iNumIndexedVertices;
	int m_iNumVertices;
	int m_iNumIndices;
	int m_iNumPrims;
	int m_iVertexStride;
	MESH_VERTEX** m_pIndexedVertices;
	MESH_VERTEX** m_pVertices;
	WORD* m_pIndices;
	bool LoadFromXFile( CDevice* pDevice, LPSTR szFileName );
	void  ReleaseTest();
public:
	CMesh();
	static CMesh* CreateMeshFromXFile(CDevice* pDevice, LPSTR szFileName);
	ULONG Release();

	inline int GetNumFrames() { return m_iNumKeyFrames; }
	inline int GetNumIndexedVertices() { return m_iNumIndexedVertices; }
	inline int GetNumVertices() { return m_iNumVertices; }
	inline int GetNumIndices() { return m_iNumIndices; }
	inline int GetNumTriangles() { return m_iNumPrims; }
	inline DWORD GetFVF() { return (DWORD)(MESH_FVF); }
	inline int GetVertexStride() { return m_iVertexStride; }
	inline D3DPRIMITIVETYPE GetPrimitiveType() { return D3DPT_TRIANGLELIST; }
	inline WORD* GetIndices() { return m_pIndices; }
	inline MESH_VERTEX* GetIndexedVertexData(int iKeyFrame) { return m_pIndexedVertices[iKeyFrame]; }
	inline MESH_VERTEX* GetVertexData(int iKeyFrame) { return m_pVertices[iKeyFrame]; }
};
#endif

#endif

/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    file.c

Abstract:

    Implementation of boot library file i/o api.

    When file's are opened, an entry is created in a file table.  On each file,
    a mount file system routine is called.  This allows for a file system to be
    mounted on a file.  To reduce the overhead of a mount call on each file,
    the file table will not purge file entries when their respective files are
    closed.

Environment:

    boot

--*/

// ------------------------------------------------------------------- Includes

#include "file.h"

// -------------------------------------------------------------------- Globals

//
// There is a global pointer to the file table and the number of entries in the
// table.  The file table makes use of the boot environment library's generic
// table api.
//

PFILE_ENTRY *FileTable;
ULONG FileEntries;

#define INITIAL_FILE_ENTRIES (16)
#define CAPPED_FILE_ENTRIES  (512)

//
// Global list of registered file systems.  Each file system must register a
// mount routine.
//

LIST_ENTRY RegisteredFileSystems;

// ----------------------------------------------- Internal Function Prototypes

NTSTATUS
BlpFileOpen (
    __in DEVICE_ID DeviceId,
    __in PWSTR OpenPath,
    __in OPEN_FLAGS OpenFlags,
    __in ULONG FilterLevel,
    __out PFILE_ID FileId,
    __out_opt PWIM_PROVIDER_EXTERNAL_INFO ReparseData
    );

NTSTATUS
FileInformationCheck (
    __in PFILE_INFORMATION FileInformation,
    __in BOOLEAN IsWriteOperation,
    __in_opt PULONG InputSize,
    __out_opt PULONG ByteCountRequested,
    __out_opt PULONG OutputAdjustedSize
    );

NTSTATUS
FileTableDestroyEntry (
    __in PFILE_ENTRY FileEntry,
    __in ULONG Index
    );

BOOLEAN
FileTableCompareWithSameAttributes (
    __in PFILE_ENTRY FileEntry,
    __in PDEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __in POPEN_FLAGS OpenFlags,
    __in PULONG FilterLevel
    );

BOOLEAN
FileTableCompareWithSubsetAttributes (
    __in PFILE_ENTRY FileEntry,
    __in PDEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __in POPEN_FLAGS OpenFlags,
    __in PULONG FilterLevel
    );

NTSTATUS
FileTablePurgeEntry (
    __in PFILE_ENTRY FileEntry
    );

NTSTATUS
FileIoOpen (
    __in DEVICE_ID DeviceId,
    __in PWSTR OpenPath,
    __in OPEN_FLAGS OpenFlags,
    __in ULONG FilterLevel,
    __in PTABLE_COMPARE_ENTRY CompareRoutine,
    __deref_opt_out PFILE_ENTRY *ReturnFileEntry
    );

NTSTATUS
FileIoMountFileSystem (
    __in DEVICE_ID DeviceId,
    __in ULONG FilterLevel,
    __deref_out PFILE_ENTRY *FileEntry
    );

NTSTATUS
FileIoDestroyRegisteredFileSystems (
    VOID
    );

VOID
FileIoSetAttributes (
    __in PFILE_ENTRY FileEntry,
    __in OPEN_FLAGS OpenFlags
    );

PWCHAR
FileIoCopyParentDirectoryPath (
    __in PWSTR FilePath
    );

PWCHAR
FileIoCopyFileName (
    __in PWSTR FilePath
    );

NTSTATUS
FileIoPrint (
    __in PFILE_ENTRY FileEntry,
    __in FILE_ID FileId
    );

// ------------------------------------------------------------------ Functions

//
// Implementation of Boot Environment File I/O API.
//

NTSTATUS
BlFileOpen (
    __in DEVICE_ID DeviceId,
    __in PWSTR OpenPath,
    __in OPEN_FLAGS OpenFlags,
    __out PFILE_ID FileId
    )

/*++

Routine Description:

    This function opens a file.  In order to open the file, a filesystem must
    be mounted on the file's block i/o device and the file system's function
    table must be associated with the newly opened file.

    In order to 'open' a file, the parent directory must be 'opened' in order
    for a file system to determine the existence of a file and location on the
    block i/o disk of the file.

    If the file is already open and the OpenFlags flags do not conflict, the
    same file descriptor id may be returned and the reference count to the
    file increased.

Arguments:

    DeviceId - Supplies the handle for the containing the file to open.

    OpenPath - Supplies the path to the file to open.

    OpenFlags - Supplies a bitmask containing flags to use when opening the
        file.

    FileId - Supplies a pointer to a variable that receives a handle to file.

Return Value:

    STATUS_SUCCESS when successful.  (FileId will also contain the file's
        handle).

    STATUS_INVALID_PARAMETER if the specified device is invalid (Invalid handle
        or a handle to a non-block i/o device).

    STATUS_INVALID_PARAMETER if the specified file path is invalid (NULL or
        does not start with a '\').

    STATUS_NO_MEMORY if an allocation fails for an internal data structure.

    STATUS_UNSUCCESSFUL if a filesystem could not be mounted on the underlying
        device.

    STATUS_NO_SUCH_FILE if the requested file does not exist.

--*/

{

    DEVICE_ID OverlayDeviceId;
    WIM_PROVIDER_EXTERNAL_INFO ReparseData;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);
    Status = BlpFileOpen(DeviceId, OpenPath, OpenFlags, 0, FileId, &ReparseData);
    if(Status == STATUS_REPARSE) {
        BlFileClose(*FileId);
        Status = STATUS_NOT_FOUND;

        //
        // If the file is a reparse point, try to open the actual file, unless
        // the caller explicity requested to not follow reparse points.
        //

        if (!IS_OPEN_NO_REPARSE_SET(OpenFlags)) {
            OverlayDeviceId = BlDeviceGetOverlay(DeviceId, ReparseData.DataSourceId);
            if (OverlayDeviceId != INVALID_DEVICE_ID) {
                Status = BlpFileOpen(OverlayDeviceId,
                                     (PWSTR)ReparseData.ResourceHash,
                                     OpenFlags | OPEN_BY_HASH,
                                     0,
                                     FileId,
                                     NULL);
            }
        }
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileClose (
    __in FILE_ID FileId
    )

/*++

Routine Description:

    Closes a device by decrementing the reference count on the file handle.

    N.B. This routine does not purge the file's entry in the global file table.
         Entries remain resident and are only purged when required.  This allows
         a the file entry to be reused if the file is ever reopened.

Arguments:

    FileId - Supplies the handle to the file to close.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the file handle is invalid or if the file is
        already closed.

--*/

{

    EVENT_DATA_DESCRIPTOR EventDataDescriptor[4];
    BOOLEAN IsCatalog;
    BOOLEAN IsDirectory;
    ULONG PathSize;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Make sure this is a valid file id.
    //

    if ((FileEntries <= FileId) ||
        (FileTable[FileId] == NULL) ||
        (!IS_FILE_OPENED(FileTable[FileId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        //
        // Close an instance of the file.
        //

        FileTable[FileId]->ReferenceCount -= 1;

        //
        // If there are no more instances of the file open, clear the open flag
        // and log the ETW event.
        //

        if (FileTable[FileId]->ReferenceCount == 0) {
            CLEAR_FILE_OPEN(FileTable[FileId]->Attributes);
            IsDirectory =
                IS_FILE_DIRECTORY_SET(FileTable[FileId]->Attributes) != FALSE;

            IsCatalog =
                IS_FILE_CATALOG_SET(FileTable[FileId]->Attributes) != FALSE;

            if ((!IsDirectory) && (!IsCatalog)) {

                EventDataDescCreate(&EventDataDescriptor[0],
                                    (PVOID)&(FileTable[FileId]->DeviceId),
                                    sizeof(DEVICE_ID));

                PathSize = (ULONG)((wcslen(FileTable[FileId]->FilePath) + 1) *
                                   sizeof(WCHAR));

                EventDataDescCreate(&EventDataDescriptor[1],
                                    (PVOID)FileTable[FileId]->FilePath,
                                    PathSize);

                EventDataDescCreate(&EventDataDescriptor[2],
                                    (PVOID)&(FileTable[FileId]->BytesRead),
                                    sizeof(ULONGLONG));

                EventDataDescCreate(&EventDataDescriptor[3],
                                    (PVOID)&(FileTable[FileId]->BytesWritten),
                                    sizeof(ULONGLONG));

                BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                              &BOOT_FILE_ACCESS,
                              NULL,
                              4,
                              EventDataDescriptor);

                FileTable[FileId]->BytesRead = 0;
                FileTable[FileId]->BytesWritten = 0;
            }
        }

        Status = STATUS_SUCCESS;
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileReadEx (
    __in FILE_ID FileId,
    __out_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine reads from a file at the offset specified by the internal
    file pointer. In the case where the requested number of bytes exceeds the
    remaining bytes in the file (from the file pointer to the end), behavior
    depends upon the presence of the Count output parameter. If present, the
    actual remaining bytes are returned and the call completes successfully.
    If not present, an error is returned. In all cases, a successful read
    operation will advance the file pointer by the number of bytes read.

    N.B. File contents may be cached and a file read request does not
         necessarily result in a device read.

Arguments:

    FileId - Supplies a handle to a file.

    Buffer - Supplies a buffer to receive the contents of the read operation.

    Size - Supplies the number of bytes to read.

    Count - Supplies an optional pointer to a variable that receives the number
        of bytes read. The presence of this argument may affect the behavior
        of this routine.

    Flags - Supplies the flags indicating how the file should be read.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the file information reveals that the
        current file offset exceeds the file's size.

    STATUS_INVALID_PARAMETER if the transfer buffer is NULL.

    STATUS_INVALID_PARAMETER if the file ID is invalid.

    STATUS_INVALID_PARAMETER if the file is not opened with read access.

    STATUS_INVALID_PARAMETER if the size requested would cause an overflow
        when added to the current file offset.

    STATUS_INVALID_PARAMETER if the number of bytes to be read crosses
        the end of the file and the "Count" parameter is not specified.

    Other failing error status codes are possible based on the underlying
        file system's implementation.

--*/

{

    ULONG AdjustedSize;
    DEVICE_INFORMATION DeviceInformation = {0};
    NTSTATUS DeviceStatus;
    FILE_INFORMATION FileInformation;
    ULONG OriginalIoAccessFlags;
    PFILESYSTEM_READ_ROUTINE ReadRoutine;
    BOOLEAN RestoreIoAccessFlags;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    RestoreIoAccessFlags = FALSE;

    //
    // Initialize OriginalIoAccessFlags to satisfy the compiler.
    //

    OriginalIoAccessFlags = 0;

    //
    // Ensure that the file handle is valid and the file was opened for read.
    //

    if ((Buffer == NULL) ||
        (FileEntries <= FileId) ||
        (FileTable[FileId] == NULL) ||
        (!IS_FILE_OPENED_FOR_READ(FileTable[FileId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlFileReadExEnd;
    }

    Status = BlFileGetInformation(FileId, &FileInformation);
    if (!NT_SUCCESS(Status)) {
        goto BlFileReadExEnd;
    }

    //
    // Perform common checks and actual count determination.
    //

    AdjustedSize = Size;
    Status = FileInformationCheck(&FileInformation,
                                  FALSE,
                                  &AdjustedSize,
                                  Count,
                                  &AdjustedSize);

    if (!NT_SUCCESS(Status)) {
        goto BlFileReadExEnd;
    }

    //
    // Specifically handle a request of zero bytes, avoiding passing such a
    // request to the filesystem stub.  Forward all other requests to the
    // stub.
    //

    if (AdjustedSize == 0) {
        Status = STATUS_SUCCESS;
        if (ARGUMENT_PRESENT(Count) != FALSE) {
            *Count = 0;
        }

    } else {

        //
        // If this is a block IO or partition device, and the read request
        // is for cache-enabled or prefetch-enabled, then modify the device IO
        // access flags for this read when necessary, however, only assert if
        // this task fails as the read can still succeed.
        //

        if (CHECK_FLAG(Flags, FILE_FLAGS_READ_CACHE) ||
            CHECK_FLAG(Flags, FILE_FLAGS_READ_PREFETCH)) {

            DeviceStatus = BlDeviceGetInformation(FileTable[FileId]->DeviceId,
                                                  &DeviceInformation);

            ASSERT(DeviceStatus == STATUS_SUCCESS);

            if (((DeviceInformation.DeviceType == DEVICE_BLOCK_IO) ||
                 (DeviceInformation.DeviceType == DEVICE_PARTITION)) &&
                ((!CHECK_FLAG(DeviceInformation.u.BlockIo.IoAccessFlags,
                              BLOCK_IO_ACCESS_FLAGS_CACHE_READ) &&
                  (CHECK_FLAG(Flags, FILE_FLAGS_READ_CACHE))) ||
                 (!CHECK_FLAG(DeviceInformation.u.BlockIo.IoAccessFlags,
                              BLOCK_IO_ACCESS_FLAGS_DEVICE_PREFETCH) &&
                  (CHECK_FLAG(Flags, FILE_FLAGS_READ_PREFETCH))))) {

                RestoreIoAccessFlags = TRUE;
                OriginalIoAccessFlags =
                    DeviceInformation.u.BlockIo.IoAccessFlags;

                if (CHECK_FLAG(Flags, FILE_FLAGS_READ_CACHE)) {
                    SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
                              BLOCK_IO_ACCESS_FLAGS_CACHE_READ);
                }

                if (CHECK_FLAG(Flags, FILE_FLAGS_READ_PREFETCH)) {
                    SET_FLAGS(DeviceInformation.u.BlockIo.IoAccessFlags,
                              BLOCK_IO_ACCESS_FLAGS_DEVICE_PREFETCH);
                }

                DeviceStatus =
                    BlDeviceSetInformation(FileTable[FileId]->DeviceId,
                                           &DeviceInformation);

                ASSERT(DeviceStatus == STATUS_SUCCESS);
            }
        }

        ReadRoutine = FileTable[FileId]->FunctionTable.Read;
        Status = ReadRoutine(FileTable[FileId], Buffer, AdjustedSize, Count);

        //
        //  N.B. FileInformation has been updated by a successful read, so the
        //       local copy will be out of synch.
        //

        if (!NT_SUCCESS(Status)) {
            goto BlFileReadExLogRead;
        }
    }

    FileTable[FileId]->BytesRead += AdjustedSize;

BlFileReadExLogRead:

    //
    // Log this read operation.
    //

    BlpFileLogAccess(FileTable[FileId]->DeviceId,
                     FileTable[FileId]->FilePath,
                     FileInformation.FileOffset,
                     AdjustedSize,
                     (NT_SUCCESS(Status) ? BlLogReadAccess :
                                           BlLogReadAccessFailure));

    //
    // Restore the device IO access attributes if necessary.
    //

BlFileReadExEnd:
    if (RestoreIoAccessFlags != FALSE) {
        DeviceInformation.u.BlockIo.IoAccessFlags = OriginalIoAccessFlags;
        DeviceStatus = BlDeviceSetInformation(FileTable[FileId]->DeviceId,
                                              &DeviceInformation);

        ASSERT(DeviceStatus == STATUS_SUCCESS);
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileWrite (
    __in FILE_ID FileId,
    __in_bcount(Size) PVOID Buffer,
    __in ULONG Size,
    __out_opt PULONG Count
    )

/*++

Routine Description:

    This routine writes to a file at the offset specified by the internal
    file pointer.  A write operation will advance the file pointer by the
    number of bytes written.

    N.B. A boot environment filesystem implementation can only support write
         back.  File growth would require interaction with the Operating
         System driver to allow for sector allocation.

    N.B. Not all filesystems will support this routine and may return
         STATUS_NOT_IMPLEMENTED.

Arguments:

    FileId - Supplies a handle to a file.

    Buffer - Supplies a buffer to write to a file.

    Size - Supplies the number of bytes to read to a file.

    Count - Supplies an optional pointer to a variable that receives the number
        of bytes written.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the transfer buffer is NULL.

    STATUS_INVALID_PARAMETER if the file ID is invalid.

    STATUS_INVALID_PARAMETER if the file is not opened with write access.

    Other failing error status codes are possible based on the underlying
        file system's implementation.

--*/

{

    ULONG AdjustedSize;
    FILE_INFORMATION FileInformation;
    NTSTATUS Status;
    PFILESYSTEM_WRITE_ROUTINE WriteRoutine;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Ensure that the file handle is valid and the file was opened for write.
    //

    if ((Buffer == NULL) ||
        (FileEntries <= FileId) ||
        (FileTable[FileId] == NULL) ||
        (!IS_FILE_OPENED_FOR_WRITE(FileTable[FileId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlFileWriteEnd;
    }

    //
    // Ensure the write operation will not go off the end of the file.
    //

    Status = BlFileGetInformation(FileId, &FileInformation);
    if (!NT_SUCCESS(Status)) {
        goto BlFileWriteEnd;
    }

    //
    // Perform common checks and actual count determination.
    //

    AdjustedSize = Size;
    Status = FileInformationCheck(&FileInformation,
                                  TRUE,
                                  &AdjustedSize,
                                  Count,
                                  &AdjustedSize);

    if (!NT_SUCCESS(Status)) {
        goto BlFileWriteEnd;
    }

    //
    // Specifically handle a request of zero bytes, avoiding passing such a
    // request to the filesystem stub.  Forward all other requests to the
    // stub.
    //

    if (AdjustedSize == 0) {
        Status = STATUS_SUCCESS;

    } else {
        WriteRoutine = FileTable[FileId]->FunctionTable.Write;

        //
        //  N.B. The file information has been updated by a successful write, so the
        //       local copy may be out of synch.
        //

        Status =  WriteRoutine(FileTable[FileId], Buffer, AdjustedSize, Count);
        if (!NT_SUCCESS(Status)) {
            goto BlFileWriteLogWrite;
        }
    }

    FileTable[FileId]->BytesWritten += AdjustedSize;

BlFileWriteLogWrite:

    //
    // Log the write operation.
    //

    BlpFileLogAccess(FileTable[FileId]->DeviceId,
                     FileTable[FileId]->FilePath,
                     FileInformation.FileOffset,
                     AdjustedSize,
                     (NT_SUCCESS(Status) ? BlLogWriteAccess :
                                           BlLogWriteAccessFailure));

BlFileWriteEnd:
    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileGetNextFile (
    __in FILE_ID DirectoryId,
    __deref_inout_ecount(*BufferSize) PWCHAR *FileName,
    __inout PULONG BufferSize,
    __in BOOLEAN AllocateBuffer
    )

/*++

Routine Description:

    This routine enumerates the file entries in a directory.

    This routine will return the next file entry in a directory.  Each call
    requires the file name returned by the previous call and the output is the
    next file name.

    To initiate the search, the input file name pointer must point to NULL or
    to a unicode string whose first character is the NULL character.
    (ie. *FileName == NULL || *FileName[0] == L'\0').

    A search is ended with a return value of STATUS_NOT_FOUND, indicating the
    next filename was not found.

Arguments:

    DirectoryId - Supplies a handle to a directory.

    FileName - Supplies a pointer to an unicode string containing the last
        file name returned by this API.  On output, the string contains the
        next file in the directory.

    BufferSize - Supplies a pointer to a variable that contains the size, in
        WCHARs, of the buffer pointed to by FileName.  If
        STATUS_BUFFER_TOO_SMALL is returned by the API, on output, this
        variable contains the required buffer size for the filename.

    AllocateBuffer - Supplies a boolean indicating whether a new buffer should
        be allocated if the supplied file name buffer is too small.

        N.B. If this boolean is set to true and the API allocates a new buffer,
             the current buffer is freed.  This value should not be set to true
             if the existing buffer is a stack buffer or an invalid pointer.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_FOUND to terminate the enumeration.

    STATUS_BUFFER_TOO_SMALL if the file name buffer is too small.

    STATUS_NOT_A_DIRECTORY if the specified file handle is not a directory.

    STATUS_INVALID_PARAMETER if the specified file handle is invalid or if
        BufferSize is NULL, FileName is NULL.

--*/

{

    PFILESYSTEM_GET_NEXT_FILE_ROUTINE GetNextFile;
    PWCHAR LocalFileName;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Fail if the specified handle is invalid, if the specified handle does
    // not refer to a directory or if BufferSize or FileName are NULL.
    //

    if ((FileEntries <= DirectoryId) ||
        (FileTable[DirectoryId] == NULL) ||
        (BufferSize == NULL) ||
        (FileName == NULL) ||
        (!IS_FILE_OPENED(FileTable[DirectoryId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlFileGetNextFileEnd;
    }

    if (!IS_FILE_DIRECTORY_SET(FileTable[DirectoryId]->Attributes)) {
        Status = STATUS_NOT_A_DIRECTORY;
        goto BlFileGetNextFileEnd;
    }

    //
    // Use the filesystem stub to determine the next filename.
    //

    GetNextFile = FileTable[DirectoryId]->FunctionTable.GetNextFile;
    Status = GetNextFile(FileTable[DirectoryId], *FileName, BufferSize);

    //
    // If the user's buffer was too small and the caller requested a new buffer,
    // allocate a new one and try the filesystem GetNextFile stub again.
    //

    if ((Status == STATUS_BUFFER_TOO_SMALL) && (AllocateBuffer != FALSE)) {
        LocalFileName = BlMmAllocateHeap(*BufferSize * sizeof(WCHAR));
        if (LocalFileName == NULL) {
            Status = STATUS_NO_MEMORY;
            goto BlFileGetNextFileEnd;
        }

        //
        // Copy the current file name into the newly allocated buffer and call
        // into the filesystem stub again.
        //

        if (*FileName != NULL) {
            wcscpy_s(LocalFileName, *BufferSize, *FileName);
            BlMmFreeHeap(*FileName);

        } else {
            LocalFileName[0] = L'\0';
        }

        *FileName = LocalFileName;
        Status = GetNextFile(FileTable[DirectoryId], *FileName, BufferSize);
    }

BlFileGetNextFileEnd:
    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileGetInformation (
    __in FILE_ID FileId,
    __out PFILE_INFORMATION FileInformation
    )

/*++

Routine Description:

    This routine queries for file information.

Arguments:

    FileId - Supplies a handle to a file.

    FileInformation - Supplies a pointer to a buffer that is populated with
        file information.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the file handle is invalid.

    STATUS_INVALID_PARAMETER if the file information pointer is NULL.

    Other status codes are possible based on the implementation of the
        underlying filesystem.

--*/

{

    PFILESYSTEM_GET_INFORMATION_ROUTINE GetInformation;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Parameter checking.  Ensure FileId is valid and that the file is
    // currently opened.
    //

    if ((FileInformation == NULL) ||
        (FileEntries <= FileId) ||
        (FileTable[FileId] == NULL) ||
        (!IS_FILE_OPENED(FileTable[FileId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        //
        // Perform the GetInformation operation by calling the filesystem stub.
        //

        GetInformation = FileTable[FileId]->FunctionTable.GetInformation;
        Status = GetInformation(FileTable[FileId], FileInformation);
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileSetInformation (
    __in FILE_ID FileId,
    __in PFILE_INFORMATION FileInformation
    )

/*++

Routine Description:

    Sets file information.

    N.B. Not all file information is configurable (some read-only) and there is
         no specification to the mannar in which the filesystem must handle
         configurable data being modified.  It is recommended to always call
         BlFileGetInformation prior to calling BlFileSetInformation to ensure
         non-configurable fields are properly set.

Arguments:

    FileId - Supplies a handle to a file.

    FileInformation - Supplies the file information to set.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the file handle is invalid.

    STATUS_INVALID_PARAMETER if the file information pointer is NULL.

    Other status codes are possible based on the implementation of the
        underlying filesystem.

--*/

{

    PFILESYSTEM_SET_INFORMATION_ROUTINE SetInformation;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Parameter checking.  Ensure that FileId is valid and the file is
    // currently opened.
    //

    if ((FileInformation == NULL) ||
        (FileEntries <= FileId) ||
        (FileTable[FileId] == NULL) ||
        (!IS_FILE_OPENED(FileTable[FileId]->Attributes))) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        //
        // Perform SetInformation operation by calling the file system stub.
        //

        SetInformation = FileTable[FileId]->FunctionTable.SetInformation;
        Status = SetInformation(FileTable[FileId], FileInformation);
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileMount (
    __in DEVICE_ID ParentDeviceId,
    __in PWSTR FileName,
    __out PDEVICE_ID DeviceId
    )

/*++

Routine Description:

    Attempts to mount a file system on a file treated as a block i/o device.

Arguments:

    ParentDeviceId - Supplies the handle to the device containing the file
        to mount.

    FileName - Supplies the path to the file to mount.

    DeviceId - Supplies a pointer to a variable that receives the handle to
        the file as a block i/o device.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the specified file name is NULL.

    STATUS_INVALID_PARAMETER if the specified device handle is invalid.

    STATUS_NO_SUCH_FILE if the specified file does not exist.

    STATUS_UNSUCCESSFUL if a file system could not be mounted on the specified
        file.

--*/

{

    PVOID Buffer;
    PBOOT_ENVIRONMENT_DEVICE Device;
    FILE_ID FileId;
    SIZE_T FileNameSize;
    PUNALIGNED_FILE_PATH FilePath;
    SIZE_T FilePathLength;
    UINTN FilePathOffset;
    FILE_ID RootDirectoryId;
    PBOOT_ENVIRONMENT_DEVICE ParentDevice;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    FileId = INVALID_FILE_ID;

    //
    // Simple parameter checking.
    //

    if (DeviceId == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlFileMountEnd;
    }

    *DeviceId = INVALID_DEVICE_ID;
    if (FileName == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlFileMountEnd;
    }

    //
    // Attempt to open the specified file on the parent device.
    //

    Status = BlFileOpen(ParentDeviceId, FileName, OPEN_READ_WRITE, &FileId);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    //
    // Get the device identifier for the parent device.  This is necessary
    // to construct the source path for the file for opening it as a device.
    //

    Status = BlGetDeviceIdentifier(ParentDeviceId, &ParentDevice);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    //
    // We were successful in opening the file.  Construct the device identifer
    // for a file as a block I/O device.  The identifier contains a full source
    // path to the file to open it as a device.  This includes the source
    // device identifier and the file name.
    //

    FilePathOffset = FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u) +
                     FIELD_OFFSET(BLOCK_IO_IDENTIFIER, u);

    FilePathLength = FIELD_OFFSET(UNALIGNED_FILE_PATH, FilePath);
    Status = RtlSIZETAdd(FilePathLength, ParentDevice->Size, &FilePathLength);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    Status = RtlSIZETMult(wcslen(FileName), sizeof(WCHAR), &FileNameSize);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    Status = RtlSIZETAdd(FileNameSize, sizeof(UNICODE_NULL), &FileNameSize);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    Status = RtlSIZETAdd(FilePathLength, FileNameSize, &FilePathLength);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    MARK_SAFE_ADDITION(FilePathOffset, FilePathLength);
    Device = BlMmAllocateHeap(FilePathOffset + FilePathLength);
    if (Device == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlFileMountEnd;
    }

    Device->DeviceType = DEVICE_BLOCK_IO;
    Device->Size = (ULONG)(FilePathOffset + FilePathLength);
    Device->u.BlockIo.BlockIoType = BLOCK_IO_FILE;

    //
    // Construct the file path identifier for the file to be mounted.
    //

    FilePath = &Device->u.BlockIo.u.File.SourcePath;

    ASSERT(FilePathOffset == PtrOffset(Device, FilePath));

    FilePath->Version = FILE_PATH_VERSION;
    FilePath->Type = FILE_PATH_TYPE_BOOT_ENV;
    FilePath->Length = (ULONG)FilePathLength;
    Buffer = &FilePath->FilePath[0];
    RtlCopyMemory(Buffer, ParentDevice, ParentDevice->Size);
    Buffer = Add2Ptr(Buffer, ParentDevice->Size);
    RtlCopyMemory(Buffer, FileName, FileNameSize);

    //
    // Open the file as a block I/O device.
    //

    Status = BlDeviceOpen(Device, OPEN_READ_WRITE, DeviceId);
    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    //
    // Attempt to see if a file system can be detected for the block i/o file
    // by opening the root directory.
    //

    Status = BlFileOpen(*DeviceId,
                        L"\\",
                        (OPEN_READ | OPEN_DIRECTORY),
                        &RootDirectoryId);

    if (!NT_SUCCESS(Status)) {
        goto BlFileMountEnd;
    }

    //
    // A file system was detected on the block i/o file device.  Close the
    // root directory entry and successfully return to the caller.
    //

    BlFileClose(RootDirectoryId);

BlFileMountEnd:
    if (!NT_SUCCESS(Status)) {
        if (FileId != INVALID_FILE_ID) {
            BlFileClose(FileId);
        }

        if ((DeviceId != NULL) && (*DeviceId != INVALID_DEVICE_ID)) {
            BlDeviceClose(*DeviceId);
        }
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileReadAtOffsetEx (
    __in FILE_ID FileId,
    __in ULONG Size,
    __in ULONGLONG ByteOffset,
    __out_bcount(Size) PVOID Buffer,
    __out_opt PULONG BytesRead,
    __in ULONG Flags
    )

/*++

Routine Description:

    Reads from a file at the specified offset.

Arguments:

    FileId - Supplies a handle to a file.

    Size - Supplies the number of bytes to read.

    ByteOffset - Supplies the file offset to start the read operation.

    Buffer - Supplies a buffer to receive the contents of the read operation.

    BytesRead - Supplies an optional pointer to a variable that receives the
        number of bytes read.

    Flags - Supplies the flags indicating how the file should be read.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the transfer buffer is NULL.

    STATUS_INVALID_PARAMETER if the file ID is invalid.

    STATUS_INVALID_PARAMETER if the file is not opened with read access.

    STATUS_INVALID_PARAMETER if the requested offset is invalid.

    Other failing error status codes are possible based on the underlying
        file system's implementation.

--*/

{

    ULONG AdjustedSize;
    FILE_INFORMATION Information;
    ULONGLONG OriginalFileOffset;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Get the current file offset and other file information.
    //

    Status = BlFileGetInformation(FileId, &Information);
    if (!NT_SUCCESS(Status)) {
        goto BlFileReadAtOffsetEnd;
    }

    //
    // Perform common checks and actual count determination.
    //

    AdjustedSize = Size;
    OriginalFileOffset = Information.FileOffset;
    Information.FileOffset = ByteOffset;
    Status = FileInformationCheck(&Information,
                                  FALSE,
                                  &AdjustedSize,
                                  BytesRead,
                                  &AdjustedSize);

    if (!NT_SUCCESS(Status)) {
        goto BlFileReadAtOffsetEnd;
    }

    //
    // If the modified file pointer differs from its original value,
    // the file information needs to be updated.
    //

    if (Information.FileOffset != OriginalFileOffset) {
        Status = BlFileSetInformation(FileId, &Information);
        if (!NT_SUCCESS(Status)) {
            goto BlFileReadAtOffsetEnd;
        }
    }

    //
    // Perform the read request on the specified file.
    //

    Status = BlFileReadEx(FileId, Buffer, AdjustedSize, BytesRead, Flags);
    if (!NT_SUCCESS(Status)) {

        //
        // If the read failed, need to restore the file offset back
        // to its original value.
        //
        // N.B. Ignore the status of the Set and return the original error.
        //

        if (Information.FileOffset != OriginalFileOffset) {
            Information.FileOffset = OriginalFileOffset;
            BlFileSetInformation(FileId, &Information);
        }

        goto BlFileReadAtOffsetEnd;
    }

BlFileReadAtOffsetEnd:
    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlFileWriteAtOffset (
    __in FILE_ID FileId,
    __in ULONG Size,
    __in ULONGLONG ByteOffset,
    __in_bcount(Size) PVOID Buffer,
    __out_opt PULONG BytesWritten
    )

/*++

Routine Description:

    Writes to a file at the specified offset.

Arguments:

    FileId - Supplies a handle to a file.

    Size - Supplies the number of bytes to read to a file.

    ByteOffset - Supplies the file offset to start the write operation.

    Buffer - Supplies a buffer to write to a file.

    BytesWritten - Supplies an optional pointer to a variable that receives the
        number of bytes written.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the transfer buffer is NULL.

    STATUS_INVALID_PARAMETER if the file ID is invalid.

    STATUS_INVALID_PARAMETER if the file is not opened with write access.

    STATUS_INVALID_PARAMETER if the specified offset is invalid.

    Other failing error status codes are possible based on the underlying
        file system's implementation.

--*/

{

    ULONG AdjustedSize;
    FILE_INFORMATION Information;
    ULONGLONG OriginalFileOffset;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Get the current file offset and other file information.
    //

    Status = BlFileGetInformation(FileId, &Information);
    if (!NT_SUCCESS(Status)) {
        goto BlFileWriteAtOffsetEnd;
    }

    //
    // Perform common checks and actual count determination.
    //

    AdjustedSize = Size;
    OriginalFileOffset = Information.FileOffset;
    Information.FileOffset = ByteOffset;
    Status = FileInformationCheck(&Information,
                                  TRUE,
                                  &AdjustedSize,
                                  BytesWritten,
                                  &AdjustedSize);

    if (!NT_SUCCESS(Status)) {
        goto BlFileWriteAtOffsetEnd;
    }

    //
    // If the updated file pointer differs from its original value,
    // the file information needs to be updated.
    //

    if (Information.FileOffset != OriginalFileOffset) {
        Status = BlFileSetInformation(FileId, &Information);
        if (!NT_SUCCESS(Status)) {
            goto BlFileWriteAtOffsetEnd;
        }
    }

    //
    // Perform the write request on the specified file.
    //

    Status = BlFileWrite(FileId, Buffer, AdjustedSize, BytesWritten);
    if (!NT_SUCCESS(Status)) {

        //
        // If the write failed, restore the file offset to its original value.
        //
        // N.B. Ignore the status of the Set operation and return the original error.
        //

        if (Information.FileOffset != OriginalFileOffset) {
            Information.FileOffset = OriginalFileOffset;
            BlFileSetInformation(FileId, &Information);
        }

        goto BlFileWriteAtOffsetEnd;
    }

BlFileWriteAtOffsetEnd:
    BlpInterfaceExit(InterfaceFile);
    return Status;
}

// --------------------------------------------------------------- Private APIs

NTSTATUS
BlpFileInitialize (
    VOID
    )

/*++

Routine Description:

    Initializes data structures used for file i/o (i.e. the file table).

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if allocation fails for internal data structure.

    Other failing error status code possible based on the implementation of the
        registering filesystem.

--*/

{

    const PCFILESYSTEM_REGISTER_FUNCTION_TABLE* FsItem;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Initialize the global file table.
    //

    FileEntries = INITIAL_FILE_ENTRIES;
    FileTable = BlMmAllocateHeap(FileEntries * sizeof(PFILE_ENTRY));
    if (FileTable == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlpFileInitializeEnd;
    }

    Status = STATUS_SUCCESS;
    RtlZeroMemory(FileTable, FileEntries * sizeof(PFILE_ENTRY));
    InitializeListHead(&RegisteredFileSystems);

    //
    // Register the statically implemented file systems.
    //

    for (FsItem = &FsTable[0]; NULL != *FsItem; FsItem += 1) {
        Status = BlpFileRegisterFileSystem((*FsItem)->InitializeRoutine,
                                           (*FsItem)->DestroyRoutine,
                                           (*FsItem)->MountRoutine,
                                           (*FsItem)->PurgeCacheRoutine,
                                           FS_FLAGS_INSERT_AT_TAIL);

        if (!NT_SUCCESS(Status)) {
            goto BlpFileInitializeEnd;
        }
    }


BlpFileInitializeEnd:
    if (!NT_SUCCESS(Status)) {
        if (FileTable != NULL) {
            BlpFileDestroy();
        }
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlpFileDestroy (
    VOID
    )

/*++

Routine Description:

    Destroys all data structures allocated for the file i/o manager.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.

    Failing error status codes are determined based on the implementation of
        the underlying filesystems (Close + Destroy routines).

--*/

{

    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);
    ReturnStatus = STATUS_SUCCESS;

    //
    // Purge each file entry that is in the file table.
    //

    Status = BlTblMap(FileTable, FileEntries, FileTableDestroyEntry, FALSE);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    //
    // Free memory allocated for the global file table. Set the number of
    // file entries to zero to prevent file APIs from referencing the
    // file table after its been freed.
    //

    BlMmFreeHeap(FileTable);
    FileTable = NULL;
    FileEntries = 0;

    //
    // Free the global registered file system list.
    //

    Status = FileIoDestroyRegisteredFileSystems();
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    BlpInterfaceExit(InterfaceFile);
    return ReturnStatus;
}

NTSTATUS
BlpFileRegisterFileSystem (
    __in PFILESYSTEM_INITIALIZE_ROUTINE InitializeRoutine,
    __in PFILESYSTEM_DESTROY_ROUTINE DestroyRoutine,
    __in PFILESYSTEM_MOUNT_ROUTINE MountRoutine,
    __in_opt PFILESYSTEM_PURGE_CACHE_ROUTINE PurgeCacheRoutine,
    __in ULONG Flags
    )

/*++

Routine Description:

    Registers a filesystem with the file i/o manager.  This module maintains
    a list of registered filesystems.  Each registered filesystem contains
    an initialize routine a destroy routine and a mount routine.  The
    initialize routine is called prior to inserting the filesystem into the
    registered list.  The mount routine is called when attempting to mount a
    filesystem to a file.  Lastly, the destroy routine is called from
    from BlpFileDestroy, giving a each filesystem a chance to destroy data
    structures before the FiloIo subsystem is destroyed.

Arguments:

    InitializeRoutine - Supplies an initialization routine for a file system
        implementation to be called by this routine.

    DestroyRoutine - Supplies a destruction routine for a file system
        implementation to be called during the destruction of the file i/o
        manager.

    MountRoutine - Supplies a mount routine for a file system implementation.

    PurgeCacheRoutine - Supplies a routine that purges all file caches.

    Flags - Registration flags (FS_FLAGS_*).  Valid flags include,

        FS_FLAGS_INSERT_AT_HEAD : Indicates that the filesystem should be
            inserted at the head of the registered list.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if allocation fails for internal data structure.

    Other failing error status code possible based on the implementation of the
        registering filesystem.

--*/

{

    PREGISTERED_FILESYSTEM FileSystem;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Allocate memory for the registered filesystem.
    //

    FileSystem = BlMmAllocateHeap(sizeof(REGISTERED_FILESYSTEM));
    if (FileSystem == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlpFileRegisterFileSystemEnd;
    }

    //
    // Give the boot envirionment filesystem implementation a chance to
    // initialize any data structures it has.
    //

    Status = InitializeRoutine();
    if (!NT_SUCCESS(Status)) {
        goto BlpFileRegisterFileSystemEnd;
    }

    //
    // Save the three filesystem routine pointers and insert them into a list
    // with the other registered filesystems.
    //

    FileSystem->FunctionTable.InitializeRoutine = InitializeRoutine;
    FileSystem->FunctionTable.DestroyRoutine = DestroyRoutine;
    FileSystem->FunctionTable.MountRoutine = MountRoutine;
    FileSystem->FunctionTable.PurgeCacheRoutine = PurgeCacheRoutine;
    if (CHECK_FLAG(Flags, FS_FLAGS_INSERT_AT_HEAD)) {
        InsertHeadList(&RegisteredFileSystems, &FileSystem->ListEntry);

    } else {
        InsertTailList(&RegisteredFileSystems, &FileSystem->ListEntry);
    }

BlpFileRegisterFileSystemEnd:
    if (!NT_SUCCESS(Status)) {
        if (FileSystem != NULL) {
            BlMmFreeHeap(FileSystem);
        }
    }

    BlpInterfaceExit(InterfaceFile);
    return Status;
}

NTSTATUS
BlpFileOpen (
    __in DEVICE_ID DeviceId,
    __in PWSTR OpenPath,
    __in OPEN_FLAGS OpenFlags,
    __in ULONG FilterLevel,
    __out PFILE_ID FileId,
    __out_opt PWIM_PROVIDER_EXTERNAL_INFO ReparseData
    )

/*++

Routine Description:

    This function opens a file.  In order to open the file, a filesystem must
    be mounted on the file's block i/o device and the file system's function
    table must be associated with the newly opened file.

    In order to 'open' a file, the parent directory must be 'opened' in order
    for a file system to determine the existence of a file and location on the
    block i/o disk of the file.

    If the file is already open and the OpenFlags flags do not conflict, the
    same file descriptor id may be returned and the reference count to the
    file increased.

Arguments:

    DeviceId - Supplies the handle for the containing the file to open.

    OpenPath - Supplies the path to the file to open.

    OpenFlags - Supplies a bitmask containing flags to use when opening the
        file.

    FilterLevel - Supplies the file system filter level on the underlying block
        i/o device.

    FileId - Supplies a pointer to a variable that receives a handle to file.

Return Value:

    STATUS_SUCCESS when successful.  (FileId will also contain the file's
        handle).

    STATUS_REPARSE when file is a reparse point, and actual data should be
        extracted from a backing file in a WIM.

    STATUS_INVALID_PARAMETER if the specified device is invalid (Invalid handle
        or a handle to a non-block i/o device).

    STATUS_INVALID_PARAMETER if the specified file path is invalid (NULL or
        does not start with a '\').

    STATUS_NO_MEMORY if an allocation fails for an internal data structure.

    STATUS_UNSUCCESSFUL if a filesystem could not be mounted on the underlying
        device.

    STATUS_NO_SUCH_FILE if the requested file does not exist.

--*/

{

    DEVICE_INFORMATION Information;
    PFILE_ENTRY FileEntry;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFile);

    //
    // Basic parameter checking.
    //

    if ((OpenPath == NULL) ||
        (!IS_OPEN_BY_HASH_SET(OpenFlags) && (OpenPath[0] != L'\\')) ||
        (FileId == NULL) ||
        (!IS_OPEN_READ_SET(OpenFlags) && !IS_OPEN_WRITE_SET(OpenFlags))) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlpFileOpenEnd;
    }

    //
    // Make sure DeviceId is a valid ID and that it refers to a Block I/O
    // Device, a UDP device, or a VMBus device.
    //

    Status = BlDeviceGetInformation(DeviceId, &Information);
    if (!NT_SUCCESS(Status) ||
        ((Information.DeviceType != DEVICE_BLOCK_IO) &&
         (Information.DeviceType != DEVICE_PARTITION) &&
         (Information.DeviceType != DEVICE_UDP) &&
         (Information.DeviceType != DEVICE_VMBUS))) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlpFileOpenEnd;
    }

    //
    // Open the requested file.
    //

    Status = FileIoOpen(DeviceId,
                        OpenPath,
                        OpenFlags,
                        FilterLevel,
                        FileTableCompareWithSameAttributes,
                        &FileEntry);

    if (!NT_SUCCESS(Status)) {
        goto BlpFileOpenEnd;
    }

    *FileId = FileEntry->FileId;
    if (IS_FILE_REPARSE_SET(FileEntry->Attributes) != FALSE) {

        NT_ASSERT(IS_OPEN_BY_HASH_SET(OpenFlags) == FALSE);

        if (ARGUMENT_PRESENT(ReparseData)) {

            NT_ASSERT(FileEntry->FunctionTable.GetReparseData != NULL);

            NT_VERIFY(
                NT_SUCCESS(
                    FileEntry->FunctionTable.GetReparseData(FileEntry,
                                                            ReparseData)));
        }

        Status = STATUS_REPARSE;
    }

BlpFileOpenEnd:
    BlpInterfaceExit(InterfaceFile);
    return Status;
}

// ------------------------------------------------------- File Table Functions

NTSTATUS
FileTableDestroyEntry (
    __in PFILE_ENTRY FileEntry,
    __in ULONG Index
    )

/*++

Routine Description:

    Destroys an entry in the file table by calling the entry's close routine
    and freeing memory used by the entry.

Arguments:

    FileEntry - Entry in the file table.

    Index - Index in the file table for FileEntry.

Return Value:

    STATUS_SUCCESS when successful.
    Failing status code determined by the underlying file system
    implementation.

--*/

{

    NTSTATUS Status;

    ASSERT(FileEntry != NULL);

    //
    // Decrement the reference count on the underlying device.  The converse
    // was done when the entry was inserted in the table to ensure a device
    // would not be closed while an entry existed in the file table.
    //

    Status = BlpDeviceDecrementReference(FileEntry->DeviceId);

    ASSERT(Status == STATUS_SUCCESS);

    //
    // Close the File.
    //

    Status = FileEntry->FunctionTable.Close(FileEntry);

    //
    // Free the entry's memory.
    //

    BlMmFreeHeap(FileEntry);
    FileTable[Index] = NULL;
    return Status;
}

BOOLEAN
FileTableCompareWithSameAttributes (
    __in PFILE_ENTRY FileEntry,
    __in PDEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __in POPEN_FLAGS OpenFlags,
    __in PULONG FilterLevel
    )

/*++

Routine Description:

    This routine checks to see if an existing entry in the file table
    (FileEntry) is an instance of a previous open request for the same file.
    This routine will only return positively (TRUE) if the OpenFlags of the
    current request and the existing entry are the same.

Arguments:

    FileEntry - Entry in the file table to compare with the other parameters.

    DeviceId - Device Id for the file to be compared against.

    FilePath - Filepath for the file to be compared against.

    OpenFlags - The open flags for the file to be compared against.

    FilterLevel - The filter level for the filesystem the entry should be
        compared against.

Return Value:

    TRUE, if FileEntry is an entry in the file table for an equivalent file to
    that described by (DeviceId, FilePath, OpenFlags).
    FALSE, otherwise.

--*/

{

    FILE_ATTRIBUTES Attributes;

    //
    // Check to see if the FileEntry was opened on the same device as that
    // passed as a parameter.
    //

    if (FileEntry->DeviceId != *DeviceId) {
        return FALSE;
    }

    //
    // Check to see if the filepath's match.
    //

    if (_wcsicmp(FileEntry->FilePath, FilePath) != 0) {
        return FALSE;
    }

    //
    // Check to see if the filter level (filesystem) match.
    //

    if (FileEntry->FilterLevel != *FilterLevel) {
        return FALSE;
    }

    //
    // Check to see if the read/write flags are the same.
    //

    Attributes = FileEntry->Attributes;
    if ((IS_OPEN_READ_SET(*OpenFlags) && !IS_FILE_READ_SET(Attributes)) ||
        (!IS_OPEN_READ_SET(*OpenFlags) && IS_FILE_READ_SET(Attributes)) ||
        (IS_OPEN_WRITE_SET(*OpenFlags) && !IS_FILE_WRITE_SET(Attributes)) ||
        (!IS_OPEN_WRITE_SET(*OpenFlags) && IS_FILE_WRITE_SET(Attributes)) ) {

        return FALSE;
    }

    //
    // Check to see if the directory flag matches directory attribute.
    //

    if ((IS_OPEN_DIRECTORY_SET(*OpenFlags) &&
         !IS_FILE_DIRECTORY_SET(Attributes)) ||
        (!IS_OPEN_DIRECTORY_SET(*OpenFlags) &&
         IS_FILE_DIRECTORY_SET(Attributes)) ) {

        return FALSE;
    }


    return TRUE;
}

BOOLEAN
FileTableCompareWithSubsetAttributes (
    __in PFILE_ENTRY FileEntry,
    __in PDEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __in POPEN_FLAGS OpenFlags,
    __in PULONG FilterLevel
    )

/*++

Routine Description:

    This routine checks to see if an existing entry in the file table
    (FileEntry) is an instance of a previous open request for the same file.
    This routine will only return positively (TRUE) if the OpenFlags of the
    current request is a subset of those for the existing entry (FileEntry).

Arguments:

    FileEntry - Entry in the file table to compare with the other parameters.

    DeviceId - Device Id for the file to be compared against.

    FilePath - Filepath for the file to be compared 