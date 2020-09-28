#ifndef __Tessellation_h__
#define __Tessellation_h__

#include "d3dx9.h"
#include "CD3DTest.h"
#include "Types.h"
#include "CBuffers.h"
#include "CTextures.h"

const UINT SURFACEHANDLEBASE = 1;

#define MAX_TESS_LEVEL 9
#define MAX_VIEW 5
#define MAX_DEGREE 4

struct VERTEX
{
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
	D3DVALUE    tu, tv;

	VERTEX() { }
	VERTEX( const D3DXVECTOR3& v, const D3DXVECTOR3& n, D3DVALUE _u, D3DVALUE _v )
	{     
		position = v;
		normal = n;
		tu = _u;  tv = _v;
    }
};
const int FVF_VERTEX = D3DFVF_XYZ | D3DFVF_NORMAL |  D3DFVF_TEX1;

struct TESSVERTEX
{
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
	D3DVALUE    tu, tv;
	float		tessFactor;

	TESSVERTEX() { }
    TESSVERTEX(const D3DXVECTOR3& v, const D3DXVECTOR3& n, D3DVALUE _u, D3DVALUE _v, float factor)
	{  
		position = v;
		normal = n;
		tu = _u; tv = _v;
	    tessFactor = factor;
    }
};

typedef union _TESTCASE
{
	struct
    {
		UINT View : 3;        //max 7 - we have 5 views
        UINT Degree : 3;      //max 7 - we have 4 degrees
		UINT Basis : 3;       //max 7 - we have 3 basises
		UINT NumPatches : 3;  //max 7 -> can have 1 << 6 = 32 patches
		UINT TessLevels : 7; //the highest tess level you can have is 127
		UINT VPMethod : 3;   
     };
     UINT nTest;
} TESTCASE, *LPTESTCASE;

typedef union _MAXMINTESTCASE
{
	struct
    {
		UINT View : 3;        //max 7 - we have 5 views
        UINT Degree : 3;      //max 7 - we have 4 degrees
		UINT Basis : 3;       //max 7 - we have 3 basises
		UINT NumPatches : 3;  //max 7 -> can have 1 << 6 = 32 patches
		UINT TessLevels : 7;  //the highest tess level you can have is 100
		UINT MaxMinTessLevels : 8;
     };
     UINT nTest;
} MAXMINTESTCASE, *LPMAXMINTESTCASE;

typedef union _DATATESTCASE
{
	struct
    {
		UINT View : 3;        //max 7 - we have 5 views
        UINT Degree : 3;      //max 7 - we have 4 degrees
		UINT Basis : 3;       //max 7 - we have 3 basises
		UINT NumPatches : 3;  //max 7 -> can have 1 << 6 = 32 patches
		UINT TessLevels : 5; //max 31
		UINT Method : 3; //max 7
		UINT DataType : 5;   //max 31
     };
     UINT nTest;
} DATATESTCASE, *LPDATATESTCASE;

typedef union _NEWTESTCASE
{
	struct
    {
		UINT isEntire   : 1;   //only need to values: solid and 4 pathces 
		UINT NormDegree : 2;   //only need to values: LINEAR and QUADRATIC
		UINT PosDegree  : 2;   //only need to values: LINEAR and CUBIC
		UINT PatchStyle : 1;   //only need to values: DISCRETE and CONTINUOUS
		UINT TessLevels : 5;   //max 31
		UINT VPMethod   : 2;   //need 4 methods
     };
     UINT nTest;
} NEWTESTCASE, *LPNEWTESTCASE;

class Tessellation: public CD3DTest
{
public:
     Tessellation();
	 ~Tessellation() {};

	virtual bool Setup(); 
	virtual void Cleanup();
	virtual bool CapsCheck();
	virtual bool SetDefaultMatrices(void);
	virtual bool SetDefaultRenderStates(void);
	virtual bool SetDefaultLights(void);
	void SetNewMaterial(UINT index);
    //virtual void CommandLineHelp(void);

protected:
	TCHAR			msgString[100];
	LIGHT		    m_pLights[3];
	float           m_MaxTess;
	D3DXMATRIX		m_ProjMatrix, m_WorldMatrix;
	D3DXMATRIX		m_ViewMatrices[MAX_VIEW];
	int 			m_nViewMatrices;
	D3DXMATRIX		m_WVMatrices[5];
	int 			m_nWVMatrices;
	MATERIAL	  	m_HOMat;
	bool			m_AdapTess;
	bool            m_UseDeclShader;
	bool            m_UseFVFShader;
	bool            m_UseFVFFixed;
	bool            m_UseDeclFixed;
	bool            m_UseQuinticRT;
	D3DBASISTYPE    Basises[3];
	TCHAR          *BasisStr[3];
	D3DDEGREETYPE   Degrees[MAX_DEGREE];
	TCHAR          *DegreeStr[MAX_DEGREE];
	CnTexture      *m_pTexture;
	float           m_TessLevels[4][3];
	CnVertexShader *m_pShader;
	CnVertexDeclaration *m_pDecl;
};

class CRectPatchesTest: public Tessellation
{
	public:
		CRectPatchesTest();
		~CRectPatchesTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ExecuteTest(UINT);
		bool GenerateBuffers(TESTCASE tc);
		bool GenerateVertexShader();

	private:
		CnVertexBuffer *m_pVB;
		float          *m_Tess;
		UINT      	    m_nVertices;
		UINT            m_nPatchesPerRow;
		int				m_numPatches;
		WORD			m_nBasises;
		WORD			m_nDegrees;
		WORD			m_nTessLevels;
		UINT			m_uPatchHeight, m_uBufferHeight;
		UINT			m_uPatchWidth,  m_uBufferWidth;
		TESTCASE		tcCount;
};

class CTriPatchesTest: public Tessellation
{
	public:
		CTriPatchesTest();
		~CTriPatchesTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ExecuteTest(UINT);
		bool GenerateBuffers(TESTCASE tc);
		bool GenerateVertexShader();

	private:
		CnVertexBuffer  *m_pVB;
		UINT      		 m_nVertices;
		UINT             m_nPatchesPerRow;
		int				m_numPatches;
		WORD			m_nBasises;
		WORD			m_nDegrees;
		WORD			m_nTessLevels;
		UINT			m_uPatchHeight, m_uBufferHeight;
		UINT			m_uPatchWidth,  m_uBufferWidth;
		TESTCASE		tcCount;
};

class CNPatchesTest: public Tessellation
{
	public:
		CNPatchesTest();
		~CNPatchesTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ExecuteTest(UINT);
		bool GenerateBuffers(TESTCASE tc, D3DPRIMITIVETYPE);
		bool GenerateVertexShader();

	private:
		CnVertexBuffer  *m_pVB;
		CnIndexBuffer   *m_pIB;
		UINT      		 m_nVertices;
		UINT             m_nPatchesPerRow;
		int				m_numPatches;
		WORD			m_nBasises;
		WORD			m_nDegrees;
		WORD			m_nTessLevels;
		UINT			m_uPatchHeight, m_uBufferHeight;
		UINT			m_uPatchWidth,  m_uBufferWidth;
		TESTCASE		tcCount;
};

class CMinimizingRectTest: public Tessellation
{
	public:
		CMinimizingRectTest();
		~CMinimizingRectTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool SetDefaultMatrices(void);
		bool ExecuteTest(UINT);
		bool GenerateBuffers(TESTCASE tc);
		bool GenerateVertexShader();

	private:
		CnVertexBuffer *m_pVB;
		UINT      	    m_nVertices;
		UINT            m_nPatchesPerRow;
		int				m_numPatches;
		WORD			m_nBasises;
		WORD			m_nDegrees;
		WORD			m_nTessLevels;
		UINT			m_uPatchHeight, m_uBufferHeight;
		UINT			m_uPatchWidth,  m_uBufferWidth;
		TESTCASE		tcCount;
};

class CMinimizingTriTest: public Tessellation
{
	public:
		CMinimizingTriTest();
		~CMinimizingTriTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool SetDefaultMatrices(void);
		bool ExecuteTest(UINT);
		bool GenerateBuffers(TESTCASE tc);
		bool GenerateVertexShader();

	private:
		CnVertexBuffer *m_pVB;
		UINT      	    m_nVertices;
		UINT            m_nPatchesPerRow;
		int				m_numPatches;
		WORD			m_nBasises;
		WORD			m_nDegrees;
		WORD			m_nTessLevels;
		UINT			m_uPatchHeight, m_uBufferHeight;
		UINT			m_uPatchWidth,  m_uBufferWidth;
		TESTCASE		tcCount;
};

class CMaxMinRectTest: public Tessellation
{
	public:
		CMaxMinRectTest();
		~CMaxMinRectTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ProcessFrame(void);
		bool ExecuteTest(UINT);
		bool GenerateBuffers(MAXMINTESTCASE tc);
		void ChooseTessLevels(MAXMINTESTCASE tc);
		bool GenerateVertexShader();

	private:
		bool            m_doComparison;
		CnVertexBuffer *m_pVB;
		UINT      	    m_nVertices;
		UINT            m_nPatchesPerRow;
		int				m_numPatches;
		WORD			m_nBasises;
		WORD			m_nDegrees;
		float           m_MaxMinTessLevels[MAX_TESS_LEVEL][2];
		float			m_TessLevels[14];
		WORD			m_nTessLevels;
		WORD			m_nMaxMinTessLevels;
		UINT			m_uPatchHeight, m_uBufferHeight;
		UINT			m_uPatchWidth,  m_uBufferWidth;
		MAXMINTESTCASE	tcCount;
};

class CMaxMinTriTest: public Tessellation
{
	public:
		CMaxMinTriTest();
		~CMaxMinTriTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ProcessFrame(void);
		bool ExecuteTest(UINT);
		bool GenerateBuffers(MAXMINTESTCASE tc);
		void ChooseTessLevels(MAXMINTESTCASE tc);
		bool GenerateVertexShader();

	private:
		bool            m_doComparison;
		CnVertexBuffer *m_pVB;
		UINT      	    m_nVertices;
		UINT            m_nPatchesPerRow;
		int				m_numPatches;
		WORD			m_nBasises;
		WORD			m_nDegrees;
		float           m_MaxMinTessLevels[MAX_TESS_LEVEL][2];
		float			m_TessLevels[12];
		WORD			m_nTessLevels;
		WORD			m_nMaxMinTessLevels;
		UINT			m_uPatchHeight, m_uBufferHeight;
		UINT			m_uPatchWidth,  m_uBufferWidth;
		MAXMINTESTCASE	tcCount;
};

class CTessFactorRectTest: public Tessellation
{
	public:
		CTessFactorRectTest();
		~CTessFactorRectTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ExecuteTest(UINT);
		bool GenerateBuffers(TESTCASE tc);
		bool GenerateVertexShader();

	private:
		CnVertexBuffer *m_pVB;
		CnIndexBuffer  *m_pIB;
		UINT      		m_nVertices;
		UINT            m_nPatchesPerRow;
		int				m_numPatches;
		WORD			m_nBasises;
		WORD			m_nDegrees;
		WORD			m_nTessLevels;
		UINT			m_uPatchHeight, m_uBufferHeight;
		UINT			m_uPatchWidth,  m_uBufferWidth;
		TESTCASE		tcCount;
};

class CTessFactorTriTest: public Tessellation
{
	public:
		CTessFactorTriTest();
		~CTessFactorTriTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ExecuteTest(UINT);
		bool GenerateBuffers(TESTCASE tc);
		bool GenerateVertexShader();

	private:
		CnVertexBuffer *m_pVB;
		CnIndexBuffer  *m_pIB;
		UINT      		m_nVertices;
		UINT            m_nPatchesPerRow;
		int				m_numPatches;
		WORD			m_nBasises;
		WORD			m_nDegrees;
		WORD			m_nTessLevels;
		UINT			m_uPatchHeight, m_uBufferHeight;
		UINT			m_uPatchWidth,  m_uBufferWidth;
		TESTCASE		tcCount;
};

class CDataTypesRectTest: public Tessellation
{
	public:
		CDataTypesRectTest();
		~CDataTypesRectTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		virtual bool CapsCheck();
		bool ExecuteTest(UINT);
		bool GenerateBuffers(DATATESTCASE tc);
		bool GenerateVertexShader(DATATESTCASE tc);

	private:
		CnVertexBuffer  *m_pVB;
		UINT      		 m_nVertices;
		UINT             m_nPatchesPerRow;
		int				 m_numPatches;
		WORD             m_StreamSize;
		WORD             m_nDataTypes;
		WORD             m_nMethods;
		WORD			 m_nBasises;
		WORD			 m_nDegrees;
		WORD			 m_nTessLevels;
		UINT			 m_uPatchHeight, m_uBufferHeight;
		UINT			 m_uPatchWidth,  m_uBufferWidth;
		DATATESTCASE	 tcCount;
};

class CDataTypesTriTest: public Tessellation
{
	public:
		CDataTypesTriTest();
		~CDataTypesTriTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ExecuteTest(UINT);
		bool GenerateBuffers(DATATESTCASE tc);
		bool GenerateVertexShader(DATATESTCASE tc);

	private:
		CnVertexBuffer  *m_pVB;
		UINT      		 m_nVertices;
		UINT             m_nPatchesPerRow;
		int				 m_numPatches;
		WORD             m_StreamSize;
		WORD             m_nDataTypes;
		WORD             m_nMethods;
		WORD			 m_nBasises;
		WORD			 m_nDegrees;
		WORD			 m_nTessLevels;
		UINT			 m_uPatchHeight, m_uBufferHeight;
		UINT			 m_uPatchWidth,  m_uBufferWidth;
		DATATESTCASE	 tcCount;
};

class CDataTypesNTest: public Tessellation
{
	public:
		CDataTypesNTest();
		~CDataTypesNTest() {};
		bool Setup(); 
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ExecuteTest(UINT);
		bool GenerateBuffers(DATATESTCASE tc, D3DPRIMITIVETYPE);
		bool GenerateVertexShader(DATATESTCASE tc);

	private:
		CnVertexBuffer  *m_pVB;
		CnIndexBuffer   *m_pIB;
		UINT      		 m_nVertices;
		UINT             m_nPatchesPerRow;
		int				 m_numPatches;
		WORD             m_StreamSize;
		WORD             m_nDataTypes;
		WORD			 m_nBasises;
		WORD			 m_nDegrees;
		WORD			 m_nTessLevels;
		DWORD           *m_dwStateBlockHandles;
		UINT			 m_uPatchHeight, m_uBufferHeight;
		UINT			 m_uPatchWidth,  m_uBufferWidth;
		DATATESTCASE	 tcCount;

};

class CNewNTest: public Tessellation
{
	public:
		CNewNTest();
		~CNewNTest() {};
		bool Setup();
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ExecuteTest(UINT);
		bool ProcessFrame(void);
		bool GenerateBuffers(float tessLevel);
		bool GenerateVertexShader();
		void SetNewMaterial(UINT index);
		void TransformVertex(D3DXMATRIX Matrix, D3DXVECTOR3 *Vertices, D3DXVECTOR3 *NewVertices, DWORD Size);


	private:
		VERTEX     *m_pVCircle, *m_pVEllipse;
		WORD       *m_pIEllipse;
		UINT        m_nVertices;
		int		    m_numPatches;
		DWORD	    m_nTessLevels;
		WORD		m_nDegrees;
		DWORD       m_NumTri, m_NumVertices;
		int         m_iX[4], m_iY[4];
		D3DXMATRIX  m_FinalMatrix;
		NEWTESTCASE tcCount;

};

class CMinNTest: public Tessellation
{
	public:
		CMinNTest();
		~CMinNTest() {};
		bool Setup();
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ExecuteTest(UINT);
		bool ProcessFrame(void);
		bool GenerateBuffers();
		bool GenerateVertexShader();
		void SetNewMaterial(UINT index);
		void TransformVertex(D3DXMATRIX Matrix, D3DXVECTOR3 *Vertices, D3DXVECTOR3 *NewVertices, DWORD Size);


	private:
		VERTEX     *m_pVCircle, *m_pVEllipse;
		WORD       *m_pICircle, *m_pIEllipse;
		UINT        m_nVertices;
		int		    m_numPatches;
		DWORD	    m_nTessLevels;
		WORD		m_nDegrees;
		DWORD       m_NumTri, m_NumVertices;
		int         m_iX[4], m_iY[4];
		D3DXMATRIX  m_FinalMatrix;
		NEWTESTCASE tcCount;

};

class CTessFNTest: public Tessellation
{
	public:
		CTessFNTest();
		~CTessFNTest() {};
		bool Setup();
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ExecuteTest(UINT);
		bool ProcessFrame(void);
		bool GenerateBuffers();
		bool GenerateVertexShader();
		void SetNewMaterial(UINT index);
		void TransformVertex(D3DXMATRIX Matrix, D3DXVECTOR3 *Vertices, D3DXVECTOR3 *NewVertices, DWORD Size);


	private:
		TESSVERTEX *m_pVCircle, *m_pVEllipse;
		WORD       *m_pICircle, *m_pIEllipse;
		UINT        m_nVertices;
		int		    m_numPatches;
		DWORD	    m_nTessLevels;
		WORD		m_nDegrees;
		DWORD       m_NumTri, m_NumVertices;
		int         m_iX[4], m_iY[4];
		D3DXMATRIX  m_FinalMatrix;
		NEWTESTCASE tcCount;
};

class CMinMaxNTest: public Tessellation
{
	public:
		CMinMaxNTest();
		~CMinMaxNTest() {};
		bool Setup();
		void Cleanup();
		UINT TestInitialize(void);
		bool CapsCheck();
		bool ExecuteTest(UINT);
		bool GenerateBuffers();
		bool GenerateVertexShader();

	private:
		VERTEX     *m_pVCircle, *m_pVEllipse;
		WORD       *m_pICircle, *m_pIEllipse;
		UINT        m_nVertices;
		int		    m_numPatches;
		DWORD	    m_nTessLevels;
		WORD		m_nDegrees;
		DWORD       m_NumTri, m_NumVertices;
		NEWTESTCASE tcCount;
};

#endif // __Tessellation_h__