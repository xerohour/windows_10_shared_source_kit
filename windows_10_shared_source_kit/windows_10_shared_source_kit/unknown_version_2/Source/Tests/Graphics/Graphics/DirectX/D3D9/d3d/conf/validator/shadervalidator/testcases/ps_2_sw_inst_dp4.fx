 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_375_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_375_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and rep i0 break block";
string VS_375 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_376_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_376_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string VS_376 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_377_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_377_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string VS_377 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_378_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_378_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and loop aL, i0 break_ne c0.x, c0.y block";
string VS_378 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_379_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_379_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and rep i0 break_ne c0.x, c0.y block";
string VS_379 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 break_ne c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_380_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_380_Desc = "vs_2_x : sub is allowed in a rep i0 break and if b0 block";
string VS_380 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_381_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_381_Desc = "vs_2_x : sub is allowed in a rep i0 break and if b0 nop else block";
string VS_381 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_382_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_382_Desc = "vs_2_x : sub is allowed in a rep i0 break and loop aL, i0 block";
string VS_382 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_383_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_383_Desc = "vs_2_x : sub is allowed in a rep i0 break and rep i0 block";
string VS_383 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_384_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_384_Desc = "vs_2_x : sub is allowed in a rep i0 break and if_ne c0.x, c0.y block";
string VS_384 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if_ne c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_385_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_385_Desc = "vs_2_x : sub is allowed in a rep i0 break and if_ne c0.x, c0.y nop else block";
string VS_385 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if_ne c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_386_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_386_Desc = "vs_2_x : sub is allowed in a rep i0 break and if p0.x block";
string VS_386 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_387_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_387_Desc = "vs_2_x : sub is allowed in a rep i0 break and if p0.x nop else block";
string VS_387 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_388_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_388_Desc = "vs_2_x : sub is allowed in a rep i0 break and loop aL, i0 break block";
string VS_388 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_389_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_389_Desc = "vs_2_x : sub is allowed in a rep i0 break and rep i0 break block";
string VS_389 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_390_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_390_Desc = "vs_2_x : sub is allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string VS_390 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_391_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_391_Desc = "vs_2_x : sub is allowed in a rep i0 break and rep i0 breakp p0.x block";
string VS_391 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_392_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_392_Desc = "vs_2_x : sub is allowed in a rep i0 break and loop aL, i0 break_ne c0.x, c0.y block";
string VS_392 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_393_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_393_Desc = "vs_2_x : sub is allowed in a rep i0 break and rep i0 break_ne c0.x, c0.y block";
string VS_393 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break_ne c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_394_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_394_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and if b0 block";
string VS_394 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_395_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_395_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string VS_395 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_396_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_396_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string VS_396 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_397_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_397_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string VS_397 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_398_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_398_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and if_ne c0.x, c0.y block";
string VS_398 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_ne c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_399_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_399_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and if_ne c0.x, c0.y nop else block";
string VS_399 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_ne c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_400_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_400_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string VS_400 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_401_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_401_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string VS_401 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_402_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_402_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string VS_402 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_403_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_403_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string VS_403 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_404_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_404_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string VS_404 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_405_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_405_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string VS_405 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_406_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_406_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_gt c0.x, c0.y block";
string VS_406 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_407_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_407_Desc = "vs_2_x : sub is allowed in a loop aL, i0 breakp p0.x and rep i0 break_gt c0.x, c0.y block";
string VS_407 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_gt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_408_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_408_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and if b0 block";
string VS_408 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_409_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_409_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string VS_409 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_410_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_410_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string VS_410 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_411_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_411_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and rep i0 block";
string VS_411 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_412_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_412_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and if_gt c0.x, c0.y block";
string VS_412 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if_gt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_413_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_413_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and if_gt c0.x, c0.y nop else block";
string VS_413 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if_gt c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_414_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_414_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and if p0.x block";
string VS_414 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_415_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_415_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string VS_415 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_416_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_416_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string VS_416 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_417_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_417_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and rep i0 break block";
string VS_417 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_418_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_418_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string VS_418 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_419_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_419_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string VS_419 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_420_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_420_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and loop aL, i0 break_gt c0.x, c0.y block";
string VS_420 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_421_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_421_Desc = "vs_2_x : sub is allowed in a rep i0 breakp p0.x and rep i0 break_gt c0.x, c0.y block";
string VS_421 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_gt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_422_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_422_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_gt c0.x, c0.y and if b0 block";
string VS_422 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_423_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_423_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_gt c0.x, c0.y and if b0 nop else block";
string VS_423 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_424_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_424_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 block";
string VS_424 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_425_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_425_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 block";
string VS_425 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_426_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_426_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_ge c0.x, c0.y and if_ge c0.x, c0.y block";
string VS_426 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_427_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_427_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_ge c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string VS_427 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_428_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_428_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_ge c0.x, c0.y and if p0.x block";
string VS_428 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_429_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_429_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_ge c0.x, c0.y and if p0.x nop else block";
string VS_429 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_430_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_430_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 break block";
string VS_430 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_431_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_431_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 break block";
string VS_431 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_432_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_432_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_432 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_433_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_433_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 breakp p0.x block";
string VS_433 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_434_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_434_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 break_ge c0.x, c0.y block";
string VS_434 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_435_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_435_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 break_ge c0.x, c0.y block";
string VS_435 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_436_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_436_Desc = "vs_2_x : sub is allowed in a rep i0 break_ge c0.x, c0.y and if b0 block";
string VS_436 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_437_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_437_Desc = "vs_2_x : sub is allowed in a rep i0 break_ge c0.x, c0.y and if b0 nop else block";
string VS_437 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_438_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_438_Desc = "vs_2_x : sub is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 block";
string VS_438 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_439_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_439_Desc = "vs_2_x : sub is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 block";
string VS_439 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_440_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_440_Desc = "vs_2_x : sub is allowed in a rep i0 break_ge c0.x, c0.y and if_ge c0.x, c0.y block";
string VS_440 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_441_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_441_Desc = "vs_2_x : sub is allowed in a rep i0 break_ge c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string VS_441 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_442_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_442_Desc = "vs_2_x : sub is allowed in a rep i0 break_ge c0.x, c0.y and if p0.x block";
string VS_442 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_443_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_443_Desc = "vs_2_x : sub is allowed in a rep i0 break_ge c0.x, c0.y and if p0.x nop else block";
string VS_443 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_444_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_444_Desc = "vs_2_x : sub is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 break block";
string VS_444 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_445_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_445_Desc = "vs_2_x : sub is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 break block";
string VS_445 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_446_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_446_Desc = "vs_2_x : sub is allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_446 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_447_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_447_Desc = "vs_2_x : sub is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 breakp p0.x block";
string VS_447 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_448_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_448_Desc = "vs_2_x : sub is allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 break_lt c0.x, c0.y block";
string VS_448 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_449_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_449_Desc = "vs_2_x : sub is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 break_lt c0.x, c0.y block";
string VS_449 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";


//{{NO_DEPENDENCIES}}
// Microsoft Developer Studio generated include file.
// Used by wavetest.rc
//
#define APPICON                         100
#define IDD_FORMATS                     101
#define IDD_TIMER                       102

#define IDC_8BIT_R                      1020
#define IDC_16BIT_R                     1021
#define IDC_20BIT_R                     1022
#define IDC_24BIT_R                     1023
#define IDC_32BIT_R                     1024

#define IDC_8000HZ_R                    1025
#define IDC_11025HZ_R                   1026
#define IDC_16000HZ_R                   1027
#define IDC_22050HZ_R                   1028
#define IDC_32000HZ_R                   1029
#define IDC_44100HZ_R                   1030
#define IDC_48000HZ_R                   1031

#define IDC_MONO_R                      1032
#define IDC_STEREO_R                    1033
#define IDC_QUAD_R                      1034
#define IDC_5_1_R                       1035

#define IDC_8BIT_C                      1040
#define IDC_16BIT_C                     1041
#define IDC_20BIT_C                     1042
#define IDC_24BIT_C                     1043
#define IDC_32BIT_C                     1044

#define IDC_8000HZ_C                    1045
#define IDC_11025HZ_C                   1046
#define IDC_16000HZ_C                   1047
#define IDC_22050HZ_C                   1048
#define IDC_32000HZ_C                   1049
#define IDC_44100HZ_C                   1050
#define IDC_48000HZ_C                   1051

#define IDC_MONO_C                      1052
#define IDC_STEREO_C                    1053
#define IDC_QUAD_C                      1054
#define IDC_5_1_C                       1055
/*
#define IDC_BUFFSIZE_RENDER             1060
#define IDC_BUFFSIZE_CAPTURE            1061
*/
#define IDC_ALLPINS                     1070
#define IDC_PINS_R                      1071
#define IDC_PINS_C                      1072
#define IDC_TIMER                       1073

#define IDC_STATIC                      -1

#define IDM_FORMATS                     IDM_FNSUSER + 80
#define IDM_SYSAUDIO                    IDM_FNSUSER + 81
#define IDM_KSFILTER                    IDM_FNSUSER + 82
#define D3D_OVERLOADS
#include <d3d8.h>
#include "types.h"
#include "Perspective.h"

//************************************************************************
// Perspective Color Test functions

CColorTest::CColorTest()
{
	m_szTestName = "Perspective Color";
	m_szCommandKey = "Color";

    // Init Framework options
    m_Options.D3DOptions.fMinDXVersion = 8.0f;
}

CColorTest::~CColorTest()
{
}

UINT CColorTest::TestInitialize(void)
{
#if FAST_TEST
    m_totalNumTestCases = MAX_GRID_TESTS + MAX_TRI_TESTS;
    // Setup the Test range
    SetTestRange(1, 1);
#else
	// Setup the Test range
	SetTestRange(1, MAX_GRID_TESTS + MAX_TRI_TESTS);
#endif

	// Make sure we are on at lease DX8
	if (m_dwVersion <= 0x0700)
	{
		WriteToLog("This test requires at least Dx8.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check the caps we are interested in
	DWORD dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;

	// Check the appropriate Raster cap
	if (!(dwRasterCaps & D3DPRASTERCAPS_COLORPERSPECTIVE))
	{
		WriteToLog("Device capability not supported: Raster ColorPerspective.\n");
		return D3DTESTINIT_SKIPALL;
    }

	return D3DTESTINIT_RUN;
}
#include "Test_Include.fx"

int Test_Count = 412;

string TestInfo
<
	string TestType = "PS";
>
= "ps_2_sw_inst_dp4";

PS_CRITERIA PS_001_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_2_sw : dp4 source reg combination v0, v1 is allowed";
string PS_001 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_2_sw : dp4 source reg combination v0, r0 is allowed";
string PS_002 = 
	"ps_2_sw "
	"dcl v0 "
	"mov r0, c0 "
	"dp4 r0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_003_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_2_sw : dp4 source reg combination v0, c0 is allowed";
string PS_003 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dp4 r0, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_004_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_2_sw : dp4 source reg combination v0, t0 is allowed";
string PS_004 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl t0 "
	"dp4 r0, v0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_005_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_2_sw : dp4 source reg combination r0, v0 is allowed";
string PS_005 = 
	"ps_2_sw "
	"dcl v0 "
	"mov r0, c0 "
	"dp4 r0, r0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_006_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_2_sw : dp4 source reg combination r0, r1 is allowed";
string PS_006 = 
	"ps_2_sw "
	"mov r0, c0 "
	"mov r1, c0 "
	"dp4 r0, r0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_007_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_2_sw : dp4 source reg combination r0, c0 is allowed";
string PS_007 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp4 r0, r0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_008_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_2_sw : dp4 source reg combination r0, t0 is allowed";
string PS_008 = 
	"ps_2_sw "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0, r0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_009_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_2_sw : dp4 source reg combination c0, v0 is allowed";
string PS_009 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dp4 r0, c0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_010_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_2_sw : dp4 source reg combination c0, r0 is allowed";
string PS_010 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp4 r0, c0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_011_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_2_sw : dp4 source reg combination c0, c1 is allowed";
string PS_011 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_012_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_2_sw : dp4 source reg combination c0, t0 is allowed";
string PS_012 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, c0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_013_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_2_sw : dp4 source reg combination t0, v0 is allowed";
string PS_013 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl v0 "
	"dp4 r0, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_014_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_2_sw : dp4 source reg combination t0, r0 is allowed";
string PS_014 = 
	"ps_2_sw "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_015_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_2_sw : dp4 source reg combination t0, c0 is allowed";
string PS_015 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_016_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_016_Desc = "ps_2_sw : dp4 source reg combination t0, t0 is allowed";
string PS_016 = 
	"ps_2_sw "
	"dcl t0 "
	"dp4 r0, t0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_017_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_2_sw : dp4 source reg combination t0, t1 is allowed";
string PS_017 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl t1 "
	"dp4 r0, t0, t1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_018_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_2_sw : dp4 source reg combination -c0, -c1 is allowed";
string PS_018 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, -c0, -c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_019_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_2_sw : dp4 source reg combination -c0, c1 is allowed";
string PS_019 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, -c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_020_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_2_sw : dp4 source reg combination c0, -c1 is allowed";
string PS_020 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, -c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_021_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_2_sw : dp4 source reg combination c0, c1 is allowed";
string PS_021 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_022_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_2_sw : dp4 source reg combination c0.r, c1 is allowed";
string PS_022 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.r, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_023_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_2_sw : dp4 source reg combination c0.g, c1 is allowed";
string PS_023 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.g, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_024_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_2_sw : dp4 source reg combination c0.b, c1 is allowed";
string PS_024 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.b, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_025_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_2_sw : dp4 source reg combination c0.a, c1 is allowed";
string PS_025 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.a, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_026_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_2_sw : dp4 source reg combination c0.gbra, c1 is allowed";
string PS_026 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.gbra, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_027_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_2_sw : dp4 source reg combination c0.brga, c1 is allowed";
string PS_027 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.brga, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_028_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_2_sw : dp4 source reg combination c0.abgr, c1 is allowed";
string PS_028 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.abgr, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_029_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_2_sw : dp4 source reg combination c0.agrb, c1 is allowed";
string PS_029 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.agrb, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_030_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_2_sw : dp4 source reg combination c0.rbga, c1 is allowed";
string PS_030 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.rbga, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_031_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_2_sw : dp4 source reg combination c0.rgab, c1 is allowed";
string PS_031 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.rgab, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_032_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_2_sw : dp4 source reg combination c0.bgr, c1 is allowed";
string PS_032 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.bgr, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_033_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_2_sw : dp4 source reg combination c0.rbg, c1 is allowed";
string PS_033 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.rbg, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_034_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_2_sw : dp4 source reg combination c0.gar, c1 is allowed";
string PS_034 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.gar, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_035_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_2_sw : dp4 source reg combination c0.gr, c1 is allowed";
string PS_035 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.gr, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_036_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_2_sw : dp4 source reg combination c0.ab, c1 is allowed";
string PS_036 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.ab, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_037_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_2_sw : dp4 source reg combination c0.bg, c1 is allowed";
string PS_037 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0.bg, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_038_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_2_sw : dp4 source reg combination c0, c1.r is allowed";
string PS_038 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.r "
	"mov oC0, c0 ";

PS_CRITERIA PS_039_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_2_sw : dp4 source reg combination c0, c1.g is allowed";
string PS_039 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.g "
	"mov oC0, c0 ";

PS_CRITERIA PS_040_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_2_sw : dp4 source reg combination c0, c1.b is allowed";
string PS_040 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.b "
	"mov oC0, c0 ";

PS_CRITERIA PS_041_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_2_sw : dp4 source reg combination c0, c1.a is allowed";
string PS_041 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.a "
	"mov oC0, c0 ";

PS_CRITERIA PS_042_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_2_sw : dp4 source reg combination c0, c1.gbra is allowed";
string PS_042 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.gbra "
	"mov oC0, c0 ";

PS_CRITERIA PS_043_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_2_sw : dp4 source reg combination c0, c1.brga is allowed";
string PS_043 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.brga "
	"mov oC0, c0 ";

PS_CRITERIA PS_044_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_2_sw : dp4 source reg combination c0, c1.abgr is allowed";
string PS_044 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.abgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_045_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_2_sw : dp4 source reg combination c0, c1.agrb is allowed";
string PS_045 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.agrb "
	"mov oC0, c0 ";

PS_CRITERIA PS_046_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_2_sw : dp4 source reg combination c0, c1.rbga is allowed";
string PS_046 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.rbga "
	"mov oC0, c0 ";

PS_CRITERIA PS_047_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_2_sw : dp4 source reg combination c0, c1.rgab is allowed";
string PS_047 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.rgab "
	"mov oC0, c0 ";

PS_CRITERIA PS_048_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_2_sw : dp4 source reg combination c0, c1.bgr is allowed";
string PS_048 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.bgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_049_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_2_sw : dp4 source reg combination c0, c1.rbg is allowed";
string PS_049 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.rbg "
	"mov oC0, c0 ";

PS_CRITERIA PS_050_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_2_sw : dp4 source reg combination c0, c1.gar is allowed";
string PS_050 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.gar "
	"mov oC0, c0 ";

PS_CRITERIA PS_051_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_2_sw : dp4 source reg combination c0, c1.gr is allowed";
string PS_051 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.gr "
	"mov oC0, c0 ";

PS_CRITERIA PS_052_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_2_sw : dp4 source reg combination c0, c1.ab is allowed";
string PS_052 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.ab "
	"mov oC0, c0 ";

PS_CRITERIA PS_053_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_2_sw : dp4 source reg combination c0, c1.bg is allowed";
string PS_053 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1.bg "
	"mov oC0, c0 ";

PS_CRITERIA PS_054_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_2_sw : dp4 source reg combination b0, b0 is NOT allowed";
string PS_054 = 
	"ps_2_sw "
	"defb b0, true "
	"dp4 r0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_055_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_2_sw : dp4 source reg combination b0, i0 is NOT allowed";
string PS_055 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dp4 r0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_056_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_2_sw : dp4 source reg combination b0, p0 is NOT allowed";
string PS_056 = 
	"ps_2_sw "
	"defb b0, true "
	"dp4 r0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_057_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_2_sw : dp4 source reg combination b0, s0 is NOT allowed";
string PS_057 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"dp4 r0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_058_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_2_sw : dp4 source reg combination b0, oC0 is NOT allowed";
string PS_058 = 
	"ps_2_sw "
	"defb b0, true "
	"dp4 r0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_059_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_2_sw : dp4 source reg combination b0, oDepth is NOT allowed";
string PS_059 = 
	"ps_2_sw "
	"defb b0, true "
	"dp4 r0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_060_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_2_sw : dp4 source reg combination i0, b0 is NOT allowed";
string PS_060 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_061_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_2_sw : dp4 source reg combination i0, i0 is NOT allowed";
string PS_061 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_062_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_2_sw : dp4 source reg combination i0, p0 is NOT allowed";
string PS_062 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_063_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_2_sw : dp4 source reg combination i0, s0 is NOT allowed";
string PS_063 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dp4 r0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_064_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_2_sw : dp4 source reg combination i0, oC0 is NOT allowed";
string PS_064 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_065_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_2_sw : dp4 source reg combination i0, oDepth is NOT allowed";
string PS_065 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_066_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_2_sw : dp4 source reg combination p0, b0 is NOT allowed";
string PS_066 = 
	"ps_2_sw "
	"defb b0, true "
	"dp4 r0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_067_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_2_sw : dp4 source reg combination p0, i0 is NOT allowed";
string PS_067 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_068_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_2_sw : dp4 source reg combination p0, p0 is NOT allowed";
string PS_068 = 
	"ps_2_sw "
	"dp4 r0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_069_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_2_sw : dp4 source reg combination p0, s0 is NOT allowed";
string PS_069 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"dp4 r0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_070_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_2_sw : dp4 source reg combination p0, oC0 is NOT allowed";
string PS_070 = 
	"ps_2_sw "
	"dp4 r0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_071_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_2_sw : dp4 source reg combination p0, oDepth is NOT allowed";
string PS_071 = 
	"ps_2_sw "
	"dp4 r0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_072_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_2_sw : dp4 source reg combination s0, b0 is NOT allowed";
string PS_072 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"dp4 r0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_073_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_2_sw : dp4 source reg combination s0, i0 is NOT allowed";
string PS_073 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dp4 r0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_074_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_2_sw : dp4 source reg combination s0, p0 is NOT allowed";
string PS_074 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"dp4 r0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_075_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_2_sw : dp4 source reg combination s0, s0 is NOT allowed";
string PS_075 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"dp4 r0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_076_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_2_sw : dp4 source reg combination s0, oC0 is NOT allowed";
string PS_076 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"dp4 r0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_077_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_2_sw : dp4 source reg combination s0, oDepth is NOT allowed";
string PS_077 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"dp4 r0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_078_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_2_sw : dp4 source reg combination oC0, b0 is NOT allowed";
string PS_078 = 
	"ps_2_sw "
	"defb b0, true "
	"dp4 r0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_079_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_2_sw : dp4 source reg combination oC0, i0 is NOT allowed";
string PS_079 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_080_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_2_sw : dp4 source reg combination oC0, p0 is NOT allowed";
string PS_080 = 
	"ps_2_sw "
	"dp4 r0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_081_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_2_sw : dp4 source reg combination oC0, s0 is NOT allowed";
string PS_081 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"dp4 r0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_082_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_2_sw : dp4 source reg combination oC0, oC0 is NOT allowed";
string PS_082 = 
	"ps_2_sw "
	"dp4 r0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_083_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_2_sw : dp4 source reg combination oC0, oDepth is NOT allowed";
string PS_083 = 
	"ps_2_sw "
	"dp4 r0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_084_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_2_sw : dp4 source reg combination oDepth, b0 is NOT allowed";
string PS_084 = 
	"ps_2_sw "
	"defb b0, true "
	"dp4 r0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_085_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_2_sw : dp4 source reg combination oDepth, i0 is NOT allowed";
string PS_085 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_086_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_2_sw : dp4 source reg combination oDepth, p0 is NOT allowed";
string PS_086 = 
	"ps_2_sw "
	"dp4 r0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_087_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_2_sw : dp4 source reg combination oDepth, s0 is NOT allowed";
string PS_087 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"dp4 r0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_088_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_2_sw : dp4 source reg combination oDepth, oC0 is NOT allowed";
string PS_088 = 
	"ps_2_sw "
	"dp4 r0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_089_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_2_sw : dp4 source reg combination oDepth, oDepth is NOT allowed";
string PS_089 = 
	"ps_2_sw "
	"dp4 r0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_090_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_2_sw : dp4 dest r# is allowed";
string PS_090 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_091_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_2_sw : dp4 _pp dest r# is allowed";
string PS_091 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_pp r0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_092_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_2_sw : dp4 _sat dest r# is allowed";
string PS_092 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_sat r0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_093_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_2_sw : dp4 dest v# is NOT allowed";
string PS_093 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 v0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_094_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_2_sw : dp4 _pp dest v# is NOT allowed";
string PS_094 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_pp v0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_095_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_2_sw : dp4 _sat dest v# is NOT allowed";
string PS_095 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_sat v0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_096_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_2_sw : dp4 dest c# is NOT allowed";
string PS_096 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 c1, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_097_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_2_sw : dp4 _pp dest c# is NOT allowed";
string PS_097 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_pp c1, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_098_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_2_sw : dp4 _sat dest c# is NOT allowed";
string PS_098 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_sat c1, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_099_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_2_sw : dp4 dest b# is NOT allowed";
string PS_099 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 b0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_100_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_2_sw : dp4 _pp dest b# is NOT allowed";
string PS_100 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_pp b0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_101_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_2_sw : dp4 _sat dest b# is NOT allowed";
string PS_101 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_sat b0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_102_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_2_sw : dp4 dest i# is NOT allowed";
string PS_102 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 i0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_103_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_2_sw : dp4 _pp dest i# is NOT allowed";
string PS_103 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_pp i0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_104_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_2_sw : dp4 _sat dest i# is NOT allowed";
string PS_104 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_sat i0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_105_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_105_Desc = "ps_2_sw : dp4 dest p0 is NOT allowed";
string PS_105 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 p0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_106_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_106_Desc = "ps_2_sw : dp4 _pp dest p0 is NOT allowed";
string PS_106 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_pp p0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_107_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_107_Desc = "ps_2_sw : dp4 _sat dest p0 is NOT allowed";
string PS_107 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_sat p0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_108_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_2_sw : dp4 dest s# is NOT allowed";
string PS_108 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 s0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_109_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_2_sw : dp4 _pp dest s# is NOT allowed";
string PS_109 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_pp s0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_110_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_2_sw : dp4 _sat dest s# is NOT allowed";
string PS_110 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_sat s0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_111_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_2_sw : dp4 dest t# is NOT allowed";
string PS_111 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 t0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_112_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_2_sw : dp4 _pp dest t# is NOT allowed";
string PS_112 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_pp t0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_113_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_2_sw : dp4 _sat dest t# is NOT allowed";
string PS_113 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_sat t0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_114_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_2_sw : dp4 dest oC# is NOT allowed";
string PS_114 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 oC0, v0, v1 ";

PS_CRITERIA PS_115_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_2_sw : dp4 _pp dest oC# is NOT allowed";
string PS_115 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_pp oC0, v0, v1 ";

PS_CRITERIA PS_116_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_116_Desc = "ps_2_sw : dp4 _sat dest oC# is NOT allowed";
string PS_116 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_sat oC0, v0, v1 ";

PS_CRITERIA PS_117_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_117_Desc = "ps_2_sw : dp4 dest oDepth is NOT allowed";
string PS_117 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 oDepth, v0.x, v1.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_118_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_118_Desc = "ps_2_sw : dp4 _pp dest oDepth is NOT allowed";
string PS_118 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_pp oDepth, v0.x, v1.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_119_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_119_Desc = "ps_2_sw : dp4 _sat dest oDepth is NOT allowed";
string PS_119 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4_sat oDepth, v0.x, v1.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_120_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_120_Desc = "ps_2_sw : dp4 write mask .r is allowed";
string PS_120 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.r, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_121_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_121_Desc = "ps_2_sw : dp4 write mask .g is allowed";
string PS_121 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.g, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_122_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_122_Desc = "ps_2_sw : dp4 write mask .b is allowed";
string PS_122 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.b, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_123_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_123_Desc = "ps_2_sw : dp4 write mask .a is allowed";
string PS_123 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.a, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_124_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_124_Desc = "ps_2_sw : dp4 write mask .rg is allowed";
string PS_124 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.rg, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_125_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_125_Desc = "ps_2_sw : dp4 write mask .gb is allowed";
string PS_125 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.gb, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_126_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_126_Desc = "ps_2_sw : dp4 write mask .ba is allowed";
string PS_126 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.ba, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_127_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_127_Desc = "ps_2_sw : dp4 write mask .rb is allowed";
string PS_127 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.rb, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_128_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_128_Desc = "ps_2_sw : dp4 write mask .ra is allowed";
string PS_128 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.ra, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_129_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_129_Desc = "ps_2_sw : dp4 write mask .ga is allowed";
string PS_129 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.ga, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_130_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_130_Desc = "ps_2_sw : dp4 write mask .rgb is allowed";
string PS_130 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.rgb, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_131_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_131_Desc = "ps_2_sw : dp4 write mask .gba is allowed";
string PS_131 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.gba, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_132_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_132_Desc = "ps_2_sw : dp4 write mask .rba is allowed";
string PS_132 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.rba, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_133_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_133_Desc = "ps_2_sw : dp4 write mask .rgba is allowed";
string PS_133 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.rgba, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_134_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_134_Desc = "ps_2_sw : dp4 write mask .yx is NOT allowed";
string PS_134 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.yx, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_135_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_135_Desc = "ps_2_sw : dp4 write mask .zx is NOT allowed";
string PS_135 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.zx, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_136_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_136_Desc = "ps_2_sw : dp4 write mask .zy is NOT allowed";
string PS_136 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.zy, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_137_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_137_Desc = "ps_2_sw : dp4 write mask .wx is NOT allowed";
string PS_137 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.wx, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_138_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_138_Desc = "ps_2_sw : dp4 write mask .wz is NOT allowed";
string PS_138 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.wz, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_139_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_139_Desc = "ps_2_sw : dp4 write mask .wy is NOT allowed";
string PS_139 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.wy, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_140_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_140_Desc = "ps_2_sw : dp4 write mask .zyx is NOT allowed";
string PS_140 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.zyx, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_141_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_141_Desc = "ps_2_sw : dp4 write mask .wzy is NOT allowed";
string PS_141 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.wzy, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_142_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_142_Desc = "ps_2_sw : dp4 write mask .wzx is NOT allowed";
string PS_142 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.wzx, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_143_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_143_Desc = "ps_2_sw : dp4 write mask .wyx is NOT allowed";
string PS_143 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.wyx, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_144_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_144_Desc = "ps_2_sw : dp4 write mask .yxw is NOT allowed";
string PS_144 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.yxw, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_145_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_145_Desc = "ps_2_sw : dp4 write mask .wzyx is NOT allowed";
string PS_145 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.wzyx, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_146_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_146_Desc = "ps_2_sw : dp4 write mask .zxwy is NOT allowed";
string PS_146 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0.zxwy, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_147_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_147_Desc = "ps_2_sw : dp4 is allowed in a 1 level if b0 block";
string PS_147 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	dp4 r0, v0, v1 "
	"endif ";

PS_CRITERIA PS_148_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_148_Desc = "ps_2_sw : dp4 is allowed in a 8 level if b0 block";
string PS_148 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								dp4 r0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_149_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_149_Desc = "ps_2_sw : dp4 is allowed in a 16 level if b0 block";
string PS_149 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																dp4 r0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_150_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_150_Desc = "ps_2_sw : dp4 is allowed in a 24 level if b0 block";
string PS_150 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								dp4 r0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_151_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_151_Desc = "ps_2_sw : dp4 is NOT allowed in a 25 level if b0 block";
string PS_151 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								if b0 "
	"																									dp4 r0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_152_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_152_Desc = "ps_2_sw : dp4 is allowed in a 1 level if b0 nop else block";
string PS_152 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	dp4 r0, v0, v1 "
	"endif ";

PS_CRITERIA PS_153_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_153_Desc = "ps_2_sw : dp4 is allowed in a 8 level if b0 nop else block";
string PS_153 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								dp4 r0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_154_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_154_Desc = "ps_2_sw : dp4 is allowed in a 16 level if b0 nop else block";
string PS_154 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																dp4 r0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_155_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_155_Desc = "ps_2_sw : dp4 is allowed in a 24 level if b0 nop else block";
string PS_155 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								dp4 r0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_156_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_156_Desc = "ps_2_sw : dp4 is NOT allowed in a 25 level if b0 nop else block";
string PS_156 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								if b0 nop else "
	"																									dp4 r0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_157_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_157_Desc = "ps_2_sw : dp4 is NOT allowed in a 1 level loop aL, i0 block";
string PS_157 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	dp4 r0, v0, v1 "
	"endloop ";

PS_CRITERIA PS_158_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_158_Desc = "ps_2_sw : dp4 is NOT allowed in a 2 level loop aL, i0 block";
string PS_158 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_159_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_159_Desc = "ps_2_sw : dp4 is NOT allowed in a 3 level loop aL, i0 block";
string PS_159 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			dp4 r0, v0, v1 "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_160_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_160_Desc = "ps_2_sw : dp4 is NOT allowed in a 4 level loop aL, i0 block";
string PS_160 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				dp4 r0, v0, v1 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_161_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_161_Desc = "ps_2_sw : dp4 is NOT allowed in a 5 level loop aL, i0 block";
string PS_161 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					dp4 r0, v0, v1 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_162_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_162_Desc = "ps_2_sw : dp4 is allowed in a 1 level rep i0 block";
string PS_162 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	dp4 r0, v0, v1 "
	"endrep ";

PS_CRITERIA PS_163_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_163_Desc = "ps_2_sw : dp4 is allowed in a 2 level rep i0 block";
string PS_163 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_164_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_164_Desc = "ps_2_sw : dp4 is allowed in a 3 level rep i0 block";
string PS_164 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			dp4 r0, v0, v1 "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_165_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_165_Desc = "ps_2_sw : dp4 is allowed in a 4 level rep i0 block";
string PS_165 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				dp4 r0, v0, v1 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_166_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_166_Desc = "ps_2_sw : dp4 is NOT allowed in a 5 level rep i0 block";
string PS_166 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					dp4 r0, v0, v1 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_167_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_167_Desc = "ps_2_sw : dp4 is allowed in a 1 level if_gt c0.x, c0.y block";
string PS_167 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_gt c0.x, c0.y "
	"	dp4 r0, v0, v1 "
	"endif ";

PS_CRITERIA PS_168_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_168_Desc = "ps_2_sw : dp4 is allowed in a 8 level if_gt c0.x, c0.y block";
string PS_168 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								dp4 r0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_169_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_169_Desc = "ps_2_sw : dp4 is allowed in a 16 level if_gt c0.x, c0.y block";
string PS_169 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								if_gt c0.x, c0.y "
	"									if_gt c0.x, c0.y "
	"										if_gt c0.x, c0.y "
	"											if_gt c0.x, c0.y "
	"												if_gt c0.x, c0.y "
	"													if_gt c0.x, c0.y "
	"														if_gt c0.x, c0.y "
	"															if_gt c0.x, c0.y "
	"																dp4 r0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_170_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_170_Desc = "ps_2_sw : dp4 is allowed in a 24 level if_gt c0.x, c0.y block";
string PS_170 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								if_gt c0.x, c0.y "
	"									if_gt c0.x, c0.y "
	"										if_gt c0.x, c0.y "
	"											if_gt c0.x, c0.y "
	"												if_gt c0.x, c0.y "
	"													if_gt c0.x, c0.y "
	"														if_gt c0.x, c0.y "
	"															if_gt c0.x, c0.y "
	"																if_gt c0.x, c0.y "
	"																	if_gt c0.x, c0.y "
	"																		if_gt c0.x, c0.y "
	"																			if_gt c0.x, c0.y "
	"																				if_gt c0.x, c0.y "
	"																					if_gt c0.x, c0.y "
	"																						if_gt c0.x, c0.y "
	"																							if_gt c0.x, c0.y "
	"																								dp4 r0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_171_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_171_Desc = "ps_2_sw : dp4 is NOT allowed in a 25 level if_gt c0.x, c0.y block";
string PS_171 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								if_gt c0.x, c0.y "
	"									if_gt c0.x, c0.y "
	"										if_gt c0.x, c0.y "
	"											if_gt c0.x, c0.y "
	"												if_gt c0.x, c0.y "
	"													if_gt c0.x, c0.y "
	"														if_gt c0.x, c0.y "
	"															if_gt c0.x, c0.y "
	"																if_gt c0.x, c0.y "
	"																	if_gt c0.x, c0.y "
	"																		if_gt c0.x, c0.y "
	"																			if_gt c0.x, c0.y "
	"																				if_gt c0.x, c0.y "
	"																					if_gt c0.x, c0.y "
	"																						if_gt c0.x, c0.y "
	"																							if_gt c0.x, c0.y "
	"																								if_gt c0.x, c0.y "
	"																									dp4 r0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_172_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_172_Desc = "ps_2_sw : dp4 is allowed in a 1 level if_gt c0.x, c0.y nop else block";
string PS_172 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_gt c0.x, c0.y nop else "
	"	dp4 r0, v0, v1 "
	"endif ";

PS_CRITERIA PS_173_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_173_Desc = "ps_2_sw : dp4 is allowed in a 8 level if_gt c0.x, c0.y nop else block";
string PS_173 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		if_gt c0.x, c0.y nop else "
	"			if_gt c0.x, c0.y nop else "
	"				if_gt c0.x, c0.y nop else "
	"					if_gt c0.x, c0.y nop else "
	"						if_gt c0.x, c0.y nop else "
	"							if_gt c0.x, c0.y nop else "
	"								dp4 r0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_174_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_174_Desc = "ps_2_sw : dp4 is allowed in a 16 level if_gt c0.x, c0.y nop else block";
string PS_174 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		if_gt c0.x, c0.y nop else "
	"			if_gt c0.x, c0.y nop else "
	"				if_gt c0.x, c0.y nop else "
	"					if_gt c0.x, c0.y nop else "
	"						if_gt c0.x, c0.y nop else "
	"							if_gt c0.x, c0.y nop else "
	"								if_gt c0.x, c0.y nop else "
	"									if_gt c0.x, c0.y nop else "
	"										if_gt c0.x, c0.y nop else "
	"											if_gt c0.x, c0.y nop else "
	"												if_gt c0.x, c0.y nop else "
	"													if_gt c0.x, c0.y nop else "
	"														if_gt c0.x, c0.y nop else "
	"															if_gt c0.x, c0.y nop else "
	"																dp4 r0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_175_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_175_Desc = "ps_2_sw : dp4 is allowed in a 24 level if_gt c0.x, c0.y nop else block";
string PS_175 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		if_gt c0.x, c0.y nop else "
	"			if_gt c0.x, c0.y nop else "
	"				if_gt c0.x, c0.y nop else "
	"					if_gt c0.x, c0.y nop else "
	"						if_gt c0.x, c0.y nop else "
	"							if_gt c0.x, c0.y nop else "
	"								if_gt c0.x, c0.y nop else "
	"									if_gt c0.x, c0.y nop else "
	"										if_gt c0.x, c0.y nop else "
	"											if_gt c0.x, c0.y nop else "
	"												if_gt c0.x, c0.y nop else "
	"													if_gt c0.x, c0.y nop else "
	"														if_gt c0.x, c0.y nop else "
	"															if_gt c0.x, c0.y nop else "
	"																if_gt c0.x, c0.y nop else "
	"																	if_gt c0.x, c0.y nop else "
	"																		if_gt c0.x, c0.y nop else "
	"																			if_gt c0.x, c0.y nop else "
	"																				if_gt c0.x, c0.y nop else "
	"																					if_gt c0.x, c0.y nop else "
	"																						if_gt c0.x, c0.y nop else "
	"																							if_gt c0.x, c0.y nop else "
	"																								dp4 r0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_176_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_176_Desc = "ps_2_sw : dp4 is NOT allowed in a 25 level if_gt c0.x, c0.y nop else block";
string PS_176 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		if_gt c0.x, c0.y nop else "
	"			if_gt c0.x, c0.y nop else "
	"				if_gt c0.x, c0.y nop else "
	"					if_gt c0.x, c0.y nop else "
	"						if_gt c0.x, c0.y nop else "
	"							if_gt c0.x, c0.y nop else "
	"								if_gt c0.x, c0.y nop else "
	"									if_gt c0.x, c0.y nop else "
	"										if_gt c0.x, c0.y nop else "
	"											if_gt c0.x, c0.y nop else "
	"												if_gt c0.x, c0.y nop else "
	"													if_gt c0.x, c0.y nop else "
	"														if_gt c0.x, c0.y nop else "
	"															if_gt c0.x, c0.y nop else "
	"																if_gt c0.x, c0.y nop else "
	"																	if_gt c0.x, c0.y nop else "
	"																		if_gt c0.x, c0.y nop else "
	"																			if_gt c0.x, c0.y nop else "
	"																				if_gt c0.x, c0.y nop else "
	"																					if_gt c0.x, c0.y nop else "
	"																						if_gt c0.x, c0.y nop else "
	"																							if_gt c0.x, c0.y nop else "
	"																								if_gt c0.x, c0.y nop else "
	"																									dp4 r0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_177_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_177_Desc = "ps_2_sw : dp4 is allowed in a 1 level if p0.x block";
string PS_177 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	dp4 r0, v0, v1 "
	"endif ";

PS_CRITERIA PS_178_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_178_Desc = "ps_2_sw : dp4 is allowed in a 8 level if p0.x block";
string PS_178 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								dp4 r0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_179_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_179_Desc = "ps_2_sw : dp4 is allowed in a 16 level if p0.x block";
string PS_179 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																dp4 r0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_180_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_180_Desc = "ps_2_sw : dp4 is allowed in a 24 level if p0.x block";
string PS_180 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								dp4 r0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_181_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_181_Desc = "ps_2_sw : dp4 is NOT allowed in a 25 level if p0.x block";
string PS_181 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								if p0.x "
	"																									dp4 r0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_182_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_182_Desc = "ps_2_sw : dp4 is allowed in a 1 level if p0.x nop else block";
string PS_182 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	dp4 r0, v0, v1 "
	"endif ";

PS_CRITERIA PS_183_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_183_Desc = "ps_2_sw : dp4 is allowed in a 8 level if p0.x nop else block";
string PS_183 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								dp4 r0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_184_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_184_Desc = "ps_2_sw : dp4 is allowed in a 16 level if p0.x nop else block";
string PS_184 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																dp4 r0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_185_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_185_Desc = "ps_2_sw : dp4 is allowed in a 24 level if p0.x nop else block";
string PS_185 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								dp4 r0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_186_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_186_Desc = "ps_2_sw : dp4 is NOT allowed in a 25 level if p0.x nop else block";
string PS_186 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								if p0.x nop else "
	"																									dp4 r0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_187_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_187_Desc = "ps_2_sw : dp4 is NOT allowed in a 1 level loop aL, i0 break block";
string PS_187 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	dp4 r0, v0, v1 "
	"endloop ";

PS_CRITERIA PS_188_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_188_Desc = "ps_2_sw : dp4 is NOT allowed in a 8 level loop aL, i0 break block";
string PS_188 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								dp4 r0, v0, v1 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_189_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_189_Desc = "ps_2_sw : dp4 is NOT allowed in a 16 level loop aL, i0 break block";
string PS_189 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																dp4 r0, v0, v1 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_190_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_190_Desc = "ps_2_sw : dp4 is NOT allowed in a 24 level loop aL, i0 break block";
string PS_190 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								dp4 r0, v0, v1 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_191_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_191_Desc = "ps_2_sw : dp4 is NOT allowed in a 25 level loop aL, i0 break block";
string PS_191 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								loop aL, i0 break "
	"																									dp4 r0, v0, v1 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_192_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_192_Desc = "ps_2_sw : dp4 is allowed in a 1 level rep i0 break block";
string PS_192 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	dp4 r0, v0, v1 "
	"endrep ";

PS_CRITERIA PS_193_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_193_Desc = "ps_2_sw : dp4 is NOT allowed in a 8 level rep i0 break block";
string PS_193 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								dp4 r0, v0, v1 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_194_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_194_Desc = "ps_2_sw : dp4 is NOT allowed in a 16 level rep i0 break block";
string PS_194 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																dp4 r0, v0, v1 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_195_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_195_Desc = "ps_2_sw : dp4 is NOT allowed in a 24 level rep i0 break block";
string PS_195 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								dp4 r0, v0, v1 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_196_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_196_Desc = "ps_2_sw : dp4 is NOT allowed in a 25 level rep i0 break block";
string PS_196 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								rep i0 break "
	"																									dp4 r0, v0, v1 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_197_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_197_Desc = "ps_2_sw : dp4 is NOT allowed in a 1 level loop aL, i0 breakp p0.x block";
string PS_197 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	dp4 r0, v0, v1 "
	"endloop ";

PS_CRITERIA PS_198_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_198_Desc = "ps_2_sw : dp4 is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string PS_198 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								dp4 r0, v0, v1 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_199_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_199_Desc = "ps_2_sw : dp4 is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string PS_199 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																dp4 r0, v0, v1 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_200_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_200_Desc = "ps_2_sw : dp4 is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string PS_200 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								dp4 r0, v0, v1 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_201_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_201_Desc = "ps_2_sw : dp4 is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string PS_201 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								loop aL, i0 breakp p0.x "
	"																									dp4 r0, v0, v1 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_202_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_202_Desc = "ps_2_sw : dp4 is allowed in a 1 level rep i0 breakp p0.x block";
string PS_202 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	dp4 r0, v0, v1 "
	"endrep ";

PS_CRITERIA PS_203_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_203_Desc = "ps_2_sw : dp4 is NOT allowed in a 8 level rep i0 breakp p0.x block";
string PS_203 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								dp4 r0, v0, v1 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_204_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_204_Desc = "ps_2_sw : dp4 is NOT allowed in a 16 level rep i0 breakp p0.x block";
string PS_204 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																dp4 r0, v0, v1 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_205_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_205_Desc = "ps_2_sw : dp4 is NOT allowed in a 24 level rep i0 breakp p0.x block";
string PS_205 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								dp4 r0, v0, v1 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_206_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_206_Desc = "ps_2_sw : dp4 is NOT allowed in a 25 level rep i0 breakp p0.x block";
string PS_206 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								rep i0 breakp p0.x "
	"																									dp4 r0, v0, v1 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_207_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_207_Desc = "ps_2_sw : dp4 is NOT allowed in a 1 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_207 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	dp4 r0, v0, v1 "
	"endloop ";

PS_CRITERIA PS_208_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_208_Desc = "ps_2_sw : dp4 is NOT allowed in a 8 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_208 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								dp4 r0, v0, v1 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_209_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_209_Desc = "ps_2_sw : dp4 is NOT allowed in a 16 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_209 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								loop aL, i0 break_ge c0.x, c0.y "
	"									loop aL, i0 break_ge c0.x, c0.y "
	"										loop aL, i0 break_ge c0.x, c0.y "
	"											loop aL, i0 break_ge c0.x, c0.y "
	"												loop aL, i0 break_ge c0.x, c0.y "
	"													loop aL, i0 break_ge c0.x, c0.y "
	"														loop aL, i0 break_ge c0.x, c0.y "
	"															loop aL, i0 break_ge c0.x, c0.y "
	"																dp4 r0, v0, v1 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_210_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_210_Desc = "ps_2_sw : dp4 is NOT allowed in a 24 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_210 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								loop aL, i0 break_ge c0.x, c0.y "
	"									loop aL, i0 break_ge c0.x, c0.y "
	"										loop aL, i0 break_ge c0.x, c0.y "
	"											loop aL, i0 break_ge c0.x, c0.y "
	"												loop aL, i0 break_ge c0.x, c0.y "
	"													loop aL, i0 break_ge c0.x, c0.y "
	"														loop aL, i0 break_ge c0.x, c0.y "
	"															loop aL, i0 break_ge c0.x, c0.y "
	"																loop aL, i0 break_ge c0.x, c0.y "
	"																	loop aL, i0 break_ge c0.x, c0.y "
	"																		loop aL, i0 break_ge c0.x, c0.y "
	"																			loop aL, i0 break_ge c0.x, c0.y "
	"																				loop aL, i0 break_ge c0.x, c0.y "
	"																					loop aL, i0 break_ge c0.x, c0.y "
	"																						loop aL, i0 break_ge c0.x, c0.y "
	"																							loop aL, i0 break_ge c0.x, c0.y "
	"																								dp4 r0, v0, v1 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_211_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_211_Desc = "ps_2_sw : dp4 is NOT allowed in a 25 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_211 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								loop aL, i0 break_ge c0.x, c0.y "
	"									loop aL, i0 break_ge c0.x, c0.y "
	"										loop aL, i0 break_ge c0.x, c0.y "
	"											loop aL, i0 break_ge c0.x, c0.y "
	"												loop aL, i0 break_ge c0.x, c0.y "
	"													loop aL, i0 break_ge c0.x, c0.y "
	"														loop aL, i0 break_ge c0.x, c0.y "
	"															loop aL, i0 break_ge c0.x, c0.y "
	"																loop aL, i0 break_ge c0.x, c0.y "
	"																	loop aL, i0 break_ge c0.x, c0.y "
	"																		loop aL, i0 break_ge c0.x, c0.y "
	"																			loop aL, i0 break_ge c0.x, c0.y "
	"																				loop aL, i0 break_ge c0.x, c0.y "
	"																					loop aL, i0 break_ge c0.x, c0.y "
	"																						loop aL, i0 break_ge c0.x, c0.y "
	"																							loop aL, i0 break_ge c0.x, c0.y "
	"																								loop aL, i0 break_ge c0.x, c0.y "
	"																									dp4 r0, v0, v1 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_212_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_212_Desc = "ps_2_sw : dp4 is allowed in a 1 level rep i0 break_ge c0.x, c0.y block";
string PS_212 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	dp4 r0, v0, v1 "
	"endrep ";

PS_CRITERIA PS_213_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_213_Desc = "ps_2_sw : dp4 is NOT allowed in a 8 level rep i0 break_ge c0.x, c0.y block";
string PS_213 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								dp4 r0, v0, v1 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_214_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_214_Desc = "ps_2_sw : dp4 is NOT allowed in a 16 level rep i0 break_ge c0.x, c0.y block";
string PS_214 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								rep i0 break_ge c0.x, c0.y "
	"									rep i0 break_ge c0.x, c0.y "
	"										rep i0 break_ge c0.x, c0.y "
	"											rep i0 break_ge c0.x, c0.y "
	"												rep i0 break_ge c0.x, c0.y "
	"													rep i0 break_ge c0.x, c0.y "
	"														rep i0 break_ge c0.x, c0.y "
	"															rep i0 break_ge c0.x, c0.y "
	"																dp4 r0, v0, v1 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_215_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_215_Desc = "ps_2_sw : dp4 is NOT allowed in a 24 level rep i0 break_ge c0.x, c0.y block";
string PS_215 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								rep i0 break_ge c0.x, c0.y "
	"									rep i0 break_ge c0.x, c0.y "
	"										rep i0 break_ge c0.x, c0.y "
	"											rep i0 break_ge c0.x, c0.y "
	"												rep i0 break_ge c0.x, c0.y "
	"													rep i0 break_ge c0.x, c0.y "
	"														rep i0 break_ge c0.x, c0.y "
	"															rep i0 break_ge c0.x, c0.y "
	"																rep i0 break_ge c0.x, c0.y "
	"																	rep i0 break_ge c0.x, c0.y "
	"																		rep i0 break_ge c0.x, c0.y "
	"																			rep i0 break_ge c0.x, c0.y "
	"																				rep i0 break_ge c0.x, c0.y "
	"																					rep i0 break_ge c0.x, c0.y "
	"																						rep i0 break_ge c0.x, c0.y "
	"																							rep i0 break_ge c0.x, c0.y "
	"																								dp4 r0, v0, v1 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_216_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_216_Desc = "ps_2_sw : dp4 is NOT allowed in a 25 level rep i0 break_ge c0.x, c0.y block";
string PS_216 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								rep i0 break_ge c0.x, c0.y "
	"									rep i0 break_ge c0.x, c0.y "
	"										rep i0 break_ge c0.x, c0.y "
	"											rep i0 break_ge c0.x, c0.y "
	"												rep i0 break_ge c0.x, c0.y "
	"													rep i0 break_ge c0.x, c0.y "
	"														rep i0 break_ge c0.x, c0.y "
	"															rep i0 break_ge c0.x, c0.y "
	"																rep i0 break_ge c0.x, c0.y "
	"																	rep i0 break_ge c0.x, c0.y "
	"																		rep i0 break_ge c0.x, c0.y "
	"																			rep i0 break_ge c0.x, c0.y "
	"																				rep i0 break_ge c0.x, c0.y "
	"																					rep i0 break_ge c0.x, c0.y "
	"																						rep i0 break_ge c0.x, c0.y "
	"																							rep i0 break_ge c0.x, c0.y "
	"																								rep i0 break_ge c0.x, c0.y "
	"																									dp4 r0, v0, v1 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_217_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_217_Desc = "ps_2_sw : dp4 is allowed in a if b0 and if b0 block";
string PS_217 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_218_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_218_Desc = "ps_2_sw : dp4 is allowed in a if b0 and if b0 nop else block";
string PS_218 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_219_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_219_Desc = "ps_2_sw : dp4 is NOT allowed in a if b0 and loop aL, i0 block";
string PS_219 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_220_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_220_Desc = "ps_2_sw : dp4 is allowed in a if b0 and rep i0 block";
string PS_220 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_221_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_221_Desc = "ps_2_sw : dp4 is allowed in a if b0 and if_ge c0.x, c0.y block";
string PS_221 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	if_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_222_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_222_Desc = "ps_2_sw : dp4 is allowed in a if b0 and if_ge c0.x, c0.y nop else block";
string PS_222 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	if_ge c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_223_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_223_Desc = "ps_2_sw : dp4 is allowed in a if b0 and if p0.x block";
string PS_223 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_224_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_224_Desc = "ps_2_sw : dp4 is allowed in a if b0 and if p0.x nop else block";
string PS_224 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_225_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_225_Desc = "ps_2_sw : dp4 is NOT allowed in a if b0 and loop aL, i0 break block";
string PS_225 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_226_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_226_Desc = "ps_2_sw : dp4 is allowed in a if b0 and rep i0 break block";
string PS_226 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_227_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_227_Desc = "ps_2_sw : dp4 is NOT allowed in a if b0 and loop aL, i0 breakp p0.x block";
string PS_227 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_228_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_228_Desc = "ps_2_sw : dp4 is allowed in a if b0 and rep i0 breakp p0.x block";
string PS_228 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_229_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_229_Desc = "ps_2_sw : dp4 is NOT allowed in a if b0 and loop aL, i0 break_ge c0.x, c0.y block";
string PS_229 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_230_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_230_Desc = "ps_2_sw : dp4 is allowed in a if b0 and rep i0 break_ge c0.x, c0.y block";
string PS_230 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_231_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_231_Desc = "ps_2_sw : dp4 is allowed in a if b0 nop else and if b0 block";
string PS_231 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_232_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_232_Desc = "ps_2_sw : dp4 is allowed in a if b0 nop else and if b0 nop else block";
string PS_232 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_233_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_233_Desc = "ps_2_sw : dp4 is NOT allowed in a if b0 nop else and loop aL, i0 block";
string PS_233 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_234_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_234_Desc = "ps_2_sw : dp4 is allowed in a if b0 nop else and rep i0 block";
string PS_234 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_235_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_235_Desc = "ps_2_sw : dp4 is allowed in a if b0 nop else and if_ge c0.x, c0.y block";
string PS_235 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	if_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_236_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_236_Desc = "ps_2_sw : dp4 is allowed in a if b0 nop else and if_ge c0.x, c0.y nop else block";
string PS_236 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	if_ge c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_237_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_237_Desc = "ps_2_sw : dp4 is allowed in a if b0 nop else and if p0.x block";
string PS_237 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_238_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_238_Desc = "ps_2_sw : dp4 is allowed in a if b0 nop else and if p0.x nop else block";
string PS_238 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_239_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_239_Desc = "ps_2_sw : dp4 is NOT allowed in a if b0 nop else and loop aL, i0 break block";
string PS_239 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_240_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_240_Desc = "ps_2_sw : dp4 is allowed in a if b0 nop else and rep i0 break block";
string PS_240 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_241_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_241_Desc = "ps_2_sw : dp4 is NOT allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string PS_241 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_242_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_242_Desc = "ps_2_sw : dp4 is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string PS_242 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_243_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_243_Desc = "ps_2_sw : dp4 is NOT allowed in a if b0 nop else and loop aL, i0 break_ge c0.x, c0.y block";
string PS_243 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_244_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_244_Desc = "ps_2_sw : dp4 is allowed in a if b0 nop else and rep i0 break_ge c0.x, c0.y block";
string PS_244 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if b0 nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_245_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_245_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and if b0 block";
string PS_245 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_246_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_246_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and if b0 nop else block";
string PS_246 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_247_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_247_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and loop aL, i0 block";
string PS_247 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_248_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_248_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and rep i0 block";
string PS_248 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_249_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_249_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and if_lt c0.x, c0.y block";
string PS_249 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_250_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_250_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and if_lt c0.x, c0.y nop else block";
string PS_250 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_251_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_251_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and if p0.x block";
string PS_251 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_252_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_252_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and if p0.x nop else block";
string PS_252 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_253_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_253_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and loop aL, i0 break block";
string PS_253 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_254_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_254_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and rep i0 break block";
string PS_254 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_255_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_255_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string PS_255 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_256_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_256_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string PS_256 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_257_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_257_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and loop aL, i0 break_ge c0.x, c0.y block";
string PS_257 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_258_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_258_Desc = "ps_2_sw : dp4 is NOT allowed in a loop aL, i0 and rep i0 break_ge c0.x, c0.y block";
string PS_258 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_259_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_259_Desc = "ps_2_sw : dp4 is allowed in a rep i0 and if b0 block";
string PS_259 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_260_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_260_Desc = "ps_2_sw : dp4 is allowed in a rep i0 and if b0 nop else block";
string PS_260 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_261_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_261_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 and loop aL, i0 block";
string PS_261 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_262_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_262_Desc = "ps_2_sw : dp4 is allowed in a rep i0 and rep i0 block";
string PS_262 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_263_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_263_Desc = "ps_2_sw : dp4 is allowed in a rep i0 and if_le c0.x, c0.y block";
string PS_263 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	if_le c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_264_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_264_Desc = "ps_2_sw : dp4 is allowed in a rep i0 and if_le c0.x, c0.y nop else block";
string PS_264 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	if_le c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_265_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_265_Desc = "ps_2_sw : dp4 is allowed in a rep i0 and if p0.x block";
string PS_265 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_266_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_266_Desc = "ps_2_sw : dp4 is allowed in a rep i0 and if p0.x nop else block";
string PS_266 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_267_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_267_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 and loop aL, i0 break block";
string PS_267 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_268_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_268_Desc = "ps_2_sw : dp4 is allowed in a rep i0 and rep i0 break block";
string PS_268 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_269_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_269_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string PS_269 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_270_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_270_Desc = "ps_2_sw : dp4 is allowed in a rep i0 and rep i0 breakp p0.x block";
string PS_270 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_271_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_271_Desc = "ps_2_sw : dp4 is NOT allowed in a rep i0 and loop aL, i0 break_ge c0.x, c0.y block";
string PS_271 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_272_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_272_Desc = "ps_2_sw : dp4 is allowed in a rep i0 and rep i0 break_ge c0.x, c0.y block";
string PS_272 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_273_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_273_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y and if b0 block";
string PS_273 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_274_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_274_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y and if b0 nop else block";
string PS_274 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_275_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_275_Desc = "ps_2_sw : dp4 is NOT allowed in a if_le c0.x, c0.y and loop aL, i0 block";
string PS_275 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_276_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_276_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y and rep i0 block";
string PS_276 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_277_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_277_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y and if_le c0.x, c0.y block";
string PS_277 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	if_le c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_278_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_278_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y and if_le c0.x, c0.y nop else block";
string PS_278 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	if_le c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_279_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_279_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y and if p0.x block";
string PS_279 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_280_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_280_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y and if p0.x nop else block";
string PS_280 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_281_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_281_Desc = "ps_2_sw : dp4 is NOT allowed in a if_le c0.x, c0.y and loop aL, i0 break block";
string PS_281 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_282_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_282_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y and rep i0 break block";
string PS_282 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_283_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_283_Desc = "ps_2_sw : dp4 is NOT allowed in a if_le c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_283 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_284_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_284_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y and rep i0 breakp p0.x block";
string PS_284 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_285_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_285_Desc = "ps_2_sw : dp4 is NOT allowed in a if_le c0.x, c0.y and loop aL, i0 break_ge c0.x, c0.y block";
string PS_285 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_286_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_286_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y and rep i0 break_ge c0.x, c0.y block";
string PS_286 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_287_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_287_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y nop else and if b0 block";
string PS_287 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y nop else "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_288_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_288_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y nop else and if b0 nop else block";
string PS_288 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y nop else "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_289_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_289_Desc = "ps_2_sw : dp4 is NOT allowed in a if_le c0.x, c0.y nop else and loop aL, i0 block";
string PS_289 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_290_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_290_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y nop else and rep i0 block";
string PS_290 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y nop else "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_291_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_291_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y nop else and if_le c0.x, c0.y block";
string PS_291 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y nop else "
	"	if_le c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_292_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_292_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y nop else and if_le c0.x, c0.y nop else block";
string PS_292 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y nop else "
	"	if_le c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_293_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_293_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y nop else and if p0.x block";
string PS_293 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y nop else "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_294_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_294_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y nop else and if p0.x nop else block";
string PS_294 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_295_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_295_Desc = "ps_2_sw : dp4 is NOT allowed in a if_le c0.x, c0.y nop else and loop aL, i0 break block";
string PS_295 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_296_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_296_Desc = "ps_2_sw : dp4 is allowed in a if_le c0.x, c0.y nop else and rep i0 break block";
string PS_296 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y nop else "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_297_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_297_Desc = "ps_2_sw : dp4 is NOT allowed in a if_le c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string PS_297 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_le c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_298_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_298_Desc = "ps_2_sw : dp4 is allowed in a if_eq c0.x, c0.y nop else and rep i0 breakp p0.x block";
string PS_298 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_eq c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_299_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_299_Desc = "ps_2_sw : dp4 is NOT allowed in a if_eq c0.x, c0.y nop else and loop aL, i0 break_ge c0.x, c0.y block";
string PS_299 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_eq c0.x, c0.y nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_300_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_300_Desc = "ps_2_sw : dp4 is allowed in a if_eq c0.x, c0.y nop else and rep i0 break_ge c0.x, c0.y block";
string PS_300 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if_eq c0.x, c0.y nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_301_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_301_Desc = "ps_2_sw : dp4 is allowed in a if p0.x and if b0 block";
string PS_301 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_302_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_302_Desc = "ps_2_sw : dp4 is allowed in a if p0.x and if b0 nop else block";
string PS_302 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_303_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_303_Desc = "ps_2_sw : dp4 is NOT allowed in a if p0.x and loop aL, i0 block";
string PS_303 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_304_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_304_Desc = "ps_2_sw : dp4 is allowed in a if p0.x and rep i0 block";
string PS_304 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_305_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_305_Desc = "ps_2_sw : dp4 is allowed in a if p0.x and if_eq c0.x, c0.y block";
string PS_305 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	if_eq c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_306_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_306_Desc = "ps_2_sw : dp4 is allowed in a if p0.x and if_eq c0.x, c0.y nop else block";
string PS_306 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	if_eq c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_307_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_307_Desc = "ps_2_sw : dp4 is allowed in a if p0.x and if p0.x block";
string PS_307 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_308_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_308_Desc = "ps_2_sw : dp4 is allowed in a if p0.x and if p0.x nop else block";
string PS_308 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_309_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_309_Desc = "ps_2_sw : dp4 is NOT allowed in a if p0.x and loop aL, i0 break block";
string PS_309 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_310_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_310_Desc = "ps_2_sw : dp4 is allowed in a if p0.x and rep i0 break block";
string PS_310 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_311_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_311_Desc = "ps_2_sw : dp4 is NOT allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string PS_311 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_312_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_312_Desc = "ps_2_sw : dp4 is allowed in a if p0.x and rep i0 breakp p0.x block";
string PS_312 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_313_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_313_Desc = "ps_2_sw : dp4 is NOT allowed in a if p0.x and loop aL, i0 break_ge c0.x, c0.y block";
string PS_313 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_314_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_314_Desc = "ps_2_sw : dp4 is allowed in a if p0.x and rep i0 break_ge c0.x, c0.y block";
string PS_314 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x "
	"	rep i0 break_ge c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_315_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_315_Desc = "ps_2_sw : dp4 is allowed in a if p0.x nop else and if b0 block";
string PS_315 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	if b0 "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_316_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_316_Desc = "ps_2_sw : dp4 is allowed in a if p0.x nop else and if b0 nop else block";
string PS_316 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_317_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_317_Desc = "ps_2_sw : dp4 is NOT allowed in a if p0.x nop else and loop aL, i0 block";
string PS_317 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_318_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_318_Desc = "ps_2_sw : dp4 is allowed in a if p0.x nop else and rep i0 block";
string PS_318 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	rep i0 "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_319_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_319_Desc = "ps_2_sw : dp4 is allowed in a if p0.x nop else and if_eq c0.x, c0.y block";
string PS_319 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	if_eq c0.x, c0.y "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_320_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_320_Desc = "ps_2_sw : dp4 is allowed in a if p0.x nop else and if_eq c0.x, c0.y nop else block";
string PS_320 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	if_eq c0.x, c0.y nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_321_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_321_Desc = "ps_2_sw : dp4 is allowed in a if p0.x nop else and if p0.x block";
string PS_321 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	if p0.x "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_322_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_322_Desc = "ps_2_sw : dp4 is allowed in a if p0.x nop else and if p0.x nop else block";
string PS_322 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		dp4 r0, v0, v1 "
	"	endif "
	"endif ";

PS_CRITERIA PS_323_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_323_Desc = "ps_2_sw : dp4 is NOT allowed in a if p0.x nop else and loop aL, i0 break block";
string PS_323 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		dp4 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_324_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_324_Desc = "ps_2_sw : dp4 is allowed in a if p0.x nop else and rep i0 break block";
string PS_324 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	rep i0 break "
	"		dp4 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_325_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_325_Desc = "ps_2_sw : dp4 is NOT allowed in a if p0.x nop else a