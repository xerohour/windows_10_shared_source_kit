// ***************************************************************
//  StreamOut   version:  1.0   ·  date: 12/16/2005
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2005 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#include "StreamOut.h"
#include "SingleBuffer.h"
#include "MultiBuffer.h"
#include "MultiStream.h"
#include "StreamOutPatches.h"

BEGIN_NAMED_VALUES( DECL_MODE )
	NAMED_VALUE( _T( "Simple" ), DECL_MODE_SIMPLE )
	NAMED_VALUE( _T( "Gaps" ), DECL_MODE_GAPS )
	NAMED_VALUE( _T( "SplitElements" ), DECL_MODE_SPLIT_ELEMENTS )
	NAMED_VALUE( _T( "ScrambleElements" ), DECL_MODE_SCRAMBLE_ELEMENTS )
	NAMED_VALUE( _T( "PartialElements" ), DECL_MODE_PARTIAL_ELEMENTS )
	NAMED_VALUE( _T( "SkipSlot" ), DECL_MODE_SKIP_SLOT )
	NAMED_VALUE( _T( "SingleSlot" ), DECL_MODE_SINGLE_SLOT )
END_NAMED_VALUES( DECL_MODE )

BEGIN_NAMED_VALUES( SHADER_MODEL )
	NAMED_VALUE( _T( "SM_4_0" ),  SHADER_MODEL_4_0 )
	NAMED_VALUE( _T( "SM_4_1" ),  SHADER_MODEL_4_1 )
	NAMED_VALUE( _T( "SM_5_0" ),  SHADER_MODEL_5_0 )
END_NAMED_VALUES( SHADER_MODEL )

//////////////////////////////////////////////////////////////////////////
// Vertex data table
//////////////////////////////////////////////////////////////////////////
CStreamOutTest::VSIN CStreamOutTest::s_vbdataPoints[] =
{
	{ { 0, 0, 0, 1 }, 100 },
	{ { 0, 1, 0, 1 }, 200 },
	{ { 0, 2, 0, 1 }, 300 },
	{ { 0, 3, 0, 1 }, 400 },
};

UINT CStreamOutTest::s_pointsSize = sizeof( CStreamOutTest::s_vbdataPoints ) / sizeof( CStreamOutTest::s_vbdataPoints[0] );

CStreamOutTest::VSIN CStreamOutTest::s_vbdataLineList[] =
{
	// line 1
	{ { 0, 0, 0, 1 }, 100 },
	{ { 0, 0, 0, 1 }, 101 },
	
	// line 2
	{ { 0, 1, 0, 1}, 200 },
	{ { 0, 1, 0, 1}, 201 },
	
	// line 3
	{ { 0, 2, 0, 1}, 300 },
	{ { 0, 2, 0, 1}, 301 },
	
	// line 4
	{ { 0, 3, 0, 1}, 400 },
	{ { 0, 3, 0, 1}, 401 },
};

UINT CStreamOutTest::s_lineListSize = sizeof( CStreamOutTest::s_vbdataLineList ) / sizeof( CStreamOutTest::s_vbdataLineList[0] );

CStreamOutTest::VSIN CStreamOutTest::s_vbdataLineListAdj[] =
{
	// line 1
	{ { 0, 0, 0, 1 }, 0xdeafca1f },
	{ { 0, 0, 0, 1 }, 100 },
	{ { 0, 0, 0, 1 }, 101 },
	{ { 0, 0, 0, 1 }, 0xdeafca1f },

	// line 2
	{ { 0, 1, 0, 1 }, 0xdeafca1f },
	{ { 0, 1, 0, 1 }, 200 },
	{ { 0, 1, 0, 1 }, 201 },
	{ { 0, 1, 0, 1 }, 0xdeafca1f },

	// line 3
	{ { 0, 2, 0, 1 }, 0xdeafca1f },
	{ { 0, 2, 0, 1 }, 300 },
	{ { 0, 2, 0, 1 }, 301 },
	{ { 0, 2, 0, 1 }, 0xdeafca1f },

	// line 4
	{ { 0, 3, 0, 1 }, 0xdeafca1f },
	{ { 0, 3, 0, 1 }, 400 },
	{ { 0, 3, 0, 1 }, 401 },
	{ { 0, 3, 0, 1 }, 0xdeafca1f },
};

UINT CStreamOutTest::s_lineListAdjSize = sizeof( CStreamOutTest::s_vbdataLineListAdj ) / sizeof( CStreamOutTest::s_vbdataLineListAdj[0] );

CStreamOutTest::VSIN CStreamOutTest::s_vbdataLineStrip[] =
{
	{ { 0, 0, 0, 1 }, 100 },
	{ { 0, 1, 0, 1 }, 110 },
	{ { 0, 2, 0, 1 }, 120 },
	{ { 0, 3, 0, 1 }, 130 },
	{ { 0, 4, 0, 1 }, 140 },
	{ { 0, 5, 0, 1 }, 150 },
	{ { 0, 6, 0, 1 }, 160 },
	{ { 0, 7, 0, 1 }, 170 },
	{ { 0, 8, 0, 1 }, 180 },
	{ { 0, 9, 0, 1 }, 190 },
};

UINT CStreamOutTest::s_lineStripSize = sizeof( CStreamOutTest::s_vbdataLineStrip ) / sizeof( CStreamOutTest::s_vbdataLineStrip[0] );

CStreamOutTest::VSIN CStreamOutTest::s_vbdataLineStripAdj[] =
{
	{ { 0, 0, 0, 1 }, 0xbaadbeef },
	{ { 0, 0, 0, 1 }, 100 },
	{ { 0, 1, 0, 1 }, 110 },
	{ { 0, 2, 0, 1 }, 120 },
	{ { 0, 3, 0, 1 }, 130 },
	{ { 0, 4, 0, 1 }, 140 },
	{ { 0, 5, 0, 1 }, 150 },
	{ { 0, 6, 0, 1 }, 160 },
	{ { 0, 7, 0, 1 }, 170 },
	{ { 0, 8, 0, 1 }, 180 },
	{ { 0, 9, 0, 1 }, 190 },
	{ { 0, 9, 0, 1 }, 0xbaadbeef },
};

UINT CStreamOutTest::s_lineStripAdjSize = sizeof( CStreamOutTest::s_vbdataLineStripAdj ) / sizeof( CStreamOutTest::s_vbdataLineStripAdj[0] );

CStreamOutTest::VSIN CStreamOutTest::s_vbdataTriangleList[] =
{
	// quad 1
	{ { 0, 0, 0, 1 }, 100 },
	{ { 0, 0, 0, 1 }, 101 },
	{ { 0, 0, 0, 1 }, 102 },
	{ { 0, 0, 0, 1 }, 101 },
	{ { 0, 0, 0, 1 }, 103 },
	{ { 0, 0, 0, 1 }, 102 },

	// quad 2
	{ { 0, 1, 0, 1 }, 200 },
	{ { 0, 1, 0, 1 }, 201 },
	{ { 0, 1, 0, 1 }, 202 },
	{ { 0, 1, 0, 1 }, 201 },
	{ { 0, 1, 0, 1 }, 203 },
	{ { 0, 1, 0, 1 }, 202 },

	// quad 3
	{ { 0, 2, 0, 1 }, 300 },
	{ { 0, 2, 0, 1 }, 301 },
	{ { 0, 2, 0, 1 }, 302 },
	{ { 0, 2, 0, 1 }, 301 },
	{ { 0, 2, 0, 1 }, 303 },
	{ { 0, 2, 0, 1 }, 302 },

	// quad 4
	{ { 0, 3, 0, 1 }, 400 },
	{ { 0, 3, 0, 1 }, 401 },
	{ { 0, 3, 0, 1 }, 402 },
	{ { 0, 3, 0, 1 }, 401 },
	{ { 0, 3, 0, 1 }, 403 },
	{ { 0, 3, 0, 1 }, 402 },
};

UINT CStreamOutTest::s_triangleListSize = sizeof( CStreamOutTest::s_vbdataTriangleList ) / sizeof( CStreamOutTest::s_vbdataTriangleList[0] );

CStreamOutTest::VSIN CStreamOutTest::s_vbdataTriangleListAdj[] =
{
	// quad 1
	{ { 0, 0, 0, 1 }, 100 },
	{ { 0, 0, 0, 1 }, 0xdeafca1f },
	{ { 0, 0, 0, 1 }, 101 },
	{ { 0, 0, 0, 1 }, 0xdeafca1f },
	{ { 0, 0, 0, 1 }, 102 },
	{ { 0, 0, 0, 1 }, 0xdeafca1f },
	{ { 0, 0, 0, 1 }, 101 },
	{ { 0, 0, 0, 1 }, 0xdeafca1f },
	{ { 0, 0, 0, 1 }, 103 },
	{ { 0, 0, 0, 1 }, 0xdeafca1f },
	{ { 0, 0, 0, 1 }, 102 },
	{ { 0, 0, 0, 1 }, 0xdeafca1f },

	// quad 2
	{ { 0, 1, 0, 1 }, 200 },
	{ { 0, 1, 0, 1 }, 0xdeafca1f },
	{ { 0, 1, 0, 1 }, 201 },
	{ { 0, 1, 0, 1 }, 0xdeafca1f },
	{ { 0, 1, 0, 1 }, 202 },
	{ { 0, 1, 0, 1 }, 0xdeafca1f },
	{ { 0, 1, 0, 1 }, 201 },
	{ { 0, 1, 0, 1 }, 0xdeafca1f },
	{ { 0, 1, 0, 1 }, 203 },
	{ { 0, 1, 0, 1 }, 0xdeafca1f },
	{ { 0, 1, 0, 1 }, 202 },
	{ { 0, 1, 0, 1 }, 0xdeafca1f },

	// quad 3
	{ { 0, 2, 0, 1 }, 300 },
	{ { 0, 2, 0, 1 }, 0xdeafca1f },
	{ { 0, 2, 0, 1 }, 301 },
	{ { 0, 2, 0, 1 }, 0xdeafca1f },
	{ { 0, 2, 0, 1 }, 302 },
	{ { 0, 2, 0, 1 }, 0xdeafca1f },
	{ { 0, 2, 0, 1 }, 301 },
	{ { 0, 2, 0, 1 }, 0xdeafca1f },
	{ { 0, 2, 0, 1 }, 303 },
	{ { 0, 2, 0, 1 }, 0xdeafca1f },
	{ { 0, 2, 0, 1 }, 302 },
	{ { 0, 2, 0, 1 }, 0xdeafca1f },

	// quad 4
	{ { 0, 3, 0, 1 }, 400 },
	{ { 0, 3, 0, 1 }, 0xdeafca1f },
	{ { 0, 3, 0, 1 }, 401 },
	{ { 0, 3, 0, 1 }, 0xdeafca1f },
	{ { 0, 3, 0, 1 }, 402 },
	{ { 0, 3, 0, 1 }, 0xdeafca1f },
	{ { 0, 3, 0, 1 }, 401 },
	{ { 0, 3, 0, 1 }, 0xdeafca1f },
	{ { 0, 3, 0, 1 }, 403 },
	{ { 0, 3, 0, 1 }, 0xdeafca1f },
	{ { 0, 3, 0, 1 }, 402 },
	{ { 0, 3, 0, 1 }, 0xdeafca1f },
};

UINT CStreamOutTest::s_triangleListAdjSize = sizeof( CStreamOutTest::s_vbdataTriangleListAdj ) / sizeof( CStreamOutTest::s_vbdataTriangleListAdj[0] );

CStreamOutTest::VSIN CStreamOutTest::s_vbdataTriangleStrip[] =
{
	// quad 1
	{ { 0, 0, 0, 1 }, 100 },
	{ { 0, 0, 0, 1 }, 101 },
	{ { 0, 0, 1, 1 }, 112 },
	{ { 0, 0, 1, 1 }, 113 },

	// quad 2
	{ { 0, 0, 2, 1 }, 124 },
	{ { 0, 0, 2, 1 }, 125 },

	// quad 3
	{ { 0, 0, 3, 1 }, 136 },
	{ { 0, 0, 3, 1 }, 137 },

	// quad 4
	{ { 0, 0, 4, 1 }, 148 },
	{ { 0, 0, 4, 1 }, 149 },
};

UINT CStreamOutTest::s_triangleStripSize = sizeof( CStreamOutTest::s_vbdataTriangleStrip ) / sizeof( CStreamOutTest::s_vbdataTriangleStrip[0] );

CStreamOutTest::VSIN CStreamOutTest::s_vbdataTriangleStripAdj[] =
{
	// quad 1
	{ { 0, 0, 0, 1 }, 100 },
	{ { 0, 0, 0, 1 }, 0xdeafca1f },
	{ { 0, 0, 0, 1 }, 101 },
	{ { 0, 0, 0, 1 }, 0xdeafca1f },
	{ { 0, 0, 1, 1 }, 112 },
	{ { 0, 0, 1, 1 }, 0xdeafca1f },
	{ { 0, 0, 1, 1 }, 113 },
	{ { 0, 0, 1, 1 }, 0xdeafca1f },

	// quad 2
	{ { 0, 0, 2, 1 }, 124 },
	{ { 0, 0, 2, 1 }, 0xdeafca1f },
	{ { 0, 0, 2, 1 }, 125 },
	{ { 0, 0, 2, 1 }, 0xdeafca1f },

	// quad 3
	{ { 0, 0, 3, 1 }, 136 },
	{ { 0, 0, 3, 1 }, 0xdeafca1f },
	{ { 0, 0, 3, 1 }, 137 },
	{ { 0, 0, 3, 1 }, 0xdeafca1f },

	// quad 4
	{ { 0, 0, 4, 1 }, 148 },
	{ { 0, 0, 4, 1 }, 0xdeafca1f },
	{ { 0, 0, 4, 1 }, 149 },
	{ { 0, 0, 4, 1 }, 0xdeafca1f },
};

UINT CStreamOutTest::s_triangleStripAdjSize = sizeof( CStreamOutTest::s_vbdataTriangleStripAdj ) / sizeof( CStreamOutTest::s_vbdataTriangleStripAdj[0] );

CStreamOutTest::VSIN CStreamOutTest::s_vbdata32CPPatchList[] =
{
	{ { 0, 0, 0, 1 }, 100 },
	{ { 0, 1, 0, 1 }, 101 },
	{ { 0, 2, 0, 1 }, 102 },
	{ { 0, 3, 0, 1 }, 103 },
	{ { 0, 4, 0, 1 }, 104 },
	{ { 0, 5, 0, 1 }, 105 },
	{ { 0, 6, 0, 1 }, 106 },
	{ { 0, 7, 0, 1 }, 107 },
	{ { 0, 8, 0, 1 }, 108 },
	{ { 0, 9, 0, 1 }, 109 },

	{ { 1, 0, 0, 1 }, 110 },
	{ { 1, 1, 0, 1 }, 111 },
	{ { 1, 2, 0, 1 }, 112 },
	{ { 1, 3, 0, 1 }, 113 },
	{ { 1, 4, 0, 1 }, 114 },
	{ { 1, 5, 0, 1 }, 115 },
	{ { 1, 6, 0, 1 }, 116 },
	{ { 1, 7, 0, 1 }, 117 },
	{ { 1, 8, 0, 1 }, 118 },
	{ { 1, 9, 0, 1 }, 119 },

	{ { 2, 0, 0, 1 }, 120 },
	{ { 2, 1, 0, 1 }, 121 },
	{ { 2, 2, 0, 1 }, 122 },
	{ { 2, 3, 0, 1 }, 123 },
	{ { 2, 4, 0, 1 }, 124 },
	{ { 2, 5, 0, 1 }, 125 },
	{ { 2, 6, 0, 1 }, 126 },
	{ { 2, 7, 0, 1 }, 127 },
	{ { 2, 8, 0, 1 }, 128 },
	{ { 2, 9, 0, 1 }, 129 },

	{ { 3, 0, 0, 1 }, 130 },
	{ { 3, 1, 0, 1 }, 131 },
	{ { 3, 2, 0, 1 }, 132 },
	{ { 3, 3, 0, 1 }, 133 },
	{ { 3, 4, 0, 1 }, 134 },
	{ { 3, 5, 0, 1 }, 135 },
	{ { 3, 6, 0, 1 }, 136 },
	{ { 3, 7, 0, 1 }, 137 },
	{ { 3, 8, 0, 1 }, 138 },
	{ { 3, 9, 0, 1 }, 139 },

	{ { 4, 0, 0, 1 }, 140 },
	{ { 4, 1, 0, 1 }, 141 },
	{ { 4, 2, 0, 1 }, 142 },
	{ { 4, 3, 0, 1 }, 143 },
	{ { 4, 4, 0, 1 }, 144 },
	{ { 4, 5, 0, 1 }, 145 },
	{ { 4, 6, 0, 1 }, 146 },
	{ { 4, 7, 0, 1 }, 147 },
	{ { 4, 8, 0, 1 }, 148 },
	{ { 4, 9, 0, 1 }, 149 },

	{ { 5, 0, 0, 1 }, 150 },
	{ { 5, 1, 0, 1 }, 151 },
	{ { 5, 2, 0, 1 }, 152 },
	{ { 5, 3, 0, 1 }, 153 },
	{ { 5, 4, 0, 1 }, 154 },
	{ { 5, 5, 0, 1 }, 155 },
	{ { 5, 6, 0, 1 }, 156 },
	{ { 5, 7, 0, 1 }, 157 },
	{ { 5, 8, 0, 1 }, 158 },
	{ { 5, 9, 0, 1 }, 159 },

	{ { 6, 0, 0, 1 }, 160 },
	{ { 6, 1, 0, 1 }, 161 },
	{ { 6, 2, 0, 1 }, 162 },
	{ { 6, 3, 0, 1 }, 163 },
	{ { 6, 4, 0, 1 }, 164 },
	{ { 6, 5, 0, 1 }, 165 },
	{ { 6, 6, 0, 1 }, 166 },
	{ { 6, 7, 0, 1 }, 167 },
	{ { 6, 8, 0, 1 }, 168 },
	{ { 6, 9, 0, 1 }, 169 },

	{ { 7, 0, 0, 1 }, 170 },
	{ { 7, 1, 0, 1 }, 171 },
	{ { 7, 2, 0, 1 }, 172 },
	{ { 7, 3, 0, 1 }, 173 },
	{ { 7, 4, 0, 1 }, 174 },
	{ { 7, 5, 0, 1 }, 175 },
	{ { 7, 6, 0, 1 }, 176 },
	{ { 7, 7, 0, 1 }, 177 },
	{ { 7, 8, 0, 1 }, 178 },
	{ { 7, 9, 0, 1 }, 179 },

	{ { 8, 0, 0, 1 }, 180 },
	{ { 8, 1, 0, 1 }, 181 },
	{ { 8, 2, 0, 1 }, 182 },
	{ { 8, 3, 0, 1 }, 183 },
	{ { 8, 4, 0, 1 }, 184 },
	{ { 8, 5, 0, 1 }, 185 },
	{ { 8, 6, 0, 1 }, 186 },
	{ { 8, 7, 0, 1 }, 187 },
	{ { 8, 8, 0, 1 }, 188 },
	{ { 8, 9, 0, 1 }, 189 },

	{ { 9, 0, 0, 1 }, 190 },
	{ { 9, 1, 0, 1 }, 191 },
	{ { 9, 2, 0, 1 }, 192 },
	{ { 9, 3, 0, 1 }, 193 },
	{ { 9, 4, 0, 1 }, 194 },
	{ { 9, 5, 0, 1 }, 195 },
	{ { 9, 6, 0, 1 }, 196 },
	{ { 9, 7, 0, 1 }, 197 },
	{ { 9, 8, 0, 1 }, 198 },
	{ { 9, 9, 0, 1 }, 199 },

	{ { 0, 0, 0, 1 }, 200 },
	{ { 0, 1, 0, 1 }, 201 },
	{ { 0, 2, 0, 1 }, 202 },
	{ { 0, 3, 0, 1 }, 203 },
	{ { 0, 4, 0, 1 }, 204 },
	{ { 0, 5, 0, 1 }, 205 },
	{ { 0, 6, 0, 1 }, 206 },
	{ { 0, 7, 0, 1 }, 207 },
	{ { 0, 8, 0, 1 }, 208 },
	{ { 0, 9, 0, 1 }, 209 },

	{ { 1, 0, 0, 1 }, 210 },
	{ { 1, 1, 0, 1 }, 211 },
	{ { 1, 2, 0, 1 }, 212 },
	{ { 1, 3, 0, 1 }, 213 },
	{ { 1, 4, 0, 1 }, 214 },
	{ { 1, 5, 0, 1 }, 215 },
	{ { 1, 6, 0, 1 }, 216 },
	{ { 1, 7, 0, 1 }, 217 },
	{ { 1, 8, 0, 1 }, 218 },
	{ { 1, 9, 0, 1 }, 219 },

	{ { 2, 0, 0, 1 }, 220 },
	{ { 2, 1, 0, 1 }, 221 },
	{ { 2, 2, 0, 1 }, 222 },
	{ { 2, 3, 0, 1 }, 223 },
	{ { 2, 4, 0, 1 }, 224 },
	{ { 2, 5, 0, 1 }, 225 },
	{ { 2, 6, 0, 1 }, 226 },
	{ { 2, 7, 0, 1 }, 227 },
	{ { 2, 8, 0, 1 }, 228 },
	{ { 2, 9, 0, 1 }, 229 },

	{ { 3, 0, 0, 1 }, 230 },
	{ { 3, 1, 0, 1 }, 231 },
	{ { 3, 2, 0, 1 }, 232 },
	{ { 3, 3, 0, 1 }, 233 },
	{ { 3, 4, 0, 1 }, 234 },
	{ { 3, 5, 0, 1 }, 235 },
	{ { 3, 6, 0, 1 }, 236 },
	{ { 3, 7, 0, 1 }, 237 },
	{ { 3, 8, 0, 1 }, 238 },
	{ { 3, 9, 0, 1 }, 239 },

	{ { 4, 0, 0, 1 }, 240 },
	{ { 4, 1, 0, 1 }, 241 },
	{ { 4, 2, 0, 1 }, 242 },
	{ { 4, 3, 0, 1 }, 243 },
	{ { 4, 4, 0, 1 }, 244 },
	{ { 4, 5, 0, 1 }, 245 },
	{ { 4, 6, 0, 1 }, 246 },
	{ { 4, 7, 0, 1 }, 247 },
	{ { 4, 8, 0, 1 }, 248 },
	{ { 4, 9, 0, 1 }, 249 },

	{ { 5, 0, 0, 1 }, 250 },
	{ { 5, 1, 0, 1 }, 251 },
	{ { 5, 2, 0, 1 }, 252 },
	{ { 5, 3, 0, 1 }, 253 },
	{ { 5, 4, 0, 1 }, 254 },
	{ { 5, 5, 0, 1 }, 255 },
	{ { 5, 6, 0, 1 }, 256 },
	{ { 5, 7, 0, 1 }, 257 },
	{ { 5, 8, 0, 1 }, 258 },
	{ { 5, 9, 0, 1 }, 259 },

	{ { 6, 0, 0, 1 }, 260 },
	{ { 6, 1, 0, 1 }, 261 },
	{ { 6, 2, 0, 1 }, 262 },
	{ { 6, 3, 0, 1 }, 263 },
	{ { 6, 4, 0, 1 }, 264 },
	{ { 6, 5, 0, 1 }, 265 },
	{ { 6, 6, 0, 1 }, 266 },
	{ { 6, 7, 0, 1 }, 267 },
	{ { 6, 8, 0, 1 }, 268 },
	{ { 6, 9, 0, 1 }, 269 },

	{ { 7, 0, 0, 1 }, 270 },
	{ { 7, 1, 0, 1 }, 271 },
	{ { 7, 2, 0, 1 }, 272 },
	{ { 7, 3, 0, 1 }, 273 },
	{ { 7, 4, 0, 1 }, 274 },
	{ { 7, 5, 0, 1 }, 275 },
	{ { 7, 6, 0, 1 }, 276 },
	{ { 7, 7, 0, 1 }, 277 },
	{ { 7, 8, 0, 1 }, 278 },
	{ { 7, 9, 0, 1 }, 279 },

	{ { 8, 0, 0, 1 }, 280 },
	{ { 8, 1, 0, 1 }, 281 },
	{ { 8, 2, 0, 1 }, 282 },
	{ { 8, 3, 0, 1 }, 283 },
	{ { 8, 4, 0, 1 }, 284 },
	{ { 8, 5, 0, 1 }, 285 },
	{ { 8, 6, 0, 1 }, 286 },
	{ { 8, 7, 0, 1 }, 287 },
	{ { 8, 8, 0, 1 }, 288 },
	{ { 8, 9, 0, 1 }, 289 },

	{ { 9, 0, 0, 1 }, 290 },
	{ { 9, 1, 0, 1 }, 291 },
	{ { 9, 2, 0, 1 }, 292 },
	{ { 9, 3, 0, 1 }, 293 },
	{ { 9, 4, 0, 1 }, 294 },
	{ { 9, 5, 0, 1 }, 295 },
	{ { 9, 6, 0, 1 }, 296 },
	{ { 9, 7, 0, 1 }, 297 },
	{ { 9, 8, 0, 1 }, 298 },
	{ { 9, 9, 0, 1 }, 299 },
};

UINT CStreamOutTest::s_32CPPatchListSize = ARRAY_SIZE( CStreamOutTest::s_vbdata32CPPatchList );

//////////////////////////////////////////////////////////////////////////
// CStreamOutTest
//////////////////////////////////////////////////////////////////////////

CStreamOutTest::CStreamOutTest() :
	m_pRS( 0 ),
	m_pDSDisable( 0 ),
	m_pPS( 0 ),
	m_pGS( 0 ),
	m_pVS( 0 ),
	m_pInputLayout( 0 ),
	m_pVB( 0 ),
	m_pCB( 0 ),
	m_pSOStatsQuery( 0 ),
	m_pSOOverflowPredicate( 0 ),
	m_pPipelineStatsQuery( 0 ),
	m_gsMaxVertexCount( 0 ),
	m_pVSClear( 0 ),
	m_pGSClear( 0 ),
	m_rasterizeStream( 0 ),
	m_haveRuntimeCalculateSOStrides( false )
{
	memset( &m_soBufferDesc, 0, sizeof( m_soBufferDesc ) );
	memset( &m_soBuffers, 0, sizeof( m_soBuffers ) );
	memset( &m_soOffsets, 0, sizeof( m_soOffsets ) );
	memset( &m_soBufferSize, 0, sizeof( m_soBufferSize ) );

	// Set the shader model based on the feature level used in the test
	switch( GetFramework()->m_D3DOptions.FeatureLevel )
	{
	default:
		throw("Unknown feature level");
		break;

	case D3D_FEATURE_LEVEL_11_1:
	case D3D_FEATURE_LEVEL_11_0:
		m_sm = SHADER_MODEL_5_0;
		break;

	case D3D_FEATURE_LEVEL_10_1:
		m_sm = SHADER_MODEL_4_1;
		break;

	case D3D_FEATURE_LEVEL_10_0:
		m_sm = SHADER_MODEL_4_0;
		break;
	}
}

TEST_RESULT CStreamOutTest::SetupSOClearObjects()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	
	D3D11_SO_DECLARATION_ENTRY clearDecl[] =
	{
		{ 0, "data", 0, 0, 1, 0 }
	};

	// Setup VS
	char magic_number_string[32];
	_sntprintf( magic_number_string, _countof( magic_number_string ) - 1, "0x%8.8x", MAGIC_NUMBER );
	D3D10_SHADER_MACRO shaderDefines[] = 
	{
		{ "MAGIC_NUMBER", magic_number_string },
		{ NULL, NULL }
	};

	tr = SetupVSFromResource( _T( "ShaderClear.sh" ), "VSClear", NULL, 0, shaderDefines, NULL, 0, &m_pVSClear, NULL );
	if( tr != RESULT_PASS )
		goto FAIL;
	
	// Setup GS
	UINT stride = sizeof(UINT);
	tr = SetupGSFromResource( _T( "ShaderClear.sh" ), "VSClear", clearDecl, 1, shaderDefines, &stride, 1, false, &m_pGSClear );
	if( tr != RESULT_PASS )
		goto FAIL;
		
	return tr;
		
FAIL:
	CleanupSOClearObjects();
	return tr;
}

void CStreamOutTest::CleanupSOClearObjects()
{
	SAFE_RELEASE( m_pVSClear );
	SAFE_RELEASE( m_pGSClear );
}

TEST_RESULT CStreamOutTest::ClearSOBuffer( ID3D11Buffer *soBuffer, UINT size )
{
	TEST_RESULT tr = RESULT_PASS;
	
	GetEffectiveContext()->IASetInputLayout( NULL ); // No input vertices
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	GetEffectiveContext()->IASetIndexBuffer( NULL, DXGI_FORMAT_UNKNOWN, 0 );
	GetEffectiveContext()->VSSetShader( m_pVSClear, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGSClear, NULL, 0 );
	
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->OMSetDepthStencilState( m_pDSDisable, 0 );
	
	ID3D11Buffer *soTargets[] = { soBuffer };
	UINT soOffsets[] = { 0 };
	GetEffectiveContext()->SOSetTargets( 1, soTargets, soOffsets );
	
	// Draw <size> vertices
	GetEffectiveContext()->Draw( size / 4, 0 );
	
	return tr;
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CStreamOutTest::SetupVertexBuffer( UINT numVerts, void *pVertexData )
{
	//
	// Setup vertex buffer
	//
	const D3D11_BUFFER_DESC vbdesc = 
	{
		numVerts * sizeof( VSIN ),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER,
		0,
		0
	};

	HRESULT hr;

	D3D11_SUBRESOURCE_DATA vbsrup = { pVertexData, 0, 0 };	
	hr = GetDevice()->CreateBuffer( &vbdesc, &vbsrup, &m_pVB );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateBuffer() unexpectedly failed creating vertex buffer - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	
	return RESULT_PASS;
}

TEST_RESULT CStreamOutTest::SetupRasterizer()
{
	D3D11_RASTERIZER_DESC rsdesc = 
	{
		D3D11_FILL_SOLID, // FillMode
		D3D11_CULL_NONE, // CullMode
		FALSE, // FrontCounterClockwise
		0, // DepthBias
		0.f, // DepthBiasClamp
		0.f, // SlopeScaledDepthBias
		FALSE, // DepthClipEnable
		FALSE, // ScissorEnable
		FALSE, // MultisampleEnable
		FALSE, // AntialiasedLineEnable
	};

	HRESULT hr = GetDevice()->CreateRasterizerState( &rsdesc, &m_pRS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateRasterizerState() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Create depth/stencil state
	//
	D3D11_DEPTH_STENCIL_DESC dsdesc;
	memset( &dsdesc, 0, sizeof( dsdesc ) );
	dsdesc.DepthEnable = FALSE;
	dsdesc.StencilEnable = FALSE;

	hr = GetDevice()->CreateDepthStencilState( &dsdesc, &m_pDSDisable );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateDepthStencilState() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	
	return RESULT_PASS;
}

TEST_RESULT CStreamOutTest::SetupQueryObjects()
{
	D3D11_QUERY_DESC qdesc;
	qdesc.MiscFlags = 0;
	qdesc.Query = D3D11_QUERY_SO_STATISTICS;
	HRESULT hr = GetDevice()->CreateQuery( &qdesc, &m_pSOStatsQuery );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateQuery() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Setup SO statistics query
	//
	qdesc.Query = D3D11_QUERY_SO_OVERFLOW_PREDICATE;
	hr = GetDevice()->CreateQuery( &qdesc, &m_pSOOverflowPredicate );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateQuery() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	
	qdesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
	hr = GetDevice()->CreateQuery( &qdesc, &m_pPipelineStatsQuery );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateQuery() unexpectedly failed creating pipeline stats - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	
	return RESULT_PASS;
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CStreamOutTest::CreateSOBuffers( ID3D11Buffer *SOBuffers[4], D3D11_BUFFER_DESC SOBufferDesc[4] )
{
	for( int i = 0; i < D3D11_SO_BUFFER_SLOT_COUNT; ++i )
	{
		if( SOBufferDesc[i].ByteWidth > 0 )
		{
			HRESULT hr = GetDevice()->CreateBuffer( &SOBufferDesc[i], NULL, &SOBuffers[i] );
			if( FAILED( hr ) )
			{
				WriteToLog( "CreateBuffer() unexpectedly failed with result %s", D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}
		}
		else
		{
			SOBuffers[i] = NULL;
		}
	}
	
	return RESULT_PASS;
}

//////////////////////////////////////////////////////////////////////////
#define MEMBER_OFFSET( member, instance ) \
	(int) ( ptrdiff_t( &instance.member ) - ptrdiff_t( &instance ) )

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CStreamOutTest::SetupVSFromResource( LPCTSTR resourceID, LPCSTR entryProc, D3D11_SO_DECLARATION_ENTRY *soDecl, UINT numSOElements, const D3D10_SHADER_MACRO *shaderDefines, const UINT* outputBufferStride, UINT numStrides, ID3D11VertexShader **ppVS, ID3D11InputLayout **ppInputLayout )
{
	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;
	TEST_RESULT tr = RESULT_PASS;
	tstring szShaderProfile = "";

	// For featurelevel < 11.0
	// numStrides must be 1 (there is only 1 stream)
	if( GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
	{
		for( UINT i = 1; i < numStrides; i++ )
		{
			assert( 0 == outputBufferStride[i] );
		}

		numStrides = min(numStrides, 1);
	}

	//
	// Compile shader source
	//
	tstring szShaderSuffix = GetShaderModelSuffix();

	if( "" == szShaderSuffix )
	{
		WriteToLog( _T( "GetShaderModelSuffix() unexpectedly failed to generate the shader model suffix." ) );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	szShaderProfile = "vs" + szShaderSuffix;

	HRESULT hr = D3DX10CompileFromResource( NULL, resourceID, NULL, shaderDefines, NULL, entryProc, szShaderProfile.c_str(), D3D10_SHADER_DEBUG, 0, NULL, &pShader, &pError, NULL );

	if( pError )
	{
		WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CompileShaderFromResource() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	//
	// Create vertex shader object
	//
	hr = GetDevice()->CreateVertexShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, ppVS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateVertexShader() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto setupDone;
	}
	
	if( ppInputLayout )
	{
		//
		// Create an input layout to correspond with this shader
		//
		VSIN dmmy;
		D3D11_INPUT_ELEMENT_DESC iedesc[] =
		{
			{ "pos", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, MEMBER_OFFSET( pos, dmmy ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "data", 0, DXGI_FORMAT_R32_UINT, 0, MEMBER_OFFSET( data, dmmy ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		
		hr = GetDevice()->CreateInputLayout( iedesc, 2, pShader->GetBufferPointer(), pShader->GetBufferSize(), ppInputLayout );
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "CreateInputLayout() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto setupDone;
		}
	}
	
	//
	// Create NULL GS for stream out direct from VS
	//
	if( soDecl && numSOElements > 0 )
	{
		hr = GetDevice()->CreateGeometryShaderWithStreamOutput( pShader->GetBufferPointer(), pShader->GetBufferSize(), soDecl, numSOElements, outputBufferStride, numStrides, m_rasterizeStream, NULL, &m_pGS );

		if( FAILED( hr ) )
		{
			WriteToLog( _T( "CreateGeometryShaderWithStreamOutput() failed unexpectedly creating NULL GS, result code %s" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto setupDone;
		}
	}
	
setupDone:
	SAFE_RELEASE( pError );
	SAFE_RELEASE( pShader );
	
	return tr;
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CStreamOutTest::SetupGSFromResource( LPCTSTR resourceID, LPCSTR entryProc, D3D11_SO_DECLARATION_ENTRY *soDecl, UINT numElements, const D3D10_SHADER_MACRO* pDefines, const UINT* outputBufferStride, UINT numStrides, bool multiStream, ID3D11GeometryShader **ppGS )
{
	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;
	TEST_RESULT tr = RESULT_PASS;
	tstring szShaderPrefix = "";
	tstring szShaderProfile = "";

	// For featurelevel < 11.0
	// numStrides must be 1 (there is only 1 stream)
	if( GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
	{
		for( UINT i = 1; i < numStrides; i++ )
		{
			assert( 0 == outputBufferStride[i] );
		}

		numStrides = min(numStrides, 1);
	}

	// GS entry points all start with "GS", VS entry points start with "VS"
	// If the GS outputs to multiple streams
	// then use SM 5_0
	if( entryProc[0] == 'G' )
		szShaderPrefix = "gs";
	else
		szShaderPrefix = "vs";

	tstring szShaderSuffix = multiStream ? "_5_0" : GetShaderModelSuffix();

	if( "" == szShaderSuffix )
	{
		WriteToLog( _T( "GetShaderModelSuffix() unexpectedly failed to generate the shader model suffix." ) );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	szShaderProfile = szShaderPrefix + szShaderSuffix;

	HRESULT hr = m_shaderCache.CompileFromResource( resourceID, pDefines, entryProc, szShaderProfile.c_str(), D3D10_SHADER_DEBUG, 0, &pShader, &pError );
	
	if( pError )
	{
		WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CompileShaderFromResource() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	hr = GetDevice()->CreateGeometryShaderWithStreamOutput( pShader->GetBufferPointer(), pShader->GetBufferSize(), soDecl, numElements, outputBufferStride, numStrides, m_rasterizeStream, NULL, ppGS );

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateGeometryShaderWithStreamOutput() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto setupDone;
	}

setupDone:
	SAFE_RELEASE( pError );
	SAFE_RELEASE( pShader );
	
	return tr;
};

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CStreamOutTest::SetupPSFromResource( LPCTSTR resourceID, LPCSTR entryProc, ID3D11PixelShader **ppPS )
{
	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;
	TEST_RESULT tr = RESULT_PASS;
	tstring szShaderProfile = "";
	tstring szShaderSuffix = GetShaderModelSuffix();

	if( "" == szShaderSuffix )
	{
		WriteToLog( _T( "GetShaderModelSuffix() unexpectedly failed to generate the shader model suffix." ) );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	szShaderProfile = "ps" + szShaderSuffix;
	HRESULT hr = D3DX10CompileFromResource( NULL, resourceID, NULL, NULL, NULL, entryProc, szShaderProfile.c_str(), D3D10_SHADER_DEBUG, 0, NULL, &pShader, &pError, NULL );

	if( pError )
	{
		WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
		pError->Release();
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CompileShaderFromResource() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	hr = GetDevice()->CreatePixelShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, ppPS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreatePixelShader() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto setupDone;
	}


setupDone:
	SAFE_RELEASE( pError );
	SAFE_RELEASE( pShader );

	return tr;
}

tstring CStreamOutTest::ComposeGSName()
{
	tstring inputPrimitive;

	switch( m_inputTopology )
	{
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		inputPrimitive = _T( "Point" );
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		inputPrimitive = _T( "Line" );
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		inputPrimitive = _T( "Triangle" );
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		inputPrimitive = _T( "LineAdj" );
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		inputPrimitive = _T( "TriangleAdj" );
		break;
	}

	tstring gsName = tstring( _T( "GS" ) ) + inputPrimitive + tstring( _T( "In" ) ) + m_gsOutputPrimitiveType + tstring( _T( "Out" ) ) + ToString( m_gsMaxVertexCount );
	return gsName;
};


void CStreamOutTest::WriteDeclToLog( D3D11_SO_DECLARATION_ENTRY *decl, UINT numEntries )
{
	for( UINT i = 0; i < numEntries; ++i )
	{
		TCHAR name[512];
		if( decl[i].SemanticName == NULL )
			CopyString( name, _T( "(null)" ), sizeof( name ) - 1 );
		else
			CopyString( name, decl[i].SemanticName, sizeof( name ) - 1 );
		WriteToLog( _T( "SODecl entry %i - Stream: %u, SemanticName: %s, SemanticIndex: %i, StartComponent: %i: ComponentCount: %i, OutputSlot: %i" ), i, decl[i].Stream, name, decl[i].SemanticIndex, decl[i].StartComponent, decl[i].ComponentCount, decl[i].OutputSlot );
	}
}

tstring CStreamOutTest::GetShaderModelSuffix()
{
	// Local variables
	tstring result = "";

	switch( m_sm )
	{
	default:
		WriteToLog( _T("GetShaderModelSuffix() - Invalid shader model used in the test.") );
		break;

	case SHADER_MODEL_4_0:
		result = "_4_0";
		break;

	case SHADER_MODEL_4_1:
		result = "_4_1";
		break;

	case SHADER_MODEL_5_0:
		result = "_5_0";
		break;
	};

	return result;
}


//////////////////////////////////////////////////////////////////////////
// CStreamOutTestApp
//////////////////////////////////////////////////////////////////////////
class CStreamOutTestApp : public CD3D11TestFramework
{
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }

	void InitOptionVariables()
	{
		// Single device conformance test.
		CD3D11TestFramework::InitOptionVariables();
		UnRegisterOptionVariable( "SrcOnly" );
		m_D3DOptions.SrcOnly = true;
	}

	bool InitTestGroups()
	{
		if( !ADD_TEST( "SingleBuffer", CStreamOutSingleBuffer ) ) return false;
		if( !ADD_TEST( "MultiBuffer", CStreamOutMultiBuffer ) ) return false;

		if( m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
		{
			if( !ADD_TEST( "MultiStream", CStreamOutMultiStream ) ) return false;
			if( !ADD_TEST( "StreamOutPatches", CStreamOutPatches )) return false;
		}

		return true;
	}
};

CStreamOutTestApp g_app;


//////////////////////////////////////////////////////////////////////////
//CShaderCache
//////////////////////////////////////////////////////////////////////////
CShaderCache::CShaderCache()
{
	// Mark all entries as invalid
	for( UINT i = 0; i < CACHE_SIZE; i++ )
	{
		_Cache[i].pBlob = NULL;
	}

	m_Age = 0;
}

CShaderCache::~CShaderCache()
{
	// Free all resources
	Clear();
}

void CShaderCache::Clear()
{
	for( UINT i = 0; i < CACHE_SIZE; i++ )
	{
		SAFE_RELEASE( _Cache[i].pBlob );
	}
}

HRESULT CShaderCache::CompileFromResource(LPCSTR pSrcResource, 
										  const D3D10_SHADER_MACRO* pDefines, 
										  LPCSTR pFunctionName, 
										  LPCSTR pProfile, 
										  UINT flags1, 
										  UINT flags2, 
										  ID3D10Blob** ppBlob,
										  ID3D10Blob** ppErrorMessages)
{
	HRESULT hr = S_OK;
	UINT targetIndex = UINT_MAX;

	*ppBlob = NULL;

	if( ppErrorMessages )
	{
		*ppErrorMessages = NULL;
	}

	m_Age++;

	// build key
	KEY key;
	key.srcResource = std::string(pSrcResource);
	key.functionName = std::string(pFunctionName);
	key.profile = std::string(pProfile);
	key.flags1 = flags1;
	key.flags2 = flags2;

	if( pDefines )
	{
		for( UINT i = 0; pDefines[i].Name != NULL; i++ )
		{
			key.defines.push_back( std::pair<std::string, std::string>( pDefines[i].Name, pDefines[i].Definition) );
		}
	}

	// Search for a valid cache entry with the same key
	for( UINT i = 0; i < CACHE_SIZE; i++ )
	{
		if( _Cache[i].pBlob && _Cache[i].key == key )
		{
			targetIndex = i;
			break;
		}
	}

	if( UINT_MAX == targetIndex )
	{
		// cache miss
		// search for any empty cache slot
		for( UINT i = 0; i < CACHE_SIZE; i++ )
		{
			if( !_Cache[i].pBlob )
			{
				targetIndex = i;
				break;
			}
		}

		if( UINT_MAX == targetIndex )
		{
			// evict the oldest cache entry
			UINT oldest_index = 0;

			for( UINT i = 0; i < CACHE_SIZE; i++ )
			{
				if( _Cache[i].age < _Cache[oldest_index].age )
				{
					oldest_index = i;
				}
			}

			targetIndex = oldest_index;

			SAFE_RELEASE( _Cache[targetIndex].pBlob );
		}
	}

	if( NULL == _Cache[targetIndex].pBlob )
	{
		std::vector<D3D10_SHADER_MACRO> defines;

		for( UINT i = 0; i < key.defines.size(); i++ )
		{
			D3D10_SHADER_MACRO macro = { key.defines[i].first.c_str(), key.defines[i].second.c_str() };
			defines.push_back( macro );
		}

		D3D10_SHADER_MACRO terminator = { NULL, NULL };
		defines.push_back( terminator );

		hr = D3DX10CompileFromResource(NULL, 
									   key.srcResource.c_str(), 
									   NULL, 
									   &(defines[0]),
									   NULL, 
									   key.functionName.c_str(), 
									   key.profile.c_str(), 
									   key.flags1, 
									   key.flags2, 
									   NULL, 
									   &_Cache[targetIndex].pBlob, 
									   ppErrorMessages, 
									   NULL);
	}

	if( SUCCEEDED( hr ) )
	{
		_Cache[targetIndex].key = key;
		_Cache[targetIndex].age = m_Age;
		_Cache[targetIndex].pBlob->AddRef();
		*ppBlob = _Cache[targetIndex].pBlob;
	}

	return hr;
}
