#ifndef __MVertex_h__
#define __MVertex_h__

#include "CD3DTest.h"
#include "CTextures.h"

struct VERTEX
{
	float	x, y, z;
	float	nx, ny, nz;
	float	tu, tv;
};

const int FVF_VERTEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


class MVertex : public CD3DTest
{
public:
    MVertex();
    ~MVertex();

	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
    virtual bool TestTerminate(void);

	virtual bool SetDefaultMatrices(void);
	virtual bool SetDefaultMaterials(void);
	virtual bool SetDefaultRenderStates(void);
	virtual bool SetDefaultLights(void);

private:
	CnTexture*			m_pTextureArray[8];

    VERTEX*				m_pVertices;
    DWORD				m_dwVertices;
	LPDWORD				m_pIndices;
	DWORD				m_dwIndices;
	
    DWORD				m_dwCurrentTestNumber;
	D3DPRIMITIVETYPE	m_d3dptPrimitiveType;
    DWORD				m_dwNumFailed;
	bool				m_bIs32BitIndices;

	D3DPRIMITIVETYPE	m_d3dpLastPrimitiveType;
	RENDERPRIMITIVEAPI	m_dwRPLastAPI;

    bool				GeneratePrimitive(void);
};

#endif // __LineTex_h__