 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_638_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_638_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break and rep i0 block";
string PS_638 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_639_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_639_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break and if_ne c0.x, c0.y block";
string PS_639 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if_ne c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_640_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_640_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break and if_ne c0.x, c0.y nop else block";
string PS_640 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if_ne c0.x, c0.y nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_641_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_641_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break and if p0.x block";
string PS_641 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_642_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_642_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_642 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_643_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_643_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_643 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_644_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_644_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break and rep i0 break block";
string PS_644 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_645_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_645_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_645 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_646_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_646_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_646 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_647_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_647_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break and loop aL, i0 break_eq c0.x, c0.y block";
string PS_647 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_648_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_648_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break and rep i0 break_eq c0.x, c0.y block";
string PS_648 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 break_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_649_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_649_Desc = "ps_2_sw : lrp is allowed in a rep i0 break and if b0 block";
string PS_649 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if b0 "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_650_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_650_Desc = "ps_2_sw : lrp is allowed in a rep i0 break and if b0 nop else block";
string PS_650 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if b0 nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_651_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_651_Desc = "ps_2_sw : lrp is NOT allowed in a rep i0 break and loop aL, i0 block";
string PS_651 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_652_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_652_Desc = "ps_2_sw : lrp is allowed in a rep i0 break and rep i0 block";
string PS_652 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_653_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_653_Desc = "ps_2_sw : lrp is allowed in a rep i0 break and if_le c0.x, c0.y block";
string PS_653 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if_le c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_654_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_654_Desc = "ps_2_sw : lrp is allowed in a rep i0 break and if_le c0.x, c0.y nop else block";
string PS_654 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if_le c0.x, c0.y nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_655_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_655_Desc = "ps_2_sw : lrp is allowed in a rep i0 break and if p0.x block";
string PS_655 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_656_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_656_Desc = "ps_2_sw : lrp is allowed in a rep i0 break and if p0.x nop else block";
string PS_656 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if p0.x nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_657_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_657_Desc = "ps_2_sw : lrp is NOT allowed in a rep i0 break and loop aL, i0 break block";
string PS_657 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_658_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_658_Desc = "ps_2_sw : lrp is allowed in a rep i0 break and rep i0 break block";
string PS_658 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 break "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_659_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_659_Desc = "ps_2_sw : lrp is NOT allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string PS_659 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_660_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_660_Desc = "ps_2_sw : lrp is allowed in a rep i0 break and rep i0 breakp p0.x block";
string PS_660 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_661_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_661_Desc = "ps_2_sw : lrp is NOT allowed in a rep i0 break and loop aL, i0 break_eq c0.x, c0.y block";
string PS_661 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_662_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_662_Desc = "ps_2_sw : lrp is allowed in a rep i0 break and rep i0 break_eq c0.x, c0.y block";
string PS_662 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 break_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_663_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_663_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and if b0 block";
string PS_663 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_664_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_664_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string PS_664 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_665_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_665_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string PS_665 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_666_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_666_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string PS_666 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_667_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_667_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and if_le c0.x, c0.y block";
string PS_667 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if_le c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_668_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_668_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and if_le c0.x, c0.y nop else block";
string PS_668 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if_le c0.x, c0.y nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_669_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_669_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string PS_669 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_670_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_670_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string PS_670 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_671_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_671_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string PS_671 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_672_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_672_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string PS_672 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_673_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_673_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_673 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_674_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_674_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_674 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_675_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_675_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string PS_675 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_676_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_676_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break_eq c0.x, c0.y block";
string PS_676 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_677_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_677_Desc = "ps_2_sw : lrp is allowed in a rep i0 breakp p0.x and if b0 block";
string PS_677 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_678_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_678_Desc = "ps_2_sw : lrp is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string PS_678 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_679_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_679_Desc = "ps_2_sw : lrp is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string PS_679 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_680_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_680_Desc = "ps_2_sw : lrp is allowed in a rep i0 breakp p0.x and rep i0 block";
string PS_680 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_681_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_681_Desc = "ps_2_sw : lrp is allowed in a rep i0 breakp p0.x and if_eq c0.x, c0.y block";
string PS_681 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_682_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_682_Desc = "ps_2_sw : lrp is allowed in a rep i0 breakp p0.x and if_eq c0.x, c0.y nop else block";
string PS_682 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if_eq c0.x, c0.y nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_683_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_683_Desc = "ps_2_sw : lrp is allowed in a rep i0 breakp p0.x and if p0.x block";
string PS_683 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_684_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_684_Desc = "ps_2_sw : lrp is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string PS_684 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_685_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_685_Desc = "ps_2_sw : lrp is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string PS_685 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_686_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_686_Desc = "ps_2_sw : lrp is allowed in a rep i0 breakp p0.x and rep i0 break block";
string PS_686 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_687_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_687_Desc = "ps_2_sw : lrp is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_687 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_688_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_688_Desc = "ps_2_sw : lrp is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_688 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_689_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_689_Desc = "ps_2_sw : lrp is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string PS_689 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_690_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_690_Desc = "ps_2_sw : lrp is allowed in a rep i0 breakp p0.x and rep i0 break_eq c0.x, c0.y block";
string PS_690 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 breakp p0.x "
	"	rep i0 break_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_691_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_691_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if b0 block";
string PS_691 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if b0 "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_692_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_692_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if b0 nop else block";
string PS_692 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if b0 nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_693_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_693_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 block";
string PS_693 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_694_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_694_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 block";
string PS_694 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_695_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_695_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if_ge c0.x, c0.y block";
string PS_695 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_696_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_696_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string PS_696 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_697_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_697_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if p0.x block";
string PS_697 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_698_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_698_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and if p0.x nop else block";
string PS_698 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if p0.x nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_699_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_699_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 break block";
string PS_699 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_700_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_700_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 break block";
string PS_700 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 break "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_701_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_701_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_701 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_702_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_702_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 breakp p0.x block";
string PS_702 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_703_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_703_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string PS_703 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_704_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_704_Desc = "ps_2_sw : lrp is NOT allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 break_eq c0.x, c0.y block";
string PS_704 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_705_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_705_Desc = "ps_2_sw : lrp is allowed in a rep i0 break_eq c0.x, c0.y and if b0 block";
string PS_705 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	if b0 "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_706_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_706_Desc = "ps_2_sw : lrp is allowed in a rep i0 break_eq c0.x, c0.y and if b0 nop else block";
string PS_706 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	if b0 nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_707_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_707_Desc = "ps_2_sw : lrp is NOT allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 block";
string PS_707 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_708_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_708_Desc = "ps_2_sw : lrp is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 block";
string PS_708 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_709_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_709_Desc = "ps_2_sw : lrp is allowed in a rep i0 break_eq c0.x, c0.y and if_ge c0.x, c0.y block";
string PS_709 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_710_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_710_Desc = "ps_2_sw : lrp is allowed in a rep i0 break_eq c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string PS_710 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_711_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_711_Desc = "ps_2_sw : lrp is allowed in a rep i0 break_eq c0.x, c0.y and if p0.x block";
string PS_711 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	if p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_712_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_712_Desc = "ps_2_sw : lrp is allowed in a rep i0 break_eq c0.x, c0.y and if p0.x nop else block";
string PS_712 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	if p0.x nop else "
	"		lrp r0, v0, v1, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_713_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_713_Desc = "ps_2_sw : lrp is NOT allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 break block";
string PS_713 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_714_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_714_Desc = "ps_2_sw : lrp is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 break block";
string PS_714 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_715_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_715_Desc = "ps_2_sw : lrp is NOT allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_715 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_716_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_716_Desc = "ps_2_sw : lrp is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 breakp p0.x block";
string PS_716 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_717_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_717_Desc = "ps_2_sw : lrp is NOT allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string PS_717 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_718_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_718_Desc = "ps_2_sw : lrp is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 break_eq c0.x, c0.y block";
string PS_718 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		lrp r0, v0, v1, c0 "
	"	endrep "
	"endrep ";


/*++

Copyright (c) Microsoft Corporation

Module Name:

    si.c

Abstract:

    Implements OS loader-speci