;

PS_CRITERIA PS_282_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_282_Desc = "ps_3_0 : nrm is allowed in a if p0.x nop else and if p0.x nop else block";
string PS_282 = 
	"ps_3_0 "
	"dcl_depth v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		nrm r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_283_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_283_Desc = "ps_3_0 : nrm is allowed in a if p0.x nop else and loop aL, i0 break block";
string PS_283 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		nrm r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_284_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_284_Desc = "ps_3_0 : nrm is allowed in a if p0.x nop else and rep i0 break block";
string PS_284 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"if p0.x nop else "
	"	rep i0 break "
	"		nrm r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_285_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_285_Desc = "ps_3_0 : nrm is allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string PS_285 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_286_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_286_Desc = "ps_3_0 : nrm is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string PS_286 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_287_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_287_Desc = "ps_3_0 : nrm is allowed in a if p0.x nop else and loop aL, i0 break_lt c0.x, c0.y block";
string PS_287 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"if p0.x nop else "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_288_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_288_Desc = "ps_3_0 : nrm is allowed in a if p0.x nop else and rep i0 break_lt c0.x, c0.y block";
string PS_288 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"if p0.x nop else "
	"	rep i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_289_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_289_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and if b0 block";
string PS_289 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	if b0 "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_290_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_290_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and if b0 nop else block";
string PS_290 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_291_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_291_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and loop aL, i0 block";
string PS_291 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		nrm r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_292_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_292_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and rep i0 block";
string PS_292 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	rep i0 "
	"		nrm r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_293_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_293_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and if_eq c0.x, c0.y block";
string PS_293 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	if_eq c0.x, c0.y "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_294_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_294_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and if_eq c0.x, c0.y nop else block";
string PS_294 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	if_eq c0.x, c0.y nop else "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_295_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_295_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and if p0.x block";
string PS_295 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_296_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_296_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_296 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_297_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_297_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_297 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		nrm r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_298_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_298_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and rep i0 break block";
string PS_298 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		nrm r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_299_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_299_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_299 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_300_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_300_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_300 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_301_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_301_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and loop aL, i0 break_lt c0.x, c0.y block";
string PS_301 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_302_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_302_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break and rep i0 break_lt c0.x, c0.y block";
string PS_302 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 break "
	"	rep i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_303_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_303_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and if b0 block";
string PS_303 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	if b0 "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_304_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_304_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and if b0 nop else block";
string PS_304 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	if b0 nop else "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_305_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_305_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and loop aL, i0 block";
string PS_305 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		nrm r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_306_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_306_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and rep i0 block";
string PS_306 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	rep i0 "
	"		nrm r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_307_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_307_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and if_eq c0.x, c0.y block";
string PS_307 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	if_eq c0.x, c0.y "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_308_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_308_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and if_eq c0.x, c0.y nop else block";
string PS_308 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	if_eq c0.x, c0.y nop else "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_309_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_309_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and if p0.x block";
string PS_309 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	if p0.x "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_310_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_310_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and if p0.x nop else block";
string PS_310 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_311_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_311_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and loop aL, i0 break block";
string PS_311 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		nrm r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_312_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_312_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and rep i0 break block";
string PS_312 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	rep i0 break "
	"		nrm r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_313_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_313_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string PS_313 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_314_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_314_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and rep i0 breakp p0.x block";
string PS_314 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_315_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_315_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and loop aL, i0 break_lt c0.x, c0.y block";
string PS_315 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_316_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_316_Desc = "ps_3_0 : nrm is allowed in a rep i0 break and rep i0 break_lt c0.x, c0.y block";
string PS_316 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break "
	"	rep i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_317_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_317_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and if b0 block";
string PS_317 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_318_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_318_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string PS_318 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_319_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_319_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string PS_319 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		nrm r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_320_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_320_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string PS_320 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		nrm r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_321_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_321_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and if_eq c0.x, c0.y block";
string PS_321 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	if_eq c0.x, c0.y "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_322_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_322_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and if_eq c0.x, c0.y nop else block";
string PS_322 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	if_eq c0.x, c0.y nop else "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_323_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_323_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string PS_323 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_324_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_324_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string PS_324 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_325_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_325_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string PS_325 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		nrm r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_326_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_326_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string PS_326 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		nrm r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_327_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_327_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_327 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_328_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_328_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_328 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_329_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_329_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_lt c0.x, c0.y block";
string PS_329 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_330_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_330_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 breakp p0.x and rep i0 break_lt c0.x, c0.y block";
string PS_330 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_331_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_331_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and if b0 block";
string PS_331 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_332_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_332_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string PS_332 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_333_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_333_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string PS_333 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		nrm r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_334_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_334_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and rep i0 block";
string PS_334 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		nrm r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_335_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_335_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and if_eq c0.x, c0.y block";
string PS_335 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	if_eq c0.x, c0.y "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_336_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_336_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and if_eq c0.x, c0.y nop else block";
string PS_336 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	if_eq c0.x, c0.y nop else "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_337_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_337_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and if p0.x block";
string PS_337 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_338_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_338_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string PS_338 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_339_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_339_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string PS_339 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		nrm r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_340_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_340_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and rep i0 break block";
string PS_340 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		nrm r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_341_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_341_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_341 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_342_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_342_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_342 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_343_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_343_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and loop aL, i0 break_lt c0.x, c0.y block";
string PS_343 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_344_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_344_Desc = "ps_3_0 : nrm is allowed in a rep i0 breakp p0.x and rep i0 break_lt c0.x, c0.y block";
string PS_344 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_345_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_345_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and if b0 block";
string PS_345 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	if b0 "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_346_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_346_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and if b0 nop else block";
string PS_346 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	if b0 nop else "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_347_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_347_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and loop aL, i0 block";
string PS_347 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	loop aL, i0 "
	"		nrm r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_348_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_348_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and rep i0 block";
string PS_348 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	rep i0 "
	"		nrm r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_349_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_349_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and if_eq c0.x, c0.y block";
string PS_349 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_350_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_350_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and if_eq c0.x, c0.y nop else block";
string PS_350 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	if_eq c0.x, c0.y nop else "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_351_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_351_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and if p0.x block";
string PS_351 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	if p0.x "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_352_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_352_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and if p0.x nop else block";
string PS_352 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	if p0.x nop else "
	"		nrm r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_353_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_353_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and loop aL, i0 break block";
string PS_353 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	loop aL, i0 break "
	"		nrm r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_354_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_354_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and rep i0 break block";
string PS_354 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	rep i0 break "
	"		nrm r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_355_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_355_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_355 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_356_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_356_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and rep i0 breakp p0.x block";
string PS_356 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_357_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_357_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and loop aL, i0 break_lt c0.x, c0.y block";
string PS_357 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_358_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_358_Desc = "ps_3_0 : nrm is allowed in a loop aL, i0 break_lt c0.x, c0.y and rep i0 break_lt c0.x, c0.y block";
string PS_358 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_359_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_359_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and if b0 block";
string PS_359 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if b0 "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_360_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_360_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and if b0 nop else block";
string PS_360 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if b0 nop else "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_361_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_361_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 block";
string PS_361 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 "
	"		nrm r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_362_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_362_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 block";
string PS_362 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 "
	"		nrm r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_363_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_363_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and if_eq c0.x, c0.y block";
string PS_363 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_364_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_364_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and if_eq c0.x, c0.y nop else block";
string PS_364 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if_eq c0.x, c0.y nop else "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_365_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_365_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and if p0.x block";
string PS_365 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if p0.x "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_366_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_366_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and if p0.x nop else block";
string PS_366 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if p0.x nop else "
	"		nrm r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_367_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_367_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 break block";
string PS_367 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 break "
	"		nrm r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_368_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_368_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 break block";
string PS_368 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 break "
	"		nrm r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_369_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_369_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_369 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_370_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_370_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 breakp p0.x block";
string PS_370 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		nrm r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_371_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_371_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 break_lt c0.x, c0.y block";
string PS_371 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_372_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_372_Desc = "ps_3_0 : nrm is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 break_lt c0.x, c0.y block";
string PS_372 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		nrm r0, v0 "
	"	endrep "
	"endrep ";


// ======================================================================================
//
//  CD3D9.cpp
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#include "D3DPCH.h"
#include <d3d8rgbrast.h>
#include "Framework.h"
#include "D3DX9.h"
#include "rtl_simple.h"
#include <strsafe.h>

typedef IDirect3D9* (WINAPI *D3DCREATE)(UINT);
typedef HRESULT (WINAPI *D3DCREATEEX)(UINT, IDirect3D9Ex** );
typedef void (WINAPI *FORCEHYBRIDENUMERATION)(UINT);

D3DDEVTYPE ConvertDeviceType(DWORD DeviceType);


// --------------------------------------------------------------------------------------
// Define all known D3D formats

static FMT g_TextureFormatList9[] = 
{
	FMT_A8R8G8B8,
	FMT_X8R8G8B8,
	FMT_R5G6B5,
	FMT_X1R5G5B5,
	FMT_A1R5G5B5,
	FMT_A4R4G4B4,
	FMT_R3G3B2,
	FMT_A8,
	FMT_A8R3G3B2,
	FMT_X4R4G4B4,
	FMT_R8G8B8,
	FMT_A2B10G10R10,
	FMT_A8B8G8R8,
	FMT_X8B8G8R8,
	FMT_G16R16,
	FMT_A2R10G10B10,
	FMT_A2B10G10R10_XR_BIAS,
	FMT_A16B16G16R16,
	FMT_R16F,
	FMT_G16R16F,
	FMT_A16B16G16R16F,
	FMT_R32F,
	FMT_G32R32F,
	FMT_A32B32G32R32F,
	FMT_A1,

	FMT_A8P8,
	FMT_P8,

	FMT_L8,
	FMT_A8L8,
	FMT_A4L4,
	FMT_L16,

	FMT_V8U8,
	FMT_L6V5U5,
	FMT_X8L8V8U8,
	FMT_Q8W8V8U8,
	FMT_V16U16,
	FMT_A2W10V10U10,
	FMT_Q16W16V16U16,
	FMT_CxV8U8,
	
	FMT_UYVY,
	FMT_R8G8_B8G8,
	FMT_YUY2,
	FMT_G8R8_G8B8,
	FMT_DXT1,
	FMT_DXT2,
	FMT_DXT3,
	FMT_DXT4,
	FMT_DXT5,

	FMT_MULTI2_ARGB8
};

static DWORD g_dwTextureFormatListSize9 = sizeof(g_TextureFormatList9) / sizeof(FMT);

static FMT g_ZBufferFormatList9[] = 
{
	FMT_D16,

	FMT_D32,
	FMT_D15S1,
	FMT_D24S8,
	FMT_D24X8,
	FMT_D24FS8,
	FMT_D24X4S4,

	FMT_D16_LOCKABLE,
	FMT_D32F_LOCKABLE
};
static DWORD g_dwZBufferFormatListSize9 = sizeof(g_ZBufferFormatList9) / sizeof(FMT);

MULTISAMPLE_TYPE g_MultiSampleTypeList9[] = 
{
	MULTISAMPLE_NONE,
	MULTISAMPLE_NONMASKABLE,
	MULTISAMPLE_2_SAMPLES,
	MULTISAMPLE_3_SAMPLES,
	MULTISAMPLE_4_SAMPLES,
	MULTISAMPLE_5_SAMPLES,
	MULTISAMPLE_6_SAMPLES,
	MULTISAMPLE_7_SAMPLES,
	MULTISAMPLE_8_SAMPLES,
	MULTISAMPLE_9_SAMPLES,
	MULTISAMPLE_10_SAMPLES,
	MULTISAMPLE_11_SAMPLES,
	MULTISAMPLE_12_SAMPLES,
	MULTISAMPLE_13_SAMPLES,
	MULTISAMPLE_14_SAMPLES,
	MULTISAMPLE_15_SAMPLES,
	MULTISAMPLE_16_SAMPLES
};
DWORD g_dwMultiSampleTypeListSize9 = sizeof(g_MultiSampleTypeList9) / sizeof(MULTISAMPLE_TYPE);

FMT g_DisplayModeFormats9[] = {
	FMT_X1R5G5B5,
	FMT_R5G6B5  ,
	FMT_R8G8B8  ,
	FMT_X8R8G8B8,
	FMT_A2R10G10B10,
	FMT_A2B10G10R10_XR_BIAS
};
static DWORD g_dwDisplayModeFormats9 = sizeof(g_DisplayModeFormats9) / sizeof(FMT);

FMT g_BackBufferFormats9[] = {
	FMT_A2R10G10B10,
	FMT_A2B10G10R10_XR_BIAS,
	FMT_A8R8G8B8,
	FMT_X8R8G8B8,
	FMT_A1R5G5B5,
	FMT_X1R5G5B5,
	FMT_R5G6B5
};
static DWORD g_dwBackBufferFormats9 = sizeof(g_BackBufferFormats9) / sizeof(FMT);

// --------------------------------------------------------------------------------------

HRESULT CD3D::InitD3D9()
{
	HRESULT                     hr = S_OK;
	D3DCREATE                   pProc;
	D3DCREATEEX					pProcEx;
	FORCEHYBRIDENUMERATION      pfnForceHybridEnumeration = nullptr;
	D3DADAPTER_IDENTIFIER9      Adapter;
	D3DDISPLAYMODE              Mode;
	DWORD                       dwNumModes = 0;
	DWORD                       dwAdapters = 0;
	DWORD						dwAdapterModes = 0;

	// Load the SRC D3D9 library
	m_hSrcD3DDLL = LoadLibraryEx( m_sSrcDLL, NULL, 0 );
	if(m_hSrcD3DDLL == NULL)
	{
		DPF(1, _T("CD3D::InitD3D9() - LoadLibrary failed for %s.\n"), m_sSrcDLL);
		return E_FAIL;
	}
	
	// Get the D3D creation entry point
	pProc = (D3DCREATE)GetProcAddress( m_hSrcD3DDLL, "Direct3DCreate9" );
	if (NULL == pProc)
	{
		DPF(1, _T("CD3D::InitD3D9() - Couldn't get Direct3DCreate9 address for %s.\n"), m_sSrcDLL);
		hr = E_FAIL;
		goto Exit;
	}

    // Get the Windows version
//    OSVERSIONINFO windowsVersionInfo;
//    windowsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//    GetVersionEx(&windowsVersionInfo);
    RTL_OSVERSIONINFOW windowsVersionInfo;
    windowsVersionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
    RtlGetVersion(&windowsVersionInfo);

	// Get the D3D9 Force Hybrid GPU Choice entry point
    // Older OS versions don't have this export, so ship this for older OS Versions.
    // Note: Exported by Ordinal, not by name
    if( windowsVersionInfo.dwMajorVersion > 6 || (windowsVersionInfo.dwMajorVersion == 6 && windowsVersionInfo.dwMinorVersion >= 3 && windowsVersionInfo.dwBuildNumber >= 9321) )
    {
	    if( g_pFramework  )
	    {
		    // Direct3D9ForceHybridEnumeration is exported at ordinal 16
		    const WORD forceHybridGPUOrdinal = LOWORD(16);
		    pfnForceHybridEnumeration = (FORCEHYBRIDENUMERATION)GetProcAddress( m_hSrcD3DDLL, reinterpret_cast<LPCSTR>(forceHybridGPUOrdinal) );
		    if( g_pFramework->GetHybridOverride() )
		    {
			    if (NULL == pfnForceHybridEnumeration)
			    {
				    DPF(1, _T("CD3D::InitD3D9() - Couldn't get Direct3D9ForceHybridEnumeration address in %s.\n"), m_sSrcDLL);
				    hr = E_FAIL;
				    goto Exit;
			    }
			    else
			    {
				    DPF(1, _T("CD3D::InitD3D9() - ForceHybridEnumeration (%d).\n"), g_pFramework->GetHybridOverride() );
				    pfnForceHybridEnumeration(g_pFramework->GetHybridOverride());
			    }
		    }
	    }
	    else
	    {
		    DPF(1, _T("CD3D::InitD3D9() - Framework pointer is NULL. Unable to check force hybrid GPU parameter.\n"));
		    hr = E_FAIL;
		    goto Exit;
	    }
    }

	if( FLOAT_EQUAL( GetVersion(), 9.1f ) )
	{
		pProcEx = (D3DCREATEEX)GetProcAddress( m_hSrcD3DDLL, "Direct3DCreate9Ex" );
		if (NULL == pProcEx)
		{
			DPF(1, _T("CD3D::InitD3D9() - Couldn't get Direct3DCreate9Ex address for %s.\n"), m_sSrcDLL);
			hr = E_FAIL;
			goto Exit;
		}

		pProcEx(D3D_SDK_VERSION | (m_bD3DDebugHelper ? 0x80000000 : 0), & m_pSrcD3D9Ex );
	}
	else
	{
		m_pSrcD3D9 = pProc(D3D_SDK_VERSION | (m_bD3DDebugHelper ? 0x80000000 : 0) );
	}
	if (NULL == m_pSrcD3D9)
	{
		DPF(1, _T("CD3D::InitD3D9() - Direct3DCreate9() failed for %s.  Header/Runtime mismatch likely.\n"), m_sSrcDLL);
		hr = E_ABORT;
		goto Exit;
	}

	// If we're using identical SRC / REF, just AddRef().
	if(_tcsicmp(m_sSrcDLL, m_sRefDLL) == 0)
	{
		m_pRefD3D9 = m_pSrcD3D9;
		m_pRefD3D9->AddRef();
	}
	else
	{
		// Load the REF D3D9 library
		m_hRefD3DDLL = LoadLibraryEx( m_sRefDLL, NULL, 0 );
		if(m_hRefD3DDLL == NULL)
		{
			DPF(1, _T("CD3D::InitD3D9() - LoadLibrary failed for %s.\n"), m_sRefDLL);
			return E_FAIL;
		}
		
		// Get the D3D creation entry point
		pProc = (D3DCREATE)GetProcAddress( m_hRefD3DDLL, "Direct3DCreate9" );
		if (NULL == pProc)
		{
			DPF(1, _T("CD3D::InitD3D9() - Couldn't get Direct3DCreate9 address for %s.\n"), m_sRefDLL);
			hr = E_FAIL;
			goto Exit;
		}

		if( FLOAT_EQUAL( GetVersion(), 9.1f ) )
		{
			pProcEx = (D3DCREATEEX)GetProcAddress( m_hRefD3DDLL, "Direct3DCreate9Ex" );
			if (NULL == pProcEx)
			{
				DPF(1, _T("CD3D::InitD3D9() - Couldn't get Direct3DCreate9Ex address for %s.\n"), m_hRefD3DDLL);
				hr = E_FAIL;
				goto Exit;
			}

			pProcEx(D3D_SDK_VERSION, & m_pRefD3D9Ex);
		}
		else
		{
			m_pRefD3D9 = pProc(D3D_SDK_VERSION);
		}
		if (NULL == m_pRefD3D9)
		{
			DPF(1, _T("CD3D::InitD3D9() - Direct3DCreate9() failed for %s.  Header/Runtime mismatch likely.\n"), m_sRefDLL);
			hr = E_ABORT;
			goto Exit;
		}
	}

	// Get the debug info from the registry
	GetDebugInfoFromReg();
	
	// Set DebugSetMute() function ptr
	if( m_bDebug && GetModuleHandle( m_sDebugDLL ) )
	{
		m_hDebugD3DDLL = LoadLibraryEx( m_sDebugDLL, NULL, 0 );
		m_pDebugMute = (LPDEBUGSETMUTE) GetProcAddress( m_hDebugD3DDLL, "DebugSetMute" );
		if( m_pDebugMute == NULL )
		{
			DPF( 1, _T( "CD3D::InitD3D9() - Couldn't get DebugSetMute address for %s.\n" ), m_sDebugDLL );
			hr = E_FAIL;
			goto Exit;
		}
	}

	// Register any SW rasterizers
	if(!RegisterRasterizer9(m_sSWRast, m_sSWRastEntry))
	{
		DPF(1, _T("CD3D::InitD3D9 - Failed to register SW rasterizer.\n"));;
	}

	// Get the number of adapters
	m_nAdapters = m_pSrcD3D9->GetAdapterCount();
	if(0 == m_nAdapters)
	{
		DPF(1, _T("CD3D::InitD3D9() - GetAdapterCount() returned 0.  No display devices available.\n"));
		hr = E_FAIL;
		goto Exit;
	}

	// Allocate the ADAPTER infos
	m_pAdapters = new ADAPTER[m_nAdapters];
	if(m_pAdapters == NULL)
	{
		DPF(1, _T("CD3D::InitD3D9() - Out of memory.\n"));
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	// Zero the memory
	memset(m_pAdapters, 0, m_nAdapters * sizeof(ADAPTER));

	// Enumerate the adapters
	for (dwAdapters = 0; dwAdapters < m_nAdapters; dwAdapters++)
	{
		PADAPTER pAdapter = &m_pAdapters[dwAdapters];

		hr = m_pSrcD3D9->GetAdapterIdentifier(dwAdapters, NULL, &Adapter);
		if (FAILED(hr))
		{
			DPF(1, _T("CD3D::InitD3D9() - GetAdapterIdentifier() failed.  hr=%s(%x)"), HResultToString(hr));
			goto Exit;
		}

		// create the version string.
		{
			TCHAR tcsVer[20] = {0};
			memset( tcsVer, 0, sizeof tcsVer );
		
			StringCchPrintf(tcsVer, ARRAYSIZE(tcsVer) - 1, _T("%d.%d.%d.%d"),
				HIWORD(Adapter.DriverVersion.HighPart), // product
				LOWORD(Adapter.DriverVersion.HighPart), // version
				HIWORD(Adapter.DriverVersion.LowPart),  // sub-version
				LOWORD(Adapter.DriverVersion.LowPart)); // build
		
			COPYSTRING(pAdapter->sVersion,tcsVer);
		}
		
		// get the adapter name
		COPYSTRING(pAdapter->sAdapterName, Adapter.Description);
		COPYSTRING(pAdapter->sDevicePath, Adapter.DeviceName);
		
		// get the driver name
		COPYSTRING(pAdapter->sDriver, Adapter.Driver);
		
		// the vendor and device ID
		pAdapter->dwVendorID = Adapter.VendorId;
		pAdapter->dwChipID = Adapter.DeviceId;

		// Save adapter information to the internal structure
		memcpy(&pAdapter->Guid, &Adapter.DeviceIdentifier, sizeof(GUID));
		COPYSTRING(pAdapter->sDescription, Adapter.Description);
		pAdapter->hMonitor = m_pSrcD3D9->GetAdapterMonitor(dwAdapters);

		// create PnP ID string
		char szBuffer[48];
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_%02X"),
				Adapter.VendorId, Adapter.DeviceId, Adapter.SubSysId, Adapter.Revision);
		COPYSTRING(pAdapter->sPnPID, szBuffer);
	
		if( NULL == pAdapter->sVersion || NULL == pAdapter->sAdapterName ||
			NULL == pAdapter->sDriver  || NULL == pAdapter->sDescription ||
			NULL == pAdapter->sPnPID || NULL == pAdapter->sDevicePath )
		{
			DPF(1, _T("CD3D::InitD3D9() - Out of memory.\n"));
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		// Obtain current format
		hr = m_pSrcD3D9->GetAdapterDisplayMode(dwAdapters, &Mode);
		if (FAILED(hr))
		{
			DPF(1, _T("CD3D::InitD3D9() - Failed to obtain current display mode, hr=%s(%x)\n"),
						   HResultToString(hr), hr);
			goto Exit;
		}
		
    
		// Generate the resolution string
		pAdapter->DesktopMode.dwWidth = Mode.Width;
		pAdapter->DesktopMode.dwHeight = Mode.Height;
		pAdapter->DesktopMode.Format.d3dfFormat = Mode.Format;
		d3df2ddpf(Mode.Format, &pAdapter->DesktopMode.Format.ddpfPixelFormat);
		pAdapter->DesktopMode.dwBPP = pAdapter->DesktopMode.Format.ddpfPixelFormat.dwRGBBitCount;
		pAdapter->DesktopMode.dwRefreshRate = Mode.RefreshRate;

		
		for(dwAdapterModes = 0; dwAdapterModes < g_dwDisplayModeFormats9; dwAdapterModes++)
		{
			DISPLAYMODE *pTempModes = NULL;

			// Enumerate display modes
			dwNumModes = m_pSrcD3D9->GetAdapterModeCount(dwAdapters, (D3DFORMAT)g_DisplayModeFormats9[dwAdapterModes]);
			
			// Allocate the display modes
			pTempModes = new DISPLAYMODE[dwNumModes+pAdapter->nModes];
			if(pTempModes == NULL)
			{
				DPF(1, _T("CD3D::InitD3D9() - Out of memory.\n"));
				hr = E_OUTOFMEMORY;
				goto Exit;
			}
	
			memset(pTempModes, 0, sizeof(DISPLAYMODE) * (dwNumModes+pAdapter->nModes));
			memcpy(pTempModes, pAdapter->pModes, sizeof(DISPLAYMODE) * pAdapter->nModes);
			SAFEDELETEA(pAdapter->pModes);
			pAdapter->pModes = pTempModes;
        
			for(int i=0; i < dwNumModes; i++)
			{
				DISPLAYMODE *pMode = &pAdapter->pModes[pAdapter->nModes++];
	
				hr = m_pSrcD3D9->EnumAdapterModes(dwAdapters, (D3DFORMAT)g_DisplayModeFormats9[dwAdapterModes], i, &Mode);
				if (FAILED(hr))
				{
					DPF(1, _T("CD3D::InitD3D9() - EnumAdapterModes failed. hr=%s(%x)\n"), HResultToString(hr), hr);
					goto Exit;
				}
	
				pMode->dwWidth = (DWORD)Mode.Width;
				pMode->dwHeight = (DWORD)Mode.Height;
				pMode->dwRefreshRate = (DWORD)Mode.RefreshRate;
				pMode->Format.d3dfFormat = Mode.Format;
				d3df2ddpf(pMode->Format.d3dfFormat, &pMode->Format.ddpfPixelFormat);
				pMode->dwBPP = pMode->Format.ddpfPixelFormat.dwRGBBitCount;
			}
		}
	}

	//  Update other adapter related information
	UpdateAdapterInfo();

Exit:
	if(FAILED(hr))
	{
		ReleaseAll();
	}
	return hr;
}

// --------------------------------------------------------------------------------------

bool CD3D::RegisterRasterizer9
(
	__in LPCTSTR pcsDLLName,
	__in LPCTSTR pcsEntryPointName
)
{
	DPF(4, _T("CD3D::RegisterRasterizer()\n"));


	bool bOverallResult = false;

	LPCSTR pcsAdjustedDLLName = pcsDLLName;
	LPCSTR pcsAdjustedEntryPointName = pcsEntryPointName;

	typedef HRESULT (WINAPI *PD3D9GETSWINFO)
	(
		LPVOID pCaps,
		LPVOID pCallbacks,
		DWORD*,
		LPVOID,
		DWORD*,
		LPVOID
	);

	PD3D9GETSWINFO fn = ( PD3D9GETSWINFO ) NULL;

	if (!m_bUserSelectedSWRast)
	{
		pcsAdjustedDLLName = _T("d3dref9.dll");
		pcsAdjustedEntryPointName = _T("D3D9GetSWInfo");

		// Save the rasterizer name
		COPYSTRING(m_sSWRast, pcsAdjustedDLLName);
		if (m_sSWRast == NULL)
		{
			DPF(1, _T("CD3D::RegisterRasterizer9() - Out of memory.\n"));

			goto Cleanup;
		}

		COPYSTRING(m_sSWRastEntry, pcsAdjustedEntryPointName);
		if (m_sSWRastEntry == NULL)
		{
			DPF(1, _T("CD3D::RegisterRasterizer9() - Out of memory.\n"));

			goto Cleanup;
		}
	}

	m_hSWRast = LoadLibraryEx( pcsAdjustedDLLName, NULL, 0 );
	if ( m_hSWRast == NULL )
	{
		DPF
		(
			1,
			"CD3D::RegisterRasterizer9() - Failed to load software rasterizer %s.\n",
			pcsAdjustedDLLName
		);

		goto Cleanup;
	}


	fn = (PD3D9GETSWINFO) GetProcAddress( m_hSWRast, pcsAdjustedEntryPointName );
	if ( fn == NULL )
	{
		DPF
		(
			1,
			"CD3D::RegisterRasterizer9() - Failed to locate software rasterizer entry point %s.\n",
			pcsAdjustedEntryPointName
		);

		goto Cleanup;
	}


	HRESULT hRegisterRasterizerResult = m_pSrcD3D9->RegisterSoftwareDevice(fn);
	if ( FAILED( hRegisterRasterizerResult ) ) 
	{
		DPF
		(
			1,
			_T("CD3D::RegisterRasterizer9() - Failed to register software rasterizer. hr=%x\n"),
			hRegisterRasterizerResult
		);

		goto Cleanup;
	}


	bOverallResult = true;


Cleanup:
	fn = NULL;

	if ( bOverallResult != true )
	{
		if ( m_hSWRast != NULL )
		{
			FreeLibrary( m_hSWRast );
			m_hSWRast = NULL;
		};
	}


	return bOverallResult;
}

// --------------------------------------------------------------------------------------

void CD3D::ReleaseD3D9()
{
	ULONG dwRef = 0;

	UnRegisterRasterizer();

	RELEASE(m_pRefD3D9);

	if(m_pSrcD3D9)
	{
		dwRef = m_pSrcD3D9->Release();
		if ( dwRef > 0 )
		{
			DPF(1, _T("CD3D::ReleaseAll9() - m_pD3D9 ref count nonzero(%d).  TestApp leaking D3D objects.\n"), dwRef);

			ASSERT(g_pFramework != NULL);
			if(g_pFramework->m_bStressMode)
			{
				ASSERT(dwRef == 0);
			}
		}

		m_pSrcD3D9 = NULL;
	}
}


///////////////////////////////////////////////////////
// Function:  ResolveBehaviors9
//
// Purpose:
// Resolves ambiguous behavior flags requested by a
// test group to a mask of actual behavior flags.
//
// Details:
// On entry to this function, fBehaviors is a mask of
// DEVICECREATE_xxx flags.  Several of these are
// either/or types of flags which need to be turned
// into one or the other:
// - DEVICECREATE_SW_OR_HW_VP maps to DEVICECREATE_HWVP
//   if the device supports hardware vertex processing,
//   or DEVICECREATE_SWVP otherwise.
// - DEVICECREATE_HWVP_OR_PURE maps to the union of
//   DEVICECREATE_HWVP with either DEVICECREATE_PURE
//   if the device supports PURE, or else just
//   DEVICECREATE_STANDARD if it doesn't.
// - DEVICECREATE_BESTAVAILABLE turns out to be the
//   same as DEVICECREATE_HWVP_OR_PURE.
//
// Additionally, the test may have requested features
// that the device doesn't support:
// - If the device doesn't support hardware vertex
//   processing, then DEVICECREATE_HWVP and
//   DEVICECREATE_MIXED are both bumped down to
//   DEVICECREATE_SWVP.
// - DEVICECREATE_PURE negates DEVICECREATE_MIXED.
//
// Note that we don't necessarily mask out behavior
// flags like DEVICECREATE_STAND