#ifndef __Lines_h__
#define __Lines_h__

#include "d3dx9.h"
#include "Types.h"
#include "CD3DTest.h"
//#include "CBuffers.h"
#include "CTextures.h"

#define GRID_SIZE 100
#define SWIRL_SIZE 1100

struct TLVERTEX
{
	float	x, y, z, w;
	D3DCOLOR color;
	float	tu, tv;

	TLVERTEX() { }
    TLVERTEX(const D3DXVECTOR4& v, D3DCOLOR c, float _u, float _v )
	{
		x = v.x;  y = v.y;  z = v.z; w = v.w;
        color = c;
        tu = _u; tv = _v; 
    }
};
#define FVF_TLVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

class Lines: public CD3DTest
{
public:
     Lines(bool isAntiAliased = false, bool withTexture = false, bool withPattern = false, D3DSHADEMODE shading = D3DSHADE_GOURAUD);
	 ~Lines() {};

	virtual bool CapsCheck();
	virtual bool SetDefaultMatrices();
	virtual bool SetDefaultMaterials();
	virtual bool SetDefaultRenderStates();
	virtual bool SetDefaultLights();
	//virtual bool Setup();
	virtual bool ProcessFrame();
	virtual bool TestTerminate();
	//virtual void Cleanup();
		
	bool TransformVertex(bool isTL, D3DXMATRIX Matrix, D3DVERTEX *Vertices, TLVERTEX *TLVertices, DWORD Size);
	bool GenerateBuffers(D3DPRIMITIVETYPE primType, DWORD dwX, DWORD dwZ, D3DXMATRIX Matrix, bool isTL = false, bool isTexAddress = false);
	bool GenerateSpecialBuffer();
	bool DrawLines(bool isTL, DWORD dwX, DWORD dwZ, D3DPRIMITIVETYPE primType);
	bool DrawSpecialLines();
	void GetPattern(TCHAR *szPattern);

protected:
	TCHAR           szTestName  [150];
	TCHAR           szCommandKey[150];
	TCHAR			msgString   [150];
	LIGHT		    m_pLights[3];
	D3DXMATRIX		m_ViewMatrices[5];
	D3DXMATRIX		m_mTransform[5];
	D3DVERTEX      *m_pVB;
	TLVERTEX       *m_pVBTL;
	WORD		   *m_pIB;
	DWORD           m_nVertices;
	int 			m_nViewMatrices;
	int             m_nPrimTypes;
	int				m_nX;
	int             m_nZ;
	MATERIAL	  	m_HOMat;
	bool			m_isAntiAliased;
	bool			m_withTexture;
	bool			m_withPattern;
	bool            m_bPatRepeat;
	DWORD           m_dwPattern;
	D3DSHADEMODE    m_ShadingMode;
	UINT			m_uPrevTestNumber;
};

class CBasicTest: public Lines
{
public:
     CBasicTest(bool isAntiAliased = false, bool withTexture = false, bool withPattern = false, D3DSHADEMODE shading = D3DSHADE_GOURAUD);
	 ~CBasicTest() {};

	bool GenerateTexture();
	void SetScissorParam(TCHAR *szScissor);
    
	bool CapsCheck();
	bool Setup();
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	bool TestTerminate(void);
	void Cleanup();
	
private:
	CnTexture  *m_pTexture;
	UINT        m_nScissors;
	RECT        m_Scissor;
	
};

class CPFogTest: public Lines
{
public:
     CPFogTest(bool isAntiAliased = false, bool withTex = false, bool withPattern = false, D3DSHADEMODE shading = D3DSHADE_GOURAUD);
	 ~CPFogTest() {};

	bool GenerateTexture();

	bool CapsCheck();
	bool Setup();
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	bool TestTerminate(void);
	void Cleanup();

private:
	CnTexture      *m_pTexture;
	D3DXMATRIX      m_ProjMatrices[2], m_mFogTransform[2];
};

class CMultiTexTest: public Lines
{
public:
     CMultiTexTest(bool isAntiAliased = false, bool withPattern = false, D3DSHADEMODE shading = D3DSHADE_GOURAUD);
	 ~CMultiTexTest() {};

	bool SetupTextures(D3DTEXTUREOP);
	void RemoveTextures();

	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	bool TestTerminate(void);
	bool CapsCheck();

private:
	CnTexture        **m_pTextures;
	DWORD              m_nTextures;
	DWORD              m_nOperations;
	DWORD              m_nMaxTextures;
	D3DTEXTUREADDRESS *m_pTexAddress;
	TCHAR			 **m_szTexAddress;
	UINT			   m_nTexAddress;
};

#endif // __Lines_h__