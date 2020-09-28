//
//  CShapes.cpp
//
//  Source for CShapes class
//

#include "CShapesPCH.h"

#include "CShapes.h"
#include "d3d.h"
#include <tchar.h>

// ============================================================================
//
//  HELPER FUNCTIONS
//
// ============================================================================

void CShapes::Initialize()
{
	m_pVertices = 0;
	m_pIndices  = 0;
	m_nVertices = 0;
	m_nIndices  = 0;
	m_pCenter = NULL;
	
	m_pCenter = new D3DVECTOR();
	if(m_pCenter)
	{
		m_pCenter->x = 0;
		m_pCenter->y = 0;
		m_pCenter->z = 0;
	}
	else
		OutputDebugString(_T("Couldn't allocate memory.\n"));
}

// ----------------------------------------------------------------------------

void CShapes::Cleanup()
{
	if(m_pCenter)
	{
		delete m_pCenter;
		m_pCenter = NULL;
	}
	if(m_pVertices)
	{
		free(m_pVertices);
		m_pVertices = NULL;
	}
	if(m_pIndices)
	{
		free(m_pIndices);
		m_pIndices = NULL;
	}
}

// ----------------------------------------------------------------------------

bool CShapes::CopyShape(const CShapes &Shape)
{
	int i;

	if(!m_pCenter)
		return false;

	if(m_pVertices || m_pIndices)
	{
		Cleanup();
		Initialize();
	}

	//
	// allocate Vertices and Indices
	//
	m_pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * Shape.m_nVertices);
	m_pIndices = (WORD *)malloc(sizeof(WORD) * Shape.m_nIndices);
	
	if((!m_pVertices) || (!m_pIndices))
	{
		OutputDebugString("Couldn't allocate object memory");
		Cleanup();
		Initialize();
		return false;
	}

	m_nVertices = Shape.m_nVertices;
	m_nIndices = Shape.m_nIndices;

	//
	// copy the shape vertices and indices
	//
	for(i = 0; i < m_nVertices; i++)
	{
		m_pVertices[i].x = Shape.m_pVertices[i].x;
		m_pVertices[i].y = Shape.m_pVertices[i].y;
		m_pVertices[i].z = Shape.m_pVertices[i].z;
		m_pVertices[i].nx = Shape.m_pVertices[i].nx;
		m_pVertices[i].ny = Shape.m_pVertices[i].ny;
		m_pVertices[i].nz = Shape.m_pVertices[i].nz;
		m_pVertices[i].tu = Shape.m_pVertices[i].tu;
		m_pVertices[i].tv = Shape.m_pVertices[i].tv;
	}

	for(i = 0; i < m_nIndices; i++)
		m_pIndices[i] = Shape.m_pIndices[i];

	m_pCenter->x = Shape.m_pCenter->x;
	m_pCenter->y = Shape.m_pCenter->y;
	m_pCenter->z = Shape.m_pCenter->z;
	m_CurShape = Shape.m_CurShape;

	return true;
}

// ----------------------------------------------------------------------------

bool CShapes::AddShape(const CShapes &Shape)
{
	int nCurVtx = 0;
	int nCurIdx = 0;
	int i;
	D3DVERTEX * pVertices = NULL;
	WORD * pIndices = NULL;
	int nVertices, nIndices, nPrevVtx;

    if(!m_pCenter)
		return false;
	if(!m_pVertices)
		return CopyShape(Shape);

	//
	// allocate new shape memory large enough for both shapes
	//
	nVertices = m_nVertices + Shape.m_nVertices;
	nIndices =  m_nIndices + Shape.m_nIndices;

	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * nVertices);
	pIndices = (WORD *)malloc(sizeof(WORD) * nIndices);

	if((!pVertices) || (!pIndices))
	{
		if (pVertices)
			free(pVertices);
		if (pIndices)
			free(pIndices);
		OutputDebugString("Couldn't allocate object memory");
		return false;
	}

	//
	// add first (this) shape to composite
	// 
	for(i = 0; i < m_nVertices; i++)
	{
		pVertices[nCurVtx] = m_pVertices[i];
		nCurVtx++;
	}
	for(i = 0; i < m_nIndices; i++)
	{
		pIndices[nCurIdx] = m_pIndices[i];
		nCurIdx++;
	}

	nPrevVtx = nCurVtx;

	//
	// add second (passed-in) shape to composite
	//
	for(i = 0; i < Shape.m_nVertices; i++)
	{
		pVertices[nCurVtx] = Shape.m_pVertices[i];
		nCurVtx++;
	}
	for(i = 0; i < Shape.m_nIndices; i++)
	{
		pIndices[nCurIdx] = (WORD)(Shape.m_pIndices[i] + (WORD)nPrevVtx);
		nCurIdx++;
	}

	free(m_pVertices);
	free(m_pIndices);

	m_pVertices = pVertices;
	m_nVertices = nVertices;
	m_pIndices = pIndices;
	m_nIndices = nIndices;
	m_pCenter->x = 0.0f;
	m_pCenter->y = 0.0f;
	m_pCenter->z = 0.0f;
	m_CurShape = CS_COMPOSITE;

	return true;
}

// ============================================================================
//
//  CONSTRUCTOR / DESTRUCTOR
//
// ============================================================================

CShapes::CShapes()
{
	Initialize();
}

// ----------------------------------------------------------------------------

CShapes::CShapes(const CShapes &Shape)
{
	CopyShape(Shape);
}

// ----------------------------------------------------------------------------

CShapes::~CShapes()
{
	Cleanup();
}

// ----------------------------------------------------------------------------

CShapes &CShapes::operator=(const CShapes &Shape)
{
	CopyShape(Shape);
	return *this;
}

// ----------------------------------------------------------------------------

CShapes &CShapes::operator+=(const CShapes &Shape)
{
	AddShape(Shape);
	return *this;
}

// ============================================================================
//
//  PUBLIC MEMBER FUNCTIONS
//
// ============================================================================

bool CShapes::NewShape(
	CS_SHAPES Type,
	const int Divisions1,
	const int Divisions2,
	const int Divisions3)
{
	// if old shape exists... destroy it first.
	if(m_pVertices)
	{
		Cleanup();
		Initialize();
	}

	// hack to keep the warnings out.
	Divisions3;

	// now make the shape type requested.
	m_CurShape = Type;

	switch(Type)
	{
		case CS_COMPOSITE:
			return true;

		case CS_BOX:
			return GenerateBox();

		case CS_ICOSOHEDRON:
			return GenerateIcosohedron();

		case CS_SPHERE:
			if(Divisions1)
				return GenerateSphere(Divisions1);
			else
				return GenerateSphere();

		case CS_PYRAMID:
			return GeneratePyramid();

		case CS_CYLINDER:
			if(Divisions1 && Divisions2)
				return GenerateCylinder(Divisions1, Divisions2);
			else if(Divisions1)
				return GenerateCylinder(Divisions1);
			else
				return GenerateCylinder();
		
		case CS_CONE:
			if(Divisions1)
				return GenerateCone(Divisions1);
			else
				return GenerateCone();

		case CS_MESH:
			if(Divisions1 && Divisions2)
				return GenerateMesh(Divisions1, Divisions2);
			else if(Divisions1)
				return GenerateMesh(Divisions1, Divisions1);
			else
				return GenerateMesh();

		case CS_BOID:
			return GenerateBoid();

		case CS_TEAPOT:
			return GenerateTeapot();

		default:
			return false;
	}
}

// ----------------------------------------------------------------------------

void CShapes::Rotate(float x, float y, float z)
{
	D3DMATRIX RotX, RotY, RotZ;
	D3DMATRIX RMatrix;
	D3DVECTOR OldPos;

    if(m_pCenter)
	{
		RotX = RotateXMatrix(x);
		RotY = RotateYMatrix(y);
		RotZ = RotateZMatrix(z);

		RMatrix = MatrixMult(RotX, RotY);
		RMatrix = MatrixMult(RMatrix, RotZ);

		// translate back to the origin, rotate, then translate back
		// to original position
		OldPos.dvX = m_pCenter->dvX;
		OldPos.dvY = m_pCenter->dvY;
		OldPos.dvZ = m_pCenter->dvZ;
		Translate(-m_pCenter->dvX, -m_pCenter->dvY, -m_pCenter->dvZ);
		Transform(RMatrix, true);
		Translate(OldPos.dvX, OldPos.dvY, OldPos.dvZ);
	}
}

// ----------------------------------------------------------------------------

void CShapes::Translate(float x, float y, float z)
{
	D3DMATRIX TMatrix;

	TMatrix = TranslateMatrix(x, y, z);

	Transform(TMatrix, false);
}

// ----------------------------------------------------------------------------

void CShapes::Scale(float factor)
{
	Scale(factor, factor, factor);
}

// ----------------------------------------------------------------------------

void CShapes::Scale(float x, float y, float z)
{
	D3DMATRIX SMatrix;
	
	SMatrix = ScaleMatrix(x, y, z);

	Transform(SMatrix, false);
}

// ----------------------------------------------------------------------------

void CShapes::Orbit(D3DVECTOR point1, float xRot, float yRot, float ZRot)
{
	// not implemented yet
	point1.x = 0;
	xRot = yRot = ZRot = 0.0f;
}

// ----------------------------------------------------------------------------

void CShapes::Fit(D3DVECTOR point1, D3DVECTOR point2)
{
	// not implemented yet
	point1.x = point2.x = 0;
}

// ----------------------------------------------------------------------------

void CShapes::MoveTo(D3DVECTOR point1)
{
    if(m_pCenter)
	{
		// translate back to the origin
		Translate(-m_pCenter->dvX, -m_pCenter->dvY, -m_pCenter->dvZ);
	
		// now translate to the new point
		Translate(point1.dvX, point1.dvY, point1.dvZ);
	}
}

// ----------------------------------------------------------------------------

void CShapes::Transform(D3DMATRIX& Matrix, bool bTransformNormals)
{
	D3DVECTOR SrcVector;
	D3DVECTOR DstVector;

	if(m_pCenter)
	{
		memset(&SrcVector, 0, sizeof(D3DVECTOR));
		memset(&DstVector, 0, sizeof(D3DVECTOR));
	
		// transform center
		*m_pCenter = TransformVector(*m_pCenter, Matrix);
	
		// apply transformation to every vertex in model.
		for(int i = 0; i < m_nVertices; i++)
		{
			// transform vertices
			SrcVector.dvX = m_pVertices[i].dvX;
			SrcVector.dvY = m_pVertices[i].dvY;
			SrcVector.dvZ = m_pVertices[i].dvZ;
			DstVector = TransformVector(SrcVector, Matrix);
			m_pVertices[i].dvX = DstVector.dvX;
			m_pVertices[i].dvY = DstVector.dvY;
			m_pVertices[i].dvZ = DstVector.dvZ;
		}
	
		if(bTransformNormals)
		{
			// apply transformation to every normal in model.
			for(int i = 0; i < m_nVertices; i++)
			{
				SrcVector.dvX = m_pVertices[i].dvNX;
				SrcVector.dvY = m_pVertices[i].dvNY;
				SrcVector.dvZ = m_pVertices[i].dvNZ;
				DstVector = TransformNormal(SrcVector, Matrix);
				m_pVertices[i].dvNX = DstVector.dvX;
				m_pVertices[i].dvNY = DstVector.dvY;
				m_pVertices[i].dvNZ = DstVector.dvZ;
			}
		}
	}
}

// ----------------------------------------------------------------------------

PTSTR CShapes::GetShapeName()
{
	switch(m_CurShape)
	{
		case CS_COMPOSITE :
			return _T("Composite");
		case CS_BOX :
			return _T("Box");
		case CS_SPHERE :
			return _T("Sphere");
		case CS_PYRAMID :
			return _T("Pyramid");
		case CS_CYLINDER :
			return _T("Cylinder");
		case CS_CONE :
			return _T("Cone");
		case CS_MESH :
			return _T("Flat Mesh");
		case CS_BOID :
			return _T("Boid");
		case CS_ICOSOHEDRON :
			return _T("Icosohedron");
		default:
			return _T("Unknown");
	}
}
