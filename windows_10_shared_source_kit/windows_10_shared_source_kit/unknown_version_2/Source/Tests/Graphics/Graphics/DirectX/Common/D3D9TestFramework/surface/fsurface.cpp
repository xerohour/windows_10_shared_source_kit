v oC0, c0 ";

PS_CRITERIA PS_024_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_2_0 : dp4 source reg combination t0, c0.g is allowed";
string PS_024 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.g "
	"mov oC0, c0 ";

PS_CRITERIA PS_025_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_2_0 : dp4 source reg combination t0, c0.b is allowed";
string PS_025 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.b "
	"mov oC0, c0 ";

PS_CRITERIA PS_026_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_2_0 : dp4 source reg combination t0, c0.a is allowed";
string PS_026 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.a "
	"mov oC0, c0 ";

PS_CRITERIA PS_027_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_2_0 : dp4 source reg combination t0, c0.gbra is allowed";
string PS_027 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.gbra "
	"mov oC0, c0 ";

PS_CRITERIA PS_028_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_2_0 : dp4 source reg combination t0, c0.brga is allowed";
string PS_028 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.brga "
	"mov oC0, c0 ";

PS_CRITERIA PS_029_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_2_0 : dp4 source reg combination t0, c0.abgr is allowed";
string PS_029 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.abgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_030_Desc = "ps_2_0 : dp4 source reg combination t0, t0 is NOT allowed";
string PS_030 = 
	"ps_2_0 "
	"dcl t0 "
	"dp4 r0, t0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_2_0 : dp4 source reg combination t0, t1 is NOT allowed";
string PS_031 = 
	"ps_2_0 "
	"dcl t0 "
	"dcl t1 "
	"dp4 r0, t0, t1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_2_0 : dp4 source reg combination t0, v0 is NOT allowed";
string PS_032 = 
	"ps_2_0 "
	"dcl t0 "
	"dcl v0 "
	"dp4 r0, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_2_0 : dp4 source reg combination v0, t0 is NOT allowed";
string PS_033 = 
	"ps_2_0 "
	"dcl v0 "
	"dcl t0 "
	"dp4 r0, v0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_2_0 : dp4 source reg combination v0, v1 is NOT allowed";
string PS_034 = 
	"ps_2_0 "
	"dcl v0 "
	"dcl v1 "
	"dp4 r0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_2_0 : dp4 source reg combination c0, c1 is NOT allowed";
string PS_035 = 
	"ps_2_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp4 r0, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_2_0 : dp4 source reg combination b0, b0 is NOT allowed";
string PS_036 = 
	"ps_2_0 "
	"defb b0, true "
	"dp4 r0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_2_0 : dp4 source reg combination b0, i0 is NOT allowed";
string PS_037 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dp4 r0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_2_0 : dp4 source reg combination b0, p0 is NOT allowed";
string PS_038 = 
	"ps_2_0 "
	"defb b0, true "
	"dp4 r0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_2_0 : dp4 source reg combination b0, s0 is NOT allowed";
string PS_039 = 
	"ps_2_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"dp4 r0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_2_0 : dp4 source reg combination b0, oC0 is NOT allowed";
string PS_040 = 
	"ps_2_0 "
	"defb b0, true "
	"dp4 r0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_2_0 : dp4 source reg combination b0, oDepth is NOT allowed";
string PS_041 = 
	"ps_2_0 "
	"defb b0, true "
	"dp4 r0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_2_0 : dp4 source reg combination i0, b0 is NOT allowed";
string PS_042 = 
	"ps_2_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_2_0 : dp4 source reg combination i0, i0 is NOT allowed";
string PS_043 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_2_0 : dp4 source reg combination i0, p0 is NOT allowed";
string PS_044 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_2_0 : dp4 source reg combination i0, s0 is NOT allowed";
string PS_045 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dp4 r0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_2_0 : dp4 source reg combination i0, oC0 is NOT allowed";
string PS_046 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_2_0 : dp4 source reg combination i0, oDepth is NOT allowed";
string PS_047 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_048_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_2_0 : dp4 source reg combination p0, b0 is NOT allowed";
string PS_048 = 
	"ps_2_0 "
	"defb b0, true "
	"dp4 r0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_049_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_2_0 : dp4 source reg combination p0, i0 is NOT allowed";
string PS_049 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_2_0 : dp4 source reg combination p0, p0 is NOT allowed";
string PS_050 = 
	"ps_2_0 "
	"dp4 r0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_051_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_2_0 : dp4 source reg combination p0, s0 is NOT allowed";
string PS_051 = 
	"ps_2_0 "
	"dcl_2d s0 "
	"dp4 r0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_052_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_2_0 : dp4 source reg combination p0, oC0 is NOT allowed";
string PS_052 = 
	"ps_2_0 "
	"dp4 r0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_053_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_2_0 : dp4 source reg combination p0, oDepth is NOT allowed";
string PS_053 = 
	"ps_2_0 "
	"dp4 r0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_054_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_2_0 : dp4 source reg combination s0, b0 is NOT allowed";
string PS_054 = 
	"ps_2_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"dp4 r0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_055_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_2_0 : dp4 source reg combination s0, i0 is NOT allowed";
string PS_055 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"dp4 r0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_056_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_2_0 : dp4 source reg combination s0, p0 is NOT allowed";
string PS_056 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"dp4 r0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_057_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_2_0 : dp4 source reg combination s0, s0 is NOT allowed";
string PS_057 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"dp4 r0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_058_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_2_0 : dp4 source reg combination s0, oC0 is NOT allowed";
string PS_058 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"dp4 r0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_059_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_2_0 : dp4 source reg combination s0, oDepth is NOT allowed";
string PS_059 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"dp4 r0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_060_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_2_0 : dp4 source reg combination oC0, b0 is NOT allowed";
string PS_060 = 
	"ps_2_0 "
	"defb b0, true "
	"dp4 r0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_061_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_2_0 : dp4 source reg combination oC0, i0 is NOT allowed";
string PS_061 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_062_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_2_0 : dp4 source reg combination oC0, p0 is NOT allowed";
string PS_062 = 
	"ps_2_0 "
	"dp4 r0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_063_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_2_0 : dp4 source reg combination oC0, s0 is NOT allowed";
string PS_063 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"dp4 r0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_064_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_2_0 : dp4 source reg combination oC0, oC0 is NOT allowed";
string PS_064 = 
	"ps_2_0 "
	"dp4 r0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_065_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_2_0 : dp4 source reg combination oC0, oDepth is NOT allowed";
string PS_065 = 
	"ps_2_0 "
	"dp4 r0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_066_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_2_0 : dp4 source reg combination oDepth, b0 is NOT allowed";
string PS_066 = 
	"ps_2_0 "
	"defb b0, true "
	"dp4 r0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_067_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_2_0 : dp4 source reg combination oDepth, i0 is NOT allowed";
string PS_067 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp4 r0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_068_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_2_0 : dp4 source reg combination oDepth, p0 is NOT allowed";
string PS_068 = 
	"ps_2_0 "
	"dp4 r0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_069_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_2_0 : dp4 source reg combination oDepth, s0 is NOT allowed";
string PS_069 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"dp4 r0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_070_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_2_0 : dp4 source reg combination oDepth, oC0 is NOT allowed";
string PS_070 = 
	"ps_2_0 "
	"dp4 r0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_071_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_2_0 : dp4 source reg combination oDepth, oDepth is NOT allowed";
string PS_071 = 
	"ps_2_0 "
	"dp4 r0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_072_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_2_0 : dp4 source reg combination t0.agrb, c0 is NOT allowed";
string PS_072 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0.agrb, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_073_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_2_0 : dp4 source reg combination t0.rbga, c0 is NOT allowed";
string PS_073 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0.rbga, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_074_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_2_0 : dp4 source reg combination t0.rgab, c0 is NOT allowed";
string PS_074 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0.rgab, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_075_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_2_0 : dp4 source reg combination t0.bgr, c0 is NOT allowed";
string PS_075 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0.bgr, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_076_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_2_0 : dp4 source reg combination t0.rbg, c0 is NOT allowed";
string PS_076 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0.rbg, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_077_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_2_0 : dp4 source reg combination t0.gar, c0 is NOT allowed";
string PS_077 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0.gar, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_078_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_2_0 : dp4 source reg combination t0.gr, c0 is NOT allowed";
string PS_078 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0.gr, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_079_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_2_0 : dp4 source reg combination t0.ab, c0 is NOT allowed";
string PS_079 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0.ab, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_080_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_2_0 : dp4 source reg combination t0.bg, c0 is NOT allowed";
string PS_080 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0.bg, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_081_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_2_0 : dp4 source reg combination t0, c0.agrb is NOT allowed";
string PS_081 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.agrb "
	"mov oC0, c0 ";

PS_CRITERIA PS_082_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_2_0 : dp4 source reg combination t0, c0.rbga is NOT allowed";
string PS_082 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.rbga "
	"mov oC0, c0 ";

PS_CRITERIA PS_083_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_2_0 : dp4 source reg combination t0, c0.rgab is NOT allowed";
string PS_083 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.rgab "
	"mov oC0, c0 ";

PS_CRITERIA PS_084_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_2_0 : dp4 source reg combination t0, c0.bgr is NOT allowed";
string PS_084 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.bgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_085_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_2_0 : dp4 source reg combination t0, c0.rbg is NOT allowed";
string PS_085 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.rbg "
	"mov oC0, c0 ";

PS_CRITERIA PS_086_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_2_0 : dp4 source reg combination t0, c0.gar is NOT allowed";
string PS_086 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.gar "
	"mov oC0, c0 ";

PS_CRITERIA PS_087_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_2_0 : dp4 source reg combination t0, c0.gr is NOT allowed";
string PS_087 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.gr "
	"mov oC0, c0 ";

PS_CRITERIA PS_088_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_2_0 : dp4 source reg combination t0, c0.ab is NOT allowed";
string PS_088 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.ab "
	"mov oC0, c0 ";

PS_CRITERIA PS_089_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_2_0 : dp4 source reg combination t0, c0.bg is NOT allowed";
string PS_089 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dp4 r0, t0, c0.bg "
	"mov oC0, c0 ";

PS_CRITERIA PS_090_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_2_0 : dp4 dest r# is allowed";
string PS_090 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_091_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_2_0 : dp4 _pp dest r# is allowed";
string PS_091 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_pp r0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_092_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_2_0 : dp4 _sat dest r# is allowed";
string PS_092 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_sat r0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_093_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_2_0 : dp4 dest v# is NOT allowed";
string PS_093 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 v0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_094_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_2_0 : dp4 _pp dest v# is NOT allowed";
string PS_094 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_pp v0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_095_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_2_0 : dp4 _sat dest v# is NOT allowed";
string PS_095 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_sat v0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_096_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_2_0 : dp4 dest c# is NOT allowed";
string PS_096 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 c1, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_097_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_2_0 : dp4 _pp dest c# is NOT allowed";
string PS_097 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_pp c1, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_098_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_2_0 : dp4 _sat dest c# is NOT allowed";
string PS_098 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_sat c1, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_099_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_2_0 : dp4 dest b# is NOT allowed";
string PS_099 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 b0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_100_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_2_0 : dp4 _pp dest b# is NOT allowed";
string PS_100 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_pp b0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_101_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_2_0 : dp4 _sat dest b# is NOT allowed";
string PS_101 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_sat b0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_102_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_2_0 : dp4 dest i# is NOT allowed";
string PS_102 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 i0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_103_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_2_0 : dp4 _pp dest i# is NOT allowed";
string PS_103 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_pp i0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_104_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_2_0 : dp4 _sat dest i# is NOT allowed";
string PS_104 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_sat i0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_105_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_105_Desc = "ps_2_0 : dp4 dest p0 is NOT allowed";
string PS_105 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 p0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_106_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_106_Desc = "ps_2_0 : dp4 _pp dest p0 is NOT allowed";
string PS_106 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_pp p0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_107_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_107_Desc = "ps_2_0 : dp4 _sat dest p0 is NOT allowed";
string PS_107 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_sat p0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_108_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_2_0 : dp4 dest s# is NOT allowed";
string PS_108 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 s0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_109_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_2_0 : dp4 _pp dest s# is NOT allowed";
string PS_109 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_pp s0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_110_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_2_0 : dp4 _sat dest s# is NOT allowed";
string PS_110 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_sat s0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_111_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_2_0 : dp4 dest t# is NOT allowed";
string PS_111 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 t0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_112_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_2_0 : dp4 _pp dest t# is NOT allowed";
string PS_112 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_pp t0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_113_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_2_0 : dp4 _sat dest t# is NOT allowed";
string PS_113 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_sat t0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_114_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_2_0 : dp4 dest oC# is NOT allowed";
string PS_114 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 oC0, t0, r0 ";

PS_CRITERIA PS_115_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_2_0 : dp4 _pp dest oC# is NOT allowed";
string PS_115 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_pp oC0, t0, r0 ";

PS_CRITERIA PS_116_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_116_Desc = "ps_2_0 : dp4 _sat dest oC# is NOT allowed";
string PS_116 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_sat oC0, t0, r0 ";

PS_CRITERIA PS_117_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_117_Desc = "ps_2_0 : dp4 dest oDepth is NOT allowed";
string PS_117 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 oDepth, t0.x, r0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_118_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_118_Desc = "ps_2_0 : dp4 _pp dest oDepth is NOT allowed";
string PS_118 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_pp oDepth, t0.x, r0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_119_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_119_Desc = "ps_2_0 : dp4 _sat dest oDepth is NOT allowed";
string PS_119 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4_sat oDepth, t0.x, r0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_120_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_120_Desc = "ps_2_0 : dp4 write mask .r is allowed";
string PS_120 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.r, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_121_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_121_Desc = "ps_2_0 : dp4 write mask .g is allowed";
string PS_121 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.g, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_122_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_122_Desc = "ps_2_0 : dp4 write mask .b is allowed";
string PS_122 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.b, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_123_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_123_Desc = "ps_2_0 : dp4 write mask .a is allowed";
string PS_123 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.a, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_124_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_124_Desc = "ps_2_0 : dp4 write mask .rg is allowed";
string PS_124 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.rg, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_125_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_125_Desc = "ps_2_0 : dp4 write mask .gb is allowed";
string PS_125 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.gb, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_126_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_126_Desc = "ps_2_0 : dp4 write mask .ba is allowed";
string PS_126 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.ba, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_127_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_127_Desc = "ps_2_0 : dp4 write mask .rb is allowed";
string PS_127 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.rb, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_128_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_128_Desc = "ps_2_0 : dp4 write mask .ra is allowed";
string PS_128 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.ra, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_129_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_129_Desc = "ps_2_0 : dp4 write mask .ga is allowed";
string PS_129 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.ga, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_130_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_130_Desc = "ps_2_0 : dp4 write mask .rgb is allowed";
string PS_130 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.rgb, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_131_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_131_Desc = "ps_2_0 : dp4 write mask .gba is allowed";
string PS_131 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.gba, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_132_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_132_Desc = "ps_2_0 : dp4 write mask .rba is allowed";
string PS_132 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.rba, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_133_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_133_Desc = "ps_2_0 : dp4 write mask .rgba is allowed";
string PS_133 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.rgba, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_134_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_134_Desc = "ps_2_0 : dp4 write mask .yx is NOT allowed";
string PS_134 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.yx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_135_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_135_Desc = "ps_2_0 : dp4 write mask .zx is NOT allowed";
string PS_135 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.zx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_136_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_136_Desc = "ps_2_0 : dp4 write mask .zy is NOT allowed";
string PS_136 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.zy, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_137_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_137_Desc = "ps_2_0 : dp4 write mask .wx is NOT allowed";
string PS_137 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.wx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_138_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_138_Desc = "ps_2_0 : dp4 write mask .wz is NOT allowed";
string PS_138 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.wz, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_139_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_139_Desc = "ps_2_0 : dp4 write mask .wy is NOT allowed";
string PS_139 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.wy, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_140_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_140_Desc = "ps_2_0 : dp4 write mask .zyx is NOT allowed";
string PS_140 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.zyx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_141_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_141_Desc = "ps_2_0 : dp4 write mask .wzy is NOT allowed";
string PS_141 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.wzy, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_142_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_142_Desc = "ps_2_0 : dp4 write mask .wzx is NOT allowed";
string PS_142 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.wzx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_143_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_143_Desc = "ps_2_0 : dp4 write mask .wyx is NOT allowed";
string PS_143 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.wyx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_144_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_144_Desc = "ps_2_0 : dp4 write mask .yxw is NOT allowed";
string PS_144 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.yxw, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_145_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_145_Desc = "ps_2_0 : dp4 write mask .wzyx is NOT allowed";
string PS_145 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.wzyx, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_146_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_146_Desc = "ps_2_0 : dp4 write mask .zxwy is NOT allowed";
string PS_146 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"dp4 r0.zxwy, t0, r0 "
	"mov oC0, c0 ";


Microsoft Visual Studio Solution File, Format Version 9.00
# Visual Studio 2005
Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "GetDC", "GetDC.vcproj", "{AF4552B0-DC56-447B-A0F7-B9057991AFA1}"
EndProject
Global
	GlobalSection(SolutionConfigurationPlatforms) = preSolution
		Debug|Win32 = Debug|Win32
		Release|Win32 = Release|Win32
	EndGlobalSection
	GlobalSection(ProjectConfigurationPlatforms) = postSolution
		{AF4552B0-DC56-447B-A0F7-B9057991AFA1}.Debug|Win32.ActiveCfg = Debug|Win32
		{AF4552B0-DC56-447B-A0F7-B9057991AFA1}.Release|Win32.ActiveCfg = Release|Win32
		{AF4552B0-DC56-447B-A0F7-B9057991AFA1}.Release|Win32.Build.0 = Release|Win32
	EndGlobalSection
	GlobalSection(SolutionProperties) = preSolution
		HideSolutionNode = FALSE
	EndGlobalSection
EndGlobal
#include "SurfacePCH.h"
#include "FSurface.h"

// -------------------------------------------------------------------------------------
// FColor class
//
// Defines a float based color class that has one 32-bit float per channel.
// Conversions to and from this format are provided most D3DFORMATs.

const FLOAT FColor::fDelta = .00001f;

// -------------------------------------------------------------------------------------

void FColor::UndoGammaCorrection()
{
    R = UndoGammaForChannel( R );
    G = UndoGammaForChannel( G );
    B = UndoGammaForChannel( B );
}

// -------------------------------------------------------------------------------------

void FColor::ConvertFrom(FMT Type, const char* pSurfaceBits, BOOL bGammaCorrect,
                            ELEMENTINDEX ElementIndex)
{
    UINT16 u16BITS;
    UINT8 u8BITS;

    switch (Type)
    {
    case FMT_A8R8G8B8: *this = *((UINT32*)pSurfaceBits); break;
    case FMT_X8R8G8B8: *this = *((UINT32*)pSurfaceBits); A = 1.F; break;

    case FMT_R5G6B5:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = ((u16BITS>>(6+5)) & 0x001F)/31.f;
        G = ((u16BITS>>   5) & 0x003F)/63.f;
        B = ((u16BITS      ) & 0x001F)/31.f;
        A = 1.F;
        break;

    case FMT_X1R5G5B5:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = ((u16BITS>>(5+5)) & 0x001F)/31.f;
        G = ((u16BITS>>   5) & 0x001F)/31.f;
        B = ((u16BITS      ) & 0x001F)/31.f;
        A = 1.F;
        break;

    case FMT_A1R5G5B5:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = ((u16BITS>>(5+5)) & 0x001F)/31.f;
        G = ((u16BITS>>   5) & 0x001F)/31.f;
        B = ((u16BITS      ) & 0x001F)/31.f;
        A = ( u16BITS & 0x8000 ) ? 1.f : 0.f;
        break;

    case FMT_A4R4G4B4:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = ((u16BITS>>  (4+4)) & 0x000F)/15.f;
        G = ((u16BITS>>    (4)) & 0x000F)/15.f;
        B = ((u16BITS         ) & 0x000F)/15.f;
        A = ((u16BITS>>(4+4+4)) & 0x000F)/15.f;
        break;

    case FMT_X4R4G4B4:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = ((u16BITS>>(4+4)) & 0x000F)/15.f;
        G = ((u16BITS>>  (4)) & 0x000F)/15.f;
        B = ((u16BITS       ) & 0x000F)/15.f;
        A = 1.f;
        break;

    case FMT_R8G8B8:
        R = *((UINT8*)pSurfaceBits+2)/255.f;
        G = *((UINT8*)pSurfaceBits+1)/255.f;
        B = *((UINT8*)pSurfaceBits+0)/255.f;
        A = 1.F;
        break;

    case FMT_L8:
        R = G = B = *((UINT8*)pSurfaceBits)/255.f;
        A = 1.F;
        break;

    case FMT_L16:
        R = G = B = *((UINT16*)pSurfaceBits)/65535.f;
        A = 1.F;
        break;

    case FMT_A8L8:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = G = B = (UINT8)(0xff & u16BITS)/255.f;
        A = (UINT8)(0xff & (u16BITS >> 8))/255.f;
        break;

    case FMT_A8:
        R = G = B = 0.f;
        A = *((UINT8*)pSurfaceBits)/255.f;
        break;

    case FMT_R3G3B2:
        u8BITS = *((UINT8*)pSurfaceBits);
        R = ((u8BITS>>(3+2)) & 0x07)/7.f;
        G = ((u8BITS>>   2) & 0x07)/7.f;
        B = ((u8BITS      ) & 0x03)/3.f;
        A = 1.F;
        break;

    case FMT_A4L4:
        u16BITS = *((UINT8*)pSurfaceBits);
        R = G = B = (u16BITS & 0x0f)/15.f;
        A =    ((u16BITS>>4) & 0x0f)/15.f;
        break;

    case FMT_A8R3G3B2:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = ((u16BITS>>(3+2)) & 0x07)/7.f;
        G = ((u16BITS>>   2) & 0x07)/7.f;
        B = ((u16BITS      ) & 0x03)/3.f;
        A = (UINT8)(0xff & (u16BITS >> 8))/255.f;
        break;

    case FMT_V8U8:
        {
            INT8 iDU = *(( INT8*)pSurfaceBits+0);
            INT8 iDV = *(( INT8*)pSurfaceBits+1);
            R = (FLOAT)(iDU+127) / 255.f;     // fDU
            G = (FLOAT)(iDV+127) / 255.f;     // fDV

            B = 1.0F;                   // fL
            A = 1.F;
        }
        break;

    case FMT_CxV8U8:
    {
        INT8 iDU = *(( INT8*)pSurfaceBits+0);
        INT8 iDV = *(( INT8*)pSurfaceBits+1);
        R = CLAMP_SIGNED8(iDU);     // fDU
        G = CLAMP_SIGNED8(iDV);     // fDV
        float fUV = 1.0f - R * R - G * G;
        B = (fUV <= 0.0f) ? 0.0f : sqrtf(fUV);

        R = (FLOAT)(iDU+127) / 255.f; G = (FLOAT)(iDV+127) / 255.f;
        B = B * 0.5f + 0.5f;
        A = 1.F;
    }
    break;

    case FMT_V16U16:
        {
            INT16 iDU = *(( INT16*)pSurfaceBits+0);
            INT16 iDV = *(( INT16*)pSurfaceBits+1);
            R = (FLOAT)(iDU+32768) / 65535.f;     // fDU
            G = (FLOAT)(iDV+32768) / 65535.f;     // fDV
            B = 1.0f;   // 1.0 here is intentional
            A = 1.0f;
        }
        break;

    case FMT_L6V5U5:
        {
            u16BITS = *((UINT16*)pSurfaceBits);
            INT8 iU = (INT8)(u16BITS & 0x1f);
            INT8 iV = (INT8)((u16BITS>>5) & 0x1f);
            UINT8 uL = (UINT8)(u16BITS >> 10);
                        iU <<= 3; iU >>= 3;
                        iV <<= 3; iV >>= 3; 
                        iU += (-16 == iU);
                        iV += (-16 == iV);
                        R = (FLOAT)(iU + 15) / 30.0f;
                        G = (FLOAT)(iV + 15) / 30.0f;
            // the unsigned uL is normalized with 2^N - 1, since this is the
            // largest representable value
            B = 1.0f;
            A = (FLOAT)uL * (1.0F/63.0F);       // fL
        }
        break;

    case FMT_X8L8V8U8:
        {
            INT8 iDU = *(( INT8*)pSurfaceBits+0);
            INT8 iDV = *(( INT8*)pSurfaceBits+1);
            UINT8 uL  = *((UINT8*)pSurfaceBits+2);
            R = (FLOAT)(iDU+127) / 255.f;     // fDU
            G = (FLOAT)(iDV+127) / 255.f;     // fDV
            // the unsigned uL is normalized with 2^N - 1, since this is the
            // largest representable value
            A = (FLOAT)uL * (1.0F/255.0F);      // fL
            B = 1.0f;
        }
        break;
    case FMT_Q8W8V8U8:
        {
            INT8 iDU = *(( INT8*)pSurfaceBits+0);
            INT8 iDV = *(( INT8*)pSurfaceBits+1);
            INT8 iDW = *(( INT8*)pSurfaceBits+2);
            INT8 iDQ = *(( INT8*)pSurfaceBits+3);
            // signed values are normalized with 2^(N-1), since -2^(N-1) can
            // be exactly expressed in N bits
            R = (FLOAT)(iDU+127) / 255.f;     // fDU
            G = (FLOAT)(iDV+127) / 255.f;     // fDV
            B = (FLOAT)(iDW+127) / 255.f;     // fDU
            A = (FLOAT)(iDQ+127) / 255.f;     // fDV
        }
        break;
    case FMT_W11V11U10:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            INT16 iDU = (INT16)((u32BITS>>(0    )) & 0x3FF);
            INT16 iDV = (INT16)((u32BITS>>(10   )) & 0x7FF);
            INT16 iDW = (INT16)((u32BITS>>(10+11)) & 0x7FF);

            // signed values are normalized with 2^(N-1), since -2^(N-1) can
            // be exactly expressed in N bits
            R = (FLOAT)(iDU+512) / 1023.f;    // fDU
            G = (FLOAT)(iDV+1024) / 2047.f;    // fDV
            B = (FLOAT)(iDW+1024) / 2047.f;    // fDW
            A = 1.0f;
        }
        break;
    case FMT_A2B10G10R10:
	case FMT_A2B10G10R10_XR_BIAS:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            R = ((u32BITS>>(0    ))  & 0x3FF)/1023.f;
            G = ((u32BITS>>(10   ))  & 0x3FF)/1023.f;
            B = ((u32BITS>>(10+10))  & 0x3FF)/1023.f;
            A = ((u32BITS>>(10+10+10)) & 0x3)/3.f;
        }
    break;
    case FMT_A2R10G10B10:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            B = ((u32BITS>>(0    ))  & 0x3FF)/1023.f;
            G = ((u32BITS>>(10   ))  & 0x3FF)/1023.f;
            R = ((u32BITS>>(10+10))  & 0x3FF)/1023.f;
            A = ((u32BITS>>(10+10+10)) & 0x3)/3.f;
        }
    break;
    case FMT_A8B8G8R8:
        {
            R = *(( UINT8*)pSurfaceBits+0)/255.f;
            G = *(( UINT8*)pSurfaceBits+1)/255.f;
            B = *(( UINT8*)pSurfaceBits+2)/255.f;
            A = *(( UINT8*)pSurfaceBits+3)/255.f;
        }
    break;
    case FMT_X8B8G8R8:
        {
            R = *(( UINT8*)pSurfaceBits+0)/255.f;
            G = *(( UINT8*)pSurfaceBits+1)/255.f;
            B = *(( UINT8*)pSurfaceBits+2)/255.f;
            A = 1.f;
        }
    break;
    case FMT_G16R16:
        {
            R = *(( UINT16*)pSurfaceBits+0)/65535.f;
            G = *(( UINT16*)pSurfaceBits+1)/65535.f;
            B = 1.0f;   // 1.0 here is intentional
            A = 1.0f;
        }
    break;
    case FMT_W10V11U11:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            INT16 iDU = (INT16)((u32BITS>>(0    )) & 0x7FF);
            INT16 iDV = (INT16)((u32BITS>>(11   )) & 0x7FF);
            INT16 iDW = (INT16)((u32BITS>>(11+11)) & 0x3FF);

            // signed values are normalized with 2^(N-1), since -2^(N-1) can
            // be exactly expressed in N bits
            R = (FLOAT)(iDU+1024) / 2047.f;    // fDU
            G = (FLOAT)(iDV+1024) / 2047.f;    // fDV
            B = (FLOAT)(iDW+512) / 1023.f;    // fDW
            A = 1.0f;
        }
    break;
    case FMT_A2W10V10U10:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            INT16 iDU = (INT16)((u32BITS>>(0    )) & 0x3FF);
            INT16 iDV = (INT16)((u32BITS>>(10   )) & 0x3FF);
            INT16 iDW = (INT16)((u32BITS>>(10+10)) & 0x3FF);

            // Sign extension
            iDU <<= 6; iDU >>= 6;
            iDV <<= 6; iDV >>= 6;
            iDW <<= 6; iDW >>= 6;

            iDU += (-512 == iDU);
            iDV += (-512 == iDV);
            iDW += (-512 == iDW);

            // signed values are normalized with 2^(N-1), since -2^(N-1) can
            // be exactly expressed in N bits
            R = (FLOAT)(iDU+512) / 1023.f;    // fDU
            G = (FLOAT)(iDV+512) / 1023.f;    // fDV
            B = (FLOAT)(iDW+512) / 1023.f;    // fDW

            // Note: The A component is treated as an unsigned component
            A = ((u32BITS>>(10+10+10)) & 0x3)/3.f; 
        }
	    break;
#if 0  //Removed from DX9
    case FMT_A8X8V8U8:
        {
            INT8 iU = *(( INT8*)pSurfaceBits+0);
            INT8 iV = *(( INT8*)pSurfaceBits+1);
            
            // signed values are normalized with 2^(N-1), since -2^(N-1) can
            // be exactly expressed in N bits
            R = (FLOAT)(iU+127) / 255.f;     // fDU
            G = (FLOAT)(iV+127) / 255.f;     // fDV
            B = 1.0f;

            // Note: The A component is treated as unsigned 
            A = *(( UINT8*)pSurfaceBits+3)/255.f;
        }
		break;
    case FMT_L8X8V8U8:
        {
            INT8 iU = *(( INT8*)pSurfaceBits+0);
            INT8 iV = *(( INT8*)pSurfaceBits+1);
            UINT8 iL = *(( UINT8*)pSurfaceBits+3);
            
            // signed values are normalized with 2^(N-1), since -2^(N-1) can
            // be exactly expressed in N bits
            R = (FLOAT)(iU+127) / 255.f;     // fDU
            G = (FLOAT)(iV+127) / 255.f;     // fDV
                        B = 1.0f;
            A = (FLOAT)iL / 255.f;
            
        }
		break;
#endif
	// shadow map texture formats (read only, not needed for ConvertTo)
    case FMT_D16:
        {
            u16BITS = *((UINT16*)pSurfaceBits);
            R = 0.0F;
            G = (FLOAT)u16BITS * (1.0F/(FLOAT)0xffff);
            B = 0.0F;
            A = 1.0f;
        }
        break;

    case FMT_D24S8:
    case FMT_D24X8:
    case FMT_D24X4S4:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            R = 0.0F;
            G = (FLOAT)(u32BITS>>8) * (1.0F/(FLOAT)0xffffff);
            B = 0.0F;
            A = 1.0f;
        }
        break;

    case FMT_S8D24:
    case FMT_X8D24:
    case FMT_X4S4D24:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            R = 0.0F;
            G = (FLOAT)(u32BITS&0x00ffffff) * (1.0F/(FLOAT)0xffffff);
            B = 0.0F;
            A = 1.0f;
        }
        break;

    case FMT_D15S1:
        {
            u16BITS = *((UINT16*)pSurfaceBits);
            R = 0.0F;
            G = (FLOAT)(u16BITS>>1) * (1.0F/(FLOAT)0x7fff);
            B = 0.0F;
            A = 1.0f;
        }
        break;

    case FMT_S1D15:
        {
            u16BITS = *((UINT16*)pSurfaceBits);
            R = 0.0F;
            G = (FLOAT)(u16BITS&0x7fff) * (1.0F/(FLOAT)0x7fff);
            B = 0.0F;
            A = 1.0f;
        }
        break;

    case FMT_D32:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            R = 0.0F;
            G = (FLOAT)u32BITS * (1.0F/(FLOAT)0xffffffff);
            B = 0.0F;
            A = 1.0f;
        }
        break;

    case FMT_MULTI2_ARGB8:       // TODO: Is this correct?
        {
            if( ELEMENTINDEX_DEFAULT == ElementIndex )
                ElementIndex = ELEMENTINDEX_0; // default to reading from field 0

            UINT8* pOffsetSurfaceBits = (UINT8*)pSurfaceBits + (UINT)ElementIndex * 4;
            R = *(pOffsetSurfaceBits+0)/255.f;
            G = *(pOffsetSurfaceBits+1)/255.f;
            B = *(pOffsetSurfaceBits+2)/255.f;
            A = *(pOffsetSurfaceBits+3)/255.f;
        }
        break;

    case FMT_A16B16G16R16:
        {
            R = *((UINT16*)pSurfaceBits+0)/65535.f;
            G = *((UINT16*)pSurfaceBits+1)/65535.f;
            B = *((UINT16*)pSurfaceBits+2)/65535.f;
            A = *((UINT16*)pSurfaceBits+3)/65535.f;
        }
        break;

    case FMT_Q16W16V16U16:
        {
            INT16 iDU = *(( INT16*)pSurfaceBits+0);
            INT16 iDV = *(( INT16*)pSurfaceBits+1);
            INT16 iDW = *(( INT16*)pSurfaceBits+2);
            INT16 iDQ = *(( INT16*)pSurfaceBits+3);
            R = (FLOAT)(iDU + 32768) / 65536.f;     // fDU
            G = (FLOAT)(iDV + 32768) / 65536.f;     // fDV
            B = (FLOAT)(iDW + 32768) / 65536.f;     // fDW
            A = (FLOAT)(iDQ + 32768) / 65536.f;     // fDQ
        }
        break;

    case FMT_R16F:
        {
            R = (( CFloat16*)pSurfaceBits)[0];
            G = 1.0f;
            B = 1.0f;
            A = 1.0f;
        }
        break;

    case FMT_G16R16F:
        {
            R = (( CFloat16*)pSurfaceBits)[0];
            G = (( CFloat16*)pSurfaceBits)[1];
            B = 1.0f;
            A = 1.0f;
        }
        break;

    case FMT_A16B16G16R16F:
        {
            R = (( CFloat16*)pSurfaceBits)[0];
            G = (( CFloat16*)pSurfaceBits)[1];
            B = (( CFloat16*)pSurfaceBits)[2];
            A = (( CFloat16*)pSurfaceBits)[3];
        }
        break;

    case FMT_R32F:
        {
            R = *(float*)pSurfaceBits;
            G = 1.0f;
            B = 1.0f;
            A = 1.0f;
        }
        break;

    case FMT_G32R32F:
        {
            R = (( float*)pSurfaceBits)[0];
            G = (( float*)pSurfaceBits)[1];
            B = 1.0f;
            A = 1.0f;
        }
        break;

    case FMT_A32B32G32R32F:
        {
            R = (( float*)pSurfaceBits)[0];
            G = (( float*)pSurfaceBits)[1];
            B = (( float*)pSurfaceBits)[2];
            A = (( float*)pSurfaceBits)[3];
        }
        break;

	case FMT_A1:
        R = G = B = A = *((bool *)pSurfaceBits);
        break;

        default:
        {
            DPF(2, _T("FColor::ConvertFrom() - Unsupported format type. FMT=%d.\n"), (UINT)Type);
        }
        break;
    }
        
    if( bGammaCorrect )
    {
        UndoGammaCorrection();
    }
}

// -------------------------------------------------------------------------------------

void FColor::ConvertTo( FMT Type, float fRoundOffset, char* pSurfaceBits, 
                        BOOL bGammaCorrect, ELEMENTINDEX ElementIndex) const
{
    int iR, iG, iB, iA;
    FLOAT Rg, Gg, Bg; // Gamma Corrected values
        
    if( bGammaCorrect )
    {
        Rg = GammaCorrectChannel( R );
        Gg = GammaCorrectChannel( G );
        Bg = GammaCorrectChannel( B );
    }
    else
    {
        Rg = R;
        Gg = G;
        Bg = B;
    }

    switch (Type)
    {
    case FMT_A8R8G8B8:
        *((UINT8*)pSurfaceBits+0) = (UINT8)((FLOAT)Bg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+1) = (UINT8)((FLOAT)Gg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+2) = (UINT8)((FLOAT)Rg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+3) = (UINT8)((FLOAT)A * 255. + fRoundOffset);
        break;

    case FMT_X8R8G8B8:
        *((UINT8*)pSurfaceBits+0) = (UINT8)((FLOAT)Bg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+1) = (UINT8)((FLOAT)Gg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+2) = (UINT8)((FLOAT)Rg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+3) = 0x00;
        break;

    case FMT_R8G8B8:
        *((UINT8*)pSurfaceBits+0) = (UINT8)((FLOAT)Bg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+1) = (UINT8)((FLOAT)Gg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+2) = (UINT8)((FLOAT)Rg * 255. 