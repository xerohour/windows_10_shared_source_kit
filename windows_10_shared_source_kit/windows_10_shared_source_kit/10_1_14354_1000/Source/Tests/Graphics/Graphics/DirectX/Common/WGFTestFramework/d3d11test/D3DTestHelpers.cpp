// ***************************************************************
//  D3DTestHelpers   version:  1.0   �  date: 06/06/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#include "D3DTestHelpers.h"

unsigned int IntLog2( unsigned int v )
{
	static const int MultiplyDeBruijnBitPosition[32] = 
	{
	  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
	  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};

	v |= v >> 1; // first round down to power of 2 
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v = (v >> 1) + 1;

	return MultiplyDeBruijnBitPosition[(v * 0x077CB531UL) >> 27];
}
