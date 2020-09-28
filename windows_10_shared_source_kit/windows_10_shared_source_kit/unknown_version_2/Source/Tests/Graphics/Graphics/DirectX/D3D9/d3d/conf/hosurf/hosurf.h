#ifndef _HOSURF_H_
#define _HOSURF_H_

#include "d3dx9.h"

#include "CD3DTest.h"
#include "TestShapes.h"
#include "Types.h"
#include "CBuffers.h"
#include "CTextures.h"

//
//  Copied here from d3d8types.h
//

typedef enum _D3DVSD_TOKENTYPE
{
    D3DVSD_TOKEN_NOP        = 0,    // NOP or extension
    D3DVSD_TOKEN_STREAM,            // stream selector
    D3DVSD_TOKEN_STREAMDATA,        // stream data definition (map to vertex input memory)
    D3DVSD_TOKEN_TESSELLATOR,       // vertex input memory from tessellator
    D3DVSD_TOKEN_CONSTMEM,          // constant memory from shader
    D3DVSD_TOKEN_EXT,               // extension
    D3DVSD_TOKEN_END = 7,           // end-of-array (requires all DWORD bits to be 1)
    D3DVSD_FORCE_DWORD = 0x7fffffff,// force 32-bit size enum
} D3DVSD_TOKENTYPE;

#define D3DVSD_TOKENTYPESHIFT   29
#define D3DVSD_TOKENTYPEMASK    (7 << D3DVSD_TOKENTYPESHIFT)

#define D3DVSD_STREAMNUMBERSHIFT 0
#define D3DVSD_STREAMNUMBERMASK (0xF << D3DVSD_STREAMNUMBERSHIFT)

#define D3DVSD_DATALOADTYPESHIFT 28
#define D3DVSD_DATALOADTYPEMASK (0x1 << D3DVSD_DATALOADTYPESHIFT)

#define D3DVSD_DATATYPESHIFT 16
#define D3DVSD_DATATYPEMASK (0xF << D3DVSD_DATATYPESHIFT)

#define D3DVSD_SKIPCOUNTSHIFT 16
#define D3DVSD_SKIPCOUNTMASK (0xF << D3DVSD_SKIPCOUNTSHIFT)

#define D3DVSD_VERTEXREGSHIFT 0
#define D3DVSD_VERTEXREGMASK (0x1F << D3DVSD_VERTEXREGSHIFT)

#define D3DVSD_VERTEXREGINSHIFT 20
#define D3DVSD_VERTEXREGINMASK (0xF << D3DVSD_VERTEXREGINSHIFT)

#define D3DVSD_CONSTCOUNTSHIFT 25
#define D3DVSD_CONSTCOUNTMASK (0xF << D3DVSD_CONSTCOUNTSHIFT)

#define D3DVSD_CONSTADDRESSSHIFT 0
#define D3DVSD_CONSTADDRESSMASK (0x7F << D3DVSD_CONSTADDRESSSHIFT)

#define D3DVSD_CONSTRSSHIFT 16
#define D3DVSD_CONSTRSMASK (0x1FFF << D3DVSD_CONSTRSSHIFT)

#define D3DVSD_EXTCOUNTSHIFT 24
#define D3DVSD_EXTCOUNTMASK (0x1F << D3DVSD_EXTCOUNTSHIFT)

#define D3DVSD_EXTINFOSHIFT 0
#define D3DVSD_EXTINFOMASK (0xFFFFFF << D3DVSD_EXTINFOSHIFT)

#define D3DVSD_MAKETOKENTYPE(tokenType) ((tokenType << D3DVSD_TOKENTYPESHIFT) & D3DVSD_TOKENTYPEMASK)

// macros for generation of CreateVertexShader Declaration token array

// Set current stream
// _StreamNumber [0..(MaxStreams-1)] stream to get data from
//
#define D3DVSD_STREAM( _StreamNumber ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAM) | (_StreamNumber))

// Set tessellator stream
//
#define D3DVSD_STREAMTESSSHIFT 28
#define D3DVSD_STREAMTESSMASK (1 << D3DVSD_STREAMTESSSHIFT)
#define D3DVSD_STREAM_TESS( ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAM) | (D3DVSD_STREAMTESSMASK))

// bind single vertex register to vertex element from vertex stream
//
// _VertexRegister [0..15] address of the vertex register
// _Type [D3DVSDT_*] dimensionality and arithmetic data type

#define D3DVSD_REG( _VertexRegister, _Type ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAMDATA) |            \
     ((_Type) << D3DVSD_DATATYPESHIFT) | (_VertexRegister))

// Skip _DWORDCount DWORDs in vertex
//
#define D3DVSD_SKIP( _DWORDCount ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAMDATA) | 0x10000000 | \
     ((_DWORDCount) << D3DVSD_SKIPCOUNTSHIFT))

// load data into vertex shader constant memory
//
// _ConstantAddress [0..95] - address of constant array to begin filling data
// _Count [0..15] - number of constant vectors to load (4 DWORDs each)
// followed by 4*_Count DWORDS of data
//
#define D3DVSD_CONST( _ConstantAddress, _Count ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_CONSTMEM) | \
     ((_Count) << D3DVSD_CONSTCOUNTSHIFT) | (_ConstantAddress))

// enable tessellator generated normals
//
// _VertexRegisterIn  [0..15] address of vertex register whose input stream
//                            will be used in normal computation
// _VertexRegisterOut [0..15] address of vertex register to output the normal to
//
#define D3DVSD_TESSNORMAL( _VertexRegisterIn, _VertexRegisterOut ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_TESSELLATOR) | \
     ((_VertexRegisterIn) << D3DVSD_VERTEXREGINSHIFT) | \
     ((0x02) << D3DVSD_DATATYPESHIFT) | (_VertexRegisterOut))

// enable tessellator generated surface parameters
//
// _VertexRegister [0..15] address of vertex register to output parameters
//
#define D3DVSD_TESSUV( _VertexRegister ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_TESSELLATOR) | 0x10000000 | \
     ((0x01) << D3DVSD_DATATYPESHIFT) | (_VertexRegister))


// Generates END token
//
#define D3DVSD_END() 0xFFFFFFFF

// Generates NOP token
#define D3DVSD_NOP() 0x00000000

// bit declarations for _Type fields
#define D3DVSDT_FLOAT1      0x00    // 1D float expanded to (value, 0., 0., 1.)
#define D3DVSDT_FLOAT2      0x01    // 2D float expanded to (value, value, 0., 1.)
#define D3DVSDT_FLOAT3      0x02    // 3D float expanded to (value, value, value, 1.)
#define D3DVSDT_FLOAT4      0x03    // 4D float
#define D3DVSDT_D3DCOLOR    0x04    // 4D packed unsigned bytes mapped to 0. to 1. range
                                    // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
#define D3DVSDT_UBYTE4      0x05    // 4D unsigned byte
#define D3DVSDT_SHORT2      0x06    // 2D signed short expanded to (value, value, 0., 1.)
#define D3DVSDT_SHORT4      0x07    // 4D signed short

// assignments of vertex input registers for fixed function vertex shader
//
#define D3DVSDE_POSITION        0
#define D3DVSDE_BLENDWEIGHT     1
#define D3DVSDE_BLENDINDICES    2
#define D3DVSDE_NORMAL          3
#define D3DVSDE_PSIZE           4
#define D3DVSDE_DIFFUSE         5
#define D3DVSDE_SPECULAR        6
#define D3DVSDE_TEXCOORD0       7
#define D3DVSDE_TEXCOORD1       8
#define D3DVSDE_TEXCOORD2       9
#define D3DVSDE_TEXCOORD3       10
#define D3DVSDE_TEXCOORD4       11
#define D3DVSDE_TEXCOORD5       12
#define D3DVSDE_TEXCOORD6       13
#define D3DVSDE_TEXCOORD7       14
#define D3DVSDE_POSITION2       15
#define D3DVSDE_NORMAL2         16

enum PATCHTYPE { PLANAR, UTAHTEAPOT, WART, WAVE, DIMPLE, CSPHERE, XTORUS, XCYLINDER, DEGEN0, FAN };
enum DRAWMODE  { ONEPATCH, NPATCH, SEQUENTIAL};
const DWORD MAX_SHADERS = 32;

const DWORD TEX_NONE = 1;
const DWORD TEX_COORD0 = 2;
const DWORD TEX_COORD1 = 3;



struct TRIPATCH
{
	int 		nVertices;
	int 		nIndices;
	D3DVERTEX	*pVertices;
	WORD		*pIndices;
};

struct HOTEST
{
	D3DBASISTYPE HOBasis;
	D3DDEGREETYPE HODegree;
	int nWidth;
	int nHeight;
	int nTessIndex;
	bool bSpecular;
    DWORD dwTexture;
    DRAWMODE DrawMode;
	PATCHTYPE PatchType;
    D3DPATCHEDGESTYLE HOPStyle;
	float fRSTess;
	int nShader;
};

struct HOSURFACE
{
	CnVertexBuffer	*pVB;
    CnVertexBuffer	*pColor;
    CnIndexBuffer	*pIB;
	UINT			uVertices;
	UINT			uIndices;
	HOTEST			*pTest;
    TCHAR			tcsTess[128];
	TCHAR			tcsSurf[128];
	TCHAR			tcsShader[128];
	DWORD			dwFVF;
};

extern char *g_sBasis[];
extern char *g_sDegree[];

class CHOSurf : public CD3DTest
{
public:
	
	D3DXVECTOR3 	m_vLoc, m_vAt, m_vUp;
	float			m_fYaw, m_fPitch;
	DWORD			m_pShaders8[MAX_SHADERS];
	CnVertexShader*		m_pShaders[MAX_SHADERS];
	CnVertexDeclaration*		m_pDecls[MAX_SHADERS];
	
	// used for D3DX Tesselation:
	CnVertexDeclaration*		m_pDecls_Out[MAX_SHADERS];
	VERTEXELEMENT*	m_pRawDecls[MAX_SHADERS];
	VERTEXELEMENT*	m_pRawDecls_Out[MAX_SHADERS];

	int				m_nShaders;
	D3DXMATRIX		m_TexTrans[3];
    D3DXMATRIX		m_ProjMatrix;
    int 			m_nViewMatrix;
    D3DXMATRIX		m_ViewMatrix[5];
    int 			m_nTPViewMatrix;
    D3DXMATRIX		m_TPViewMatrix[5];
	
	int				m_nLightCount;
	LIGHT		   *m_pLights;

    int				m_nTextureCount;
	CnTexture	  **m_pTextures;

	int				m_nGeom;
	HOSURFACE	   *m_pGeom[1024];

	int				m_nSurfCount;
	HOSURFACE	   *m_pSurfs[1024];

	int				m_nSurfHandles;
	int				m_nCurrentSurf;
	int				m_nLastSurf;
	int 			m_nFramesPerRot;
	float			m_fAngle;
	D3DXMATRIX	    m_RotMatrix, m_PosMatrix;
	MATERIAL	  	m_HOMat, m_PointMat, m_TeapotMat;
	bool			m_bQuintic;
	DWORD			m_dwUsage;
	bool			m_bVS;

	bool			m_bRotate;
	bool			m_bWireFrame;
    bool			m_bSpecular;
    bool			m_bTexture;
	bool			m_bInspect;
	int				m_nPickTest;
	int				m_nFramesPerSurface;
	float			m_fFrameCount;
	float			m_fRange;
	float			m_fStep;
	float			m_fMaxSegments;


	// D3DX Software Tessellation:
	bool				m_bD3DXTessellation;
	CVertexBuffer		*m_pD3DXTessVB;
	CIndexBuffer		*m_pD3DXTessIB;
	UINT				m_uD3DXTessVertices;
	UINT				m_uD3DXTessIndices;
	UINT				m_uD3DXTessVertexSize;


public: 
	CHOSurf();

	// Class methods
	virtual bool LoadGeometry()=0;
	virtual bool CheckCaps()=0;
	virtual bool LoadTextures();
	virtual bool SetupTextures(DWORD dwTex);
    virtual bool SetupVertexShader();
	virtual bool SetupBuffers(DWORD dwFVF, int nVertices, int nStride, void *pVertices, int nIndices, WORD *pIndices, CnVertexBuffer **pVB, CnIndexBuffer **pIB, CnVertexBuffer **pColor);
	virtual HOSURFACE* CreateHOSurf(CTestShapes *pShape, HOTEST *pTest, DWORD dwFVF);
    virtual HOSURFACE* CreateHOSurf(TRIPATCH *pPatch, HOTEST *pTest, DWORD dwFVF);
    virtual HOSURFACE* CreateHOSurf(LPD3DXMESH pMesh, HOTEST *pTest, DWORD dwFVF);
	virtual void   ReleaseTest();
	virtual bool   DrawPoints(HOSURFACE *pSurf);
	virtual bool   DrawSurface(HOSURFACE *pSurf)=0;
    virtual bool   DeleteSurfHandles(bool bAll);
    virtual void   CheckInput();
    virtual void   TweakGeometry(HOTEST *pTest, D3DVERTEX *pVertices);
	virtual void   GetLogTags(TCHAR *pcsName, TCHAR *pcsLine);

	// use d3dx tessellation to draw patch:
	virtual bool DrawD3DXRectPatch(CnVertexBuffer* pNVB, const float* pfEdgeTess, UINT uShaderIndex, const D3DRECTPATCH_INFO* pRectInfo, UINT nDevice);
	virtual bool DrawD3DXTriPatch(CnVertexBuffer* pNVB, const float* pfEdgeTess, UINT uShaderIndex, const D3DTRIPATCH_INFO* pTriInfo, UINT nDevice);
    
	// Framework overloads
	virtual bool SetDefaultMatrices(void);
	virtual bool SetDefaultMaterials(void);
	virtual bool SetDefaultLights(void);
	virtual bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
    virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
    virtual void CommandLineHelp(void);
    virtual void UpdateStatus();
    virtual bool ClearFrame(void);

	// hook for skipping tests based on test specific stuff:
	virtual bool PreExecuteTest(UINT);
        
};


class CRectSurf : public CHOSurf
{
public:
    CRectSurf::CRectSurf();
    virtual void TweakGeometry(HOTEST *pTest, D3DVERTEX *pVertices);
	virtual bool CheckCaps();
    virtual bool LoadGeometry();
	virtual bool DrawSurface(HOSURFACE *pSurf);
	virtual UINT TestInitialize();

	virtual bool PreExecuteTest(UINT);
};

class CTriSurf : public CHOSurf
{
public:
    CTriSurf::CTriSurf();
    virtual bool CheckCaps();
    virtual void QuadToTri(HOTEST *pTest, CTestShapes *pShape, TRIPATCH *pOut);
    virtual bool LoadGeometry();
	virtual bool DrawSurface(HOSURFACE *pSurf);
	virtual UINT TestInitialize();

	virtual bool SetupVertexShader();

	virtual bool PreExecuteTest(UINT);
};

class CNPSurf : public CHOSurf
{
	TCHAR m_tcsDrawType[1024];
    RPOPTIONS m_RPOptions;

public:
    CNPSurf::CNPSurf();
	virtual bool SetupVertexShader();
    virtual bool SetDefaultMatrices(void);
	virtual void RenderNotify(TCHAR *pcsDraw, TCHAR *pcsVertex);
    virtual bool CheckCaps();
    virtual bool LoadGeometry();
	virtual bool DrawSurface(HOSURFACE *pSurf);
    virtual void UpdateStatus();
    virtual void GetLogTags(TCHAR *pcsName, TCHAR *pcsLine);
	virtual bool SetDefaultLights(void);

	virtual bool PreExecuteTest(UINT);
};

#endif