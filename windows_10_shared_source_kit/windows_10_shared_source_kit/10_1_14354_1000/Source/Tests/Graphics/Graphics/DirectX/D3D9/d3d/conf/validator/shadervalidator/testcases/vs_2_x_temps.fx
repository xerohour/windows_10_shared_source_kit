#include "Test_Include.fx"

int Test_Count = 4;

string TestInfo
<
	string TestType = "VS";
>
= "vs_2_x_temps";

VS_CRITERIA VS_001_Criteria = { true, false, 0x0200, -1, 12, -1, -1, 0, -1, 9.109f };
string VS_001_Desc = "vs_2_x : 12 temp regs are supported (as dest registers)";
string VS_001 = 
	"vs_2_x "
	"add r11, c0, c0 ";
	
VS_CRITERIA VS_002_Criteria = { true, false, 0x0200, -1, 32, -1, -1, 0, -1, 9.109f };
string VS_002_Desc = "vs_2_x : 32 temp regs are supported (as dest registers)";
string VS_002 = 
	"vs_2_x "
	"add r31, c0, c0 ";

VS_CRITERIA VS_003_Criteria = { true, false, 0x0200, -1, 12, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_2_x : 12 temp regs are supported (as src registers)";
string VS_003 = 
	"vs_2_x "
	"add r0, r11, c0 ";
	
VS_CRITERIA VS_004_Criteria = { true, false, 0x0200, -1, 32, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_2_x : 32 temp regs are supported (as src registers)";
string VS_004 = 
	"vs_2_x "
	"add r0, r31, c0 ";
