/********************************************************************
	created:	2005/08/12
	filename: 	shapelib.cpp
	author:		billkris
	
	purpose:	
*********************************************************************/

#include "shapelib.h"
#include <assert.h>
#include <math.h>
#include <memory.h>

void CShape::VectorSubtract( VECTOR &vout, const VECTOR &v0, const VECTOR &v1 )
{
	vout[0] = v1[0] - v0[0];
	vout[1] = v1[1] - v0[1];
	vout[2] = v1[2] - v0[2];
}

void CShape::VectorAdd( VECTOR &vout, const VECTOR &v0, const VECTOR &v1 )
{
	vout[0] = v1[0] + v0[0];
	vout[1] = v1[1] + v0[1];
	vout[2] = v1[2] + v0[2];
}

void CShape::VectorCrossProd( VECTOR &vout, const VECTOR &v0, const VECTOR &v1 )
{
	vout[0] = v0[1] * v1[2] - v0[2] * v1[1];
	vout[1] = v0[2] * v1[0] - v0[0] * v1[2];
	vout[2] = v0[0] * v1[1] - v0[1] * v1[0];
}

float CShape::VectorDotProd( const VECTOR &v0, const VECTOR &v1 )
{
	return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

bool CShape::VectorAlmostEqual( const VECTOR &v0, const VECTOR &v1, float epsilon )
{
	return( 
		fabsf( v0[0] - v1[0] ) <= epsilon &&
		fabsf( v0[1] - v1[1] ) <= epsilon &&
		fabsf( v0[2] - v1[2] ) <= epsilon );
}

float CShape::VectorNormalize( VECTOR &vout, const VECTOR &v )
{
	float dist = VectorDotProd( v, v );

	if( dist > 0 )
	{
		dist = sqrtf( dist );
		float invdist = 1.f / dist;
		vout[0] = v[0] * invdist;
		vout[1] = v[1] * invdist;
		vout[2] = v[2] * invdist;
	}
	
	return dist;
}

void CShape::VectorCopy( VECTOR &vout, const VECTOR &v )
{
	vout[0] = v[0];
	vout[1] = v[1];
	vout[2] = v[2];
}

void CShape::VectorScale( VECTOR &vout, const VECTOR &v, float scale )
{
	vout[0] = v[0] * scale;
	vout[1] = v[1] * scale;
	vout[2] = v[2] * scale;
}

void CShape::VectorMultiply( VECTOR &vout, const VECTOR &v0, const VECTOR &v1 )
{
	vout[0] = v0[0] * v1[0];
	vout[1] = v0[1] * v1[1];
	vout[2] = v0[2] * v1[2];
}

CShape::CShape() :
	m_positions( 0 ),
	m_normals( 0 ),
	m_positionIndices( 0 ),
	m_adjacentIndices( 0 ),
	m_normalIndices( 0 )
{
}

CShape::~CShape()
{
	delete[]( m_positions );
	delete[]( m_positionIndices );
	delete[]( m_normals );
	delete[]( m_adjacentIndices );
}


void CShape::MakeFaceted()
{
	int i, j;
	
	int triCount = m_indexCount / 3;
	VECTOR *normals = new VECTOR[triCount];
	int *indices = new int[m_indexCount];
	int normalCount = 0;
	
	for( j = triCount - 1, i = m_indexCount - 3; i >= 0; --j, i -= 3 )
	{
		// Calculate the triangle normal
		GetTriangleNormal( normals[j], i );
		VectorNormalize( normals[j], normals[j] );
		
		indices[i] = 
		indices[i + 1] = 
		indices[i + 2] = j;
	}
	
	normalCount = triCount;
	
	// Optimize the normals by combining duplicates
	for( i = 0; i < normalCount; ++i )
	{
		VECTOR &n = normals[i];
		
		for( j = i + 1; j < normalCount; )
		{
			if( VectorAlmostEqual( n, normals[j], 0 ) )
			{
				// Replace vector with vector in last position.
				--normalCount;
				if( j < normalCount )
					VectorCopy( normals[j], normals[normalCount] );
				
				// Fixup the normal indices
				for( int k = 0; k < m_indexCount; ++k )
				{
					if( indices[k] == j )
						indices[k] = i;
					else	
					if( indices[k] == normalCount )
						indices[k] = j;
				}
			}
			else
			{
				++j;
			}
		}
	}
	
	// Replace the normals and normal indices
	delete[]( m_normals );
	delete[]( m_normalIndices );
	
	m_normals = normals;
	m_normalIndices = indices;
	m_normalCount = normalCount;
}


void CShape::MakeSmooth()
{
	int i, j;
	
	int triCount = m_indexCount / 3;
	VECTOR *normals = new VECTOR[m_positionCount];
	int *indices = new int[m_indexCount];
	
	// Initialize vertex normals to zero
	memset( normals, 0, sizeof( VECTOR ) * triCount );
	
	for( i = m_indexCount - 3; i >= 0; i -= 3 )
	{
		VECTOR normal;
		
		// Calculate the triangle normal
		GetTriangleNormal( normal, i );
		
		// Add the normal to each of the triangle vertices
		VectorAdd( normals[m_positionIndices[i]], normals[m_positionIndices[i]], normal );
		VectorAdd( normals[m_positionIndices[i + 1]], normals[m_positionIndices[i + 1]], normal );
		VectorAdd( normals[m_positionIndices[i + 2]], normals[m_positionIndices[i + 2]], normal );
		
		// Copy position indices into normal indices array
		indices[i] = m_positionIndices[i];
		indices[i + 1] = m_positionIndices[i + 1];
		indices[i + 2] = m_positionIndices[i + 2];
	}
	
	// Normalize each of the normals
	for( i = 0; i < m_positionCount; ++i )
		VectorNormalize( normals[i], normals[i] );
		
	delete[]( m_normals );
	delete[]( m_normalIndices );
	delete[]( m_adjacentIndices );
	m_adjacentIndices = 0;
		
	m_normals = normals;
	m_normalIndices = indices;
	m_normalCount = m_positionCount;
}


void CShape::ConvergeVertexIndices()
{
	if( m_indexCount <= 0 || m_positionCount <= 0 || m_normalCount <= 0 )
		return;
		
	int worstCaseVertexCount = m_indexCount;
	
	struct IndexMapRecord
	{
		int oldnormalindex;
		int next;
	};
	
	// Create temporary position and normal arrays
	VECTOR *positions = new VECTOR[worstCaseVertexCount];
	VECTOR *normals = new VECTOR[worstCaseVertexCount];
	
	// Create temporary index map
	IndexMapRecord *indexMap = new IndexMapRecord[worstCaseVertexCount];
	
	int vertexCount = m_positionCount;

	// Copy position array	
	memset( indexMap, -1, sizeof( IndexMapRecord ) * worstCaseVertexCount );

	for( int i = 0; i < m_indexCount; ++i )
	{
		int pi = m_positionIndices[i];
		int ni = m_normalIndices[i];
		int vi = pi;
		
		for( ; indexMap[vi].oldnormalindex != ni; )
		{
			if( indexMap[vi].oldnormalindex == -1 )
			{
				indexMap[vi].oldnormalindex = ni;
				indexMap[vi].next = -1;
				
				VectorCopy( normals[vi], m_normals[ni] );
				VectorCopy( positions[vi], m_positions[pi] );
				
				break;
			}
			
			if( indexMap[vi].next == -1 )
			{
				indexMap[vi].next = vertexCount;
				vertexCount++;
			}
			
			vi = indexMap[vi].next;
		}
		
		m_normalIndices[i] =
		m_positionIndices[i] = vi;
	}
	
	delete[]( m_positions );
	delete[]( m_normals );
	delete[]( m_adjacentIndices );
	m_adjacentIndices = 0;
	
	m_positions = new VECTOR[vertexCount];
	m_normals = new VECTOR[vertexCount];
	m_positionCount = vertexCount;
	m_normalCount = vertexCount;
	memcpy( m_positions, positions, vertexCount * sizeof( VECTOR ) );
	memcpy( m_normals, normals, vertexCount * sizeof( VECTOR ) );
	delete[]( positions );
	delete[]( normals );
	delete[]( indexMap );
}


void CShape::MakeTriangleList()
{
	VECTOR *positions = new VECTOR[m_indexCount];
	VECTOR *normals = new VECTOR[m_indexCount];
	
	for( int i = 0; i < m_indexCount; ++i )
	{
		VectorCopy( positions[i], m_positions[m_positionIndices[i]] );
		VectorCopy( normals[i], m_normals[m_normalIndices[i]] );
		m_positionIndices[i] =
		m_normalIndices[i] = i;
	}
	
	delete[]( m_positions );
	delete[]( m_normals );
	m_positionCount = m_indexCount;
	m_normalCount = m_indexCount;
	m_positions = positions;
	m_normals = normals;
}


void CShape::Invert()
{
	int i, j;
	VECTOR tempVec;
	
	// Swap position vectors at both ends
	for( i = m_positionCount / 2, j = m_positionCount - i; --i >= 0; ++j  )
	{
		VectorCopy( tempVec, m_positions[i] );
		VectorCopy( m_positions[i], m_positions[j] );
		VectorCopy( m_positions[j], tempVec );
	}
	
	// Swap normal vectors at both ends (also invert normals)
	for( i = m_normalCount / 2, j = m_normalCount - i; --i >= 0; ++j  )
	{
		VectorScale( tempVec, m_normals[i], -1 );
		VectorScale( m_normals[i], m_normals[j], -1 );
		VectorCopy( m_normals[j], tempVec );
	}
	
	// Make sure the "middle" normal vector gets negated
	if( m_normalCount & 0x01 )
	{
		i = m_normalCount / 2 + 1;
		VectorScale( m_normals[i], m_normals[i], -1 );
	}

	int *positionIndices = new int[m_indexCount];
	int *normalIndices = new int[m_indexCount];
		
	// Invert position indices
	for( j = 0, i = m_indexCount - 1; i >= 0; --i, ++j )
	{
		positionIndices[j] = m_positionCount - m_positionIndices[i] - 1;
	}
	
	// Invert normal indices
	for( j = 0, i = m_indexCount - 1; i >= 0; --i, ++j )
	{
		normalIndices[j] = m_normalCount - m_normalIndices[i] - 1;
	}
	
	delete( m_positionIndices );
	delete( m_normalIndices );
	
	m_positionIndices = positionIndices;
	m_normalIndices = normalIndices;
	
	// Invert adjacent indices
	if( m_adjacentIndices )
	{
		for( i = m_indexCount - 1; i >= 0; --i )
		{
			m_adjacentIndices[i] = m_positionCount - m_adjacentIndices[i] - 1;
		}
	}
}


void CShape::GetTriangleNormal( VECTOR &vout, int offset )
{
	VECTOR s0, s1;
	VectorSubtract( s0, m_positions[m_positionIndices[offset + 1]], m_positions[m_positionIndices[offset]] );
	VectorSubtract( s1, m_positions[m_positionIndices[offset + 2]], m_positions[m_positionIndices[offset]] );
	VectorCrossProd( vout, s0, s1 );
}


int CShape::BuildAdjacentIndices()
{
	int unsharedTotal = 0;
	int numTriangles = m_indexCount / 3;
	if( m_indexCount < 3 )
		return m_indexCount;
		
	assert( numTriangles * 3 == m_indexCount ); // Make sure this is a complete list of triangles
	
	int *adjacentIndices = new int[m_indexCount];
	
	// Iterate through the triangle list and searching for shared edges
	for( int i = 0; i < m_indexCount; i += 3 )
	{
		// Iterate over triangle edges
		for( int n = 0; n < 3; ++n )
		{
			int unsharedCount = 3;
			int p1 = m_positionIndices[i + n];
			int p2 = m_positionIndices[(i + n + 1) % 3];
			
			// Initialize adjacent indices as degenerate
			// by duplicating one of the corresponding edge indices.
			// This handles the case where there is no adjacent triangle.			
			adjacentIndices[i + n] = p1;
		
			// Find the first triangle with at least one shared edge.
			for( int j = 0; j < m_indexCount && unsharedCount > 0; j += 3 )
			{
				for( int m = 0; m < 3; ++m )
				{
					// First see of the first vertex position matches.
					if( VectorAlmostEqual( m_positions[p2], m_positions[m_positionIndices[j + m]], 0 ) )
					{
						// Now see of the second index matches.
						if( VectorAlmostEqual( m_positions[p1], m_positions[m_positionIndices[(j + m + 1) % 3]], 0 ) )
						{
							// This is a shared edge.  Set the adjacent index
							// to the non-shared vertex of the adjacent triangle.
							adjacentIndices[i + n] = m_positionIndices[(j + m + 2) % 3];
							unsharedCount--;
							break;
						}
					}
				}
			}
			
			unsharedTotal += unsharedCount;
		}
	}
	
	delete( m_adjacentIndices );
	m_adjacentIndices = adjacentIndices;
	
	return unsharedTotal;
}


CTetrahedron::CTetrahedron()
{
	static const VECTOR coords[4] = 
	{
		{  1,  1,  1 },
		{  1, -1, -1 },
		{ -1,  1, -1 },
		{ -1, -1,  1 },		
	};
	
	static const int indices[12] = 
	{
		0, 1, 2,
		2, 3, 0,
		0, 3, 1,
		1, 3, 2
	};
	
	static const int adjacent[12] =
	{
		3, 3, 3,
		1, 1, 1,
		2, 2, 2,
		0, 0, 0
	};
	
	m_positions = new VECTOR[sizeof( coords ) / sizeof( coords[0] )];
	m_positionIndices = new int[sizeof( indices ) / sizeof( indices[0] )];
	memcpy( m_positions, coords, sizeof( coords ) );
	memcpy( m_positionIndices, indices, sizeof( indices ) );
	
	m_positionCount = 4;
	m_indexCount = 3 * 4;
	
	MakeFaceted();
}

CCube::CCube()
{
	static const VECTOR coords[8] =
	{
		{  1,  1,  1 },
		{  1,  1, -1 },
		{  1, -1,  1 },
		{  1, -1, -1 },
		{ -1,  1,  1 },
		{ -1,  1, -1 },
		{ -1, -1,  1 },
		{ -1, -1, -1 }
	};
	
	static const int indices[3 * 2 * 6] =
	{
		0, 2, 3, 3, 1, 0,
		1, 3, 7, 7, 5, 1,
		5, 7, 6, 6, 4, 5,
		4, 6, 2, 2, 0, 4,
		1, 5, 4, 4, 0, 1,
		2, 6, 7, 7, 3, 2,
	};
	
	m_positions = new VECTOR[sizeof( coords ) / sizeof( coords[0] )];
	m_positionIndices = new int[sizeof( indices ) / sizeof( indices[0] )];
	memcpy( m_positions, coords, sizeof( coords ) );
	memcpy( m_positionIndices, indices, sizeof( indices ) );
	
	m_positionCount = 8;
	m_indexCount = 3 * 2 * 6;
	
	MakeFaceted();
}

COctahedron::COctahedron()
{
	static const VECTOR coords[6] =
	{
		{  1,  0,  0 },
		{ -1,  0,  0 },
		{  0,  1,  0 },
		{  0, -1,  0 },
		{  0,  0,  1 },
		{  0,  0, -1 }
	};
	
	static const int indices[3 * 8] =
	{
		0, 5, 2,
		5, 1, 2,
		1, 4, 2,
		4, 0, 2,
		5, 0, 3,
		1, 5, 3,
		4, 1, 3,
		0, 4, 3,
	};
	
	static const int adjacent[3 * 8] =
	{
		3, 1, 4,
		3, 4, 0,
		3, 0, 5, 
		3, 5, 1,
		2, 4, 1,
		2, 0, 5,
		2, 5, 1,
		2, 1, 4,
	};
	
	m_positions = new VECTOR[sizeof( coords ) / sizeof( coords[0] )];
	m_positionIndices = new int[sizeof( indices ) / sizeof( indices[0] )];
	memcpy( m_positions, coords, sizeof( coords ) );
	memcpy( m_positionIndices, indices, sizeof( indices ) );
	
	m_positionCount = 6;
	m_indexCount = 3 * 8;
	
	MakeFaceted();
};


CDodecahedron::CDodecahedron()
{
	assert( !"Not yet implemented.  If you need this feel free to write the code yourself" );
}


CIcosahedron::CIcosahedron()
{
	static const float sqrt5 = 2.2360679774997896964091736687313f;
	static const float a = .5f;
	static const float phi = ( 1 + sqrt5 ) / 2.f;
	static const float b = 1.f / ( 2.f * phi );
	
	static const VECTOR coords[12] =
	{
		{ 0, b, -a },	// ZPBNA,
		{ b, a, 0 },	// PBPAZ,
		{ -b, a, 0 },	// NBPAZ,
		{ 0, b, a },	// ZPBPA,
		{ 0, -b, a },	// ZNBPA,
		{ -a, 0, b },	// NAZPB,
		{ a, 0, b },	// PAZPB,
		{ 0, -b, -a },	// ZNBNA,
		{ a, 0, -b },	// PAZNB,
		{ -a, 0, -b },	// NAZNB,
		{ b, -a, 0 },	// PBNAZ,
		{ -b, -a, 0 },	// NBNAZ,
	};

	enum IndexNames
	{
		ZPBNA,
		PBPAZ,
		NBPAZ,
		ZPBPA,
		ZNBPA,
		NAZPB,
		PAZPB,
		ZNBNA,
		PAZNB,
		NAZNB,
		PBNAZ,
		NBNAZ,
	};
	
	static const int indices[3 * 20] =
	{
		ZPBNA,	PBPAZ,	NBPAZ,
		ZPBPA,	NBPAZ,	PBPAZ,
		ZPBPA,	ZNBPA,	NAZPB,
		ZPBPA,	PAZPB,	ZNBPA,
		ZPBNA,	ZNBNA,	PAZNB,
		ZPBNA,	NAZNB,	ZNBNA,
		ZNBPA,	PBNAZ,	NBNAZ,
		ZNBNA,	NBNAZ,	PBNAZ,
		NBPAZ,	NAZPB,	NAZNB,
		NBNAZ,	NAZNB,	NAZPB,
		PBPAZ,	PAZNB,	PAZPB,
		PBNAZ,	PAZPB,	PAZNB,
		ZPBPA,	NAZPB,	NBPAZ,
		ZPBPA,	PBPAZ,	PAZPB,
		ZPBNA,	NBPAZ,	NAZNB,
		ZPBNA,	PAZNB,	PBPAZ,
		ZNBNA,	NAZNB,	NBNAZ,
		ZNBNA,	PBNAZ,	PAZNB,
		ZNBPA,	NBNAZ,	NAZPB,
		ZNBPA,	PAZPB,	PBNAZ,
	};
	
	static int adjacent[3 * 20] = {0};
	
	m_positions = new VECTOR[sizeof( coords ) / sizeof( coords[0] )];
	m_positionIndices = new int[sizeof( indices ) / sizeof( indices[0] )];
	memcpy( m_positions, coords, sizeof( coords ) );
	memcpy( m_positionIndices, indices, sizeof( indices ) );
	
	m_positionCount = 12;
	m_indexCount = 3 * 20;
	
	MakeFaceted();
}










