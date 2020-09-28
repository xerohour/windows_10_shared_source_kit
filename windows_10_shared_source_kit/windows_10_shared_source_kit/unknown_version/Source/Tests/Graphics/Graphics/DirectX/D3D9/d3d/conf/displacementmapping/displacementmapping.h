#ifndef __DisplacementMapping_h__
#define __DisplacementMapping_h__

#include "d3dx9.h"
#include "Types.h"
#include "CD3DTest.h"
#include "CBuffers.h"
#include "CTextures.h"

const WORD LOOKUP = 0;
const WORD IMPLICIT = 1;
const WORD PRESAMPLED = 2;

#define VIEW_COUNT 5

struct VERTEX
{
    D3DVALUE    x, y, z;
    D3DVALUE    nx, ny, nz;
	DWORD       color;
	float		tu, tv;

	VERTEX() { }
        VERTEX(const D3DXVECTOR3& v, const D3DXVECTOR3& n, DWORD c, float _u, float _v )
		{      x = v.x;  y = v.y;  z = v.z; 
              nx = n.x; ny = n.y; nz = n.z;
			  color = c;
			  tu = _u; tv = _v; 
        }
};
const int FVF_VERTEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;

typedef union __BIASTESTCASE
{
	struct
    {
		UINT View : 3;
		UINT MipLevel : 4;
		UINT Bias : 4; //max 15
		UINT LookupType : 2;
		UINT AddressMode : 3;
		UINT MipFilter : 3;
        UINT MinFilter : 3;
		UINT MagFilter : 3;
     };
     UINT nTest;
} BIASTESTCASE, *LPBIASTESTCASE;

typedef union __ADDRESSTESTCASE
{
	struct
    {
		UINT View : 3;
		UINT MipLevel : 4;
		UINT LookupType : 2;
		UINT AddressMode : 3;
     };
     UINT nTest;
} ADDRESSTESTCASE, *LPADDRESSTESTCASE;

typedef union __TESTCASE
{
	struct
    {
		UINT View : 3;
		UINT MipLevel : 4;
		UINT LookupType : 2;
		UINT MipFilter : 3;
        UINT MinFilter : 3;
		UINT MagFilter : 3;
     };
     UINT nTest;
} TESTCASE, *LPTESTCASE;

class DisplacementMapping: public CD3DTest
{
public:
     DisplacementMapping();
	 ~DisplacementMapping() {};

	virtual bool CapsCheck();
	virtual UINT TestInitialize(void);
	virtual bool SetDefaultMatrices(void);
	virtual bool SetDefaultMaterials(void);
	virtual bool SetDefaultRenderStates(void);
	virtual bool SetDefaultLights(void);

	bool GenerateTextures(bool, bool = false);
	bool GenerateVertexBuffer(bool = false);
	bool GenerateVertexShader();
	bool CheckFilterSupport(D3DTEXTUREFILTERTYPE, DWORD, WORD);

protected:
	TCHAR			msgString[120];
	CnTexture       *m_pDMTexture, *m_pTexture;
	CnVertexBuffer  *m_pVB1, * m_pVB2;
	WORD			m_nShaders;
	WORD			m_nViewMatrices;
	CnVertexShader*		m_pShaders[7];
	CnVertexDeclaration*		m_pDecls[7];
	D3DXMATRIX		m_ViewMatrices[VIEW_COUNT], m_WVMatrices[VIEW_COUNT], m_ProjMatrix;
	float			m_TessLevel;
	BOOL			m_isPreswizzled;
	WORD			LookupTypes[3];
	TCHAR          *LookupStr[3];
	TCHAR          *FiltersStr[3];
	D3DTEXTUREFILTERTYPE Filters[3];
	bool			m_Presampled, m_Lookup;
	bool            m_AdapTess;
	bool			m_Tex;
	bool			m_DMap;
	float			m_MaxTess;
};

class CMipMapTest: public DisplacementMapping
{
public:
     CMipMapTest();
    ~CMipMapTest();

	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	bool TestTerminate(void);

private:
	TESTCASE		tcCount;
};

class CMaxMipLevelTest: public DisplacementMapping
{
public:
     CMaxMipLevelTest();
    ~CMaxMipLevelTest();

	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	bool TestTerminate(void);

private:
	TESTCASE		tcCount;
};

class CBiasTest: public DisplacementMapping
{
public:
     CBiasTest();
    ~CBiasTest();

	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	bool TestTerminate(void);

private:
	BIASTESTCASE	tcCount;
};

class CAddressModesTest: public DisplacementMapping
{
public:
     CAddressModesTest();
    ~CAddressModesTest();

	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	bool TestTerminate(void);
	bool CheckAddressModeSupport(D3DTEXTUREADDRESS);

private:
	ADDRESSTESTCASE		tcCount;
};

#endif // __DisplacementMapping_h__