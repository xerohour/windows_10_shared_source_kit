ring PS_046_Desc = "ps_2_0 : add source reg combination i0, oC0 is NOT allowed";
string PS_046 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"add r0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_2_0 : add source reg combination i0, oDepth is NOT allowed";
string PS_047 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"add r0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_048_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_2_0 : add source reg combination p0, b0 is NOT allowed";
string PS_048 = 
	"ps_2_0 "
	"defb b0, true "
	"add r0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_049_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_2_0 : add source reg combination p0, i0 is NOT allowed";
string PS_049 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"add r0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_2_0 : add source reg combination p0, p0 is NOT allowed";
string PS_050 = 
	"ps_2_0 "
	"add r0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_051_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_2_0 : add source reg combination p0, s0 is NOT allowed";
string PS_051 = 
	"ps_2_0 "
	"dcl_cube s0 "
	"add r0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_052_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_2_0 : add source reg combination p0, oC0 is NOT allowed";
string PS_052 = 
	"ps_2_0 "
	"add r0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_053_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_2_0 : add source reg combination p0, oDepth is NOT allowed";
string PS_053 = 
	"ps_2_0 "
	"add r0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_054_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_2_0 : add source reg combination s0, b0 is NOT allowed";
string PS_054 = 
	"ps_2_0 "
	"defb b0, true "
	"dcl_cube s0 "
	"add r0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_055_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_2_0 : add source reg combination s0, i0 is NOT allowed";
string PS_055 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"add r0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_056_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_2_0 : add source reg combination s0, p0 is NOT allowed";
string PS_056 = 
	"ps_2_0 "
	"dcl_cube s0 "
	"add r0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_057_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_2_0 : add source reg combination s0, s0 is NOT allowed";
string PS_057 = 
	"ps_2_0 "
	"dcl_cube s0 "
	"add r0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_058_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_2_0 : add source reg combination s0, oC0 is NOT allowed";
string PS_058 = 
	"ps_2_0 "
	"dcl_cube s0 "
	"add r0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_059_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_2_0 : add source reg combination s0, oDepth is NOT allowed";
string PS_059 = 
	"ps_2_0 "
	"dcl_cube s0 "
	"add r0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_060_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_2_0 : add source reg combination oC0, b0 is NOT allowed";
string PS_060 = 
	"ps_2_0 "
	"defb b0, true "
	"add r0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_061_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_2_0 : add source reg combination oC0, i0 is NOT allowed";
string PS_061 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"add r0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_062_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_2_0 : add source reg combination oC0, p0 is NOT allowed";
string PS_062 = 
	"ps_2_0 "
	"add r0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_063_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_2_0 : add source reg combination oC0, s0 is NOT allowed";
string PS_063 = 
	"ps_2_0 "
	"dcl_cube s0 "
	"add r0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_064_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_2_0 : add source reg combination oC0, oC0 is NOT allowed";
string PS_064 = 
	"ps_2_0 "
	"add r0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_065_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_2_0 : add source reg combination oC0, oDepth is NOT allowed";
string PS_065 = 
	"ps_2_0 "
	"add r0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_066_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_2_0 : add source reg combination oDepth, b0 is NOT allowed";
string PS_066 = 
	"ps_2_0 "
	"defb b0, true "
	"add r0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_067_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_2_0 : add source reg combination oDepth, i0 is NOT allowed";
string PS_067 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"add r0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_068_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_2_0 : add source reg combination oDepth, p0 is NOT allowed";
string PS_068 = 
	"ps_2_0 "
	"add r0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_069_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_2_0 : add source reg combination oDepth, s0 is NOT allowed";
string PS_069 = 
	"ps_2_0 "
	"dcl_cube s0 "
	"add r0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_070_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_2_0 : add source reg combination oDepth, oC0 is NOT allowed";
string PS_070 = 
	"ps_2_0 "
	"add r0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_071_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_2_0 : add source reg combination oDepth, oDepth is NOT allowed";
string PS_071 = 
	"ps_2_0 "
	"add r0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_072_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_2_0 : add source reg combination r0.agrb, r1 is NOT allowed";
string PS_072 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0.agrb, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_073_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_2_0 : add source reg combination r0.rbga, r1 is NOT allowed";
string PS_073 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0.rbga, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_074_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_2_0 : add source reg combination r0.rgab, r1 is NOT allowed";
string PS_074 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0.rgab, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_075_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_2_0 : add source reg combination r0.bgr, r1 is NOT allowed";
string PS_075 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0.bgr, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_076_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_2_0 : add source reg combination r0.rbg, r1 is NOT allowed";
string PS_076 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0.rbg, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_077_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_2_0 : add source reg combination r0.gar, r1 is NOT allowed";
string PS_077 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0.gar, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_078_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_2_0 : add source reg combination r0.gr, r1 is NOT allowed";
string PS_078 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0.gr, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_079_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_2_0 : add source reg combination r0.ab, r1 is NOT allowed";
string PS_079 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0.ab, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_080_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_2_0 : add source reg combination r0.bg, r1 is NOT allowed";
string PS_080 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0.bg, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_081_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_2_0 : add source reg combination r0, r1.agrb is NOT allowed";
string PS_081 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.agrb "
	"mov oC0, c0 ";

PS_CRITERIA PS_082_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_2_0 : add source reg combination r0, r1.rbga is NOT allowed";
string PS_082 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.rbga "
	"mov oC0, c0 ";

PS_CRITERIA PS_083_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_2_0 : add source reg combination r0, r1.rgab is NOT allowed";
string PS_083 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.rgab "
	"mov oC0, c0 ";

PS_CRITERIA PS_084_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_2_0 : add source reg combination r0, r1.bgr is NOT allowed";
string PS_084 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.bgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_085_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_2_0 : add source reg combination r0, r1.rbg is NOT allowed";
string PS_085 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.rbg "
	"mov oC0, c0 ";

PS_CRITERIA PS_086_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_2_0 : add source reg combination r0, r1.gar is NOT allowed";
string PS_086 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.gar "
	"mov oC0, c0 ";

PS_CRITERIA PS_087_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_2_0 : add source reg combination r0, r1.gr is NOT allowed";
string PS_087 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.gr "
	"mov oC0, c0 ";

PS_CRITERIA PS_088_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_2_0 : add source reg combination r0, r1.ab is NOT allowed";
string PS_088 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.ab "
	"mov oC0, c0 ";

PS_CRITERIA PS_089_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_2_0 : add source reg combination r0, r1.bg is NOT allowed";
string PS_089 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.bg "
	"mov oC0, c0 ";

PS_CRITERIA PS_090_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_2_0 : add dest r# is allowed";
string PS_090 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_091_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_2_0 : add _pp dest r# is allowed";
string PS_091 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_pp r0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_092_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_2_0 : add _sat dest r# is allowed";
string PS_092 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_sat r0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_093_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_2_0 : add dest v# is NOT allowed";
string PS_093 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add v0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_094_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_2_0 : add _pp dest v# is NOT allowed";
string PS_094 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_pp v0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_095_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_2_0 : add _sat dest v# is NOT allowed";
string PS_095 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_sat v0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_096_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_2_0 : add dest c# is NOT allowed";
string PS_096 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add c1, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_097_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_2_0 : add _pp dest c# is NOT allowed";
string PS_097 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_pp c1, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_098_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_2_0 : add _sat dest c# is NOT allowed";
string PS_098 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_sat c1, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_099_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_2_0 : add dest b# is NOT allowed";
string PS_099 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add b0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_100_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_2_0 : add _pp dest b# is NOT allowed";
string PS_100 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_pp b0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_101_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_2_0 : add _sat dest b# is NOT allowed";
string PS_101 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_sat b0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_102_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_2_0 : add dest i# is NOT allowed";
string PS_102 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add i0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_103_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_2_0 : add _pp dest i# is NOT allowed";
string PS_103 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_pp i0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_104_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_2_0 : add _sat dest i# is NOT allowed";
string PS_104 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_sat i0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_105_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_105_Desc = "ps_2_0 : add dest p0 is NOT allowed";
string PS_105 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add p0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_106_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_106_Desc = "ps_2_0 : add _pp dest p0 is NOT allowed";
string PS_106 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_pp p0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_107_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_107_Desc = "ps_2_0 : add _sat dest p0 is NOT allowed";
string PS_107 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_sat p0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_108_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_2_0 : add dest s# is NOT allowed";
string PS_108 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add s0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_109_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_2_0 : add _pp dest s# is NOT allowed";
string PS_109 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_pp s0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_110_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_2_0 : add _sat dest s# is NOT allowed";
string PS_110 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_sat s0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_111_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_2_0 : add dest t# is NOT allowed";
string PS_111 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add t0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_112_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_2_0 : add _pp dest t# is NOT allowed";
string PS_112 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_pp t0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_113_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_2_0 : add _sat dest t# is NOT allowed";
string PS_113 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_sat t0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_114_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_2_0 : add dest oC# is NOT allowed";
string PS_114 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add oC0, t0, r0 ";

PS_CRITERIA PS_115_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_2_0 : add _pp dest oC# is NOT allowed";
string PS_115 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_pp oC0, t0, r0 ";

PS_CRITERIA PS_116_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_116_Desc = "ps_2_0 : add _sat dest oC# is NOT allowed";
string PS_116 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_sat oC0, t0, r0 ";

PS_CRITERIA PS_117_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_117_Desc = "ps_2_0 : add dest oDepth is NOT allowed";
string PS_117 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add oDepth, t0.x, r0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_118_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_118_Desc = "ps_2_0 : add _pp dest oDepth is NOT allowed";
string PS_118 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_pp oDepth, t0.x, r0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_119_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_119_Desc = "ps_2_0 : add _sat dest oDepth is NOT allowed";
string PS_119 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add_sat oDepth, t0.x, r0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_120_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_120_Desc = "ps_2_0 : add write mask .r is allowed";
string PS_120 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.r, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_121_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_121_Desc = "ps_2_0 : add write mask .g is allowed";
string PS_121 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.g, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_122_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_122_Desc = "ps_2_0 : add write mask .b is allowed";
string PS_122 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.b, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_123_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_123_Desc = "ps_2_0 : add write mask .a is allowed";
string PS_123 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.a, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_124_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_124_Desc = "ps_2_0 : add write mask .rg is allowed";
string PS_124 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.rg, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_125_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_125_Desc = "ps_2_0 : add write mask .gb is allowed";
string PS_125 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.gb, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_126_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_126_Desc = "ps_2_0 : add write mask .ba is allowed";
string PS_126 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.ba, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_127_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_127_Desc = "ps_2_0 : add write mask .rb is allowed";
string PS_127 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.rb, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_128_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_128_Desc = "ps_2_0 : add write mask .ra is allowed";
string PS_128 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.ra, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_129_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_129_Desc = "ps_2_0 : add write mask .ga is allowed";
string PS_129 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.ga, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_130_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_130_Desc = "ps_2_0 : add write mask .rgb is allowed";
string PS_130 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.rgb, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_131_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_131_Desc = "ps_2_0 : add write mask .gba is allowed";
string PS_131 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.gba, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_132_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_132_Desc = "ps_2_0 : add write mask .rba is allowed";
string PS_132 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.rba, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_133_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_133_Desc = "ps_2_0 : add write mask .rgba is allowed";
string PS_133 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.rgba, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_134_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_134_Desc = "ps_2_0 : add write mask .yx is NOT allowed";
string PS_134 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.yx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_135_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_135_Desc = "ps_2_0 : add write mask .zx is NOT allowed";
string PS_135 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.zx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_136_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_136_Desc = "ps_2_0 : add write mask .zy is NOT allowed";
string PS_136 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.zy, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_137_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_137_Desc = "ps_2_0 : add write mask .wx is NOT allowed";
string PS_137 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.wx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_138_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_138_Desc = "ps_2_0 : add write mask .wz is NOT allowed";
string PS_138 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.wz, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_139_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_139_Desc = "ps_2_0 : add write mask .wy is NOT allowed";
string PS_139 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.wy, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_140_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_140_Desc = "ps_2_0 : add write mask .zyx is NOT allowed";
string PS_140 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.zyx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_141_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_141_Desc = "ps_2_0 : add write mask .wzy is NOT allowed";
string PS_141 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.wzy, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_142_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_142_Desc = "ps_2_0 : add write mask .wzx is NOT allowed";
string PS_142 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.wzx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_143_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_143_Desc = "ps_2_0 : add write mask .wyx is NOT allowed";
string PS_143 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.wyx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_144_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_144_Desc = "ps_2_0 : add write mask .yxw is NOT allowed";
string PS_144 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.yxw, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_145_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_145_Desc = "ps_2_0 : add write mask .wzyx is NOT allowed";
string PS_145 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.wzyx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_146_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_146_Desc = "ps_2_0 : add write mask .zxwy is NOT allowed";
string PS_146 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"add r0.zxwy, t0, r0 "
	"mov oC0, c0 ";


// Copyright(C) Microsoft.All rights reserved.

#include "common.h"

CFilterTopoGraph::~CFilterTopoGraph()
{
    while( !vectorNodeVertex.empty() )
    {
        NodeVertex* pNodeVertex = vectorNodeVertex.back();
        if( nullptr != pNodeVertex )
        {
            delete pNodeVertex;
            pNodeVertex = nullptr;
        }

        vectorNodeVertex.pop_back();
    }

    while( !vectorFilterPinVertex.empty() )
    {
        FilterPinVertex* pFilterPinVertex = vectorFilterPinVertex.back();
        if( nullptr != pFilterPinVertex )
        {
            delete pFilterPinVertex;
            pFilterPinVertex = nullptr;
        }

        vectorFilterPinVertex.pop_back();
    }
}

bool CFilterTopoGraph::BuildFromFilter( CFilter* pFilter )
{
    bool fRet = true;

    // populate the filter pin and node vectors
    UINT connectionCount = pFilter->GetCountConnections();
    for( UINT i = 0; i < connectionCount; i++ )
    {
        CConnection* pConnection = pFilter->GetConnection(i);
        if( nullptr == pConnection )
        {
            XLOG( XFAIL, eError, "  ERROR: Unable to obtain reference to Connection[%d]", i );
            fRet = false;
            goto Exit;
        }

        // First, do the FROM side of the topology connection

        if( pConnection->FromNode() == KSFILTER_NODE )
        {
            // this side of the connection is a filter pin.
            // we will create a new FilterPinVertex if we have not created one already.

            FilterPinVertex* pFilterPinVertex = nullptr;

            for( auto iter = vectorFilterPinVertex.begin(); iter != vectorFilterPinVertex.end(); iter++ )
            {
                if( (*iter)->GetPinId() == pConnection->FromNodePin() )
                {
                    pFilterPinVertex = *iter;
                    break;
                }
            }

            if( !pFilterPinVertex )
            {
                pFilterPinVertex = new FilterPinVertex( pConnection->FromNodePin() );
                if( !pFilterPinVertex )
                {
                    XLOG( XFAIL, eError, "  ERROR: Unable to create filter pin vertex for filter pin %u", pConnection->FromNodePin() );
                    fRet = false;
                    goto Exit;
                }

                vectorFilterPinVertex.push_back( pFilterPinVertex );
            }
        }
        else
        {
            // this side of the connection is a node pin.
            // we will create a new NodeVertex if we have not created one already.
            // Then increment the connection count for the node's output pin

            NodeVertex* pNodeVertex = nullptr;

            for( auto iter = vectorNodeVertex.begin(); iter != vectorNodeVertex.end(); iter++ )
            {
                if( (*iter)->GetNodeId() == pConnection->FromNode() )
                {
                    pNodeVertex = *iter;
                    break;
                }
            }

            if( !pNodeVertex )
            {
                pNodeVertex = new NodeVertex( pConnection->FromNode() );
                if( !pNodeVertex )
                {
                    XLOG( XFAIL, eError, "  ERROR: Unable to create node vertex for node %u", pConnection->FromNode() );
                    fRet = false;
                    goto Exit;
                }

                vectorNodeVertex.push_back( pNodeVertex );
            }

            auto outputPinIter = pNodeVertex->mapOutputNodePin.find( pConnection->FromNodePin() );
            if( pNodeVertex->mapOutputNodePin.end() == outputPinIter )
            {
                // this is the first time we've seen this node pin
                pNodeVertex->mapOutputNodePin.insert( std::pair<ULONG,int>( pConnection->FromNodePin(), 1 ) );
            }
            else
            {
                // otherwise, just increment the connection count
                outputPinIter->second++;
            }
        }

        // now, do the same thing for the TO side of the topology connection

        if( pConnection->ToNode() == KSFILTER_NODE )
        {
            // this side of the connection is a filter pin.
            // we will create a new FilterPinVertex if we have not created one already.

            FilterPinVertex* pFilterPinVertex = nullptr;

            for( auto iter = vectorFilterPinVertex.begin(); iter != vectorFilterPinVertex.end(); iter++ )
            {
                if( (*iter)->GetPinId() == pConnection->ToNodePin() )
                {
                    pFilterPinVertex = *iter;
                    break;
                }
            }

            if( !pFilterPinVertex )
            {
                pFilterPinVertex = new FilterPinVertex( pConnection->ToNodePin() );
                if( !pFilterPinVertex )
                {
                    XLOG(XFAIL, eError, "  ERROR: Unable to create filter pin vertex for filter pin %u", pConnection->ToNodePin() );
                    fRet = false;
                    goto Exit;
                }

                vectorFilterPinVertex.push_back( pFilterPinVertex );
            }
        }
        else
        {
            // this side of the connection is a node pin.
            // we will create a new NodeVertex if we have not created one already.
            // Then increment the connection count for the node's input pin

            NodeVertex* pNodeVertex = nullptr;

            for( auto iter = vectorNodeVertex.begin(); iter != vectorNodeVertex.end(); iter++ )
            {
                if( (*iter)->GetNodeId() == pConnection->ToNode() )
                {
                    pNodeVertex = *iter;
                    break;
                }
            }

            if( !pNodeVertex )
            {
                pNodeVertex = new NodeVertex( pConnection->ToNode() );
                if( !pNodeVertex )
                {
                    XLOG( XFAIL, eError, "  ERROR: Unable to create node vertex for node %u", pConnection->ToNode() );
                    fRet = false;
                    goto Exit;
                }

                vectorNodeVertex.push_back( pNodeVertex );
            }

            auto inputPinIter = pNodeVertex->mapInputNodePin.find( pConnection->ToNodePin() );
            if( pNodeVertex->mapInputNodePin.end() == inputPinIter )
            {
                // this is the first time we've seen this node pin
                pNodeVertex->mapInputNodePin.insert( std::pair<ULONG,int>( pConnection->ToNodePin(), 1 ) );
            }
            else
            {
                // otherwise, just increment the connection count
                (inputPinIter->second)++;
            }
        }
    }

Exit:

    return fRet;
}
/*++
Copyright (c) Microsoft Corporation

Module Name:

PSM20_AutoGen.h

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Enum of States in all the State Machines
//
typedef enum _PSM20_STATE {
    Psm20StateIgnored                                = 1000,
    Psm20StateEmptySubState                          = 1001,
    Psm20StateNull                                   = 1002,
    Psm20StateCompleteWithStatusFailed               = 1003,
    Psm20StateRequestPortCycle                       = 1004,
    Psm20StateSignalQueryDeviceTextEvent             = 1005,
    Psm20StateCompleteFDORequestWithStatusFailed     = 1006,
    Psm20StateCompleteGetPortStatusWithInternalError = 1007,
    Psm20StateCompleteGetPortStatusWithNoSuchDevice  = 1008,
    Psm20StateCompletePdoPreStart                    = 1009,
    Psm20StateSignalPnpPowerEvent                    = 1010,
    Psm20StatePoweredOffDisconnected                 = 3000,
    Psm20StateAcquiringInterruptReferenceOnHSMInDisabled = 3001,
    Psm20StateAcquiringInterruptReferenceOnHSMInEnabled = 3002,
    Psm20StateAcquiringInterruptReferenceOnResuming  = 3003,
    Psm20StateAcquiringPowerReferenceFromPoweredOffDisabledInS0 = 3004,
    Psm20StateAcquiringPowerReferenceFromPoweredOffDisconnectedInS0 = 3005,
    Psm20StateAcquiringPowerReferenceFromPoweredOffSuspendedInS0 = 3006,
    Psm20StateCancellingDisableOnHubStopSuspend      = 3007,
    Psm20StateCancellingDisablingPortOnPortCycleOnHubStop = 3008,
    Psm20StateCancellingDisablingPortOnPortCycleWithTimerOnHubStop = 3009,
    Psm20StateCancellingQueryPortStatus              = 3010,
    Psm20StateCancellingResetOnSurpriseRemove        = 3011,
    Psm20StateCancellingSetPortPower                 = 3012,
    Psm20StateCheckingIfConnectBitOne                = 3013,
    Psm20StateCheckingIfOvercurrentBitOne            = 3014,
    Psm20StateCheckingIfPersistentOvercurrent        = 3015,
    Psm20StateConnectedDisabled                      = 3016,
    Psm20StateConnectedEnabled                       = 3017,
    Psm20StateCreatingDevice                         = 3018,
    Psm20StateDetachingDeviceFromPortOnCycleOnHubStop = 3019,
    Psm20StateDetachingDeviceFromPortOnDisconnect    = 3020,
    Psm20StateDetachingDeviceFromPortOnDisconnectWithTimer = 3021,
    Psm20StateDetachingDeviceFromPortOnOverCurrent   = 3022,
    Psm20StateDetachingDeviceFromPortOnOverCurrentClear = 3023,
    Psm20StateDetachingDeviceFromPortOnOverCurrentClearWithTimer = 3024,
    Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer = 3025,
    Psm20StateDetachingDeviceFromPortOnPortCycle     = 3026,
    Psm20StateDetachingDeviceOnCleanup               = 3027,
    Psm20StateDetachingDeviceOnDisableAndCycle       = 3028,
    Psm20StateDetachingDeviceOnHubReset              = 3029,
    Psm20StateDetachingDeviceOnPortCycleWithTimer    = 3030,
    Psm20StateDetachingDeviceOnPortDisableAndCycleWithTimer = 3031,
    Psm20StateDisablingOnHubSuspendWithTimer         = 3032,
    Psm20StateDisablingPortBeforeConnecting          = 3033,
    Psm20StateDisablingPortOnCycle                   = 3034,
    Psm20StateDisablingPortOnDeviceRequest           = 3035,
    Psm20StateDisablingPortOnHubSuspend              = 3036,
    Psm20StateDisablingPortOnPortCycleWithTimer      = 3037,
    Psm20StateDisablingPortOnTimeOut                 = 3038,
    Psm20StateDisconnected                           = 3039,
    Psm20StateErrorOnAcquringReferenceOnHubResume    = 3040,
    Psm20StateErrorOnConnectedDisabled               = 3041,
    Psm20StateErrorOnConnectedEnabled                = 3042,
    Psm20StateErrorOnDisconnected                    = 3043,
    Psm20StateErrorOnIssuingResetComplete            = 3044,
    Psm20StateErrorOnResettingEnabled                = 3045,
    Psm20StateErrorOnResettingPort                   = 3046,
    Psm20StateErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled = 3047,
    Psm20StateErrorOnStoppingTimerOnDisconnect       = 3048,
    Psm20StateErrorOnStoppingTimerOnOverCurrent      = 3049,
    Psm20StateErrorOnStoppingTimerOnOverCurrentClear = 3050,
    Psm20StateErrorOnStoppingTimerOnPortCycle        = 3051,
    Psm20StateErrorOnSuspended                       = 3052,
    Psm20StateErrorOnWaitingForCompanionPort         = 3053,
    Psm20StateErrorOnWaitingForDebounce              = 3054,
    Psm20StateErrorOnWaitingForDSMResumeResponse     = 3055,
    Psm20StateErrorOnWaitingForOldDeviceToCleanup    = 3056,
    Psm20StateErrorOnWaitingForOverCurrentClear      = 3057,
    Psm20StateErrorOnWaitingForResumeComplete        = 3058,
    Psm20StateErrorOnWaitingForResumeRecoveryTimer   = 3059,
    Psm20StateErrorOnWaitingForResumeTimerToFlush    = 3060,
    Psm20StateFlushingStaleDeviceEvents              = 3061,
    Psm20StateFlushingUserResetForOverCurrentOnHubStopSuspend = 3062,
    Psm20StateFlushingUserResetForOverCurrentOnOverCurrentClear = 3063,
    Psm20StateInitiatingResetPort                    = 3064,
    Psm20StateInitiatingResume                       = 3065,
    Psm20StateIsItBootDevice                         = 3066,
    Psm20StateIsOldDevicePresent                     = 3067,
    Psm20StateIssuingAttachDeviceToPort              = 3068,
    Psm20StateIssuingDetachDeviceOnCycleFromPoweredOffDisabledOrSuspended = 3069,
    Psm20StateIssuingDisabledToDSMFromDisabled       = 3070,
    Psm20StateIssuingHubReset                        = 3071,
    Psm20StateIssuingHubResetFromDisconnected        = 3072,
    Psm20StateIssuingHubResetOnDisableFailure        = 3073,
    Psm20StateIssuingHubResetWhenWaitingForDSMResumeResponse = 3074,
    Psm20StateIssuingHubResetWhileHubSuspend         = 3075,
    Psm20StateIssuingHubResetWithPendingTimer        = 3076,
    Psm20StateIssuingHubResetWithTimer               = 3077,
    Psm20StateIssuingHubResetWithTimerOnHubSuspend   = 3078,
    Psm20StateIssuingHubResetWithTimerWhileHubSuspend = 3079,
    Psm20StateIssuingPortDisabledToDevice            = 3080,
    Psm20StateIssuingPortDisableFailedToDevice       = 3081,
    Psm20StateIssuingReAttachDeviceToBootDevice      = 3082,
    Psm20StateIssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled = 3083,
    Psm20StateIssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled = 3084,
    Psm20StateIssuingResumedToDeviceSM               = 3085,
    Psm20StateIssuingSuspendedToDeviceSM             = 3086,
    Psm20StateNotifyingUserAboutPersistentOverCurrent = 3087,
    Psm20StatePoweredOffDisabled                     = 3088,
    Psm20StatePoweredOffSuspended                    = 3089,
    Psm20StateQueryingPortStatusOnOverCurrent        = 3090,
    Psm20StateQueueingPortFailureToDSMOnSuspend      = 3091,
    Psm20StateQueueingPortResumeFailedDueToPendingSuspend = 3092,
    Psm20StateQueueingPortResumeTimedOut             = 3093,
    Psm20StateQueueingStateDisabledOnHubResumeFromDisabled = 3094,
    Psm20StateQueueingStateDisabledToDeviceSM        = 3095,
    Psm20StateQueueingStateDisabledToDeviceSMOnResetHub = 3096,
    Psm20StateQueueingStateEnabledToDeviceSM         = 3097,
    Psm20StateQueueingStateSuspendedToDeviceSM       = 3098,
    Psm20StateQueueingStateSuspendedToDeviceSMOnIgnoreError = 3099,
    Psm20StateReleasingInterruptReferenceAndIssuingPortResetTimedOutToDSM = 3100,
    Psm20StateReleasingInterruptReferenceAndStoppingTimer = 3101,
    Psm20StateReleasingInterruptReferenceOnResetTransferFailure = 3102,
    Psm20StateReleasingInterruptReferenceOnSuccessfulReset = 3103,
    Psm20StateReleasingPowerReferenceonPoweredOffDisabled = 3104,
    Psm20StateReleasingPowerReferenceonPoweredOffDisconnected = 3105,
    Psm20StateReleasingResetReferenceOnDeviceRemoval = 3106,
    Psm20StateResettingEnabled                       = 3107,
    Psm20StateSettingPortPowerOnOverCurrent          = 3108,
    Psm20StateStartingAndAcquiringReferenceOnHubResume = 3109,
    Psm20StateStartingAndAcquiringReferenceOnHubResumeFromDisabled = 3110,
    Psm20StateStartingAndAcquiringReferenceOnHubStart = 3111,
    Psm20StateStartingAndAcquiringReferenceOnHubStartFromDisabled = 3112,
    Psm20StateStartingAndAcquiringReferenceOnWarmResume = 3113,
    Psm20StateStartingOverCurrentTimer               = 3114,
    Psm20StateStartingResetTimer                     = 3115,
    Psm20StateStartingResumeRecoveryTimer            = 3116,
    Psm20StateStartingResumeTimer                    = 3117,
    Psm20StateStoppingResumeTimer                    = 3118,
    Psm20StateStoppingTimerAndQueueingHubResetInDisconnected = 3119,
    Psm20StateStoppingTimerOnDisconnect              = 3120,
    Psm20StateStoppingTimerOnHubStopSuspendInDisconnected = 3121,
    Psm20StateStoppingTimerOnOverCurrent             = 3122,
    Psm20StateStoppingTimerOnOverCurrentClear        = 3123,
    Psm20StateStoppingTimerOnPortCycle               = 3124,
    Psm20StateStoppingTimerOnResetComplete           = 3125,
    Psm20StateSuspended                              = 3126,
    Psm20StateSuspending                             = 3127,
    Psm20StateWaitingForDebounce                     = 3128,
    Psm20StateWaitingForDisableCompleteOnHubSuspend  = 3129,
    Psm20StateWaitingForDisableCompleteWithTimerOnHubSuspend = 3130,
    Psm20StateWaitingForDSMResumeResponse            = 3131,
    Psm20StateWaitingForDSMResumeResponseOnHubStop   = 3132,
    Psm20StateWaitingForHubStopSuspend               = 3133,
    Psm20StateWaitingForHubStopSuspendInDisconnected = 3134,
    Psm20StateWaitingForHubStopSuspendOnAttachFailure = 3135,
    Psm20StateWaitingForHubStopSuspendOrTimer        = 3136,
    Psm20StateWaitingForOldDeviceToDetach            = 3137,
    Psm20StateWaitingForOverCurrentClear             = 3138,
    Psm20StateWaitingForResetComplete                = 3139,
    Psm20StateWaitingForResetTimerToFlush            = 3140,
    Psm20StateWaitingForResumeComplete               = 3141,
    Psm20StateWaitingForResumeRecoveryTimer          = 3142,
    Psm20StateWaitingForResumeTimerToFlush           = 3143,
    Psm20StateWaitingForTimerOnResetInDisconnected   = 3144,
    Psm20StateWaitingForTimerToFlushOnDisconnect     = 3145,
    Psm20StateWaitingForTimerToFlushOnHubStopSuspend = 3146,
    Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected = 3147,
    Psm20StateWaitingForTimerToFlushOnOverCurrent    = 3148,
    Psm20StateWaitingForTimerToFlushOnOverCurrentClear = 3149,
    Psm20StateWaitingForTimerToFlushOnPortCycle      = 3150,
    Psm20StateWaitingForUserResetOnOverCurrent       = 3151,
    Psm20StateWaitingToBeDeleted                     = 3152,
    Psm20StateAckingPortChange                       = 3153,
    Psm20StateCheckIfThereIsAPortChange              = 3154,
    Psm20StateCheckIfThereIsAPortChangeOnPortStatus  = 3155,
    Psm20StateEnablingInterruptsAndGettingPortEvent  = 3156,
    Psm20StateGettingPortStatus                      = 3157,
    Psm20StateInitializingCumulativePortChangeBits   = 3158,
    Psm20StateIssuingHubResetOnControlTransferFailure = 3159,
    Psm20StateQueueingEnableInterruptTransferOnPortChange = 3160,
    Psm20StateWaitingForPortChangeEvent              = 3161,
    Psm20StateAcquiringPortReferenceOnStart          = 3162,
    Psm20StateGettingPortLostChangesOnStart          = 3163,
    Psm20StateGettingPortStatusOnStart               = 3164,
    Psm20StateIssuingHubResetOnControlTransferFailureOnStart = 3165,
    Psm20StatePoweringOnPortOnStart                  = 3166,
    Psm20StateQueueingEnableInterruptTransferOnStart = 3167,
    Psm20StateWaitingForPortPowerOnTimerOnStart      = 3168,
    Psm20StateAckingConnectChangeOnResume            = 3169,
    Psm20StateAckingPortChangeWhileWaitingForReconnect = 3170,
    Psm20StateAcquiringPortReferenceOnResume         = 3171,
    Psm20StateAcquiringPortReferenceOnResumeInFailure = 3172,
    Psm20StateCheckIfThereIsAPortChangeOnPortStatusWhileWaitingForReconnect = 3173,
    Psm20StateCheckingIfConnectBitChangeSetOnResume  = 3174,
    Psm20StateCheckingIfConnectBitIsOneOnChangeWhileWaitingForReconnect = 3175,
    Psm20StateCheckingIfDeviceDisconnectedOnResume   = 3176,
    Psm20StateCheckingIfDeviceDisconnectedOnResumeAfterWaiting = 3177,
    Psm20StateEnablingInterruptsWaitingForReconnect  = 3178,
    Psm20StateGettingPortLostChangesOnStartOnResume  = 3179,
    Psm20StateGettingPortStatusOnResume              = 3180,
    Psm20StateGettingPortStatusOnResumeAfterWaiting  = 3181,
    Psm20StateGettingPortStatusWhileWaitingForReconnect = 3182,
    Psm20StateInitializingCumulativePortChangeBitsWhileWaitingForReconnect = 3183,
    Psm20StateIssuingHubResetOnControlTransferFailureOnResume = 3184,
    Psm20StatePoweringOnPortOnResume                 = 3185,
    Psm20StateQueueingEnableInterruptTransferOnResume = 3186,
    Psm20StateQueueingStateDisabledOnFailure         = 3187,
    Psm20StateResettingHubOnFailureWhileWaitingForReconnect = 3188,
    Psm20StateReturningHubStopOnResume               = 3189,
    Psm20StateStartingTimerForAllowingReconnect      = 3190,
    Psm20StateStoppingReconnectTimerOnDeviceConnect  = 3191,
    Psm20StateStoppingReconnectTimerOnHubStop        = 3192,
    Psm20StateWaitingForDeviceToReconnect            = 3193,
    Psm20StateWaitingForPortPowerOnTimerOnResume     = 3194,
    Psm20StateWaitingForReconnectTimerToFlushOnHubStop = 3195,
    Psm20StateAckingPortChangeInSuspended            = 3196,
    Psm20StateAcquiringPowerReferenceOnHubS0IdleInD3Cold = 3197,
    Psm20StateCheckIfThereIsAPortChangeAfterInitialGetPortStatusOnSuspended = 3198,
    Psm20StateCheckIfThereIsAPortChangeOnSuspended   = 3199,
    Psm20StateCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold = 3200,
    Psm20StateCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold = 3201,
    Psm20StateCheckingIfDeviceIsConnectedOnPortChangeInD3Cold = 3202,
    Psm20StateCheckingIfDeviceIsConnectedOnPortChangeInD3ColdOnResume = 3203,
    Psm20StateCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume = 3204,
    Psm20StateCheckingIsD3ColdIsEnabled              = 3205,
    Psm20StateGettingPortChangeEventInSuspended      = 3206,
    Psm20StateGettingPortChangeOnResumeFromD3ColdSuspended = 3207,
    Psm20StateGettingPortStatusInSuspended           = 3208,
    Psm20StateInitializingCumulativePortChangeBitsInSuspended = 3209,
    Psm20StateIssuingHubResetOnControlTransferFailureInSuspended = 3210,
    Psm20StatePoweredOffOnHubSuspendFromD3Cold       = 3211,
    Psm20StatePoweringOnResumeFromD3ColdSuspended    = 3212,
    Psm20StateQueueingDisabledToDSMOnReconnectFromD3Cold = 3213,
    Psm20StateQueueingEnableInterruptTransferOnPortChangeForSuspended = 3214,
    Psm20StateQueueingPortStateDisabledOnHubResumeInD3Cold = 3215,
    Psm20StateQueueingResumeToDSMFromSuspended       = 3216,
    Psm20StateReEnablingInterruptsOnConnectChangeInSuspended = 3217,
    Psm20StateReEnablingInterruptsOnErrorInSuspended = 3218,
    Psm20StateReEnablingInterruptsOnOverCurrentClearedInSuspended = 3219,
    Psm20StateReEnablingInterruptsOnOverCurrentInSuspended = 3220,
    Psm20StateReleasingPowerReferenceOnHubS0IdleInD3Cold = 3221,
    Psm20StateReturningHubStopFromSuspendedInD3Cold  = 3222,
    Psm20StateReturningHubSuspendFromSuspendedInD3Cold = 3223,
    Psm20StateReturningPortConnectChangeFromSuspended = 3224,
    Psm20StateReturningPortErrorFromSuspended        = 3225,
    Psm20StateReturningPortOverCurrentClearedFromSuspended = 3226,
    Psm20StateReturningPortOverCurrentFromSuspended  = 3227,
    Psm20StateReturningPortResumedFromD3Cold         = 3228,
    Psm20StateReturningPortResumedFromSuspended      = 3229,
    Psm20StateStartingDebounceTimerOnResumeFromD3Cold = 3230,
    Psm20StateStartingDebounceTimerOnResumeFromD3ColdOnResume = 3231,
    Psm20StateStartingTimerForAllowingReconnectOnResumingFromD3Cold = 3232,
    Psm20StateStoppingDebounceTimerInD3Cold          = 3233,
    Psm20StateStoppingDebounceTimerInD3ColdOnResume  = 3234,
    Psm20StateStoppingReconnectTimerOnDeviceReAttachAfterD3Cold = 3235,
    Psm20StateStoppingReconnectTimerOnHubStopInD3Cold = 3236,
    Psm20StateStoppingReconnectTimerOnOvercurrentClearedInD3Cold = 3237,
    Psm20StateStoppingReconnectTimerOnOvercurrentInD3Cold = 3238,
    Psm20StateWaitingForDebounceTimerOnReconnectInD3Cold = 3239,
    Psm20StateWaitingForDebounceTimerOnReconnectInD3ColdOnResume = 3240,
    Psm20StateWaitingForDebounceTimerToFlushOnHubStop = 3241,
    Psm20StateWaitingForDebounceTimerToFlushOnHubStopOnResume = 3242,
    Psm20StateWaitingForDebounceTimerToFlushOnHubSuspend = 3243,
    Psm20StateWaitingForDebounceTimerToFlushOnPortChangeInD3Cold = 3244,
    Psm20StateWaitingForDebounceTimerToFlushOnPortChangeInD3ColdOnResume = 3245,
    Psm20StateWaitingForDeviceRequestResumeOnD3Cold  = 3246,
    Psm20StateWaitingForDeviceToReconnectOnResumeFromD3Cold = 3247,
    Psm20StateWaitingForPortChangeEventInD3Cold      = 3248,
    Psm20StateWaitingForPortChangeEventInSuspended   = 3249,
    Psm20StateWaitingForReconnectTimerToFlushOnHubStopInD3Cold = 3250,
    Psm20StateWaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold = 3251,
    Psm20StateWaitingForReconnectTimerToFlushOnOvercurrentInD3Cold = 3252,
    Psm20StateWaitingForReconnectTimerToFlushOnReattachAfterD3Cold = 3253,
    Psm20StateWaitingForResumeRecoveryTimerOnResumeInSuspended = 3254,
    Psm20StateAckingPortChangeInInWaitingForStopSuspend = 3255,
    Psm20StateCheckIfThereIsAPortChangeInInWaitingForStopSuspend = 3256,
    Psm20StateGettingPortStatusInWaitingForStopSuspend = 3257,
    Psm20StateQueueingEnableInterruptTransferInWaitingForStopSuspend = 3258,
    Psm20StateQueueingPortEventFailureToDSM          = 3259,
    Psm20StateWaitingForDevicePortEvents             = 3260,
    Psm20StateAckingPortChangeInInWaitingForStopSuspendInDisconnected = 3261,
    Psm20StateCheckIfThereIsAPortChangeInInWaitingForStopSuspendInDisconnected = 3262,
    Psm20StateGettingPortStatusInWaitingForStopSuspendInDisconnected = 3263,
    Psm20StateQueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected = 3264,
    Psm20StateWaitingForDevicePortEventsInDisconnected = 3265,
} PSM20_STATE, *PPSM20_STATE;





//
// Enum of ETW state names
//
typedef enum _PSM20_STATE_ETW {
    Psm20StateEtwPoweredOffDisconnected              = 3000,
    Psm20StateEtwAcquiringInterruptReferenceOnHSMInDisabled = 3001,
    Psm20StateEtwAcquiringInterruptReferenceOnHSMInEnabled = 3002,
    Psm20StateEtwAcquiringInterruptReferenceOnResuming = 3003,
    Psm20StateEtwAcquiringPowerReferenceFromPoweredOffDisabledInS0 = 3004,
    Psm20StateEtwAcquiringPowerReferenceFromPoweredOffDisconnectedInS0 = 3005,
    Psm20StateEtwAcquiringPowerReferenceFromPoweredOffSuspendedInS0 = 3006,
    Psm20StateEtwCancellingDisableOnHubStopSuspend   = 3007,
    Psm20StateEtwCancellingDisablingPortOnPortCycleOnHubStop = 3008,
    Psm20StateEtwCancellingDisablingPortOnPortCycleWithTimerOnHubStop = 3009,
    Psm20StateEtwCancellingQueryPortStatus           = 3010,
    Psm20StateEtwCancellingResetOnSurpriseRemove     = 3011,
    Psm20StateEtwCancellingSetPortPower              = 3012,
    Psm20StateEtwCheckingIfConnectBitOne             = 3013,
    Psm20StateEtwCheckingIfOvercurrentBitOne         = 3266,
    Psm20StateEtwCheckingIfPersistentOvercurrent     = 3265,
    Psm20StateEtwConnectedDisabled                   = 3014,
    Psm20StateEtwConnectedEnabled                    = 3015,
    Psm20StateEtwCreatingDevice                      = 3016,
    Psm20StateEtwDetachingDeviceFromPortOnCycleOnHubStop = 3017,
    Psm20StateEtwDetachingDeviceFromPortOnDisconnect = 3018,
    Psm20StateEtwDetachingDeviceFromPortOnDisconnectWithTimer = 3019,
    Psm20StateEtwDetachingDeviceFromPortOnOverCurrent = 3020,
    Psm20StateEtwDetachingDeviceFromPortOnOverCurrentClear = 3021,
    Psm20StateEtwDetachingDeviceFromPortOnOverCurrentClearWithTimer = 3022,
    Psm20StateEtwDetachingDeviceFromPortOnOverCurrentWithTimer = 3023,
    Psm20StateEtwDetachingDeviceFromPortOnPortCycle  = 3024,
    Psm20StateEtwDetachingDeviceOnCleanup            = 3025,
    Psm20StateEtwDetachingDeviceOnDisableAndCycle    = 3026,
    Psm20StateEtwDetachingDeviceOnHubReset           = 3027,
    Psm20StateEtwDetachingDeviceOnPortCycleWithTimer = 3028,
    Psm20StateEtwDetachingDeviceOnPortDisableAndCycleWithTimer = 3029,
    Psm20StateEtwDisablingOnHubSuspendWithTimer      = 3030,
    Psm20StateEtwDisablingPortBeforeConnecting       = 3031,
    Psm20StateEtwDisablingPortOnCycle                = 3032,
    Psm20StateEtwDisablingPortOnDeviceRequest        = 3033,
    Psm20StateEtwDisablingPortOnHubSuspend           = 3034,
    Psm20StateEtwDisablingPortOnPortCycleWithTimer   = 3035,
    Psm20StateEtwDisablingPortOnTimeOut              = 3036,
    Psm20StateEtwDisconnected                        = 3037,
    Psm20StateEtwErrorOnAcquringReferenceOnHubResume = 3038,
    Psm20StateEtwErrorOnConnectedDisabled            = 3039,
    Psm20StateEtwErrorOnConnectedEnabled             = 3040,
    Psm20StateEtwErrorOnDisconnected                 = 3041,
    Psm20StateEtwErrorOnIssuingResetComplete         = 3042,
    Psm20StateEtwErrorOnResettingEnabled             = 3043,
    Psm20StateEtwErrorOnResettingPort                = 3044,
    Psm20StateEtwErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled = 3045,
    Psm20StateEtwErrorOnStoppingTimerOnDisconnect    = 3046,
    Psm20StateEtwErrorOnStoppingTimerOnOverCurrent   = 3047,
    Psm20StateEtwErrorOnStoppingTimerOnOverCurrentClear = 3048,
    Psm20StateEtwErrorOnStoppingTimerOnPortCycle     = 3049,
    Psm20StateEtwErrorOnSuspended                    = 3050,
    Psm20StateEtwErrorOnWaitingForCompanionPort      = 3051,
    Psm20StateEtwErrorOnWaitingForDebounce           = 3052,
    Psm20StateEtwErrorOnWaitingForDSMResumeResponse  = 3053,
    Psm20StateEtwErrorOnWaitingForOldDeviceToCleanup = 3054,
    Psm20StateEtwErrorOnWaitingForOverCurrentClear   = 3055,
    Psm20StateEtwErrorOnWaitingForResumeComplete     = 3056,
    Psm20StateEtwErrorOnWaitingForResumeRecoveryTimer = 3057,
    Psm20StateEtwErrorOnWaitingForResumeTimerToFlush = 3058,
    Psm20StateEtwFlushingStaleDeviceEvents           = 3059,
    Psm20StateEtwFlushingUserResetForOverCurrentOnHubStopSuspend = 3060,
    Psm20StateEtwFlushingUserResetForOverCurrentOnOverCurrentClear = 3061,
    Psm20StateEtwInitiatingResetPort                 = 3062,
    Psm20StateEtwInitiatingResume                    = 3063,
    Psm20StateEtwIsItBootDevice                      = 3064,
    Psm20StateEtwIsOldDevicePresent                  = 3065,
    Psm20StateEtwIssuingAttachDeviceToPort           = 3066,
    Psm20StateEtwIssuingDetachDeviceOnCycleFromPoweredOffDisabledOrSuspended = 3067,
    Psm20StateEtwIssuingDisabledToDSMFromDisabled    = 3068,
    Psm20StateEtwIssuingHubReset                     = 3069,
    Psm20StateEtwIssuingHubResetFromDisconnected     = 3070,
    Psm20StateEtwIssuingHubResetOnDisableFailure     = 3071,
    Psm20StateEtwIssuingHubResetWhenWaitingForDSMResumeResponse = 3072,
    Psm20StateEtwIssuingHubResetWhileHubSuspend      = 3073,
    Psm20StateEtwIssuingHubResetWithPendingTimer     = 3074,
    Psm20StateEtwIssuingHubResetWithTimer            = 3075,
    Psm20StateEtwIssuingHubResetWithTimerOnHubSuspend = 3076,
    Psm20StateEtwIssuingHubResetWithTimerWhileHubSuspend = 3077,
    Psm20StateEtwIssuingPortDisabledToDevice         = 3078,
    Psm20StateEtwIssuingPortDisableFailedToDevice    = 3079,
    Psm20StateEtwIssuingReAttachDeviceToBootDevice   = 3080,
    Psm20StateEtwIssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled = 3081,
    Psm20StateEtwIssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled = 3082,
    Psm20StateEtwIssuingResumedToDeviceSM            = 3083,
    Psm20StateEtwIssuingSuspendedToDeviceSM          = 3084,
    Psm20StateEtwNotifyingUserAboutPersistentOverCurrent = 3085,
    Psm20StateEtwPoweredOffDisabled                  = 3086,
    Psm20StateEtwPoweredOffSuspended                 = 3087,
    Psm20StateEtwQueryingPortStatusOnOverCurrent     = 3088,
    Psm20StateEtwQueueingPortFailureToDSMOnSuspend   = 3089,
    Psm20StateEtwQueueingPortResumeFailedDueToPendingSuspend = 3090,
    Psm20StateEtwQueueingPortResumeTimedOut          = 3091,
    Psm20StateEtwQueueingStateDisabledOnHubResumeFromDisabled = 3092,
    Psm20StateEtwQueueingStateDisabledToDeviceSM     = 3093,
    Psm20StateEtwQueueingStateDisabledToDeviceSMOnResetHub = 3094,
    Psm20StateEtwQueueingStateEnabledToDeviceSM      = 3095,
    Psm20StateEtwQueueingStateSuspendedToDeviceSM    = 3096,
    Psm20StateEtwQueueingStateSuspendedToDeviceSMOnIgnoreError = 3097,
    Psm20StateEtwReleasingInterruptReferenceAndIssuingPortResetTimedOutToDSM = 3098,
    Psm20StateEtwReleasingInterruptReferenceAndStoppingTimer = 3099,
    Psm20StateEtwReleasingInterruptReferenceOnResetTransferFailure = 3100,
    Psm20StateEtwReleasingInterruptReferenceOnSuccessfulReset = 3101,
    Psm20StateEtwReleasingPowerReferenceonPoweredOffDisabled = 3102,
    Psm20StateEtwReleasingPowerReferenceonPoweredOffDisconnected = 3103,
    Psm20StateEtwReleasingResetReferenceOnDeviceRemoval = 3104,
    Psm20StateEtwResettingEnabled                    = 3105,
    Psm20StateEtwSettingPortPowerOnOverCurrent       = 3106,
    Psm20StateEtwStartingAndAcquiringReferenceOnHubResume = 3107,
    Psm20StateEtwStartingAndAcquiringReferenceOnHubResumeFromDisabled = 3108,
    Psm20StateEtwStartingAndAcquiringReferenceOnHubStart = 3109,
    Psm20StateEtwStartingAndAcquiringReferenceOnHubStartFromDisabled = 3110,
    Psm20StateEtwStartingAndAcquiringReferenceOnWarmResume = 3111,
    Psm20StateEtwStartingOverCurrentTimer            = 3112,
    Psm20StateEtwStartingResetTimer                  = 3113,
    Psm20StateEtwStartingResumeRecoveryTimer         = 3114,
    Psm20StateEtwStartingResumeTimer                 = 3115,
    Psm20StateEtwStoppingResumeTimer                 = 3116,
    Psm20StateEtwStoppingTimerAndQueueingHubResetInDisconnected = 3117,
    Psm20StateEtwStoppingTimerOnDisconnect           = 3118,
    Psm20StateEtwStoppingTimerOnHubStopSuspendInDisconnected = 3119,
    Psm20StateEtwStoppingTimerOnOverCurrent          = 3120,
    Psm20StateEtwStoppingTimerOnOverCurrentClear     = 3121,
    Psm20StateEtwStoppingTimerOnPortCycle            = 3122,
    Psm20StateEtwStoppingTimerOnResetComplete        = 3123,
    Psm20StateEtwSuspended                           = 3124,
    Psm20StateEtwSuspending                          = 3125,
    Psm20StateEtwWaitingForDebounce                  = 3126,
    Psm20StateEtwWaitingForDisableCompleteOnHubSuspend = 3127,
    Psm20StateEtwWaitingForDisableCompleteWithTimerOnHubSuspend = 3128,
    Psm20StateEtwWaitingForDSMResumeResponse         = 3129,
    Psm20StateEtwWaitingForDSMResumeResponseOnHubStop = 3130,
    Psm20StateEtwWaitingForHubStopSuspend            = 3131,
    Psm20StateEtwWaitingForHubStopSuspendInDisconnected = 3132,
    Psm20StateEtwWaitingForHubStopSuspendOnAttachFailure = 3133,
    Psm20StateEtwWaitingForHubStopSuspendOrTimer     = 3134,
    Psm20StateEtwWaitingForOldDeviceToDetach         = 3135,
    Psm20StateEtwWaitingForOverCurrentClear          = 3136,
    Psm20StateEtwWaitingForResetComplete             = 3137,
    Psm20StateEtwWaitingForResetTimerToFlush         = 3138,
    Psm20StateEtwWaitingForResumeComplete            = 3139,
    Psm20StateEtwWaitingForResumeRecoveryTimer       = 3140,
    Psm20StateEtwWaitingForResumeTimerToFlush        = 3141,
    Psm20StateEtwWaitingForTimerOnResetInDisconnected = 3142,
    Psm20StateEtwWaitingForTimerToFlushOnDisconnect  = 3143,
    Psm20StateEtwWaitingForTimerToFlushOnHubStopSuspend = 3144,
    Psm20StateEtwWaitingForTimerToFlushOnHubStopSuspendInDisconnected = 3145,
    Psm20StateEtwWaitingForTimerToFlushOnOverCurrent = 3146,
    Psm20StateEtwWaitingForTimerToFlushOnOverCurrentClear = 3147,
    Psm20StateEtwWaitingForTimerToFlushOnPortCycle   = 3148,
    Psm20StateEtwWaitingForUserResetOnOverCurrent    = 3149,
    Psm20StateEtwWaitingToBeDeleted                  = 3150,
    Psm20StateEtwAckingPortChange                    = 3151,
    Psm20StateEtwCheckIfThereIsAPortChange           = 3152,
    Psm20StateEtwCheckIfThereIsAPortChangeOnPortStatus = 3153,
    Psm20StateEtwEnablingInterruptsAndGettingPortEvent = 3154,
    Psm20StateEtwGettingPortStatus                   = 3155,
    Psm20StateEtwInitializingCumulativePortChangeBits = 3156,
    Psm20StateEtwIssuingHubResetOnControlTransferFailure = 3157,
    Psm20StateEtwQueueingEnableInterruptTransferOnPortChange = 3158,
    Psm20StateEtwWaitingForPortChangeEvent           = 3159,
    Psm20StateEtwAcquiringPortReferenceOnStart       = 3160,
    Psm20StateEtwGettingPortLostChangesOnStart       = 3161,
    Psm20StateEtwGettingPortStatusOnStart            = 3162,
    Psm20StateEtwIssuingHubResetOnControlTransferFailureOnStart = 3163,
    Psm20StateEtwPoweringOnPortOnStart               = 3164,
    Psm20StateEtwQueueingEnableInterruptTransferOnStart = 3165,
    Psm20StateEtwWaitingForPortPowerOnTimerOnStart   = 3166,
    Psm20StateEtwAckingConnectChangeOnResume         = 3167,
    Psm20StateEtwAckingPortChangeWhileWaitingForReconnect = 3258,
    Psm20StateEtwAcquiringPortReferenceOnResume      = 3168,
    Psm20StateEtwAcquiringPortReferenceOnResumeInFailure = 3169,
    Psm20StateEtwCheckIfThereIsAPortChangeOnPortStatusWhileWaitingForReconnect = 3259,
    Psm20StateEtwCheckingIfConnectBitChangeSetOnResume = 3170,
    Psm20StateEtwCheckingIfConnectBitIsOneOnChangeWhileWaitingForReconnect = 3260,
    Psm20StateEtwCheckingIfDeviceDisconnectedOnResume = 3171,
    Psm20StateEtwCheckingIfDeviceDisconnectedOnResumeAfterWaiting = 3172,
    Psm20StateEtwEnablingInterruptsWaitingForReconnect = 3173,
    Psm20StateEtwGettingPortLostChangesOnStartOnResume = 3174,
    Psm20StateEtwGettingPortStatusOnResume           = 3175,
    Psm20StateEtwGettingPortStatusOnResumeAfterWaiting = 3176,
    Psm20StateEtwGettingPortStatusWhileWaitingForReconnect = 3261,
    Psm20StateEtwInitializingCumulativePortChangeBitsWhileWaitingForReconnect = 3262,
    Psm20StateEtwIssuingHubResetOnControlTransferFailureOnResume = 3177,
    Psm20StateEtwPoweringOnPortOnResume              = 3178,
    Psm20StateEtwQueueingEnableInterruptTransferOnResume = 3179,
    Psm20StateEtwQueueingStateDisabledOnFailure      = 3180,
    Psm20StateEtwResettingHubOnFailureWhileWaitingForReconnect = 3263,
    Psm20StateEtwReturningHubStopOnResume            = 3181,
    Psm20StateEtwStartingTimerForAllowingReconnect   = 3182,
    Psm20StateEtwStoppingReconnectTimerOnDeviceConnect = 3264,
    Psm20StateEtwStoppingReconnectTimerOnHubStop     = 3183,
    Psm20StateEtwWaitingForDeviceToReconnect         = 3184,
    Psm20StateEtwWaitingForPortPowerOnTimerOnResume  = 3185,
    Psm20StateEtwWaitingForReconnectTimerToFlushOnHubStop = 3186,
    Psm20StateEtwAckingPortChangeInSuspended         = 3187,
    Psm20StateEtwAcquiringPowerReferenceOnHubS0IdleInD3Cold = 3188,
    Psm20StateEtwCheckIfThereIsAPortChangeAfterInitialGetPortStatusOnSuspended = 3189,
    Psm20StateEtwCheckIfThereIsAPortChangeOnSuspended = 3190,
    Psm20StateEtwCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold = 3191,
    Psm20StateEtwCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold = 3192,
    Psm20StateEtwCheckingIfDeviceIsConnectedOnPortChangeInD3Cold = 3193,
    Psm20StateEtwCheckingIfDeviceIsConnectedOnPortChangeInD3ColdOnResume = 3194,
    Psm20StateEtwCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume = 3195,
    Psm20StateEtwCheckingIsD3ColdIsEnabled           = 3196,
    Psm20StateEtwGettingPortChangeEventInSuspended   = 3197,
    Psm20StateEtwGettingPortChangeOnResumeFromD3ColdSuspended = 3198,
    Psm20StateEtwGettingPortStatusInSuspended        = 3199,
    Psm20StateEtwInitializingCumulativePortChangeBitsInSuspended = 3200,
    Psm20StateEtwIssuingHubResetOnControlTransferFailureInSuspended = 3201,
    Psm20StateEtwPoweredOffOnHubSuspendFromD3Cold    = 3202,
    Psm20StateEtwPoweringOnResumeFromD3ColdSuspended = 3203,
    Psm20StateEtwQueueingDisabledToDSMOnReconnectFromD3Cold = 3204,
    Psm20StateEtwQueueingEnableInterruptTransferOnPortChangeForSuspended = 3205,
    Psm20StateEtwQueueingPortStateDisabledOnHubResumeInD3Cold = 3206,
    Psm20StateEtwQueueingResumeToDSMFromSuspended    = 3207,
    Psm20StateEtwReEnablingInterruptsOnConnectChangeInSuspended = 3208,
    Psm20StateEtwReEnablingInterruptsOnErrorInSuspended = 3209,
    Psm20StateEtwReEnablingInterruptsOnOverCurrentClearedInSuspended = 3210,
    Psm20StateEtwReEnablingInterruptsOnOverCurrentInSuspended = 3211,
    Psm20StateEtwReleasingPowerReferenceOnHubS0IdleInD3Cold = 3212,
    Psm20StateEtwReturningHubStopFromSuspendedInD3Cold = 3213,
    Psm20StateEtwReturningHubSuspendFromSuspendedInD3Cold = 3214,
    Psm20StateEtwReturningPortConnectChangeFromSuspended = 3215,
    Psm20StateEtwReturningPortErrorFromSuspended     = 3216,
    Psm20StateEtwReturningPortOverCurrentClearedFromSuspended = 3217,
    Psm20StateEtwReturningPortOverCurrentFromSuspended = 3218,
    Psm20StateEtwReturningPortResumedFromD3Cold      = 3219,
    Psm20StateEtwReturningPortResumedFromSuspended   = 3220,
    Psm20StateEtwStartingDebounceTimerOnResumeFromD3Cold = 3221,
    Psm20StateEtwStartingDebounceTimerOnResumeFromD3ColdOnResume = 3222,
    Psm20StateEtwStartingTimerForAllowingReconnectOnResumingFromD3Cold = 3223,
    Psm20StateEtwStoppingDebounceTimerInD3Cold       = 3224,
    Psm20StateEtwStoppingDebounceTimerInD3ColdOnResume = 3225,
    Psm20StateEtwStoppingReconnectTimerOnDeviceReAttachAfterD3Cold = 3226,
    Psm20StateEtwStoppingReconnectTimerOnHubStopInD3Cold = 3227,
    Psm20StateEtwStoppingReconnectTimerOnOvercurrentClearedInD3Cold = 3228,
    Psm20StateEtwStoppingReconnectTimerOnOvercurrentInD3Cold = 3229,
    Psm20StateEtwWaitingForDebounceTimerOnReconnectInD3Cold = 3230,
    Psm20StateEtwWaitingForDebounceTimerOnReconnectInD3ColdOnResume = 3231,
    Psm20StateEtwWaitingForDebounceTimerToFlushOnHubStop = 3232,
    Psm20StateEtwWaitingForDebounceTimerToFlushOnHubStopOnResume = 3233,
    Psm20StateEtwWaitingForDebounceTimerToFlushOnHubSuspend = 3234,
    Psm20StateEtwWaitingForDebounceTimerToFlushOnPortChangeInD3Cold = 3235,
    Psm20StateEtwWaitingForDebounceTimerToFlushOnPortChangeInD3ColdOnResume = 3236,
    Psm20StateEtwWaitingForDeviceRequestResumeOnD3Cold = 3237,
    Psm20StateEtwWaitingForDeviceToReconnectOnResumeFromD3Cold = 3238,
    Psm20StateEtwWaitingForPortChangeEventInD3Cold   = 3240,
    Psm20StateEtwWaitingForPortChangeEventInSuspended = 3241,
    Psm20StateEtwWaitingForReconnectTimerToFlushOnHubStopInD3Cold = 3242,
    Psm20StateEtwWaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold = 3243,
    Psm20StateEtwWaitingForReconnectTimerToFlushOnOvercurrentInD3Cold = 3244,
    Psm20StateEtwWaitingForReconnectTimerToFlushOnReattachAfterD3Cold = 3245,
    Psm20StateEtwWaitingForResumeRecoveryTimerOnResumeInSuspended = 3246,
    Psm20StateEtwAckingPortChangeInInWaitingForStopSuspend = 3247,
    Psm20StateEtwCheckIfThereIsAPortChangeInInWaitingForStopSuspend = 3248,
    Psm20StateEtwGettingPortStatusInWaitingForStopSuspend = 3249,
    Psm20StateEtwQueueingEnableInterruptTransferInWaitingForStopSuspend = 3250,
    Psm20StateEtwQueueingPortEventFailureToDSM       = 3251,
    Psm20StateEtwWaitingForDevicePortEvents          = 3252,
    Psm20StateEtwAckingPortChangeInInWaitingForStopSuspendInDisconnected = 3253,
    Psm20StateEtwCheckIfThereIsAPortChangeInInWaitingForStopSuspendInDisconnected = 3254,
    Psm20StateEtwGettingPortStatusInWaitingForStopSuspendInDisconnected = 3255,
    Psm20StateEtwQueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected = 3256,
    Psm20StateEtwWaitingForDevicePortEventsInDisconnected = 3257,
} PSM20_STATE_ETW, *PPSM20_STATE_ETW;



//
// Definitions for State Entry Functions 
//
STATE_ENTRY_FUNCTION          HUBPSM20_AcquiringInterruptReferenceOnHSMInDisabled;

STATE_ENTRY_FUNCTION          HUBPSM20_AcquiringInterruptReferenceOnHSMInEnabled;

STATE_ENTRY_FUNCTION          HUBPSM20_AcquiringInterruptReferenceOnResuming;

STATE_ENTRY_FUNCTION          HUBPSM20_AcquiringPowerReferenceFromPoweredOffDisabledInS0;

STATE_ENTRY_FUNCTION          HUBPSM20_AcquiringPowerReferenceFromPoweredOffDisconnectedInS0;

STATE_ENTRY_FUNCTION          HUBPSM20_AcquiringPowerReferenceFromPoweredOffSuspendedInS0;

STATE_ENTRY_FUNCTION          HUBPSM20_CancellingDisableOnHubStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_CancellingDisablingPortOnPortCycleOnHubStop;

STATE_ENTRY_FUNCTION          HUBPSM20_CancellingDisablingPortOnPortCycleWithTimerOnHubStop;

STATE_ENTRY_FUNCTION          HUBPSM20_CancellingQueryPortStatus;

STATE_ENTRY_FUNCTION          HUBPSM20_CancellingResetOnSurpriseRemove;

STATE_ENTRY_FUNCTION          HUBPSM20_CancellingSetPortPower;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIfConnectBitOne;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIfOvercurrentBitOne;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIfPersistentOvercurrent;

STATE_ENTRY_FUNCTION          HUBPSM20_ConnectedDisabled;

STATE_ENTRY_FUNCTION          HUBPSM20_ConnectedEnabled;

STATE_ENTRY_FUNCTION          HUBPSM20_CreatingDevice;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceFromPortOnCycleOnHubStop;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceFromPortOnDisconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceFromPortOnDisconnectWithTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceFromPortOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceFromPortOnOverCurrentClear;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceFromPortOnOverCurrentClearWithTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceFromPortOnOverCurrentWithTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceFromPortOnPortCycle;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceOnCleanup;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceOnDisableAndCycle;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceOnHubReset;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceOnPortCycleWithTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_DetachingDeviceOnPortDisableAndCycleWithTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_DisablingOnHubSuspendWithTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_DisablingPortBeforeConnecting;

STATE_ENTRY_FUNCTION          HUBPSM20_DisablingPortOnCycle;

STATE_ENTRY_FUNCTION          HUBPSM20_DisablingPortOnDeviceRequest;

STATE_ENTRY_FUNCTION          HUBPSM20_DisablingPortOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_DisablingPortOnPortCycleWithTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_DisablingPortOnTimeOut;

STATE_ENTRY_FUNCTION          HUBPSM20_Disconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnAcquringReferenceOnHubResume;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnConnectedDisabled;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnConnectedEnabled;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnIssuingResetComplete;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnResettingEnabled;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnResettingPort;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnStoppingTimerOnDisconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnStoppingTimerOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnStoppingTimerOnOverCurrentClear;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnStoppingTimerOnPortCycle;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnWaitingForCompanionPort;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnWaitingForDebounce;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnWaitingForDSMResumeResponse;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnWaitingForOldDeviceToCleanup;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnWaitingForOverCurrentClear;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnWaitingForResumeComplete;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnWaitingForResumeRecoveryTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_ErrorOnWaitingForResumeTimerToFlush;

STATE_ENTRY_FUNCTION          HUBPSM20_FlushingStaleDeviceEvents;

STATE_ENTRY_FUNCTION          HUBPSM20_FlushingUserResetForOverCurrentOnHubStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_FlushingUserResetForOverCurrentOnOverCurrentClear;

STATE_ENTRY_FUNCTION          HUBPSM20_InitiatingResetPort;

STATE_ENTRY_FUNCTION          HUBPSM20_InitiatingResume;

STATE_ENTRY_FUNCTION          HUBPSM20_IsItBootDevice;

STATE_ENTRY_FUNCTION          HUBPSM20_IsOldDevicePresent;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingAttachDeviceToPort;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingDetachDeviceOnCycleFromPoweredOffDisabledOrSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingDisabledToDSMFromDisabled;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubReset;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubResetFromDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubResetOnDisableFailure;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubResetWhenWaitingForDSMResumeResponse;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubResetWhileHubSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubResetWithPendingTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubResetWithTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubResetWithTimerOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubResetWithTimerWhileHubSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingPortDisabledToDevice;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingPortDisableFailedToDevice;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingReAttachDeviceToBootDevice;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingResumedToDeviceSM;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingSuspendedToDeviceSM;

STATE_ENTRY_FUNCTION          HUBPSM20_NotifyingUserAboutPersistentOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM20_PoweredOffDisabled;

STATE_ENTRY_FUNCTION          HUBPSM20_PoweredOffDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_PoweredOffSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_QueryingPortStatusOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingPortFailureToDSMOnSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingPortResumeFailedDueToPendingSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingPortResumeTimedOut;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingStateDisabledOnHubResumeFromDisabled;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingStateDisabledToDeviceSM;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingStateDisabledToDeviceSMOnResetHub;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingStateEnabledToDeviceSM;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingStateSuspendedToDeviceSM;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingStateSuspendedToDeviceSMOnIgnoreError;

STATE_ENTRY_FUNCTION          HUBPSM20_ReleasingInterruptReferenceAndIssuingPortResetTimedOutToDSM;

STATE_ENTRY_FUNCTION          HUBPSM20_ReleasingInterruptReferenceAndStoppingTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_ReleasingInterruptReferenceOnResetTransferFailure;

STATE_ENTRY_FUNCTION          HUBPSM20_ReleasingInterruptReferenceOnSuccessfulReset;

STATE_ENTRY_FUNCTION          HUBPSM20_ReleasingPowerReferenceonPoweredOffDisabled;

STATE_ENTRY_FUNCTION          HUBPSM20_ReleasingPowerReferenceonPoweredOffDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_ReleasingResetReferenceOnDeviceRemoval;

STATE_ENTRY_FUNCTION          HUBPSM20_ResettingEnabled;

STATE_ENTRY_FUNCTION          HUBPSM20_SettingPortPowerOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingAndAcquiringReferenceOnHubResume;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingAndAcquiringReferenceOnHubResumeFromDisabled;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingAndAcquiringReferenceOnHubStart;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingAndAcquiringReferenceOnHubStartFromDisabled;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingAndAcquiringReferenceOnWarmResume;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingOverCurrentTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingResetTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingResumeRecoveryTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingResumeTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingResumeTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingTimerAndQueueingHubResetInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingTimerOnDisconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingTimerOnHubStopSuspendInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingTimerOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingTimerOnOverCurrentClear;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingTimerOnPortCycle;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingTimerOnResetComplete;

STATE_ENTRY_FUNCTION          HUBPSM20_Suspended;

STATE_ENTRY_FUNCTION          HUBPSM20_Suspending;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDebounce;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDisableCompleteOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDisableCompleteWithTimerOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDSMResumeResponse;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDSMResumeResponseOnHubStop;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForHubStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForHubStopSuspendInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForHubStopSuspendOnAttachFailure;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForHubStopSuspendOrTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForOldDeviceToDetach;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForOverCurrentClear;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForResetComplete;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForResetTimerToFlush;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForResumeComplete;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForResumeRecoveryTimer;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForResumeTimerToFlush;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForTimerOnResetInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForTimerToFlushOnDisconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForTimerToFlushOnHubStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForTimerToFlushOnHubStopSuspendInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForTimerToFlushOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForTimerToFlushOnOverCurrentClear;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForTimerToFlushOnPortCycle;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForUserResetOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingToBeDeleted;

STATE_ENTRY_FUNCTION          HUBPSM20_AckingPortChange;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckIfThereIsAPortChange;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckIfThereIsAPortChangeOnPortStatus;

STATE_ENTRY_FUNCTION          HUBPSM20_EnablingInterruptsAndGettingPortEvent;

STATE_ENTRY_FUNCTION          HUBPSM20_GettingPortStatus;

STATE_ENTRY_FUNCTION          HUBPSM20_InitializingCumulativePortChangeBits;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubResetOnControlTransferFailure;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingEnableInterruptTransferOnPortChange;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForPortChangeEvent;

STATE_ENTRY_FUNCTION          HUBPSM20_AcquiringPortReferenceOnStart;

STATE_ENTRY_FUNCTION          HUBPSM20_GettingPortLostChangesOnStart;

STATE_ENTRY_FUNCTION          HUBPSM20_GettingPortStatusOnStart;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubResetOnControlTransferFailureOnStart;

STATE_ENTRY_FUNCTION          HUBPSM20_PoweringOnPortOnStart;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingEnableInterruptTransferOnStart;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForPortPowerOnTimerOnStart;

STATE_ENTRY_FUNCTION          HUBPSM20_AckingConnectChangeOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_AckingPortChangeWhileWaitingForReconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_AcquiringPortReferenceOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_AcquiringPortReferenceOnResumeInFailure;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckIfThereIsAPortChangeOnPortStatusWhileWaitingForReconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIfConnectBitChangeSetOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIfConnectBitIsOneOnChangeWhileWaitingForReconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIfDeviceDisconnectedOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIfDeviceDisconnectedOnResumeAfterWaiting;

STATE_ENTRY_FUNCTION          HUBPSM20_EnablingInterruptsWaitingForReconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_GettingPortLostChangesOnStartOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_GettingPortStatusOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_GettingPortStatusOnResumeAfterWaiting;

STATE_ENTRY_FUNCTION          HUBPSM20_GettingPortStatusWhileWaitingForReconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_InitializingCumulativePortChangeBitsWhileWaitingForReconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubResetOnControlTransferFailureOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_PoweringOnPortOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingEnableInterruptTransferOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingStateDisabledOnFailure;

STATE_ENTRY_FUNCTION          HUBPSM20_ResettingHubOnFailureWhileWaitingForReconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_ReturningHubStopOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingTimerForAllowingReconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingReconnectTimerOnDeviceConnect;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingReconnectTimerOnHubStop;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDeviceToReconnect;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForPortPowerOnTimerOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForReconnectTimerToFlushOnHubStop;

STATE_ENTRY_FUNCTION          HUBPSM20_AckingPortChangeInSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_AcquiringPowerReferenceOnHubS0IdleInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckIfThereIsAPortChangeAfterInitialGetPortStatusOnSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckIfThereIsAPortChangeOnSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIfDeviceIsConnectedAfterHubResumeInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIfDeviceIsConnectedOnConnectChangeForD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangeInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangeInD3ColdOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckingIsD3ColdIsEnabled;

STATE_ENTRY_FUNCTION          HUBPSM20_GettingPortChangeEventInSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_GettingPortChangeOnResumeFromD3ColdSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_GettingPortStatusInSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_InitializingCumulativePortChangeBitsInSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_IssuingHubResetOnControlTransferFailureInSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_PoweredOffOnHubSuspendFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_PoweringOnResumeFromD3ColdSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingDisabledToDSMOnReconnectFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingEnableInterruptTransferOnPortChangeForSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingPortStateDisabledOnHubResumeInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingResumeToDSMFromSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_ReEnablingInterruptsOnConnectChangeInSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_ReEnablingInterruptsOnErrorInSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_ReEnablingInterruptsOnOverCurrentClearedInSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_ReEnablingInterruptsOnOverCurrentInSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_ReleasingPowerReferenceOnHubS0IdleInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_ReturningHubStopFromSuspendedInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_ReturningHubSuspendFromSuspendedInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_ReturningPortConnectChangeFromSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_ReturningPortErrorFromSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_ReturningPortOverCurrentClearedFromSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_ReturningPortOverCurrentFromSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_ReturningPortResumedFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_ReturningPortResumedFromSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingDebounceTimerOnResumeFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingDebounceTimerOnResumeFromD3ColdOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_StartingTimerForAllowingReconnectOnResumingFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingDebounceTimerInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingDebounceTimerInD3ColdOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingReconnectTimerOnDeviceReAttachAfterD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingReconnectTimerOnHubStopInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingReconnectTimerOnOvercurrentClearedInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_StoppingReconnectTimerOnOvercurrentInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDebounceTimerOnReconnectInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDebounceTimerOnReconnectInD3ColdOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDebounceTimerToFlushOnHubStop;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDebounceTimerToFlushOnHubStopOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDebounceTimerToFlushOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDebounceTimerToFlushOnPortChangeInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDebounceTimerToFlushOnPortChangeInD3ColdOnResume;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDeviceRequestResumeOnD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDeviceToReconnectOnResumeFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForPortChangeEventInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForPortChangeEventInSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForReconnectTimerToFlushOnHubStopInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForReconnectTimerToFlushOnOvercurrentInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForReconnectTimerToFlushOnReattachAfterD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForResumeRecoveryTimerOnResumeInSuspended;

STATE_ENTRY_FUNCTION          HUBPSM20_AckingPortChangeInInWaitingForStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckIfThereIsAPortChangeInInWaitingForStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_GettingPortStatusInWaitingForStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingEnableInterruptTransferInWaitingForStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingPortEventFailureToDSM;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDevicePortEvents;

STATE_ENTRY_FUNCTION          HUBPSM20_AckingPortChangeInInWaitingForStopSuspendInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_CheckIfThereIsAPortChangeInInWaitingForStopSuspendInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_GettingPortStatusInWaitingForStopSuspendInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_QueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM20_WaitingForDevicePortEventsInDisconnected;




//
// Sub State Entries for the states in the State Machine
//


SUBSM_ENTRY Psm20SubSmConnectedDisabled[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmConnectedEnabled[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmDisconnected[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmIssuingHubReset[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForDevicePortEvents },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmIssuingHubResetFromDisconnected[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForDevicePortEventsInDisconnected },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmResettingEnabled[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmStartingAndAcquiringReferenceOnHubResume[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StatePoweringOnPortOnResume },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmStartingAndAcquiringReferenceOnHubResumeFromDisabled[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StatePoweringOnPortOnResume },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmStartingAndAcquiringReferenceOnHubStart[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StatePoweringOnPortOnStart },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmStartingAndAcquiringReferenceOnHubStartFromDisabled[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StatePoweringOnPortOnStart },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmStartingAndAcquiringReferenceOnWarmResume[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateGettingPortStatusOnResume },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmSuspended[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEventInSuspended },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForDebounce[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForDSMResumeResponse[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForHubStopSuspend[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForDevicePortEvents },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForHubStopSuspendInDisconnected[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForDevicePortEventsInDisconnected },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForHubStopSuspendOnAttachFailure[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForDevicePortEventsInDisconnected },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForHubStopSuspendOrTimer[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForDevicePortEvents },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForOldDeviceToDetach[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForOverCurrentClear[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForResetComplete[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForResetTimerToFlush[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForResumeComplete[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForResumeRecoveryTimer[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForResumeTimerToFlush[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForTimerOnResetInDisconnected[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForDevicePortEventsInDisconnected },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForTimerToFlushOnDisconnect[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForTimerToFlushOnOverCurrent[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForTimerToFlushOnOverCurrentClear[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForTimerToFlushOnPortCycle[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForUserResetOnOverCurrent[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmGettingPortChangeOnResumeFromD3ColdSuspended[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateGettingPortStatusOnResume },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmPoweringOnResumeFromD3ColdSuspended[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StatePoweringOnPortOnResume },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForDebounceTimerOnReconnectInD3Cold[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForDebounceTimerOnReconnectInD3ColdOnResume[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForDeviceRequestResumeOnD3Cold[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForDeviceToReconnectOnResumeFromD3Cold[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


SUBSM_ENTRY Psm20SubSmWaitingForPortChangeEventInD3Cold[] = {
    // SubSmFilter, SubSmInitialState
    { Psm20SubSmFlagAny , Psm20StateWaitingForPortChangeEvent },
    { Psm20SubSmFlagNone , Psm20StateNull }
};


//
// State Entries for the states in the State Machine
//
STATE_ENTRY   Psm20StateEntryAcquiringInterruptReferenceOnHSMInDisabled = {
    // State ETW Name
    Psm20StateEtwAcquiringInterruptReferenceOnHSMInDisabled,
    // State Entry Function
    HUBPSM20_AcquiringInterruptReferenceOnHSMInDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateInitiatingResetPort },
        { PsmEventOperationFailure ,    Psm20StateIssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAcquiringInterruptReferenceOnHSMInEnabled = {
    // State ETW Name
    Psm20StateEtwAcquiringInterruptReferenceOnHSMInEnabled,
    // State Entry Function
    HUBPSM20_AcquiringInterruptReferenceOnHSMInEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateInitiatingResetPort },
        { PsmEventOperationFailure ,    Psm20StateIssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAcquiringInterruptReferenceOnResuming = {
    // State ETW Name
    Psm20StateEtwAcquiringInterruptReferenceOnResuming,
    // State Entry Function
    HUBPSM20_AcquiringInterruptReferenceOnResuming,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateInitiatingResume },
        { PsmEventOperationFailure ,    Psm20StateQueueingPortResumeFailedDueToPendingSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAcquiringPowerReferenceFromPoweredOffDisabledInS0 = {
    // State ETW Name
    Psm20StateEtwAcquiringPowerReferenceFromPoweredOffDisabledInS0,
    // State Entry Function
    HUBPSM20_AcquiringPowerReferenceFromPoweredOffDisabledInS0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAcquiringPowerReferenceFromPoweredOffDisconnectedInS0 = {
    // State ETW Name
    Psm20StateEtwAcquiringPowerReferenceFromPoweredOffDisconnectedInS0,
    // State Entry Function
    HUBPSM20_AcquiringPowerReferenceFromPoweredOffDisconnectedInS0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAcquiringPowerReferenceFromPoweredOffSuspendedInS0 = {
    // State ETW Name
    Psm20StateEtwAcquiringPowerReferenceFromPoweredOffSuspendedInS0,
    // State Entry Function
    HUBPSM20_AcquiringPowerReferenceFromPoweredOffSuspendedInS0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCancellingDisableOnHubStopSuspend = {
    // State ETW Name
    Psm20StateEtwCancellingDisableOnHubStopSuspend,
    // State Entry Function
    HUBPSM20_CancellingDisableOnHubStopSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventTransferSuccess ,     Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCancellingDisablingPortOnPortCycleOnHubStop = {
    // State ETW Name
    Psm20StateEtwCancellingDisablingPortOnPortCycleOnHubStop,
    // State Entry Function
    HUBPSM20_CancellingDisablingPortOnPortCycleOnHubStop,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventTransferSuccess ,     Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCancellingDisablingPortOnPortCycleWithTimerOnHubStop = {
    // State ETW Name
    Psm20StateEtwCancellingDisablingPortOnPortCycleWithTimerOnHubStop,
    // State Entry Function
    HUBPSM20_CancellingDisablingPortOnPortCycleWithTimerOnHubStop,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateStoppingTimerOnHubStopSuspendInDisconnected },
        { PsmEventTransferSuccess ,     Psm20StateStoppingTimerOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCancellingQueryPortStatus = {
    // State ETW Name
    Psm20StateEtwCancellingQueryPortStatus,
    // State Entry Function
    HUBPSM20_CancellingQueryPortStatus,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventTransferSuccess ,     Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCancellingResetOnSurpriseRemove = {
    // State ETW Name
    Psm20StateEtwCancellingResetOnSurpriseRemove,
    // State Entry Function
    HUBPSM20_CancellingResetOnSurpriseRemove,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm20StateWaitingForHubStopSuspend },
        { PsmEventTransferSuccess ,     Psm20StateWaitingForHubStopSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCancellingSetPortPower = {
    // State ETW Name
    Psm20StateEtwCancellingSetPortPower,
    // State Entry Function
    HUBPSM20_CancellingSetPortPower,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventTransferSuccess ,     Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIfConnectBitOne = {
    // State ETW Name
    Psm20StateEtwCheckingIfConnectBitOne,
    // State Entry Function
    HUBPSM20_CheckingIfConnectBitOne,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateWaitingForDebounce },
        { PsmEventNo ,                  Psm20StateDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIfOvercurrentBitOne = {
    // State ETW Name
    Psm20StateEtwCheckingIfOvercurrentBitOne,
    // State Entry Function
    HUBPSM20_CheckingIfOvercurrentBitOne,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateStartingOverCurrentTimer },
        { PsmEventNo ,                  Psm20StateCheckingIfConnectBitOne },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIfPersistentOvercurrent = {
    // State ETW Name
    Psm20StateEtwCheckingIfPersistentOvercurrent,
    // State Entry Function
    HUBPSM20_CheckingIfPersistentOvercurrent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateNotifyingUserAboutPersistentOverCurrent },
        { PsmEventNo ,                  Psm20StateSettingPortPowerOnOverCurrent },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryConnectedDisabled = {
    // State ETW Name
    Psm20StateEtwConnectedDisabled,
    // State Entry Function
    HUBPSM20_ConnectedDisabled,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    Psm20SubSmConnectedDisabled,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortError ,           Psm20StateErrorOnConnectedDisabled },
        { PsmEventPortDisabled ,        Psm20StateErrorOnConnectedDisabled },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnConnectedDisabled },
        { PsmEventPortResumed ,         Psm20StateErrorOnConnectedDisabled },
        { PsmEventDeviceRequestDisable ,Psm20StateIssuingDisabledToDSMFromDisabled },
        { PsmEventDeviceRequestCycle ,  Psm20StateDetachingDeviceFromPortOnPortCycle },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventHubSuspend ,          Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventDeviceRequestReset ,  Psm20StateAcquiringInterruptReferenceOnHSMInDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryConnectedEnabled = {
    // State ETW Name
    Psm20StateEtwConnectedEnabled,
    // State Entry Function
    HUBPSM20_ConnectedEnabled,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm20SubSmConnectedEnabled,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventPortError ,           Psm20StateErrorOnConnectedEnabled },
        { PsmEventPortDisabled ,        Psm20StateErrorOnConnectedEnabled },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnConnectedEnabled },
        { PsmEventPortResumed ,         Psm20StateErrorOnConnectedEnabled },
        { PsmEventDeviceRequestDisable ,Psm20StateDisablingPortOnDeviceRequest },
        { PsmEventDeviceRequestSuspend ,Psm20StateSuspending },
        { PsmEventDevicePrepareForHibernation ,Psm20StateIssuingSuspendedToDeviceSM },
        { PsmEventDeviceRequestReset ,  Psm20StateAcquiringInterruptReferenceOnHSMInEnabled },
        { PsmEventDeviceRequestCycle ,  Psm20StateDetachingDeviceOnDisableAndCycle },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCreatingDevice = {
    // State ETW Name
    Psm20StateEtwCreatingDevice,
    // State Entry Function
    HUBPSM20_CreatingDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForDebounce },
        { PsmEventOperationSuccess ,    Psm20StateIssuingAttachDeviceToPort },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceFromPortOnCycleOnHubStop = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceFromPortOnCycleOnHubStop,
    // State Entry Function
    HUBPSM20_DetachingDeviceFromPortOnCycleOnHubStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceFromPortOnDisconnect = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceFromPortOnDisconnect,
    // State Entry Function
    HUBPSM20_DetachingDeviceFromPortOnDisconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateCheckingIfConnectBitOne },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceFromPortOnDisconnectWithTimer = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceFromPortOnDisconnectWithTimer,
    // State Entry Function
    HUBPSM20_DetachingDeviceFromPortOnDisconnectWithTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateStoppingTimerOnDisconnect },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceFromPortOnOverCurrent = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceFromPortOnOverCurrent,
    // State Entry Function
    HUBPSM20_DetachingDeviceFromPortOnOverCurrent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateStartingOverCurrentTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceFromPortOnOverCurrentClear = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceFromPortOnOverCurrentClear,
    // State Entry Function
    HUBPSM20_DetachingDeviceFromPortOnOverCurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateSettingPortPowerOnOverCurrent },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceFromPortOnOverCurrentClearWithTimer = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceFromPortOnOverCurrentClearWithTimer,
    // State Entry Function
    HUBPSM20_DetachingDeviceFromPortOnOverCurrentClearWithTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateStoppingTimerOnOverCurrentClear },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceFromPortOnOverCurrentWithTimer = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceFromPortOnOverCurrentWithTimer,
    // State Entry Function
    HUBPSM20_DetachingDeviceFromPortOnOverCurrentWithTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateStoppingTimerOnOverCurrent },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceFromPortOnPortCycle = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceFromPortOnPortCycle,
    // State Entry Function
    HUBPSM20_DetachingDeviceFromPortOnPortCycle,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForOldDeviceToDetach },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceOnCleanup = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceOnCleanup,
    // State Entry Function
    HUBPSM20_DetachingDeviceOnCleanup,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm20StateWaitingToBeDeleted },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceOnDisableAndCycle = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceOnDisableAndCycle,
    // State Entry Function
    HUBPSM20_DetachingDeviceOnDisableAndCycle,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateDisablingPortOnCycle },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceOnHubReset = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceOnHubReset,
    // State Entry Function
    HUBPSM20_DetachingDeviceOnHubReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReleasingResetReferenceOnDeviceRemoval },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceOnPortCycleWithTimer = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceOnPortCycleWithTimer,
    // State Entry Function
    HUBPSM20_DetachingDeviceOnPortCycleWithTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateStoppingTimerOnDisconnect },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDetachingDeviceOnPortDisableAndCycleWithTimer = {
    // State ETW Name
    Psm20StateEtwDetachingDeviceOnPortDisableAndCycleWithTimer,
    // State Entry Function
    HUBPSM20_DetachingDeviceOnPortDisableAndCycleWithTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateDisablingPortOnPortCycleWithTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDisablingOnHubSuspendWithTimer = {
    // State ETW Name
    Psm20StateEtwDisablingOnHubSuspendWithTimer,
    // State Entry Function
    HUBPSM20_DisablingOnHubSuspendWithTimer,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetWithTimerOnHubSuspend },
        { PsmEventTransferSuccess ,     Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDisablingPortBeforeConnecting = {
    // State ETW Name
    Psm20StateEtwDisablingPortBeforeConnecting,
    // State Entry Function
    HUBPSM20_DisablingPortBeforeConnecting,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetFromDisconnected },
        { PsmEventTransferSuccess ,     Psm20StateWaitingForDebounce },
        { PsmEventHubSuspend ,          Psm20StateCancellingDisableOnHubStopSuspend },
        { PsmEventHubStop ,             Psm20StateCancellingDisableOnHubStopSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDisablingPortOnCycle = {
    // State ETW Name
    Psm20StateEtwDisablingPortOnCycle,
    // State Entry Function
    HUBPSM20_DisablingPortOnCycle,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetFromDisconnected },
        { PsmEventHubSuspend ,          Psm20StateWaitingForDisableCompleteOnHubSuspend },
        { PsmEventHubStop ,             Psm20StateCancellingDisablingPortOnPortCycleOnHubStop },
        { PsmEventTransferSuccess ,     Psm20StateWaitingForOldDeviceToDetach },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDisablingPortOnDeviceRequest = {
    // State ETW Name
    Psm20StateEtwDisablingPortOnDeviceRequest,
    // State Entry Function
    HUBPSM20_DisablingPortOnDeviceRequest,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferSuccess ,     Psm20StateIssuingPortDisabledToDevice },
        { PsmEventTransferFailure ,     Psm20StateIssuingPortDisableFailedToDevice },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDisablingPortOnHubSuspend = {
    // State ETW Name
    Psm20StateEtwDisablingPortOnHubSuspend,
    // State Entry Function
    HUBPSM20_DisablingPortOnHubSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferSuccess ,     Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetOnDisableFailure },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDisablingPortOnPortCycleWithTimer = {
    // State ETW Name
    Psm20StateEtwDisablingPortOnPortCycleWithTimer,
    // State Entry Function
    HUBPSM20_DisablingPortOnPortCycleWithTimer,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateStoppingTimerAndQueueingHubResetInDisconnected },
        { PsmEventHubStop ,             Psm20StateCancellingDisablingPortOnPortCycleWithTimerOnHubStop },
        { PsmEventHubSuspend ,          Psm20StateWaitingForDisableCompleteWithTimerOnHubSuspend },
        { PsmEventTransferSuccess ,     Psm20StateStoppingTimerOnPortCycle },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDisablingPortOnTimeOut = {
    // State ETW Name
    Psm20StateEtwDisablingPortOnTimeOut,
    // State Entry Function
    HUBPSM20_DisablingPortOnTimeOut,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateReleasingInterruptReferenceOnResetTransferFailure },
        { PsmEventTransferSuccess ,     Psm20StateReleasingInterruptReferenceAndIssuingPortResetTimedOutToDSM },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryDisconnected = {
    // State ETW Name
    Psm20StateEtwDisconnected,
    // State Entry Function
    HUBPSM20_Disconnected,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    Psm20SubSmDisconnected,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventDeviceRequestResume , Psm20StateIgnored },
        { PsmEventDeviceRequestSuspend ,Psm20StateIgnored },
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm20StateIgnored },
        { PsmEventDeviceRequestCycle ,  Psm20StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm20StateIgnored },
        { PsmEventDevicePrepareForHibernation ,Psm20StateIgnored },
        { PsmEventPortOverCurrent ,     Psm20StateStartingOverCurrentTimer },
        { PsmEventPortConnectChange ,   Psm20StateCheckingIfConnectBitOne },
        { PsmEventPortOverCurrentCleared ,Psm20StateSettingPortPowerOnOverCurrent },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnDisconnected },
        { PsmEventPortDisabled ,        Psm20StateErrorOnDisconnected },
        { PsmEventPortResumed ,         Psm20StateErrorOnDisconnected },
        { PsmEventPortError ,           Psm20StateErrorOnDisconnected },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventHubSuspend ,          Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnAcquringReferenceOnHubResume = {
    // State ETW Name
    Psm20StateEtwErrorOnAcquringReferenceOnHubResume,
    // State Entry Function
    HUBPSM20_ErrorOnAcquringReferenceOnHubResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortCycleOnError ,    Psm20StateDetachingDeviceFromPortOnPortCycle },
        { PsmEventPortIgnoreError ,     Psm20StateQueueingStateSuspendedToDeviceSMOnIgnoreError },
        { PsmEventPortResetHubOnError , Psm20StateQueueingStateDisabledToDeviceSMOnResetHub },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDetachingDeviceOnDisableAndCycle },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnConnectedDisabled = {
    // State ETW Name
    Psm20StateEtwErrorOnConnectedDisabled,
    // State Entry Function
    HUBPSM20_ErrorOnConnectedDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortIgnoreError ,     Psm20StateConnectedDisabled },
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubReset },
        { PsmEventPortCycleOnError ,    Psm20StateDetachingDeviceFromPortOnPortCycle },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDetachingDeviceOnDisableAndCycle },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnConnectedEnabled = {
    // State ETW Name
    Psm20StateEtwErrorOnConnectedEnabled,
    // State Entry Function
    HUBPSM20_ErrorOnConnectedEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortIgnoreError ,     Psm20StateConnectedEnabled },
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubReset },
        { PsmEventPortCycleOnError ,    Psm20StateDetachingDeviceFromPortOnPortCycle },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDetachingDeviceOnDisableAndCycle },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnDisconnected = {
    // State ETW Name
    Psm20StateEtwErrorOnDisconnected,
    // State Entry Function
    HUBPSM20_ErrorOnDisconnected,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortEnabledOnConnectError ,Psm20StateDisablingPortBeforeConnecting },
        { PsmEventPortIgnoreError ,     Psm20StateDisconnected },
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetFromDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnIssuingResetComplete = {
    // State ETW Name
    Psm20StateEtwErrorOnIssuingResetComplete,
    // State Entry Function
    HUBPSM20_ErrorOnIssuingResetComplete,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetWithTimer },
        { PsmEventPortCycleOnError ,    Psm20StateDetachingDeviceOnPortCycleWithTimer },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDetachingDeviceOnPortDisableAndCycleWithTimer },
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForResetTimerToFlush },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnResettingEnabled = {
    // State ETW Name
    Psm20StateEtwErrorOnResettingEnabled,
    // State Entry Function
    HUBPSM20_ErrorOnResettingEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetWithTimer },
        { PsmEventPortCycleOnError ,    Psm20StateDetachingDeviceOnPortCycleWithTimer },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDetachingDeviceOnPortDisableAndCycleWithTimer },
        { PsmEventPortIgnoreError ,     Psm20StateResettingEnabled },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortEnabledWhileResetError ,Psm20StateResettingEnabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnResettingPort = {
    // State ETW Name
    Psm20StateEtwErrorOnResettingPort,
    // State Entry Function
    HUBPSM20_ErrorOnResettingPort,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortEnabledWhileResetError ,Psm20StateResettingEnabled },
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForResetComplete },
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetWithTimer },
        { PsmEventPortCycleOnError ,    Psm20StateDetachingDeviceOnPortCycleWithTimer },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDetachingDeviceOnPortDisableAndCycleWithTimer },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled = {
    // State ETW Name
    Psm20StateEtwErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled,
    // State Entry Function
    HUBPSM20_ErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortIgnoreError ,     Psm20StateQueueingStateDisabledOnHubResumeFromDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnStoppingTimerOnDisconnect = {
    // State ETW Name
    Psm20StateEtwErrorOnStoppingTimerOnDisconnect,
    // State Entry Function
    HUBPSM20_ErrorOnStoppingTimerOnDisconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForTimerToFlushOnDisconnect },
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetWithPendingTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnStoppingTimerOnOverCurrent = {
    // State ETW Name
    Psm20StateEtwErrorOnStoppingTimerOnOverCurrent,
    // State Entry Function
    HUBPSM20_ErrorOnStoppingTimerOnOverCurrent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetWithPendingTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnStoppingTimerOnOverCurrentClear = {
    // State ETW Name
    Psm20StateEtwErrorOnStoppingTimerOnOverCurrentClear,
    // State Entry Function
    HUBPSM20_ErrorOnStoppingTimerOnOverCurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetWithPendingTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnStoppingTimerOnPortCycle = {
    // State ETW Name
    Psm20StateEtwErrorOnStoppingTimerOnPortCycle,
    // State Entry Function
    HUBPSM20_ErrorOnStoppingTimerOnPortCycle,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetWithPendingTimer },
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForTimerToFlushOnPortCycle },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnSuspended = {
    // State ETW Name
    Psm20StateEtwErrorOnSuspended,
    // State Entry Function
    HUBPSM20_ErrorOnSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubReset },
        { PsmEventPortCycleOnError ,    Psm20StateDetachingDeviceFromPortOnPortCycle },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDetachingDeviceOnDisableAndCycle },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventPortIgnoreError ,     Psm20StateSuspended },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnWaitingForCompanionPort = {
    // State ETW Name
    Psm20StateEtwErrorOnWaitingForCompanionPort,
    // State Entry Function
    HUBPSM20_ErrorOnWaitingForCompanionPort,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForUserResetOnOverCurrent },
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetFromDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnWaitingForDebounce = {
    // State ETW Name
    Psm20StateEtwErrorOnWaitingForDebounce,
    // State Entry Function
    HUBPSM20_ErrorOnWaitingForDebounce,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetHubOnError , Psm20StateStoppingTimerAndQueueingHubResetInDisconnected },
        { PsmEventPortIgnoreError ,     Psm20StateStoppingTimerOnDisconnect },
        { PsmEventPortCycleOnError ,    Psm20StateStoppingTimerOnPortCycle },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDisablingPortOnPortCycleWithTimer },
        { PsmEventPortOverCurrentCleared ,Psm20StateStoppingTimerOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateStoppingTimerOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateStoppingTimerOnDisconnect },
        { PsmEventHubStop ,             Psm20StateStoppingTimerOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnWaitingForDSMResumeResponse = {
    // State ETW Name
    Psm20StateEtwErrorOnWaitingForDSMResumeResponse,
    // State Entry Function
    HUBPSM20_ErrorOnWaitingForDSMResumeResponse,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetWhenWaitingForDSMResumeResponse },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForDSMResumeResponse },
        { PsmEventPortCycleOnError ,    Psm20StateDetachingDeviceFromPortOnPortCycle },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDetachingDeviceOnDisableAndCycle },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnWaitingForOldDeviceToCleanup = {
    // State ETW Name
    Psm20StateEtwErrorOnWaitingForOldDeviceToCleanup,
    // State Entry Function
    HUBPSM20_ErrorOnWaitingForOldDeviceToCleanup,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetFromDisconnected },
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForOldDeviceToDetach },
        { PsmEventPortCycleOnError ,    Psm20StateWaitingForDebounce },
        { PsmEventPortOverCurrent ,     Psm20StateStartingOverCurrentTimer },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDisablingPortBeforeConnecting },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnWaitingForOverCurrentClear = {
    // State ETW Name
    Psm20StateEtwErrorOnWaitingForOverCurrentClear,
    // State Entry Function
    HUBPSM20_ErrorOnWaitingForOverCurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForOverCurrentClear },
        { PsmEventPortResetHubOnError , Psm20StateStoppingTimerAndQueueingHubResetInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnWaitingForResumeComplete = {
    // State ETW Name
    Psm20StateEtwErrorOnWaitingForResumeComplete,
    // State Entry Function
    HUBPSM20_ErrorOnWaitingForResumeComplete,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForResumeComplete },
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetWithTimer },
        { PsmEventPortCycleOnError ,    Psm20StateDetachingDeviceOnPortCycleWithTimer },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDetachingDeviceOnPortDisableAndCycleWithTimer },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnWaitingForResumeRecoveryTimer = {
    // State ETW Name
    Psm20StateEtwErrorOnWaitingForResumeRecoveryTimer,
    // State Entry Function
    HUBPSM20_ErrorOnWaitingForResumeRecoveryTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForResumeRecoveryTimer },
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetWithTimer },
        { PsmEventPortCycleOnError ,    Psm20StateDetachingDeviceOnPortCycleWithTimer },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDetachingDeviceOnPortDisableAndCycleWithTimer },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryErrorOnWaitingForResumeTimerToFlush = {
    // State ETW Name
    Psm20StateEtwErrorOnWaitingForResumeTimerToFlush,
    // State Entry Function
    HUBPSM20_ErrorOnWaitingForResumeTimerToFlush,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortIgnoreError ,     Psm20StateWaitingForResumeTimerToFlush },
        { PsmEventPortResetHubOnError , Psm20StateIssuingHubResetWithTimer },
        { PsmEventPortCycleOnError ,    Psm20StateDetachingDeviceOnPortCycleWithTimer },
        { PsmEventPortDisableAndCycleOnError ,Psm20StateDetachingDeviceOnPortDisableAndCycleWithTimer },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryFlushingStaleDeviceEvents = {
    // State ETW Name
    Psm20StateEtwFlushingStaleDeviceEvents,
    // State Entry Function
    HUBPSM20_FlushingStaleDeviceEvents,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceRequestResume , Psm20StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm20StateIgnored },
        { PsmEventDeviceRequestCycle ,  Psm20StateIgnored },
        { PsmEventDeviceRequestSuspend ,Psm20StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm20StateIgnored },
        { PsmEventDevicePrepareForHibernation ,Psm20StateIgnored },
        { PsmEventOperationSuccess ,    Psm20StateIsItBootDevice },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryFlushingUserResetForOverCurrentOnHubStopSuspend = {
    // State ETW Name
    Psm20StateEtwFlushingUserResetForOverCurrentOnHubStopSuspend,
    // State Entry Function
    HUBPSM20_FlushingUserResetForOverCurrentOnHubStopSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventUserInitiatedResetOnOverCurrent ,Psm20StateIgnored },
        { PsmEventOperationSuccess ,    Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryFlushingUserResetForOverCurrentOnOverCurrentClear = {
    // State ETW Name
    Psm20StateEtwFlushingUserResetForOverCurrentOnOverCurrentClear,
    // State Entry Function
    HUBPSM20_FlushingUserResetForOverCurrentOnOverCurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventUserInitiatedResetOnOverCurrent ,Psm20StateIgnored },
        { PsmEventOperationSuccess ,    Psm20StateSettingPortPowerOnOverCurrent },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryInitiatingResetPort = {
    // State ETW Name
    Psm20StateEtwInitiatingResetPort,
    // State Entry Function
    HUBPSM20_InitiatingResetPort,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateCancellingResetOnSurpriseRemove },
        { PsmEventTransferFailure ,     Psm20StateStartingResetTimer },
        { PsmEventTransferSuccess ,     Psm20StateStartingResetTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryInitiatingResume = {
    // State ETW Name
    Psm20StateEtwInitiatingResume,
    // State Entry Function
    HUBPSM20_InitiatingResume,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateStartingResumeTimer },
        { PsmEventTransferSuccess ,     Psm20StateStartingResumeTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIsItBootDevice = {
    // State ETW Name
    Psm20StateEtwIsItBootDevice,
    // State Entry Function
    HUBPSM20_IsItBootDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateIssuingReAttachDeviceToBootDevice },
        { PsmEventNo ,                  Psm20StateCreatingDevice },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIsOldDevicePresent = {
    // State ETW Name
    Psm20StateEtwIsOldDevicePresent,
    // State Entry Function
    HUBPSM20_IsOldDevicePresent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateWaitingForOldDeviceToDetach },
        { PsmEventNo ,                  Psm20StateFlushingStaleDeviceEvents },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingAttachDeviceToPort = {
    // State ETW Name
    Psm20StateEtwIssuingAttachDeviceToPort,
    // State Entry Function
    HUBPSM20_IssuingAttachDeviceToPort,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForHubStopSuspendOnAttachFailure },
        { PsmEventOperationSuccess ,    Psm20StateConnectedDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingDetachDeviceOnCycleFromPoweredOffDisabledOrSuspended = {
    // State ETW Name
    Psm20StateEtwIssuingDetachDeviceOnCycleFromPoweredOffDisabledOrSuspended,
    // State Entry Function
    HUBPSM20_IssuingDetachDeviceOnCycleFromPoweredOffDisabledOrSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StatePoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingDisabledToDSMFromDisabled = {
    // State ETW Name
    Psm20StateEtwIssuingDisabledToDSMFromDisabled,
    // State Entry Function
    HUBPSM20_IssuingDisabledToDSMFromDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubReset = {
    // State ETW Name
    Psm20StateEtwIssuingHubReset,
    // State Entry Function
    HUBPSM20_IssuingHubReset,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm20SubSmIssuingHubReset,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventHubSuspend ,          Psm20StatePoweredOffSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubResetFromDisconnected = {
    // State ETW Name
    Psm20StateEtwIssuingHubResetFromDisconnected,
    // State Entry Function
    HUBPSM20_IssuingHubResetFromDisconnected,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm20SubSmIssuingHubResetFromDisconnected,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventHubSuspend ,          Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubResetOnDisableFailure = {
    // State ETW Name
    Psm20StateEtwIssuingHubResetOnDisableFailure,
    // State Entry Function
    HUBPSM20_IssuingHubResetOnDisableFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubResetWhenWaitingForDSMResumeResponse = {
    // State ETW Name
    Psm20StateEtwIssuingHubResetWhenWaitingForDSMResumeResponse,
    // State Entry Function
    HUBPSM20_IssuingHubResetWhenWaitingForDSMResumeResponse,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForDSMResumeResponse },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubResetWhileHubSuspend = {
    // State ETW Name
    Psm20StateEtwIssuingHubResetWhileHubSuspend,
    // State Entry Function
    HUBPSM20_IssuingHubResetWhileHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubResetWithPendingTimer = {
    // State ETW Name
    Psm20StateEtwIssuingHubResetWithPendingTimer,
    // State Entry Function
    HUBPSM20_IssuingHubResetWithPendingTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForTimerOnResetInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubResetWithTimer = {
    // State ETW Name
    Psm20StateEtwIssuingHubResetWithTimer,
    // State Entry Function
    HUBPSM20_IssuingHubResetWithTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReleasingInterruptReferenceAndStoppingTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubResetWithTimerOnHubSuspend = {
    // State ETW Name
    Psm20StateEtwIssuingHubResetWithTimerOnHubSuspend,
    // State Entry Function
    HUBPSM20_IssuingHubResetWithTimerOnHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventTimerFired ,          Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubResetWithTimerWhileHubSuspend = {
    // State ETW Name
    Psm20StateEtwIssuingHubResetWithTimerWhileHubSuspend,
    // State Entry Function
    HUBPSM20_IssuingHubResetWithTimerWhileHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateStoppingTimerOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingPortDisabledToDevice = {
    // State ETW Name
    Psm20StateEtwIssuingPortDisabledToDevice,
    // State Entry Function
    HUBPSM20_IssuingPortDisabledToDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingPortDisableFailedToDevice = {
    // State ETW Name
    Psm20StateEtwIssuingPortDisableFailedToDevice,
    // State Entry Function
    HUBPSM20_IssuingPortDisableFailedToDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingReAttachDeviceToBootDevice = {
    // State ETW Name
    Psm20StateEtwIssuingReAttachDeviceToBootDevice,
    // State Entry Function
    HUBPSM20_IssuingReAttachDeviceToBootDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled = {
    // State ETW Name
    Psm20StateEtwIssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled,
    // State Entry Function
    HUBPSM20_IssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled = {
    // State ETW Name
    Psm20StateEtwIssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled,
    // State Entry Function
    HUBPSM20_IssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedEnabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingResumedToDeviceSM = {
    // State ETW Name
    Psm20StateEtwIssuingResumedToDeviceSM,
    // State Entry Function
    HUBPSM20_IssuingResumedToDeviceSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedEnabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingSuspendedToDeviceSM = {
    // State ETW Name
    Psm20StateEtwIssuingSuspendedToDeviceSM,
    // State Entry Function
    HUBPSM20_IssuingSuspendedToDeviceSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryNotifyingUserAboutPersistentOverCurrent = {
    // State ETW Name
    Psm20StateEtwNotifyingUserAboutPersistentOverCurrent,
    // State Entry Function
    HUBPSM20_NotifyingUserAboutPersistentOverCurrent,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForUserResetOnOverCurrent },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryPoweredOffDisabled = {
    // State ETW Name
    Psm20StateEtwPoweredOffDisabled,
    // State Entry Function
    HUBPSM20_PoweredOffDisabled,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventHubStatusChange ,     Psm20StateIgnored },
        { PsmEventDeviceRequestResume , Psm20StateIgnored },
        { PsmEventDeviceRequestSuspend ,Psm20StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm20StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm20StateIgnored },
        { PsmEventHubStop ,             Psm20StateIgnored },
        { PsmEventDevicePrepareForHibernation ,Psm20StateIgnored },
        { PsmEventHubReset ,            Psm20StateDetachingDeviceOnHubReset },
        { PsmEventHubResume ,           Psm20StateStartingAndAcquiringReferenceOnHubResumeFromDisabled },
        { PsmEventDeviceRequestCycle ,  Psm20StateIssuingDetachDeviceOnCycleFromPoweredOffDisabledOrSuspended },
        { PsmEventObjectCleanup ,       Psm20StateDetachingDeviceOnCleanup },
        { PsmEventHubResumeWithReset ,  Psm20StateStartingAndAcquiringReferenceOnHubResumeFromDisabled },
        { PsmEventHubResumeInS0 ,       Psm20StateAcquiringPowerReferenceFromPoweredOffDisabledInS0 },
        { PsmEventHubStart ,            Psm20StateStartingAndAcquiringReferenceOnHubStartFromDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryPoweredOffDisconnected = {
    // State ETW Name
    Psm20StateEtwPoweredOffDisconnected,
    // State Entry Function
    HUBPSM20_PoweredOffDisconnected,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceRequestDisable ,Psm20StateIgnored },
        { PsmEventDeviceRequestSuspend ,Psm20StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm20StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventHubStop ,             Psm20StateIgnored },
        { PsmEventDeviceRequestCycle ,  Psm20StateIgnored },
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventDeviceRequestResume , Psm20StateIgnored },
        { PsmEventDevicePrepareForHibernation ,Psm20StateIgnored },
        { PsmEventHubResume ,           Psm20StateStartingAndAcquiringReferenceOnHubStart },
        { PsmEventHubStart ,            Psm20StateStartingAndAcquiringReferenceOnHubStart },
        { PsmEventHubResumeWithReset ,  Psm20StateStartingAndAcquiringReferenceOnHubStart },
        { PsmEventObjectCleanup ,       Psm20StateWaitingToBeDeleted },
        { PsmEventHubReset ,            Psm20StateReleasingResetReferenceOnDeviceRemoval },
        { PsmEventHubResumeInS0 ,       Psm20StateAcquiringPowerReferenceFromPoweredOffDisconnectedInS0 },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryPoweredOffSuspended = {
    // State ETW Name
    Psm20StateEtwPoweredOffSuspended,
    // State Entry Function
    HUBPSM20_PoweredOffSuspended,
    // State Flags
    StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm20StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventHubResume ,           Psm20StateStartingAndAcquiringReferenceOnWarmResume },
        { PsmEventHubStop ,             Psm20StatePoweredOffDisabled },
        { PsmEventHubResumeInS0 ,       Psm20StateAcquiringPowerReferenceFromPoweredOffSuspendedInS0 },
        { PsmEventHubResumeWithReset ,  Psm20StateStartingAndAcquiringReferenceOnHubResume },
        { PsmEventObjectCleanup ,       Psm20StateDetachingDeviceOnCleanup },
        { PsmEventDeviceRequestCycle ,  Psm20StateIssuingDetachDeviceOnCycleFromPoweredOffDisabledOrSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueryingPortStatusOnOverCurrent = {
    // State ETW Name
    Psm20StateEtwQueryingPortStatusOnOverCurrent,
    // State Entry Function
    HUBPSM20_QueryingPortStatusOnOverCurrent,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetFromDisconnected },
        { PsmEventTransferSuccess ,     Psm20StateCheckingIfOvercurrentBitOne },
        { PsmEventHubStop ,             Psm20StateCancellingQueryPortStatus },
        { PsmEventHubSuspend ,          Psm20StateCancellingQueryPortStatus },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingPortFailureToDSMOnSuspend = {
    // State ETW Name
    Psm20StateEtwQueueingPortFailureToDSMOnSuspend,
    // State Entry Function
    HUBPSM20_QueueingPortFailureToDSMOnSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateIssuingHubReset },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingPortResumeFailedDueToPendingSuspend = {
    // State ETW Name
    Psm20StateEtwQueueingPortResumeFailedDueToPendingSuspend,
    // State Entry Function
    HUBPSM20_QueueingPortResumeFailedDueToPendingSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingPortResumeTimedOut = {
    // State ETW Name
    Psm20StateEtwQueueingPortResumeTimedOut,
    // State Entry Function
    HUBPSM20_QueueingPortResumeTimedOut,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingStateDisabledOnHubResumeFromDisabled = {
    // State ETW Name
    Psm20StateEtwQueueingStateDisabledOnHubResumeFromDisabled,
    // State Entry Function
    HUBPSM20_QueueingStateDisabledOnHubResumeFromDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingStateDisabledToDeviceSM = {
    // State ETW Name
    Psm20StateEtwQueueingStateDisabledToDeviceSM,
    // State Entry Function
    HUBPSM20_QueueingStateDisabledToDeviceSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingStateDisabledToDeviceSMOnResetHub = {
    // State ETW Name
    Psm20StateEtwQueueingStateDisabledToDeviceSMOnResetHub,
    // State Entry Function
    HUBPSM20_QueueingStateDisabledToDeviceSMOnResetHub,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateIssuingHubReset },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingStateEnabledToDeviceSM = {
    // State ETW Name
    Psm20StateEtwQueueingStateEnabledToDeviceSM,
    // State Entry Function
    HUBPSM20_QueueingStateEnabledToDeviceSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedEnabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingStateSuspendedToDeviceSM = {
    // State ETW Name
    Psm20StateEtwQueueingStateSuspendedToDeviceSM,
    // State Entry Function
    HUBPSM20_QueueingStateSuspendedToDeviceSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingStateSuspendedToDeviceSMOnIgnoreError = {
    // State ETW Name
    Psm20StateEtwQueueingStateSuspendedToDeviceSMOnIgnoreError,
    // State Entry Function
    HUBPSM20_QueueingStateSuspendedToDeviceSMOnIgnoreError,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReleasingInterruptReferenceAndIssuingPortResetTimedOutToDSM = {
    // State ETW Name
    Psm20StateEtwReleasingInterruptReferenceAndIssuingPortResetTimedOutToDSM,
    // State Entry Function
    HUBPSM20_ReleasingInterruptReferenceAndIssuingPortResetTimedOutToDSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReleasingInterruptReferenceAndStoppingTimer = {
    // State ETW Name
    Psm20StateEtwReleasingInterruptReferenceAndStoppingTimer,
    // State Entry Function
    HUBPSM20_ReleasingInterruptReferenceAndStoppingTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForHubStopSuspendOrTimer },
        { PsmEventOperationSuccess ,    Psm20StateWaitingForHubStopSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReleasingInterruptReferenceOnResetTransferFailure = {
    // State ETW Name
    Psm20StateEtwReleasingInterruptReferenceOnResetTransferFailure,
    // State Entry Function
    HUBPSM20_ReleasingInterruptReferenceOnResetTransferFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateIssuingHubReset },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReleasingInterruptReferenceOnSuccessfulReset = {
    // State ETW Name
    Psm20StateEtwReleasingInterruptReferenceOnSuccessfulReset,
    // State Entry Function
    HUBPSM20_ReleasingInterruptReferenceOnSuccessfulReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateConnectedEnabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReleasingPowerReferenceonPoweredOffDisabled = {
    // State ETW Name
    Psm20StateEtwReleasingPowerReferenceonPoweredOffDisabled,
    // State Entry Function
    HUBPSM20_ReleasingPowerReferenceonPoweredOffDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StatePoweredOffDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReleasingPowerReferenceonPoweredOffDisconnected = {
    // State ETW Name
    Psm20StateEtwReleasingPowerReferenceonPoweredOffDisconnected,
    // State Entry Function
    HUBPSM20_ReleasingPowerReferenceonPoweredOffDisconnected,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StatePoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReleasingResetReferenceOnDeviceRemoval = {
    // State ETW Name
    Psm20StateEtwReleasingResetReferenceOnDeviceRemoval,
    // State Entry Function
    HUBPSM20_ReleasingResetReferenceOnDeviceRemoval,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StatePoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryResettingEnabled = {
    // State ETW Name
    Psm20StateEtwResettingEnabled,
    // State Entry Function
    HUBPSM20_ResettingEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmResettingEnabled,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetComplete ,   Psm20StateStoppingTimerOnResetComplete },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventPortResumed ,         Psm20StateErrorOnResettingEnabled },
        { PsmEventPortError ,           Psm20StateErrorOnResettingEnabled },
        { PsmEventPortDisabled ,        Psm20StateErrorOnResettingEnabled },
        { PsmEventTimerFired ,          Psm20StateDisablingPortOnTimeOut },
        { PsmEventHubSurpriseRemove ,   Psm20StateReleasingInterruptReferenceAndStoppingTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntrySettingPortPowerOnOverCurrent = {
    // State ETW Name
    Psm20StateEtwSettingPortPowerOnOverCurrent,
    // State Entry Function
    HUBPSM20_SettingPortPowerOnOverCurrent,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferSuccess ,     Psm20StateQueryingPortStatusOnOverCurrent },
        { PsmEventHubStop ,             Psm20StateCancellingSetPortPower },
        { PsmEventHubSuspend ,          Psm20StateCancellingSetPortPower },
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetFromDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingAndAcquiringReferenceOnHubResume = {
    // State ETW Name
    Psm20StateEtwStartingAndAcquiringReferenceOnHubResume,
    // State Entry Function
    HUBPSM20_StartingAndAcquiringReferenceOnHubResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmStartingAndAcquiringReferenceOnHubResume,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortError ,           Psm20StateErrorOnAcquringReferenceOnHubResume },
        { PsmEventPortResumed ,         Psm20StateQueueingStateEnabledToDeviceSM },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnAcquringReferenceOnHubResume },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventPortDisabled ,        Psm20StateQueueingStateDisabledToDeviceSM },
        { PsmEventOperationSuccess ,    Psm20StateQueueingStateSuspendedToDeviceSM },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingAndAcquiringReferenceOnHubResumeFromDisabled = {
    // State ETW Name
    Psm20StateEtwStartingAndAcquiringReferenceOnHubResumeFromDisabled,
    // State Entry Function
    HUBPSM20_StartingAndAcquiringReferenceOnHubResumeFromDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmStartingAndAcquiringReferenceOnHubResumeFromDisabled,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortError ,           Psm20StateErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled },
        { PsmEventPortDisabled ,        Psm20StateErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventOperationSuccess ,    Psm20StateQueueingStateDisabledOnHubResumeFromDisabled },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled },
        { PsmEventPortResumed ,         Psm20StateErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled },
        { PsmEventHubSuspend ,          Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingAndAcquiringReferenceOnHubStart = {
    // State ETW Name
    Psm20StateEtwStartingAndAcquiringReferenceOnHubStart,
    // State Entry Function
    HUBPSM20_StartingAndAcquiringReferenceOnHubStart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmStartingAndAcquiringReferenceOnHubStart,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm20StateSettingPortPowerOnOverCurrent },
        { PsmEventOperationSuccess ,    Psm20StateDisconnected },
        { PsmEventPortOverCurrent ,     Psm20StateStartingOverCurrentTimer },
        { PsmEventPortConnectChange ,   Psm20StateCheckingIfConnectBitOne },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnDisconnected },
        { PsmEventPortDisabled ,        Psm20StateErrorOnDisconnected },
        { PsmEventPortResumed ,         Psm20StateErrorOnDisconnected },
        { PsmEventPortError ,           Psm20StateErrorOnDisconnected },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventHubSuspend ,          Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingAndAcquiringReferenceOnHubStartFromDisabled = {
    // State ETW Name
    Psm20StateEtwStartingAndAcquiringReferenceOnHubStartFromDisabled,
    // State Entry Function
    HUBPSM20_StartingAndAcquiringReferenceOnHubStartFromDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmStartingAndAcquiringReferenceOnHubStartFromDisabled,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventOperationSuccess ,    Psm20StateConnectedDisabled },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnConnectedDisabled },
        { PsmEventPortResumed ,         Psm20StateErrorOnConnectedDisabled },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventPortError ,           Psm20StateErrorOnConnectedDisabled },
        { PsmEventPortDisabled ,        Psm20StateErrorOnConnectedDisabled },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventHubSuspend ,          Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingAndAcquiringReferenceOnWarmResume = {
    // State ETW Name
    Psm20StateEtwStartingAndAcquiringReferenceOnWarmResume,
    // State Entry Function
    HUBPSM20_StartingAndAcquiringReferenceOnWarmResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmStartingAndAcquiringReferenceOnWarmResume,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventOperationSuccess ,    Psm20StateQueueingStateSuspendedToDeviceSM },
        { PsmEventPortResumed ,         Psm20StateQueueingStateEnabledToDeviceSM },
        { PsmEventPortDisabled ,        Psm20StateQueueingStateDisabledToDeviceSM },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnAcquringReferenceOnHubResume },
        { PsmEventPortError ,           Psm20StateErrorOnAcquringReferenceOnHubResume },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingOverCurrentTimer = {
    // State ETW Name
    Psm20StateEtwStartingOverCurrentTimer,
    // State Entry Function
    HUBPSM20_StartingOverCurrentTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForOverCurrentClear },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingResetTimer = {
    // State ETW Name
    Psm20StateEtwStartingResetTimer,
    // State Entry Function
    HUBPSM20_StartingResetTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForResetComplete },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingResumeRecoveryTimer = {
    // State ETW Name
    Psm20StateEtwStartingResumeRecoveryTimer,
    // State Entry Function
    HUBPSM20_StartingResumeRecoveryTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForResumeRecoveryTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingResumeTimer = {
    // State ETW Name
    Psm20StateEtwStartingResumeTimer,
    // State Entry Function
    HUBPSM20_StartingResumeTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForResumeComplete },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingResumeTimer = {
    // State ETW Name
    Psm20StateEtwStoppingResumeTimer,
    // State Entry Function
    HUBPSM20_StoppingResumeTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForResumeTimerToFlush },
        { PsmEventOperationSuccess ,    Psm20StateStartingResumeRecoveryTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingTimerAndQueueingHubResetInDisconnected = {
    // State ETW Name
    Psm20StateEtwStoppingTimerAndQueueingHubResetInDisconnected,
    // State Entry Function
    HUBPSM20_StoppingTimerAndQueueingHubResetInDisconnected,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForTimerOnResetInDisconnected },
        { PsmEventOperationSuccess ,    Psm20StateWaitingForHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingTimerOnDisconnect = {
    // State ETW Name
    Psm20StateEtwStoppingTimerOnDisconnect,
    // State Entry Function
    HUBPSM20_StoppingTimerOnDisconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForTimerToFlushOnDisconnect },
        { PsmEventOperationSuccess ,    Psm20StateCheckingIfConnectBitOne },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingTimerOnHubStopSuspendInDisconnected = {
    // State ETW Name
    Psm20StateEtwStoppingTimerOnHubStopSuspendInDisconnected,
    // State Entry Function
    HUBPSM20_StoppingTimerOnHubStopSuspendInDisconnected,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventOperationSuccess ,    Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingTimerOnOverCurrent = {
    // State ETW Name
    Psm20StateEtwStoppingTimerOnOverCurrent,
    // State Entry Function
    HUBPSM20_StoppingTimerOnOverCurrent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventOperationSuccess ,    Psm20StateStartingOverCurrentTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingTimerOnOverCurrentClear = {
    // State ETW Name
    Psm20StateEtwStoppingTimerOnOverCurrentClear,
    // State Entry Function
    HUBPSM20_StoppingTimerOnOverCurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventOperationSuccess ,    Psm20StateSettingPortPowerOnOverCurrent },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingTimerOnPortCycle = {
    // State ETW Name
    Psm20StateEtwStoppingTimerOnPortCycle,
    // State Entry Function
    HUBPSM20_StoppingTimerOnPortCycle,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForTimerToFlushOnPortCycle },
        { PsmEventOperationSuccess ,    Psm20StateCheckingIfConnectBitOne },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingTimerOnResetComplete = {
    // State ETW Name
    Psm20StateEtwStoppingTimerOnResetComplete,
    // State Entry Function
    HUBPSM20_StoppingTimerOnResetComplete,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForResetTimerToFlush },
        { PsmEventOperationSuccess ,    Psm20StateReleasingInterruptReferenceOnSuccessfulReset },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntrySuspended = {
    // State ETW Name
    Psm20StateEtwSuspended,
    // State Entry Function
    HUBPSM20_Suspended,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    Psm20SubSmSuspended,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortResumed ,         Psm20StateWaitingForDSMResumeResponse },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnSuspended },
        { PsmEventPortError ,           Psm20StateErrorOnSuspended },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventPortDisabled ,        Psm20StateConnectedDisabled },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventHubSuspend ,          Psm20StatePoweredOffSuspended },
        { PsmEventDeviceRequestResume , Psm20StateAcquiringInterruptReferenceOnResuming },
        { PsmEventDeviceRequestReset ,  Psm20StateAcquiringInterruptReferenceOnHSMInDisabled },
        { PsmEventDeviceRequestCycle ,  Psm20StateDetachingDeviceOnDisableAndCycle },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntrySuspending = {
    // State ETW Name
    Psm20StateEtwSuspending,
    // State Entry Function
    HUBPSM20_Suspending,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferSuccess ,     Psm20StateIssuingSuspendedToDeviceSM },
        { PsmEventTransferFailure ,     Psm20StateQueueingPortFailureToDSMOnSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDebounce = {
    // State ETW Name
    Psm20StateEtwWaitingForDebounce,
    // State Entry Function
    HUBPSM20_WaitingForDebounce,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForDebounce,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm20StateStoppingTimerOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateStoppingTimerOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateStoppingTimerOnDisconnect },
        { PsmEventPortDisabled ,        Psm20StateErrorOnWaitingForDebounce },
        { PsmEventPortResumed ,         Psm20StateErrorOnWaitingForDebounce },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnWaitingForDebounce },
        { PsmEventPortError ,           Psm20StateErrorOnWaitingForDebounce },
        { PsmEventTimerFired ,          Psm20StateIsOldDevicePresent },
        { PsmEventHubSuspend ,          Psm20StateDisablingOnHubSuspendWithTimer },
        { PsmEventHubStop ,             Psm20StateStoppingTimerOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDisableCompleteOnHubSuspend = {
    // State ETW Name
    Psm20StateEtwWaitingForDisableCompleteOnHubSuspend,
    // State Entry Function
    HUBPSM20_WaitingForDisableCompleteOnHubSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetWhileHubSuspend },
        { PsmEventTransferSuccess ,     Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDisableCompleteWithTimerOnHubSuspend = {
    // State ETW Name
    Psm20StateEtwWaitingForDisableCompleteWithTimerOnHubSuspend,
    // State Entry Function
    HUBPSM20_WaitingForDisableCompleteWithTimerOnHubSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTransferSuccess ,     Psm20StateStoppingTimerOnHubStopSuspendInDisconnected },
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetWithTimerWhileHubSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDSMResumeResponse = {
    // State ETW Name
    Psm20StateEtwWaitingForDSMResumeResponse,
    // State Entry Function
    HUBPSM20_WaitingForDSMResumeResponse,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm20SubSmWaitingForDSMResumeResponse,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventPortError ,           Psm20StateErrorOnWaitingForDSMResumeResponse },
        { PsmEventPortDisabled ,        Psm20StateErrorOnWaitingForDSMResumeResponse },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnWaitingForDSMResumeResponse },
        { PsmEventPortResumed ,         Psm20StateErrorOnWaitingForDSMResumeResponse },
        { PsmEventDeviceRequestResume , Psm20StateConnectedEnabled },
        { PsmEventHubStop ,             Psm20StateWaitingForDSMResumeResponseOnHubStop },
        { PsmEventDeviceRequestCycle ,  Psm20StateDetachingDeviceOnDisableAndCycle },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDSMResumeResponseOnHubStop = {
    // State ETW Name
    Psm20StateEtwWaitingForDSMResumeResponseOnHubStop,
    // State Entry Function
    HUBPSM20_WaitingForDSMResumeResponseOnHubStop,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventDeviceRequestResume , Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventDeviceRequestCycle ,  Psm20StateDetachingDeviceFromPortOnCycleOnHubStop },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForHubStopSuspend = {
    // State ETW Name
    Psm20StateEtwWaitingForHubStopSuspend,
    // State Entry Function
    HUBPSM20_WaitingForHubStopSuspend,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm20SubSmWaitingForHubStopSuspend,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventHubSuspend ,          Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForHubStopSuspendInDisconnected = {
    // State ETW Name
    Psm20StateEtwWaitingForHubStopSuspendInDisconnected,
    // State Entry Function
    HUBPSM20_WaitingForHubStopSuspendInDisconnected,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm20SubSmWaitingForHubStopSuspendInDisconnected,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventHubSuspend ,          Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForHubStopSuspendOnAttachFailure = {
    // State ETW Name
    Psm20StateEtwWaitingForHubStopSuspendOnAttachFailure,
    // State Entry Function
    HUBPSM20_WaitingForHubStopSuspendOnAttachFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForHubStopSuspendOnAttachFailure,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventHubSuspend ,          Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForHubStopSuspendOrTimer = {
    // State ETW Name
    Psm20StateEtwWaitingForHubStopSuspendOrTimer,
    // State Entry Function
    HUBPSM20_WaitingForHubStopSuspendOrTimer,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm20SubSmWaitingForHubStopSuspendOrTimer,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTimerFired ,          Psm20StateWaitingForHubStopSuspend },
        { PsmEventHubStop ,             Psm20StateWaitingForTimerToFlushOnHubStopSuspend },
        { PsmEventHubSuspend ,          Psm20StateWaitingForTimerToFlushOnHubStopSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForOldDeviceToDetach = {
    // State ETW Name
    Psm20StateEtwWaitingForOldDeviceToDetach,
    // State Entry Function
    HUBPSM20_WaitingForOldDeviceToDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForOldDeviceToDetach,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm20StateSettingPortPowerOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateCheckingIfConnectBitOne },
        { PsmEventPortDisabled ,        Psm20StateErrorOnWaitingForOldDeviceToCleanup },
        { PsmEventPortResumed ,         Psm20StateErrorOnWaitingForOldDeviceToCleanup },
        { PsmEventPortOverCurrent ,     Psm20StateStartingOverCurrentTimer },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnWaitingForOldDeviceToCleanup },
        { PsmEventPortError ,           Psm20StateErrorOnWaitingForOldDeviceToCleanup },
        { PsmEventHubStop ,             Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventDeviceDetached ,      Psm20StateFlushingStaleDeviceEvents },
        { PsmEventHubSuspend ,          Psm20StateDisablingPortOnHubSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForOverCurrentClear = {
    // State ETW Name
    Psm20StateEtwWaitingForOverCurrentClear,
    // State Entry Function
    HUBPSM20_WaitingForOverCurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForOverCurrentClear,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventPortError ,           Psm20StateErrorOnWaitingForOverCurrentClear },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnWaitingForOverCurrentClear },
        { PsmEventPortDisabled ,        Psm20StateErrorOnWaitingForOverCurrentClear },
        { PsmEventPortResumed ,         Psm20StateErrorOnWaitingForOverCurrentClear },
        { PsmEventPortConnectChange ,   Psm20StateErrorOnWaitingForOverCurrentClear },
        { PsmEventPortOverCurrentCleared ,Psm20StateStoppingTimerOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateStoppingTimerOnOverCurrent },
        { PsmEventTimerFired ,          Psm20StateCheckingIfPersistentOvercurrent },
        { PsmEventHubStop ,             Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventHubSuspend ,          Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForResetComplete = {
    // State ETW Name
    Psm20StateEtwWaitingForResetComplete,
    // State Entry Function
    HUBPSM20_WaitingForResetComplete,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForResetComplete,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetComplete ,   Psm20StateStoppingTimerOnResetComplete },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortError ,           Psm20StateErrorOnResettingPort },
        { PsmEventPortDisabled ,        Psm20StateErrorOnResettingPort },
        { PsmEventPortResumed ,         Psm20StateErrorOnResettingPort },
        { PsmEventPortConnectChange ,   Psm20StateErrorOnResettingPort },
        { PsmEventTimerFired ,          Psm20StateReleasingInterruptReferenceAndIssuingPortResetTimedOutToDSM },
        { PsmEventHubSurpriseRemove ,   Psm20StateReleasingInterruptReferenceAndStoppingTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForResetTimerToFlush = {
    // State ETW Name
    Psm20StateEtwWaitingForResetTimerToFlush,
    // State Entry Function
    HUBPSM20_WaitingForResetTimerToFlush,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForResetTimerToFlush,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortError ,           Psm20StateErrorOnIssuingResetComplete },
        { PsmEventPortDisabled ,        Psm20StateErrorOnIssuingResetComplete },
        { PsmEventPortResumed ,         Psm20StateErrorOnIssuingResetComplete },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnIssuingResetComplete },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventTimerFired ,          Psm20StateReleasingInterruptReferenceOnSuccessfulReset },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForResumeComplete = {
    // State ETW Name
    Psm20StateEtwWaitingForResumeComplete,
    // State Entry Function
    HUBPSM20_WaitingForResumeComplete,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForResumeComplete,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResumed ,         Psm20StateStoppingResumeTimer },
        { PsmEventPortDisabled ,        Psm20StateErrorOnWaitingForResumeComplete },
        { PsmEventPortError ,           Psm20StateErrorOnWaitingForResumeComplete },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnWaitingForResumeComplete },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventHubSurpriseRemove ,   Psm20StateReleasingInterruptReferenceAndStoppingTimer },
        { PsmEventTimerFired ,          Psm20StateQueueingPortResumeTimedOut },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForResumeRecoveryTimer = {
    // State ETW Name
    Psm20StateEtwWaitingForResumeRecoveryTimer,
    // State Entry Function
    HUBPSM20_WaitingForResumeRecoveryTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForResumeRecoveryTimer,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortResumed ,         Psm20StateErrorOnWaitingForResumeRecoveryTimer },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnWaitingForResumeRecoveryTimer },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventPortError ,           Psm20StateErrorOnWaitingForResumeRecoveryTimer },
        { PsmEventPortDisabled ,        Psm20StateErrorOnWaitingForResumeRecoveryTimer },
        { PsmEventTimerFired ,          Psm20StateIssuingResumedToDeviceSM },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForResumeTimerToFlush = {
    // State ETW Name
    Psm20StateEtwWaitingForResumeTimerToFlush,
    // State Entry Function
    HUBPSM20_WaitingForResumeTimerToFlush,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForResumeTimerToFlush,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortDisabled ,        Psm20StateErrorOnWaitingForResumeTimerToFlush },
        { PsmEventPortError ,           Psm20StateErrorOnWaitingForResumeTimerToFlush },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnWaitingForResumeTimerToFlush },
        { PsmEventPortResumed ,         Psm20StateErrorOnWaitingForResumeTimerToFlush },
        { PsmEventPortOverCurrentCleared ,Psm20StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm20StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm20StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventTimerFired ,          Psm20StateStartingResumeRecoveryTimer },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForTimerOnResetInDisconnected = {
    // State ETW Name
    Psm20StateEtwWaitingForTimerOnResetInDisconnected,
    // State Entry Function
    HUBPSM20_WaitingForTimerOnResetInDisconnected,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm20SubSmWaitingForTimerOnResetInDisconnected,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTimerFired ,          Psm20StateWaitingForHubStopSuspendInDisconnected },
        { PsmEventHubStop ,             Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventHubSuspend ,          Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForTimerToFlushOnDisconnect = {
    // State ETW Name
    Psm20StateEtwWaitingForTimerToFlushOnDisconnect,
    // State Entry Function
    HUBPSM20_WaitingForTimerToFlushOnDisconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForTimerToFlushOnDisconnect,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortOverCurrent ,     Psm20StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateWaitingForTimerToFlushOnDisconnect },
        { PsmEventPortError ,           Psm20StateErrorOnStoppingTimerOnDisconnect },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnStoppingTimerOnDisconnect },
        { PsmEventPortConnectChange ,   Psm20StateWaitingForTimerToFlushOnDisconnect },
        { PsmEventPortOverCurrentCleared ,Psm20StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventPortDisabled ,        Psm20StateErrorOnStoppingTimerOnDisconnect },
        { PsmEventPortResumed ,         Psm20StateErrorOnStoppingTimerOnDisconnect },
        { PsmEventHubSuspend ,          Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventHubStop ,             Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventTimerFired ,          Psm20StateCheckingIfConnectBitOne },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForTimerToFlushOnHubStopSuspend = {
    // State ETW Name
    Psm20StateEtwWaitingForTimerToFlushOnHubStopSuspend,
    // State Entry Function
    HUBPSM20_WaitingForTimerToFlushOnHubStopSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm20StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTimerFired ,          Psm20StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForTimerToFlushOnHubStopSuspendInDisconnected = {
    // State ETW Name
    Psm20StateEtwWaitingForTimerToFlushOnHubStopSuspendInDisconnected,
    // State Entry Function
    HUBPSM20_WaitingForTimerToFlushOnHubStopSuspendInDisconnected,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTimerFired ,          Psm20StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForTimerToFlushOnOverCurrent = {
    // State ETW Name
    Psm20StateEtwWaitingForTimerToFlushOnOverCurrent,
    // State Entry Function
    HUBPSM20_WaitingForTimerToFlushOnOverCurrent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForTimerToFlushOnOverCurrent,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm20StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventPortError ,           Psm20StateErrorOnStoppingTimerOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm20StateErrorOnStoppingTimerOnOverCurrent },
        { PsmEventPortDisabled ,        Psm20StateErrorOnStoppingTimerOnOverCurrent },
        { PsmEventPortOverCurrent ,     Psm20StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnStoppingTimerOnOverCurrent },
        { PsmEventPortResumed ,         Psm20StateErrorOnStoppingTimerOnOverCurrent },
        { PsmEventPortOverCurrent ,     Psm20StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventTimerFired ,          Psm20StateStartingOverCurrentTimer },
        { PsmEventHubStop ,             Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventHubSuspend ,          Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForTimerToFlushOnOverCurrentClear = {
    // State ETW Name
    Psm20StateEtwWaitingForTimerToFlushOnOverCurrentClear,
    // State Entry Function
    HUBPSM20_WaitingForTimerToFlushOnOverCurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForTimerToFlushOnOverCurrentClear,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm20StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventPortError ,           Psm20StateErrorOnStoppingTimerOnOverCurrentClear },
        { PsmEventPortConnectChange ,   Psm20StateErrorOnStoppingTimerOnOverCurrentClear },
        { PsmEventPortDisabled ,        Psm20StateErrorOnStoppingTimerOnOverCurrentClear },
        { PsmEventPortOverCurrentCleared ,Psm20StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm20StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnStoppingTimerOnOverCurrentClear },
        { PsmEventPortResumed ,         Psm20StateErrorOnStoppingTimerOnOverCurrentClear },
        { PsmEventHubStop ,             Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventTimerFired ,          Psm20StateSettingPortPowerOnOverCurrent },
        { PsmEventHubSuspend ,          Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForTimerToFlushOnPortCycle = {
    // State ETW Name
    Psm20StateEtwWaitingForTimerToFlushOnPortCycle,
    // State Entry Function
    HUBPSM20_WaitingForTimerToFlushOnPortCycle,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForTimerToFlushOnPortCycle,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortConnectChange ,   Psm20StateWaitingForTimerToFlushOnDisconnect },
        { PsmEventPortError ,           Psm20StateErrorOnStoppingTimerOnPortCycle },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnStoppingTimerOnPortCycle },
        { PsmEventPortDisabled ,        Psm20StateErrorOnStoppingTimerOnPortCycle },
        { PsmEventPortOverCurrent ,     Psm20StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventPortResumed ,         Psm20StateWaitingForTimerToFlushOnPortCycle },
        { PsmEventPortResumed ,         Psm20StateWaitingForTimerToFlushOnPortCycle },
        { PsmEventPortOverCurrentCleared ,Psm20StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventTimerFired ,          Psm20StateCheckingIfConnectBitOne },
        { PsmEventHubSuspend ,          Psm20StateDisablingOnHubSuspendWithTimer },
        { PsmEventHubStop ,             Psm20StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForUserResetOnOverCurrent = {
    // State ETW Name
    Psm20StateEtwWaitingForUserResetOnOverCurrent,
    // State Entry Function
    HUBPSM20_WaitingForUserResetOnOverCurrent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForUserResetOnOverCurrent,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventPortError ,           Psm20StateErrorOnWaitingForCompanionPort },
        { PsmEventPortResetComplete ,   Psm20StateErrorOnWaitingForCompanionPort },
        { PsmEventPortDisabled ,        Psm20StateErrorOnWaitingForCompanionPort },
        { PsmEventPortResumed ,         Psm20StateErrorOnWaitingForCompanionPort },
        { PsmEventPortConnectChange ,   Psm20StateErrorOnWaitingForCompanionPort },
        { PsmEventPortOverCurrent ,     Psm20StateStartingOverCurrentTimer },
        { PsmEventPortOverCurrentCleared ,Psm20StateFlushingUserResetForOverCurrentOnOverCurrentClear },
        { PsmEventUserInitiatedResetOnOverCurrent ,Psm20StateSettingPortPowerOnOverCurrent },
        { PsmEventHubSuspend ,          Psm20StateFlushingUserResetForOverCurrentOnHubStopSuspend },
        { PsmEventHubStop ,             Psm20StateFlushingUserResetForOverCurrentOnHubStopSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingToBeDeleted = {
    // State ETW Name
    Psm20StateEtwWaitingToBeDeleted,
    // State Entry Function
    HUBPSM20_WaitingToBeDeleted,
    // State Flags
    StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAckingPortChange = {
    // State ETW Name
    Psm20StateEtwAckingPortChange,
    // State Entry Function
    HUBPSM20_AckingPortChange,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetOnControlTransferFailure },
        { PsmEventTransferSuccess ,     Psm20StateCheckIfThereIsAPortChange },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckIfThereIsAPortChange = {
    // State ETW Name
    Psm20StateEtwCheckIfThereIsAPortChange,
    // State Entry Function
    HUBPSM20_CheckIfThereIsAPortChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortError ,           Psm20StateIssuingHubResetOnControlTransferFailure },
        { PsmEventYes ,                 Psm20StateAckingPortChange },
        { PsmEventNo ,                  Psm20StateGettingPortStatus },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckIfThereIsAPortChangeOnPortStatus = {
    // State ETW Name
    Psm20StateEtwCheckIfThereIsAPortChangeOnPortStatus,
    // State Entry Function
    HUBPSM20_CheckIfThereIsAPortChangeOnPortStatus,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateAckingPortChange },
        { PsmEventPortError ,           Psm20StateIssuingHubResetOnControlTransferFailure },
        { PsmEventNo ,                  Psm20StateEnablingInterruptsAndGettingPortEvent },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryEnablingInterruptsAndGettingPortEvent = {
    // State ETW Name
    Psm20StateEtwEnablingInterruptsAndGettingPortEvent,
    // State Entry Function
    HUBPSM20_EnablingInterruptsAndGettingPortEvent,
    // State Flags
    StateFlagEndSubStateMachine|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryGettingPortStatus = {
    // State ETW Name
    Psm20StateEtwGettingPortStatus,
    // State Entry Function
    HUBPSM20_GettingPortStatus,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetOnControlTransferFailure },
        { PsmEventTransferSuccess ,     Psm20StateCheckIfThereIsAPortChangeOnPortStatus },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryInitializingCumulativePortChangeBits = {
    // State ETW Name
    Psm20StateEtwInitializingCumulativePortChangeBits,
    // State Entry Function
    HUBPSM20_InitializingCumulativePortChangeBits,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateGettingPortStatus },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubResetOnControlTransferFailure = {
    // State ETW Name
    Psm20StateEtwIssuingHubResetOnControlTransferFailure,
    // State Entry Function
    HUBPSM20_IssuingHubResetOnControlTransferFailure,
    // State Flags
    StateFlagPopIfParentStateChanges|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm20StateQueueingEnableInterruptTransferOnPortChange },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingEnableInterruptTransferOnPortChange = {
    // State ETW Name
    Psm20StateEtwQueueingEnableInterruptTransferOnPortChange,
    // State Entry Function
    HUBPSM20_QueueingEnableInterruptTransferOnPortChange,
    // State Flags
    StateFlagPopIfParentStateChanges|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm20StateQueueingEnableInterruptTransferOnPortChange },
        { PsmEventHubStatusChange ,     Psm20StateQueueingEnableInterruptTransferOnPortChange },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForPortChangeEvent = {
    // State ETW Name
    Psm20StateEtwWaitingForPortChangeEvent,
    // State Entry Function
    HUBPSM20_WaitingForPortChangeEvent,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm20StateInitializingCumulativePortChangeBits },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAcquiringPortReferenceOnStart = {
    // State ETW Name
    Psm20StateEtwAcquiringPortReferenceOnStart,
    // State Entry Function
    HUBPSM20_AcquiringPortReferenceOnStart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateGettingPortLostChangesOnStart },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryGettingPortLostChangesOnStart = {
    // State ETW Name
    Psm20StateEtwGettingPortLostChangesOnStart,
    // State Entry Function
    HUBPSM20_GettingPortLostChangesOnStart,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryGettingPortStatusOnStart = {
    // State ETW Name
    Psm20StateEtwGettingPortStatusOnStart,
    // State Entry Function
    HUBPSM20_GettingPortStatusOnStart,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetOnControlTransferFailureOnStart },
        { PsmEventTransferSuccess ,     Psm20StateAcquiringPortReferenceOnStart },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubResetOnControlTransferFailureOnStart = {
    // State ETW Name
    Psm20StateEtwIssuingHubResetOnControlTransferFailureOnStart,
    // State Entry Function
    HUBPSM20_IssuingHubResetOnControlTransferFailureOnStart,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventHubStatusChange ,     Psm20StateQueueingEnableInterruptTransferOnStart },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryPoweringOnPortOnStart = {
    // State ETW Name
    Psm20StateEtwPoweringOnPortOnStart,
    // State Entry Function
    HUBPSM20_PoweringOnPortOnStart,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetOnControlTransferFailureOnStart },
        { PsmEventTransferSuccess ,     Psm20StateWaitingForPortPowerOnTimerOnStart },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingEnableInterruptTransferOnStart = {
    // State ETW Name
    Psm20StateEtwQueueingEnableInterruptTransferOnStart,
    // State Entry Function
    HUBPSM20_QueueingEnableInterruptTransferOnStart,
    // State Flags
    StateFlagPopIfParentStateChanges|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventHubStatusChange ,     Psm20StateQueueingEnableInterruptTransferOnStart },
        { PsmEventHubStatusChange ,     Psm20StateQueueingEnableInterruptTransferOnStart },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForPortPowerOnTimerOnStart = {
    // State ETW Name
    Psm20StateEtwWaitingForPortPowerOnTimerOnStart,
    // State Entry Function
    HUBPSM20_WaitingForPortPowerOnTimerOnStart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventTimerFired ,          Psm20StateGettingPortStatusOnStart },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAckingConnectChangeOnResume = {
    // State ETW Name
    Psm20StateEtwAckingConnectChangeOnResume,
    // State Entry Function
    HUBPSM20_AckingConnectChangeOnResume,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm20StateQueueingStateDisabledOnFailure },
        { PsmEventTransferSuccess ,     Psm20StateGettingPortLostChangesOnStartOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAckingPortChangeWhileWaitingForReconnect = {
    // State ETW Name
    Psm20StateEtwAckingPortChangeWhileWaitingForReconnect,
    // State Entry Function
    HUBPSM20_AckingPortChangeWhileWaitingForReconnect,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm20StateResettingHubOnFailureWhileWaitingForReconnect },
        { PsmEventTransferSuccess ,     Psm20StateCheckIfThereIsAPortChangeOnPortStatusWhileWaitingForReconnect },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAcquiringPortReferenceOnResume = {
    // State ETW Name
    Psm20StateEtwAcquiringPortReferenceOnResume,
    // State Entry Function
    HUBPSM20_AcquiringPortReferenceOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateCheckingIfDeviceDisconnectedOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAcquiringPortReferenceOnResumeInFailure = {
    // State ETW Name
    Psm20StateEtwAcquiringPortReferenceOnResumeInFailure,
    // State Entry Function
    HUBPSM20_AcquiringPortReferenceOnResumeInFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateQueueingStateDisabledOnFailure },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckIfThereIsAPortChangeOnPortStatusWhileWaitingForReconnect = {
    // State ETW Name
    Psm20StateEtwCheckIfThereIsAPortChangeOnPortStatusWhileWaitingForReconnect,
    // State Entry Function
    HUBPSM20_CheckIfThereIsAPortChangeOnPortStatusWhileWaitingForReconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateAckingPortChangeWhileWaitingForReconnect },
        { PsmEventPortError ,           Psm20StateResettingHubOnFailureWhileWaitingForReconnect },
        { PsmEventNo ,                  Psm20StateEnablingInterruptsWaitingForReconnect },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIfConnectBitChangeSetOnResume = {
    // State ETW Name
    Psm20StateEtwCheckingIfConnectBitChangeSetOnResume,
    // State Entry Function
    HUBPSM20_CheckingIfConnectBitChangeSetOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateAckingConnectChangeOnResume },
        { PsmEventNo ,                  Psm20StateGettingPortLostChangesOnStartOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIfConnectBitIsOneOnChangeWhileWaitingForReconnect = {
    // State ETW Name
    Psm20StateEtwCheckingIfConnectBitIsOneOnChangeWhileWaitingForReconnect,
    // State Entry Function
    HUBPSM20_CheckingIfConnectBitIsOneOnChangeWhileWaitingForReconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateStoppingReconnectTimerOnDeviceConnect },
        { PsmEventNo ,                  Psm20StateWaitingForDeviceToReconnect },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIfDeviceDisconnectedOnResume = {
    // State ETW Name
    Psm20StateEtwCheckingIfDeviceDisconnectedOnResume,
    // State Entry Function
    HUBPSM20_CheckingIfDeviceDisconnectedOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateCheckingIfConnectBitChangeSetOnResume },
        { PsmEventNo ,                  Psm20StateStartingTimerForAllowingReconnect },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIfDeviceDisconnectedOnResumeAfterWaiting = {
    // State ETW Name
    Psm20StateEtwCheckingIfDeviceDisconnectedOnResumeAfterWaiting,
    // State Entry Function
    HUBPSM20_CheckingIfDeviceDisconnectedOnResumeAfterWaiting,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateCheckingIfConnectBitChangeSetOnResume },
        { PsmEventNo ,                  Psm20StateGettingPortLostChangesOnStartOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryEnablingInterruptsWaitingForReconnect = {
    // State ETW Name
    Psm20StateEtwEnablingInterruptsWaitingForReconnect,
    // State Entry Function
    HUBPSM20_EnablingInterruptsWaitingForReconnect,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateCheckingIfConnectBitIsOneOnChangeWhileWaitingForReconnect },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryGettingPortLostChangesOnStartOnResume = {
    // State ETW Name
    Psm20StateEtwGettingPortLostChangesOnStartOnResume,
    // State Entry Function
    HUBPSM20_GettingPortLostChangesOnStartOnResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryGettingPortStatusOnResume = {
    // State ETW Name
    Psm20StateEtwGettingPortStatusOnResume,
    // State Entry Function
    HUBPSM20_GettingPortStatusOnResume,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm20StateAcquiringPortReferenceOnResumeInFailure },
        { PsmEventTransferSuccess ,     Psm20StateAcquiringPortReferenceOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryGettingPortStatusOnResumeAfterWaiting = {
    // State ETW Name
    Psm20StateEtwGettingPortStatusOnResumeAfterWaiting,
    // State Entry Function
    HUBPSM20_GettingPortStatusOnResumeAfterWaiting,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm20StateQueueingStateDisabledOnFailure },
        { PsmEventTransferSuccess ,     Psm20StateCheckingIfDeviceDisconnectedOnResumeAfterWaiting },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryGettingPortStatusWhileWaitingForReconnect = {
    // State ETW Name
    Psm20StateEtwGettingPortStatusWhileWaitingForReconnect,
    // State Entry Function
    HUBPSM20_GettingPortStatusWhileWaitingForReconnect,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm20StateCheckIfThereIsAPortChangeOnPortStatusWhileWaitingForReconnect },
        { PsmEventTransferFailure ,     Psm20StateResettingHubOnFailureWhileWaitingForReconnect },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryInitializingCumulativePortChangeBitsWhileWaitingForReconnect = {
    // State ETW Name
    Psm20StateEtwInitializingCumulativePortChangeBitsWhileWaitingForReconnect,
    // State Entry Function
    HUBPSM20_InitializingCumulativePortChangeBitsWhileWaitingForReconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateGettingPortStatusWhileWaitingForReconnect },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubResetOnControlTransferFailureOnResume = {
    // State ETW Name
    Psm20StateEtwIssuingHubResetOnControlTransferFailureOnResume,
    // State Entry Function
    HUBPSM20_IssuingHubResetOnControlTransferFailureOnResume,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventHubStatusChange ,     Psm20StateQueueingEnableInterruptTransferOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryPoweringOnPortOnResume = {
    // State ETW Name
    Psm20StateEtwPoweringOnPortOnResume,
    // State Entry Function
    HUBPSM20_PoweringOnPortOnResume,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm20StateAcquiringPortReferenceOnResumeInFailure },
        { PsmEventTransferSuccess ,     Psm20StateWaitingForPortPowerOnTimerOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingEnableInterruptTransferOnResume = {
    // State ETW Name
    Psm20StateEtwQueueingEnableInterruptTransferOnResume,
    // State Entry Function
    HUBPSM20_QueueingEnableInterruptTransferOnResume,
    // State Flags
    StateFlagPopIfParentStateChanges|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventHubStatusChange ,     Psm20StateQueueingEnableInterruptTransferOnResume },
        { PsmEventHubStatusChange ,     Psm20StateQueueingEnableInterruptTransferOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingStateDisabledOnFailure = {
    // State ETW Name
    Psm20StateEtwQueueingStateDisabledOnFailure,
    // State Entry Function
    HUBPSM20_QueueingStateDisabledOnFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateIssuingHubResetOnControlTransferFailureOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryResettingHubOnFailureWhileWaitingForReconnect = {
    // State ETW Name
    Psm20StateEtwResettingHubOnFailureWhileWaitingForReconnect,
    // State Entry Function
    HUBPSM20_ResettingHubOnFailureWhileWaitingForReconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateQueueingEnableInterruptTransferOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReturningHubStopOnResume = {
    // State ETW Name
    Psm20StateEtwReturningHubStopOnResume,
    // State Entry Function
    HUBPSM20_ReturningHubStopOnResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingTimerForAllowingReconnect = {
    // State ETW Name
    Psm20StateEtwStartingTimerForAllowingReconnect,
    // State Entry Function
    HUBPSM20_StartingTimerForAllowingReconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForDeviceToReconnect },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingReconnectTimerOnDeviceConnect = {
    // State ETW Name
    Psm20StateEtwStoppingReconnectTimerOnDeviceConnect,
    // State Entry Function
    HUBPSM20_StoppingReconnectTimerOnDeviceConnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateGettingPortLostChangesOnStartOnResume },
        { PsmEventOperationFailure ,    Psm20StateWaitingForDeviceToReconnect },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingReconnectTimerOnHubStop = {
    // State ETW Name
    Psm20StateEtwStoppingReconnectTimerOnHubStop,
    // State Entry Function
    HUBPSM20_StoppingReconnectTimerOnHubStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForReconnectTimerToFlushOnHubStop },
        { PsmEventOperationSuccess ,    Psm20StateReturningHubStopOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDeviceToReconnect = {
    // State ETW Name
    Psm20StateEtwWaitingForDeviceToReconnect,
    // State Entry Function
    HUBPSM20_WaitingForDeviceToReconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm20StateInitializingCumulativePortChangeBitsWhileWaitingForReconnect },
        { PsmEventHubStop ,             Psm20StateStoppingReconnectTimerOnHubStop },
        { PsmEventTimerFired ,          Psm20StateGettingPortStatusOnResumeAfterWaiting },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForPortPowerOnTimerOnResume = {
    // State ETW Name
    Psm20StateEtwWaitingForPortPowerOnTimerOnResume,
    // State Entry Function
    HUBPSM20_WaitingForPortPowerOnTimerOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTimerFired ,          Psm20StateGettingPortStatusOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForReconnectTimerToFlushOnHubStop = {
    // State ETW Name
    Psm20StateEtwWaitingForReconnectTimerToFlushOnHubStop,
    // State Entry Function
    HUBPSM20_WaitingForReconnectTimerToFlushOnHubStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTimerFired ,          Psm20StateReturningHubStopOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAckingPortChangeInSuspended = {
    // State ETW Name
    Psm20StateEtwAckingPortChangeInSuspended,
    // State Entry Function
    HUBPSM20_AckingPortChangeInSuspended,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetOnControlTransferFailureInSuspended },
        { PsmEventTransferSuccess ,     Psm20StateCheckIfThereIsAPortChangeOnSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAcquiringPowerReferenceOnHubS0IdleInD3Cold = {
    // State ETW Name
    Psm20StateEtwAcquiringPowerReferenceOnHubS0IdleInD3Cold,
    // State Entry Function
    HUBPSM20_AcquiringPowerReferenceOnHubS0IdleInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForPortChangeEventInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckIfThereIsAPortChangeAfterInitialGetPortStatusOnSuspended = {
    // State ETW Name
    Psm20StateEtwCheckIfThereIsAPortChangeAfterInitialGetPortStatusOnSuspended,
    // State Entry Function
    HUBPSM20_CheckIfThereIsAPortChangeAfterInitialGetPortStatusOnSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateAckingPortChangeInSuspended },
        { PsmEventPortError ,           Psm20StateIssuingHubResetOnControlTransferFailureInSuspended },
        { PsmEventNo ,                  Psm20StateGettingPortChangeEventInSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckIfThereIsAPortChangeOnSuspended = {
    // State ETW Name
    Psm20StateEtwCheckIfThereIsAPortChangeOnSuspended,
    // State Entry Function
    HUBPSM20_CheckIfThereIsAPortChangeOnSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortError ,           Psm20StateIssuingHubResetOnControlTransferFailureInSuspended },
        { PsmEventYes ,                 Psm20StateAckingPortChangeInSuspended },
        { PsmEventNo ,                  Psm20StateGettingPortStatusInSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold = {
    // State ETW Name
    Psm20StateEtwCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold,
    // State Entry Function
    HUBPSM20_CheckingIfDeviceIsConnectedAfterHubResumeInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateQueueingPortStateDisabledOnHubResumeInD3Cold },
        { PsmEventNo ,                  Psm20StateReturningPortConnectChangeFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold = {
    // State ETW Name
    Psm20StateEtwCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold,
    // State Entry Function
    HUBPSM20_CheckingIfDeviceIsConnectedOnConnectChangeForD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateStartingDebounceTimerOnResumeFromD3Cold },
        { PsmEventNo ,                  Psm20StateWaitingForPortChangeEventInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIfDeviceIsConnectedOnPortChangeInD3Cold = {
    // State ETW Name
    Psm20StateEtwCheckingIfDeviceIsConnectedOnPortChangeInD3Cold,
    // State Entry Function
    HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangeInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateStartingDebounceTimerOnResumeFromD3Cold },
        { PsmEventNo ,                  Psm20StateWaitingForPortChangeEventInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIfDeviceIsConnectedOnPortChangeInD3ColdOnResume = {
    // State ETW Name
    Psm20StateEtwCheckingIfDeviceIsConnectedOnPortChangeInD3ColdOnResume,
    // State Entry Function
    HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangeInD3ColdOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateStartingDebounceTimerOnResumeFromD3ColdOnResume },
        { PsmEventNo ,                  Psm20StateReturningPortConnectChangeFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume = {
    // State ETW Name
    Psm20StateEtwCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume,
    // State Entry Function
    HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateStoppingReconnectTimerOnDeviceReAttachAfterD3Cold },
        { PsmEventNo ,                  Psm20StateWaitingForDeviceToReconnectOnResumeFromD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckingIsD3ColdIsEnabled = {
    // State ETW Name
    Psm20StateEtwCheckingIsD3ColdIsEnabled,
    // State Entry Function
    HUBPSM20_CheckingIsD3ColdIsEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold },
        { PsmEventNo ,                  Psm20StateReturningPortConnectChangeFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryGettingPortChangeEventInSuspended = {
    // State ETW Name
    Psm20StateEtwGettingPortChangeEventInSuspended,
    // State Entry Function
    HUBPSM20_GettingPortChangeEventInSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetComplete ,   Psm20StateReEnablingInterruptsOnErrorInSuspended },
        { PsmEventPortError ,           Psm20StateReEnablingInterruptsOnErrorInSuspended },
        { PsmEventPortOverCurrentCleared ,Psm20StateReEnablingInterruptsOnOverCurrentClearedInSuspended },
        { PsmEventPortResumed ,         Psm20StateWaitingForResumeRecoveryTimerOnResumeInSuspended },
        { PsmEventPortDisabled ,        Psm20StateReEnablingInterruptsOnErrorInSuspended },
        { PsmEventPortConnectChange ,   Psm20StateReEnablingInterruptsOnConnectChangeInSuspended },
        { PsmEventPortOverCurrent ,     Psm20StateReEnablingInterruptsOnOverCurrentInSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryGettingPortChangeOnResumeFromD3ColdSuspended = {
    // State ETW Name
    Psm20StateEtwGettingPortChangeOnResumeFromD3ColdSuspended,
    // State Entry Function
    HUBPSM20_GettingPortChangeOnResumeFromD3ColdSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmGettingPortChangeOnResumeFromD3ColdSuspended,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold },
        { PsmEventPortConnectChange ,   Psm20StateCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold },
        { PsmEventPortError ,           Psm20StateCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold },
        { PsmEventPortResumed ,         Psm20StateCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold },
        { PsmEventPortResetComplete ,   Psm20StateCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold },
        { PsmEventPortOverCurrentCleared ,Psm20StateReturningPortOverCurrentClearedFromSuspended },
        { PsmEventPortOverCurrent ,     Psm20StateReturningPortOverCurrentFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryGettingPortStatusInSuspended = {
    // State ETW Name
    Psm20StateEtwGettingPortStatusInSuspended,
    // State Entry Function
    HUBPSM20_GettingPortStatusInSuspended,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm20StateIssuingHubResetOnControlTransferFailureInSuspended },
        { PsmEventTransferSuccess ,     Psm20StateCheckIfThereIsAPortChangeAfterInitialGetPortStatusOnSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryInitializingCumulativePortChangeBitsInSuspended = {
    // State ETW Name
    Psm20StateEtwInitializingCumulativePortChangeBitsInSuspended,
    // State Entry Function
    HUBPSM20_InitializingCumulativePortChangeBitsInSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateGettingPortStatusInSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryIssuingHubResetOnControlTransferFailureInSuspended = {
    // State ETW Name
    Psm20StateEtwIssuingHubResetOnControlTransferFailureInSuspended,
    // State Entry Function
    HUBPSM20_IssuingHubResetOnControlTransferFailureInSuspended,
    // State Flags
    StateFlagPopIfParentStateChanges|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm20StateQueueingEnableInterruptTransferOnPortChangeForSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryPoweredOffOnHubSuspendFromD3Cold = {
    // State ETW Name
    Psm20StateEtwPoweredOffOnHubSuspendFromD3Cold,
    // State Entry Function
    HUBPSM20_PoweredOffOnHubSuspendFromD3Cold,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubResumeWithReset ,  Psm20StatePoweringOnResumeFromD3ColdSuspended },
        { PsmEventHubResumeInS0 ,       Psm20StateAcquiringPowerReferenceOnHubS0IdleInD3Cold },
        { PsmEventHubResume ,           Psm20StateGettingPortChangeOnResumeFromD3ColdSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryPoweringOnResumeFromD3ColdSuspended = {
    // State ETW Name
    Psm20StateEtwPoweringOnResumeFromD3ColdSuspended,
    // State Entry Function
    HUBPSM20_PoweringOnResumeFromD3ColdSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmPoweringOnResumeFromD3ColdSuspended,
    // Event State Pairs for Transitions
    {
        { PsmEventPortOverCurrentCleared ,Psm20StateReturningPortOverCurrentClearedFromSuspended },
        { PsmEventPortOverCurrent ,     Psm20StateReturningPortOverCurrentFromSuspended },
        { PsmEventOperationSuccess ,    Psm20StateCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold },
        { PsmEventPortConnectChange ,   Psm20StateCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold },
        { PsmEventPortError ,           Psm20StateCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold },
        { PsmEventPortResumed ,         Psm20StateCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold },
        { PsmEventPortResetComplete ,   Psm20StateCheckingIfDeviceIsConnectedAfterHubResumeInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingDisabledToDSMOnReconnectFromD3Cold = {
    // State ETW Name
    Psm20StateEtwQueueingDisabledToDSMOnReconnectFromD3Cold,
    // State Entry Function
    HUBPSM20_QueueingDisabledToDSMOnReconnectFromD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReturningPortResumedFromD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingEnableInterruptTransferOnPortChangeForSuspended = {
    // State ETW Name
    Psm20StateEtwQueueingEnableInterruptTransferOnPortChangeForSuspended,
    // State Entry Function
    HUBPSM20_QueueingEnableInterruptTransferOnPortChangeForSuspended,
    // State Flags
    StateFlagPopIfParentStateChanges|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm20StateQueueingEnableInterruptTransferOnPortChangeForSuspended },
        { PsmEventHubStatusChange ,     Psm20StateQueueingEnableInterruptTransferOnPortChangeForSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingPortStateDisabledOnHubResumeInD3Cold = {
    // State ETW Name
    Psm20StateEtwQueueingPortStateDisabledOnHubResumeInD3Cold,
    // State Entry Function
    HUBPSM20_QueueingPortStateDisabledOnHubResumeInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReturningPortResumedFromD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingResumeToDSMFromSuspended = {
    // State ETW Name
    Psm20StateEtwQueueingResumeToDSMFromSuspended,
    // State Entry Function
    HUBPSM20_QueueingResumeToDSMFromSuspended,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReturningPortResumedFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReEnablingInterruptsOnConnectChangeInSuspended = {
    // State ETW Name
    Psm20StateEtwReEnablingInterruptsOnConnectChangeInSuspended,
    // State Entry Function
    HUBPSM20_ReEnablingInterruptsOnConnectChangeInSuspended,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateCheckingIsD3ColdIsEnabled },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReEnablingInterruptsOnErrorInSuspended = {
    // State ETW Name
    Psm20StateEtwReEnablingInterruptsOnErrorInSuspended,
    // State Entry Function
    HUBPSM20_ReEnablingInterruptsOnErrorInSuspended,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReturningPortErrorFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReEnablingInterruptsOnOverCurrentClearedInSuspended = {
    // State ETW Name
    Psm20StateEtwReEnablingInterruptsOnOverCurrentClearedInSuspended,
    // State Entry Function
    HUBPSM20_ReEnablingInterruptsOnOverCurrentClearedInSuspended,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReturningPortOverCurrentClearedFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReEnablingInterruptsOnOverCurrentInSuspended = {
    // State ETW Name
    Psm20StateEtwReEnablingInterruptsOnOverCurrentInSuspended,
    // State Entry Function
    HUBPSM20_ReEnablingInterruptsOnOverCurrentInSuspended,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReturningPortOverCurrentFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReleasingPowerReferenceOnHubS0IdleInD3Cold = {
    // State ETW Name
    Psm20StateEtwReleasingPowerReferenceOnHubS0IdleInD3Cold,
    // State Entry Function
    HUBPSM20_ReleasingPowerReferenceOnHubS0IdleInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StatePoweredOffOnHubSuspendFromD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReturningHubStopFromSuspendedInD3Cold = {
    // State ETW Name
    Psm20StateEtwReturningHubStopFromSuspendedInD3Cold,
    // State Entry Function
    HUBPSM20_ReturningHubStopFromSuspendedInD3Cold,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReturningHubSuspendFromSuspendedInD3Cold = {
    // State ETW Name
    Psm20StateEtwReturningHubSuspendFromSuspendedInD3Cold,
    // State Entry Function
    HUBPSM20_ReturningHubSuspendFromSuspendedInD3Cold,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReturningPortConnectChangeFromSuspended = {
    // State ETW Name
    Psm20StateEtwReturningPortConnectChangeFromSuspended,
    // State Entry Function
    HUBPSM20_ReturningPortConnectChangeFromSuspended,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReturningPortErrorFromSuspended = {
    // State ETW Name
    Psm20StateEtwReturningPortErrorFromSuspended,
    // State Entry Function
    HUBPSM20_ReturningPortErrorFromSuspended,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReturningPortOverCurrentClearedFromSuspended = {
    // State ETW Name
    Psm20StateEtwReturningPortOverCurrentClearedFromSuspended,
    // State Entry Function
    HUBPSM20_ReturningPortOverCurrentClearedFromSuspended,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReturningPortOverCurrentFromSuspended = {
    // State ETW Name
    Psm20StateEtwReturningPortOverCurrentFromSuspended,
    // State Entry Function
    HUBPSM20_ReturningPortOverCurrentFromSuspended,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReturningPortResumedFromD3Cold = {
    // State ETW Name
    Psm20StateEtwReturningPortResumedFromD3Cold,
    // State Entry Function
    HUBPSM20_ReturningPortResumedFromD3Cold,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryReturningPortResumedFromSuspended = {
    // State ETW Name
    Psm20StateEtwReturningPortResumedFromSuspended,
    // State Entry Function
    HUBPSM20_ReturningPortResumedFromSuspended,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingDebounceTimerOnResumeFromD3Cold = {
    // State ETW Name
    Psm20StateEtwStartingDebounceTimerOnResumeFromD3Cold,
    // State Entry Function
    HUBPSM20_StartingDebounceTimerOnResumeFromD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForDebounceTimerOnReconnectInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingDebounceTimerOnResumeFromD3ColdOnResume = {
    // State ETW Name
    Psm20StateEtwStartingDebounceTimerOnResumeFromD3ColdOnResume,
    // State Entry Function
    HUBPSM20_StartingDebounceTimerOnResumeFromD3ColdOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForDebounceTimerOnReconnectInD3ColdOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStartingTimerForAllowingReconnectOnResumingFromD3Cold = {
    // State ETW Name
    Psm20StateEtwStartingTimerForAllowingReconnectOnResumingFromD3Cold,
    // State Entry Function
    HUBPSM20_StartingTimerForAllowingReconnectOnResumingFromD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForDeviceToReconnectOnResumeFromD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingDebounceTimerInD3Cold = {
    // State ETW Name
    Psm20StateEtwStoppingDebounceTimerInD3Cold,
    // State Entry Function
    HUBPSM20_StoppingDebounceTimerInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForDebounceTimerToFlushOnPortChangeInD3Cold },
        { PsmEventOperationSuccess ,    Psm20StateCheckingIfDeviceIsConnectedOnPortChangeInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingDebounceTimerInD3ColdOnResume = {
    // State ETW Name
    Psm20StateEtwStoppingDebounceTimerInD3ColdOnResume,
    // State Entry Function
    HUBPSM20_StoppingDebounceTimerInD3ColdOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForDebounceTimerToFlushOnPortChangeInD3ColdOnResume },
        { PsmEventOperationSuccess ,    Psm20StateCheckingIfDeviceIsConnectedOnPortChangeInD3ColdOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingReconnectTimerOnDeviceReAttachAfterD3Cold = {
    // State ETW Name
    Psm20StateEtwStoppingReconnectTimerOnDeviceReAttachAfterD3Cold,
    // State Entry Function
    HUBPSM20_StoppingReconnectTimerOnDeviceReAttachAfterD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateStartingDebounceTimerOnResumeFromD3ColdOnResume },
        { PsmEventOperationFailure ,    Psm20StateWaitingForReconnectTimerToFlushOnReattachAfterD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingReconnectTimerOnHubStopInD3Cold = {
    // State ETW Name
    Psm20StateEtwStoppingReconnectTimerOnHubStopInD3Cold,
    // State Entry Function
    HUBPSM20_StoppingReconnectTimerOnHubStopInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm20StateWaitingForReconnectTimerToFlushOnHubStopInD3Cold },
        { PsmEventOperationSuccess ,    Psm20StateReturningHubStopFromSuspendedInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingReconnectTimerOnOvercurrentClearedInD3Cold = {
    // State ETW Name
    Psm20StateEtwStoppingReconnectTimerOnOvercurrentClearedInD3Cold,
    // State Entry Function
    HUBPSM20_StoppingReconnectTimerOnOvercurrentClearedInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReturningPortOverCurrentClearedFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryStoppingReconnectTimerOnOvercurrentInD3Cold = {
    // State ETW Name
    Psm20StateEtwStoppingReconnectTimerOnOvercurrentInD3Cold,
    // State Entry Function
    HUBPSM20_StoppingReconnectTimerOnOvercurrentInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateReturningPortOverCurrentFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDebounceTimerOnReconnectInD3Cold = {
    // State ETW Name
    Psm20StateEtwWaitingForDebounceTimerOnReconnectInD3Cold,
    // State Entry Function
    HUBPSM20_WaitingForDebounceTimerOnReconnectInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForDebounceTimerOnReconnectInD3Cold,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortError ,           Psm20StateStoppingDebounceTimerInD3Cold },
        { PsmEventPortDisabled ,        Psm20StateStoppingDebounceTimerInD3Cold },
        { PsmEventPortConnectChange ,   Psm20StateStoppingDebounceTimerInD3Cold },
        { PsmEventPortResumed ,         Psm20StateStoppingDebounceTimerInD3Cold },
        { PsmEventPortResetComplete ,   Psm20StateStoppingDebounceTimerInD3Cold },
        { PsmEventPortOverCurrent ,     Psm20StateStoppingReconnectTimerOnOvercurrentInD3Cold },
        { PsmEventPortOverCurrentCleared ,Psm20StateStoppingReconnectTimerOnOvercurrentClearedInD3Cold },
        { PsmEventTimerFired ,          Psm20StateWaitingForDeviceRequestResumeOnD3Cold },
        { PsmEventHubStop ,             Psm20StateWaitingForDebounceTimerToFlushOnHubStop },
        { PsmEventHubSuspend ,          Psm20StateWaitingForDebounceTimerToFlushOnHubSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDebounceTimerOnReconnectInD3ColdOnResume = {
    // State ETW Name
    Psm20StateEtwWaitingForDebounceTimerOnReconnectInD3ColdOnResume,
    // State Entry Function
    HUBPSM20_WaitingForDebounceTimerOnReconnectInD3ColdOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForDebounceTimerOnReconnectInD3ColdOnResume,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventPortResetComplete ,   Psm20StateStoppingDebounceTimerInD3ColdOnResume },
        { PsmEventPortResumed ,         Psm20StateStoppingDebounceTimerInD3ColdOnResume },
        { PsmEventPortDisabled ,        Psm20StateStoppingDebounceTimerInD3ColdOnResume },
        { PsmEventPortError ,           Psm20StateStoppingDebounceTimerInD3ColdOnResume },
        { PsmEventPortConnectChange ,   Psm20StateStoppingDebounceTimerInD3ColdOnResume },
        { PsmEventPortOverCurrent ,     Psm20StateStoppingReconnectTimerOnOvercurrentInD3Cold },
        { PsmEventPortOverCurrentCleared ,Psm20StateStoppingReconnectTimerOnOvercurrentClearedInD3Cold },
        { PsmEventTimerFired ,          Psm20StateQueueingDisabledToDSMOnReconnectFromD3Cold },
        { PsmEventHubStop ,             Psm20StateWaitingForDebounceTimerToFlushOnHubStopOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDebounceTimerToFlushOnHubStop = {
    // State ETW Name
    Psm20StateEtwWaitingForDebounceTimerToFlushOnHubStop,
    // State Entry Function
    HUBPSM20_WaitingForDebounceTimerToFlushOnHubStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTimerFired ,          Psm20StateReturningHubStopFromSuspendedInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDebounceTimerToFlushOnHubStopOnResume = {
    // State ETW Name
    Psm20StateEtwWaitingForDebounceTimerToFlushOnHubStopOnResume,
    // State Entry Function
    HUBPSM20_WaitingForDebounceTimerToFlushOnHubStopOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTimerFired ,          Psm20StateReturningHubStopFromSuspendedInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDebounceTimerToFlushOnHubSuspend = {
    // State ETW Name
    Psm20StateEtwWaitingForDebounceTimerToFlushOnHubSuspend,
    // State Entry Function
    HUBPSM20_WaitingForDebounceTimerToFlushOnHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTimerFired ,          Psm20StateReturningHubSuspendFromSuspendedInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDebounceTimerToFlushOnPortChangeInD3Cold = {
    // State ETW Name
    Psm20StateEtwWaitingForDebounceTimerToFlushOnPortChangeInD3Cold,
    // State Entry Function
    HUBPSM20_WaitingForDebounceTimerToFlushOnPortChangeInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTimerFired ,          Psm20StateCheckingIfDeviceIsConnectedOnPortChangeInD3Cold },
        { PsmEventHubSuspend ,          Psm20StateWaitingForDebounceTimerToFlushOnHubSuspend },
        { PsmEventHubStop ,             Psm20StateWaitingForDebounceTimerToFlushOnHubStop },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDebounceTimerToFlushOnPortChangeInD3ColdOnResume = {
    // State ETW Name
    Psm20StateEtwWaitingForDebounceTimerToFlushOnPortChangeInD3ColdOnResume,
    // State Entry Function
    HUBPSM20_WaitingForDebounceTimerToFlushOnPortChangeInD3ColdOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm20StateIgnored },
        { PsmEventTimerFired ,          Psm20Stat