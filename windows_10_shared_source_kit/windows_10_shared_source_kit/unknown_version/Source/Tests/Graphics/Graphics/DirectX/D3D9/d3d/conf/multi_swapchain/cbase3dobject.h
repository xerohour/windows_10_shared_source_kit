//////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 2000.
//
// CBase3dObject.h
//
// CBase3dObject class - A generic base class for representing 3d objects.
//					It can be extended for more functionality.
//
// History: 12/15/2000 Bobby Paulose     - Created
//
//////////////////////////////////////////////////////////////////////


#ifndef __3DOBJECT_H__
#define __3DOBJECT_H__


#include "CD3DTest.h"
#include "CTextures.h"




#define MAX_REND_STATES 15
#define MAX_TSS_STATES 2*10
#define MAX_TEXSTAGES 7


struct rs
{
	D3DRENDERSTATETYPE state;
	DWORD value;
};

struct tss
{
	DWORD stage;
	D3DTEXTURESTAGESTATETYPE type;
	DWORD value;	
};




class CPrimitive 
{
public:
	D3DPRIMITIVETYPE primType;
	UINT numVertices;
	LPWORD indx_verts;
	class CPrimitive *nextPrim;

	struct rs RSarray[MAX_REND_STATES];
	UINT rsArrayLen;

	struct tss TSSarray[MAX_TSS_STATES];
	UINT tssArrayLen;

	int StageTextureIndex[MAX_TEXSTAGES];

public:
	CPrimitive(LPWORD pIndexData, DWORD numVerts, D3DPRIMITIVETYPE primitiveType);
	~CPrimitive();

	bool SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	bool SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
	bool SetTextureIndex(DWORD Stage, UINT uIndex );
	CPrimitive *GetNextPrimitive();
	bool SetNextPrimitive(CPrimitive * nextPrimitive);
};




// Recommendations:
// 1. For performance, the list "primitives" may be ordered according to primitive type.
// 2. For performance, the same primitive type may be sub-ordered based on the textures used; 
//			ie primitives(of the same type) using the same textures may be sequential in the list.


class CBase3dObject
{

	UINT numVertices;
	void *vertices;
	DWORD FVF;
	CPrimitive *primitives;

	CD3DTest *pThisTest;

public:
	//dimensions of the 3d object.
	int xdimension,ydimension,zdimension;

public:
	CBase3dObject(UINT numVerts, DWORD FVFcaps, void *pVertices, CD3DTest *pCallingTest, int xdim=0, int ydim=0, int zdim=0);
	~CBase3dObject();

	UINT AddIndxPrimitive(LPWORD pIndexData, DWORD numVertices, D3DPRIMITIVETYPE primitiveType);
	
	bool SetRenderStatePrimitive(UINT primNum, D3DRENDERSTATETYPE State, DWORD Value);

	bool SetTSSPrimitive(UINT primNum, DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);

	bool SetTexturePrimitiveIndex(UINT primNum, DWORD Stage, UINT uIndex );

	bool Render(D3DMATRIX* worldMatrix, D3DMATRIX* viewMatrix, D3DMATRIX* projMatrix, CnTexture *textureArray[] );
};





#endif
