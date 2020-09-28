 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_2952_Criteria = { false, false, 0x0200, 2, -1, 2, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2952_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 break and rep i0 break_eq c0.x, c0.y block";
string PS_2952 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	rep i0 break_eq c0.x, c0.y "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_2953_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2953_Desc = "ps_2_x : texldd is allowed in a rep i0 break and if b0 block";
string PS_2953 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if b0 "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_2954_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2954_Desc = "ps_2_x : texldd is allowed in a rep i0 break and if b0 nop else block";
string PS_2954 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if b0 nop else "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_2955_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2955_Desc = "ps_2_x : texldd is NOT allowed in a rep i0 break and loop aL, i0 block";
string PS_2955 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_2956_Criteria = { true, false, 0x0200, 1, -1, 2, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2956_Desc = "ps_2_x : texldd is allowed in a rep i0 break and rep i0 block";
string PS_2956 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_2957_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2957_Desc = "ps_2_x : texldd is allowed in a rep i0 break and if_eq c0.x, c0.y block";
string PS_2957 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if_eq c0.x, c0.y "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_2958_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2958_Desc = "ps_2_x : texldd is allowed in a rep i0 break and if_eq c0.x, c0.y nop else block";
string PS_2958 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if_eq c0.x, c0.y nop else "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_2959_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2959_Desc = "ps_2_x : texldd is allowed in a rep i0 break and if p0.x block";
string PS_2959 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if p0.x "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_2960_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2960_Desc = "ps_2_x : texldd is allowed in a rep i0 break and if p0.x nop else block";
string PS_2960 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_2961_Criteria = { false, false, 0x0200, 2, -1, 2, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2961_Desc = "ps_2_x : texldd is NOT allowed in a rep i0 break and loop aL, i0 break block";
string PS_2961 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_2962_Criteria = { true, false, 0x0200, 2, -1, 2, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2962_Desc = "ps_2_x : texldd is allowed in a rep i0 break and rep i0 break block";
string PS_2962 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_2963_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2963_Desc = "ps_2_x : texldd is NOT allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string PS_2963 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_2964_Criteria = { true, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2964_Desc = "ps_2_x : texldd is allowed in a rep i0 break and rep i0 breakp p0.x block";
string PS_2964 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_2965_Criteria = { false, false, 0x0200, 2, -1, 2, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2965_Desc = "ps_2_x : texldd is NOT allowed in a rep i0 break and loop aL, i0 break_eq c0.x, c0.y block";
string PS_2965 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_2966_Criteria = { true, false, 0x0200, 2, -1, 2, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2966_Desc = "ps_2_x : texldd is allowed in a rep i0 break and rep i0 break_eq c0.x, c0.y block";
string PS_2966 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 break_eq c0.x, c0.y "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_2967_Criteria = { false, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2967_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and if b0 block";
string PS_2967 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_2968_Criteria = { false, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2968_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string PS_2968 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_2969_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2969_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string PS_2969 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_2970_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2970_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string PS_2970 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_2971_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2971_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and if_ne c0.x, c0.y block";
string PS_2971 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_ne c0.x, c0.y "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_2972_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2972_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and if_ne c0.x, c0.y nop else block";
string PS_2972 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_ne c0.x, c0.y nop else "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_2973_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2973_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string PS_2973 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_2974_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2974_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string PS_2974 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_2975_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2975_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string PS_2975 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_2976_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2976_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string PS_2976 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_2977_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2977_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_2977 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_2978_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2978_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_2978 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_2979_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2979_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string PS_2979 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_2980_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2980_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break_eq c0.x, c0.y block";
string PS_2980 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_eq c0.x, c0.y "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_2981_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2981_Desc = "ps_2_x : texldd is allowed in a rep i0 breakp p0.x and if b0 block";
string PS_2981 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_2982_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2982_Desc = "ps_2_x : texldd is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string PS_2982 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_2983_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2983_Desc = "ps_2_x : texldd is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string PS_2983 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_2984_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2984_Desc = "ps_2_x : texldd is allowed in a rep i0 breakp p0.x and rep i0 block";
string PS_2984 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_2985_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2985_Desc = "ps_2_x : texldd is allowed in a rep i0 breakp p0.x and if_gt c0.x, c0.y block";
string PS_2985 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if_gt c0.x, c0.y "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_2986_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2986_Desc = "ps_2_x : texldd is allowed in a rep i0 breakp p0.x and if_gt c0.x, c0.y nop else block";
string PS_2986 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if_gt c0.x, c0.y nop else "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_2987_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2987_Desc = "ps_2_x : texldd is allowed in a rep i0 breakp p0.x and if p0.x block";
string PS_2987 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_2988_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2988_Desc = "ps_2_x : texldd is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string PS_2988 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_2989_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2989_Desc = "ps_2_x : texldd is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string PS_2989 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_2990_Criteria = { true, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2990_Desc = "ps_2_x : texldd is allowed in a rep i0 breakp p0.x and rep i0 break block";
string PS_2990 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_2991_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2991_Desc = "ps_2_x : texldd is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_2991 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_2992_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2992_Desc = "ps_2_x : texldd is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_2992 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_2993_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2993_Desc = "ps_2_x : texldd is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string PS_2993 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_2994_Criteria = { true, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2994_Desc = "ps_2_x : texldd is allowed in a rep i0 breakp p0.x and rep i0 break_eq c0.x, c0.y block";
string PS_2994 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_eq c0.x, c0.y "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_2995_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2995_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if b0 block";
string PS_2995 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if b0 "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_2996_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2996_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if b0 nop else block";
string PS_2996 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if b0 nop else "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_2997_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2997_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 block";
string PS_2997 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 "
	"		texldd r0, r0, s0, v0, r1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_2998_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2998_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 block";
string PS_2998 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 "
	"		texldd r0, r0, s0, v0, r1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_2999_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_2999_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if_gt c0.x, c0.y block";
string PS_2999 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_3000_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_3000_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if_gt c0.x, c0.y nop else block";
string PS_3000 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if_gt c0.x, c0.y nop else "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_3001_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_3001_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if p0.x block";
string PS_3001 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if p0.x "
	"		texldd r0, r0, s0, v0, r1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_3002_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION + CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_3002_Desc = "ps_2_x : texldd is NOT allowed in a loop aL, i0 break_eq c