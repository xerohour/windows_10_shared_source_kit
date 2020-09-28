// ***************************************************************
//  SingleBuffer   version:  1.0   ·  date: 01/24/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#include "SingleBuffer.h"

static const D3D11_SO_DECLARATION_ENTRY s_soDecl128[] =
{
	{ 0, "SV_Position", 0, 0, 4, 0 },
	{ 0, "data", 0, 0, 4, 0 },
	{ 0, "data", 1, 0, 4, 0 },
	{ 0, "data", 2, 0, 4, 0 },
	{ 0, "data", 3, 0, 4, 0 },
	{ 0, "data", 4, 0, 4, 0 },
	{ 0, "data", 5, 0, 4, 0 },
	{ 0, "data", 6, 0, 4, 0 },
	{ 0, "data", 7, 0, 4, 0 },
	{ 0, "data", 8, 0, 4, 0 },
	{ 0, "data", 9, 0, 4, 0 },
	{ 0, "data", 10, 0, 4, 0 },
	{ 0, "data", 11, 0, 4, 0 },
	{ 0, "data", 12, 0, 4, 0 },
	{ 0, "data", 13, 0, 4, 0 },
	{ 0, "data", 14, 0, 4, 0 },
	{ 0, "data", 15, 0, 4, 0 },
	{ 0, "data", 16, 0, 4, 0 },
	{ 0, "data", 17, 0, 4, 0 },
	{ 0, "data", 18, 0, 4, 0 },
	{ 0, "data", 19, 0, 4, 0 },
	{ 0, "data", 20, 0, 4, 0 },
	{ 0, "data", 21, 0, 4, 0 },
	{ 0, "data", 22, 0, 4, 0 },
	{ 0, "data", 23, 0, 4, 0 },
	{ 0, "data", 24, 0, 4, 0 },
	{ 0, "data", 25, 0, 4, 0 },
	{ 0, "data", 26, 0, 4, 0 },
	{ 0, "data", 27, 0, 4, 0 },
	{ 0, "data", 28, 0, 4, 0 },
	{ 0, "data", 29, 0, 4, 0 },
	{ 0, "data", 30, 0, 4, 0 },
};

static const D3D11_SO_DECLARATION_ENTRY s_soDecl64[] =
{
	{ 0, "SV_Position", 0, 0, 4, 0 },
	{ 0, "data", 0, 0, 2, 0 },
	{ 0, "data", 1, 0, 2, 0 },
	{ 0, "data", 2, 0, 2, 0 },
	{ 0, "data", 3, 0, 2, 0 },
	{ 0, "data", 4, 0, 2, 0 },
	{ 0, "data", 5, 0, 2, 0 },
	{ 0, "data", 6, 0, 2, 0 },
	{ 0, "data", 7, 0, 2, 0 },
	{ 0, "data", 8, 0, 2, 0 },
	{ 0, "data", 9, 0, 2, 0 },
	{ 0, "data", 10, 0, 2, 0 },
	{ 0, "data", 11, 0, 2, 0 },
	{ 0, "data", 12, 0, 2, 0 },
	{ 0, "data", 13, 0, 2, 0 },
	{ 0, "data", 14, 0, 2, 0 },
	{ 0, "data", 15, 0, 2, 0 },
	{ 0, "data", 16, 0, 2, 0 },
	{ 0, "data", 17, 0, 2, 0 },
	{ 0, "data", 18, 0, 2, 0 },
	{ 0, "data", 19, 0, 2, 0 },
	{ 0, "data", 20, 0, 2, 0 },
	{ 0, "data", 21, 0, 2, 0 },
	{ 0, "data", 22, 0, 2, 0 },
	{ 0, "data", 23, 0, 2, 0 },
	{ 0, "data", 24, 0, 2, 0 },
	{ 0, "data", 25, 0, 2, 0 },
	{ 0, "data", 26, 0, 2, 0 },
	{ 0, "data", 27, 0, 2, 0 },
	{ 0, "data", 28, 0, 2, 0 },
	{ 0, "data", 29, 0, 2, 0 },
};

static const D3D11_SO_DECLARATION_ENTRY s_soDecl32[] =
{
	{ 0, "SV_Position", 0, 0, 4, 0 },
	{ 0, "data", 0, 0, 2, 0 },
	{ 0, "data", 1, 0, 2, 0 },
	{ 0, "data", 2, 0, 2, 0 },
	{ 0, "data", 3, 0, 2, 0 },
	{ 0, "data", 4, 0, 2, 0 },
	{ 0, "data", 5, 0, 2, 0 },
	{ 0, "data", 6, 0, 2, 0 },
	{ 0, "data", 7, 0, 2, 0 },
	{ 0, "data", 8, 0, 2, 0 },
	{ 0, "data", 9, 0, 2, 0 },
	{ 0, "data", 10, 0, 2, 0 },
	{ 0, "data", 11, 0, 2, 0 },
	{ 0, "data", 12, 0, 2, 0 },
	{ 0, "data", 13, 0, 2, 0 },
};

static const D3D11_SO_DECLARATION_ENTRY s_soDecl64Gaps[] =
{
	{ 0, "SV_Position", 0, 0, 4, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 1, 0, 2, 0 },
	{ 0, "data", 2, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 4, 0, 2, 0 },
	{ 0, "data", 5, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 7, 0, 2, 0 },
	{ 0, "data", 8, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 10, 0, 2, 0 },
	{ 0, "data", 11, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 13, 0, 2, 0 },
	{ 0, "data", 14, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 16, 0, 2, 0 },
	{ 0, "data", 17, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 19, 0, 2, 0 },
	{ 0, "data", 20, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 22, 0, 2, 0 },
	{ 0, "data", 23, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 25, 0, 2, 0 },
	{ 0, "data", 26, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 28, 0, 2, 0 },
	{ 0, "data", 29, 0, 2, 0 },
};

static const D3D11_SO_DECLARATION_ENTRY s_soDecl128Gaps[] =
{
	{ 0, "SV_Position", 0, 0, 4, 0 },
	{ 0, NULL, 0, 0, 4, 0 },
	{ 0, "data", 1, 0, 4, 0 },
	{ 0, "data", 2, 0, 4, 0 },
	{ 0, NULL, 0, 0, 4, 0 },
	{ 0, "data", 4, 0, 4, 0 },
	{ 0, "data", 5, 0, 4, 0 },
	{ 0, NULL, 0, 0, 4, 0 },
	{ 0, "data", 7, 0, 4, 0 },
	{ 0, "data", 8, 0, 4, 0 },
	{ 0, NULL, 0, 0, 4, 0 },
	{ 0, "data", 10, 0, 4, 0 },
	{ 0, "data", 11, 0, 4, 0 },
	{ 0, NULL, 0, 0, 4, 0 },
	{ 0, "data", 13, 0, 4, 0 },
	{ 0, "data", 14, 0, 4, 0 },
	{ 0, NULL, 0, 0, 4, 0 },
	{ 0, "data", 16, 0, 4, 0 },
	{ 0, "data", 17, 0, 4, 0 },
	{ 0, NULL, 0, 0, 4, 0 },
	{ 0, "data", 19, 0, 4, 0 },
	{ 0, "data", 20, 0, 4, 0 },
	{ 0, NULL, 0, 0, 4, 0 },
	{ 0, "data", 22, 0, 4, 0 },
	{ 0, "data", 23, 0, 4, 0 },
	{ 0, NULL, 0, 0, 4, 0 },
	{ 0, "data", 25, 0, 4, 0 },
	{ 0, "data", 26, 0, 4, 0 },
	{ 0, NULL, 0, 0, 4, 0 },
	{ 0, "data", 28, 0, 4, 0 },
	{ 0, "data", 29, 0, 4, 0 },
	{ 0, NULL, 0, 0, 4, 0 },
};

static const D3D11_SO_DECLARATION_ENTRY s_soDecl32Gaps[] =
{
	{ 0, "SV_Position", 0, 0, 4, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 1, 0, 2, 0 },
	{ 0, "data", 2, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 4, 0, 2, 0 },
	{ 0, "data", 5, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 7, 0, 2, 0 },
	{ 0, "data", 8, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 10, 0, 2, 0 },
	{ 0, "data", 11, 0, 2, 0 },
	{ 0, NULL, 0, 0, 2, 0 },
	{ 0, "data", 13, 0, 2, 0 },
};

static const D3D11_SO_DECLARATION_ENTRY s_soDecl64Split[] =
{
	{ 0, "SV_Position", 0, 0, 1, 0 },
	{ 0, "SV_Position", 0, 1, 1, 0 },
	{ 0, "SV_Position", 0, 2, 1, 0 },
	{ 0, "SV_Position", 0, 3, 1, 0 },
	{ 0, "data", 0, 0, 1, 0 },
	{ 0, "data", 0, 1, 1, 0 },
	{ 0, "data", 1, 0, 1, 0 },
	{ 0, "data", 1, 1, 1, 0 },
	{ 0, "data", 2, 0, 1, 0 },
	{ 0, "data", 2, 1, 1, 0 },
	{ 0, "data", 3, 0, 1, 0 },
	{ 0, "data", 3, 1, 1, 0 },
	{ 0, "data", 4, 0, 1, 0 },
	{ 0, "data", 4, 1, 1, 0 },
	{ 0, "data", 5, 0, 1, 0 },
	{ 0, "data", 5, 1, 1, 0 },
	{ 0, "data", 6, 0, 1, 0 },
	{ 0, "data", 6, 1, 1, 0 },
	{ 0, "data", 7, 0, 1, 0 },
	{ 0, "data", 7, 1, 1, 0 },
	{ 0, "data", 8, 0, 1, 0 },
	{ 0, "data", 8, 1, 1, 0 },
	{ 0, "data", 9, 0, 1, 0 },
	{ 0, "data", 9, 1, 1, 0 },
	{ 0, "data", 10, 0, 1, 0 },
	{ 0, "data", 10, 1, 1, 0 },
	{ 0, "data", 11, 0, 1, 0 },
	{ 0, "data", 11, 1, 1, 0 },
	{ 0, "data", 12, 0, 1, 0 },
	{ 0, "data", 12, 1, 1, 0 },
	{ 0, "data", 13, 0, 1, 0 },
	{ 0, "data", 13, 1, 1, 0 },
	{ 0, "data", 14, 0, 1, 0 },
	{ 0, "data", 14, 1, 1, 0 },
	{ 0, "data", 15, 0, 1, 0 },
	{ 0, "data", 15, 1, 1, 0 },
	{ 0, "data", 16, 0, 1, 0 },
	{ 0, "data", 16, 1, 1, 0 },
	{ 0, "data", 17, 0, 1, 0 },
	{ 0, "data", 17, 1, 1, 0 },
	{ 0, "data", 18, 0, 1, 0 },
	{ 0, "data", 18, 1, 1, 0 },
	{ 0, "data", 19, 0, 1, 0 },
	{ 0, "data", 19, 1, 1, 0 },
	{ 0, "data", 20, 0, 1, 0 },
	{ 0, "data", 20, 1, 1, 0 },
	{ 0, "data", 21, 0, 1, 0 },
	{ 0, "data", 21, 1, 1, 0 },
	{ 0, "data", 22, 0, 1, 0 },
	{ 0, "data", 22, 1, 1, 0 },
	{ 0, "data", 23, 0, 1, 0 },
	{ 0, "data", 23, 1, 1, 0 },
	{ 0, "data", 24, 0, 1, 0 },
	{ 0, "data", 24, 1, 1, 0 },
	{ 0, "data", 25, 0, 1, 0 },
	{ 0, "data", 25, 1, 1, 0 },
	{ 0, "data", 26, 0, 1, 0 },
	{ 0, "data", 26, 1, 1, 0 },
	{ 0, "data", 27, 0, 1, 0 },
	{ 0, "data", 27, 1, 1, 0 },
	{ 0, "data", 28, 0, 1, 0 },
	{ 0, "data", 28, 1, 1, 0 },
	{ 0, "data", 29, 0, 1, 0 },
	{ 0, "data", 29, 1, 1, 0 },
};

static const D3D11_SO_DECLARATION_ENTRY s_soDecl128Split[] =
{
	{ 0, "SV_Position", 0, 0, 1, 0 },
	{ 0, "SV_Position", 0, 1, 1, 0 },
	{ 0, "SV_Position", 0, 2, 1, 0 },
	{ 0, "SV_Position", 0, 3, 1, 0 },
	{ 0, "data", 0, 0, 1, 0 },
	{ 0, "data", 0, 1, 1, 0 },
	{ 0, "data", 0, 2, 1, 0 },
	{ 0, "data", 0, 3, 1, 0 },
	{ 0, "data", 1, 0, 1, 0 },
	{ 0, "data", 1, 1, 1, 0 },
	{ 0, "data", 1, 2, 1, 0 },
	{ 0, "data", 1, 3, 1, 0 },
	{ 0, "data", 2, 0, 1, 0 },
	{ 0, "data", 2, 1, 1, 0 },
	{ 0, "data", 2, 2, 1, 0 },
	{ 0, "data", 2, 3, 1, 0 },
	{ 0, "data", 3, 0, 1, 0 },
	{ 0, "data", 3, 1, 1, 0 },
	{ 0, "data", 3, 2, 1, 0 },
	{ 0, "data", 3, 3, 1, 0 },
	{ 0, "data", 4, 0, 1, 0 },
	{ 0, "data", 4, 1, 1, 0 },
	{ 0, "data", 4, 2, 1, 0 },
	{ 0, "data", 4, 3, 1, 0 },
	{ 0, "data", 5, 0, 1, 0 },
	{ 0, "data", 5, 1, 1, 0 },
	{ 0, "data", 5, 2, 1, 0 },
	{ 0, "data", 5, 3, 1, 0 },
	{ 0, "data", 6, 0, 1, 0 },
	{ 0, "data", 6, 1, 1, 0 },
	{ 0, "data", 6, 2, 1, 0 },
	{ 0, "data", 6, 3, 1, 0 },
	{ 0, "data", 7, 0, 1, 0 },
	{ 0, "data", 7, 1, 1, 0 },
	{ 0, "data", 7, 2, 1, 0 },
	{ 0, "data", 7, 3, 1, 0 },
	{ 0, "data", 8, 0, 1, 0 },
	{ 0, "data", 8, 1, 1, 0 },
	{ 0, "data", 8, 2, 1, 0 },
	{ 0, "data", 8, 3, 1, 0 },
	{ 0, "data", 9, 0, 1, 0 },
	{ 0, "data", 9, 1, 1, 0 },
	{ 0, "data", 9, 2, 1, 0 },
	{ 0, "data", 9, 3, 1, 0 },
	{ 0, "data", 10, 0, 1, 0 },
	{ 0, "data", 10, 1, 1, 0 },
	{ 0, "data", 10, 2, 1, 0 },
	{ 0, "data", 10, 3, 1, 0 },
	{ 0, "data", 11, 0, 1, 0 },
	{ 0, "data", 11, 1, 1, 0 },
	{ 0, "data", 11, 2, 1, 0 },
	{ 0, "data", 11, 3, 1, 0 },
	{ 0, "data", 12, 0, 1, 0 },
	{ 0, "data", 12, 1, 1, 0 },
	{ 0, "data", 12, 2, 1, 0 },
	{ 0, "data", 12, 3, 1, 0 },
	{ 0, "data", 13, 0, 1, 0 },
	{ 0, "data", 13, 1, 1, 0 },
	{ 0, "data", 13, 2, 1, 0 },
	{ 0, "data", 13, 3, 1, 0 },
	{ 0, "data", 14, 0, 1, 0 },
	{ 0, "data", 14, 1, 1, 0 },
	{ 0, "data", 14, 2, 1, 0 },
	{ 0, "data", 14, 3, 1, 0 },
	{ 0, "data", 15, 0, 1, 0 },
	{ 0, "data", 15, 1, 1, 0 },
	{ 0, "data", 15, 2, 1, 0 },
	{ 0, "data", 15, 3, 1, 0 },
	{ 0, "data", 16, 0, 1, 0 },
	{ 0, "data", 16, 1, 1, 0 },
	{ 0, "data", 16, 2, 1, 0 },
	{ 0, "data", 16, 3, 1, 0 },
	{ 0, "data", 17, 0, 1, 0 },
	{ 0, "data", 17, 1, 1, 0 },
	{ 0, "data", 17, 2, 1, 0 },
	{ 0, "data", 17, 3, 1, 0 },
	{ 0, "data", 18, 0, 1, 0 },
	{ 0, "data", 18, 1, 1, 0 },
	{ 0, "data", 18, 2, 1, 0 },
	{ 0, "data", 18, 3, 1, 0 },
	{ 0, "data", 19, 0, 1, 0 },
	{ 0, "data", 19, 1, 1, 0 },
	{ 0, "data", 19, 2, 1, 0 },
	{ 0, "data", 19, 3, 1, 0 },
	{ 0, "data", 20, 0, 1, 0 },
	{ 0, "data", 20, 1, 1, 0 },
	{ 0, "data", 20, 2, 1, 0 },
	{ 0, "data", 20, 3, 1, 0 },
	{ 0, "data", 21, 0, 1, 0 },
	{ 0, "data", 21, 1, 1, 0 },
	{ 0, "data", 21, 2, 1, 0 },
	{ 0, "data", 21, 3, 1, 0 },
	{ 0, "data", 22, 0, 1, 0 },
	{ 0, "data", 22, 1, 1, 0 },
	{ 0, "data", 22, 2, 1, 0 },
	{ 0, "data", 22, 3, 1, 0 },
	{ 0, "data", 23, 0, 1, 0 },
	{ 0, "data", 23, 1, 1, 0 },
	{ 0, "data", 23, 2, 1, 0 },
	{ 0, "data", 23, 3, 1, 0 },
	{ 0, "data", 24, 0, 1, 0 },
	{ 0, "data", 24, 1, 1, 0 },
	{ 0, "data", 24, 2, 1, 0 },
	{ 0, "data", 24, 3, 1, 0 },
	{ 0, "data", 25, 0, 1, 0 },
	{ 0, "data", 25, 1, 1, 0 },
	{ 0, "data", 25, 2, 1, 0 },
	{ 0, "data", 25, 3, 1, 0 },
	{ 0, "data", 26, 0, 1, 0 },
	{ 0, "data", 26, 1, 1, 0 },
	{ 0, "data", 26, 2, 1, 0 },
	{ 0, "data", 26, 3, 1, 0 },
	{ 0, "data", 27, 0, 1, 0 },
	{ 0, "data", 27, 1, 1, 0 },
	{ 0, "data", 27, 2, 1, 0 },
	{ 0, "data", 27, 3, 1, 0 },
	{ 0, "data", 28, 0, 1, 0 },
	{ 0, "data", 28, 1, 1, 0 },
	{ 0, "data", 28, 2, 1, 0 },
	{ 0, "data", 28, 3, 1, 0 },
	{ 0, "data", 29, 0, 1, 0 },
	{ 0, "data", 29, 1, 1, 0 },
	{ 0, "data", 29, 2, 1, 0 },
	{ 0, "data", 29, 3, 1, 0 },
	{ 0, "data", 30, 0, 1, 0 },
	{ 0, "data", 30, 1, 1, 0 },
	{ 0, "data", 30, 2, 1, 0 },
	{ 0, "data", 30, 3, 1, 0 },
};

static const D3D11_SO_DECLARATION_ENTRY s_soDecl32Split[] =
{
	{ 0, "SV_Position", 0, 0, 1, 0 },
	{ 0, "SV_Position", 0, 1, 1, 0 },
	{ 0, "SV_Position", 0, 2, 1, 0 },
	{ 0, "SV_Position", 0, 3, 1, 0 },
	{ 0, "data", 0, 0, 1, 0 },
	{ 0, "data", 0, 1, 1, 0 },
	{ 0, "data", 1, 0, 1, 0 },
	{ 0, "data", 1, 1, 1, 0 },
	{ 0, "data", 2, 0, 1, 0 },
	{ 0, "data", 2, 1, 1, 0 },
	{ 0, "data", 3, 0, 1, 0 },
	{ 0, "data", 3, 1, 1, 0 },
	{ 0, "data", 4, 0, 1, 0 },
	{ 0, "data", 4, 1, 1, 0 },
	{ 0, "data", 5, 0, 1, 0 },
	{ 0, "data", 5, 1, 1, 0 },
	{ 0, "data", 6, 0, 1, 0 },
	{ 0, "data", 6, 1, 1, 0 },
	{ 0, "data", 7, 0, 1, 0 },
	{ 0, "data", 7, 1, 1, 0 },
	{ 0, "data", 8, 0, 1, 0 },
	{ 0, "data", 8, 1, 1, 0 },
	{ 0, "data", 9, 0, 1, 0 },
	{ 0, "data", 9, 1, 1, 0 },
	{ 0, "data", 10, 0, 1, 0 },
	{ 0, "data", 10, 1, 1, 0 },
	{ 0, "data", 11, 0, 1, 0 },
	{ 0, "data", 11, 1, 1, 0 },
	{ 0, "data", 12, 0, 1, 0 },
	{ 0, "data", 12, 1, 1, 0 },
	{ 0, "data", 13, 0, 1, 0 },
	{ 0, "data", 13, 1, 1, 0 },
};

static const D3D11_SO_DECLARATION_ENTRY s_soDecl8[] =
{
	{ 0, "SV_Position", 0, 0, 4, 0 },
	{ 0, "data", 0, 0, 2, 0 },
	{ 0, "data", 1, 0, 2, 0 },
};

static const DECL_MODE s_declModeTable[] =
{
	DECL_MODE_SIMPLE,
	DECL_MODE_GAPS,
	DECL_MODE_SPLIT_ELEMENTS
};

//////////////////////////////////////////////////////////////////////////
// CStreamOutSingleBuffer
//////////////////////////////////////////////////////////////////////////
CStreamOutSingleBuffer::CStreamOutSingleBuffer() :
	m_numInputVerts( 0 ),
	m_numPrimsPerInputVertex( 0 ),
	m_soNumVertexComponents( 0 ),
	m_numDrawCalls( 0 ),
	m_outputSlot( 0 ),
	m_bufferSize( 0 ),
	m_bufferOffset( 0 ),
	m_soVertexStride( 0 )
{
	m_numInputVerts = 4;
}

//////////////////////////////////////////////////////////////////////////

static const UINT s_soBufferSizeTable64[] =
{
	sizeof( CStreamOutSingleBuffer::GSOUT64 ) * 3 * 12,
	sizeof( CStreamOutSingleBuffer::GSOUT64 ) * 3 * 3,
};

static const UINT s_soBufferSizeTable128[] =
{
	sizeof( CStreamOutSingleBuffer::GSOUT128 ) * 3 * 12,
	sizeof( CStreamOutSingleBuffer::GSOUT128 ) * 3 * 3,
};

static const UINT s_soBufferSizeTable32[] =
{
	sizeof( CStreamOutSingleBuffer::GSOUT32 ) * 3 * 12,
	sizeof( CStreamOutSingleBuffer::GSOUT32 ) * 3 * 3,
};

static const UINT s_soBufferSizeTable8[] =
{
	sizeof( CStreamOutSingleBuffer::GSOUT8 ) * 3 * 12,
	sizeof( CStreamOutSingleBuffer::GSOUT8 ) * 3 * 3,
};

static const UINT s_soVertexStrideTable64[] =
{
	sizeof( CStreamOutSingleBuffer::GSOUT64 ),
	sizeof( CStreamOutSingleBuffer::GSOUT64 ) + 60,
};

static const UINT s_soVertexStrideTable128[] =
{
	sizeof( CStreamOutSingleBuffer::GSOUT128 ),
	sizeof( CStreamOutSingleBuffer::GSOUT128 ) + 60, // the extra 60 is a gap.
};

static const UINT s_soVertexStrideTable32[] =
{
	sizeof( CStreamOutSingleBuffer::GSOUT32 ),
	sizeof( CStreamOutSingleBuffer::GSOUT32 ) + 40,
};

static const UINT s_soVertexStrideTable8[] =
{
	sizeof( CStreamOutSingleBuffer::GSOUT8 ),
	sizeof( CStreamOutSingleBuffer::GSOUT8 ) + 8
};

static const UINT s_maxVertexCountTable[] =
{
	6,
	8
};

static const tstring s_gsOutputPrimitiveTypes[] =
{
	tstring( _T( "Triangles" ) ),
	tstring( _T( "Lines" ) ),
	tstring( _T( "Points" ) ),
};

static const tstring s_extraBindAttributes[] =
{
	tstring( _T( "None" ) ),
	tstring( _T( "All" ) ),
	tstring( _T( "VertexBuffer" ) ),
};

void CStreamOutSingleBuffer::InitTestParameters()
{
	CTableValueSet<UINT> *pBufferSizeTable128ValueSet = new CTableValueSet<UINT>( s_soBufferSizeTable128, sizeof( s_soBufferSizeTable128[0] ), sizeof( s_soBufferSizeTable128 ) / sizeof( s_soBufferSizeTable128[0] ) );
	CTableValueSet<UINT> *pBufferSizeTable64ValueSet = new CTableValueSet<UINT>( s_soBufferSizeTable64, sizeof( s_soBufferSizeTable64[0] ), sizeof( s_soBufferSizeTable64 ) / sizeof( s_soBufferSizeTable64[0] ) );
	CTableValueSet<UINT> *pBufferSizeTable32ValueSet = new CTableValueSet<UINT>( s_soBufferSizeTable32, sizeof( s_soBufferSizeTable32[0] ), sizeof( s_soBufferSizeTable32 ) / sizeof( s_soBufferSizeTable32[0] ) );
	CTableValueSet<UINT> *pBufferSizeTable8ValueSet = new CTableValueSet<UINT>( s_soBufferSizeTable8, sizeof( s_soBufferSizeTable8[0] ), sizeof( s_soBufferSizeTable8 ) / sizeof( s_soBufferSizeTable8[0] ) );
	CTableValueSet<UINT> *pVertexStrideTable128ValueSet = new CTableValueSet<UINT>( s_soVertexStrideTable128, sizeof( s_soVertexStrideTable128[0] ), sizeof( s_soVertexStrideTable128 ) / sizeof( s_soVertexStrideTable128[0] ) );
	CTableValueSet<UINT> *pVertexStrideTable64ValueSet = new CTableValueSet<UINT>( s_soVertexStrideTable64, sizeof( s_soVertexStrideTable64[0] ), sizeof( s_soVertexStrideTable64 ) / sizeof( s_soVertexStrideTable64[0] ) );
	CTableValueSet<UINT> *pVertexStrideTable32ValueSet = new CTableValueSet<UINT>( s_soVertexStrideTable32, sizeof( s_soVertexStrideTable32[0] ), sizeof( s_soVertexStrideTable32 ) / sizeof( s_soVertexStrideTable32[0] ) );
	CTableValueSet<UINT> *pVertexStrideTable8ValueSet = new CTableValueSet<UINT>( s_soVertexStrideTable8, sizeof( s_soVertexStrideTable8[0] ), sizeof( s_soVertexStrideTable8 ) / sizeof( s_soVertexStrideTable8[0] ) );
	CTableValueSet<tstring> *pGSOutputPrimitiveTypesValueSet = new CTableValueSet<tstring>( s_gsOutputPrimitiveTypes, sizeof( s_gsOutputPrimitiveTypes[0] ), sizeof( s_gsOutputPrimitiveTypes ) / sizeof( s_gsOutputPrimitiveTypes[0] ) );
	CTableValueSet<tstring> *pExtraBindAttributesValueSet = new CTableValueSet<tstring>( s_extraBindAttributes, sizeof( s_extraBindAttributes[0] ), sizeof( s_extraBindAttributes ) / sizeof( s_extraBindAttributes[0] ) );
	CUserValueSet<UINT> *pNumDrawCallsValueSet = new CUserValueSet<UINT>();
	pNumDrawCallsValueSet->AddValue( 1 );
	pNumDrawCallsValueSet->AddValue( 3 );

	CTestCaseParameter<UINT> *pNumInputVertsParam = AddParameter<UINT>( "NumInputVerts", &m_numInputVerts );
	CTestCaseParameter<UINT> *pNumPrimsPerInputVertexParam = AddParameter<UINT>( "NumPrimsPerInputVertex", &m_numPrimsPerInputVertex );
	CTestCaseParameter<UINT> *pSOOffset = AddParameter<UINT>( "SOOffset", &m_bufferOffset );
	CTestCaseParameter<UINT> *pSOBufferSize = AddParameter<UINT>( "SOBufferSize", &m_bufferSize );
	CTestCaseParameter<UINT> *pSOVertexStride = AddParameter<UINT>( "SOVertexStride", &m_soVertexStride );
	CTestCaseParameter<UINT> *pSlotIndex = AddParameter<UINT>( "SOOutputSlot", &m_outputSlot );

	testfactor::RFactor forceRebindEachDrawTrue = AddParameter( "ForceRebindEachDraw", &m_forceRebindEachDraw, true );
	testfactor::RFactor forceRebindEachDrawFalse = AddParameterValue( "ForceRebindEachDraw", false );

	testfactor::RFactor numInputVertsPointList = AddParameterValueSet<UINT>( pNumInputVertsParam, new CRangeValueSet<UINT>( 1, s_pointsSize, s_pointsSize - 1 ) );
	testfactor::RFactor numInputVertsLineList = AddParameterValueSet<UINT>( pNumInputVertsParam, new CRangeValueSet<UINT>( 1, s_lineListSize, s_lineListSize - 1 ) );
	testfactor::RFactor numInputVertsLineStrip = AddParameterValueSet<UINT>( pNumInputVertsParam, new CRangeValueSet<UINT>( 1, s_lineStripSize, s_lineStripSize - 1 ) );
	testfactor::RFactor numInputVertsTriangleList = AddParameterValueSet<UINT>( pNumInputVertsParam, new CRangeValueSet<UINT>( 1, s_triangleListSize, s_triangleListSize - 1 ) );
	testfactor::RFactor numInputVertsTriangleStrip = AddParameterValueSet<UINT>( pNumInputVertsParam, new CRangeValueSet<UINT>( 1, s_triangleStripSize, s_triangleStripSize - 1 ) );
	testfactor::RFactor numInputVertsLineListAdj = AddParameterValueSet<UINT>( pNumInputVertsParam, new CRangeValueSet<UINT>( 1, s_lineListAdjSize, s_lineListAdjSize - 1 ) );
	testfactor::RFactor numInputVertsLineStripAdj = AddParameterValueSet<UINT>( pNumInputVertsParam, new CRangeValueSet<UINT>( 1, s_lineStripSize, s_lineStripSize - 1 ) );
	testfactor::RFactor numInputVertsTriangleListAdj = AddParameterValueSet<UINT>( pNumInputVertsParam, new CRangeValueSet<UINT>( 1, s_triangleListAdjSize, s_triangleListAdjSize - 1 ) );
	testfactor::RFactor numInputVertsTriangleStripAdj = AddParameterValueSet<UINT>( pNumInputVertsParam, new CRangeValueSet<UINT>( 1, s_triangleStripAdjSize, s_triangleStripAdjSize - 1 ) );

	testfactor::RFactor defaultNumInputVertsPointList = AddParameterValue<UINT>( pNumInputVertsParam, s_pointsSize );
	testfactor::RFactor defaultNumInputVertsLineList = AddParameterValue<UINT>( pNumInputVertsParam, s_lineListSize );
	testfactor::RFactor defaultNumInputVertsLineStrip = AddParameterValue<UINT>( pNumInputVertsParam, s_lineStripSize );
	testfactor::RFactor defaultNumInputVertsTriangleList = AddParameterValue<UINT>( pNumInputVertsParam, s_triangleListSize );
	testfactor::RFactor defaultNumInputVertsTriangleStrip = AddParameterValue<UINT>( pNumInputVertsParam, s_triangleStripSize );
	testfactor::RFactor defaultNumInputVertsLineListAdj = AddParameterValue<UINT>( pNumInputVertsParam, s_lineListAdjSize );
	testfactor::RFactor defaultNumInputVertsLineStripAdj = AddParameterValue<UINT>( pNumInputVertsParam, s_lineStripSize );
	testfactor::RFactor defaultNumInputVertsTriangleListAdj = AddParameterValue<UINT>( pNumInputVertsParam, s_triangleListAdjSize );
	testfactor::RFactor defaultNumInputVertsTriangleStripAdj = AddParameterValue<UINT>( pNumInputVertsParam, s_triangleStripAdjSize );

	testfactor::RFactor soOffset = AddParameterValueSet<UINT>( pSOOffset, new CRangeValueSet<UINT>( 0, 128, 32 ) );
	testfactor::RFactor defaultSOOffset = AddParameterValue<UINT>( pSOOffset, 0 );
	testfactor::RFactor numPrimsPerInputVertex = AddParameterValueSet<UINT>( pNumPrimsPerInputVertexParam, new CRangeValueSet<UINT>( 2, 10, 2 ) );
	testfactor::RFactor defaultNumPrimsPerInputVertex = AddParameterValue<UINT>( pNumPrimsPerInputVertexParam, 2 );
	testfactor::RFactor gsMaxVertexCount = AddParameter<UINT>( "GSMaxVertexCount", &m_gsMaxVertexCount, new CTableValueSet<UINT>( s_maxVertexCountTable, sizeof( s_maxVertexCountTable[0] ), sizeof( s_maxVertexCountTable ) / sizeof( s_maxVertexCountTable[0] ) ) );
	testfactor::RFactor gsMaxVertexCountDefault = AddParameterValue<UINT>( "GSMaxVertexCount", 6 );

	testfactor::RFactor useNullGSFalse = AddParameter<bool>( "UseNullGS", &m_useNullGS, false );
	testfactor::RFactor useNullGSTrue = AddParameterValue<bool>( "UseNullGS", true );
	
	testfactor::RFactor extraBindAttributes = AddParameter( "ExtraBindAttributes", &m_extraBindAttributes, pExtraBindAttributesValueSet );

	testfactor::RFactor inputTopologyPointList = AddParameter( "InputTopology", &m_inputTopology, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	testfactor::RFactor inputTopologyLineList = AddParameterValue( "InputTopology", D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
	testfactor::RFactor inputTopologyLineStrip = AddParameterValue( "InputTopology", D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
	testfactor::RFactor inputTopologyTriangleList = AddParameterValue( "InputTopology", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	testfactor::RFactor inputTopologyTriangleStrip = AddParameterValue( "InputTopology", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	testfactor::RFactor inputTopologyLineListAdj = AddParameterValue( "InputTopology", D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ );
	testfactor::RFactor inputTopologyLineStripAdj = AddParameterValue( "InputTopology", D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ );
	testfactor::RFactor inputTopologyTriangleListAdj = AddParameterValue( "InputTopology", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ );
	testfactor::RFactor inputTopologyTriangleStripAdj = AddParameterValue( "InputTopology", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ );

	testfactor::RFactor outputPrimitiveType = AddParameter( "OutputPrimitiveType", &m_gsOutputPrimitiveType, pGSOutputPrimitiveTypesValueSet );

	testfactor::RFactor numDrawCalls = AddParameter<UINT>( "NumDrawCalls", &m_numDrawCalls, pNumDrawCallsValueSet );
	testfactor::RFactor numDrawCallsDefault = AddParameterValue<UINT>( "NumDrawCalls", 4 );	

	testfactor::RFactor soBufferSize128 = AddParameterValueSet<UINT>( pSOBufferSize, pBufferSizeTable128ValueSet );
	testfactor::RFactor soBufferSize64 = AddParameterValueSet<UINT>( pSOBufferSize, pBufferSizeTable64ValueSet );
	testfactor::RFactor soBufferSize32 = AddParameterValueSet<UINT>( pSOBufferSize, pBufferSizeTable32ValueSet );
	testfactor::RFactor soBufferSize8 = AddParameterValueSet<UINT>( pSOBufferSize, pBufferSizeTable8ValueSet );

	testfactor::RFactor soVertexStride128 = AddParameterValueSet<UINT>( pSOVertexStride, pVertexStrideTable128ValueSet );
	testfactor::RFactor soVertexStride64 = AddParameterValueSet<UINT>( pSOVertexStride, pVertexStrideTable64ValueSet );
	testfactor::RFactor soVertexStride32 = AddParameterValueSet<UINT>( pSOVertexStride, pVertexStrideTable32ValueSet );
	testfactor::RFactor soVertexStride8 = AddParameterValueSet<UINT>( pSOVertexStride, pVertexStrideTable8ValueSet );

	testfactor::RFactor soNumVertexComponents128 = AddParameter<UINT>( "NumVertexComponents", &m_soNumVertexComponents, 128 );
	testfactor::RFactor soNumVertexComponents64 = AddParameterValue<UINT>( "NumVertexComponents", 64 );
	testfactor::RFactor soNumVertexComponents32 = AddParameterValue<UINT>( "NumVertexComponents", 32 );
	testfactor::RFactor soNumVertexComponents8 = AddParameterValue<UINT>( "NumVertexComponents", 8 );
	
	testfactor::RFactor vertexInput = numInputVertsPointList * inputTopologyPointList;
	testfactor::RFactor vertexInputNoGS = ( numInputVertsPointList * inputTopologyPointList
											+ numInputVertsLineList * inputTopologyLineList
											+ numInputVertsTriangleList * inputTopologyTriangleList
											+ numInputVertsLineStrip * inputTopologyLineStrip
											+ numInputVertsLineStripAdj * inputTopologyLineStripAdj
											+ numInputVertsLineListAdj * inputTopologyLineListAdj
											+ numInputVertsTriangleListAdj * inputTopologyTriangleListAdj
											+ numInputVertsTriangleStrip * inputTopologyTriangleStrip
											+ numInputVertsTriangleStripAdj * inputTopologyTriangleStripAdj
										  );
										  
	testfactor::RFactor defaultVertexInput = defaultNumInputVertsPointList * inputTopologyPointList;
	testfactor::RFactor defaultVertexInputNoGS = ( defaultNumInputVertsPointList * inputTopologyPointList
											+ defaultNumInputVertsLineList * inputTopologyLineList
											+ defaultNumInputVertsLineListAdj * inputTopologyLineListAdj
											+ defaultNumInputVertsLineStrip * inputTopologyLineStrip
											+ defaultNumInputVertsLineStripAdj * inputTopologyLineStripAdj
											+ defaultNumInputVertsTriangleList * inputTopologyTriangleList 
											+ defaultNumInputVertsTriangleListAdj * inputTopologyTriangleListAdj 
											+ defaultNumInputVertsTriangleStrip * inputTopologyTriangleStrip
											+ defaultNumInputVertsTriangleStripAdj * inputTopologyTriangleStripAdj
										  );

	testfactor::RFactor slotIndex0 = AddParameterValue<UINT>( pSlotIndex, 0 );
	testfactor::RFactor slotIndexAll = AddParameterValueSet<UINT>( pSlotIndex, RangeValueSet<UINT>(0, 3, 1) );
	testfactor::RFactor slotIndex = (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0) ? slotIndexAll : slotIndex0; // addressable stream out is only supported on fl 11 on greater
										  
	testfactor::RFactor cycleVertexInputSet = vertexInput * defaultNumPrimsPerInputVertex * defaultSOOffset;
	testfactor::RFactor cycleVertexInputSetNoGS = vertexInputNoGS * defaultSOOffset;
	
	testfactor::RFactor cycleNumPrimsPerInputVertexSet = defaultVertexInput * numPrimsPerInputVertex * defaultSOOffset;
	
	testfactor::RFactor cycleSOOffsetSet = defaultVertexInput * defaultNumPrimsPerInputVertex * soOffset;
	testfactor::RFactor cycleSOOffsetSetNoGS = defaultVertexInputNoGS * soOffset;

	testfactor::RFactor soDeclMode32And64And128 = AddParameter( _T( "DeclMode" ), &m_declMode, new CTableValueSet<DECL_MODE>( s_declModeTable, sizeof( s_declModeTable[0] ), sizeof( s_declModeTable ) / sizeof( s_declModeTable[0] ) ) );
	testfactor::RFactor soDeclMode8 = AddParameterValue( _T( "DeclMode" ), DECL_MODE_SIMPLE );

	testfactor::RFactor soConfig128 = soBufferSize128 * soVertexStride128 * soNumVertexComponents128 * soDeclMode32And64And128;
	testfactor::RFactor soConfig64 = soBufferSize64 * soVertexStride64 * soNumVertexComponents64 * soDeclMode32And64And128;
	testfactor::RFactor soConfig32 = soBufferSize32 * soVertexStride32 * soNumVertexComponents32 * soDeclMode32And64And128;
	testfactor::RFactor soConfig8 = soBufferSize8 * soVertexStride8 * soNumVertexComponents8 * soDeclMode8;

	testfactor::RFactor testFactorNoGSSOConfig = soConfig8 + soConfig32;
	testfactor::RFactor testFactorNoGSCycle = cycleVertexInputSetNoGS + cycleSOOffsetSetNoGS;
	testfactor::XFACTOR pairwiseTableNoGS[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{extraBindAttributes, 1, NULL, 0},
		{testFactorNoGSSOConfig, 1, NULL, 0},
		{testFactorNoGSCycle, 1, NULL, 0},
		{numDrawCalls, 1, NULL, 0},
	};

	size_t groupOrdersNoGS[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};


	testfactor::RFactor rfPairwiseNoGS = NewCrossFactor(pairwiseTableNoGS, _countof(pairwiseTableNoGS), _T("PairwiseNoGS"), groupOrdersNoGS, _countof(groupOrdersNoGS));

	testfactor::RFactor testFactorNoGS = (forceRebindEachDrawFalse
										* useNullGSTrue 
										* rfPairwiseNoGS)
										% slotIndex;
	
	testfactor::RFactor testFactorSOConfig = (soConfig8 + soConfig64 + soConfig128) * gsMaxVertexCount;
	testfactor::RFactor testFactorCycle = cycleVertexInputSet + cycleNumPrimsPerInputVertexSet + cycleSOOffsetSet;
	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{extraBindAttributes, 1, NULL, 0},
		{testFactorSOConfig, 1, NULL, 0},
		{testFactorCycle, 1, NULL, 0},
		{numDrawCalls, 1, NULL, 0},
		{outputPrimitiveType, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 3), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	testfactor::RFactor rfPairwise = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), _T("Pairwise"), groupOrders, _countof(groupOrders));
	testfactor::RFactor testFactor = (forceRebindEachDrawFalse
										* useNullGSFalse 
										* rfPairwise)
										% slotIndex;

	testfactor::RFactor testFactorRebindSOConfig = soConfig8 * gsMaxVertexCountDefault;
	testfactor::RFactor testFactorRebindCycle = cycleSOOffsetSet;
	testfactor::XFACTOR pairwiseTableRebind[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{extraBindAttributes, 1, NULL, 0},
		{testFactorRebindSOConfig, 1, NULL, 0},
		{testFactorRebindCycle, 1, NULL, 0},
		{numDrawCallsDefault, 1, NULL, 0},
		{outputPrimitiveType, 1, NULL, 0},
	};

	testfactor::RFactor rfPairwiseRebind = NewCrossFactor(pairwiseTableRebind, _countof(pairwiseTableRebind), _T("PairwiseRebind"), groupOrders, _countof(groupOrders));
	testfactor::RFactor testFactorForceRebind = (forceRebindEachDrawTrue
										* useNullGSFalse
										* rfPairwiseRebind)
										% slotIndex;
										
	SetRootTestFactor( testFactor + testFactorNoGS + testFactorForceRebind );
	
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumVertexComponents"), (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0) ? 128 : 64 ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumDrawCalls"), 4 ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("OutputPrimitiveType"), _T("Points") ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("ExtraBindAttributes"), _T("All") ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DECL_MODE>(_T("DeclMode"), DECL_MODE_SIMPLE ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DECL_MODE>(_T("DeclMode"), DECL_MODE_PARTIAL_ELEMENTS ), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<DECL_MODE>(_T("DeclMode"), DECL_MODE_SPLIT_ELEMENTS ), WeightFactorSmall);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CStreamOutSingleBuffer::Setup()
{
	TEST_RESULT tr = RESULT_PASS;

	tr = SetupSOClearObjects();

	return tr;
}

void CStreamOutSingleBuffer::Cleanup()
{
	CleanupSOClearObjects();
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CStreamOutSingleBuffer::SetupTestCase()
{
	TEST_RESULT tr = RESULT_PASS;

	assert( m_outputSlot <= 4 );

	if( m_soNumVertexComponents > 64 )
	{
		if( GetDevice()->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
		{
			return RESULT_SKIP;
		}
	}

	//
	// Setup CStreamOutTest::m_soBufferSize and CStreamOutTest::m_soBufferOffset
	//
	for( UINT i = 0; i < ARRAY_SIZE(m_soBufferSize); i++ )
	{
		if( m_outputSlot == i )
		{
			m_soBufferSize[ i ] = m_bufferSize;
			m_soOffsets[ i ] = m_bufferOffset;
		}
		else
		{
			m_soBufferSize[ i ] = 0;
			m_soOffsets[ i ] = 0;
		}
	}

	//
	// Setup Output Streams
	//	
	memset( &m_soBufferDesc, 0, sizeof( m_soBufferDesc ) );
	memset( &m_soBuffers, 0, sizeof( m_soBuffers ) );

	// Single output buffer only
	if( m_extraBindAttributes == tstring( _T( "None" ) ) )
		m_soBufferDesc[m_outputSlot].BindFlags = D3D11_BIND_STREAM_OUTPUT;
	else
	if( m_extraBindAttributes == tstring( _T( "VertexBuffer" ) ) )
		m_soBufferDesc[m_outputSlot].BindFlags = D3D11_BIND_STREAM_OUTPUT|D3D11_BIND_VERTEX_BUFFER;
	else
	if( m_extraBindAttributes == tstring( _T( "IndexBuffer" ) ) )
		m_soBufferDesc[m_outputSlot].BindFlags = D3D11_BIND_STREAM_OUTPUT|D3D11_BIND_INDEX_BUFFER;
	else
	if( m_extraBindAttributes == tstring( _T( "ShaderResource" ) ) )
		m_soBufferDesc[m_outputSlot].BindFlags = D3D11_BIND_STREAM_OUTPUT|D3D11_BIND_SHADER_RESOURCE;
	else
	if( m_extraBindAttributes == tstring( _T( "RenderTarget" ) ) )
		m_soBufferDesc[m_outputSlot].BindFlags = D3D11_BIND_STREAM_OUTPUT|D3D11_BIND_RENDER_TARGET;
	else
	if( m_extraBindAttributes == tstring( _T( "All" ) ) )
		m_soBufferDesc[m_outputSlot].BindFlags = D3D11_BIND_STREAM_OUTPUT|D3D11_BIND_VERTEX_BUFFER|D3D11_BIND_INDEX_BUFFER|D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
	else
	{
		assert( FormatString( _T( "%s is not a valid bind attribute" ), m_extraBindAttributes.c_str() ) );
		return RESULT_SKIP;
	}
		
	m_soBufferDesc[m_outputSlot].CPUAccessFlags = 0;
	m_soBufferDesc[m_outputSlot].ByteWidth = m_soBufferSize[m_outputSlot]; // Room for 2 1/2 quads
	m_soBufferDesc[m_outputSlot].Usage = D3D11_USAGE_DEFAULT;
	m_soBufferDesc[m_outputSlot].MiscFlags = 0;

	tr = CreateSOBuffers( m_soBuffers, m_soBufferDesc );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Setup Shaders
	//
	std::string vsName;
	std::string gsName;

	std::vector< D3D11_SO_DECLARATION_ENTRY > vSODecl;
	GetCurSODecl( &vSODecl );

	D3D11_SO_DECLARATION_ENTRY* soDecl = &(vSODecl[0]);
	UINT numEntries = vSODecl.size();

	// Setup the correct output slot index
	for( UINT i = 0; i < numEntries; i++ )
	{
		soDecl[i].OutputSlot = (BYTE)m_outputSlot;
	}

	WriteDeclToLog( soDecl, numEntries );

	UINT bufferStrides[4] = { 0, 0, 0, 0 };
	bufferStrides[ m_outputSlot ] = m_soVertexStride;
	
	if( m_useNullGS )
	{
		FromString( &vsName, tstring( _T( "VSNoGS" ) ) + tstring( ToString( m_soNumVertexComponents ) ) );

		tr = SetupVSFromResource( "ShaderSource.sh", vsName.c_str(), soDecl, numEntries, NULL, bufferStrides, 4, &m_pVS, &m_pInputLayout );
	}
	else
	{
		FromString( &vsName, tstring( _T( "VSChangeData" ) ) );
		FromString( &gsName, ComposeGSName() + tstring( _T( "x" ) ) + ToString( m_soNumVertexComponents ) );
		tr = SetupVSFromResource( "ShaderSource.sh", vsName.c_str(), NULL, 0, NULL, NULL, 0, &m_pVS, &m_pInputLayout );
	}
	
	if( tr != RESULT_PASS )
		return tr;
	
	if( !m_useNullGS )
	{
		tr = SetupGSFromResource( "ShaderSource.sh", gsName.c_str(), soDecl, numEntries, NULL, bufferStrides, 4, false, &m_pGS );
		if( tr != RESULT_PASS )
			return tr;
	}

	UINT numVerts;
	void *pVertexData = NULL;
	
	if( !m_useNullGS )
	{
		numVerts = s_pointsSize;
		pVertexData = s_vbdataPoints;
	}
	else
	{
		switch( m_inputTopology )
		{
		case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
			numVerts = s_pointsSize;
			pVertexData = s_vbdataPoints;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
			numVerts = s_lineListSize;
			pVertexData = s_vbdataLineList;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			numVerts = s_triangleListSize;
			pVertexData = s_vbdataTriangleList;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
			numVerts = s_lineStripSize;
			pVertexData = s_vbdataLineStrip;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
			numVerts = s_triangleStripSize;
			pVertexData = s_vbdataTriangleStrip;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
			numVerts = s_lineListAdjSize;
			pVertexData = s_vbdataLineListAdj;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
			numVerts = s_triangleListAdjSize;
			pVertexData = s_vbdataTriangleListAdj;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
			numVerts = s_lineStripAdjSize;
			pVertexData = s_vbdataLineStripAdj;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
			numVerts = s_triangleStripAdjSize;
			pVertexData = s_vbdataTriangleStripAdj;
			break;
		}
	}

	//
	// Setup vertex buffer
	//
	tr = SetupVertexBuffer( numVerts, pVertexData );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Create constant buffer
	//
	const D3D11_BUFFER_DESC cbdesc =
	{
		1 * sizeof( CONSTDATA ),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0
	};

	HRESULT hr = GetDevice()->CreateBuffer( &cbdesc, NULL, &m_pCB );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateBuffer() unexpectedly failed creating constant buffer - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Setup rasterizer and depth/stencil state
	//
	tr = SetupRasterizer();
	if( tr != RESULT_PASS )
		return tr;
	

	//
	// Setup SO statistics query
	//
	tr = SetupQueryObjects();
	if( tr != RESULT_PASS )
		return tr;

	return tr;
}

void CStreamOutSingleBuffer::GetCurSODecl( std::vector< D3D11_SO_DECLARATION_ENTRY > *pSODecl )
{
	D3D11_SO_DECLARATION_ENTRY const* soDecl = NULL;
	UINT numEntries = 0;

	switch( m_soNumVertexComponents )
	{
	default:
	case 8:
		soDecl = s_soDecl8;
		numEntries = sizeof( s_soDecl8 ) / sizeof( s_soDecl8[0] );
		break;
		
	case 32:
		switch( m_declMode )
		{
		case DECL_MODE_SIMPLE:
			soDecl = s_soDecl32;
			numEntries = sizeof( s_soDecl32 ) / sizeof( s_soDecl32[0] );
			break;
		case DECL_MODE_GAPS:
			soDecl = s_soDecl32Gaps;
			numEntries = sizeof( s_soDecl32Gaps ) / sizeof( s_soDecl32Gaps[0] );
			break;
		case DECL_MODE_SPLIT_ELEMENTS:
			soDecl = s_soDecl32Split;
			numEntries = sizeof( s_soDecl32Split ) / sizeof( s_soDecl32Split[0] );
			break;
		}
		break;

	case 64:
		switch( m_declMode )
		{
		case DECL_MODE_SIMPLE:
			soDecl = s_soDecl64;
			numEntries = sizeof( s_soDecl64 ) / sizeof( s_soDecl64[0] );
			break;
		case DECL_MODE_GAPS:
			soDecl = s_soDecl64Gaps;
			numEntries = sizeof( s_soDecl64Gaps ) / sizeof( s_soDecl64Gaps[0] );
			break;
		case DECL_MODE_SPLIT_ELEMENTS:
			soDecl = s_soDecl64Split;
			numEntries = sizeof( s_soDecl64Split ) / sizeof( s_soDecl64Split[0] );
			break;
		}
		break;
	case 128:
		switch( m_declMode )
		{
		case DECL_MODE_SIMPLE:
			soDecl = s_soDecl128;
			numEntries = sizeof( s_soDecl128 ) / sizeof( s_soDecl128[0] );
			break;
		case DECL_MODE_GAPS:
			soDecl = s_soDecl128Gaps;
			numEntries = sizeof( s_soDecl128Gaps ) / sizeof( s_soDecl128Gaps[0] );
			break;
		case DECL_MODE_SPLIT_ELEMENTS:
			soDecl = s_soDecl128Split;
			numEntries = sizeof( s_soDecl128Split ) / sizeof( s_soDecl128Split[0] );
			break;
		}
		break;
	}

	*pSODecl = std::vector< D3D11_SO_DECLARATION_ENTRY >( soDecl, soDecl + numEntries );
}

void CStreamOutSingleBuffer::CleanupTestCase()
{
	SAFE_RELEASE( m_pSOStatsQuery );
	SAFE_RELEASE( m_pSOOverflowPredicate );
	SAFE_RELEASE( m_pPipelineStatsQuery );
	SAFE_RELEASE( m_pDSDisable );
	SAFE_RELEASE( m_pRS );
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pCB );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_soBuffers[0] );
	SAFE_RELEASE( m_soBuffers[1] );
	SAFE_RELEASE( m_soBuffers[2] );
	SAFE_RELEASE( m_soBuffers[3] );
}


//TEST_RESULT CStreamOutSingleBuffer::CompareSOVertexFromGS( GSOUT_4 *refVert, GSOUT_4 *srcVert, UINT numDataElements, UINT drawIndex, UINT leadingVertID, UINT primID, UINT primVertID, void *pBeginBuffer, void *pEndBuffer, UINT componentCount )
//{
//	GSOUT gsoutTemp;
//	TEST_RESULT tr = RESULT_PASS;
//	UINT *whitespace;
//	size_t soOffset = ptrdiff_t( srcVert ) - ptrdiff_t( pBeginBuffer );
//
//	for( UINT dataElement = 0; dataElement < numDataElements; ++dataElement )
//	{
//	if( memcmp( srcVert->data, refVert->data, componentCount * sizeof( UINT ) ) != 0 )
//	{
//		WriteToLog( _T( "Output stream vertex mismatch at offset %i, draw index %i, vertex id %i, primitive id %i, primitive vertex %i" ), soOffset, drawIndex, leadingVertID, primID, primVertID );
//		tr = RESULT_FAIL;
//	}
//
//	// Look for MAGIC_NUMBER pattern in 'whitespace' between SO vertices
//	UCHAR *ws = (UCHAR *) srcVert;
//	UINT offset = sizeof( srcVert[0].pos ) + numDataElements * sizeof( srcVert[0].data );
//	UINT suboffset = offset & 3;
//	const UCHAR deadbeef[] = { 0xef, 0xbe, 0xad, 0xde };
//	for( ; offset < m_soVertexStride; ++offset, suboffset = 3 & ( suboffset + 1 ) )
//	{
//		if( ws[offset] != deadbeef[suboffset] )
//		{
//			WriteToLog( _T( "SO buffer data corruption in 'whitespace' between stream-out vertices at draw index %i, vertex id %i, primitive id %i, primitive vertex %i" ), drawIndex, leadingVertID, primID, primVertID );
//			tr = RESULT_FAIL;
//			break;
//		}
//	}
//
//	return tr;
//}

TEST_RESULT CStreamOutSingleBuffer::CompareSOVertex( GSOUT *refVert, GSOUT *srcVert, UINT numDataElements, UINT drawIndex, UINT leadingVertID, UINT primID, UINT primVertID, void *pBeginBuffer, void *pEndBuffer, UINT componentCount )
{
	GSOUT gsoutTemp;
	TEST_RESULT tr = RESULT_PASS;
	UINT *whitespace;
	size_t soOffset = ptrdiff_t( srcVert ) - ptrdiff_t( pBeginBuffer );

	for( UINT dataElement = 0; dataElement < numDataElements; ++dataElement )
	{
		const unsigned int offset = dataElement * componentCount;
		if( memcmp( ((UINT*)srcVert->data) + offset, ((UINT*)refVert->data) + offset, componentCount * sizeof( UINT ) ) != 0 )
		{
			WriteToLog( _T( "Output stream vertex mismatch at offset %i, draw index %i, vertex id %i, primitive id %i, primitive vertex %i" ), soOffset, drawIndex, leadingVertID, primID, primVertID );
			tr = RESULT_FAIL;
			break;
		}
	}

	// Look for MAGIC_NUMBER pattern in 'whitespace' between SO vertices
	UCHAR *ws = (UCHAR *) srcVert;
	UINT offset = sizeof( srcVert[0].pos ) + numDataElements * componentCount * sizeof( UINT );
	UINT suboffset = offset & 3;
	union
	{
		int lmagic_number;
		UCHAR magic_number[4];
	};

	lmagic_number = MAGIC_NUMBER;

	for( ; offset < m_soVertexStride; ++offset, suboffset = 3 & ( suboffset + 1 ) )
	{
		if( ws[offset] != magic_number[suboffset] )
		{
			WriteToLog( _T( "SO buffer data corruption in 'whitespace' between stream-out vertices at draw index %i, vertex id %i, primitive id %i, primitive vertex %i" ), drawIndex, leadingVertID, primID, primVertID );
			tr = RESULT_FAIL;
			break;
		}
	}

	return tr;
}

TEST_RESULT CStreamOutSingleBuffer::ExecuteTestCase()
{
	const UINT appendOffsets[4] = { -1, -1, -1, -1 };
	const UINT nullOffsets[4] = { 0, 0, 0, 0 };
	ID3D11Buffer *nullBuffers[4] = { NULL, NULL, NULL, NULL };
	
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;

	//
	// Clear streamout buffer
	//	
	tr = ClearSOBuffer( m_soBuffers[m_outputSlot], m_soBufferSize[m_outputSlot] );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Setup pipeline state
	//
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );
	ID3D11Buffer *vbuffers[] = { m_pVB };
	UINT strides[] = { sizeof( VSIN ) };
	UINT offsets[] = { 0 };
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, vbuffers, strides, offsets );
	GetEffectiveContext()->IASetPrimitiveTopology( m_inputTopology );
	GetEffectiveContext()->IASetIndexBuffer( NULL, DXGI_FORMAT_UNKNOWN, 0 );

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	ID3D11Buffer *cbuffers[] = { m_pCB };
	GetEffectiveContext()->GSSetConstantBuffers( 0, 1, cbuffers );
	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, cbuffers );
	GetEffectiveContext()->RSSetState( m_pRS );

	// Disable pixel pipeline
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 ); 
	GetEffectiveContext()->OMSetDepthStencilState( m_pDSDisable, 0 );

	// Setup stream out
	GetEffectiveContext()->SOSetTargets( 4, m_soBuffers, m_soOffsets );


	//
	// Draw
	//
	GetEffectiveContext()->Begin( m_pSOStatsQuery );
	GetEffectiveContext()->Begin( m_pSOOverflowPredicate );
	GetEffectiveContext()->Begin( m_pPipelineStatsQuery );

	UINT gsMaxPrimitiveCount;
	
	UINT numExpectedPrimitives = 0;
	UINT vertsPerPrimitive;
	
	UINT maxPossiblePrimitivesPerDraw;
	UINT maxPossiblePrimitives;
	UINT bufferSizeRemaining = m_soBufferSize[m_outputSlot] - m_soOffsets[m_outputSlot];
	bool limitedByStreamoutSize = false;
	UINT gsInvocationsExpected = m_numInputVerts * m_numDrawCalls;

	// Starting values for our expected vsInvocations from pipeline stats
	// These are updated in the switch statement below where necessary, depending on the topology
	UINT vsInvocationsExpectedMin = gsInvocationsExpected;
	UINT vsInvocationsExpectedMax = gsInvocationsExpected;

	if( m_useNullGS )
	{
		switch( m_inputTopology )
		{
			case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
				vertsPerPrimitive = 1;
				maxPossiblePrimitivesPerDraw = m_numInputVerts / 1;
				break;
			case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
				vertsPerPrimitive = 2;
				maxPossiblePrimitivesPerDraw = m_numInputVerts / 2;
				break;
			case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
				vertsPerPrimitive = 2;
				maxPossiblePrimitivesPerDraw = m_numInputVerts >= 2 ? m_numInputVerts - 1 : 0;
				vsInvocationsExpectedMax = maxPossiblePrimitivesPerDraw * 2 * m_numDrawCalls;
				break;
			case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
				vertsPerPrimitive = 3;
				maxPossiblePrimitivesPerDraw = m_numInputVerts / 3;
				break;
			case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
				vertsPerPrimitive = 3;
				maxPossiblePrimitivesPerDraw = m_numInputVerts >= vertsPerPrimitive ? m_numInputVerts - 2 : 0;
				vsInvocationsExpectedMax = maxPossiblePrimitivesPerDraw * 3 * m_numDrawCalls;
				break;
			case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
				vertsPerPrimitive = 2;
				maxPossiblePrimitivesPerDraw = m_numInputVerts / 4;
				vsInvocationsExpectedMin = m_numInputVerts / 2 * m_numDrawCalls;
				break;
			case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
				vertsPerPrimitive = 2;
				maxPossiblePrimitivesPerDraw = m_numInputVerts >= 4 ? m_numInputVerts - 3 : 0;
				vsInvocationsExpectedMin = (m_numInputVerts - 2) * m_numDrawCalls;
				vsInvocationsExpectedMax = maxPossiblePrimitivesPerDraw * 2 * m_numDrawCalls;
				break;
			case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
				vertsPerPrimitive = 3;
				maxPossiblePrimitivesPerDraw = m_numInputVerts > 6 ? m_numInputVerts / 6 : 0;
				vsInvocationsExpectedMin = m_numInputVerts / 2;
				break;
			case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
				vertsPerPrimitive = 3;
				maxPossiblePrimitivesPerDraw = m_numInputVerts > 6 ? ( m_numInputVerts / 2 ) - 2 : 0;
				vsInvocationsExpectedMin = m_numInputVerts / 2;
				vsInvocationsExpectedMax = maxPossiblePrimitivesPerDraw * 3 * m_numDrawCalls;
				break;
		}
		
		gsMaxPrimitiveCount = 1;
		maxPossiblePrimitives = maxPossiblePrimitivesPerDraw * m_numDrawCalls;
	}
	else
	{
		if( m_gsOutputPrimitiveType == tstring( "Triangles" ) )
		{
			gsMaxPrimitiveCount = m_gsMaxVertexCount - 2;
			vertsPerPrimitive = 3;
		}
		else
		if( m_gsOutputPrimitiveType == tstring( "Lines" ) )
		{
			gsMaxPrimitiveCount = m_gsMaxVertexCount - 1;
			vertsPerPrimitive = 2;
		}
		else
		if( m_gsOutputPrimitiveType == tstring( "Points" ) )
		{
			gsMaxPrimitiveCount = m_gsMaxVertexCount;
			vertsPerPrimitive = 1;
		}
		
		maxPossiblePrimitivesPerDraw = min( gsMaxPrimitiveCount, m_numPrimsPerInputVertex ) * m_numInputVerts;
		maxPossiblePrimitives = maxPossiblePrimitivesPerDraw * m_numDrawCalls;
	}
	
	UINT primitiveStride = m_soVertexStride * vertsPerPrimitive;
		
	for( UINT i = 0; i < m_numDrawCalls; ++i )
	{
		CONSTDATA *pConstData;
		D3D11_MAPPED_SUBRESOURCE mappedRes;

		// Setup constant data for draw call
		hr = GetEffectiveContext()->Map( m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "Map() unexpectedly failed mapping dynamic constant buffer - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			return RESULT_FAIL;
		}
		pConstData = (CONSTDATA*) mappedRes.pData;

		pConstData->numPrimsPerInputVertex[0] = m_numPrimsPerInputVertex;
		pConstData->drawIndex[0] = i;
		GetEffectiveContext()->Unmap( m_pCB, 0 );

		if( m_forceRebindEachDraw )
		{
			// Force restart of targets
			GetEffectiveContext()->SOSetTargets( 4, nullBuffers, nullOffsets );
			GetEffectiveContext()->SOSetTargets( 4, m_soBuffers, appendOffsets );	
		}
			
		// Draw
		GetEffectiveContext()->Draw( m_numInputVerts, 0 );
		

		UINT maxPossiblePrimitivesThisDraw = bufferSizeRemaining / primitiveStride;
		UINT numExpectedPrimitivesThisDraw = min( maxPossiblePrimitivesPerDraw, maxPossiblePrimitivesThisDraw );
		UINT bufferSizeUsed = numExpectedPrimitivesThisDraw * primitiveStride;
		numExpectedPrimitives += numExpectedPrimitivesThisDraw;
		bufferSizeRemaining -= bufferSizeUsed;
		assert( bufferSizeRemaining <= m_soBufferSize[m_outputSlot] );

		if ( maxPossiblePrimitivesThisDraw < maxPossiblePrimitivesPerDraw )
		{
			limitedByStreamoutSize = true;
		}
	}
	GetEffectiveContext()->End( m_pSOOverflowPredicate );
	GetEffectiveContext()->End( m_pSOStatsQuery );
	GetEffectiveContext()->End( m_pPipelineStatsQuery );


	// execute any deferred operations
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext failed. hr = %s", D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	//
	// Verify the expected number of output primitives
	//
	size_t size;
	
	//
	// Check pipeline statistics
	//
	size = m_pPipelineStatsQuery->GetDataSize();
	if( size != sizeof( D3D11_QUERY_DATA_PIPELINE_STATISTICS ) )
	{
		WriteToLog( _T( "GetDataSize() return value incorrect - expected: %i, actual %i" ), sizeof( D3D11_QUERY_DATA_PIPELINE_STATISTICS ), size );
		return RESULT_FAIL;
	}
	
	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
	if (GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pPipelineStatsQuery, &pipelineStats, size, 0 ) != S_OK)
	{
		WriteToLog( _T( "GetData() failed or timed out." ) );
		return RESULT_FAIL;
	}
	
	if ( maxPossiblePrimitives == 0 )
	{
		gsInvocationsExpected = 0;
		vsInvocationsExpectedMin = 0;
	}

	if( !m_useNullGS )
	{
		if( pipelineStats.GSInvocations != gsInvocationsExpected )
		{
			WriteToLog( _T( "pipelineStats.GSInvocations is invalid - expected %i, actual %i" ), gsInvocationsExpected, pipelineStats.GSInvocations );
			tr = RESULT_FAIL;
		}
		
		// Note: The number of the GS primitives generated isn't limited by the stream out buffer size
		if( pipelineStats.GSPrimitives != numExpectedPrimitives && !(limitedByStreamoutSize && pipelineStats.GSPrimitives > numExpectedPrimitives ) )
		{
			WriteToLog( _T( "pipelineStats.GSPrimitives is invalid - expected %i, actual %i" ), numExpectedPrimitives, pipelineStats.GSPrimitives );
			tr = RESULT_FAIL;
		}
	}
	
	if( pipelineStats.VSInvocations < vsInvocationsExpectedMin || pipelineStats.VSInvocations > vsInvocationsExpectedMax )
	{
		if ( vsInvocationsExpectedMax != vsInvocationsExpectedMin )
		{
			WriteToLog( _T( "pipelineStats.VSInvocations is invalid - expected %i - %i, actual %i" ), vsInvocationsExpectedMin, vsInvocationsExpectedMax, pipelineStats.VSInvocations );
		}
		else
		{
			WriteToLog( _T( "pipelineStats.VSInvocations is invalid - expected %i, actual %i" ), vsInvocationsExpectedMax, pipelineStats.VSInvocations );
		}
		tr = RESULT_FAIL;
	}
	
	//
	// Check stream out statistics
	//
	size = m_pSOStatsQuery->GetDataSize();
	if( size != sizeof( D3D11_QUERY_DATA_SO_STATISTICS ) )
	{
		WriteToLog( _T( "GetDataSize() return value incorrect - expected: %i, actual %i" ), sizeof( D3D11_QUERY_DATA_SO_STATISTICS ), size );
		return RESULT_FAIL;
	}

	D3D11_QUERY_DATA_SO_STATISTICS soStats;
	if (GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pSOStatsQuery, &soStats, size, 0 ) != S_OK)
	{
		WriteToLog( _T( "GetData() failed or timed out." ) );
		return RESULT_FAIL;
	}

	//
	// Check overflow status
	//
	BOOL overflow;
	BOOL expectOverflow = maxPossiblePrimitives != numExpectedPrimitives;
	if (GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pSOOverflowPredicate, &overflow, sizeof( BOOL ), 0 ) != S_OK)
	{
		WriteToLog( _T( "GetData() failed or timed out." ) );
		return RESULT_FAIL;
	}

	//
	// Look at the output stream result
	//

	if( maxPossiblePrimitives != soStats.PrimitivesStorageNeeded )
	{
		WriteToLog( _T( "D3D11_QUERY_DATA_SO_STATISTICS.PrimitivesStorageNeeded incorrect - expected: %i, actual %i" ), maxPossiblePrimitives, soStats.PrimitivesStorageNeeded );
		tr = RESULT_FAIL;
	}

	if( numExpectedPrimitives != soStats.NumPrimitivesWritten )
	{
		WriteToLog( _T( "D3D11_QUERY_DATA_SO_STATISTICS.NumPrimitivesWritten incorrect - expected: %i, actual %i" ), numExpectedPrimitives, soStats.NumPrimitivesWritten );
		tr = RESULT_FAIL;
	}

	if( expectOverflow && !overflow )
	{
		WriteToLog( _T( "Stream out overflow predicate failed to indicate expected buffer overflow" ) );
		tr = RESULT_FAIL;
	}

	if( overflow && !expectOverflow )
	{
		WriteToLog( _T( "Stream out overflow predicate incorrectly indicated buffer overflow" ) );
		tr = RESULT_FAIL;
	}

	D3D11_BUFFER_DESC tempdesc;
	tempdesc.ByteWidth = m_soBufferDesc[m_outputSlot].ByteWidth;
	tempdesc.BindFlags = 0;
	tempdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	tempdesc.MiscFlags = 0;
	tempdesc.Usage = D3D11_USAGE_STAGING;

	ID3D11Buffer *pTempBuffer = NULL;
	hr = GetDevice()->CreateBuffer( &tempdesc, NULL, &pTempBuffer );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateBuffer() unexpectedly failed creating temporary buffer - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	GetImmediateContext()->CopyResource( pTempBuffer, m_soBuffers[m_outputSlot] );

	char *pSrcSOVerts;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	hr = GetImmediateContext()->Map( pTempBuffer, 0, D3D11_MAP_READ, 0, &mappedRes );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "ID3D11Buffer::Map() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	pSrcSOVerts = (char*) mappedRes.pData;

	// Examine output data
	VSIN *pVertexIn = NULL;
	
	GSOUT *pGSOutData;
	pGSOutData = (GSOUT *) ( pSrcSOVerts + m_soOffsets[m_outputSlot] );

	if( !m_useNullGS )
	{
		pVertexIn = s_vbdataPoints;
	}
	else
	{
		switch( m_inputTopology )
		{
		case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
			pVertexIn = s_vbdataPoints;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
			pVertexIn = s_vbdataLineList;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			pVertexIn = s_vbdataTriangleList;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
			pVertexIn = s_vbdataLineStrip;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
			pVertexIn = s_vbdataTriangleStrip;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
			pVertexIn = s_vbdataLineListAdj;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
			pVertexIn = s_vbdataTriangleListAdj;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
			pVertexIn = s_vbdataLineStripAdj;
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
			pVertexIn = s_vbdataTriangleStripAdj;
			break;
		}
	}

	UINT numDataElements = 0;
	UINT componentCount = 2;
	switch( m_soNumVertexComponents )
	{
	case 8:
		numDataElements = 2;
		break;
		
	case 32:
		numDataElements = 14;
		break;

	case 64:
		numDataElements = 30;
		break;
	case 128:
		numDataElements = 31;
		componentCount = 4;
		break;
	}

	UINT dataErrorCount = 0;

	// Verify the data in the so buffer is untouched where
	// "whitespace" is expected via offset, stride, decl gaps.
	for( UINT i = 0; i < m_soOffsets[m_outputSlot] / 4; ++i )
	{
		UINT whitespace = ( (UINT *) pSrcSOVerts )[i];
		if( whitespace != MAGIC_NUMBER )
		{
			WriteToLog( "SO buffer corruption within offset space" );
			tr = RESULT_FAIL;
			break;
		}
	}

	if ( tr != RESULT_PASS )
		goto endMethod;

	if( m_useNullGS )
	{
		switch( m_inputTopology )
		{
		case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
			tr = TestPointListNoGS( maxPossiblePrimitivesPerDraw, pVertexIn, pGSOutData, pSrcSOVerts, numDataElements );
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
			tr = TestLineListNoGS( maxPossiblePrimitivesPerDraw, pVertexIn, pGSOutData, pSrcSOVerts, numDataElements );
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			tr = TestTriangleListNoGS( maxPossiblePrimitivesPerDraw, pVertexIn, pGSOutData, pSrcSOVerts, numDataElements );
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
			tr = TestLineStripNoGS( maxPossiblePrimitivesPerDraw, pVertexIn, pGSOutData, pSrcSOVerts, numDataElements );
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
			tr = TestTriangleStripNoGS( maxPossiblePrimitivesPerDraw, pVertexIn, pGSOutData, pSrcSOVerts, numDataElements );
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
			tr = TestLineListAdjNoGS( maxPossiblePrimitivesPerDraw, pVertexIn, pGSOutData, pSrcSOVerts, numDataElements );
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
			tr = TestTriangleListAdjNoGS( maxPossiblePrimitivesPerDraw, pVertexIn, pGSOutData, pSrcSOVerts, numDataElements );
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
			tr = TestLineStripAdjNoGS( maxPossiblePrimitivesPerDraw, pVertexIn, pGSOutData, pSrcSOVerts, numDataElements );
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
			tr = TestTriangleStripAdjNoGS( maxPossiblePrimitivesPerDraw, pVertexIn, pGSOutData, pSrcSOVerts, numDataElements );
			break;
		}
	}
	else
	{
		switch( vertsPerPrimitive )
		{
		case 1:
			tr = TestPointList( numExpectedPrimitives, pVertexIn, pGSOutData, pSrcSOVerts, gsMaxPrimitiveCount, numDataElements, componentCount );
			break;
		case 2:
			tr = TestLineList( numExpectedPrimitives, pVertexIn, pGSOutData, pSrcSOVerts, gsMaxPrimitiveCount, numDataElements, componentCount );
			break;
		case 3:
			tr = TestTriangleList( numExpectedPrimitives, pVertexIn, pGSOutData, pSrcSOVerts, gsMaxPrimitiveCount, numDataElements, componentCount );
			break;
		}
	}

endMethod:
	GetImmediateContext()->Unmap(pTempBuffer,0);
	pTempBuffer->Release();

	return tr;
}


TEST_RESULT CStreamOutSingleBuffer::TestTriangleList( UINT numExpectedPrimitives, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT gsMaxPrimitiveCount, UINT numDataElements, UINT componentCount )
{
	TEST_RESULT tr = RESULT_PASS;
	UINT primCount = numExpectedPrimitives;
	GSOUT *pGSOut = pGSOutData;

	GSOUT *pBeginBuffer = pGSOutData;
	char *pEndBuffer = (char *) pSrcSOVerts + m_soBufferSize[m_outputSlot];
	
	// For each Draw() call
	for( UINT i = 0; i < m_numDrawCalls; i++ )
	{
		// For each GS invocation...
		for( UINT j = 0; j < m_numInputVerts; ++j )
		{
			GSIN gsin;
			gsin.pos[0] = pVertexIn[j].pos[0];
			gsin.pos[1] = pVertexIn[j].pos[1];
			gsin.pos[2] = pVertexIn[j].pos[2];
			gsin.pos[3] = pVertexIn[j].pos[3];
			gsin.data = Encode( pVertexIn[j].data );

			// Predict geometry shader output
			UINT numPrimitivesThisVertex = min( m_numPrimsPerInputVertex, min( gsMaxPrimitiveCount, primCount ) );
			
			// For each triangle
			for( UINT k = 0; k < numPrimitivesThisVertex; )
			{
				TEST_RESULT tempResult;
				UINT value = i + k + gsin.data;
				GSOUT128 refVert0;
				GSOUT128 refVert1;
				GSOUT128 refVert2;
				GSOUT128 refVert3;
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					for( UINT c = 0; c < componentCount; ++c )
					{
						value = Encode( value );
						refVert0.data[n * (componentCount / 2)][c] = gap ? MAGIC_NUMBER : value;
					}
				}
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					for( UINT c = 0; c < componentCount; ++c )
					{
						value = Encode( value );
						refVert1.data[n * (componentCount / 2)][c] = gap ? MAGIC_NUMBER : value;
					}
				}

				value = i + k + gsin.data + 2;

				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					for( UINT c = 0; c < componentCount; ++c )
					{
						value = Encode( value );
						refVert2.data[n * (componentCount / 2)][c] = gap ? MAGIC_NUMBER : value;
					}
				}
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					for( UINT c = 0; c < componentCount; ++c )
					{
						value = Encode( value );
						refVert3.data[n * (componentCount / 2)][c] = gap ? MAGIC_NUMBER : value;
					}
				}

				// Check first triangle in quad
				tempResult = CompareSOVertex( (GSOUT *) &refVert0, pGSOutData, numDataElements, i, j, k, 0, pBeginBuffer, pEndBuffer, componentCount );
				if( RESULT_PASS != tempResult )
					tr = tempResult;
				pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

				tempResult = CompareSOVertex( (GSOUT *) &refVert1, pGSOutData, numDataElements, i, j, k, 1, pBeginBuffer, pEndBuffer, componentCount );
				if( RESULT_PASS != tempResult )
					tr = tempResult;
				pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

				tempResult = CompareSOVertex( (GSOUT *) &refVert2, pGSOutData, numDataElements, i, j, k, 2, pBeginBuffer, pEndBuffer, componentCount );
				if( RESULT_PASS != tempResult )
					tr = tempResult;
				pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

				++k;

				if( k == numPrimitivesThisVertex )
					break;		

				tempResult = CompareSOVertex( (GSOUT *) &refVert1, pGSOutData, numDataElements, i, j, k, 0, pBeginBuffer, pEndBuffer, componentCount );
				if( RESULT_PASS != tempResult )
					tr = tempResult;
				pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

				tempResult = CompareSOVertex( (GSOUT *) &refVert3, pGSOutData, numDataElements, i, j, k, 1, pBeginBuffer, pEndBuffer, componentCount );
				if( RESULT_PASS != tempResult )
					tr = tempResult;
				pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

				tempResult = CompareSOVertex( (GSOUT *) &refVert2, pGSOutData, numDataElements, i, j, k, 2, pBeginBuffer, pEndBuffer, componentCount );
				if( RESULT_PASS != tempResult )
					tr = tempResult;
				pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

				++k;

				if( k == numPrimitivesThisVertex )
					break;		
			}

			primCount -= numPrimitivesThisVertex;
		}
	}

	assert( primCount == 0 );

	return tr;
}



TEST_RESULT CStreamOutSingleBuffer::TestTriangleListNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements )
{
	TEST_RESULT tr = RESULT_PASS;

	GSOUT *pBeginBuffer = pGSOutData;
	char *pEndBuffer = (char *) pSrcSOVerts + m_soBufferSize[m_outputSlot];
	
	// For each Draw() call
	for( UINT i = 0; i < m_numDrawCalls; i++ )
	{
		UINT primID = 0;
		UINT leadingVertID = 0;
		VSIN *pVert = pVertexIn;
		
		// For each primitive
		for( UINT j = 0; j < numPrimsPerDraw; ++j )
		{
			TEST_RESULT tempResult;
			const UINT vertsPerTriangle = 3;
			
			GSOUT64 refVert[3];

			for( UINT k = 0; k < vertsPerTriangle; ++k )
			{
				GSIN gsin;
				gsin.pos[0] = pVert->pos[0];
				gsin.pos[1] = pVert->pos[1];
				gsin.pos[2] = pVert->pos[2];
				gsin.pos[3] = pVert->pos[3];
				gsin.data = Encode( pVert->data );
				pVert++;

				UINT value = i + gsin.data;
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					value = Encode( value );
					refVert[k].data[n][0] = gap ? MAGIC_NUMBER : value;
					value = Encode( value );
					refVert[k].data[n][1] = gap ? MAGIC_NUMBER : value;
				}
			}

			// Test triangle
			tempResult = CompareSOVertex( (GSOUT *) &refVert[0], pGSOutData, numDataElements, i, leadingVertID, primID, 0, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			tempResult = CompareSOVertex( (GSOUT *) &refVert[1], pGSOutData, numDataElements, i, leadingVertID + 1, primID, 1, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			tempResult = CompareSOVertex( (GSOUT *) &refVert[2], pGSOutData, numDataElements, i, leadingVertID + 2, primID, 2, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );
			leadingVertID += vertsPerTriangle;

			primID++;
			
			// If there is not enough room for another primitive in the buffer then return now
			if( (UINT) ( ptrdiff_t( pEndBuffer ) - ptrdiff_t( pGSOutData ) ) < vertsPerTriangle * m_soVertexStride )
				return tr;
		}
	}

	return tr;
}


TEST_RESULT CStreamOutSingleBuffer::TestTriangleStripNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements )
{
	TEST_RESULT tr = RESULT_PASS;

	GSOUT *pBeginBuffer = pGSOutData;
	char *pEndBuffer = (char *) pSrcSOVerts + m_soBufferSize[m_outputSlot];
	
	// For each Draw() call
	for( UINT i = 0; i < m_numDrawCalls; i++ )
	{
		UINT primID = 0;
		UINT leadingVertID = 0;
		VSIN *pVert = pVertexIn;
		
		// For each primitive
		for( UINT j = 0; j < numPrimsPerDraw; ++j )
		{
			TEST_RESULT tempResult;
			const UINT vertsPerTriangle = 3;
			
			GSOUT64 refVert[3];

			VSIN *pPrimVertex = pVert;
			for( UINT k = 0; k < vertsPerTriangle; ++k )
			{
				GSIN gsin;
				gsin.pos[0] = pVert->pos[0];
				gsin.pos[1] = pVert->pos[1];
				gsin.pos[2] = pVert->pos[2];
				gsin.pos[3] = pVert->pos[3];
				gsin.data = Encode( pVert->data );
				pVert++;

				UINT value = i + gsin.data;
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					value = Encode( value );
					refVert[k].data[n][0] = gap ? MAGIC_NUMBER : value;
					value = Encode( value );
					refVert[k].data[n][1] = gap ? MAGIC_NUMBER : value;
				}
			}
			pVert = pPrimVertex;
			pVert++;

			// Determine if this is an even or odd triangle
			int odd = leadingVertID & 1;
			
			// Test triangle
			tempResult = CompareSOVertex( (GSOUT *) &refVert[0], pGSOutData, numDataElements, i, leadingVertID, primID, 0, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			tempResult = CompareSOVertex( (GSOUT *) &refVert[1 + odd], pGSOutData, numDataElements, i, leadingVertID + 1 + odd, primID, 1, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			tempResult = CompareSOVertex( (GSOUT *) &refVert[2 - odd], pGSOutData, numDataElements, i, leadingVertID + 2 - odd, primID, 2, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			leadingVertID += 1;

			primID++;
			
			// If there is not enough room for another primitive in the buffer then return now
			if( (UINT) ( ptrdiff_t( pEndBuffer ) - ptrdiff_t( pGSOutData ) ) < vertsPerTriangle * m_soVertexStride )
				return tr;
		}
	}

	return tr;
}


TEST_RESULT CStreamOutSingleBuffer::TestTriangleStripAdjNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements )
{
	TEST_RESULT tr = RESULT_PASS;

	GSOUT *pBeginBuffer = pGSOutData;
	char *pEndBuffer = (char *) pSrcSOVerts + m_soBufferSize[m_outputSlot];
	
	// For each Draw() call
	for( UINT i = 0; i < m_numDrawCalls; i++ )
	{
		UINT primID = 0;
		UINT leadingVertID = 0;
		VSIN *pVert = pVertexIn;
		
		// For each primitive
		for( UINT j = 0; j < numPrimsPerDraw; ++j )
		{
			TEST_RESULT tempResult;
			const UINT vertsPerTriangle = 3;
			
			GSOUT64 refVert[3];

			VSIN *pPrimVertex = pVert;
			for( UINT k = 0; k < vertsPerTriangle; ++k )
			{
				GSIN gsin;
				gsin.pos[0] = pVert->pos[0];
				gsin.pos[1] = pVert->pos[1];
				gsin.pos[2] = pVert->pos[2];
				gsin.pos[3] = pVert->pos[3];
				gsin.data = Encode( pVert->data );
				pVert++;
				pVert++;

				UINT value = i + gsin.data;
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					value = Encode( value );
					refVert[k].data[n][0] = gap ? MAGIC_NUMBER : value;
					value = Encode( value );
					refVert[k].data[n][1] = gap ? MAGIC_NUMBER : value;
				}
			}
			pVert = pPrimVertex;
			pVert++;
			pVert++;

			// Determine if this is an even or odd triangle
			int odd = leadingVertID & 3;
			
			// Test triangle
			tempResult = CompareSOVertex( (GSOUT *) &refVert[0], pGSOutData, numDataElements, i, leadingVertID, primID, 0, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			tempResult = CompareSOVertex( (GSOUT *) &refVert[1 + odd / 2], pGSOutData, numDataElements, i, leadingVertID + 2 + odd, primID, 1, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			tempResult = CompareSOVertex( (GSOUT *) &refVert[2 - odd / 2], pGSOutData, numDataElements, i, leadingVertID + 4 - odd, primID, 2, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			leadingVertID += 2;

			primID++;
			
			// If there is not enough room for another primitive in the buffer then return now
			if( (UINT) ( ptrdiff_t( pEndBuffer ) - ptrdiff_t( pGSOutData ) ) < vertsPerTriangle * m_soVertexStride )
				return tr;
		}
	}

	return tr;
}


TEST_RESULT CStreamOutSingleBuffer::TestTriangleListAdjNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements )
{
	TEST_RESULT tr = RESULT_PASS;

	GSOUT *pBeginBuffer = pGSOutData;
	char *pEndBuffer = (char *) pSrcSOVerts + m_soBufferSize[m_outputSlot];
	
	// For each Draw() call
	for( UINT i = 0; i < m_numDrawCalls; i++ )
	{
		UINT primID = 0;
		UINT leadingVertID = 0;
		VSIN *pVert = pVertexIn;
		
		// For each primitive
		for( UINT j = 0; j < numPrimsPerDraw; ++j )
		{
			TEST_RESULT tempResult;
			const UINT vertsPerTriangle = 3;
			
			GSOUT64 refVert[3];

			for( UINT k = 0; k < vertsPerTriangle; ++k )
			{
				GSIN gsin;
				gsin.pos[0] = pVert->pos[0];
				gsin.pos[1] = pVert->pos[1];
				gsin.pos[2] = pVert->pos[2];
				gsin.pos[3] = pVert->pos[3];
				gsin.data = Encode( pVert->data );
				pVert++;
				pVert++;

				UINT value = i + gsin.data;
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					value = Encode( value );
					refVert[k].data[n][0] = gap ? MAGIC_NUMBER : value;
					value = Encode( value );
					refVert[k].data[n][1] = gap ? MAGIC_NUMBER : value;
				}
			}

			// Test line segment
			tempResult = CompareSOVertex( (GSOUT *) &refVert[0], pGSOutData, numDataElements, i, leadingVertID, primID, 0, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			tempResult = CompareSOVertex( (GSOUT *) &refVert[1], pGSOutData, numDataElements, i, leadingVertID + 1, primID, 1, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			tempResult = CompareSOVertex( (GSOUT *) &refVert[2], pGSOutData, numDataElements, i, leadingVertID + 2, primID, 2, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );
			leadingVertID += vertsPerTriangle;

			primID++;
			
			// If there is not enough room for another primitive in the buffer then return now
			if( (UINT) ( ptrdiff_t( pEndBuffer ) - ptrdiff_t( pGSOutData ) ) < vertsPerTriangle * m_soVertexStride )
				return tr;
		}
	}

	return tr;
}

TEST_RESULT CStreamOutSingleBuffer::TestLineList( UINT numExpectedPrimitives, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT gsMaxPrimitiveCount, UINT numDataElements, UINT componentCount )
{
	TEST_RESULT tr = RESULT_PASS;
	UINT primCount = numExpectedPrimitives;

	GSOUT *pBeginBuffer = pGSOutData;
	char *pEndBuffer = (char *) pSrcSOVerts + m_soBufferSize[m_outputSlot];
	
	// For each Draw() call
	for( UINT i = 0; i < m_numDrawCalls; i++ )
	{
		// For each GS invocation...
		for( UINT j = 0; j < m_numInputVerts; ++j )
		{
			GSIN gsin;
			gsin.pos[0] = pVertexIn[j].pos[0];
			gsin.pos[1] = pVertexIn[j].pos[1];
			gsin.pos[2] = pVertexIn[j].pos[2];
			gsin.pos[3] = pVertexIn[j].pos[3];
			gsin.data = Encode( pVertexIn[j].data );

			// Predict geometry shader output
			UINT numPrimitivesThisVertex = min( m_numPrimsPerInputVertex, min( gsMaxPrimitiveCount, primCount ) );
			
			// For each line
			for( UINT k = 0; k < numPrimitivesThisVertex; )
			{
				TEST_RESULT tempResult;
				UINT value = i + 2 * k + gsin.data;
				GSOUT128 refVert0;
				GSOUT128 refVert1;
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					for( UINT c = 0; c < componentCount; ++c )
					{
						value = Encode( value );
						refVert0.data[n * (componentCount / 2)][c] = gap ? MAGIC_NUMBER : value;
					}
				}
				
				value = 2 + i + 2 * k + gsin.data;
				
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					for( UINT c = 0; c < componentCount; ++c )
					{
						value = Encode( value );
						refVert1.data[n * (componentCount / 2)][c] = gap ? MAGIC_NUMBER : value;
					}
				}

				// Test line segment
				tempResult = CompareSOVertex( (GSOUT *) &refVert0, pGSOutData, numDataElements, i, j, k, 0, pBeginBuffer, pEndBuffer, componentCount );
				if( RESULT_PASS != tempResult )
					tr = tempResult;
				pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

				tempResult = CompareSOVertex( (GSOUT *) &refVert1, pGSOutData, numDataElements, i, j, k, 1, pBeginBuffer, pEndBuffer, componentCount );
				if( RESULT_PASS != tempResult )
					tr = tempResult;
				pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

				++k;

				if( k == numPrimitivesThisVertex )
					break;		
			}

			primCount -= numPrimitivesThisVertex;
		}
	}

	assert( primCount == 0 );

	return tr;
}

TEST_RESULT CStreamOutSingleBuffer::TestLineListNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements )
{
	TEST_RESULT tr = RESULT_PASS;

	GSOUT *pBeginBuffer = pGSOutData;
	char *pEndBuffer = (char *) pSrcSOVerts + m_soBufferSize[m_outputSlot];
	
	// For each Draw() call
	for( UINT i = 0; i < m_numDrawCalls; i++ )
	{
		UINT primID = 0;
		UINT leadingVertID = 0;
		VSIN *pVert = pVertexIn;
		
		// For each primitive
		for( UINT j = 0; j < numPrimsPerDraw; ++j )
		{
			TEST_RESULT tempResult;
			const UINT vertsPerLine = 2;
			
			GSOUT64 refVert[2];

			for( UINT k = 0; k < vertsPerLine; ++k )
			{
				GSIN gsin;
				gsin.pos[0] = pVert->pos[0];
				gsin.pos[1] = pVert->pos[1];
				gsin.pos[2] = pVert->pos[2];
				gsin.pos[3] = pVert->pos[3];
				gsin.data = Encode( pVert->data );
				pVert++;

				UINT value = i + gsin.data;
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					value = Encode( value );
					refVert[k].data[n][0] = gap ? MAGIC_NUMBER : value;
					value = Encode( value );
					refVert[k].data[n][1] = gap ? MAGIC_NUMBER : value;
				}
			}

			// Test line segment
			tempResult = CompareSOVertex( (GSOUT *) &refVert[0], pGSOutData, numDataElements, i, leadingVertID, primID, 0, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			tempResult = CompareSOVertex( (GSOUT *) &refVert[1], pGSOutData, numDataElements, i, leadingVertID + 1, primID, 1, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );
			leadingVertID += vertsPerLine;

			primID++;
			
			// If there is not enough room for another primitive in the buffer then return now
			if( (UINT) ( ptrdiff_t( pEndBuffer ) - ptrdiff_t( pGSOutData ) ) < vertsPerLine * m_soVertexStride )
				return tr;
		}
	}

	return tr;
}

TEST_RESULT CStreamOutSingleBuffer::TestLineListAdjNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements )
{
	TEST_RESULT tr = RESULT_PASS;

	GSOUT *pBeginBuffer = pGSOutData;
	char *pEndBuffer = (char *) pSrcSOVerts + m_soBufferSize[m_outputSlot];
	
	// For each Draw() call
	for( UINT i = 0; i < m_numDrawCalls; i++ )
	{
		VSIN *pVert = pVertexIn;
		UINT primID = 0;
		UINT leadingVertID = 0;
		
		// For each primitive
		for( UINT j = 0; j < numPrimsPerDraw; ++j )
		{
			TEST_RESULT tempResult;
			const UINT vertsPerLine = 2;
			
			GSOUT64 refVert[2];

			// Skip leading adjacent vertex
			pVert++;
			
			for( UINT k = 0; k < vertsPerLine; ++k )
			{
				GSIN gsin;
				gsin.pos[0] = pVert->pos[0];
				gsin.pos[1] = pVert->pos[1];
				gsin.pos[2] = pVert->pos[2];
				gsin.pos[3] = pVert->pos[3];
				gsin.data = Encode( pVert->data );
				pVert++;

				UINT value = i + gsin.data;
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					value = Encode( value );
					refVert[k].data[n][0] = gap ? MAGIC_NUMBER : value;
					value = Encode( value );
					refVert[k].data[n][1] = gap ? MAGIC_NUMBER : value;
				}
			}
			
			// Skip trailing adjacent vertex
			pVert++;
			
			// Test line segment
			tempResult = CompareSOVertex( (GSOUT *) &refVert[0], pGSOutData, numDataElements, i, leadingVertID, primID, 0, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			tempResult = CompareSOVertex( (GSOUT *) &refVert[1], pGSOutData, numDataElements, i, leadingVertID + 1, primID, 1, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );
			leadingVertID += vertsPerLine;

			primID++;
			
			// If there is not enough room for another primitive in the buffer then return now
			if( (UINT) ( ptrdiff_t( pEndBuffer ) - ptrdiff_t( pGSOutData ) ) < vertsPerLine * m_soVertexStride )
				return tr;
		}
	}

	return tr;
}

TEST_RESULT CStreamOutSingleBuffer::TestLineStripNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements )
{
	TEST_RESULT tr = RESULT_PASS;

	GSOUT *pBeginBuffer = pGSOutData;
	char *pEndBuffer = (char *) pSrcSOVerts + m_soBufferSize[m_outputSlot];
	
	// For each Draw() call
	for( UINT i = 0; i < m_numDrawCalls; i++ )
	{
		UINT primID = 0;
		UINT leadingVertID = 0;
		VSIN *pVert = pVertexIn;
		
		// For each primitive
		for( UINT j = 0; j < numPrimsPerDraw; ++j )
		{
			TEST_RESULT tempResult;
			const UINT vertsPerLine = 2;
			
			GSOUT64 refVert[2];

			VSIN *pPrimVertex = pVert;
			for( UINT k = 0; k < vertsPerLine; ++k )
			{
				GSIN gsin;
				gsin.pos[0] = pVert->pos[0];
				gsin.pos[1] = pVert->pos[1];
				gsin.pos[2] = pVert->pos[2];
				gsin.pos[3] = pVert->pos[3];
				gsin.data = Encode( pVert->data );
				pVert++;

				UINT value = i + gsin.data;
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					value = Encode( value );
					refVert[k].data[n][0] = gap ? MAGIC_NUMBER : value;
					value = Encode( value );
					refVert[k].data[n][1] = gap ? MAGIC_NUMBER : value;
				}
			}
			pVert = pPrimVertex;
			pVert++;

			// Test line segment
			tempResult = CompareSOVertex( (GSOUT *) &refVert[0], pGSOutData, numDataElements, i, leadingVertID, primID, 0, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			tempResult = CompareSOVertex( (GSOUT *) &refVert[1], pGSOutData, numDataElements, i, leadingVertID + 1, primID, 1, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );
			leadingVertID += vertsPerLine;

			primID++;
			
			// If there is not enough room for another primitive in the buffer then return now
			if( (UINT) ( ptrdiff_t( pEndBuffer ) - ptrdiff_t( pGSOutData ) ) < vertsPerLine * m_soVertexStride )
				return tr;
		}
	}

	return tr;
}


TEST_RESULT CStreamOutSingleBuffer::TestLineStripAdjNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements )
{
	// Skip first vertex since this is adjacent data which should be ignored
	pVertexIn++;
	
	return TestLineStripNoGS( numPrimsPerDraw, pVertexIn, pGSOutData, pSrcSOVerts, numDataElements );
}

TEST_RESULT CStreamOutSingleBuffer::TestPointList( UINT numExpectedPrimitives, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT gsMaxPrimitiveCount, UINT numDataElements, UINT componentCount )
{
	TEST_RESULT tr = RESULT_PASS;
	UINT primCount = numExpectedPrimitives;

	GSOUT *pBeginBuffer = pGSOutData;
	char *pEndBuffer = (char *) pSrcSOVerts + m_soBufferSize[m_outputSlot];
	
	// For each Draw() call
	for( UINT i = 0; i < m_numDrawCalls; i++ )
	{
		// For each GS invocation...
		for( UINT j = 0; j < m_numInputVerts; ++j )
		{
			GSIN gsin;
			gsin.pos[0] = pVertexIn[j].pos[0];
			gsin.pos[1] = pVertexIn[j].pos[1];
			gsin.pos[2] = pVertexIn[j].pos[2];
			gsin.pos[3] = pVertexIn[j].pos[3];
			gsin.data = Encode( pVertexIn[j].data );

			// Predict geometry shader output
			UINT numPrimitivesThisVertex = min( m_numPrimsPerInputVertex, min( gsMaxPrimitiveCount, primCount ) );
			
			// For each line
			for( UINT k = 0; k < numPrimitivesThisVertex; )
			{
				TEST_RESULT tempResult;
				UINT value = i + 2 * k + gsin.data;
				GSOUT128 refVert0;
				for( UINT n = 0; n < numDataElements; ++n )
				{
					bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
					for( UINT c = 0; c < componentCount; ++c )
					{
						value = Encode( value );
						refVert0.data[n * (componentCount / 2)][c] = gap ? MAGIC_NUMBER : value;
					}
				}

				// Test point
				tempResult = CompareSOVertex( (GSOUT *) &refVert0, pGSOutData, numDataElements, i, j, k, 0, pBeginBuffer, pEndBuffer, componentCount );
				if( RESULT_PASS != tempResult )
					tr = tempResult;
				pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

				++k;

				if( k == numPrimitivesThisVertex )
					break;		
			}

			primCount -= numPrimitivesThisVertex;
		}
	}

	assert( primCount == 0 );

	return tr;
}

TEST_RESULT CStreamOutSingleBuffer::TestPointListNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements )
{
	TEST_RESULT tr = RESULT_PASS;

	GSOUT *pBeginBuffer = pGSOutData;
	char *pEndBuffer = (char *) pSrcSOVerts + m_soBufferSize[m_outputSlot];
	
	// For each Draw() call
	for( UINT i = 0; i < m_numDrawCalls; i++ )
	{
		UINT primID = 0;
		
		// For each GS invocation...
		for( UINT j = 0; j < m_numInputVerts; ++j )
		{
			TEST_RESULT tempResult;
			GSIN gsin;
			gsin.pos[0] = pVertexIn[j].pos[0];
			gsin.pos[1] = pVertexIn[j].pos[1];
			gsin.pos[2] = pVertexIn[j].pos[2];
			gsin.pos[3] = pVertexIn[j].pos[3];
			gsin.data = Encode( pVertexIn[j].data );

			// For each line
			UINT baseValue = i + gsin.data;
			UINT value = baseValue;
			GSOUT64 refVert0;
			for( UINT n = 0; n < numDataElements; ++n )
			{
				bool gap = ( m_declMode == DECL_MODE_GAPS ) && ( ( n % 3 ) == 0 );
				value = Encode( value );
				refVert0.data[n][0] = gap ? MAGIC_NUMBER : value;
				value = Encode( value );
				refVert0.data[n][1] = gap ? MAGIC_NUMBER : value;
			}

			// Test point
			tempResult = CompareSOVertex( (GSOUT *) &refVert0, pGSOutData, numDataElements, i, j, primID, 0, pBeginBuffer, pEndBuffer );
			if( RESULT_PASS != tempResult )
				tr = tempResult;
			pGSOutData = (GSOUT *) ( (char *) pGSOutData + m_soVertexStride );

			primID++;
			
			// If there is not enough room for another primitive in the buffer then return now
			if( (UINT) ( ptrdiff_t( pEndBuffer ) - ptrdiff_t( pGSOutData ) ) < m_soVertexStride )
				return tr;
		}
	}

	return tr;
}

