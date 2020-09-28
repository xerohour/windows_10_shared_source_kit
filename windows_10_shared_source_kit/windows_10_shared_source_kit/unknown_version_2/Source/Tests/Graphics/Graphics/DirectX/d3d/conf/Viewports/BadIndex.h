os, c0 ";

VS_CRITERIA VS_081_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_2_0 : pow source reg combination oD0, oD0 is NOT allowed";
string VS_081 = 
	"vs_2_0 "
	"pow r0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_082_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc 