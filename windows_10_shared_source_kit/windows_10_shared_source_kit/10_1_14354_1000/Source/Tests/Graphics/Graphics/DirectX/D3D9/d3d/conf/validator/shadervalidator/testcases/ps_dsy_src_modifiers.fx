#include "Test_Include.fx"

int Test_Count = 108;

string TestInfo
<
    string TestType = "PS";
>
= "ps_dsy_src_modifiers";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_001_Desc = "ps_2_x : dsy - no src modifiers";
string PS_001 =
    "ps_2_x "
    "dsy r0, r1 "
    "mov oC0, r0 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_002_Desc = "ps_2_x : dsy -src is allowed";
string PS_002 =
    "ps_2_x "
    "dsy r0, -c0 "
    "mov oC0, r0 ";

PS_CRITERIA PS_003_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_003_Desc = "ps_2_x : dsy -src.x is allowed";
string PS_003 =
    "ps_2_x "
    "dsy r0, -c0.x "
    "mov oC0, r0 ";
    
PS_CRITERIA PS_004_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_004_Desc = "ps_2_x : dsy -src.y is allowed";
string PS_004 =
    "ps_2_x "
    "dsy r0, -c0.y "
    "mov oC0, r0 ";

PS_CRITERIA PS_005_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_005_Desc = "ps_2_x : dsy -src.z is allowed";
string PS_005 =
    "ps_2_x "
    "dsy r0, -c0.z "
    "mov oC0, r0 ";

PS_CRITERIA PS_006_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_006_Desc = "ps_2_x : dsy -src.w is allowed";
string PS_006 =
    "ps_2_x "
    "dsy r0, -c0.w "
    "mov oC0, r0 ";

PS_CRITERIA PS_007_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_007_Desc = "ps_2_x : dsy -src.xyzw is allowed";
string PS_007 =
    "ps_2_x "
    "dsy r0, -c0.xyzw "
    "mov oC0, r0 ";

PS_CRITERIA PS_008_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_008_Desc = "ps_2_x : dsy -src.yzxw is allowed";
string PS_008 =
    "ps_2_x "
    "dsy r0, -c0.yzxw "
    "mov oC0, r0 ";

PS_CRITERIA PS_009_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_009_Desc = "ps_2_x : dsy -src.zxyw is allowed";
string PS_009 =
    "ps_2_x "
    "dsy r0, -c0.zxyw "
    "mov oC0, r0 ";

PS_CRITERIA PS_010_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };
string PS_010_Desc = "ps_2_x : dsy -src.wzyx is allowed";
string PS_010 =
    "ps_2_x "
    "dsy r0, -c0.wzyx "
    "mov oC0, r0 ";
   
PS_CRITERIA PS_011_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS + CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_011_Desc = "ps_2_x : dsy -src.zx is allowed";
string PS_011 =
    "ps_2_x "
    "dsy r0, -c0.zx "
    "mov oC0, r0 ";

PS_CRITERIA PS_012_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS + CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_012_Desc = "ps_2_x : dsy -src.yw is allowed";
string PS_012 =
    "ps_2_x "
    "dsy r0, -c0.yw "
    "mov oC0, r0 ";

PS_CRITERIA PS_013_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS + CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_013_Desc = "ps_2_x : dsy -src.zyw is allowed";
string PS_013 =
    "ps_2_x "
    "dsy r0, -c0.zyw "
    "mov oC0, r0 ";

PS_CRITERIA PS_014_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS + CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_014_Desc = "ps_2_x : dsy -src.yxwz is allowed";
string PS_014 =
    "ps_2_x "
    "dsy r0, -c0.yxwz "
    "mov oC0, r0 ";

//ps_2_sw

PS_CRITERIA PS_015_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_2_sw : dsy - no src modifiers";
string PS_015 =
    "ps_2_sw "
    "dsy r0, r1 "
    "mov oC0, r0 ";

PS_CRITERIA PS_016_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_2_sw : dsy -src is allowed";
string PS_016 =
    "ps_2_sw "
    "dsy r0, -c0 "
    "mov oC0, r0 ";

PS_CRITERIA PS_017_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_2_sw : dsy -src.x is allowed";
string PS_017 =
    "ps_2_sw "
    "dsy r0, -c0.x "
    "mov oC0, r0 ";
    
PS_CRITERIA PS_018_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_2_sw : dsy -src.y is allowed";
string PS_018 =
    "ps_2_sw "
    "dsy r0, -c0.y "
    "mov oC0, r0 ";

PS_CRITERIA PS_019_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_2_sw : dsy -src.z is allowed";
string PS_019 =
    "ps_2_sw "
    "dsy r0, -c0.z "
    "mov oC0, r0 ";

PS_CRITERIA PS_020_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_2_sw : dsy -src.w is allowed";
string PS_020 =
    "ps_2_sw "
    "dsy r0, -c0.w "
    "mov oC0, r0 ";

PS_CRITERIA PS_021_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_2_sw : dsy -src.xyzw is allowed";
string PS_021 =
    "ps_2_sw "
    "dsy r0, -c0.xyzw "
    "mov oC0, r0 ";

PS_CRITERIA PS_022_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_2_sw : dsy -src.yzxw is allowed";
string PS_022 =
    "ps_2_sw "
    "dsy r0, -c0.yzxw "
    "mov oC0, r0 ";

PS_CRITERIA PS_023_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_2_sw : dsy -src.zxyw is allowed";
string PS_023 =
    "ps_2_sw "
    "dsy r0, -c0.zxyw "
    "mov oC0, r0 ";

PS_CRITERIA PS_024_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_2_sw : dsy -src.wzyx is allowed";
string PS_024 =
    "ps_2_sw "
    "dsy r0, -c0.wzyx "
    "mov oC0, r0 ";
   
PS_CRITERIA PS_025_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_2_sw : dsy -src.xy is allowed";
string PS_025 =
    "ps_2_sw "
    "dsy r0, -c0.xy "
    "mov oC0, r0 ";

PS_CRITERIA PS_026_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_2_sw : dsy -src.wz is allowed";
string PS_026 =
    "ps_2_sw "
    "dsy r0, -c0.wz "
    "mov oC0, r0 ";

PS_CRITERIA PS_027_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_2_sw : dsy -src.xyz is allowed";
string PS_027 =
    "ps_2_sw "
    "dsy r0, -c0.xyz "
    "mov oC0, r0 ";

PS_CRITERIA PS_028_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_2_sw : dsy -src.wxyz is allowed";
string PS_028 =
    "ps_2_sw "
    "dsy r0, -c0.wxyz "
    "mov oC0, r0 ";
    
//ps_3_0

PS_CRITERIA PS_029_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_3_0 : dsy - no src modifiers";
string PS_029 =
    "ps_3_0 "
    "dsy r0, c0 "
    "mov oC0, r0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_030_Desc = "ps_3_0 : dsy -src is NOT allowed";
string PS_030 =
    "ps_3_0 "
    "dsy r0, -c0 "
    "mov oC0, r0 ";

PS_CRITERIA PS_031_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_031_Desc = "ps_3_0 : dsy -src.x is NOT allowed";
string PS_031 =
    "ps_3_0 "
    "dsy r0, -c0.x "
    "mov oC0, r0 ";
    
PS_CRITERIA PS_032_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_032_Desc = "ps_3_0 : dsy -src.y is NOT allowed";
string PS_032 =
    "ps_3_0 "
    "dsy r0, -c0.y "
    "mov oC0, r0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_033_Desc = "ps_3_0 : dsy -src.z is NOT allowed";
string PS_033 =
    "ps_3_0 "
    "dsy r0, -c0.z "
    "mov oC0, r0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_034_Desc = "ps_3_0 : dsy -src.w is NOT allowed";
string PS_034 =
    "ps_3_0 "
    "dsy r0, -c0.w "
    "mov oC0, r0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_035_Desc = "ps_3_0 : dsy -src.xyzw is NOT allowed";
string PS_035 =
    "ps_3_0 "
    "dsy r0, -c0.xyzw "
    "mov oC0, r0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_036_Desc = "ps_3_0 : dsy -src.yzxw is NOT allowed";
string PS_036 =
    "ps_3_0 "
    "dsy r0, -c0.yzxw "
    "mov oC0, r0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_037_Desc = "ps_3_0 : dsy -src.zxyw is NOT allowed";
string PS_037 =
    "ps_3_0 "
    "dsy r0, -c0.zxyw "
    "mov oC0, r0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_038_Desc = "ps_3_0 : dsy -src.wzyx is NOT allowed";
string PS_038 =
    "ps_3_0 "
    "dsy r0, -c0.wzyx "
    "mov oC0, r0 ";
   
PS_CRITERIA PS_039_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_039_Desc = "ps_3_0 : dsy -src.xw is NOT allowed";
string PS_039 =
    "ps_3_0 "
    "dsy r0, -c0.xw "
    "mov oC0, r0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_040_Desc = "ps_3_0 : dsy -src.zy is NOT allowed";
string PS_040 =
    "ps_3_0 "
    "dsy r0, -c0.zy "
    "mov oC0, r0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_041_Desc = "ps_3_0 : dsy -src.zwx is NOT allowed";
string PS_041 =
    "ps_3_0 "
    "dsy r0, -c0.zwx "
    "mov oC0, r0 ";

PS_CRITERIA PS_042_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_042_Desc = "ps_3_0 : dsy -src.xwzy is NOT allowed";
string PS_042 =
    "ps_3_0 "
    "dsy r0, -c0.xwzy "
    "mov oC0, r0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_043_Desc = "ps_3_0 : dsy src_abs is not allowed";
string PS_043 =
    "ps_3_0 "
    "dsy r0, c0_abs "
    "mov oC0, r0 ";
    
PS_CRITERIA PS_044_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_044_Desc = "ps_3_0 : dsy src_abs.x is NOT allowed";
string PS_044 =
    "ps_3_0 "
    "dsy r0, c0_abs.x "
    "mov oC0, r0 ";
    
PS_CRITERIA PS_045_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_045_Desc = "ps_3_0 : dsy src_abs.y is NOT allowed";
string PS_045 =
    "ps_3_0 "
    "dsy r0, c0_abs.y "
    "mov oC0, r0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_046_Desc = "ps_3_0 : dsy src_abs.z is NOT allowed";
string PS_046 =
    "ps_3_0 "
    "dsy r0, c0_abs.z "
    "mov oC0, r0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_047_Desc = "ps_3_0 : dsy src_abs.w is NOT allowed";
string PS_047 =
    "ps_3_0 "
    "dsy r0, c0_abs.w "
    "mov oC0, r0 ";

PS_CRITERIA PS_048_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_048_Desc = "ps_3_0 : dsy src_abs.xyzw is NOT allowed";
string PS_048 =
    "ps_3_0 "
    "dsy r0, c0_abs.xyzw "
    "mov oC0, r0 ";

PS_CRITERIA PS_049_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_049_Desc = "ps_3_0 : dsy src_abs.yzxw is NOT allowed";
string PS_049 =
    "ps_3_0 "
    "dsy r0, c0_abs.yzxw "
    "mov oC0, r0 ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_050_Desc = "ps_3_0 : dsy src_abs.zxyw is NOT allowed";
string PS_050 =
    "ps_3_0 "
    "dsy r0, c0_abs.zxyw "
    "mov oC0, r0 ";

PS_CRITERIA PS_051_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_051_Desc = "ps_3_0 : dsy src_abs.wzyx is NOT allowed";
string PS_051 =
    "ps_3_0 "
    "dsy r0, c0_abs.wzyx "
    "mov oC0, r0 ";
   
PS_CRITERIA PS_052_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_052_Desc = "ps_3_0 : dsy src_abs.xz is NOT allowed";
string PS_052 =
    "ps_3_0 "
    "dsy r0, c0_abs.xz "
    "mov oC0, r0 ";

PS_CRITERIA PS_053_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_053_Desc = "ps_3_0 : dsy src_abs.wy is NOT allowed";
string PS_053 =
    "ps_3_0 "
    "dsy r0, c0_abs.wy "
    "mov oC0, r0 ";

PS_CRITERIA PS_054_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_054_Desc = "ps_3_0 : dsy src_abs.wzy is NOT allowed";
string PS_054 =
    "ps_3_0 "
    "dsy r0, c0_abs.wzy "
    "mov oC0, r0 ";

PS_CRITERIA PS_055_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_055_Desc = "ps_3_0 : dsy src_abs.zyxw is NOT allowed";
string PS_055 =
    "ps_3_0 "
    "dsy r0, c0_abs.zyxw "
    "mov oC0, r0 ";

PS_CRITERIA PS_056_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_056_Desc = "ps_3_0 : dsy -src_abs is not allowed";
string PS_056 =
    "ps_3_0 "
    "dsy r0, -c0_abs "
    "mov oC0, r0 ";
    
PS_CRITERIA PS_057_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_057_Desc = "ps_3_0 : dsy -src_abs.x is NOT allowed";
string PS_057 =
    "ps_3_0 "
    "dsy r0, -c0_abs.x "
    "mov oC0, r0 ";
    
PS_CRITERIA PS_058_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_058_Desc = "ps_3_0 : dsy -src_abs.y is NOT allowed";
string PS_058 =
    "ps_3_0 "
    "dsy r0, -c0_abs.y "
    "mov oC0, r0 ";

PS_CRITERIA PS_059_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_059_Desc = "ps_3_0 : dsy -src_abs.z is NOT allowed";
string PS_059 =
    "ps_3_0 "
    "dsy r0, -c0_abs.z "
    "mov oC0, r0 ";

PS_CRITERIA PS_060_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_060_Desc = "ps_3_0 : dsy -src_abs.w is NOT allowed";
string PS_060 =
    "ps_3_0 "
    "dsy r0, -c0_abs.w "
    "mov oC0, r0 ";

PS_CRITERIA PS_061_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_061_Desc = "ps_3_0 : dsy -src_abs.xyzw is NOT allowed";
string PS_061 =
    "ps_3_0 "
    "dsy r0, -c0_abs.xyzw "
    "mov oC0, r0 ";

PS_CRITERIA PS_062_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_062_Desc = "ps_3_0 : dsy -src_abs.yzxw is NOT allowed";
string PS_062 =
    "ps_3_0 "
    "dsy r0, -c0_abs.yzxw "
    "mov oC0, r0 ";

PS_CRITERIA PS_063_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_063_Desc = "ps_3_0 : dsy -src_abs.zxyw is NOT allowed";
string PS_063 =
    "ps_3_0 "
    "dsy r0, -c0_abs.zxyw "
    "mov oC0, r0 ";

PS_CRITERIA PS_064_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_064_Desc = "ps_3_0 : dsy -src_abs.wzyx is NOT allowed";
string PS_064 =
    "ps_3_0 "
    "dsy r0, -c0_abs.wzyx "
    "mov oC0, r0 ";
   
PS_CRITERIA PS_065_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_065_Desc = "ps_3_0 : dsy -src_abs.yx is NOT allowed";
string PS_065 =
    "ps_3_0 "
    "dsy r0, -c0_abs.yx "
    "mov oC0, r0 ";

PS_CRITERIA PS_066_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_066_Desc = "ps_3_0 : dsy -src_abs.zw is NOT allowed";
string PS_066 =
    "ps_3_0 "
    "dsy r0, -c0_abs.zw "
    "mov oC0, r0 ";

PS_CRITERIA PS_067_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_067_Desc = "ps_3_0 : dsy -src_abs.yxz is NOT allowed";
string PS_067 =
    "ps_3_0 "
    "dsy r0, -c0_abs.yxz "
    "mov oC0, r0 ";

PS_CRITERIA PS_068_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_068_Desc = "ps_3_0 : dsy -src_abs.zywx is NOT allowed";
string PS_068 =
    "ps_3_0 "
    "dsy r0, -c0_abs.zywx "
    "mov oC0, r0 ";
    
//ps_3_sw

PS_CRITERIA PS_069_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_069_Desc = "ps_3_sw : dsy - no src modifiers";
string PS_069 =
    "ps_3_sw "
    "dsy r0, c0 "
    "mov oC0, r0 ";

PS_CRITERIA PS_070_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_070_Desc = "ps_3_sw : dsy -src is not allowed";
string PS_070 =
    "ps_3_sw "
    "dsy r0, -c0 "
    "mov oC0, r0 ";

PS_CRITERIA PS_071_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_071_Desc = "ps_3_sw : dsy -src.x is NOT allowed";
string PS_071 =
    "ps_3_sw "
    "dsy r0, -c0.x "
    "mov oC0, r0 ";
    
PS_CRITERIA PS_072_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_072_Desc = "ps_3_sw : dsy -src.y is NOT allowed";
string PS_072 =
    "ps_3_sw "
    "dsy r0, -c0.y "
    "mov oC0, r0 ";

PS_CRITERIA PS_073_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_073_Desc = "ps_3_sw : dsy -src.z is NOT allowed";
string PS_073 =
    "ps_3_sw "
    "dsy r0, -c0.z "
    "mov oC0, r0 ";

PS_CRITERIA PS_074_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_074_Desc = "ps_3_sw : dsy -src.w is NOT allowed";
string PS_074 =
    "ps_3_sw "
    "dsy r0, -c0.w "
    "mov oC0, r0 ";

PS_CRITERIA PS_075_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_075_Desc = "ps_3_sw : dsy -src.xyzw is NOT allowed";
string PS_075 =
    "ps_3_sw "
    "dsy r0, -c0.xyzw "
    "mov oC0, r0 ";

PS_CRITERIA PS_076_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_076_Desc = "ps_3_sw : dsy -src.yzxw is NOT allowed";
string PS_076 =
    "ps_3_sw "
    "dsy r0, -c0.yzxw "
    "mov oC0, r0 ";

PS_CRITERIA PS_077_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_077_Desc = "ps_3_sw : dsy -src.zxyw is NOT allowed";
string PS_077 =
    "ps_3_sw "
    "dsy r0, -c0.zxyw "
    "mov oC0, r0 ";

PS_CRITERIA PS_078_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_078_Desc = "ps_3_sw : dsy -src.wzyx is NOT allowed";
string PS_078 =
    "ps_3_sw "
    "dsy r0, -c0.wzyx "
    "mov oC0, r0 ";
   
PS_CRITERIA PS_079_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_079_Desc = "ps_3_sw : dsy -src.yz is NOT allowed";
string PS_079 =
    "ps_3_sw "
    "dsy r0, -c0.yz "
    "mov oC0, r0 ";

PS_CRITERIA PS_080_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_080_Desc = "ps_3_sw : dsy -src.xx is NOT allowed";
string PS_080 =
    "ps_3_sw "
    "dsy r0, -c0.xx "
    "mov oC0, r0 ";

PS_CRITERIA PS_081_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_081_Desc = "ps_3_sw : dsy -src.xwz is NOT allowed";
string PS_081 =
    "ps_3_sw "
    "dsy r0, -c0.xwz "
    "mov oC0, r0 ";

PS_CRITERIA PS_082_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_082_Desc = "ps_3_sw : dsy -src.xzwy is NOT allowed";
string PS_082 =
    "ps_3_sw "
    "dsy r0, -c0.xzwy "
    "mov oC0, r0 ";

PS_CRITERIA PS_083_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_083_Desc = "ps_3_sw : dsy src_abs is not allowed";
string PS_083 =
    "ps_3_sw "
    "dsy r0, c0_abs "
    "mov oC0, r0 ";
    
PS_CRITERIA PS_084_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_084_Desc = "ps_3_sw : dsy src_abs.x is NOT allowed";
string PS_084 =
    "ps_3_sw "
    "dsy r0, c0_abs.x "
    "mov oC0, r0 ";
    
PS_CRITERIA PS_085_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_085_Desc = "ps_3_sw : dsy src_abs.y is NOT allowed";
string PS_085 =
    "ps_3_sw "
    "dsy r0, c0_abs.y "
    "mov oC0, r0 ";

PS_CRITERIA PS_086_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_086_Desc = "ps_3_sw : dsy src_abs.z is NOT allowed";
string PS_086 =
    "ps_3_sw "
    "dsy r0, c0_abs.z "
    "mov oC0, r0 ";

PS_CRITERIA PS_087_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_087_Desc = "ps_3_sw : dsy src_abs.w is NOT allowed";
string PS_087 =
    "ps_3_sw "
    "dsy r0, c0_abs.w "
    "mov oC0, r0 ";

PS_CRITERIA PS_088_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_088_Desc = "ps_3_sw : dsy src_abs.xyzw is NOT allowed";
string PS_088 =
    "ps_3_sw "
    "dsy r0, c0_abs.xyzw "
    "mov oC0, r0 ";

PS_CRITERIA PS_089_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_089_Desc = "ps_3_sw : dsy src_abs.yzxw is NOT allowed";
string PS_089 =
    "ps_3_sw "
    "dsy r0, c0_abs.yzxw "
    "mov oC0, r0 ";

PS_CRITERIA PS_090_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_090_Desc = "ps_3_sw : dsy src_abs.zxyw is NOT allowed";
string PS_090 =
    "ps_3_sw "
    "dsy r0, c0_abs.zxyw "
    "mov oC0, r0 ";

PS_CRITERIA PS_091_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_091_Desc = "ps_3_sw : dsy src_abs.wzyx is NOT allowed";
string PS_091 =
    "ps_3_sw "
    "dsy r0, c0_abs.wzyx "
    "mov oC0, r0 ";
   
PS_CRITERIA PS_092_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_092_Desc = "ps_3_sw : dsy src_abs.wx is NOT allowed";
string PS_092 =
    "ps_3_sw "
    "dsy r0, c0_abs.wx "
    "mov oC0, r0 ";

PS_CRITERIA PS_093_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_093_Desc = "ps_3_sw : dsy src_abs.yw is NOT allowed";
string PS_093 =
    "ps_3_sw "
    "dsy r0, c0_abs.yw "
    "mov oC0, r0 ";

PS_CRITERIA PS_094_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_094_Desc = "ps_3_sw : dsy src_abs.wxz is NOT allowed";
string PS_094 =
    "ps_3_sw "
    "dsy r0, c0_abs.wxz "
    "mov oC0, r0 ";

PS_CRITERIA PS_095_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_095_Desc = "ps_3_sw : dsy src_abs.yxzw is NOT allowed";
string PS_095 =
    "ps_3_sw "
    "dsy r0, c0_abs.yxzw "
    "mov oC0, r0 ";

PS_CRITERIA PS_096_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_096_Desc = "ps_3_sw : dsy -src_abs is not allowed";
string PS_096 =
    "ps_3_sw "
    "dsy r0, -c0_abs "
    "mov oC0, r0 ";
    
PS_CRITERIA PS_097_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_097_Desc = "ps_3_sw : dsy -src_abs.x is NOT allowed";
string PS_097 =
    "ps_3_sw "
    "dsy r0, -c0_abs.x "
    "mov oC0, r0 ";
    
PS_CRITERIA PS_098_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_098_Desc = "ps_3_sw : dsy -src_abs.y is NOT allowed";
string PS_098 =
    "ps_3_sw "
    "dsy r0, -c0_abs.y "
    "mov oC0, r0 ";

PS_CRITERIA PS_099_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_099_Desc = "ps_3_sw : dsy -src_abs.z is NOT allowed";
string PS_099 =
    "ps_3_sw "
    "dsy r0, -c0_abs.z "
    "mov oC0, r0 ";

PS_CRITERIA PS_100_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_100_Desc = "ps_3_sw : dsy -src_abs.w is NOT allowed";
string PS_100 =
    "ps_3_sw "
    "dsy r0, -c0_abs.w "
    "mov oC0, r0 ";

PS_CRITERIA PS_101_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_101_Desc = "ps_3_sw : dsy -src_abs.xyzw is NOT allowed";
string PS_101 =
    "ps_3_sw "
    "dsy r0, -c0_abs.xyzw "
    "mov oC0, r0 ";

PS_CRITERIA PS_102_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_102_Desc = "ps_3_sw : dsy -src_abs.yzxw is NOT allowed";
string PS_102 =
    "ps_3_sw "
    "dsy r0, -c0_abs.yzxw "
    "mov oC0, r0 ";

PS_CRITERIA PS_103_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_103_Desc = "ps_3_sw : dsy -src_abs.zxyw is NOT allowed";
string PS_103 =
    "ps_3_sw "
    "dsy r0, -c0_abs.zxyw "
    "mov oC0, r0 ";

PS_CRITERIA PS_104_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_104_Desc = "ps_3_sw : dsy -src_abs.wzyx is NOT allowed";
string PS_104 =
    "ps_3_sw "
    "dsy r0, -c0_abs.wzyx "
    "mov oC0, r0 ";
   
PS_CRITERIA PS_105_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_105_Desc = "ps_3_sw : dsy -src_abs.xz is NOT allowed";
string PS_105 =
    "ps_3_sw "
    "dsy r0, -c0_abs.xz "
    "mov oC0, r0 ";

PS_CRITERIA PS_106_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_106_Desc = "ps_3_sw : dsy -src_abs.zw is NOT allowed";
string PS_106 =
    "ps_3_sw "
    "dsy r0, -c0_abs.zw "
    "mov oC0, r0 ";

PS_CRITERIA PS_107_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_107_Desc = "ps_3_sw : dsy -src_abs.yzw is NOT allowed";
string PS_107 =
    "ps_3_sw "
    "dsy r0, -c0_abs.yzw "
    "mov oC0, r0 ";

PS_CRITERIA PS_108_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string PS_108_Desc = "ps_3_sw : dsy -src_abs.wxzy is NOT allowed";
string PS_108 =
    "ps_3_sw "
    "dsy r0, -c0_abs.wxzy "
    "mov oC0, r0 ";