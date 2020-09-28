//
//  CShapes.h
//
//  Header file for CShapes class.
//

// ============================================================================

#ifndef _CSHAPES_H_
#define _CSHAPES_H_

#include <windows.h>
#include "cd3d.h"
#include "3DMath.h"

#define CS_NSHAPES 9
enum CS_SHAPES {
	CS_COMPOSITE,
	CS_BOX,
	CS_SPHERE,
	CS_PYRAMID,
	CS_CYLINDER,
	CS_CONE,
	CS_MESH,
	CS_BOID,
    CS_ICOSOHEDRON,
	CS_TEAPOT
};

class CShapes
{
public: // public data members
	D3DVERTEX *      m_pVertices;
	WORD *           m_pIndices;
	int              m_nVertices;
	int              m_nIndices;
	D3DVECTOR *      m_pCenter;    // the object's "center"
	CS_SHAPES        m_CurShape;
	bool             m_bWrapU;    // should D3DRENDERSTATE_WRAPU be used?
	bool             m_bWrapV;    // should D3DRENDERSTATE_WRAPV be used?

protected: // helper functions
	void Initialize();
	void Cleanup();

	// shape generation
	bool GenerateBox();
	bool GenerateSphere(const int nDivisions = 24);
	bool GeneratePyramid();
	bool GenerateCylinder(const int nDivisions = 16, const int hDivisions = 1);
	bool GenerateCone(const int nDivisions = 16);
	bool GenerateMesh(const int xDivisions = 1, const int yDivisions = 1);
	bool GenerateBoid();
    bool GenerateIcosohedron();
	bool GenerateTeapot();

public: // constructors / destructors / operators
	CShapes();
	CShapes(const CShapes &Shape); // copy constructor
	~CShapes();
	CShapes &operator=(const CShapes &Shape);
    CShapes &operator+=(const CShapes &Shape);
	bool AddShape(const CShapes &Shape);
	bool CopyShape(const CShapes &Shape);
	
public: // member functions
	bool NewShape(
		CS_SHAPES Type,
		const int Divisions1 = 0,
		const int Divisions2 = 0,
		const int Divisions3 = 0);

	// shape manipulations
	void Rotate(float x, float y, float z);
	void Translate(float x, float y, float z);
	void Scale(float factor); // uniform scaling
	void Scale(float x, float y, float z); // non-uniform scaling
	void Orbit(D3DVECTOR point1, float xRot, float yRot, float ZRot);
	void Fit(D3DVECTOR point1, D3DVECTOR point2);
	void MoveTo(D3DVECTOR point1);
	void Transform(D3DMATRIX& Matrix, bool bTransformNormals);

	// misc functions
	PTSTR GetShapeName();
};

#endif