#include "SphereTree.h"
#include "CShapes.h"

CnVertexBuffer* CSphereTree::s_pVB = NULL;
CnIndexBuffer*  CSphereTree::s_pIB = NULL;

UINT			CSphereTree::s_nIndexCount  = 0;
UINT			CSphereTree::s_nVertexCount = 0;

DWORD			CSphereTree::s_dwFVF = D3DFVF_VERTEX;
DWORD			CSphereTree::s_dwFVFSize = sizeof(D3DFVF_VERTEX);

CSphereTree::CSphereTree()
{
	ZeroMemory(m_rgpChildren, sizeof(m_rgpChildren));
	ZeroMemory(&m_vPosition, sizeof(m_vPosition));
	ZeroMemory(&m_vRotationalVelocity, sizeof(m_vRotationalVelocity));
	m_fRadius = 0.0f;
}

//------------------------------------------------------------------------------

CSphereTree::~CSphereTree()
{
	Cleanup(); //this call will remove all the sub-trees
}

//------------------------------------------------------------------------------

void CSphereTree::Cleanup()
{
	RELEASE(s_pVB);
	RELEASE(s_pIB);

	for(UINT nCtr = 0; nCtr < GetChildCount(); nCtr++)
	{
		//if (NULL != m_rgpChildren[nCtr])
		//	m_rgpChildren[nCtr]->Cleanup();
		delete m_rgpChildren[nCtr];
		m_rgpChildren[nCtr] = NULL;
	}

	ZeroMemory(&m_vPosition, sizeof(m_vPosition));
	ZeroMemory(&m_vRotationalVelocity, sizeof(m_vRotationalVelocity));
	m_fRadius = 0.0f;
}

//------------------------------------------------------------------------------

bool CSphereTree::CreateBuffers(CD3DTest* pTest, const UINT nSphereDivisions)
{
	CShapes Sphere;

	if(!Sphere.NewShape(CS_SPHERE, nSphereDivisions))
	{
		return false;
	}

	Sphere.Scale(2.0f, 2.0f, 2.0f);

	DWORD dwUsage = 0;

	if(pTest->m_pSrcDevice->GetBehaviors() & DEVICECREATE_SWVP)
	{ 
		dwUsage = USAGE_SOFTWAREPROCESSING;
	}

	//Software VS not allowed on pure device
	if(pTest->m_pSrcDevice->GetBehaviors() & DEVICECREATE_PURE)
	{
		dwUsage = 0;
	}

	if(NULL == s_pVB)
	{
		if(!pTest->CreateVertexBuffer(sizeof(D3DVERTEX) * Sphere.m_nVertices, dwUsage, GetFVF(), POOL_MANAGED, &s_pVB))
		{
			return false;
		}

		D3DVERTEX* pVertices = NULL;

		if(!s_pVB->Lock(0, 0, (BYTE**)&pVertices, 0))
		{
			return false;
		}

		memcpy(pVertices, Sphere.m_pVertices, sizeof(D3DVERTEX) * Sphere.m_nVertices);

		if(!s_pVB->Unlock())
		{
			return false;
		}

		s_nVertexCount = Sphere.m_nVertices;
	}

	if(NULL == s_pIB)
	{
		if(!pTest->CreateIndexBuffer(sizeof(WORD) * Sphere.m_nIndices, dwUsage, FMT_INDEX16, POOL_MANAGED, &s_pIB))
		{
			return false;
		}

		WORD* pIndices = NULL;

		if(!s_pIB->Lock(0, 0, (BYTE**) &pIndices, 0))
		{
			return false;
		}

		memcpy(pIndices, Sphere.m_pIndices, sizeof(WORD) * Sphere.m_nIndices);

		if(!s_pIB->Unlock())
		{
			return false;
		}

		s_nIndexCount = Sphere.m_nIndices;
	}

	return true;
}

//------------------------------------------------------------------------------

void CSphereTree::ForEach(void (*pCallback)(CSphereTree*))
{
	if(NULL == pCallback)
	{
		return;
	}

	pCallback(this);

	for(UINT nCtr = 0; nCtr<GetChildCount(); nCtr++)
	{
		CSphereTree* pChild = GetChild(nCtr);

		if(pChild)
		{
			pChild->ForEach(pCallback);
		}
	}
}

//------------------------------------------------------------------------------

bool CSphereTree::CreateTree(CD3DTest* pTest,
							 const unsigned int nLevels, 
							 const unsigned int nSphereDivisions,
							 const float fInitialRadius,
							 const float fRadiusFalloff,
							 const D3DXVECTOR3& vPosition)
{
	if(NULL == pTest || NULL == pTest->m_pSrcDevice)
	{
		return false;
	}

	if( (NULL == s_pVB) || (NULL == s_pIB))
	{
		if(!CreateBuffers(pTest, nSphereDivisions))
		{
			return false;
		}
	}

	m_vPosition = vPosition;
	m_fRadius = fInitialRadius;

	bool bResultVal = true;

	if(nLevels>1)
	{
		for(UINT nCtr = 0; nCtr < GetChildCount(); nCtr++)
		{
			float fChildRadius = fInitialRadius * fRadiusFalloff;
			float fOffset      = fInitialRadius + fChildRadius;
			
			D3DXVECTOR3 vNewPosition = D3DXVECTOR3(0, 0, 0);

			switch(nCtr)
			{
			case 0:
				vNewPosition.x += fOffset;
				break;
			case 1:
				vNewPosition.y += fOffset;
				break;
			case 2:
				vNewPosition.z += fOffset;
				break;
			case 3:
				vNewPosition.x -= fOffset;
				break;
			case 4:
				vNewPosition.y -= fOffset;
				break;
			case 5:
				vNewPosition.z -= fOffset;
				break;
			default:
				return false;//should never ever get here
			}

			m_rgpChildren[nCtr] = new CSphereTree();
			if(NULL == m_rgpChildren[nCtr])
			{
				bResultVal = false;
			}
			else
			{
				bResultVal &= m_rgpChildren[nCtr]->CreateTree(pTest, nLevels-1, nSphereDivisions, fChildRadius, fRadiusFalloff, vNewPosition);
			}
		}
	}

	return bResultVal;
}
	
//------------------------------------------------------------------------------

bool CSphereTree::ChangeFVF(CD3DTest* pTest, DWORD dwNewFVF, DWORD dwVertexBytes,
								   void (*pConverter)(const D3DVERTEX& In, void* pOut))
{
	if(NULL == pConverter || NULL == s_pVB || NULL == pTest)
	{
		return false;
	}

	CnVertexBuffer* pNewVB = NULL;

	DWORD dwUsage = 0;

	if(pTest->m_pSrcDevice->GetBehaviors() & DEVICECREATE_SWVP)
	{ 
		dwUsage = USAGE_SOFTWAREPROCESSING;
	}

	//Software VS not allowed on pure device
	if(pTest->m_pSrcDevice->GetBehaviors() & DEVICECREATE_PURE)
	{
		dwUsage = 0;
	}

	if(!pTest->CreateVertexBuffer(dwVertexBytes * s_nVertexCount, dwUsage, dwNewFVF, POOL_MANAGED, &pNewVB))
	{
		return false;
	}

	D3DVERTEX* pOldVerts = NULL;
	void*      pNewVerts = NULL;

	if(!s_pVB->Lock(0, 0, (BYTE**)&pOldVerts, 0))
	{
		return false;
	}

	if(!pNewVB->Lock(0, 0, (BYTE**)&pNewVerts, 0))
	{
		s_pVB->Unlock();
		return false;
	}

	for(UINT nCtr = 0; nCtr<s_nVertexCount; nCtr++)
	{
#ifdef _WIN64
		void* pVert = (void*)((UINT64)pNewVerts + (dwVertexBytes*nCtr));
#else
		void* pVert = (void*)((DWORD)pNewVerts + (dwVertexBytes*nCtr));
#endif // WIN64
		pConverter(pOldVerts[nCtr], pVert);
	}

	bool bResult = true;

	bResult &= s_pVB->Unlock();

	bResult &= pNewVB->Unlock();

	if(bResult)
	{
		RELEASE(s_pVB);
		s_pVB = pNewVB;

		s_dwFVF = dwNewFVF;
		s_dwFVFSize = dwVertexBytes;
	}
	else
	{
		RELEASE(pNewVB);
	}

	return bResult;
}

//------------------------------------------------------------------------------

bool RenderSphereTree(CD3DTest* pTest, CSphereTree* pTree, const D3DXMATRIX& mWorld, const D3DXMATRIX& mView, const D3DXMATRIX& mProj, const float fTime, const bool bUseVShader)
{
	if(NULL == pTest || NULL == pTree)
	{
		return false;
	}

	D3DXMATRIX mTranslate;
	D3DXMATRIX mRotate;
	D3DXMATRIX mScale;
	
	D3DXMatrixTranslation(&mTranslate, 
						   pTree->GetPosition().x,
						   pTree->GetPosition().y,
						   pTree->GetPosition().z);

	D3DXMatrixRotationYawPitchRoll(&mRotate,
									pTree->GetRotationalVelocity().y * fTime,
									pTree->GetRotationalVelocity().x * fTime,
									pTree->GetRotationalVelocity().z * fTime);

	D3DXMatrixScaling(&mScale, 
					   pTree->GetRadius(),
					   pTree->GetRadius(),
					   pTree->GetRadius());

	D3DXMATRIX mTotal;

	mTotal =  mRotate * mScale * mTranslate  * mWorld;

	if(!pTest->SetTransform(D3DTS_WORLD, &mTotal))
	{
		return false;
	}

	//set up matrices for vertex shaders
	D3DXMATRIX temp;
	D3DXMatrixMultiply( &temp, &mTotal, &mView); 
	D3DXMatrixMultiplyTranspose( &mTotal, &temp, &mProj); 
	
	if (bUseVShader && !pTest->SetVertexShaderConstant(0, &mTotal, 4))
	{
		return false;
	}

	if(!pTest->SetIndices(pTree->GetIndices(), 0))
	{
		return false;
	}

	if(!pTest->SetStreamSource(0, pTree->GetVertices(), pTree->GetFVFSize(), 0))
	{
		return false;
	}

	if(!pTest->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, pTree->GetVertexCount(),
									0, pTree->GetTriangleCount(), 0))
	{
		return false;
	}

	D3DXMATRIX mChildMat = mRotate * mTranslate * mWorld;

	bool bResultVal = true;

	for(UINT nCtr = 0; nCtr<pTree->GetChildCount(); nCtr++)
	{
		CSphereTree* pChild = pTree->GetChild(nCtr);

		if(NULL != pChild)
		{
			bResultVal &= RenderSphereTree(pTest, pChild, mChildMat, mView, mProj, fTime, bUseVShader);
		}
	}

	return bResultVal;
}