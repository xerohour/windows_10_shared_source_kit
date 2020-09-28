//////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 2000.
//
// CBase3dObject.cpp
//
// CBase3dObject class implementation - A generic base class for representing 3d objects.
//									It can be extended for more functionality.
//
// History: 12/15/2000 Bobby Paulose     - Created
//
//////////////////////////////////////////////////////////////////////

#include "CBase3dObject.h"
#include "d3d.h"



/////////////////////////////////////////////////////////////////////////////



CPrimitive::CPrimitive(LPWORD pIndexData, DWORD numVerts, D3DPRIMITIVETYPE primitiveType)
{
	primType = primitiveType;
	numVertices = numVerts;
	indx_verts = (LPWORD)malloc( sizeof(WORD) * numVerts );
	if (indx_verts)
	{
		memcpy( indx_verts, pIndexData, sizeof(WORD) * numVerts );
	}

	nextPrim = NULL;
	rsArrayLen = 0;
	tssArrayLen = 0;

	for (int i=0; i<MAX_TEXSTAGES; i++)
	{
		StageTextureIndex[i] = -1;
	}
}


CPrimitive::~CPrimitive()
{
	if (indx_verts) free(indx_verts);
	if (nextPrim) delete(nextPrim);
}


bool CPrimitive::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	if (rsArrayLen < MAX_REND_STATES)
	{
		RSarray[rsArrayLen].state = State;
		RSarray[rsArrayLen].value = Value;
		rsArrayLen++;
		return true;
	}
	else
	{
		return false;
	}
}

bool CPrimitive::SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	if (tssArrayLen < MAX_TSS_STATES)
	{
		TSSarray[tssArrayLen].stage = Stage;
		TSSarray[tssArrayLen].type = Type;
		TSSarray[tssArrayLen].value = Value;
		tssArrayLen++;
		return true;
	}
	else
	{
		return false;
	}
}


CPrimitive * CPrimitive::GetNextPrimitive()
{
	return nextPrim;
}

bool CPrimitive::SetNextPrimitive(CPrimitive * nextPrimitive)
{
	if (!nextPrim)
	{
		nextPrim = nextPrimitive;
		return true;
	}
	else
	{
		return false;
	}
}

bool CPrimitive::SetTextureIndex(DWORD Stage, UINT uIndex )
{
	if (Stage >= MAX_TEXSTAGES)
		return false;
	StageTextureIndex[Stage] = (int) uIndex;
	return true;
}



/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

CBase3dObject::CBase3dObject(UINT numVerts, DWORD FVFcaps, void *pVertices, CD3DTest *pCallingTest, int xdim, int ydim, int zdim)
{
	numVertices = numVerts;
	FVF = FVFcaps;
	primitives = NULL;
	pThisTest = pCallingTest;

	//initialize dimensions. Ideally, this can be automated by using data in pVertices.
	xdimension = xdim;
	ydimension = ydim;
	zdimension = zdim;

	vertices = malloc( pThisTest->GetVertexSize(FVFcaps) * numVerts );
	if (vertices)
	{
		memcpy( vertices, pVertices, pThisTest->GetVertexSize(FVFcaps) * numVerts );
	}
}


CBase3dObject::~CBase3dObject()
{
	if (vertices) free(vertices);
	if (primitives) delete(primitives);
}


UINT CBase3dObject::AddIndxPrimitive(LPWORD pIndexData, DWORD numVertices, D3DPRIMITIVETYPE primitiveType)
{
	UINT u=1;
	if (!primitives)
	{
		primitives = new CPrimitive(pIndexData, numVertices, primitiveType);
		if (primitives) 
			return 1; 
		else 
			return 0;
	}
	else
	{
		class CPrimitive *prevPrim = primitives,*nextPrim = primitives;
		while (nextPrim)
		{
			u++;
			prevPrim = nextPrim;
			nextPrim = nextPrim->GetNextPrimitive();
		}

		if ( prevPrim->SetNextPrimitive(new CPrimitive(pIndexData, numVertices, primitiveType)) )
			return u;
		else
			return 0;
	}
}


bool CBase3dObject::SetRenderStatePrimitive(UINT primNum, D3DRENDERSTATETYPE State, DWORD Value)
{
	UINT u=1;
	if (primNum == 1)
	{
		if (primitives)
		{
			primitives->SetRenderState( State,  Value);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (primNum > 1)
	{
		class CPrimitive *prevPrim = primitives,*nextPrim = primitives;
		while (nextPrim)
		{
			prevPrim = nextPrim;
			nextPrim = nextPrim->GetNextPrimitive();
			if (nextPrim) u++;
			if (primNum == u)
			{
				nextPrim->SetRenderState( State,  Value);
				return true;
			}
		}

		return false;
	}
	else //primNum == 0
	{
		return false;
	}

}



bool CBase3dObject::SetTSSPrimitive(UINT primNum, DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	UINT u=1;
	if (primNum == 1)
	{
		if (primitives)
		{
			primitives->SetTextureStageState( Stage, Type, Value);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (primNum > 1)
	{
		class CPrimitive *prevPrim = primitives,*nextPrim = primitives;
		while (nextPrim)
		{
			prevPrim = nextPrim;
			nextPrim = nextPrim->GetNextPrimitive();
			if (nextPrim) u++;
			if (primNum == u)
			{
				nextPrim->SetTextureStageState( Stage, Type, Value);
				return true;
			}
		}

		return false;
	}
	else //primNum == 0
	{
		return false;
	}

}


bool CBase3dObject::SetTexturePrimitiveIndex(UINT primNum, DWORD Stage, UINT uIndex )
{
	UINT u=1;
	if (primNum == 1)
	{
		if (primitives)
		{
			primitives->SetTextureIndex( Stage, uIndex );
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (primNum > 1)
	{
		class CPrimitive *prevPrim = primitives,*nextPrim = primitives;
		while (nextPrim)
		{
			prevPrim = nextPrim;
			nextPrim = nextPrim->GetNextPrimitive();
			if (nextPrim) u++;
			if (primNum == u)
			{
				nextPrim->SetTextureIndex( Stage, uIndex );
				return true;
			}
		}

		return false;
	}
	else //primNum == 0
	{
		return false;
	}

}



bool CBase3dObject::Render(D3DMATRIX* worldMatrix, D3DMATRIX* viewMatrix, D3DMATRIX* projMatrix, CnTexture *textureArray[] )
{
	// Go through all primitives
	// For each primitive
	// Set RenderState, Set TSS, Set Texture, Set Transform
	// then Render.

	RPOPTIONS	ROpt;
	CPrimitive *pPrim = primitives;

	ROpt.Pool = POOL_DEFAULT;
	ROpt.Usage = 0;
	ROpt.uIBBitCount = 16;
	ROpt.Flags = 0;


	while( pPrim)
	{
		UINT i;
		
		for (i=0; i<pPrim->rsArrayLen; i++)
		{
			pThisTest->SetRenderState(pPrim->RSarray[i].state, pPrim->RSarray[i].value);
		}

		for (i=0; i<pPrim->tssArrayLen; i++)
		{
			pThisTest->SetTextureStageState(pPrim->TSSarray[i].stage, pPrim->TSSarray[i].type, pPrim->TSSarray[i].value);
		}

		for (i=0; i<MAX_TEXSTAGES; i++)
		{
			// not used stages disabled.   if (!StageTexture) break;
			//pThisTest->SetTexture(i, pPrim->StageTexture[i]);
			int nIndex = pPrim->StageTextureIndex[i];
			pThisTest->SetTexture( i, nIndex < 0 ? NULL : textureArray[ nIndex ] );
		}

		pThisTest->SetTransform(D3DTRANSFORMSTATE_WORLD, worldMatrix);
		pThisTest->SetTransform(D3DTRANSFORMSTATE_VIEW, viewMatrix);
		pThisTest->SetTransform(D3DTRANSFORMSTATE_PROJECTION, projMatrix);


		pThisTest->RenderPrimitive(pPrim->primType, FVF, vertices, numVertices, pPrim->indx_verts, pPrim->numVertices, &ROpt);

		// go to next primitive
		pPrim = pPrim->GetNextPrimitive();
	}

	return true;
}


