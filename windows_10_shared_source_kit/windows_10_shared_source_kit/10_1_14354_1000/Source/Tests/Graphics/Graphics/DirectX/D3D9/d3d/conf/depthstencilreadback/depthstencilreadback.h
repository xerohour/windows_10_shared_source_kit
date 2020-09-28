//
//  CDepthStencilReadBack.h
//
#ifndef _DEPTHSTENCILREADBACK_H_
#define _DEPTHSTENCILREADBACK_H_

#include <vector>
#include <algorithm>

#include "CD3DTest.h"
#include "CSurface.h"
#include "CBuffers.h"


/*
 *  CProportions - utility class with RECT and POINT proportions (all between [0,1])
 */

class CProportions
{
public:
    CProportions();
    CProportions( float left,
                  float top,
                  float right,
                  float bottom,
                  float x,
                  float y );
    RECT &getRect( LONG width, LONG height, RECT &rect ) const;
    POINT &getPoint( LONG width, LONG height, POINT &point ) const;
protected:
    float m_Left;
    float m_Top;
    float m_Right;
    float m_Bottom;
    float m_X;
    float m_Y;
};


/*
 *  Test Group Definitions
 */

class CDepthStencilReadBack_Support : public CD3DTest
{
public:
    CDepthStencilReadBack_Support();

    virtual bool 	Setup() {return CD3DTest::Setup();};

    virtual bool    ExecuteTest(UINT);

    virtual UINT TestInitialize(void)
    {
        SetTestRange( 1, 2 );
        return CD3DTest::TestInitialize();
    };

    bool IsDepthStencilFormatSupported( FORMAT fmtDepthStencil );
    bool IsSurfaceFormatSupported( FORMAT fmt );
};

class CDepthStencilReadBack : public CDepthStencilReadBack_Support
{
public:
	CDepthStencilReadBack();

	virtual void	SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast);
    virtual bool	FilterFormats(FORMAT *pFmtIn, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *uCount);
    virtual bool	FilterResolution(PDISPLAYMODE pMode);
	virtual bool	CapsCheck();
    virtual bool 	Setup();
    virtual UINT    TestInitialize(void);
    virtual bool    ClearFrame(void);
    virtual bool    ExecuteTest(UINT);
    virtual bool    DisplayFrame(void);
    virtual void	UpdateStatus();
	virtual void	UpdateDetails();
    virtual void    EndTestCase(void);
    virtual bool    TestTerminate(void);
	virtual void	Cleanup();

	virtual bool    ProcessFrame(void);
	virtual void	OnDeviceLost();
    virtual void	OnReset();
    virtual bool	DrawStats(int nHeight, int nWidth, CDevice *pDevice);

    virtual void    SceneRefresh(void);
	virtual bool 	SetDefaultMaterials();
    virtual bool 	SetDefaultTextures();
    virtual bool 	SetDefaultViewport();
    virtual bool 	SetDefaultLights();
    virtual bool 	SetDefaultRenderStates();
    virtual bool 	SetDefaultMatrices();

    virtual void    CommandLineHelp();
    virtual bool	ParseCmdLine();

    bool AddFormatConversion( FORMAT fmtSrc, FORMAT fmtDes );

protected:
    BOOL m_bRunInvalid;
    LPSURFACES m_pDepthStencilSurface;
    LPSURFACES m_pSrcSurface;
    LPSURFACES m_pDestSurface;

    typedef std::vector< std::pair< FORMAT, FORMAT > > FormatSetType;
    FormatSetType m_Formats;
    typedef std::vector< CProportions > SubSetType;
    SubSetType m_Subsets;
};

#endif
