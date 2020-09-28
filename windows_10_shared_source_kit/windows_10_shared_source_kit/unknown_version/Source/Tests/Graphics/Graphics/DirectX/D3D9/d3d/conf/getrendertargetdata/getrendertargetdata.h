//
//  GetRenderTargetData.h
//

#ifndef _GETRENDERTARGETDATA_H_
#define _GETRENDERTARGETDATA_H_

#include "D3DNodes.h"

/*
 *  Test Group Definitions
 */

class CGetRenderTargetData : public CD3DTest, public CParameters
{
public:
	CGetRenderTargetData( unsigned maxDimension = 1024 );

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

protected:
    BOOL m_bRunInvalid;
    UINT m_nVertices;
	unsigned m_Dimension;
	unsigned m_maxLevels;
    static BOOL m_firstTime;
};

class CBackBuffer : public CGetRenderTargetData
{
public:
	CBackBuffer();

    virtual void 	SetTestType(TESTTYPE Type);
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
    virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);
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

private:
    LPTEXTURES m_pTexture;
    LPVERTEXBUFFERS m_pVertexBuffer;
    LPSURFACES m_pPlainSurface;
};

class CBackBufferTexture : public CGetRenderTargetData
{
public:
	CBackBufferTexture();

    virtual void 	SetTestType(TESTTYPE Type);
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
    virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);
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

private:
    LPTEXTURE m_pTexture;
    LPVERTEXBUFFER m_pVertexBuffer;
    LPTEXTURE m_pTextureSurface;
};

class CBackBufferCubeTexture : public CGetRenderTargetData
{
public:
	CBackBufferCubeTexture();

    virtual void 	SetTestType(TESTTYPE Type);
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
    virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);
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

private:
    LPTEXTURE m_pTexture;
    LPVERTEXBUFFER m_pVertexBuffer;
    LPCUBETEXTURE m_pTextureSurface;
};

class CRenderTarget : public CGetRenderTargetData
{
public:
	CRenderTarget();

    virtual void 	SetTestType(TESTTYPE Type);
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
    virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);
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

private:
    LPTEXTURE m_pTexture;
    LPVERTEXBUFFER m_pVertexBuffer;
    LPSURFACE m_pRenderTarget;
    LPSURFACE m_pPlainSurface;
    LPSURFACE m_pSrcRenderTarget;
};

class CRenderTargetTexture : public CGetRenderTargetData
{
public:
	CRenderTargetTexture();

    virtual void 	SetTestType(TESTTYPE Type);
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
    virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);
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

private:
    LPTEXTURE m_pTexture;
    LPVERTEXBUFFER m_pVertexBuffer;
    LPSURFACE m_pRenderTarget;
    LPTEXTURE m_pMipTexture;
    LPSURFACE m_pSrcRenderTarget;
};

class CRenderTargetCubeTexture : public CGetRenderTargetData
{
public:
	CRenderTargetCubeTexture();

    virtual void 	SetTestType(TESTTYPE Type);
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
    virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);
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

private:
    LPTEXTURE m_pTexture;
    LPVERTEXBUFFER m_pVertexBuffer;
    LPSURFACE m_pRenderTarget;
    LPCUBETEXTURE m_pCubeTexture;
    LPSURFACE m_pSrcRenderTarget;
};

class CTextureSurface : public CGetRenderTargetData
{
public:
	CTextureSurface();

    virtual void 	SetTestType(TESTTYPE Type);
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
    virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);
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

private:
    LPTEXTURE m_pTexture;
    LPVERTEXBUFFER m_pVertexBuffer;
    LPTEXTURE m_pMipTexture;
    LPSURFACE m_pTextureSurface;
    LPSURFACE m_pPlainSurface;
    LPSURFACE m_pSrcRenderTarget;
};

class CCubeTextureSurface : public CGetRenderTargetData
{
public:
	CCubeTextureSurface();

    virtual void 	SetTestType(TESTTYPE Type);
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
    virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);
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

private:
    LPTEXTURE m_pTexture;
    LPVERTEXBUFFER m_pVertexBuffer;
    LPCUBETEXTURE m_pCubeTexture;
    LPSURFACE m_pTextureSurface;
    LPSURFACE m_pPlainSurface;
    LPSURFACE m_pSrcRenderTarget;
};

class CTextureTexture : public CGetRenderTargetData
{
public:
	CTextureTexture();

    virtual void 	SetTestType(TESTTYPE Type);
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
    virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);
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

private:
    LPTEXTURE m_pTexture;
    LPVERTEXBUFFER m_pVertexBuffer;
    LPTEXTURE m_pMipTexture;
    LPSURFACE m_pMipSurface;
    LPTEXTURE m_pTextureSurface;
    LPSURFACE m_pSrcRenderTarget;
};

class CTextureCubeTexture : public CGetRenderTargetData
{
public:
	CTextureCubeTexture();

    virtual void 	SetTestType(TESTTYPE Type);
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
    virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);
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

private:
    LPTEXTURE m_pTexture;
    LPVERTEXBUFFER m_pVertexBuffer;
    LPTEXTURE m_pMipTexture;
    LPSURFACE m_pMipSurface;
    LPCUBETEXTURE m_pCubeTexture;
    LPSURFACE m_pSrcRenderTarget;
};

class CCubeTextureTexture : public CGetRenderTargetData
{
public:
	CCubeTextureTexture();

    virtual void 	SetTestType(TESTTYPE Type);
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
    virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);
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

private:
    LPTEXTURE m_pTexture;
    LPVERTEXBUFFER m_pVertexBuffer;
    LPCUBETEXTURE m_pCubeTexture;
    LPSURFACE m_pTextureSurface;
    LPTEXTURE m_pMipTexture;
    LPSURFACE m_pSrcRenderTarget;
};

class CCubeTextureCubeTexture : public CGetRenderTargetData
{
public:
	CCubeTextureCubeTexture();

    virtual void 	SetTestType(TESTTYPE Type);
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
    virtual void    BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber);
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

private:
    LPTEXTURE m_pTexture;
    LPVERTEXBUFFER m_pVertexBuffer;
    LPCUBETEXTURE m_pCubeTexture;
    LPSURFACE m_pTextureSurface;
    LPCUBETEXTURE m_pMipTexture;
    LPSURFACE m_pSrcRenderTarget;
};

class COddBalls : public CD3DTest
{
public:
	COddBalls();
    virtual bool CapsCheck();
    virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
};

#endif
