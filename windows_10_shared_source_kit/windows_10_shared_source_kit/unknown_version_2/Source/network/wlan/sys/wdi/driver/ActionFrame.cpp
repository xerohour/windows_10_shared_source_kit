";

VS_CRITERIA VS_090_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_090_Desc = "vs_2_0 : pow source reg combination oT0, oPts is NOT allowed";
string VS_090 = 
	"vs_2_0 "
	"dcl ts "
	"pow r0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_091_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_091_Desc = "vs_2_0 : pow source reg combination oT0, oD0 is NOT allowed";
string VS_091 = 
	"vs_2_0 "
	"pow r0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_092_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_092_Desc = "vs_2_0 : pow source reg combination oT0, oT0 is NOT allowed";
string VS_092 = 
	"vs_2_0 "
	"pow r0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_093_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_093_Desc = "vs_2_0 : pow source reg combination -r0, c0 is NOT allowed";
string VS_093 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"pow r0, -r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_094_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_094_Desc = "vs_2_0 : pow source reg combination r0, -c0 is NOT allowed";
string VS_094 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"pow r0, r0, -c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_095_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_095_Desc = "vs_2_0 : pow dest r# is allowed";
string VS_095 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_096_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_2_0 : pow dest v# is NOT allowed";
string VS_096 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow v0, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_097_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_2_0 : pow dest c# is NOT allowed";
string VS_097 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow c1, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_098_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_2_0 : pow dest a0 is NOT allowed";
string VS_098 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow a0, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_099_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_2_0 : pow dest b# is NOT allowed";
string VS_099 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow b0, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_100_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_100_Desc = "vs_2_0 : pow dest i# is NOT allowed";
string VS_100 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow i0, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_101_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_2_0 : pow dest aL is NOT allowed";
string VS_101 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow aL, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_102_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_2_0 : pow dest oPos is NOT allowed";
string VS_102 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow oPos, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_103_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_2_0 : pow dest oD# is NOT allowed";
string VS_103 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow oD0, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_104_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_2_0 : pow dest oT# is NOT allowed";
string VS_104 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow oT0, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_105_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_2_0 : pow write mask .x is allowed";
string VS_105 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.x, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_106_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_2_0 : pow write mask .y is allowed";
string VS_106 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.y, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_107_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_2_0 : pow write mask .z is allowed";
string VS_107 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.z, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_108_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_2_0 : pow write mask .w is allowed";
string VS_108 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.w, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_109_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_109_Desc = "vs_2_0 : pow write mask .xy is allowed";
string VS_109 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.xy, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_110_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_110_Desc = "vs_2_0 : pow write mask .yz is allowed";
string VS_110 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.yz, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_111_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_2_0 : pow write mask .zw is allowed";
string VS_111 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.zw, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_112_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_2_0 : pow write mask .xz is allowed";
string VS_112 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.xz, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_113_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_2_0 : pow write mask .xw is allowed";
string VS_113 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.xw, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_114_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_2_0 : pow write mask .yw is allowed";
string VS_114 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.yw, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_115_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_2_0 : pow write mask .xyz is allowed";
string VS_115 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.xyz, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_116_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_2_0 : pow write mask .yzw is allowed";
string VS_116 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.yzw, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_117_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_2_0 : pow write mask .xzw is allowed";
string VS_117 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.xzw, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_118_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_2_0 : pow write mask .xyzw is allowed";
string VS_118 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.xyzw, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_119_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_119_Desc = "vs_2_0 : pow write mask .yx is NOT allowed";
string VS_119 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.yx, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_120_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_120_Desc = "vs_2_0 : pow write mask .zx is NOT allowed";
string VS_120 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.zx, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_121_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_2_0 : pow write mask .zy is NOT allowed";
string VS_121 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.zy, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_122_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_2_0 : pow write mask .wx is NOT allowed";
string VS_122 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.wx, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_123_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_2_0 : pow write mask .wz is NOT allowed";
string VS_123 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.wz, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_124_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_2_0 : pow write mask .wy is NOT allowed";
string VS_124 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.wy, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_125_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_125_Desc = "vs_2_0 : pow write mask .zyx is NOT allowed";
string VS_125 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.zyx, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_126_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_126_Desc = "vs_2_0 : pow write mask .wzy is NOT allowed";
string VS_126 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.wzy, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_127_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_127_Desc = "vs_2_0 : pow write mask .wzx is NOT allowed";
string VS_127 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.wzx, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_128_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_128_Desc = "vs_2_0 : pow write mask .wyx is NOT allowed";
string VS_128 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.wyx, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_129_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_129_Desc = "vs_2_0 : pow write mask .yxw is NOT allowed";
string VS_129 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.yxw, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_130_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_130_Desc = "vs_2_0 : pow write mask .wzyx is NOT allowed";
string VS_130 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.wzyx, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_131_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_131_Desc = "vs_2_0 : pow write mask .zxwy is NOT allowed";
string VS_131 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"pow r0.zxwy, v0.x, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_132_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_132_Desc = "vs_2_0 : pow is allowed in a 1 level if b0 block";
string VS_132 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if b0 "
	"	pow r0, v0.x, c0.x "
	"endif ";

VS_CRITERIA VS_133_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_133_Desc = "vs_2_0 : pow is allowed in a 8 level if b0 block";
string VS_133 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								pow r0, v0.x, c0.x "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_134_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_134_Desc = "vs_2_0 : pow is allowed in a 16 level if b0 block";
string VS_134 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
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
	"																pow r0, v0.x, c0.x "
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

VS_CRITERIA VS_135_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_135_Desc = "vs_2_0 : pow is NOT allowed in a 24 level if b0 block";
string VS_135 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
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
	"																								pow r0, v0.x, c0.x "
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

VS_CRITERIA VS_136_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_136_Desc = "vs_2_0 : pow is NOT allowed in a 25 level if b0 block";
string VS_136 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
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
	"																									pow r0, v0.x, c0.x "
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

VS_CRITERIA VS_137_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_2_0 : pow is allowed in a 1 level if b0 nop else block";
string VS_137 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if b0 nop else "
	"	pow r0, v0.x, c0.x "
	"endif ";

VS_CRITERIA VS_138_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_2_0 : pow is allowed in a 8 level if b0 nop else block";
string VS_138 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								pow r0, v0.x, c0.x "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_139_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_139_Desc = "vs_2_0 : pow is NOT allowed in a 16 level if b0 nop else block";
string VS_139 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
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
	"																pow r0, v0.x, c0.x "
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

VS_CRITERIA VS_140_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_140_Desc = "vs_2_0 : pow is NOT allowed in a 24 level if b0 nop else block";
string VS_140 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
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
	"																								pow r0, v0.x, c0.x "
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

VS_CRITERIA VS_141_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_141_Desc = "vs_2_0 : pow is NOT allowed in a 25 level if b0 nop else block";
string VS_141 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
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
	"																									pow r0, v0.x, c0.x "
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

VS_CRITERIA VS_142_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_142_Desc = "vs_2_0 : pow is allowed in a 1 level loop aL, i0 block";
string VS_142 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	pow r0, v0.x, c0.x "
	"endloop ";

VS_CRITERIA VS_143_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_143_Desc = "vs_2_0 : pow is NOT allowed in a 2 level loop aL, i0 block";
string VS_143 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		pow r0, v0.x, c0.x "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_144_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_144_Desc = "vs_2_0 : pow is NOT allowed in a 3 level loop aL, i0 block";
string VS_144 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			pow r0, v0.x, c0.x "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_145_Criter