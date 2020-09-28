e, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_386_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and loop aL, i0 block";
string PS_386 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_387_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_387_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and rep i0 block";
string PS_387 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_388_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_388_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and if_eq c0.x, c0.y block";
string PS_388 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_389_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_389_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and if_eq c0.x, c0.y nop else block";
string PS_389 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if_eq c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_390_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_390_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and if p0.x block";
string PS_390 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_391_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_391_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_391 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_392_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_392_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_392 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_393_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_393_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and rep i0 break block";
string PS_393 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_394_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_394_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_394 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_395_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_395_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_395 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_396_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_396_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and loop aL, i0 break_eq c0.x, c0.y block";
string PS_396 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_397_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_397_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and rep i0 break_eq c0.x, c0.y block";
string PS_397 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_398_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_398_Desc = "ps_3_0 : sub is allowed in a rep i0 break and if b0 block";
string PS_398 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_399_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_399_Desc = "ps_3_0 : sub is allowed in a rep i0 break and if b0 nop else block";
string PS_399 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_400_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_400_Desc = "ps_3_0 : sub is allowed in a rep i0 break and loop aL, i0 block";
string PS_400 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_401_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_401_Desc = "ps_3_0 : sub is allowed in a rep i0 break and rep i0 block";
string PS_401 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_402_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_402_Desc = "ps_3_0 : sub is allowed in a rep i0 break and if_eq c0.x, c0.y block";
string PS_402 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_403_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_403_Desc = "ps_3_0 : sub is allowed in a rep i0 break and if_eq c0.x, c0.y nop else block";
string PS_403 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if_eq c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_404_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_404_Desc = "ps_3_0 : sub is allowed in a rep i0 break and if p0.x block";
string PS_404 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_405_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_405_Desc = "ps_3_0 : sub is allowed in a rep i0 break and if p0.x nop else block";
string PS_405 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_406_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_406_Desc = "ps_3_0 : sub is allowed in a rep i0 break and loop aL, i0 break block";
string PS_406 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_407_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_407_Desc = "ps_3_0 : sub is allowed in a rep i0 break and rep i0 break block";
string PS_407 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_408_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_408_Desc = "ps_3_0 : sub is allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string PS_408 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_409_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_409_Desc = "ps_3_0 : sub is allowed in a rep i0 break and rep i0 breakp p0.x block";
string PS_409 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_410_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_410_Desc = "ps_3_0 : sub is allowed in a rep i0 break