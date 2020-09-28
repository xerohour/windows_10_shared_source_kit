#include "Test_Include.fx"

int Test_Count = 2;

string TestInfo
<
	string TestType = "PS";
>
= "ps_2_x_temps";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0200, -1, 12, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_2_x : 12 temp regs are supported";
string PS_001 = 
	"ps_2_x "
	"add r11, c0, c0 "
	"mov oC0, r11 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0200, -1, 32, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_2_x : 32 temp regs are supported";
string PS_002 = 
	"ps_2_x "
	"add r31, c0, c0 "
	"mov oC0, r31 ";