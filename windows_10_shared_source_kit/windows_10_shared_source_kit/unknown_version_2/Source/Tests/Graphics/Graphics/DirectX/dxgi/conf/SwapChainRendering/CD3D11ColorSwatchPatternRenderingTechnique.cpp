
	"mov r0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_034_Desc = "ps_1_4 : mul source reg combination v0.rbg, c0 is NOT allowed";
string PS_034 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.rbg, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_035_Desc = "ps_1_4 : mul source reg combination v0.gar, c0 is NOT allowed";
string PS_035 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.gar, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_036_Desc = "ps_1_4 : mul source reg combination v0.gr, c0 is NOT allowed";
string PS_036 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.gr, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_037_Desc = "ps_1_4 : mul source reg combination v0.ab, c0 is NOT allowed";
string PS_037 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.ab, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_038_Desc = "ps_1_4 : mul source reg combination v0.bg, c0 is NOT allowed";
string PS_038 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.bg, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_1_4 : mul source reg combination v0, c0.gbra is NOT allowed";
string PS_039 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.gbra "
	"mov r0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_1_4 : mul source reg combination v0, c0.brga is NOT allowed";
string PS_040 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.brga "
	"mov r0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_1_4 : mul source reg combination v0, c0.abgr is NOT allowed";
string PS_041 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.abgr "
	"mov r0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_042_Desc = "ps_1_4 : mul source reg combination v0, c0.agrb is NOT allowed";
string PS_042 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.agrb "
	"mov r0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_043_Desc = "ps_1_4 : mul source reg combination v0, c0.rbga is NOT allowed";
string PS_043 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.rbga "
	"mov r0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_044_Desc = "ps_1_4 : mul source reg combination v0, c0.rgab is NOT allowed";
string PS_044 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.rgab "
	"mov r0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_045_Desc = "ps_1_4 : mul source reg combination v0, c0.bgr is NOT allowed";
string PS_045 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.bgr "
	"mov r0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_046_Desc = "ps_1_4 : mul source reg combination v0, c0.rbg is NOT allowed";
string PS_046 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.rbg "
	"mov r0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_047_Desc = "ps_1_4 : mul source reg combination v0, c0.gar is NOT allowed";
string PS_047 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.gar "
	"mov r0, c0 ";

PS_CRITERIA PS_048_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_048_Desc = "ps_1_4 : mul source reg combination v0, c0.gr is NOT allowed";
string PS_048 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.gr "
	"mov r0, c0 ";

PS_CRITERIA PS_049_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_049_Desc = "ps_1_4 : mul source reg combination v0, c0.ab is NOT allowed";
string PS_049 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.ab "
	"mov r0, c0 ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_050_Desc = "ps_1_4 : mul source reg combination v0, c0.bg is NOT allowed";
string PS_050 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.bg "
	"mov r0, c0 ";

PS_CRITERIA PS_051_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_1_4 : mul dest r# is allowed";
string PS_051 = 
	"ps_1_4 "
	"mul r0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_052_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_1_4 : mul _x2 dest r# is allowed";
string PS_052 = 
	"ps_1_4 "
	"mul_x2 r0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_053_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_1_4 : mul _x4 dest r# is allowed";
string PS_053 = 
	"ps_1_4 "
	"mul_x4 r0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_054_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_1_4 : mul _x8 dest r# is allowed";
string PS_054 = 
	"ps_1_4 "
	"mul_x8 r0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_055_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_1_4 : mul _d2 dest r# is allowed";
string PS_055 = 
	"ps_1_4 "
	"mul_d2 r0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_056_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_1_4 : mul _d4 dest r# is allowed";
string PS_056 = 
	"ps_1_4 "
	"mul_d4 r0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_057_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_1_4 : mul _d8 dest r# is allowed";
string PS_057 = 
	"ps_1_4 "
	"mul_d8 r0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_058_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_1_4 : mul _sat dest r# is allowed";
string PS_058 = 
	"ps_1_4 "
	"mul_sat r0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_059_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_1_4 : mul dest v# is NOT allowed";
string PS_059 = 
	"ps_1_4 "
	"mul v0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_060_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_1_4 : mul _x2 dest v# is NOT allowed";
string PS_060 = 
	"ps_1_4 "
	"mul_x2 v0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_061_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_1_4 : mul _x4 dest v# is NOT allowed";
string PS_061 = 
	"ps_1_4 "
	"mul_x4 v0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_062_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_1_4 : mul _x8 dest v# is NOT allowed";
string PS_062 = 
	"ps_1_4 "
	"mul_x8 v0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_063_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_1_4 : mul _d2 dest v# is NOT allowed";
string PS_063 = 
	"ps_1_4 "
	"mul_d2 v0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_064_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_1_4 : mul _d4 dest v# is NOT allowed";
string PS_064 = 
	"ps_1_4 "
	"mul_d4 v0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_065_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_1_4 : mul _d8 dest v# is NOT allowed";
string PS_065 = 
	"ps_1_4 "
	"mul_d8 v0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_066_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_1_4 : mul _sat dest v# is NOT allowed";
string PS_066 = 
	"ps_1_4 "
	"mul_sat v0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_067_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_1_4 : mul dest c# is NOT allowed";
string PS_067 = 
	"ps_1_4 "
	"mul c1, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_068_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_1_4 : mul _x2 dest c# is NOT allowed";
string PS_068 = 
	"ps_1_4 "
	"mul_x2 c1, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_069_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_1_4 : mul _x4 dest c# is NOT allowed";
string PS_069 = 
	"ps_1_4 "
	"mul_x4 c1, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_070_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_1_4 : mul _x8 dest c# is NOT allowed";
string PS_070 = 
	"ps_1_4 "
	"mul_x8 c1, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_071_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_1_4 : mul _d2 dest c# is NOT allowed";
string PS_071 = 
	"ps_1_4 "
	"mul_d2 c1, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_072_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_1_4 : mul _d4 dest c# is NOT allowed";
string PS_072 = 
	"ps_1_4 "
	"mul_d4 c1, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_073_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_1_4 : mul _d8 dest c# is NOT allowed";
string PS_073 = 
	"ps_1_4 "
	"mul_d8 c1, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_074_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_1_4 : mul _sat dest c# is NOT allowed";
string PS_074 = 
	"ps_1_4 "
	"mul_sat c1, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_075_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_1_4 : mul dest t# is NOT allowed";
string PS_075 = 
	"ps_1_4 "
	"mul t0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_076_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_1_4 : mul _x2 dest t# is NOT allowed";
string PS_076 = 
	"ps_1_4 "
	"mul_x2 t0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_077_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_1_4 : mul _x4 dest t# is NOT allowed";
string PS_077 = 
	"ps_1_4 "
	"mul_x4 t0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_078_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_1_4 : mul _x8 dest t# is NOT allowed";
string PS_078 = 
	"ps_1_4 "
	"mul_x8 t0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_079_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_1_4 : mul _d2 dest t# is NOT allowed";
string PS_079 = 
	"ps_1_4 "
	"mul_d2 t0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_080_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_1_4 : mul _d4 dest t# is NOT allowed";
string PS_080 = 
	"ps_1_4 "
	"mul_d4 t0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_081_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_1_4 : mul _d8 dest t# is NOT allowed";
string PS_081 = 
	"ps_1_4 "
	"mul_d8 t0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_082_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_1_4 : mul _sat dest t# is NOT allowed";
string PS_082 = 
	"ps_1_4 "
	"mul_sat t0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_083_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_1_4 : mul write mask .r is allowed";
string PS_083 = 
	"ps_1_4 "
	"mul r0.r, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_084_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_1_4 : mul write mask .g is allowed";
string PS_084 = 
	"ps_1_4 "
	"mul r0.g, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_085_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_1_4 : mul write mask .b is allowed";
string PS_085 = 
	"ps_1_4 "
	"mul r0.b, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_086_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_1_4 : mul write mask .a is allowed";
string PS_086 = 
	"ps_1_4 "
	"mul r0.a, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_087_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_1_4 : mul write mask .rg is allowed";
string PS_087 = 
	"ps_1_4 "
	"mul r0.rg, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_088_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_1_4 : mul write mask .gb is allowed";
string PS_088 = 
	"ps_1_4 "
	"mul r0.gb, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_089_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_1_4 : mul write mask .ba is allowed";
string PS_089 = 
	"ps_1_4 "
	"mul r0.ba, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_090_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_1_4 : mul write mask .rb is allowed";
string PS_090 = 
	"ps_1_4 "
	"mul r0.rb, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_091_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_1_4 : mul write mask .ra is allowed";
string PS_091 = 
	"ps_1_4 "
	"mul r0.ra, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_092_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_1_4 : mul write mask .ga is allowed";
string PS_092 = 
	"ps_1_4 "
	"mul r0.ga, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_093_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_1_4 : mul write mask .rgb is allowed";
string PS_093 = 
	"ps_1_4 "
	"mul r0.rgb, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_094_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_1_4 : mul write mask .gba is allowed";
string PS_094 = 
	"ps_1_4 "
	"mul r0.gba, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_095_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_1_4 : mul write mask .rba is allowed";
string PS_095 = 
	"ps_1_4 "
	"mul r0.rba, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_096_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_1_4 : mul write mask .rgba is allowed";
string PS_096 = 
	"ps_1_4 "
	"mul r0.rgba, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_097_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_1_4 : mul write mask .yx is NOT allowed";
string PS_097 = 
	"ps_1_4 "
	"mul r0.yx, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_098_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_1_4 : mul write mask .zx is NOT allowed";
string PS_098 = 
	"ps_1_4 "
	"mul r0.zx, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_099_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_1_4 : mul write mask .zy is NOT allowed";
string PS_099 = 
	"ps_1_4 "
	"mul r0.zy, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_100_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_1_4 : mul write mask .wx is NOT allowed";
string PS_100 = 
	"ps_1_4 "
	"mul r0.wx, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_101_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_1_4 : mul write mask .wz is NOT allowed";
string PS_101 = 
	"ps_1_4 "
	"mul r0.wz, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_102_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_1_4 : mul write mask .wy is NOT allowed";
string PS_102 = 
	"ps_1_4 "
	"mul r0.wy, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_103_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_1_4 : mul write mask .zyx is NOT allowed";
string PS_103 = 
	"ps_1_4 "
	"mul r0.zyx, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_104_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_1_4 : mul write mask .wzy is NOT allowed";
string PS_104 = 
	"ps_1_4 "
	"mul r0.wzy, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_105_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_105_Desc = "ps_1_4 : mul write mask .wzx is NOT allowed";
string PS_105 = 
	"ps_1_4 "
	"mul r0.wzx, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_106_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_106_Desc = "ps_1_4 : mul write mask .wyx is NOT allowed";
string PS_106 = 
	"ps_1_4 "
	"mul r0.wyx, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_107_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_107_Desc = "ps_1_4 : mul write mask .yxw is NOT allowed";
string PS_107 = 
	"ps_1_4 "
	"mul r0.yxw, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_108_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_1_4 : mul write mask .wzyx is NOT allowed";
string PS_108 = 
	"ps_1_4 "
	"mul r0.wzyx, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_109_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_1_4 : mul write mask .zxwy is NOT allowed";
string PS_109 = 
	"ps_1_4 "
	"mul r0.zxwy, v0, v1 "
	"mov r0, c0 ";


//////////////////////////////////////////////////////////////////////
// File:  DXGICustomTestApp.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a DXGI custom test app object.
//
// History:
// 10 Mar 2006  SEdmison  Factored from other tests into a shared lib.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "DXGICustomTestApp.hpp"


BEGIN_NAMED_VALUES( D3D10_DRIVER_TYPE )
    NAMED_VALUE( _T( "HW" ), D3D10_DRIVER_TYPE_HARDWARE )
    NAMED_VALUE( _T( "REF" ), D3D10_DRIVER_TYPE_REFERENCE )
    NAMED_VALUE( _T( "NULL" ), D3D10_DRIVER_TYPE_NULL )
    NAMED_VALUE( _T( "SW" ), D3D10_DRIVER_TYPE_SOFTWARE )
END_NAMED_VALUES( D3D10_DRIVER_TYPE )
#include "include.fx"

//-----------------------------------------------------------------------------
// Operate on R0, R1 register
//-----------------------------------------------------------------------------

Technique
<
	String	Name = "Inst: add (r1 = r0 + c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	1.0,	0.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
			
		        add	r1,	r0,	c1      // r1 = ( 1, 1, 1, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add: neg (r1 = r0 + c0)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0

			mov	r0,	c1

	                add     r1,	r0,	c0      // r1 = (0,1,1,0)
                        mov     r1.a,   c0.a            // r1 = (0,1,1,1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add sat (r1 = sat(r0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r1,	r0,	c1      // r1 = ( 1, 1, 1, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg (r1 = sat(r0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r1,	r0,	c1      // r1 = ( 1, 1, 1, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add -r1 (r1 = r0 + -r1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
                        mov	r1,	c1
			
		        add	r1,	r0,	-r1      // r1 = ( 0, 1, 0, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add neg -r1 (r1 = r0 + -r1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0

			mov	r0,	c0
                        mov     r1,     c1
			
		        add	r1,	r0,	-r1      // r1 = ( 0, 1, 1, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add sat -r1 (r1 = sat(r0 + -r1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     4.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
                        mov     r1,     c1
			
		        add_sat	r1,	r0,	-r1      // r1 = ( 1, 0, 0, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg -r1 (r1 = sat(r0 + -r1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0

			mov	r0,	c0
                        mov	r1,	c1
			
		        add_sat	r1,	r0,	-r1      // r1 = ( 1, 1, 0, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add (t) (r0 = t0 + c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.0,	1.0,	0.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };


		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	1.0,	0.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	1.0
                        
                        tex     t0

                        add	r0,	t0,	c1      // r0 = ( 1, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Inst: add: neg (t) (r0 = t0 + c0)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	-1.0,	0.0,	0.0,	-1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };

		PixelShader =
		asm
		{
			ps_1_1

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0
                        tex     t0


	                add     r0,	t0,	c0      // r0 = (0,1,1,0)
                        mov     r0.a,   c0.a            // r0 = (0,1,1,1)
		};
	}
}

Technique
<
	String	Name = "Inst: add sat (t) (r0 = sat(t0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;                

                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.0,	1.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0
                        tex     t0

		        add_sat	r0,	t0,	c1      // r0 = ( 1, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg (t) (r0 = sat(t0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.0,	-1.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0
                        tex     t0

		        add_sat	r0,	t0,	c1      // r0 = ( 1, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Inst: add -r1 (t) (r0 = t0 + -r1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	1.0,	1.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0
                        tex     t0                        

		        mov     r1,     c1
                        add	r0,	t0,	-r1      // r4 = ( 0, 1, 0, 1)
		};
	}
}

Technique
<
	String	Name = "Inst: add neg -r1 (t) (r0 = t0 + -r1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.5,	1.0,	0.5,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0
                        tex     t0                        

		        mov     r1,     c1
                        add	r0,	t0,	-r1      // r0 = ( 0, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Inst: add sat -r1 (t) (r0 = sat(t0 + -r1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	1.0,	0.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     1.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0
                        tex     t0                        

		        mov     r1,     c1
                        add_sat	r0,	t0,	-r1      // r0 = ( 1, 0, 0, 1)
		};
	}
}

Technique
<
	String	Name = "Inst: 