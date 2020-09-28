// FILE:        sprite.h
// DESC:        point sprite class header for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:

#define D3D_OVERLOADS
#include <ddraw.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "CD3DTest.h"
#include "CD3D.h"
#include "CFlags.h"
#include "CImage.h"
#include "CTextures.h"
#include "path.h"

// cliffo: Disable the type mismatching warnings. This test will soon be obsolete and I'm not inclined to
// make a huge number of changes and risk changing test behavior for a near-dead test.... at this time. :)
#pragma warning(disable: 4244)

#define GLE(x) (m_pD3D->HResultToString(x))

#define MYD3DFVF_LVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define MYD3DFVF_LSVERTEX (D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define MYD3DFVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define MYD3DFVF_TLSVERTEX (D3DFVF_XYZRHW | D3DFVF_PSIZE | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define MYD3DFVF_TLVERTEX_NOTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR)

#define MAXBUFFER 128 // max buffer length for general strings

#define MAXFORMAT 16 // max buffer length for format strings

#define MAXSIZE 256 // max texture size (width and height)

#if FAST_TEST
#define NVDEF 2   // number of variations (default)
#define NVMIN 2   // number of variations (min)
#define NVMAX 256 // number of variations (max)

#define NOFORMAT -1 // no format flag

#define	NCLIPTESTS 24 // number of clip tests (translates to (x,y) positions)

#define NUMPOINTSPRITES_ONSCREEN 6 // number of sprites rendered on render target

#define NCSETS 4 // number of coefficient sets (constant, linear, quadratic, full)

#define EPSILON 1.0e-3 // clip plane epsilon (moves primitives just inside clip volume)

#define ZERO 1.0e-8 // floating point zero

#define CLIPBORDER 10 // border between render target and viewport

#define MIPLEVELS 9 // mipmap levels (256x256 to 1x1)

#define COFACTINC 0.1f // cofactor increment

#define MIN_MAXPOINTSIZE 16.0f  // minimum value for MaxPointSize

#define NPLANES 2 // number of user clip planes

#define MIN_POINTSPRITE_SIZE 7.0f // minimum size for a some point sprites
#else //FAST_TEST
#define NVDEF 8   // number of variations (default)
#define NVMIN 2   // number of variations (min)
#define NVMAX 256 // number of variations (max)

#define NOFORMAT -1 // no format flag

#define	NCLIPTESTS 24 // number of clip tests (translates to (x,y) positions)

#define NUMPOINTSPRITES_ONSCREEN 6 // number of sprites rendered on render target

#define NCSETS 4 // number of coefficient sets (constant, linear, quadratic, full)

#define EPSILON 1.0e-3 // clip plane epsilon (moves primitives just inside clip volume)

#define ZERO 1.0e-8 // floating point zero

#define CLIPBORDER 10 // border between render target and viewport

#define MIPLEVELS 9 // mipmap levels (256x256 to 1x1)

#define COFACTINC 0.1f // cofactor increment

#define MIN_MAXPOINTSIZE 16.0f  // minimum value for MaxPointSize

#define NPLANES 8 // number of user clip planes

#define MIN_POINTSPRITE_SIZE 7.0f // minimum size for a some point sprites
#endif// FAST_TEST

#define TOPLEFT_INSIDE		0 // indices into point sprite position array
#define TOPCENTER_INSIDE	1
#define TOPRIGHT_INSIDE		2
#define RIGHTCENTER_INSIDE	3
#define BOTRIGHT_INSIDE		4
#define BOTCENTER_INSIDE	5
#define BOTLEFT_INSIDE		6
#define LEFTCENTER_INSIDE	7

const MAX_ADJUSTMENTS = 100; // max number of vertex adjustments to find valid point size/location 
const MAX_ATTEMPTS = 100000; // max number of vertex adjustment attempts to find valid point size/location

const D3DVALUE MINDIFF = 0.1f;

const D3DVALUE DISTINC = 0.1f;

#define CF_VERTEXPOINTSIZE 0x00000002
#define CF_INVALID         0x00000004
#define CF_TEXTURE         0x00000010
#define CF_FLAT		       0x00000020
#define CF_MIPMAP          0x00000040
#define CF_PSGP            0x00000080
#define CF_EMULATING       0x00000100
#define CF_DRAWPORT        0x00000200
#define CF_SCISSOR         0x00000400
#define CF_SPAWN           0x00000800
#define CF_PRECISION       0x00001000

#define NUM_VERTS 8

#define SNAKE_STEPS 90

#define NUM_ROT_ANGLES 100

#define VERT_SIZE 20

const NSTATUS = 11;

typedef union _FLOATUINT32
{
    FLOAT f;
    DWORD dw;
    UINT32 u;
} FLOATUINT32;

typedef struct _COEFFDATA
{
    FLOATUINT32 Ka;
    FLOATUINT32 Kb;
    FLOATUINT32 Kc;
} COEFFDATA, *PCOEFFDATA;

typedef struct _SPRITEDATA
{
    FLOATUINT32 Ka;
    FLOATUINT32 Kb;
    FLOATUINT32 Kc;
    FLOATUINT32 PointSize;
    FLOATUINT32 PointSizeMax;
    FLOATUINT32 PointSizeMin;
} SPRITEDATA, *PSPRITEDATA;

typedef struct _MYD3DLVERTEX
{
    D3DVALUE dvX;
    D3DVALUE dvY;
    D3DVALUE dvZ;
    D3DCOLOR dcColor;
    D3DCOLOR dcSpecular;
    D3DVALUE dvTU;
    D3DVALUE dvTV;
} MYD3DLVERTEX, *PMYD3DLVERTEX;

typedef struct _MYD3DFVFLS
{
    D3DVALUE dvX;
    D3DVALUE dvY;
    D3DVALUE dvZ;
    D3DVALUE dvS;
    D3DCOLOR dcColor;
    D3DCOLOR dcSpecular;
    D3DVALUE dvTU;
    D3DVALUE dvTV;
} MYD3DLSVERTEX, *PMYD3DLSVERTEX;

typedef struct _MYD3DFVFTL
{
    D3DVALUE dvSX;
    D3DVALUE dvSY;
    D3DVALUE dvSZ;
    D3DVALUE dvRHW;
    D3DCOLOR dcColor;
    D3DCOLOR dcSpecular;
    D3DVALUE dvTU;
    D3DVALUE dvTV;
} MYD3DTLVERTEX, *PMYD3DTLVERTEX;

typedef struct _MYD3DFVFTLNOTEX
{
    D3DVALUE dvSX;
    D3DVALUE dvSY;
    D3DVALUE dvSZ;
    D3DVALUE dvRHW;
    D3DCOLOR dcColor;
    D3DCOLOR dcSpecular;
} MYD3DTLVERTEXNOTEX, *PMYD3DTLVERTEXNOTEX;

typedef struct _MYD3DFVFTLS
{
    D3DVALUE dvSX;
    D3DVALUE dvSY;
    D3DVALUE dvSZ;
    D3DVALUE dvRHW;
    D3DVALUE dvS;
    D3DCOLOR dcColor;
    D3DCOLOR dcSpecular;
    D3DVALUE dvTU;
    D3DVALUE dvTV;
} MYD3DTLSVERTEX, *PMYD3DTLSVERTEX;

// CPort values are pixel co-ordinates and start at (0,0)
// (right,bottom) is last pixel in viewport

class CPort
{
    private:
    	long lTop;
        long lLeft;
        long lBottom;
        long lRight;

    public:
    	CPort()
    	{
    		lTop = 0;
            lLeft = 0;
            lBottom = 0;
            lRight = 0;
    	};
    	~CPort(){};
    	VOID SetPort(long x, long y, long Width, long Height)
        {
            lTop = y;
            lLeft = x;
            lBottom = y + Height-1;
            lRight = x + Width-1;
        }
    	bool Inside(long x, long y)
        {
            if ((y >= lTop) && (y <= lBottom) && (x >= lLeft) && (x <= lRight))
                return true;
            else
                return false;
        }
    	long Width(VOID)
        { return (lRight - lLeft+1); }
        long Height(VOID)
        { return (lBottom - lTop+1); }
    	long Top(VOID)
    	{ return lTop; }
	    long Left(VOID)
    	{ return lLeft; }
	    long Bottom(VOID)
    	{ return lBottom; }
    	long Right(VOID)
	    { return lRight; }
    	long CenterX(VOID)
	    { return lLeft + ((lRight - lLeft) >> 1); }
    	long CenterY(VOID)
	    { return lTop + ((lBottom - lTop) >> 1); }
	    long CenterZ(VOID)
    	{ return (long) 0; } // only valid value for simplified version of viewports
};

class CSprite: public CD3DTest
{
    private:
        INT m_iFormat;                                    // current texture format index     
        CImage *m_rgpImage[D3DDP_MAXTEXCOORD][MIPLEVELS]; // image class array
        CnTexture *m_rgpTexture[D3DDP_MAXTEXCOORD];       // texture class
        D3DVALUE m_fFixed;                                // fixed max point size
		D3DXMATRIX m_Projection;                          // projection matrix
		D3DXMATRIX m_View;                                // view matrix
		D3DXMATRIX m_ScreenToWorld;                       // screen to world matrix

    protected:
        CHAR m_szBuffer[MAXBUFFER];             // text buffer
        CHAR m_szFormat[MAXFORMAT];             // text buffer (format)
        CHAR m_szStatus[NSTATUS][2][MAXBUFFER]; // text buffer (status)
        D3DCOLOR m_dcBackground;                // background color
        D3DVALUE m_dvAngle;                     // view angle (fov)
        D3DVALUE m_dvFar;                       // far clipping plane
        D3DVALUE m_dvNear;                      // near clipping plane
        D3DVALUE m_dvPointSizeMax;              // point sprite size max wrt viewport
        D3DXVECTOR3 m_At;                       // at point
        D3DXVECTOR3 m_From;                     // from point
        D3DXVECTOR3 m_Up;                       // up vector
        DWORD m_dwMinLevelStd;                  // min level - standard textures
        DWORD m_dwMinLevelYUV;                  // min level - YUV textures
        DWORD m_dwMinLevel;                     // min mipmap level
        DWORD m_dwMaxLevel;                     // max mipmap level
        UINT m_nVariationsPerFormat;            // variations per texture format
        RECT m_Rect;                            // scissor rect
        SPRITEDATA m_Sprite;                    // sprite data
		CPort m_Port;                           // port class
        UINT m_iNumStagesSet;                   // number of stages to set textures (default = 1)

    public:
        CFlags m_Flags;                         // flags class (common)
        CFlags m_Group;                         // flags class (group)
        CSprite();
        ~CSprite();
		virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        virtual bool TestTerminate(VOID);
        virtual bool ClearFrame(VOID);
        virtual bool BeginScene(VOID);
        virtual VOID UpdateStatus(VOID);
        VOID ClearStatus(VOID);
        bool Clear(DWORD dwColor);
        bool SetDefaultMatrices(VOID);
        bool SetDefaultRenderStates(VOID);
		bool SetViewport(UINT nX, UINT nY, UINT nWidth, UINT nHeight);
        VOID DrawScissor(VOID);
        VOID DrawViewport(VOID);
        VOID ProcessArgs(VOID);
 		VOID SetPosition(UINT uTest, float *pfX, float *pfY);
		VOID SetPositionWorld(UINT nPosition, float *pfX, float *pfY);
		bool AdjustVertex(D3DXVECTOR3 *pVector);
		bool AdjustVertexAttenuated(D3DXVECTOR3 *pVertex, D3DVALUE dvSi);
		VOID UpdateScreenToWorldMatrix(VOID);
		HRESULT GetLastError(VOID);
		float CalcWorldPixelSize();
		VOID GetWorldPosition(D3DXVECTOR3 *pVector);
        bool DisableScissorTest(VOID);
        bool EnableScissorTest(RECT *pRect);
};

class CSpriteAttenuateFVFL : public CSprite
{
    private:
        D3DXVECTOR3 m_LineOfSight;        // line of sight (normalized)
        COEFFDATA m_Coefficients[NCSETS]; // attenuation coefficient data

    public:
        CSpriteAttenuateFVFL();
        ~CSpriteAttenuateFVFL();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CSpriteAttenuateFVFLS : public CSprite
{
    private:
        D3DXVECTOR3 m_LineOfSight;        // line of sight (normalized)
        COEFFDATA m_Coefficients[NCSETS]; // attenuation coefficient data
		bool bInvalid;

    public:
        CSpriteAttenuateFVFLS();
        ~CSpriteAttenuateFVFLS();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
		bool TestTerminate(VOID);
};

class CSpriteScaleFVFL : public CSprite
{
    public:
        CSpriteScaleFVFL();
        ~CSpriteScaleFVFL();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CSpriteScaleFVFLS : public CSprite
{
	private:
		bool bInvalid;

    public:
        CSpriteScaleFVFLS();
        ~CSpriteScaleFVFLS();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
		bool TestTerminate(VOID);
        bool PreModeSetup();
        VOID CleanUpReg(VOID);
};

class CSpriteScaleFVFTL : public CSprite
{
    public:
        CSpriteScaleFVFTL();
        ~CSpriteScaleFVFTL();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CSpriteScaleFVFTLS : public CSprite
{
	private:
		bool bInvalid;
    
    public:
        CSpriteScaleFVFTLS();
        ~CSpriteScaleFVFTLS();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
		bool TestTerminate(VOID);
};

class CSpriteBatch : public CSprite
{
	private:
        DWORD m_dwPointSpriteEnable;

    public:
        CSpriteBatch();
        ~CSpriteBatch();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
		bool ProcessFrame(VOID);
		bool DrawPointSprites(LPVOID Vertex, DWORD dwFormat, UINT StartPosNum, UINT NumToDraw);
		bool DrawCenterQuads(VOID);
};

class CSpriteCap : public CSprite
{
    public:
        CSpriteCap();
        ~CSpriteCap();
        UINT TestInitialize(VOID);
        bool ExecuteTest(UINT uTest);
};

class CSpriteMipMap : public CSpriteAttenuateFVFL
{
    public:
        CSpriteMipMap();
        ~CSpriteMipMap();
		virtual UINT TestInitialize(VOID);
};

class CSpriteMipMapMulti : public CSpriteMipMap
{
    public:
        CSpriteMipMapMulti();
		UINT TestInitialize(VOID);
};

class CSpriteCull : public CSpriteBatch
{
	private:
		int m_nLastCullMode;

	public:
        CSpriteCull();
        ~CSpriteCull();
		UINT TestInitialize(VOID);
		bool ExecuteTest(UINT uTest);	
};

class CSpriteWrapTrans : public CSpriteMipMapMulti
{
	private:
		

	public:
        CSpriteWrapTrans();
        ~CSpriteWrapTrans();
		UINT TestInitialize(VOID);
		bool ExecuteTest(UINT uTest);	
};

class CSpriteFill : public CSpriteBatch
{
	private:
		int m_nLastFillMode;
		bool m_bInvalid;
        bool m_bTexture;

	public:
        CSpriteFill();
        ~CSpriteFill();
		UINT TestInitialize(VOID);
		bool ExecuteTest(UINT uTest);	
};

class CSpriteUClip : public CSprite
{
	private:
		float m_Planes[NPLANES][4];
		MYD3DLVERTEX m_Verts[4];
		MYD3DLVERTEX m_Lines[NPLANES][2];

	public:
        CSpriteUClip();
        ~CSpriteUClip();
		UINT TestInitialize(VOID);
		bool ExecuteTest(UINT uTest);
		VOID GetUnitNormal(D3DXVECTOR3 *pA, D3DXVECTOR3 *pB, D3DXVECTOR3 *pC);
};

class CSpriteVClip : public CSprite
{
	private:
        int m_nSnakeStart;
        CPath m_Path;
        CPath m_PathTL;
        CPath m_Top;
        CPath m_Bottom;

    public:
        CSpriteVClip();
        ~CSpriteVClip();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};
