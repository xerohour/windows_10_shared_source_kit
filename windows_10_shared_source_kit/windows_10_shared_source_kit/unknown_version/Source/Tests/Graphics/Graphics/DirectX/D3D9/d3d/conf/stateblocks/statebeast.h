#ifndef _STATEBEAST_H_
#define _STATEBEAST_H_

#include <d3dx8.h>
#include "Framework.h"
#include "CShapes.h"
#include "CTextures.h"

// DX9 defines
#define D3DPRASTERCAPS_SCISSORTEST            0x01000000L
#define D3DRS_SCISSORTESTENABLE				  (D3DRENDERSTATETYPE)174

#define NTEXTURES 9
#define NLIGHTS   5

#define D3DSBT_CUSTOM 4

struct FLAGS
{
	DWORD dwFlags;
	TCHAR tcsDesc[1024];
};

struct SVIEWPORT
{
	DWORD dwWidth, dwHeight;
	DWORD dwX, dwY;
	float dwMinZ, dwMaxZ;
};

struct BLOCKINFO
{
	int   nVersion;
    DWORD dwSrcHandle;
    DWORD dwRefHandle;
	DWORD dwCreateType;
	TCHAR tcsDesc[MAX_PATH];
};

typedef struct BEAST  _BEAST;
typedef struct OBJECT _OBJECT;

class CStateBeast : public CD3DTest
{
public:

    VIEWPORT  	 *m_pViewports;
    int           m_nViewports;

	CShapes		  m_FrontPlane;
    CShapes		  m_FrontPlane2;
    CShapes		  m_BackPlane;
    MATERIAL    *m_pFrontMat, *m_pFrontMat2;
    MATERIAL    *m_pBackMat;


	// State blocks data
    BLOCKINFO	 *m_pBlocks;
    int          m_nCurrentBlock;
	int			 m_nTotalBlocks;
	int			 m_nStates;
    BLOCKINFO    m_OriginalBlock;
    
    // App vars
	bool			m_bCmp;					// Compare against SetRenderState
    bool            m_bSWEmulation;         // Allow SW emulation for test
    DWORD 			m_dwPosLightsSupported; // Positions lights supported?

    // Rendering objects and vars
    D3DXVECTOR3   *m_pvLoc;                   // Camera location
    D3DXVECTOR3   *m_pvUp;                    // VUP
    D3DXMATRIX   *m_pVMatrix;
    D3DXMATRIX   *m_pProjMatrix;
    LIGHT       *m_pLight[NLIGHTS];
	LPTEXTURES  m_pTextures[NTEXTURES];

	// DX8 components
	DWORD			m_dwFVF;
	DWORD		 	m_dwVSShader, m_dwVSShader2, m_dwVSShader3; 
	DWORD			m_dwPSShader, m_dwPSShader3;
	CnVertexBuffer *m_pVB;
	CnIndexBuffer  *m_pIB;


public: // class functions
	CStateBeast();
	~CStateBeast();

    UINT TestInitialize(void);
    bool ExecuteTest(UINT);

    
	// Framework overloads
	bool SetDefaultMatrices(void);
	bool SetDefaultMaterials(void);
    bool SetDefaultLights(void);
	bool SetDefaultRenderStates(void);
	void CommandLineHelp(void);
    bool PreDeviceSetup(bool fReference);
	bool Setup();
	void Cleanup();


    // Statebeast code
	HRESULT LoadTextures(void);
	void ClearState();
    void PostApply(UINT nBlock);
	bool SetState(int nIndex, char *szName, int *nVersion, DWORD dwFlags=CD3D_BOTH);
    void CreateSB(UINT nTest);
    void ReleaseTest();

	// DX8 and above
	bool SetupBuffers(CShapes *pShapes, CnVertexBuffer **ppVB, CnIndexBuffer **ppIB);
    bool CreateShaders();

};

#endif
