#ifndef __RENDER_TARGET_TEST_H
#define __RENDER_TARGET_TEST_H

#include "CD3DTest.h"
#include "CSurface.h"
#include "CTextures.h"

struct SVertex
{
    float   rgfPosition[3];
    DWORD   dwColor;
    float   rgfTexCoords[2];
};

#define VERTEX_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#define TEST_ARRAY_SIZE 30

class CNonPowTwoRTTest : public CD3DTest
{
public:
    CNonPowTwoRTTest();
    virtual ~CNonPowTwoRTTest();
    
	virtual UINT TestInitialize();
    virtual bool CapsCheck();
    virtual bool Setup();
    virtual bool ExecuteTest(UINT nTestNum);
    virtual void Cleanup();
	virtual void CommandLineHelp();

protected:
    

    virtual bool RenderTriangle();//renders a triangle on the screen
    virtual bool CreateRTTexture(UINT nWidth, UINT nHeight,
                                 CnTexture** ppTexOut, CnSurface** ppZBufferOut);
	UINT         ForcePOW2(UINT nDimension);
	UINT         ForceDWORDMultiple(UINT nDimension);

protected:
    CnSurface* m_pBackBuffer;
    CnSurface* m_pZBuffer;

	TCHAR	   m_testDesc[80];

	UINT       m_uSizes[TEST_ARRAY_SIZE][2];

	bool         m_bTexPow2;
	bool		 m_bNonPow2Cond;
};



#endif //__RENDER_TARGET_TEST_H
