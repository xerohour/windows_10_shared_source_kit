
	"if p0.x nop else "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_262_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_262_Desc = "ps_2_x : abs is NOT allowed in a if p0.x nop else and loop aL, i0 break block";
string PS_262 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_263_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_263_Desc = "ps_2_x : abs is allowed in a if p0.x nop else and rep i0 break block";
string PS_263 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x nop else "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_264_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_264_Desc = "ps_2_x : abs is NOT allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string PS_264 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_265_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_265_Desc = "ps_2_x : abs is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string PS_265 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_266_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_266_Desc = "ps_2_x : abs is NOT allowed in a if p0.x nop else and loop aL, i0 break_eq c0.x, c0.y block";
string PS_266 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_267_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_267_Desc = "ps_2_x : abs is allowed in a if p0.x nop else and rep i0 break_lt c0.x, c0.y block";
string PS_267 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x nop else "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_268_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_268_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and if b0 block";
string PS_268 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_269_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_269_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and if b0 nop else block";
string PS_269 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_270_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_270_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and loop aL, i0 block";
string PS_270 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_271_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_271_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and rep i0 block";
string PS_271 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_272_Criteria = { false, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_272_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and if_eq c0.x, c0.y block";
string PS_272 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_273_Criteria = { false, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_273_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and if_eq c0.x, c0.y nop else block";
string PS_273 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_274_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_274_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and if p0.x block";
string PS_274 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_275_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_275_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_275 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_276_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_276_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_276 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_277_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_277_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and rep i0 break block";
string PS_277 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_278_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_278_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_278 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_279_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_279_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_279 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_280_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_280_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and loop aL, i0 break_eq c0.x, c0.y block";
string PS_280 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_281_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_281_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 break and rep i0 break_lt c0.x, c0.y block";
string PS_281 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_282_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_282_Desc = "ps_2_x : abs is allowed in a rep i0 break and if b0 block";
string PS_282 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_283_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_283_Desc = "ps_2_x : abs is allowed in a rep i0 break and if b0 nop else block";
string PS_283 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_284_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_284_Desc = "ps_2_x : abs is NOT allowed in a rep i0 break and loop aL, i0 block";
string PS_284 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_285_Criteria = { true, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_285_Desc = "ps_2_x : abs is allowed in a rep i0 break and rep i0 block";
string PS_285 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_286_Criteria = { true, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_286_Desc = "ps_2_x : abs is allowed in a rep i0 break and if_eq c0.x, c0.y block";
string PS_286 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_287_Criteria = { true, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_287_Desc = "ps_2_x : abs is allowed in a rep i0 break and if_eq c0.x, c0.y nop else block";
string PS_287 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_288_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_288_Desc = "ps_2_x : abs is allowed in a rep i0 break and if p0.x block";
string PS_288 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_289_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_289_Desc = "ps_2_x : abs is allowed in a rep i0 break and if p0.x nop else block";
string PS_289 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_290_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_290_Desc = "ps_2_x : abs is NOT allowed in a rep i0 break and loop aL, i0 break block";
string PS_290 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_291_Criteria = { true, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_291_Desc = "ps_2_x : abs is allowed in a rep i0 break and rep i0 break block";
string PS_291 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_292_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_292_Desc = "ps_2_x : abs is NOT allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string PS_292 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_293_Criteria = { true, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_293_Desc = "ps_2_x : abs is allowed in a rep i0 break and rep i0 breakp p0.x block";
string PS_293 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_294_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_294_Desc = "ps_2_x : abs is NOT allowed in a rep i0 break and loop aL, i0 break_eq c0.x, c0.y block";
string PS_294 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_295_Criteria = { true, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_295_Desc = "ps_2_x : abs is allowed in a rep i0 break and rep i0 break_lt c0.x, c0.y block";
string PS_295 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_296_Criteria = { false, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_296_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and if b0 block";
string PS_296 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_297_Criteria = { false, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_297_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string PS_297 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_298_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_298_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string PS_298 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_299_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_299_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string PS_299 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "