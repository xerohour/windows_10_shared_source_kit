#ifndef _CPSHADERX_H_
#define _CPSHADERX_H_

#include "d3dx9.h"
#include "CD3DTest.h"
#include "CShapes.h"

struct DECL_INFO
{
	VOID*    pVertices;
	WORD*    pIndices;
	CnVertexDeclaration* pDecl;
	int      nVertices;
	int      nIndices;

	DECL_INFO() { pVertices = NULL; pIndices = NULL; pDecl = NULL; nVertices = nIndices = 0; }
};

struct TVERTEX
{
	float   x,  y,  z, w;
	float  tx, ty, tz, tw;

	TVERTEX() { }
    TVERTEX(const D3DXVECTOR3& v, const D3DXVECTOR4& t)
	{
		x = v.x;  y = v.y;  z = v.z; w = 1.f;
		tx = t.x;  ty = t.y;
		tz = t.z;  tw = t.w;
    }
};
//const int FVF_TVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

struct TLVERTEX
{
	float	 x, y, z, w;
	D3DCOLOR color;
	float	 tx1, ty1, tz1, tw1;
	float    tx2, ty2, tz2, tw2;
	float    tx3, ty3, tz3, tw3;

	TLVERTEX() { }
    TLVERTEX(const D3DXVECTOR3& v, D3DCOLOR c, const D3DXVECTOR4& t1, const D3DXVECTOR4& t2, const D3DXVECTOR4& t3 )
	{
		x = v.x;  y = v.y;  z = v.z; w = 1.f; 
        color = c;
        tx1 = t1.x;  ty1 = t1.y;  tz1 = t1.z; tw1 = t1.w;
		tx2 = t2.x;  ty2 = t2.y;  tz2 = t2.z; tw2 = t2.w;
		tx3 = t3.x;  ty3 = t3.y;  tz3 = t3.z; tw3 = t3.w;
    }
};
const int FVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX6;

struct TLVOLVERTEX
{
	float	x, y, z, w;
	D3DCOLOR color;
	float	 tx1, ty1, tz1, tw1;
	float    tx2, ty2, tz2, tw2;
	float    tx3, ty3, tz3, tw3;

	TLVOLVERTEX() { }
    TLVOLVERTEX(const D3DXVECTOR3& v, D3DCOLOR c, const D3DXVECTOR4& t1, const D3DXVECTOR4& t2, const D3DXVECTOR4& t3)
	{
		x = v.x;  y = v.y;  z = v.z; w = 1.f; 
        color = c;
        tx1 = t1.x;  ty1 = t1.y;  tz1 = t1.z; tw1 = t1.w;
		tx2 = t2.x;  ty2 = t2.y;  tz2 = t2.z; tw2 = t2.w;
		tx3 = t3.x;  ty3 = t3.y;  tz3 = t3.z; tw3 = t3.w;
    }
};
const int FVF_TLVOLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

class CInclude : public ID3DXInclude
{
	bool m_bLoadFromFile;
public:
	CInclude() { m_bLoadFromFile = false; }
	inline void		SetLoadFromFile(bool bFile) { m_bLoadFromFile = bFile; }
    STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
    STDMETHOD(Close)(LPCVOID pData);
};

typedef struct _EFFECTDESC
{
    UINT	nTechniques;
	int    *pIndices;
    LPTSTR sName;
}EFFECTDESC, *LPEFFECTDESC;

class CPShaderX : public CD3DTest
{
  public: 
    CPShaderX();

    BOOL GetEffectDesc(LPTSTR lpszName);
        
    // Framework overloads
    virtual bool Setup(void);
    virtual void Cleanup(void);
    virtual UINT TestInitialize(void);
    virtual bool ExecuteTest(UINT);
    virtual bool TestTerminate(void);
    virtual bool ClearFrame(void);
    virtual bool CapsCheck(void);
	virtual bool ProcessFrame();		

	bool CreatePrecisionBuffers();
    bool CreateTextures();
    bool CreateCubeTextures();
    bool CreateVolTextures();
	
	/*void GenerateTechniques();
	void GenerateTexldTechniques();
	void GenerateTexldlTechniques();
	void GenerateTexldbTechniques();
	void GenerateTexldpTechniques();
	void GenerateTexlddTechniques();
	void GenerateDsxyTechniques();
	void GenerateSwizzleTechniques();
	void GenerateMaskTechniques();
	void GenerateMasks(FILE *, char *, int, int, int);
	void GenerateSwizzles(FILE *, char *, int, int, int);
	void Generate_aL_Techniques();
	void GenerateLabels(int i = 2048);
	void GenerateExponentPrecisionTechniques(int i = 127);
	void GenerateMentissaPrecisionTechniques(int i = 127);
	void GenerateTechnique();
	void GenerateInstTechniques(TCHAR *tempStr);
	void GenerateRandInstTechniques(TCHAR *tempStr);*/

    LPTSTR m_szPSPrefix;

  protected:
    D3DXVECTOR3       m_vLoc, m_vAt, m_vUp;
    D3DXMATRIX        m_mWorld, m_mView, m_mPerc, m_mOrtho, m_mTotal[2];
    DECL_INFO         m_QuadInfo, m_SphereInfo, m_TinyQuadInfo, m_TexTLInfo, m_CubeTLInfo, m_VolTLInfo, m_PrecisionInfo;
    D3DCOLOR          m_Stripes[8];
    bool              m_bLoadFromFile, m_bLoadFile;
	TCHAR             m_sFile[MAX_PATH];
	float             m_vPosLimits[4], m_TexSizes[4], m_CubeSizes[4], m_VolSizes[4], m_MaxTexRepeat[4];
	float			  m_fPrevTolPerPixel, m_fPrevTolPerPixelChannel;

	//varibles used to determine number of bits used by shaders to store floats
	bool			  m_bCheckPrecision;
    UINT              m_uExponentPrecision;
	UINT              m_uMantissaPrecision;

    UINT              m_nEffectDesc, m_nEffectDescAlloc;
    LPEFFECTDESC      m_pEffectDesc;

    DWORD             m_dwFXCreateFlags;
    CInclude          m_Include;
    CnEffect         *m_pCurrentEffect;
    UINT              m_nCurrentEffect;
        
    static const UINT m_nMaxTex2D = 3;
    UINT              m_nTex2D;
    CnTexture*        m_pTex2D[m_nMaxTex2D];

    static const UINT m_nMaxTexCube = 2;
    UINT              m_nTexCube;
    CnCubeTexture*    m_pTexCube[m_nMaxTexCube];

    static const UINT m_nMaxTexVol = 2;
    UINT              m_nTexVol;
    CnVolTexture*     m_pTexVol[m_nMaxTexVol];

    LPD3DXMACRO		  m_pMacros;
};

class CPShader3_0 : public CPShaderX
{
  public: 
    CPShader3_0(LPTSTR str = "");
    virtual bool CapsCheck(void);
};

class CPShader2_x : public CPShaderX
{
  public: 
    CPShader2_x(LPTSTR str = "");
    virtual bool CapsCheck(void);    
};

class CPShader2_0: public CPShaderX
{
    public:
    
    CPShader2_0(LPTSTR str = "");
    virtual bool CapsCheck(void);    
};

class CPShader1_x: public CPShaderX
{
    public:
    
    CPShader1_x(LPTSTR str = "");
    virtual bool CapsCheck(void);    
    virtual bool PreModeSetup();
    virtual void Cleanup(void);
};

class CLandscape2_0: public CPShaderX
{
    LPTEXTURES  m_pLandColorTex;
    LPTEXTURES  m_pRockCoverTex;
    LPTEXTURES  m_pDetailTex;
    LPTEXTURES  m_pGroundCoverTex;
    LPTEXTURES  m_pRockMaskTex;
    LPTEXTURES  m_pWaterTex;
    LPEFFECTS   m_pEffect;

    public:
    
    CLandscape2_0(LPTSTR str = "");
    virtual bool CapsCheck(void);    
    virtual bool ExecuteTest(UINT uTestNumber);
    virtual UINT TestInitialize(void);
    virtual void Cleanup(void);
};

class CStarburst2_0: public CPShaderX
{
    LPEFFECTS   m_pEffect;

    public:
    
    CStarburst2_0(LPTSTR str = "");
    virtual bool CapsCheck(void);    
    virtual bool ExecuteTest(UINT uTestNumber);
    virtual UINT TestInitialize(void);
    virtual void Cleanup(void);
};

BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam);

#endif
