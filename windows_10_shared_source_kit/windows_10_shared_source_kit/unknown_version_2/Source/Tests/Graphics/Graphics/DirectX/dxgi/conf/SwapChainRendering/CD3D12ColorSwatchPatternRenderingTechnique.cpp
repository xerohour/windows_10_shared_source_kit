b0 "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_350_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_350_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and if b0 nop else block";
string PS_350 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_351_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_351_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and loop aL, i0 block";
string PS_351 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_352_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_352_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and rep i0 block";
string PS_352 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_353_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_353_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and if_ne c0.x, c0.y block";
string PS_353 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_354_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_354_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and if_ne c0.x, c0.y nop else block";
string PS_354 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if_ne c0.x, c0.y nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_355_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_355_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and if p0.x block";
string PS_355 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_356_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_356_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_356 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_357_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_357_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_357 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_358_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_358_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and rep i0 break block";
string PS_358 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_359_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_359_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_359 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_360_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_360_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_360 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_361_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_361_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and loop aL, i0 break_ne c0.x, c0.y block";
string PS_361 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_362_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_362_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break and rep i0 break_ne c0.x, c0.y block";
string PS_362 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 break_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_363_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_363_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and if b0 block";
string PS_363 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if b0 "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_364_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_364_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and if b0 nop else block";
string PS_364 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if b0 nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_365_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_365_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and loop aL, i0 block";
string PS_365 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_366_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_366_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and rep i0 block";
string PS_366 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_367_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_367_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and if_ne c0.x, c0.y block";
string PS_367 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_368_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_368_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and if_ne c0.x, c0.y nop else block";
string PS_368 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if_ne c0.x, c0.y nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_369_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_369_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and if p0.x block";
string PS_369 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if p0.x "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_370_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_370_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and if p0.x nop else block";
string PS_370 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_371_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_371_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and loop aL, i0 break block";
string PS_371 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_372_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_372_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and rep i0 break block";
string PS_372 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_373_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_373_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string PS_373 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_374_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_374_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and rep i0 breakp p0.x block";
string PS_374 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_375_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_375_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and loop aL, i0 break_ne c0.x, c0.y block";
string PS_375 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_376_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_376_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break and rep i0 break_ne c0.x, c0.y block";
string PS_376 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_377_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_377_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and if b0 block";
string PS_377 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_378_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_378_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string PS_378 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_379_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_379_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string PS_379 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_380_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_380_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string PS_380 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_381_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_381_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and if_ne c0.x, c0.y block";
string PS_381 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_382_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_382_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and if_ne c0.x, c0.y nop else block";
string PS_382 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_ne c0.x, c0.y nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_383_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_383_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string PS_383 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_384_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_384_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string PS_384 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_385_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_385_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string PS_385 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_386_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_386_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string PS_386 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_387_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_387_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_387 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_388_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_388_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_388 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_389_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_389_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_ne c0.x, c0.y block";
string PS_389 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_390_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_390_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break_ne c0.x, c0.y block";
string PS_390 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_391_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_391_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and if b0 block";
string PS_391 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_392_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_392_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and if b0 nop else block";
string PS_392 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_393_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_393_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string PS_393 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_394_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_394_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and rep i0 block";
string PS_394 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_395_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_395_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and if_ne c0.x, c0.y block";
string PS_395 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_396_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_396_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and if_ne c0.x, c0.y nop else block";
string PS_396 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if_ne c0.x, c0.y nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_397_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_397_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and if p0.x block";
string PS_397 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_398_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_398_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string PS_398 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_399_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_399_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string PS_399 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_400_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_400_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and rep i0 break block";
string PS_400 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_401_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_401_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_401 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_402_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_402_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_402 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_403_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_403_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break_ne c0.x, c0.y block";
string PS_403 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_404_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_404_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 breakp p0.x and rep i0 break_ne c0.x, c0.y block";
string PS_404 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_405_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_405_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and if b0 block";
string PS_405 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	if b0 "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_406_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_406_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and if b0 nop else block";
string PS_406 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	if b0 nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_407_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_407_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and loop aL, i0 block";
string PS_407 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	loop aL, i0 "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_408_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_408_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and rep i0 block";
string PS_408 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	rep i0 "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_409_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_409_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and if_ne c0.x, c0.y block";
string PS_409 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_410_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_410_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and if_ne c0.x, c0.y nop else block";
string PS_410 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_411_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_411_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and if p0.x block";
string PS_411 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	if p0.x "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_412_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_412_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and if p0.x nop else block";
string PS_412 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	if p0.x nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_413_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_413_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and loop aL, i0 break block";
string PS_413 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	loop aL, i0 break "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_414_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_414_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and rep i0 break block";
string PS_414 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	rep i0 break "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_415_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_415_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_415 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_416_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_416_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and rep i0 breakp p0.x block";
string PS_416 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_417_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_417_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and loop aL, i0 break_ne c0.x, c0.y block";
string PS_417 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_418_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_418_Desc = "ps_2_sw : texldb is NOT allowed in a loop aL, i0 break_ne c0.x, c0.y and rep i0 break_ne c0.x, c0.y block";
string PS_418 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	rep i0 break_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_419_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_419_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and if b0 block";
string PS_419 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	if b0 "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_420_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_420_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and if b0 nop else block";
string PS_420 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	if b0 nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_421_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_421_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and loop aL, i0 block";
string PS_421 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	loop aL, i0 "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_422_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_422_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and rep i0 block";
string PS_422 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	rep i0 "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_423_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_423_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and if_ne c0.x, c0.y block";
string PS_423 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_424_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_424_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and if_ne c0.x, c0.y nop else block";
string PS_424 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_425_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_425_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and if p0.x block";
string PS_425 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	if p0.x "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_426_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_426_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and if p0.x nop else block";
string PS_426 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	if p0.x nop else "
	"		texldb r0, r0, s0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_427_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_427_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and loop aL, i0 break block";
string PS_427 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	loop aL, i0 break "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_428_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_428_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and rep i0 break block";
string PS_428 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	rep i0 break "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_429_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_429_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_429 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_430_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_430_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and rep i0 breakp p0.x block";
string PS_430 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_431_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_431_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and loop aL, i0 break_ne c0.x, c0.y block";
string PS_431 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_432_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_432_Desc = "ps_2_sw : texldb is NOT allowed in a rep i0 break_ne c0.x, c0.y and rep i0 break_ne c0.x, c0.y block";
string PS_432 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mov r0, c0 "
	"rep i0 break_ne c0.x, c0.y "
	"	rep i0 break_ne c0.x, c0.y "
	"		texldb r0, r0, s0 "
	"	endrep "
	"endrep ";


//////////////////////////////////////////////////////////////////////
// File:  CD3D12ColorSwatchPatternRenderingTechnique.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a technique to render a pattern of colored
// swatches.
//
// History:
// 12 Aug 2015  SEdmison  Factored out of CD3D12SceneRenderer.cpp.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "CD3D12ColorSwatchPatternRenderingTechnique.hpp"
#include "CD3D12SceneRenderer.hpp"

// Pre-compiled shader blobs
#include "ShadedObjectVertexShader_5_1.h"
#include "ShadedObjectPixelShader_5_1.h"


/////////////////////////////////////////////////////////////////
//
// CD3D12ColorSwatchPatternRenderingTechnique implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Static initializers
//
////////////////////////////////////////////////////////////

COLOR_SWATCH CD3D12ColorSwatchPatternRenderingTechnique::s_BackgroundSwapChainColorSwatches[] =
{
	// White swatch in the upper-left 10% rown and column of the render target
	{
		{ 0.f, 0.f, .1f, .1f },
		{ 1.f, 1.f, 1.f, 1.f },
	},

	// Light gray swatch in the upper row, second column
	{
		{ .1f, 0.f, .2f, .1f },
		{ .75f, .75f, .75f, 1.f },
	},

	// Medium gray swatch in the upper row, third column
	{
		{ .2f, 0.f, .3f, .1f },
		{ .5f, .5f, .5f, 1.f },
	},

	// Dark gray swatch in the upper row, fourth column
	{
		{ .3f, 0.f, .4f, .1f },
		{ .25f, .25f, .25f, 1.f },
	},

	// Medium red swatch in the third row, first column
	{
		{ 0.f, .2f, .1f, .3f },
		{ .5f, 0.f, 0.f, 1.f },
	},

	// Medium green swatch in the third row, second column
	{
		{ .1f, .2f, .2f, .3f },
		{ 0.f, .5f, 0.f, 1.f },
	},

	// Medium blue swatch in the third row, fourth column
	{
		{ .2f, .2f, .3f, .3f },
		{ 0.f, 0.f, 0.5f, 1.f },
	},
};


COLOR_SWATCH CD3D12ColorSwatchPatternRenderingTechnique::s_ForegroundSwapChainColorSwatches[] =
{
	// White swatch in the second row, first column
	{
		{ 0.f, .1f, .1f, .2f },
		{ 1.f, 1.f, 1.f, 1.f },
	},

	// Light gray swatch in the second row, second column
	{
		{ .1f, .1f, .2f, .2f },
		{ .75f, .75f, .75f, 1.f },
	},

	// Medium gray swatch in the second row, third column
	{
		{ .2f, .1f, .3f, .2f },
		{ .5f, .5f, .5f, 1.f },
	},

	// Dark gray swatch in the second row, fourth column
	{
		{ .3f, .1f, .4f, .2f },
		{ .25f, .25f, .25f, 1.f },
	},

	// Medium red swatch in the fourth row, first column
	{
		{ 0.f, .3f, .1f, .4f },
		{ .5f, 0.f, 0.f, 1.f },
	},

	// Medium green swatch in the fourth row, second column
	{
		{ .1f, .3f, .2f, .4f },
		{ 0.f, .5f, 0.f, 1.f },
	},

	// Medium blue swatch in the fourth row, third column
	{
		{ .2f, .3f, .3f, .4f },
		{ 0.f, 0.f, 0.5f, 1.f },
	},

	// Medium red, half-opaque swatch in the firth row, first column
	{
		{ 0.f, .4f, .1f, .5f },
		{ .5f, 0.f, 0.f, .5f },
	},

	// Medium green, half-opaque swatch in the fifth row, second column
	{
		{ .1f, .4f, .2f, .5f },
		{ 0.f, .5f, 0.f, .5f },
	},

	// Medium blue, half-opaque swatch in the firth row, third column
	{
		{ .2f, .4f, .3f, .5f },
		{ 0.f, 0.f, 0.5f, .5f },
	},
};


////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D12ColorSwatchPatternRenderingTechnique
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D12ColorSwatchPatternRenderingTechnique::CD3D12ColorSwatchPatternRenderingTechnique
(
	_In_ CD3D12SceneRenderer * pParentRenderer
)
:
	CD3D12RenderingTechnique( pParentRenderer ),
	m_pColorSwatchGraphicsPipelineState( nullptr )
{
};


///////////////////////////////////////////////////////
// Function:  ~CD3D12ColorSwatchPatternRenderingTechnique
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CD3D12ColorSwatchPatternRenderingTechnique::~CD3D12ColorSwatchPatternRenderingTechnique()
{
	assert( m_pColorSwatchGraphicsPipelineState == nullptr );
};


////////////////////////////////////////////////////////////
//
// Public method implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CreateResources
//
// Purpose:
// Creates 