#ifndef __SPHERETREE_H
#define __SPHERETREE_H


#include "CShapes.h"
#include "D3DX9.h"
#include "types.h"
#include "CD3DTest.h"
#include "CBuffers.h"

#define CHILD_COUNT 6

class CSphereTree
{
public:
	CSphereTree();
	~CSphereTree();

	void		ForEach(void (*pCallback)(CSphereTree*));

	void		Cleanup();
	
	bool		CreateTree(CD3DTest* pTest,
							const unsigned int nLevels, 
							const unsigned int nSphereDivisions,
							const float fInitialRadius,
							const float fRadiusFalloff,
							const D3DXVECTOR3& vPosition);

	bool		ChangeFVF(CD3DTest* pTest,
							DWORD dwNewFVF,
						   DWORD dwVertexBytes,
						   void (*pConverter)(const D3DVERTEX& In, void* pOut));

	inline DWORD	GetFVF() const { return s_dwFVF; }

	inline DWORD	GetFVFSize() const { return s_dwFVFSize; }

	inline DWORD	GetVertexCount() const { return s_nVertexCount; }

	inline DWORD	GetTriangleCount() const { return s_nIndexCount/3; }

	inline UINT		GetChildCount() const { return CHILD_COUNT; }

	inline CSphereTree*	GetChild(const UINT nIndex)
	{
		if(nIndex<GetChildCount())
		{
			return m_rgpChildren[nIndex];
		}
		else
		{
			return NULL;
		}
	}

	inline CnVertexBuffer*	GetVertices() { return s_pVB; }

	inline CnIndexBuffer*	GetIndices() { return s_pIB; }

	inline const D3DXVECTOR3& GetPosition() const { return m_vPosition; }

	inline float GetRadius() const { return m_fRadius; }

	inline const D3DXVECTOR3& GetRotationalVelocity() const
	{
		return m_vRotationalVelocity;
	}

	inline void SetRotationalVelocity(const D3DXVECTOR3& NewVel)
	{
		m_vRotationalVelocity = NewVel;
	}
	

private:
	bool CreateBuffers(CD3DTest* pTest, const UINT nSphereDivisions);

private:
	static			CnVertexBuffer*	s_pVB;
	static			CnIndexBuffer*	s_pIB;

	static			DWORD			s_dwFVF;
	static			DWORD			s_dwFVFSize;

	static			UINT			s_nVertexCount;
	static			UINT			s_nIndexCount;

	CSphereTree*	m_rgpChildren[CHILD_COUNT];

	D3DXVECTOR3		m_vPosition;
	float			m_fRadius;

	D3DXVECTOR3		m_vRotationalVelocity;

};

bool RenderSphereTree(CD3DTest* pTest, 
					 CSphereTree* pTree,
					 const D3DXMATRIX& mWorld, const D3DXMATRIX& mView, const D3DXMATRIX& mProj,
					 const float fTime, const bool bUseVShader = false);


#endif