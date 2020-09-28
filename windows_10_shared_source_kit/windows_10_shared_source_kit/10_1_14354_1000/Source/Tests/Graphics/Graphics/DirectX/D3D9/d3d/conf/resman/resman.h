#ifndef __ResMan_h__
#define __ResMan_h__

#include "CD3DTest.h"
#include "CBuffers.h"
#include "CTextures.h"
#include <list>
using namespace std;

#define NUMBER_OF_TESTS 0xffe
#define MAXPRIORITY 0xffff
#define QPC_DIFFERENCE(end, start)  ( (end >= start) ? (end-start) : (0x7fffffffffffffff-start+end) )
#define PI 3.14159265
#define MAX_ERROR_MESSAGE 5000

#define FAIL -1
#define SKIP 0
#define RUN  1

typedef enum _PRIM_TYPES
{
	IS_NULL            = 0,
	IS_16INDEXBUFFER   = 1,
	IS_32INDEXBUFFER   = 2,
	IS_TLVERTEXBUFFER  = 3,
	IS_LVERTEXBUFFER   = 4,
	IS_VERTEXBUFFER    = 5,
	IS_TEXTURE         = 6,
	IS_CUBEMAP         = 7,
	IS_VOLUME          = 8
} PRIM_TYPES;

struct RESINFO
{
    PRIM_TYPES Type;
	DWORD      NumSlices;
	FORMAT     Format;
	bool      *pCheckColor;
	D3DCOLOR  *pColors;

	RESINFO() { Type = IS_NULL; NumSlices = 0; Format = FMT_UNKNOWN; pCheckColor = NULL; pColors = NULL; }
};	

typedef list<PRIM_TYPES       > TYPELIST;
typedef list<RESINFO       *> RESINFOLIST;
typedef list<CVertexBuffer *> VBUFFERLIST;
typedef list<CIndexBuffer  *> IBUFFERLIST;
typedef list<CTexture      *> TEXTURELIST;
typedef list<CCubeTexture  *> CUBEMAPLIST;
typedef list<CVolTexture   *> VOLTEXLIST;

struct LVERTEX
{
    D3DVALUE    x, y, z;
	D3DCOLOR    color;
	D3DCOLOR    specular;
	D3DVALUE    tu, tv;
	
	LVERTEX() { }
    LVERTEX(const D3DXVECTOR3& v, D3DCOLOR _color, D3DCOLOR _specular, float _u, float _v)
    {
		x = v.x; y = v.y; z = v.z; 
        color = _color; specular = _specular;
        tu = _u; tv = _v;
    }
};
const int FVF_LVERTEX = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1;

struct VERTEX
{
    D3DVALUE    x, y, z;
    D3DVALUE    nx, ny, nz;
	D3DVALUE    tu, tv;

	VERTEX() { }
    VERTEX(const D3DXVECTOR3& v, const D3DXVECTOR3& n, const float _u, const float _v)
	{
		x = v.x;  y = v.y;  z = v.z; 
        nx = n.x; ny = n.y; nz = n.z;
        tu = _u; tv = _v; 
    }
};
const int FVF_VERTEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

struct VOLVERTEX
{
    D3DVALUE    x, y, z;
    D3DCOLOR    color;
	D3DVALUE    tu, tv, tw;

	VOLVERTEX() { }
    VOLVERTEX(const D3DXVECTOR3& v, D3DCOLOR _color, const D3DXVECTOR3& t)
	{
		x = v.x;  y = v.y;  z = v.z; 
        color = _color;
	    tu = t.x; tv = t.y; tw = t.z;
    }
};
const int FVF_VOLVERTEX = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0);

class ResMan: public CD3DTest
{
public:
     ResMan();
	 ~ResMan(){};

	virtual bool PreModeSetup();
	virtual bool CapsCheck();
	virtual bool SetDefaultMatrices();
	virtual bool SetDefaultMaterials();
	virtual bool SetDefaultRenderStates();
	virtual bool SetDefaultLights();
    virtual UINT TestInitialize();
	virtual bool ClearFrame();
	virtual bool Setup(); 
	virtual void Cleanup();
	virtual void CommandLineHelp();

/*private:
	DWORD		 m_screenRedMask;
	DWORD		 m_screenGreenMask;
	DWORD		 m_screenBlueMask;*/

protected:
	TCHAR		 msgString[120];
	DWORD		 m_TotalNumberOfSlices;
	WORD		 m_MaxTextureWidth;
	WORD		 m_MaxTextureHeight;
	WORD		 m_MaxCubeWidth;
	WORD		 m_MaxVolumeExtent;
	bool         m_UseLODBias;
	bool		 m_UseTextures;
	bool		 m_UseCubeMaps;
	bool		 m_UseVolMaps;
	bool		 m_UseMipMaps;
	bool		 m_UseMipCubeMaps;
	bool		 m_UseMipVolMaps;
	bool         m_Use32BitIB;
	bool         m_CubeMapIsPow2;
	bool         m_VolumeMapIsPow2;
	bool         m_TextureIsPow2;
	bool		 m_NonPow2Conditional;
	bool		 m_SquareOnly;
	bool        *m_bWrapAndMipTex;
	bool        *m_bWrapAndMipCube;
	bool        *m_bWrapAndMipVol;
	FORMAT		 m_ScreenFormat;
	FILE        *m_input, *m_output;
	UINT		 m_uPrevMaxMipLevel;

public:
	//ResMan.cpp
	bool        FindScreenFormat();
	bool        FindMaxTexSizes();
	//CheckColors.cpp
	bool		CheckColors ( CSurface *pSrcSurf, DWORD primColor, void *pBuf, int nPitch, D3DCOLOR *color, UINT index, UINT wnum, UINT hnum);
	bool        CheckTexture( CSurface *pSrcSurf, void *pBuf, int nPitch, CTexture *pTexture, bool *bCheckColor, D3DCOLOR *Colors, UINT startIndex, UINT wnum, UINT hnum);
	bool		CheckCubeMap( CSurface *pSrcSurf, void *pBuf, int nPitch, CCubeTexture *pCubeMap, bool *bCheckColor, D3DCOLOR *Colors, UINT startIndex, UINT wnum, UINT hnum);
	bool		CheckVolumeTexture( CSurface *pSrcSurf, void *pBuf, int nPitch, CVolTexture *pVolTexture, bool *bCheckColor, D3DCOLOR *Colors, UINT startIndex, UINT wnum, UINT hnum);
	bool        ConvertSurf( CSurface *pSrcSurf, void *pBuf, int nPitch, CSurface *pSurf, CSurface *pNewSurf, D3DCOLOR *color, PALETTEENTRY *pPal, LONG *position, TCHAR *szErrorMessage, UINT *puCountWhiteSlices );
	bool        CheckColorsInRect( CSurface *pSrcSurf, void *pBuf, int nPitch, CSurface *pSurf, D3DCOLOR *color, PALETTEENTRY *pPal, LONG *position, TCHAR *szErrorMessage, UINT *puCountWhiteSlices );
	bool		CheckColorsInVolume( CVolume *pTexVolume, CSurface *pSrcSurf, bool *pCheckColor, D3DCOLOR *Colors, VOLUME_DESC *pDesc, PALETTEENTRY *pPal, void *pBuf, int nPitch, 
									 UINT startIndex, UINT wnum, UINT hnum, FLOAT wcell, FLOAT hcell, TCHAR *szErrorMessage, bool *bOnlyFirstSliceWhite, UINT *puCountAllWhiteLevels );
	//checkdxcaps.cpp
	void        CheckDX9Caps();
	void        CheckDX8Caps();
	void        CheckDX7Caps();
//	void		CheckDX6Caps();
};

class CBasicTest: public ResMan
{
	public:
		CBasicTest();
		~CBasicTest() {};
		UINT TestInitialize();
		bool Setup(); 
		void Cleanup();
		bool ExecuteTest(UINT);
		bool ProcessFrame();
		bool TestTerminate();
		void UpdateStatus();
		void CheckBasicTestCaps();
		bool GenerateTexture(DWORD texIndex);
		bool GenerateCubeTexture(DWORD texIndex);
		bool GenerateVolumeTexture(DWORD texIndex);
		bool PrioritiesVB(CVertexBuffer *, DWORD *);
		bool PrioritiesIB(CIndexBuffer *, DWORD *);
		bool PrioritiesT(CTexture *, DWORD *);
		bool PrioritiesCT(CCubeTexture *, DWORD *);
		bool PrioritiesVT(CVolTexture *, DWORD *);
	private:
		bool draw_Textures;
		bool draw_CubeMaps;
		bool draw_VolTextures;
		bool m_bCheckColor[48]; //the max number of pieces used is for cube maps 6x8=48

		LONGLONG	   llFreq, llStart, llEnd, llDif;    
		LONGLONG	   m_PreLoadSum;
		LONGLONG       m_NonPreLoadSum;

		CSurface      *m_pBackBuffer;

		CVertexBuffer *m_pTLVertexBuffer;
		CVertexBuffer *m_pLVertexBuffer;
		CVertexBuffer *m_pVertexBuffer;
		CVertexBuffer *m_pCubeVertexBuffer;
		CVertexBuffer *m_pVolVertexBuffer;

		CIndexBuffer  *m_p16bitIndexBuffer1;
		CIndexBuffer  *m_p16bitIndexBuffer2;
		CIndexBuffer  *m_p32bitIndexBuffer1;
		CIndexBuffer  *m_p32bitIndexBuffer2;
		CIndexBuffer  *m_p16bitTexIndexBuffer1;
		CIndexBuffer  *m_p16bitTexIndexBuffer2;
		CIndexBuffer  *m_p16bitCubeIndexBuffer1;
		CIndexBuffer  *m_p16bitCubeIndexBuffer2;
		CIndexBuffer  *m_p16bitVolIndexBuffer1;
		CIndexBuffer  *m_p16bitVolIndexBuffer2;
		CIndexBuffer  *m_p32bitTexIndexBuffer1;
		CIndexBuffer  *m_p32bitTexIndexBuffer2;
		CIndexBuffer  *m_p32bitCubeIndexBuffer1;
		CIndexBuffer  *m_p32bitCubeIndexBuffer2;
		CIndexBuffer  *m_p32bitVolIndexBuffer1;
		CIndexBuffer  *m_p32bitVolIndexBuffer2;

		CTexture      *m_pTexture;
		CCubeTexture  *m_pCubeTexture;
		CVolTexture   *m_pVolTexture;
};

class ListTest: public ResMan
{
	public:
		ListTest() {};
		~ListTest() {};
		virtual void UpdateStatus();
		void         CommandLineHelp();
		HRESULT      DrawFunction(bool);
		UINT         FillVideoMemory();
		bool         CheckListColors(bool);
		//ResourceGenerationFunctions.cpp
		int         AddTextureToList(UINT, WORD = 400, WORD = 200);
		int         AddCubeMapToList(UINT, WORD = 400, WORD = 200);
		int         AddVolTextureToList(UINT, WORD = 400, WORD = 200);
		int         AddIndexBufferToList(UINT);
		int			AddTLVertexBufferToList();
		int			ChangeTLVertexBufferInList(VBUFFERLIST::iterator, WORD, WORD, FLOAT, FLOAT);
		int			AddLVertexBufferToList();
		int			AddVertexBufferToList();
		HRESULT     AddTextureVertexBufferToList();
		HRESULT 	AddCubeMapVertexBufferToList();
		HRESULT     AddVolVertexBufferToList(UINT uLevels, UINT depth, UINT NumSlices);
		void		DestroyAllResInfosFromList();
		void        DestroyAllVertexBuffersFromList();
		void        DestroyAllIndexBuffersFromList();
		void        DestroyAllTexturesFromList();	    
		void        DestroyAllCubeMapsFromList();
		void        DestroyAllVolTexturesFromList();
		void        DestroyAllDefaultTexList();

	protected:
		TCHAR		m_Message[2000];
		DWORD		m_CurrentOccupiedMemory;
		DWORD		m_CountConstMemoryLeft;
		DWORD		m_CountNegativeMemoryUsage;
		bool        m_FixedEnd;
		bool		m_IsOutOfMemory;
		DWORD       m_MinExhaustNum;
		DWORD       m_PrimCount;

		RESINFOLIST m_ResInfosList;
		TEXTURELIST m_TexturesList;
		TEXTURELIST m_DefaultTexList;
		CUBEMAPLIST	m_CubesList;
		VOLTEXLIST	m_VolumesList;
		IBUFFERLIST m_IBuffersList; 
		VBUFFERLIST m_VBuffersList; 
		RESINFOLIST::iterator startResInfo;
		VBUFFERLIST::iterator startVBuffer;
		IBUFFERLIST::iterator startIBuffer;
		TEXTURELIST::iterator startTexture;
		CUBEMAPLIST::iterator startCubeMap;
		VOLTEXLIST::iterator  startVolTex;
};

class CChangeColorsTest: public ListTest
{
	public:
		CChangeColorsTest();
		~CChangeColorsTest(){};
		UINT TestInitialize(void);
		bool ExecuteTest(UINT);
		bool ProcessFrame(void);
		bool TestTerminate(void);
		void CheckChangeColorsTestCaps(void);
		bool ChangeTextureColors(CTexture *,  DWORD, WORD);
		bool ChangeCubeMapColors(CCubeTexture *, DWORD, WORD);
		bool ChangeVolumeColors(CVolTexture *, DWORD, WORD);
		int  CreateTextures(CTexture *, DWORD);
		int  CreateCubeTextures(CCubeTexture *, DWORD);
		int  CreateVolTextures(CVolTexture *, DWORD);
	private:
		UINT      m_prevTest;
};

class CDiscardBytesTest: public ListTest
{
	public:
		CDiscardBytesTest();
		~CDiscardBytesTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool ExecuteTest(UINT);
		bool ProcessFrame(void);
		bool TestTerminate(void);	
		void UpdateStatus(void);
		void CheckDiscardBytesTestCaps(void);
	private:
		LONGLONG   llFreq, llStart, llEnd, llDif;
		CSurface   *m_pBackBuffer;
		TCHAR		m_Messages[3][2000];
		DWORD		m_NumberOfSlices[3];
};

class CLRUTest: public ListTest
{
	public:
		CLRUTest(PRIM_TYPES types = IS_NULL);
		~CLRUTest(){};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool ExecuteTest(UINT);
		bool ProcessFrame(void);
		bool TestTerminate(void);
	private:
		MEMORYSTATUS Buffer;
		FILE *memfile;
		PRIM_TYPES m_testType;
		BOOL       m_IsHWProcessing;
		DWORD      m_EndIndex;
		DWORD      m_ExtraCount;
		UINT       m_GetMem, m_GetMemPrev, m_TotalUsedMem;
};

class CMRUTest: public ListTest
{
	public:
		CMRUTest(PRIM_TYPES types = IS_NULL);
		~CMRUTest(){};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool ExecuteTest(UINT);
		bool ProcessFrame(void);
		bool TestTerminate(void);
	private:
		PRIM_TYPES m_testType;
		BOOL       m_IsHWProcessing;
		DWORD      m_EndIndex;
		DWORD      m_ExtraCount;
		UINT       m_GetMem, m_GetMemPrev, m_TotalUsedMem;
};

#endif // __ResMan_h__