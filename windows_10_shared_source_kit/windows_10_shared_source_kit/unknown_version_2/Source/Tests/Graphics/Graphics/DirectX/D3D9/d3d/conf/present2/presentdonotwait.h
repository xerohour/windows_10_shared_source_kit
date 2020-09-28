// PresentDoNotWait.h: interface for the CPresentDoNotWait class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRESENTDONOTWAIT_H__09091F95_2147_4FCF_AD00_7626E00806B9__INCLUDED_)
#define AFX_PRESENTDONOTWAIT_H__09091F95_2147_4FCF_AD00_7626E00806B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d9.h>
#include <d3dx9.h>
#include "CD3DTest.h"
#include <vector>
#include "CShapes.h"
#include "CBuffers.h"
#include "CTextures.h"

struct FVFVERTEX
{
	float x, y, z;
	float nx, ny, nz;
	float tu, tv;
};

#define FVFVERTEXHANDLE	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

class CPresentDoNotWait : public CD3DTest
{
public:
	CPresentDoNotWait();

	virtual bool	CapsCheck();						// Check caps
	virtual UINT	TestInitialize();
	bool			TestTerminate();					// cleanup
	virtual bool	ExecuteTest(UINT);
	virtual void	CommandLineHelp();					// command line help for the test
    virtual bool    DisplayFrame(){ return true;};

	bool SetDefaultMatrices(float width, float height);
	bool SetDefaultMaterials();
	bool SetDefaultRenderStates();
	bool SetDefaultLights();

protected:
    bool    m_bFullscreen;
    DWORD    m_swapEffect;

protected:
	void onCreate();
	virtual void onDestroy(); // release the pointers
	void onDraw(); // draw the scene
    bool IsDWMEnabled();
	
	// helper methods
	void Check_HRFail( HRESULT hResult, const char* sMethod );	// if hResult is not D3D_OK, throws an exception

	std::vector< DWORD > m_vecPresentIntervals;

	MATERIAL				m_sMaterial;
	LIGHT					m_sLight;
	LPVERTEXBUFFER			m_pVB;
	LPTEXTURE				m_pTexture;
};


class CPresentDoNotWaitFullScreen : public CPresentDoNotWait
{
public:
	CPresentDoNotWaitFullScreen()
    {
        m_bFullscreen = true;
        m_swapEffect = SWAPEFFECT_FLIP;
        m_szCommandKey	= _T("PresentDoNotWaitFullScreen");
	    m_szTestName	= _T("PresentDoNotWait - FullScreen");
    };
};

class CPresentDoNotWaitWindowed : public CPresentDoNotWait
{
public:
	CPresentDoNotWaitWindowed()
    {
        m_bFullscreen = false;
        m_swapEffect = SWAPEFFECT_FLIPEX;
        m_szCommandKey	= _T("PresentDoNotWaitWindowed");
	    m_szTestName	= _T("PresentDoNotWait - Windowed");
        m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
    };
};

#endif // !defined(AFX_PRESENTDONOTWAIT_H__09091F95_2147_4FCF_AD00_7626E00806B9__INCLUDED_)
