/********************************************************************
	created:	2005/08/12
	filename: 	shapelib.h
	author:		billkris
	
	purpose:	Library of 3D geometric shapes providing
				vertex coordinates and indices (including adjacency). 
*********************************************************************/

#pragma once

class CShape
{
public:
	typedef float VECTOR[3];
	
	CShape();
	virtual ~CShape();

	const VECTOR *GetVertexPositions() const { return m_positions; }

	const VECTOR *GetVertexNormals() const { return m_normals; }

	int GetVertexPositionCount() const { return m_positionCount; }

	int GetVertexNormalCount() const { return m_normalCount; }

	int GetIndexCount() const { return m_indexCount; }

	const int *GetPositionIndices() const { return m_positionIndices; }

	const int *GetNormalIndices() const { return m_normalIndices; }

	const int *GetAdjacentIndices() const { return m_adjacentIndices; }

	void GetTriangleNormal( VECTOR &vout, int offset );
	
	// MakeFacted()
	//
	// Recomputes normals and normalIndices so that each face's vertices
	// have the same normal.
	// NOTE: Any adjacency information is destroyed
	// by this call and must be recreated if needed.
	void MakeFaceted();
	
	// MakeSmooth()
	//
	// Recomputes vertices and indices to that all vertices
	// sharing the same position have a normal equal to the
	// average face normal of all triangles connected to the
	// vertex.
	// NOTE: Any adjacency information is destroyed
	// by this call and must be recreated if needed.
	void MakeSmooth();
	
	// ConvergeVertexIndices()
	//
	// Recomputes the positions and normals such that
	// the position indices and normal indices are in
	// parity.  Useful for creating a single array
	// of vertex structures.
	// NOTE: Any adjacency information is destroyed
	// by this call and must be recreated if needed.
	// NOTE: Any adjacency information is destroyed
	// by this call and must be recreated if needed.
	void ConvergeVertexIndices();
	
	// DivergeVertexIndices()
	//
	// Reverse of ConvergeVertexIndices()
	// NOTE: Any adjacency information is destroyed
	// by this call and must be recreated if needed.
	void DivergeVertexIndices();
	
	// MakeTriangleList()
	//
	// Rearranges positions and normals so that
	// indexing is not needed.
	// NOTE: Any adjacency information is destroyed
	// by this call and must be recreated if needed.
	void MakeTriangleList();
	
	// BuildAdjacentIndices()
	//
	// Builds adjacent index array from the shape
	// in the current form.  Changing the shape
	// by calling MakeFaceted(), MakeSmooth(),
	// ConvergeVertexIndices(), etc... will invalidate
	// adjacency data.
	// Returns the number of unshared vertices.
	int BuildAdjacentIndices();
	
	// Invert
	//
	// Reverse the winding order of the triangles
	// by turning the position, normal, and index
	// buffers "upside-down".  This way triangle
	// list and triangle strip shapes are preserved.
	void Invert();

//////////////////////////////////////////////////////////////////////////
// Helper utility functions
//////////////////////////////////////////////////////////////////////////
	
	static void VectorAdd( VECTOR &vout, const VECTOR &v0, const VECTOR &v1 );
	static void VectorSubtract( VECTOR &vout, const VECTOR &v0, const VECTOR &v1 );
	static void VectorCrossProd( VECTOR &vout, const VECTOR &v0, const VECTOR &v1 );
	static float VectorDotProd( const VECTOR &v0, const VECTOR &v1 );
	static bool VectorAlmostEqual( const VECTOR &v0, const VECTOR &v1, float epsilon = 0.00001f );
	static float VectorNormalize( VECTOR &vout, const VECTOR &v );
	static void VectorCopy( VECTOR &vout, const VECTOR &v );
	static void VectorScale( VECTOR &vout, const VECTOR &v, float scale );
	static void VectorMultiply( VECTOR &vout, const VECTOR &v0, const VECTOR &v1 );

protected:
	VECTOR *m_positions;
	VECTOR *m_normals;
	int *m_positionIndices;
	int *m_normalIndices;
	int m_positionCount;
	int m_normalCount;
	int m_indexCount;

	int *m_adjacentIndices;
};

class CTetrahedron : public CShape
{
public:
	CTetrahedron();
};

class CCube : public CShape
{
public:
	CCube();
};

class COctahedron : public CShape
{
public:
	COctahedron();
};

class CDodecahedron : public CShape
{
public:
	CDodecahedron();
};

class CIcosahedron : public CShape
{
public:
	CIcosahedron();
};

class CSphere : public CShape
{
public:
	CSphere( float radius, int stacks, int slices );
};

class CGeoSphere : public CShape
{
public:
	enum BASE_SHAPE
	{
		OCTAHEDRON,
		ICOSAHEDRON,
	};
	
	CGeoSphere( float radius, BASE_SHAPE baseShape, int subdivisions );
	
	// SubdivideTriangles
	//
	// Subdivides a triangle into four separate triangles intersecting
	// the input triangle at the center of each edge.
	//
	//        v0
	//        /\
	//       /  \
	//      /____\
	//     /\    /\
	//    /  \  /  \
	//   /____\/____\
	// v2            v1
	//
	// The buffers pointed to by pOutputTriangleIndices and pOutputVerts must be allocated by
	// the user.  
	// It is assumed that the buffer pointed to by pOutputTriangleIndices is 4 * numSourceIndices in size.
	// It is assumed that the buffer pointed to by pOutputVerts is 2 * numSourceVerts in size.
	static void SubdivideTriangles( int *pOutputTriangleIndices, VECTOR *pOutputVerts, const int *pSourceTriangleIndices, const VECTOR *pSourceVerts, int numSourceIndices, int numSourceVerts );
};

class CTorus : public CShape
{
public:
	CTorus( float innerRadius, float outerRadius, int sides, int rings );
};

class CTeapot : public CShape
{
public:
	CTeapot();
};

