//-----------------------------------------------------------------------------
//	File:			zbias.h
//	Description:    zbias class header for zbias conformance tests
//	Author:			Todd M. Frost
//	Modifications:	Matthew E. Haigh - 07/2003 thru 08/2003
//						-Improved test for better feature coverage.
//-----------------------------------------------------------------------------

#include <d3dx9.h>
#include "CD3DTest.h"
#include "CFlags.h"
#include "CShapes.h"
#include "CEffect.h"

//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------

#define GLE(x) (m_pD3D->HResultToString(x))

//handy error macro
#define FAIL_AND_EXIT \
	Fail(); \
	bReturn = false; \
	goto Exit;


//-----------------------------------------------------------------------------
//  Structures
//-----------------------------------------------------------------------------

typedef union _FloatDWord
{
    FLOAT f;
    DWORD dw;
} FLOATDWORD;

typedef struct _Rates
{
    FLOAT fDzDxMin;
    FLOAT fDzDxMax;
    FLOAT fDzDyMin;
    FLOAT fDzDyMax;
} RATES, *PRATES;

typedef struct _FlexVert
{
    D3DVALUE x;
    D3DVALUE y;
    D3DVALUE z;
    D3DVALUE rhw;
    D3DCOLOR diffuse;

	inline _FlexVert () {}
	inline _FlexVert (D3DVALUE _x, D3DVALUE _y, D3DVALUE _z, D3DCOLOR _dif) : x(_x), y(_y), z(_z), diffuse(_dif)
	{
		if (_z == 0)
			rhw = 1.0f / 0.000001f;
		else
			rhw = 1.0f / _z;
	}

} FLEXVERT, *PFLEXVERT;

const DWORD D3DFMT = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
const DWORD FLEXFMT = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

struct SpriteVertex
{
	float x, y, z;
	D3DCOLOR diffCol;
};

//-----------------------------------------------------------------------------
//  Constants
//-----------------------------------------------------------------------------

const DWORD CF_WBUFFER		= 0x00000001;
const DWORD CF_WENABLED		= 0x00000002;
const DWORD CF_FOG			= 0x00000004;
const DWORD CF_PIXELSHADER	= 0x00000008;

const UINT MAXBUFFER = 128;
const int MAXLARGEBUF = 256;

const UINT MATERIAL_GREEN  = 0;
const UINT MATERIAL_RED    = 1;
const UINT MATERIAL_BLUE   = 2;
const UINT MATERIAL_WHITE  = 3;
const UINT MATERIAL_YELLOW = 4;
const UINT MATERIAL_ORANGE = 5;
const UINT MATERIAL_CYAN   = 6;
const UINT NMATERIALS      = 7;

const D3DCOLOR COLOR_GREEN	= RGBA_MAKE (  0, 255,   0, 255);
const D3DCOLOR COLOR_RED	= RGBA_MAKE (255,   0,   0, 255);
const D3DCOLOR COLOR_BLUE	= RGBA_MAKE (  0,   0, 255, 255);
const D3DCOLOR COLOR_WHITE	= RGBA_MAKE (255, 255, 255, 255);
const D3DCOLOR COLOR_YELLOW = RGBA_MAKE (230, 255,  20, 255);
const D3DCOLOR COLOR_ORANGE = RGBA_MAKE (238, 118,   0, 255);
const D3DCOLOR COLOR_CYAN	= RGBA_MAKE (  0, 205, 205, 255);
const UINT NCOLORS = 7;
const D3DCOLOR COLORARRAY[NCOLORS] = {COLOR_GREEN, COLOR_RED, COLOR_BLUE, COLOR_WHITE, COLOR_YELLOW, COLOR_ORANGE, COLOR_CYAN};

const UINT NSTATUS = 16;

const UINT NVERTICES = 32;

const UINT PSPRITE_SPACING = 20;
const FLOAT PSPRITE_BORDER_OFFSET = 14.0f;

const float VIEW_ANGLE = (float) pi / 2.0f;
const float VIEW_NEAR = 1.0f;
const float VIEW_FAR = 20.0f;
const float LARGE_VIEW_FAR = 5000.0f;
const float TRANSLATE_VALUE = 4500.0f;
const float SCALE_VALUE = 450.0f;

#ifdef FAST_TEST
#define NSLOPESCALETESTS 15
#define NBIASTESTS 3

#define NTEST_TYPES 1
#define NTEST_VALUES 5
#else
#define NSLOPESCALETESTS 29
#define NBIASTESTS 6

#define NTEST_TYPES 6
#define NTEST_VALUES 9
#endif

#define NFOGTESTS 1
#define NSHADERTESTS 1

const UINT NTESTS = (NTEST_TYPES * NTEST_VALUES) + NSLOPESCALETESTS + NBIASTESTS + NFOGTESTS + NSHADERTESTS;

#define SPHERE_LINE_TEST      0
#define LINE_SPHERE_TEST      1
#define PLANE_POINT_TEST      2
#define POINT_PLANE_TEST      3
#define PLANE_SPHERE_TEST     4
#define FAR_PLANE_SPHERE_TEST 5

//-----------------------------------------------------------------------------
// Classes
//-----------------------------------------------------------------------------

class CZBias: public CD3DTest
{
    protected:
        CHAR m_szBuffer[MAXBUFFER];             //text buffer
        CHAR m_szStatus[NSTATUS][2][MAXBUFFER]; //text buffer (status)
        D3DVALUE m_fBias;                       //z bias override
        D3DVALUE m_fScale;                      //z bias slope scale override
        D3DXMATRIX m_Projection;                //projection matrix
        D3DXMATRIX m_View;                      //view matrix
        UINT m_nAPI;                            //potential API changes
        MATERIAL m_pMaterials[NMATERIALS];      //material (red, green, blue, white)
        CShapes m_Plane;                        //plane mesh
        CShapes m_Sphere;                       //sphere mesh
        CFlags m_Flags;                         //flags class

		CShapes m_FarSphere;					//sphere mesh very far away
		CShapes m_FarPlane;						//plane mesh very far away

	private:
		UINT  upSSIndices[NSLOPESCALETESTS];
		UINT  upBiasIndices[NBIASTESTS];
		float fpSSFactors[NSLOPESCALETESTS][4];	//slope scale test factors
		float fpBiasFactors[NBIASTESTS][2];		//bias test factors
		float fpFactors[NTEST_VALUES][2];		//base factors for primitive tests

	public:
    	CZBias();
    	~CZBias();
        virtual void CommandLineHelp();
    	virtual bool CapsCheck();
    	virtual bool Setup();
    	virtual UINT TestInitialize();
    	virtual bool ExecuteTest (UINT uTest);
    	virtual void Cleanup();
        virtual void UpdateStatus();
        void ClearStatus();
		void GenerateTestName(int TestNum);
        bool GetRange (PRATES pRates, CShapes *pShape);
        bool GetRates (float *pfDzDx, float *pfDzDy, D3DXVECTOR3 *pTri);

	private:
        bool SetupLights();
        bool SetupMaterials();
        bool SetupMatrices();
        bool SetupRenderStates();
        bool SetupShapes();
		bool SetupFactors();
		bool ExecuteSphereLineTest (FLOATDWORD bias, FLOATDWORD scale);
		bool ExecuteLineSphereTest (FLOATDWORD bias, FLOATDWORD scale);
		bool ExecutePlanePointTest (FLOATDWORD bias, FLOATDWORD scale);
		bool ExecutePointPlaneTest (FLOATDWORD bias, FLOATDWORD scale);
		bool ExecuteFogTest();
		bool ExecutePixelShaderText();
		bool ExecuteBiasTest (UINT index, float inc, float biasStart);
		bool ExecuteSlopeScaleTest (UINT index, float inc, float slopeScaleStart, float dzdx, float dzdy);
		bool DrawBorderPointPrimitives (UINT nMaterial);
		bool DrawBorderLinePrimitives (UINT nMaterial);
		bool DrawPlaneSphere (FLOATDWORD bias, FLOATDWORD scale, CShapes * pPlane, CShapes * pSphere);

		bool DrawPlanes ();

};

//-----------------------------------------------------------------------------

class CZBiasZBuffer : public CZBias
{
    public:
        CZBiasZBuffer();
        ~CZBiasZBuffer();
        virtual bool CapsCheck();
        virtual bool ExecuteTest (UINT uTest);
};

class CZBiasWBuffer : public CZBias
{
    public:
        CZBiasWBuffer();
        ~CZBiasWBuffer();
        virtual bool CapsCheck();
        virtual bool ExecuteTest (UINT uTest);
};
