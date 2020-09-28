ing VS_129_Desc = "vs_2_x : dst source reg combination oD0, aL is NOT allowed";
string VS_129 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"dst r0, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_130_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_130_Desc = "vs_2_x : dst source reg combination oD0, p0 is NOT allowed";
string VS_130 = 
	"vs_2_x "
	"dst r0, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_131_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_131_Desc = "vs_2_x : dst source reg combination oD0, oPos is NOT allowed";
string VS_131 = 
	"vs_2_x "
	"dst r0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_132_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_132_Desc = "vs_2_x : dst source reg combination oD0, oFog is NOT allowed";
string VS_132 = 
	"vs_2_x "
	"dst r0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_133_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_133_Desc = "vs_2_x : dst source reg combination oD0, oPts is NOT allowed";
string VS_133 = 
	"vs_2_x "
	"dcl ts "
	"dst r0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_134_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_134_Desc = "vs_2_x : dst source reg combination oD0, oD0 is NOT allowed";
string VS_134 = 
	"vs_2_x "
	"dst r0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_135_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_135_Desc = "vs_2_x : dst source reg combination oD0, oT0 is NOT allowed";
string VS_135 = 
	"vs_2_x "
	"dst r0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_136_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_136_Desc = "vs_2_x : dst source reg combination oT0, a0 is NOT allowed";
string VS_136 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"dst r0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_137_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_2_x : dst source reg combination oT0, b0 is NOT allowed";
string VS_137 = 
	"vs_2_x "
	"defb b0, true "
	"dst r0, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_138_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_2_x : dst source reg combination oT0, i0 is NOT allowed";
string VS_138 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dst r0, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_139_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_139_Desc = "vs_2_x : dst source reg combination oT0, aL is NOT allowed";
string VS_139 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"dst r0, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_140_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_140_Desc = "vs_2_x : dst source reg combination oT0, p0 is NOT allowed";
string VS_140 = 
	"vs_2_x "
	"dst r0, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_141_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_141_Desc = "vs_2_x : dst source reg combination oT0, oPos is NOT allowed";
string VS_141 = 
	"vs_2_x "
	"dst r0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_142_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_142_Desc = "vs_2_x : dst source reg combination oT0, oFog is NOT allowed";
string VS_142 = 
	"vs_2_x "
	"dst r0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_143_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_143_Desc = "vs_2_x : dst source reg combination oT0, oPts is NOT allowed";
string VS_143 = 
	"vs_2_x "
	"dcl ts "
	"dst r0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_144_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_144_Desc = "vs_2_x : dst source reg combination oT0, oD0 is NOT allowed";
string VS_144 = 
	"vs_2_x "
	"dst r0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_145_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_145_Desc = "vs_2_x : dst source reg combination oT0, oT0 is NOT allowed";
string VS_145 = 
	"vs_2_x "
	"dst r0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_146_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_146_Desc = "vs_2_x : dst dest r# is allowed";
string VS_146 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_147_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_147_Desc = "vs_2_x : dst dest oPos is allowed";
string VS_147 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst oPos, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_148_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_148_Desc = "vs_2_x : dst dest oD# is allowed";
string VS_148 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst oD0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_149_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_149_Desc = "vs_2_x : dst dest oT# is allowed";
string VS_149 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst oT0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_150_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_150_Desc = "vs_2_x : dst dest v# is NOT allowed";
string VS_150 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst v0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_151_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_151_Desc = "vs_2_x : dst dest c# is NOT allowed";
string VS_151 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst c1, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_152_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_152_Desc = "vs_2_x : dst dest a0 is NOT allowed";
string VS_152 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst a0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_153_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_153_Desc = "vs_2_x : dst dest b# is NOT allowed";
string VS_153 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst b0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_154_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_154_Desc = "vs_2_x : dst dest i# is NOT allowed";
string VS_154 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst i0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_155_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_155_Desc = "vs_2_x : dst dest aL is NOT allowed";
string VS_155 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst aL, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_156_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_156_Desc = "vs_2_x : dst dest p0 is NOT allowed";
string VS_156 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst p0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_157_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_157_Desc = "vs_2_x : dst write mask .x is allowed";
string VS_157 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.x, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_158_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_158_Desc = "vs_2_x : dst write mask .y is allowed";
string VS_158 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.y, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_159_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_159_Desc = "vs_2_x : dst write mask .z is allowed";
string VS_159 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.z, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_160_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_160_Desc = "vs_2_x : dst write mask .w is allowed";
string VS_160 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.w, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_161_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_161_Desc = "vs_2_x : dst write mask .xy is allowed";
string VS_161 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.xy, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_162_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_162_Desc = "vs_2_x : dst write mask .yz is allowed";
string VS_162 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.yz, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_163_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_163_Desc = "vs_2_x : dst write mask .zw is allowed";
string VS_163 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.zw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_164_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_164_Desc = "vs_2_x : dst write mask .xz is allowed";
string VS_164 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.xz, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_165_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_165_Desc = "vs_2_x : dst write mask .xw is allowed";
string VS_165 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.xw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_166_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_166_Desc = "vs_2_x : dst write mask .yw is allowed";
string VS_166 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.yw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_167_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_167_Desc = "vs_2_x : dst write mask .xyz is allowed";
string VS_167 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.xyz, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_168_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_168_Desc = "vs_2_x : dst write mask .yzw is allowed";
string VS_168 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.yzw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_169_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_169_Desc = "vs_2_x : dst write mask .xzw is allowed";
string VS_169 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.xzw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_170_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_170_Desc = "vs_2_x : dst write mask .xyzw is allowed";
string VS_170 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.xyzw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_171_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_171_Desc = "vs_2_x : dst write mask .yx is NOT allowed";
string VS_171 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.yx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_172_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_172_Desc = "vs_2_x : dst write mask .zx is NOT allowed";
string VS_172 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.zx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_173_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_173_Desc = "vs_2_x : dst write mask .zy is NOT allowed";
string VS_173 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.zy, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_174_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_174_Desc = "vs_2_x : dst write mask .wx is NOT allowed";
string VS_174 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.wx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_175_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_175_Desc = "vs_2_x : dst write mask .wz is NOT allowed";
string VS_175 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.wz, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_176_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_176_Desc = "vs_2_x : dst write mask .wy is NOT allowed";
string VS_176 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.wy, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_177_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_177_Desc = "vs_2_x : dst write mask .zyx is NOT allowed";
string VS_177 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.zyx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_178_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_178_Desc = "vs_2_x : dst write mask .wzy is NOT allowed";
string VS_178 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.wzy, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_179_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_179_Desc = "vs_2_x : dst write mask .wzx is NOT allowed";
string VS_179 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.wzx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_180_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_180_Desc = "vs_2_x : dst write mask .wyx is NOT allowed";
string VS_180 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.wyx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_181_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_181_Desc = "vs_2_x : dst write mask .yxw is NOT allowed";
string VS_181 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.yxw, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_182_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_182_Desc = "vs_2_x : dst write mask .wzyx is NOT allowed";
string VS_182 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.wzyx, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_183_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_183_Desc = "vs_2_x : dst write mask .zxwy is NOT allowed";
string VS_183 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"dst r0.zxwy, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_184_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_184_Desc = "vs_2_x : dst is allowed in a 1 level if b0 block";
string VS_184 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	dst r0, v0, r0 "
	"endif ";

VS_CRITERIA VS_185_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_185_Desc = "vs_2_x : dst is allowed in a 8 level if b0 block";
string VS_185 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								dst r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_186_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_186_Desc = "vs_2_x : dst is allowed in a 16 level if b0 block";
string VS_186 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
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
	"																dst r0, v0, r0 "
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

VS_CRITERIA VS_187_Criteria = { false, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_187_Desc = "vs_2_x : dst is NOT allowed in a 24 level if b0 block";
string VS_187 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
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
	"																								dst r0, v0, r0 "
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

VS_CRITERIA VS_188_Criteria = { false, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_188_Desc = "vs_2_x : dst is NOT allowed in a 25 level if b0 block";
string VS_188 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
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
	"							