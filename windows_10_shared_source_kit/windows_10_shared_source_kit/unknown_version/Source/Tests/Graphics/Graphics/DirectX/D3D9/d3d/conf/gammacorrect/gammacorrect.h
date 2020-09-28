#ifndef __GammaCorrect_h__
#define __GammaCorrect_h__

#include <d3dx9.h>
#include "Types.h"
#include "CD3DTest.h"
#include "CBuffers.h"
#include "CTextures.h"
#include <list>
using namespace std;

#ifdef FAST_TEST
#define MAX_LEVEL_COUNT 2
#else
#define MAX_LEVEL_COUNT 5
#endif

struct VOLVERTEX
{
    D3DVALUE    x,   y,  z;
	D3DVALUE    nx, ny, nz;
	D3DVALUE    tu, tv, tw;

	VOLVERTEX() { }
    VOLVERTEX(const D3DXVECTOR3& v, D3DXVECTOR3& n, const D3DXVECTOR3& t)
	{
		x = v.x;   y = v.y;  z = v.z; 
	    nx = n.x; ny = n.y; nz = n.z;
		tu = t.x; tv = t.y; tw = t.z;
    }
};
const int FVF_VOLVERTEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

class GammaCorrect: public CD3DTest
{
public:
     GammaCorrect(DWORD = 0, RESOURCETYPE = RTYPE_TEXTURE, D3DPOOL = D3DPOOL_MANAGED);
	 ~GammaCorrect() {};

#ifdef FAST_TEST
     bool AddFormatModes(DISPLAYMODE *pMode, bool bWindowed);
#endif 

	virtual bool CapsCheck(void);
	virtual bool PreModeSetup();
	virtual bool Setup();
	virtual bool SetDefaultMatrices();
	virtual bool SetDefaultRenderStates();
	virtual void Cleanup();
    virtual void CommandLineHelp(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	
	bool GenerateTextures();
	bool GenerateCubeTextures();
	bool GenerateVolTextures();
	bool GenerateVertexBuffer();
	bool GenerateCubeVertexBuffer();
	bool GenerateVolVertexBuffer();

private:
	TCHAR            szTestName[ 120 ];
	TCHAR            szCommandKey[ 30 ];
	TCHAR			 msgString[ 120 ];
	CnVertexBuffer  *m_pVertexBuffer;
	CnBaseTexture   *m_pTexture1, *m_pTexture2;
	DWORD            m_nTextureOp, m_curTextureOp;
	bool			 m_isWrite, m_isSrcWrite, m_isRefWrite;
	bool			 m_isRead, m_isSrcRead, m_isRefRead;
	FORMAT           m_Format;
	RESOURCETYPE     m_RType;
	D3DPOOL          m_Pool;
	UINT			 m_uLevels;
	UINT			 m_LevelIndex;
	bool             m_isLDDMDriver;
	bool             m_bCapsFileExisted;
	TCHAR            m_szCapsFileValue[ 300 ];
};

#endif // __GammaCorrect_h__