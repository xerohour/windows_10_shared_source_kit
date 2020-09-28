// FILE:        TLVertexClip.h
// DESC:        TL vertex clip class header for TL vertex clip conformance tests

#include <ddraw.h>
#include <d3d9.h>
#include "CD3DTest.h"
#include "CD3D.h"

#define GLE(x) (m_pD3D->HResultToString(x))

// port is 90% of clip

#define PORT_X      ((long) (0.05*m_pCurrentMode->nBackBufferWidth + 0.5))
#define PORT_Y      ((long) (0.05*m_pCurrentMode->nBackBufferHeight + 0.5))
#define PORT_WIDTH  ((long) (0.90*m_pCurrentMode->nBackBufferWidth + 0.5))
#define PORT_HEIGHT ((long) (0.90*m_pCurrentMode->nBackBufferHeight + 0.5))

#define CLIP_X      ((long) 0)
#define CLIP_Y      ((long) 0)
#define CLIP_WIDTH  ((long) m_pCurrentMode->nBackBufferWidth)
#define CLIP_HEIGHT ((long) m_pCurrentMode->nBackBufferHeight)

typedef struct _FlexVert
{
    D3DVALUE fPx;
    D3DVALUE fPy;
    D3DVALUE fPz;
    D3DVALUE fRHW;
    D3DCOLOR dcDiffuse;
} FLEXVERT, *PFLEXVERT;

const DWORD FLEXFMT = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

const UINT MAXBUFFER = 128;
const UINT NVERTICES = 10;
const UINT EDGES = 4;
const UINT POINTS_PER_SIDE = 5;
const UINT TESTS_PER_EDGE = 3;
const UINT EDGE_TESTS = (EDGES*TESTS_PER_EDGE);
const UINT QUAD_TESTS = ((4*POINTS_PER_SIDE)*(4*POINTS_PER_SIDE));

const UINT NSTATUS = 1;

const float CLIP_LARGER_THAN_XY = 500.0f;

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

    	// All values in CPort are in pixel coordinates and start at 0.
    	// Therefore, Right,Bottom is last pixel in viewport.
    	void SetPort(long x, long y, long Width, long Height)
        {
            lTop = y;
            lLeft = x;
            lBottom = y + Height - 1;
            lRight = x + Width - 1;
        }
	    bool Inside(long x, long y)
        {
            if ((y >= lTop) && (y <= lBottom) && (x >= lLeft) && (x <= lRight))
                return true;
            else
                return false;
        }
    	long Width(void)
        { return (lRight - lLeft + 1); }
        long Height(void)
        { return (lBottom - lTop + 1); }
    	long Top(void)
    	{ return lTop; }
    	long Left(void)
	    { return lLeft; }
    	long Bottom(void)
    	{ return lBottom; }
    	long Right(void)
	    { return lRight; }
    	long CenterX(void)
    	{ return lLeft+((lRight - lLeft) >> 1); }
    	long CenterY(void)
        { return lTop +((lBottom - lTop) >> 1); }
    	long CenterZ(void)
	    { return (long)0.5; }
};

class CTLVertexClip: public CD3DTest
{
	public:
    	bool m_bShow;
        bool m_bInvalid;
    	CHAR m_szBuffer[MAXBUFFER];
        CHAR m_szStatus[NSTATUS][2][MAXBUFFER];
    	D3DPRIMITIVETYPE m_Type;
    	DWORD m_dwLineColor;
    	DWORD m_dwPortColor;
        FLEXVERT m_Vertices[NVERTICES];
    	UINT m_nTests;
	    CPort m_Port;

	public:
       	CTLVertexClip();
       	~CTLVertexClip();
	    virtual void CommandLineHelp(void);
       	virtual bool SetViewport(UINT nX, UINT nY, UINT nWidth, UINT nHeight);
       	virtual bool ClearFrame(void);
        virtual UINT TestInitialize(void);
       	virtual bool ExecuteTest(UINT);
       	virtual bool ProcessFrame(void);
       	virtual void SceneRefresh(void);
        virtual VOID UpdateStatus();
        VOID ClearStatus();
        bool AnyExtraPixels(void);
        float PxFromTest(UINT nPoint);
        float PyFromTest(UINT nPoint);
		HRESULT GetLastError(void);
		void LargestDist(long lx, long ly, long *plDx, long *plDy);
    	virtual void SetEndPoints(D3DVECTOR *A, D3DVECTOR *B, UINT uTest) { }
};

class CTLVertexClipQuad : public CTLVertexClip
{
    public:
        CTLVertexClipQuad();
        ~CTLVertexClipQuad();
        virtual void SetEndPoints(D3DVECTOR *A, D3DVECTOR *B, UINT uTest);
};

class CTLVertexClipEdge : public CTLVertexClip
{
    public:
        CTLVertexClipEdge();
        ~CTLVertexClipEdge();
        virtual void SetEndPoints(D3DVECTOR *A, D3DVECTOR *B, UINT uTest);
};
