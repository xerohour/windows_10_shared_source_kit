// ***************************************************************
//  GSTopology   version:  1.0   ·  date: 03/27/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#include "GSTopology.h"
#include "GeometryShader.h"
#include <cmath>

struct VERTEX
{
	UINT id;
};

struct SOSTRUCT
{
	float	pos[4];
	UINT	primID;
};

typedef unsigned char PIXEL[4];

#define MAX_VERTICES	128
#define MAX_INDICES		128
#define RT_SIZE_X		37
#define RT_SIZE_Y		31

static UINT Encode( UINT n )
{
	return ( n >> 1 ) ^ n;
}

static UINT MakeVertexID(UINT x)
{
	return ( Encode( x ) << 16 ) + x;
}

static void GenerateNonAdjVertices( VERTEX *outVerts, UINT numVerts )
{
	for( UINT i = 0; i < numVerts; ++i )
	{
		outVerts[i].id = MakeVertexID(i);
	}
}

static void GenerateLineAdjVertices( VERTEX *outVerts, UINT numVerts )
{
	// Initialize vertex data
	UINT i;
	for( i = 0; i < numVerts; ++i )
	{
		outVerts[i].id = MakeVertexID(i);
	}
}

static void GenerateTriangleAdjVertices( VERTEX *outVerts, UINT numVerts )
{
	// Initialize vertex data
	UINT i;
	for( i = 0; i < numVerts; ++i )
	{
		outVerts[i].id = MakeVertexID(i);
	}
}

static void GeneratePointListIndices( UINT16 *outIndices, UINT numIndices, UINT cutInterval )
{
	UINT16 *indices = outIndices;
	for( UINT i = 0; i < numIndices; ++i )
	{
		bool cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		indices[i] = cut ? 0xffff : i;
		//indices[0] = cutInterval && ( i % cutInterval ) == (cutInterval - 1) ? 0xffff : i;
		//++indices;
	}
}

static void GenerateLineListIndices( UINT16 *outIndices, UINT numIndices, UINT cutInterval )
{
	UINT16 *indices = outIndices;
	UINT i = 0;
	UINT index = 0;
	bool cut;
	for(;;)
	{
		// First line vertex
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		indices[i] = cut ? 0xffff : index;
		index++;
		if( ++i == numIndices )
			break;
		
		if( cut )
			continue;
		
		// Second line vertex
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		indices[i] = cut ? 0xffff : index;
		
		if( ++i == numIndices )
			break;
	}
}

static void GenerateTriangleListIndices( UINT16 *outIndices, UINT numIndices, UINT cutInterval )
{
	UINT i = 0;
	UINT index = 0;
	bool cut;
	for( ;; index += 2 )
	{
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 1;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 2;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 1;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 3;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 2;
		if( ++i == numIndices ) break;
		if( cut ) continue; 
	}
}

static void GenerateLineStripIndices( UINT16 *outIndices, UINT numIndices, UINT cutInterval )
{
	UINT i = 0;
	UINT index = 0;
	bool cut = false;
	for( ;; )
	{
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index++;
		if( ++i == numIndices )
			break;
	}
}

static void GenerateTriangleStripIndices( UINT16 *outIndices, UINT numIndices, UINT cutInterval )
{
	UINT16 *indices = outIndices;
	UINT i = 0;
	UINT index = 0;
	bool cut;
	for( ;; )
	{
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		indices[i] = cut ? 0xffff : index;
		if( ++i == numIndices ) break;
		if( cut ) continue;

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		indices[i] = cut ? 0xffff : index + 1;
		if( ++i == numIndices ) break;
		if( cut ) continue;
		index += 2;
	}
}

static void GenerateLineListAdjIndices( UINT16 *outIndices, UINT numIndices, UINT cutInterval )
{
	UINT16 *indices = outIndices;
	UINT i = 0;
	UINT index = 0;
	bool cut;
	for( ;; ++index )
	{
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		indices[i] = cut ? 0xffff : index;
		if( ++i == numIndices ) break;
		if( cut ) continue;

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		indices[i] = cut ? 0xffff : index + 1;
		if( ++i == numIndices ) break;
		if( cut ) continue;

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		indices[i] = cut ? 0xffff : index + 2;
		if( ++i == numIndices ) break;
		if( cut ) continue;

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		indices[i] = cut ? 0xffff : index + 3;
		if( ++i == numIndices ) break;
		if( cut ) continue;
	}
}

static void GenerateTriangleListAdjIndices( UINT16 *outIndices, UINT numIndices, UINT cutInterval )
{
	// Assume the following vertex grid numbering
	// 3    7    11   15   19   23
	// *    *----*----*    *    *
	//      |\   |\   |\
	//      | \  | \  | \  
	//      |  \ |  \ |  \
	// 2    |6  \|10 \|14 \18   22
	// *----*----*----*----*    *
	//  \   |\ B |\ D |\ F |\
	//   \  | \  | \  | \  | \ 
	//    \ |  \ |  \ |  \ |  \
	// 1   \| A \| C \| E \|   \21
	// *    *----*----*----*----*
	//     5 \  9|\ 13|\ 17|   15
	//        \  | \  | \  |
	//         \ |  \ |  \ |
	// 0    4   \|8  \|12 \|16  20
	// *    *    *    *    *    *
	//
	// Therefore:
	// triangle A is  5, 2, 6,10, 9, 8
	// triangle B is  6, 7,10,13, 9, 5
	// triangle C is  9, 6,10,14,13,12
	// triangle D is 10,11,14,17,13, 9
	// triangle E is 13,10,14,18,17,16
	// triangle F is 14,15,18,21,17,13
	UINT i = 0;
	UINT index = 0;
	bool cut;
	for( ;; index += 4 )
	{
		// First triangle in quad
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 5;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 2; // special case for first triangle
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 6;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 10;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 9;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 8;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		// Second triangle in quad
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 6;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 7;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 10;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 13; // special case for last triangle
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 9;
		if( ++i == numIndices ) break;
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 5;
		if( ++i == numIndices ) break;
		if( cut ) continue; 
	}
}

static void GenerateLineStripAdjIndices( UINT16 *outIndices, UINT numIndices, UINT cutInterval )
{
	// Assume the following vertex numbering for each strip
	//
	// 0    1    2    3    4    5    6    7    8
	// *----*----*----*----*----*----*----*----*
	//
	
	UINT i = 0;
	UINT index = 0;
	bool cut = false;
	for( ;; index++ )
	{
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index;
		if( ++i == numIndices ) break;
		if( cut ) continue; 
		
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 1;
		if( ++i == numIndices ) break;
		if( cut ) continue; 
		
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 2;
		if( ++i == numIndices ) break;
		if( cut ) continue; 
lastVert:
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 3;
		if( ++i == numIndices ) break;
		if( cut ) continue; 
		++index;
		goto lastVert;
	}
}

static void GenerateTriangleStripAdjIndices( UINT16 *outIndices, UINT numIndices, UINT cutInterval )
{
	// Assume the following vertex grid numbering
	// 3    7    11   15   19   23
	// *    *----*----*    *    *
	//      |\   |\   |\
	//      | \  | \  | \  
	//      |  \ |  \ |  \
	// 2    |6  \|10 \|14 \18   22
	// *----*----*----*----*    *
	//  \   |\ B |\ D |\ F |\
	//   \  | \  | \  | \  | \ 
	//    \ |  \ |  \ |  \ |  \
	// 1   \| A \| C \| E \|   \21
	// *    *----*----*----*----*
	//     5 \  9|\ 13|\ 17|   15
	//        \  | \  | \  |
	//         \ |  \ |  \ |
	// 0    4   \|8  \|12 \|16  20
	// *    *    *    *    *    *
	//
	// Therefore:
	// Strip numbering is: 5, 2, 6, 8, 9, 7, 10, 12, 13, 11, 14, 16, 17, 15, 18, 21
	
	UINT i = 0;
	UINT index = 0;
	bool cut = true;
	bool aftercut = false;
	for( ;; aftercut = false )
	{
		aftercut = cut;
		
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		if( i > 0 )
		{
			if( cut )
				outIndices[i-1] += 1; // Fixup last index
			else
				index += 4;
		}
		outIndices[i] = cut ? 0xffff : index + 5;
		if( ++i == numIndices ) break;
		if( cut ) continue; 
		
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : ( aftercut ? index + 2 : index + 3 );
		if( ++i == numIndices ) break;
		if( cut ) continue; 
		
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 6;
		if( ++i == numIndices ) break;
		if( cut ) continue; 
		
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 8;
		if( ++i == numIndices ) break;
		if( cut ) continue; 
		
last4verts:
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 9;
		if (cut)
			outIndices[i - 1] += 1; // Fixup last index
		if( ++i == numIndices ) break;
		if( cut ) continue; 
		
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 7;
		if( ++i == numIndices )
		{
			if (!cut)
				outIndices[i - 1] += 3; // Fixup last index
			break;
		}
		if( cut ) continue; 

		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 10;
		if (cut)
			outIndices[i - 1] += 3; // Fixup last index
		if( ++i == numIndices ) break;
		if( cut ) 
			continue; 
		
		cut = cutInterval && ( ( i % cutInterval ) == cutInterval - 1 );
		outIndices[i] = cut ? 0xffff : index + 12;
		if( ++i == numIndices )
		{
			if (!cut)
				outIndices[i - 1] += 1; // Fixup last index
			break;
		}
		if( cut ) 
			continue; 
		index += 4; 
		goto last4verts;
	}
}

//////////////////////////////////////////////////////////////////////////

typedef UINT32 ADJACENCY_ENTRY[4];

// Opposite index is the index of the adjacent vertex opposite each 
// primitive vertex in the winding order starting with the leading vertex.
//
// For lines the opposite index is the index of the vertex adjacent to the opposing
// vertex in a line segment.
//
// Example: 
// 0   1   2   3 
// *---*---*---* 
// If the leading vertex is 1 then element[1] in the adjacency table is [3,0,X,X]
//
// For triangles the opposite index is the index of the vertex adjacent to the opposing
// edge in a triangle.
//
// Example:
// 2     6    10
// *----*----*
//  \   |\   |
//   \  | \  |
//    \ |  \ |
//     \|   \|
//      *----*
//     5 \  9|
//        \  |
//         \ |
//          \|
//          8*
// If the leading vertex is 5 then element[5] in the adjacency table is [10,8,2,X]
static void GenerateLineAdjacencyTable( ADJACENCY_ENTRY *outElements, UINT count, 
								UINT cutInterval, D3D11_PRIMITIVE_TOPOLOGY topology )
{
	// Assume the following vertex numbering for each strip
	//
	// 0    1    2    3    4    5    6    7    8
	// *----*----*----*----*----*----*----*----*
	//
	
	// Initialize all element values to 0xffffffff
	memset( outElements, 0xff, count * sizeof( ADJACENCY_ENTRY ) );

	UINT cut = cutInterval;
	UINT vertCount = 4;
	UINT primIdx = 0;
	UINT startVert = 0;
	for( UINT i = 0; i < count; ++i )
	{
		if( cut )
		{
			if( --cut == 0 )
			{
				cut = cutInterval;
				vertCount = 4;
				++startVert;
				continue;
			}
		}
		if( --vertCount == 0 )
		{
			vertCount = (topology==D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ)?4:1;
			outElements[primIdx][0] = startVert + 3;
			outElements[primIdx][1] = startVert;
			outElements[primIdx][3] = startVert + 1;
			if( outElements[primIdx][0] >= count )
				outElements[primIdx][0] = 0xffffffff;
			++primIdx;
			++startVert;
		}
	}
}

static void GenerateTriangleAdjacencyTable( ADJACENCY_ENTRY *outElements, UINT count, 
									UINT cutInterval, D3D11_PRIMITIVE_TOPOLOGY topology )
{
	// Assume the following vertex grid numbering
	// 3    7    11   15   19   23
	// *    *----*----*    *    *
	//      |\   |\   |\ 
	//      | \  | \  | \  
	//      |  \ |  \ |  \ 
	// 2    |6  \|10 \|14 \18   22
	// *----*----*----*----*    *
	//  \   |\ B |\ D |\ F |\ 
	//   \  | \  | \  | \  | \ 
	//    \ |  \ |  \ |  \ |  \ 
	// 1   \| A \| C \| E \|   \21
	// *    *----*----*----*----*
	//     5 \  9|\ 13|\ 17|   15
	//        \  | \  | \  |
	//         \ |  \ |  \ |
	// 0    4   \|8  \|12 \|16  20
	//
	// For odd vertex numbering:
	// 3    7    11
	// *    *    *
	//      |\ 
	//      | \
	//      |  \
	// 2    |6  \10
	// *----*----*
	//  \   |\ B ||
	//   \  | \  |\
	//    \ |  \ | |
	// 1   \| A \| \
	// *    *----*  |
	//     5 \  9|\ \
	//        \  | \ |
	//         \ |  \\
	// 0    4   \|8  \|12

	// Initialize all element values to 0xffffffff
	memset( outElements, 0xff, count * sizeof( ADJACENCY_ENTRY ) );
	
	UINT cut = cutInterval;
	UINT vertCount = 6;
	UINT primIdx = 0;
	UINT startVert = 5;
	for( ; count > 0;  )
	{
		--count;

		if ((startVert % 4) < 1)
			startVert += 1;
		if ((startVert % 4) > 2)
			startVert += 2;
		if( cut )
		{
			if( --cut == 0 )
			{
				cut = cutInterval;
				vertCount = 6;
				startVert += 3;
				continue;
			}
		}
		
		if( --vertCount == 0 )
		{
			bool cutNext = cutInterval && cut == 1;
			bool cutNextNext = cutInterval && cut == 2;
			bool oddcount = topology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ && (( count == 1 && !cutNext ) || ( count >= 2 && cutNextNext ));
			vertCount = (topology==D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ)?6:2;
			if ((startVert % 2) == 0)
			{
				outElements[primIdx][0] = startVert + ( oddcount ? 6 : 7 ); // May be + 6 if # vert indices after last cut is odd
				outElements[primIdx][1] = startVert - 1;
				outElements[primIdx][2] = startVert + 1;
				outElements[primIdx][3] = startVert;
			}
			else
			{
				outElements[primIdx][0] = startVert + ( oddcount ? 2 : 5 ); // May be + 2 if # vert indices after last cut is odd
				outElements[primIdx][1] = startVert + 3;
				outElements[primIdx][2] = startVert - 3;
				outElements[primIdx][3] = startVert;
			}
			++primIdx;
			startVert += 1;
		}
	}
}


static TEST_RESULT TestPixelOn( UINT x, UINT y, PIXEL *pPixels, UINT pitch )
{
	TEST_RESULT tr = RESULT_PASS;
	PIXEL *pixel = (PIXEL *) ( (char *) pPixels + y * pitch ) + x;

	if( (*pixel)[0] == 0 )
	{
		GetFramework()->WriteToLogF( "Failed to render expected primitive at pixel location %i, %i", x, y );
		tr = RESULT_FAIL;
	}
	else
	{
		if( (*pixel)[0] < 200 )
		{
			GetFramework()->WriteToLogF( "Incorrect FIRST adjacent index for primitive rendered at pixel location %i, %i", x, y );
			tr = RESULT_FAIL;
		}

		if( (*pixel)[1] < 200 )
		{
			GetFramework()->WriteToLogF( "Incorrect SECOND adjacent index for primitive rendered at pixel location %i, %i", x, y );
			tr = RESULT_FAIL;
		}

		if( (*pixel)[2] < 200 )
		{
			GetFramework()->WriteToLogF( "Incorrect THIRD adjacent index for primitive rendered at pixel location %i, %i", x, y );
			tr = RESULT_FAIL;
		}
	}

	return tr;
}

static TEST_RESULT TestPixelOff( UINT x, UINT y, PIXEL *pPixels, UINT pitch )
{
	PIXEL *pixel = (PIXEL *) ( (char *) pPixels + y * pitch ) + x;
	if( (*pixel)[0] || (*pixel)[1] || (*pixel)[2] )
	{
		GetFramework()->WriteToLogF( "Unexpected pixel rendered at location %i, %i", x, y );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

static bool CompareVecN(float a[], float b[], int N, float epsilon)
{
	for (int i = 0; i < N; ++i)
		if (epsilon < fabsf(a[i] - b[i]))
			return false;
	return true;
}

static TEST_RESULT TestSOPoint( UINT soIdx, UINT primID, UINT x, UINT y, SOSTRUCT* pSOVerts )
{
	TEST_RESULT tr = RESULT_PASS;
	SOSTRUCT* pThisPrim = &pSOVerts[soIdx];
	float expectedPos[4] = {
		( ( 2.0f * ( x + 0.6f ) ) / float( RT_SIZE_X ) ) - 1.0f,
		1.0f - ( ( 2.0f * ( 0.6f ) ) / float( RT_SIZE_Y ) ),
		0, 1
	};
	if (!CompareVecN(pThisPrim->pos, expectedPos, 4, 0.000001f)
		|| pThisPrim->primID != primID)
	{
		GetFramework()->WriteToLogF( "Incorrect streamout value for index %i. Expected (%g, %g, %g, %g), primID=%i. Found (%g, %g, %g, %g), primID=%i", 
			soIdx,
			expectedPos[0], expectedPos[1], expectedPos[2], expectedPos[3], primID, 
			pThisPrim->pos[0], pThisPrim->pos[1], pThisPrim->pos[2], pThisPrim->pos[3], pThisPrim->primID);
		tr = RESULT_FAIL;
	}
	return tr;
}

static TEST_RESULT TestSOLine( UINT soIdx, UINT primID, UINT x, UINT y, SOSTRUCT* pSOVerts )
{
	TEST_RESULT tr = RESULT_PASS;
	for (int n = 0; n < 2; ++n)
	{
		SOSTRUCT* pThisPrim = &pSOVerts[soIdx + n];
		float expectedPos[4] = {
			( ( 2.0f * ( x + n + 0.5f ) ) / float( RT_SIZE_X ) ) - 1.0f,
			1.0f - ( ( 2.0f * ( 0.5f ) ) / float( RT_SIZE_Y ) ),
			0, 1
		};
		if (!CompareVecN(pThisPrim->pos, expectedPos, 4, 0.000001f)
			|| pThisPrim->primID != primID)
		{
			tr = RESULT_FAIL;
			GetFramework()->WriteToLogF( "Incorrect streamout value for index %i. Expected (%g, %g, %g, %g), primID=%i. Found (%g, %g, %g, %g), primID=%i", 
				soIdx + n,
				expectedPos[0], expectedPos[1], expectedPos[2], expectedPos[3], primID, 
				pThisPrim->pos[0], pThisPrim->pos[1], pThisPrim->pos[2], pThisPrim->pos[3], pThisPrim->primID);
		}
	}
	return tr;
}

static TEST_RESULT TestSOTriangle( UINT soIdx, UINT primID, UINT x, UINT y, SOSTRUCT* pSOVerts )
{
	TEST_RESULT tr = RESULT_PASS;
	for (int n = 0; n < 3; ++n)
	{
		SOSTRUCT* pThisPrim = &pSOVerts[soIdx + n];
		int row, col;
		if (y == 0)
		{
			col = x + ((n+1) >> 1);
			row = 2 * (n >> 1);
		}
		else
		{
			col = x + (n >> 1);
			row = 2 * (1 - (n & 1));
		}
		float expectedPos[4] = {
			( ( 2.0f * ( col + 0.5f ) ) / float( RT_SIZE_X ) ) - 1.0f,
			1.0f - ( ( 2.0f * ( row + 0.5f ) ) / float( RT_SIZE_Y ) ),
			0, 1
		};
		if (!CompareVecN(pThisPrim->pos, expectedPos, 4, 0.000001f)
			|| pThisPrim->primID != primID)
		{
			tr = RESULT_FAIL;
			GetFramework()->WriteToLogF( "Incorrect streamout value for index %i. Expected (%g, %g, %g, %g), primID=%i. Found (%g, %g, %g, %g), primID=%i", 
				soIdx + n,
				expectedPos[0], expectedPos[1], expectedPos[2], expectedPos[3], primID, 
				pThisPrim->pos[0], pThisPrim->pos[1], pThisPrim->pos[2], pThisPrim->pos[3], pThisPrim->primID);
		}
	}
	return tr;
}

//////////////////////////////////////////////////////////////////////////

CGSTopologyTest::CGSTopologyTest() 
: m_topology( D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED )
, m_bUseGS( false )
, m_bStreamOut( false )
, m_bRasterize( false )
, m_pSOBuffer( NULL )
, m_pVSPoint( NULL )
, m_pVSLine( NULL )
, m_pVSTriangle( NULL )
, m_pVSTriangleAdj( NULL )
, m_pInputVertsPoint( NULL )
, m_pInputVertsLine( NULL )
, m_pInputVertsLineAdj( NULL )
, m_pInputVertsTriangle( NULL )
, m_pInputVertsTriangleAdj( NULL )
, m_pInputIndices( NULL )
, m_pCBAdjacencyTable( NULL )
, m_numIndices( 0 )
, m_cutInterval( 0 )
{
	ZeroMemory(m_pGSPoint, sizeof(m_pGSPoint));
	ZeroMemory(m_pGSLine, sizeof(m_pGSLine));
	ZeroMemory(m_pGSTriangle, sizeof(m_pGSTriangle));
	ZeroMemory(m_pGSLineAdj, sizeof(m_pGSLineAdj));
	ZeroMemory(m_pGSTriangleAdj, sizeof(m_pGSTriangleAdj));
}

CGSTopologyTest::~CGSTopologyTest()
{
}

static D3D11_PRIMITIVE_TOPOLOGY topologyTable[] =
{
    D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
    D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
    D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
    D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
    D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
};


void CGSTopologyTest::InitTestParameters()
{
	CTestCaseParameter< D3D11_PRIMITIVE_TOPOLOGY > *pTopologyParam = AddParameter<D3D11_PRIMITIVE_TOPOLOGY>( _T( "InputTopology" ), &m_topology );
	CTestCaseParameter< UINT > *pcutIntervalParam = AddParameter< UINT >( _T( "cutInterval" ), &m_cutInterval );
	CTestCaseParameter< UINT > *pNumIndicesParam = AddParameter< UINT >( _T( "NumIndices" ), &m_numIndices );
	CTestCaseParameter< bool > *pUseGSParam = AddParameter< bool >( _T( "UseGS" ), &m_bUseGS );
	CTestCaseParameter< bool > *pRasterizeParam = AddParameter< bool >( _T( "Rasterize" ), &m_bRasterize );
	CTestCaseParameter< bool > *pStreamOutParam = AddParameter< bool >( _T( "StreamOut" ), &m_bStreamOut );

	CTableValueSet< D3D11_PRIMITIVE_TOPOLOGY > *pTopologyValueSet = new CTableValueSet< D3D11_PRIMITIVE_TOPOLOGY >( topologyTable, sizeof( topologyTable[0] ), sizeof( topologyTable ) / sizeof( topologyTable[0] ) );
	CBoolValueSet *pUseGSValueSet = new CBoolValueSet();
	CUserValueSet< UINT > *pNumIndicesValueSet = new CUserValueSet< UINT >();
	pNumIndicesValueSet->AddValue( 1 );
	pNumIndicesValueSet->AddValue( 4 );
	pNumIndicesValueSet->AddValue( 27 );
	pNumIndicesValueSet->AddValue( 31 );
	pNumIndicesValueSet->AddValue( 32 );	
	CUserValueSet< UINT > *pcutIntervalValueSet = new CUserValueSet< UINT >();
	pcutIntervalValueSet->AddValue( 0 );
	pcutIntervalValueSet->AddValue( 3 );
	pcutIntervalValueSet->AddValue( 13 );
	pcutIntervalValueSet->AddValue( 14 );
	pcutIntervalValueSet->AddValue( 17 );
	CUserValueSet<bool>* pRasterizeBools = new CUserValueSet<bool>( true, false, true );
	CUserValueSet<bool>* pStreamOutBools = new CUserValueSet<bool>( false, true, true );

	testfactor::RFactor topology = AddParameterValueSet( pTopologyParam, pTopologyValueSet );
	testfactor::RFactor cutInterval = AddParameterValueSet( pcutIntervalParam, pcutIntervalValueSet );
	testfactor::RFactor numIndices = AddParameterValueSet( pNumIndicesParam, pNumIndicesValueSet );
	testfactor::RFactor useGS = AddParameterValueSet( pUseGSParam, pUseGSValueSet );
	testfactor::RFactor rasterize = AddParameterValueSet( pRasterizeParam, pRasterizeBools );
	testfactor::RFactor streamOut = AddParameterValueSet( pStreamOutParam, pStreamOutBools );

	SetRootTestFactor( topology * cutInterval * numIndices * useGS * ( rasterize % streamOut ) );

	// Setup priority weights

	// Reduce less interesting NumIndices
	AddPriorityPatternFilter(FilterNotEqual<UINT>(pNumIndicesParam, 32), 0.5f);
	AddPriorityPatternFilter(FilterEqual<UINT>(pNumIndicesParam, 1), 0.5f);
	
	// Reduce less interesting topology/cut interval variations
	AddPriorityPatternFilter(FilterRegExp(pTopologyParam, _T("POINT")), 0.5); // points are dull in topology testing
	AddPriorityPatternFilter(FilterRegExp(pTopologyParam, _T("TRIANGLELIST")) && FilterNotEqual<UINT>(pcutIntervalParam, 13), 0.5);
	AddPriorityPatternFilter(FilterRegExp(pTopologyParam, _T("LINELIST")) && FilterNotEqual<UINT>(pcutIntervalParam, 17), 0.5f);
	AddPriorityPatternFilter(FilterRegExp(pTopologyParam, _T("TRIANGLESTRIP")) && !(FilterEqual<UINT>(pcutIntervalParam, 14) && FilterEqual<UINT>(pNumIndicesParam, 27)), 0.5f);
	AddPriorityPatternFilter(FilterRegExp(pTopologyParam, _T("LINESTRIP")) && FilterLess<UINT>(pcutIntervalParam, 4), 0.5f);

	// Reduce NULL GS except
	AddPriorityPatternFilter(FilterEqual<bool>(pUseGSParam, false), 0.5f);

	// Reduce simultaneous streaming and rasterizing
	AddPriorityPatternFilter(FilterEqual<bool>(pStreamOutParam, true) && FilterEqual<bool>(pRasterizeParam, true), 0.5f);

	// Promote some TRIANGLELIST and TRIANGLESTRIP variations
	AddPriorityPatternFilter(FilterRegExp(pTopologyParam, _T("TRIANGLE(LIST|STRIP)$")), 2.0f);

	DeclarePriorityLevelByPriorityValue( 1, 1.0f );
}

TEST_RESULT CGSTopologyTest::Setup()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_BUFFER_DESC bufDesc;

	//
	// Resize the output window
	//
	GetFramework()->m_RenderTargetOptions.SwapChainDesc.Height = RT_SIZE_Y;
	GetFramework()->m_RenderTargetOptions.SwapChainDesc.Width = RT_SIZE_X;
	GetFramework()->m_RenderTargetOptions.FullscreenDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	GetFramework()->CleanupRenderTarget();
	GetFramework()->SetupRenderTarget();
	
	tr = CommonSetup();
	if( tr != RESULT_PASS )
		return tr;
		
	//
	// Create vertex buffers for each topology class
	//
	VERTEX stagingVerts[MAX_VERTICES];
	D3D11_SUBRESOURCE_DATA data;
	
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.ByteWidth = MAX_VERTICES * sizeof( VERTEX );
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
	
	// Point
	GenerateNonAdjVertices( stagingVerts, MAX_VERTICES );
	data.pSysMem = stagingVerts;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	hr = GetDevice()->CreateBuffer( &bufDesc, &data, &m_pInputVertsPoint );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create vertex buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	
	// Line
	data.pSysMem = stagingVerts;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	hr = GetDevice()->CreateBuffer( &bufDesc, &data, &m_pInputVertsLine );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create vertex buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	
	// Triangle
	data.pSysMem = stagingVerts;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	hr = GetDevice()->CreateBuffer( &bufDesc, &data, &m_pInputVertsTriangle );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create vertex buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	
	// Line Adj
	GenerateLineAdjVertices( stagingVerts, MAX_VERTICES );
	data.pSysMem = stagingVerts;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	hr = GetDevice()->CreateBuffer( &bufDesc, &data, &m_pInputVertsLineAdj );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create vertex buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	
	// Triangle Adj
	GenerateTriangleAdjVertices( stagingVerts, MAX_VERTICES );
	data.pSysMem = stagingVerts;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	hr = GetDevice()->CreateBuffer( &bufDesc, &data, &m_pInputVertsTriangleAdj );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create vertex buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	

	//
	// Create index buffer
	//
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.ByteWidth = MAX_INDICES * sizeof( UINT16 );
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	hr = GetDevice()->CreateBuffer( &bufDesc, NULL, &m_pInputIndices );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create index buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Create constant buffer for adjacency table
	//
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.ByteWidth = 4096 * 4 * 4;
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	hr = GetDevice()->CreateBuffer( &bufDesc, NULL, &m_pCBAdjacencyTable );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create constant buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Create SO buffer
	//
	bufDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.ByteWidth = MAX_VERTICES * 3 * 4 * sizeof(float);
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	hr = GetDevice()->CreateBuffer( &bufDesc, NULL, &m_pSOBuffer );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateBuffer() unexpectedly failed to create streamout buffer - result code %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	D3D11_SO_DECLARATION_ENTRY pointSODecl[] =
	{
		{ 0, "SV_POSITION", 0, 0, 4, 0 },
		{ 0, "primID", 0, 0, 1, 0 },
	};
	const UINT numSOElements = sizeof(pointSODecl)/sizeof(*pointSODecl);

	//
	// Setup vertex shaders
	//
	D3D11_INPUT_ELEMENT_DESC iedesc[] = 
	{
		{ "id", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	const UINT numInputElements = sizeof(iedesc)/sizeof(*iedesc);
	
	tr = SetupVSFromResource( "GSTopology.sh", "VSPassThrough", iedesc, sizeof(iedesc)/sizeof(*iedesc), &m_pVS, &m_pInputLayout );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupVSFromResource( "GSTopology.sh", "VSPoint", iedesc, sizeof(iedesc)/sizeof(*iedesc), &m_pVSPoint, NULL );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSTopology.sh", "VSPoint", pointSODecl, numSOElements, sizeof(SOSTRUCT), &m_pGSPoint[2] );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupVSFromResource( "GSTopology.sh", "VSLine", iedesc, sizeof(iedesc)/sizeof(*iedesc), &m_pVSLine, NULL );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSTopology.sh", "VSLine", pointSODecl, numSOElements, sizeof(SOSTRUCT), &m_pGSLine[2] );
	if( tr != RESULT_PASS )
		return tr;
	m_pGSLineAdj[2] = m_pGSLine[2];
	m_pGSLineAdj[2]->AddRef();

	tr = SetupVSFromResource( "GSTopology.sh", "VSTriangle", iedesc, sizeof(iedesc)/sizeof(*iedesc), &m_pVSTriangle, NULL );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSTopology.sh", "VSTriangle", pointSODecl, numSOElements, sizeof(SOSTRUCT), &m_pGSTriangle[2] );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupVSFromResource( "GSTopology.sh", "VSTriangleAdj", iedesc, sizeof(iedesc)/sizeof(*iedesc), &m_pVSTriangleAdj, NULL );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSTopology.sh", "VSTriangleAdj", pointSODecl, numSOElements, sizeof(SOSTRUCT), &m_pGSTriangleAdj[2] );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Setup geometry shaders
	//
	tr = SetupGSFromResource( "GSTopology.sh", "GSPoint", &m_pGSPoint[0] );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSTopology.sh", "GSPoint", pointSODecl, numSOElements, sizeof(SOSTRUCT), &m_pGSPoint[1] );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupGSFromResource( "GSTopology.sh", "GSLine", &m_pGSLine[0] );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSTopology.sh", "GSLine", pointSODecl, numSOElements, sizeof(SOSTRUCT), &m_pGSLine[1] );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupGSFromResource( "GSTopology.sh", "GSTriangle", &m_pGSTriangle[0] );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSTopology.sh", "GSTriangle", pointSODecl, numSOElements, sizeof(SOSTRUCT), &m_pGSTriangle[1] );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupGSFromResource( "GSTopology.sh", "GSLineAdj", &m_pGSLineAdj[0] );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSTopology.sh", "GSLineAdj", pointSODecl, numSOElements, sizeof(SOSTRUCT), &m_pGSLineAdj[1] );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupGSFromResource( "GSTopology.sh", "GSTriangleAdj", &m_pGSTriangleAdj[0] );
	if( tr != RESULT_PASS )
		return tr;
	tr = SetupGSWithSOFromResource( "GSTopology.sh", "GSTriangleAdj", pointSODecl, numSOElements, sizeof(SOSTRUCT), &m_pGSTriangleAdj[1] );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Setup pixel shaders
	//
	tr = SetupPSFromResource( "GSTopology.sh", "PSMain", &m_pPS );
	if( tr != RESULT_PASS )
		return tr;
	
	return tr;
}

void CGSTopologyTest::Cleanup()
{
	SAFE_RELEASE( m_pInputIndices );
	SAFE_RELEASE( m_pInputVertsPoint );
	SAFE_RELEASE( m_pInputVertsLine );
	SAFE_RELEASE( m_pInputVertsLineAdj );
	SAFE_RELEASE( m_pInputVertsTriangle );
	SAFE_RELEASE( m_pInputVertsTriangleAdj );
	SAFE_RELEASE( m_pInputIndices );
	SAFE_RELEASE( m_pVSLine );
	SAFE_RELEASE( m_pVSPoint );
	SAFE_RELEASE( m_pVSTriangle );
	SAFE_RELEASE( m_pVSTriangleAdj );
	for (int n = 0; n < ARRAYSIZE(m_pGSPoint); ++n)
	{
		SAFE_RELEASE( m_pGSPoint[n] );
	}
	for (int n = 0; n < ARRAYSIZE(m_pGSLine); ++n)
	{
		SAFE_RELEASE( m_pGSLine[n] );
	}
	for (int n = 0; n < ARRAYSIZE(m_pGSTriangle); ++n)
	{
		SAFE_RELEASE( m_pGSTriangle[n] );
	}
	for (int n = 0; n < ARRAYSIZE(m_pGSLineAdj); ++n)
	{
		SAFE_RELEASE( m_pGSLineAdj[n] );
	}
	for (int n = 0; n < ARRAYSIZE(m_pGSTriangleAdj); ++n)
	{
		SAFE_RELEASE( m_pGSTriangleAdj[n] );
	}
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pSOBuffer );
	SAFE_RELEASE( m_pCBAdjacencyTable );
	
	CommonCleanup();
}


TEST_RESULT CGSTopologyTest::ExecuteTestCase()
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext *pDeviceContext = GetEffectiveContext();
	HRESULT hr = E_FAIL;
	TEST_RESULT tr = RESULT_PASS;
	ID3D11VertexShader *pVS = NULL;
	ID3D11GeometryShader *pGS = NULL;
	ID3D11PixelShader *pPS = m_pPS;
	ID3D11Buffer *pCBAdjacency = NULL;
	ID3D11Buffer *vertexBuffers[1];
	ID3D11Query *pPipelineQuery = NULL;
	ID3D11RenderTargetView* pCachedRenderTarget = NULL;
	ID3D11DepthStencilView* pCachedDepthStencil = NULL;
	ID3D11Buffer *pTempBuffer = NULL;
	SOSTRUCT *pSOVerts = NULL;
	UINT vertexOffsets[1] = { 0 };
	UINT vertexStrides[1] = { sizeof( VERTEX ) };
	
	//
	// Fetch the viewport dimensions
	//
	CONST_DATA *constData;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	GetEffectiveContext()->Map( m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	constData = (CONST_DATA*) mappedRes.pData;
	constData->vpSize[0] = float( RT_SIZE_X );
	constData->vpSize[1] = float( RT_SIZE_Y );
	GetEffectiveContext()->Unmap( m_pCB, 0 );
	
	//
	// Update adjacency table constant buffer
	//
	ADJACENCY_ENTRY *adjData;

	//
	// Update index buffers
	//
	UINT16 *pIndices;
	GetEffectiveContext()->Map( m_pInputIndices, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	pIndices = (UINT16*) mappedRes.pData;
	
	D3D11_MAPPED_SUBRESOURCE mappedResAdj;
	switch( m_topology )
	{
		case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
			GeneratePointListIndices( pIndices, m_numIndices, m_cutInterval );
			vertexBuffers[0] = m_pInputVertsPoint;
			pVS = m_pVSPoint;
			pGS = m_pGSPoint[(!m_bUseGS)?2:m_bStreamOut?1:0];
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
			GenerateLineListIndices( pIndices, m_numIndices, m_cutInterval );
			vertexBuffers[0] = m_pInputVertsLine;
			pVS = m_pVSLine;
			pGS = m_pGSLine[(!m_bUseGS)?2:m_bStreamOut?1:0];
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
			GenerateLineStripIndices( pIndices, m_numIndices, m_cutInterval );
			vertexBuffers[0] = m_pInputVertsLine;
			pVS = m_pVSLine;
			pGS = m_pGSLine[(!m_bUseGS)?2:m_bStreamOut?1:0];
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			GenerateTriangleListIndices( pIndices, m_numIndices, m_cutInterval );
			vertexBuffers[0] = m_pInputVertsTriangle;
			pVS = m_pVSTriangle;
			pGS = m_pGSTriangle[(!m_bUseGS)?2:m_bStreamOut?1:0];
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
			GenerateTriangleStripIndices( pIndices, m_numIndices, m_cutInterval );
			vertexBuffers[0] = m_pInputVertsTriangle;
			pVS = m_pVSTriangle;
			pGS = m_pGSTriangle[(!m_bUseGS)?2:m_bStreamOut?1:0];
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
			GenerateLineListAdjIndices( pIndices, m_numIndices, m_cutInterval );
			vertexBuffers[0] = m_pInputVertsLineAdj;
			pVS = m_pVSLine;
			pGS = m_pGSLineAdj[(!m_bUseGS)?2:m_bStreamOut?1:0];
			GetEffectiveContext()->Map( m_pCBAdjacencyTable, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResAdj );
			GenerateLineAdjacencyTable( (ADJACENCY_ENTRY*)mappedResAdj.pData, m_numIndices, m_cutInterval, D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ );
			GetEffectiveContext()->Unmap(m_pCBAdjacencyTable,0);
			pCBAdjacency = m_pCBAdjacencyTable;
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
			GenerateLineStripAdjIndices( pIndices, m_numIndices, m_cutInterval );
			vertexBuffers[0] = m_pInputVertsLineAdj;
			pVS = m_pVSLine;
			pGS = m_pGSLineAdj[(!m_bUseGS)?2:m_bStreamOut?1:0];
			GetEffectiveContext()->Map( m_pCBAdjacencyTable, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResAdj );
			GenerateLineAdjacencyTable( (ADJACENCY_ENTRY*)mappedResAdj.pData, m_numIndices, m_cutInterval, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ );
			GetEffectiveContext()->Unmap(m_pCBAdjacencyTable,0);
			pCBAdjacency = m_pCBAdjacencyTable;
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
			GenerateTriangleListAdjIndices( pIndices, m_numIndices, m_cutInterval );
			vertexBuffers[0] = m_pInputVertsTriangleAdj;
			pVS = m_pVSTriangleAdj;
			pGS = m_pGSTriangleAdj[(!m_bUseGS)?2:m_bStreamOut?1:0];
			GetEffectiveContext()->Map( m_pCBAdjacencyTable, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResAdj );
			GenerateTriangleAdjacencyTable( (ADJACENCY_ENTRY*)mappedResAdj.pData, m_numIndices, m_cutInterval, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ );
			GetEffectiveContext()->Unmap(m_pCBAdjacencyTable,0);
			pCBAdjacency = m_pCBAdjacencyTable;
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
			GenerateTriangleStripAdjIndices( pIndices, m_numIndices, m_cutInterval );
			vertexBuffers[0] = m_pInputVertsTriangleAdj;
			pVS = m_pVSTriangleAdj;
			pGS = m_pGSTriangleAdj[(!m_bUseGS)?2:m_bStreamOut?1:0];
			GetEffectiveContext()->Map( m_pCBAdjacencyTable, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResAdj );
			GenerateTriangleAdjacencyTable( (ADJACENCY_ENTRY*)mappedResAdj.pData, m_numIndices, m_cutInterval, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ );
			GetEffectiveContext()->Unmap(m_pCBAdjacencyTable,0);
			pCBAdjacency = m_pCBAdjacencyTable;
			break;
		
		default:
			assert( !"UNKNOWN INPUT TOPOLOGY" );
			WriteToLog( "UNKNOWN INPUT TOPOLOGY" );
			return RESULT_SKIP;
			break;
	}
	
	GetEffectiveContext()->Unmap(m_pInputIndices,0);
	
	ID3D11Buffer *constBuffers[] = { m_pCB, pCBAdjacency };
	GetEffectiveContext()->GSSetConstantBuffers( 0, 2, constBuffers );

	if( m_bUseGS )
		pVS = m_pVS;
	else if (!m_bStreamOut)
		pGS = NULL;
	
	{
		D3D11_QUERY_DESC soDesc;
		soDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
		soDesc.MiscFlags = 0;
		hr = pDevice->CreateQuery(&soDesc, &pPipelineQuery);
		if (FAILED(hr))
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "CreateQuery(PIPELINE_STATISTICS) failed. hr=%s" ), D3DHResultToString( hr ).c_str() );
			goto testDone;
		}
	}

	//
	// Setup the pipeline states
	//
	pDeviceContext->IASetVertexBuffers( 0, 1, &m_pInputVertsPoint, vertexStrides, vertexOffsets );
	pDeviceContext->IASetInputLayout( m_pInputLayout );
	pDeviceContext->IASetPrimitiveTopology( m_topology );
	pDeviceContext->IASetIndexBuffer( m_pInputIndices, DXGI_FORMAT_R16_UINT, 0 );
	
	pDeviceContext->VSSetShader( pVS, NULL, 0 );
	pDeviceContext->GSSetShader( pGS, NULL, 0 );
	pDeviceContext->PSSetShader( pPS, NULL, 0 );
	
	pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pCB );
	pDeviceContext->GSSetConstantBuffers( 0, 1, &m_pCB );
	pDeviceContext->PSSetConstantBuffers( 0, 1, &m_pCB );

	pDeviceContext->RSSetState( m_pRS );

	if (m_bStreamOut)
	{
		UINT soOffset = 0;
		pDeviceContext->SOSetTargets( 1, &m_pSOBuffer, &soOffset );
	}
	else
		pDeviceContext->SOSetTargets( 0, NULL, NULL );

	pDeviceContext->OMGetRenderTargets(1, &pCachedRenderTarget, &pCachedDepthStencil);
	if (m_bRasterize)
	{
		pDeviceContext->OMSetDepthStencilState( m_pDSS, 0xffffffff );
		float clearColor[4] = { 0, 0, 0, 1 };
		pDeviceContext->ClearRenderTargetView( GetFramework()->GetRTView(), clearColor );
	}
	else
	{
		pDeviceContext->OMSetRenderTargets(0, NULL, NULL);
		pDeviceContext->OMSetDepthStencilState( NULL, 0xffffffff );
	}
	
	pDeviceContext->Begin(pPipelineQuery);
	pDeviceContext->DrawIndexed( m_numIndices, 0, 0 );
	pDeviceContext->End(pPipelineQuery);

	// Copy the back buffer to the memory surface
	if (m_bRasterize)
	{
		pDeviceContext->CopyResource( m_pMemTexture, g_App.GetRTTexture() );
	}
	
	// execute any deferred operations
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		tr = RESULT_FAIL;
		WriteToLog("ExecuteEffectiveContext failed. hr = %s", D3DHResultToString(hr).c_str() );
		goto testDone;
	}

	// copy stream out to a staging buffer
	HRESULT hrTempBufferMap = E_FAIL;
	if (m_bStreamOut)
	{
		D3D11_BUFFER_DESC tempdesc;
		m_pSOBuffer->GetDesc(&tempdesc);
		//tempdesc.ByteWidth = m_numIndices * 6 * sizeof(SOSTRUCT); // plenty of room
		tempdesc.BindFlags = 0;
		tempdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tempdesc.MiscFlags = 0;
		tempdesc.Usage = D3D11_USAGE_STAGING;
		hr = pDevice->CreateBuffer( &tempdesc, NULL, &pTempBuffer );
		if( FAILED( hr ) || !pTempBuffer )
		{
			WriteToLog( _T( "CreateBuffer() unexpectedly failed creating temporary buffer - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}
		GetImmediateContext()->CopyResource( pTempBuffer, m_pSOBuffer );
		hr = pDevice->GetDeviceRemovedReason();
		if (FAILED(hr))
		{
			WriteToLog( _T( "CopyResource(pTempBuffer, m_pSOBuffer) caused DeviceRemoved - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}
		D3D11_MAPPED_SUBRESOURCE mappedRes;
		hr = GetImmediateContext()->Map( pTempBuffer, 0, D3D11_MAP_READ, 0, &mappedRes );
		pSOVerts = (SOSTRUCT*)mappedRes.pData;
		hrTempBufferMap = hr;
		if( FAILED( hr ) || !pSOVerts)
		{
			WriteToLog( _T( "pTempBuffer->Map() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}
	}

	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
	hr = GetFramework()->GetDataWithTimeout(GetImmediateContext(), pPipelineQuery, &pipelineStats, sizeof(pipelineStats), 0 );
	if (hr != S_OK)
	{
		tr = RESULT_FAIL; 
		WriteToLog( _T( "pPipelineQuery->GetData() failed or timed out. hr=%s" ), D3DHResultToString( hr ).c_str() );
	}

	D3D11_MAPPED_SUBRESOURCE map;
	map.pData = NULL;
	hr = GetImmediateContext()->Map( m_pMemTexture, 0, D3D11_MAP_READ, 0, &map );
	HRESULT hrMemTexMap = hr;
	if( FAILED( hr ) || !map.pData)
	{
		WriteToLog( _T( "m_pMemTexture->Map() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto testDone;
	}
	UINT x = 0, y = 0;
	UINT cut = m_cutInterval;
	UINT i;
	UINT vertCount;
	UINT soIdx = 0;
	UINT soSize = 1;
	switch( m_topology )
	{
		case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
			soSize = 1;
			for( i = 0; i < m_numIndices; ++i )
			{
				if( cut )
				{
					if( --cut == 0 )
					{
						cut = m_cutInterval;
						if (m_bRasterize)
							tr = TRLatch(tr, TestPixelOff( x, y, (PIXEL *) map.pData, map.RowPitch ));
						++x;
						continue;
					}
				}
				if (m_bRasterize)
					tr = TRLatch(tr, TestPixelOn( x, y, (PIXEL *) map.pData, map.RowPitch ));
				if (m_bStreamOut)
					tr = TRLatch(tr, TestSOPoint(soIdx, (m_bUseGS?soIdx:-1), x, y, pSOVerts));
				soIdx += 1;
				++x;
			}
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
			soSize = 2;
			vertCount = 2;
			for( i = 0; i < m_numIndices; ++i )
			{
				if( cut )
				{
					if( --cut == 0 )
					{
						cut = m_cutInterval;
						if (m_bRasterize)
							tr = TRLatch(tr, TestPixelOff( x, y, (PIXEL *) map.pData, map.RowPitch ));
						++x;
						vertCount = 2;
						continue;
					}
				}
				if( --vertCount == 0 )
				{
					vertCount = 2;
					if (m_bRasterize)
						tr = TRLatch(tr, TestPixelOn( x, y, (PIXEL *) map.pData, map.RowPitch ));
					if (m_bStreamOut)
						tr = TRLatch(tr, TestSOLine(soIdx, (m_bUseGS?soIdx/2:-1), x, y, pSOVerts));
					soIdx += 2;
					++x;
				}
			}
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
			soSize = 2;
			vertCount = 2;
			for( i = 0; i < m_numIndices; ++i )
			{
				if( cut )
				{
					if( --cut == 0 )
					{
						cut = m_cutInterval;
						if (m_bRasterize)
							tr = TRLatch(tr, TestPixelOff( x, y, (PIXEL *) map.pData, map.RowPitch ));
						++x;
						vertCount = 2;
						continue;
					}
				}
				if( --vertCount == 0 )
				{
					vertCount = 1;
					if (m_bRasterize)
						tr = TRLatch(tr, TestPixelOn( x, y, (PIXEL *) map.pData, map.RowPitch ));
					if (m_bStreamOut)
						tr = TRLatch(tr, TestSOLine(soIdx, (m_bUseGS?soIdx/2:-1), x, y, pSOVerts));
					soIdx += 2;
					++x;
				}
			}
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			soSize = 3;
			vertCount = 3;
			y = 1;
			for( i = 0; i < m_numIndices; ++i )
			{
				if( cut )
				{
					if( --cut == 0 )
					{
						cut = m_cutInterval;
						if (m_bRasterize)
							tr = TRLatch(tr, TestPixelOff( x, y, (PIXEL *) map.pData, map.RowPitch ));
						++x;
						y = 1;
						vertCount = 3;
						continue;
					}
				}
				if( --vertCount == 0 )
				{
					vertCount = 3;
					if (m_bRasterize)
						tr = TRLatch(tr, TestPixelOn( x, y, (PIXEL *) map.pData, map.RowPitch ));
					if (m_bStreamOut)
						tr = TRLatch(tr, TestSOTriangle(soIdx, (m_bUseGS?soIdx/3:-1), x, y, pSOVerts));
					soIdx += 3;
					if( y == 0 )
					{
						y = 1;
						++x;
					}
					else
					{
						y = 0;
					}
				}
			}
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
			soSize = 3;
			vertCount = 3;
			y = 1;
			for( i = 0; i < m_numIndices; ++i )
			{
				if( cut )
				{
					if( --cut == 0 )
					{
						cut = m_cutInterval;
						if (m_bRasterize)
							tr = TRLatch(tr, TestPixelOff( x, y, (PIXEL *) map.pData, map.RowPitch ));
						++x;
						y = 1;
						vertCount = 3;
						continue;
					}
				}
				if( --vertCount == 0 )
				{
					vertCount = 1;
					if (m_bRasterize)
						tr = TRLatch(tr, TestPixelOn( x, y, (PIXEL *) map.pData, map.RowPitch ));
					if (m_bStreamOut)
						tr = TRLatch(tr, TestSOTriangle(soIdx, (m_bUseGS?soIdx/3:-1), x, y, pSOVerts));
					soIdx += 3;
					if( y == 0 )
					{
						y = 1;
						++x;
					}
					else
					{
						y = 0;
					}
				}
			}
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
			soSize = 2;
			vertCount = 4;
			x = 1;
			for( i = 0; i < m_numIndices; ++i )
			{
				if( cut )
				{
					if( --cut == 0 )
					{
						cut = m_cutInterval;
						if (m_bRasterize)
							tr = TRLatch(tr, TestPixelOff( x, y, (PIXEL *) map.pData, map.RowPitch ));
						++x;
						vertCount = 4;
						continue;
					}
				}
				if( --vertCount == 0 )
				{
					vertCount = 4;
					if (m_bRasterize)
						tr = TRLatch(tr, TestPixelOn( x, y, (PIXEL *) map.pData, map.RowPitch ));
					if (m_bStreamOut)
						tr = TRLatch(tr, TestSOLine(soIdx, (m_bUseGS?soIdx/2:-1), x, y, pSOVerts));
					soIdx += 2;
					++x;
				}
			}
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
			soSize = 2;
			vertCount = 4;
			x = 1;
			for( i = 0; i < m_numIndices; ++i )
			{
				if( cut )
				{
					if( --cut == 0 )
					{
						cut = m_cutInterval;
						if (m_bRasterize)
							tr = TRLatch(tr, TestPixelOff( x, y, (PIXEL *) map.pData, map.RowPitch ));
						++x;
						vertCount = 4;
						continue;
					}
				}
				if( --vertCount == 0 )
				{
					vertCount = 1;
					if (m_bRasterize)
						tr = TRLatch(tr, TestPixelOn( x, y, (PIXEL *) map.pData, map.RowPitch ));
					if (m_bStreamOut)
						tr = TRLatch(tr, TestSOLine(soIdx, (m_bUseGS?soIdx/2:-1), x, y, pSOVerts));
					soIdx += 2;
					++x;
				}
			}
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
			soSize = 3;
			vertCount = 6;
			x = 1;
			y = 1;
			for( i = 0; i < m_numIndices; ++i )
			{
				if( cut )
				{
					if( --cut == 0 )
					{
						cut = m_cutInterval;
						if (m_bRasterize)
							tr = TRLatch(tr, TestPixelOff( x, y, (PIXEL *) map.pData, map.RowPitch ));
						++x;
						y = 1;
						vertCount = 6;
						continue;
					}
				}
				if( --vertCount == 0 )
				{
					vertCount = 6;
					if (m_bRasterize)
						tr = TRLatch(tr, TestPixelOn( x, y, (PIXEL *) map.pData, map.RowPitch ));
					if (m_bStreamOut)
						tr = TRLatch(tr, TestSOTriangle(soIdx, (m_bUseGS?soIdx/3:-1), x, y, pSOVerts));
					soIdx += 3;
					if( y == 0 )
					{
						y = 1;
						++x;
					}
					else
					{
						y = 0;
					}
				}
			}
			break;
			
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
			soSize = 3;
			vertCount = 6;
			x = 1;
			y = 1;
			for( i = 0; i < m_numIndices; ++i )
			{
				if( cut )
				{
					if( --cut == 0 )
					{
						cut = m_cutInterval;
						if (m_bRasterize)
							tr = TRLatch(tr, TestPixelOff( x, y, (PIXEL *) map.pData, map.RowPitch ));
						++x;
						y = 1;
						vertCount = 6;
						continue;
					}
				}
				if( --vertCount == 0 )
				{
					vertCount = 2;
					if (m_bRasterize)
						tr = TRLatch(tr, TestPixelOn( x, y, (PIXEL *) map.pData, map.RowPitch ));
					if (m_bStreamOut)
						tr = TRLatch(tr, TestSOTriangle(soIdx, (m_bUseGS?soIdx/3:-1), x, y, pSOVerts));
					soIdx += 3;
					if( y == 0 )
					{
						y = 1;
						++x;
					}
					else
					{
						y = 0;
					}
				}
			}
			break;
	}

	UINT64 numInputPrimitives = 0;
	switch (m_topology)
	{
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		numInputPrimitives = m_numIndices;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		numInputPrimitives = m_numIndices / 2;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		numInputPrimitives = m_numIndices - 1;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		numInputPrimitives = m_numIndices / 3;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		numInputPrimitives = m_numIndices - 2;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		numInputPrimitives = m_numIndices / 4;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		numInputPrimitives = m_numIndices - 2;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		numInputPrimitives = m_numIndices / 6;
		break;

	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		numInputPrimitives = (m_numIndices / 2) - 2;
		break;
	}
	
	bool bTestPipelineStats = GetFramework()->GetExecutionPriorityLevel() != 1;
	if (bTestPipelineStats)
	{
		const UINT64 numUncutPrimitives = (soIdx / soSize);
		if (m_bUseGS && pipelineStats.GSPrimitives != numUncutPrimitives)
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "Wrong number of GS primitives output. Expected %I64d, Found %I64d." ), numUncutPrimitives, pipelineStats.GSPrimitives );
		}
		if (m_bUseGS && pipelineStats.GSInvocations != numUncutPrimitives)
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "Wrong number of GS Invocations. Expected %I64d, Found %I64d." ), numUncutPrimitives, pipelineStats.GSInvocations );
		}
		if (pipelineStats.IAPrimitives < numUncutPrimitives || pipelineStats.IAPrimitives > numInputPrimitives)
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "Wrong number of IA Primitives. Expected %I64d-%I64d, Found %I64d." ), numUncutPrimitives, numInputPrimitives, pipelineStats.IAPrimitives );
		}
		if (pipelineStats.IAVertices > m_numIndices)
		{
			tr = RESULT_FAIL; 
			WriteToLog( _T( "Wrong number of IA Vertices. Expected %I64d, Found %I64d." ), (UINT64)m_numIndices, pipelineStats.IAVertices );
		}
	}

	GetFramework()->Present( NULL, NULL, NULL, NULL, 0, 0 );

testDone:
	pDeviceContext->OMSetRenderTargets(1, &pCachedRenderTarget, pCachedDepthStencil);
	SAFE_RELEASE(pCachedRenderTarget);
	SAFE_RELEASE(pCachedDepthStencil);
	if (SUCCEEDED(hrMemTexMap))
		GetImmediateContext()->Unmap( m_pMemTexture, 0 );
	if (pTempBuffer && SUCCEEDED(hrTempBufferMap))
		GetImmediateContext()->Unmap( pTempBuffer, 0 );
	SAFE_RELEASE(pTempBuffer);
	SAFE_RELEASE(pPipelineQuery);
	
	return tr;
}
