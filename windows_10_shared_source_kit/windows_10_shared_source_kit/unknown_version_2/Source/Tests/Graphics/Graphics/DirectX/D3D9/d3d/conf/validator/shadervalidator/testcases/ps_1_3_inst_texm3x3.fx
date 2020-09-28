 : frc source reg combination c0.yx is allowed";
string VS_019 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"frc r0, c0.yx "
	"mov oPos, c0 ";

VS_CRITERIA VS_020_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_2_0 : frc source reg combination c0.wz is allowed";
string VS_020 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"frc r0, c0.wz "
	"mov oPos, c0 ";

VS_CRITERIA VS_021_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_2_0 : frc source reg combination c0.zy is allowed";
string VS_021 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"frc r0, c0.zy "
	"mov oPos, c0 ";

VS_CRITERIA VS_022_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_2_0 : frc source reg combination a0 is NOT allowed";
string VS_022 = 
	"vs_2_0 "
	"mova a0.x, c0.x "
	"frc r0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_023_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_2_0 : frc source reg combination b0 is NOT allowed";
string VS_023 = 
	"vs_2_0 "
	"defb b0, true "
	"frc r0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_024_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_2_0 : frc source reg combination i0 is NOT allowed";
string VS_024 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"frc r0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_025_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_2_0 : frc source reg combination aL is NOT allowed";
string VS_025 = 
	"vs_2_0 "
	"mova aL.x, c0.x "
	"frc r0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_026_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_2_0 : frc source reg combination oPos is NOT allowed";
string VS_026 = 
	"vs_2_0 "
	"frc r0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_027_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_2_0 : frc source reg combination oFog is NOT allowed";
string VS_027 = 
	"vs_2_0 "
	"frc r0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_028_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_2_0 : frc source reg combination oPts is NOT allowed";
string VS_028 = 
	"vs_2_0 "
	"dcl ts "
	"frc r0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_029_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_2_0 : frc source reg combination oD0 is NOT allowed";
string VS_029 = 
	"vs_2_0 "
	"frc r0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_030_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_2_0 : frc source reg combination oT0 is NOT allowed";
string VS_030 = 
	"vs_2_0 "
	"frc r0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_031_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_2_0 : frc dest r# is allowed";
string VS_031 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_032_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_2_0 : frc dest oPos is allowed";
string VS_032 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc oPos, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_033_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_2_0 : frc dest oD# is allowed";
string VS_033 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc oD0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_034_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_2_0 : frc dest oT# is allowed";
string VS_034 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc oT0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_035_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_2_0 : frc dest v# is NOT allowed";
string VS_035 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc v0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_036_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_2_0 : frc dest c# is NOT allowed";
string VS_036 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc c1, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_037_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_2_0 : frc dest a0 is NOT allowed";
string VS_037 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc a0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_038_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_2_0 : frc dest b# is NOT allowed";
string VS_038 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc b0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_039_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_2_0 : frc dest i# is NOT allowed";
string VS_039 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc i0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_040_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_2_0 : frc dest aL is NOT allowed";
string VS_040 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc aL, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_041_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_2_0 : frc write mask .x is allowed";
string VS_041 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.x, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_042_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_2_0 : frc write mask .y is allowed";
string VS_042 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.y, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_043_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_2_0 : frc write mask .z is allowed";
string VS_043 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.z, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_044_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_2_0 : frc write mask .w is allowed";
string VS_044 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.w, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_045_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_2_0 : frc write mask .xy is allowed";
string VS_045 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.xy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_046_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_2_0 : frc write mask .yz is allowed";
string VS_046 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.yz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_047_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_2_0 : frc write mask .zw is allowed";
string VS_047 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.zw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_048_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_2_0 : frc write mask .xz is allowed";
string VS_048 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.xz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_049_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_2_0 : frc write mask .xw is allowed";
string VS_049 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.xw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_050_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_2_0 : frc write mask .yw is allowed";
string VS_050 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.yw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_051_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_2_0 : frc write mask .xyz is allowed";
string VS_051 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.xyz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_052_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_2_0 : frc write mask .yzw is allowed";
string VS_052 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.yzw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_053_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_2_0 : frc write mask .xzw is allowed";
string VS_053 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.xzw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_054_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_2_0 : frc write mask .xyzw is allowed";
string VS_054 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.xyzw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_055_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_2_0 : frc write mask .yx is NOT allowed";
string VS_055 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.yx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_056_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_2_0 : frc write mask .zx is NOT allowed";
string VS_056 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.zx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_057_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_2_0 : frc write mask .zy is NOT allowed";
string VS_057 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.zy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_058_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_2_0 : frc write mask .wx is NOT allowed";
string VS_058 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.wx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_059_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_2_0 : frc write mask .wz is NOT allowed";
string VS_059 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.wz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_060_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_060_Desc = "vs_2_0 : frc write mask .wy is NOT allowed";
string VS_060 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.wy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_061_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_2_0 : frc write mask .zyx is NOT allowed";
string VS_061 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.zyx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_062_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_2_0 : frc write mask .wzy is NOT allowed";
string VS_062 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.wzy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_063_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_2_0 : frc write mask .wzx is NOT allowed";
string VS_063 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.wzx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_064_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_2_0 : frc write mask .wyx is NOT allowed";
string VS_064 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.wyx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_065_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_2_0 : frc write mask .yxw is NOT allowed";
string VS_065 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.yxw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_066_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_2_0 : frc write mask .wzyx is NOT allowed";
string VS_066 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.wzyx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_067_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_2_0 : frc write mask .zxwy is NOT allowed";
string VS_067 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0.zxwy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_068_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_2_0 : frc is allowed in a 1 level if b0 block";
string VS_068 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 "
	"	frc r0, v0 "
	"endif ";

VS_CRITERIA VS_069_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_2_0 : frc is allowed in a 8 level if b0 block";
string VS_069 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								frc r0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_070_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_070_Desc = "vs_2_0 : frc is allowed in a 16 level if b0 block";
string VS_070 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
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
	"																frc r0, v0 "
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

VS_CRITERIA VS_071_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_2_0 : frc is NOT allowed in a 24 level if b0 block";
string VS_071 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
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
	"																								frc r0, v0 "
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

VS_CRITERIA VS_072_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_2_0 : frc is NOT allowed in a 25 level if b0 block";
string VS_072 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
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
	"																									frc r0, v0 "
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

VS_CRITERIA VS_073_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_2_0 : frc is allowed in a 1 level if b0 nop else block";
string VS_073 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 nop else "
	"	frc r0, v0 "
	"endif ";

VS_CRITERIA VS_074_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_2_0 : frc is allowed in a 8 level if b0 nop else block";
string VS_074 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								frc r0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_075_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_2_0 : frc is NOT allowed in a 16 level if b0 nop else block";
string VS_075 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
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
	"																frc r0, v0 "
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

VS_CRITERIA VS_076_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_2_0 : frc is NOT allowed in a 24 level if b0 nop else block";
string VS_076 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
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
	"																								frc r0, v0 "
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

VS_CRITERIA VS_077_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_2_0 : frc is NOT allowed in a 25 level if b0 nop else block";
string VS_077 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
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
	"																	if b0 nop else