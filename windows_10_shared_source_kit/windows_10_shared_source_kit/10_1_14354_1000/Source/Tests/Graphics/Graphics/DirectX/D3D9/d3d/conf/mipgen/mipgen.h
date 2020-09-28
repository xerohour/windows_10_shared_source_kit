/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	MipGen.h
 *
 ***************************************************************************/

#ifndef __MIPGEN_H__
#define __MIPGEN_H__

#include <vector>

#include "CD3DTest.h"
#include "CTestFactor.h"
#include "CTextures.h"

// Defines
#define MAX_TESTS	278

#define TYPE_TEXTURE		0
#define TYPE_CUBETEXTURE	1

#define FUNC_LOCK				0x0001
#define FUNC_GETDC				0x0002
#define FUNC_UPDATESURFACE		0x0004		
#define FUNC_STRETCHRECT		0x0008		
#define FUNC_COLORFILL			0x0010	
#define FUNC_UPDATETEXTURE		0x0020		
#define FUNC_SETRENDERTARGET	0x0040			
#define FUNC_LOSTDEVICE			0x0080
#define FUNC_EVICTMANAGED		0x0100			

#define PATTERN_1X1				0x0001
#define PATTERN_16X16			0x0002
#define PATTERN_17X17			0x0004
#define PATTERN_FUNKYCHECKER	0x0008
#define PATTERN_DIAGONALS		0x0010
#define PATTERN_COLORFILL		0x0020

// structures for our textures cases
struct TEXTURE_USAGE
{
	float fMinDX;
	float fMaxDX;
	DWORD dwUsage;
	TCHAR szUsage[255];
};

struct TEXTURE_POOL
{
	float fMinDX;
	float fMaxDX;
	DWORD dwPool;
	TCHAR szPool[20];
};

struct TEXTURE_FILTER
{
	float fMinDX;
	float fMaxDX;
	UINT  uIndex;
	DWORD dwFilter;
	TCHAR szFilter[20];
};
#define MAXFILTER 5

struct TEXTURE_PATTERN
{
	DWORD dwPattern;
	TCHAR szPattern[40];
};

struct TEXTURE_GENFUNC
{
	float fMinDX;
	float fMaxDX;
	DWORD dwGenFunc;
	TCHAR szGenFunc[255];
};

struct TEXTURE_FORMAT
{
	float fMinDX;
	float fMaxDX;
	UINT uBPP;
	FMT format;
	TCHAR szFormat[20];
};

struct TEXTURE_CASE
{
	DWORD dwUsage;
	TCHAR szUsage[255];
	FMT  format;
	UINT uBPP;
	TCHAR szFormat[20];
	DWORD dwPool;
	TCHAR szPool[20];
	DWORD dwPattern;
	TCHAR szPattern[40];
	DWORD dwAutoGenFilter;
	TCHAR szAutoGenFilter[20];
	UINT  uAutoGenFilterIndex;
	DWORD dwDisplayFilter;
	TCHAR szDisplayFilter[20];
	UINT  uDisplayFilterIndex;
	DWORD dwGenFunc;
	TCHAR szGenFunc[255];
	bool bCreateTexture;
	bool bSetAutoGenFilter;
	bool bCreatePatternSurface;
	bool bColorFillTopLevel;
	bool bLockable;
	bool bUseOffscreenPlain;
	bool bUsePlainRT;
	bool bUseRTTexture;
	bool bUseSubRect;
};

extern TEXTURE_FILTER g_filters[];
extern const UINT cFilter;

extern TEXTURE_FORMAT g_formats[];
extern const UINT cFormat;

extern TEXTURE_PATTERN g_patterns[];
extern const UINT cPattern;

// Base Class definitions
class CMipGenTest: public CD3DTest
{
// Data
protected:
	typedef struct _MYDISPLAYVERTEX 
	{
		FLOAT     x;             
		FLOAT     y;
		FLOAT     z;
		FLOAT	  rhw;
		FLOAT     tu;         
		FLOAT     tv;
		FLOAT     tw;
	} MYDISPLAYVERTEX;

	//New members
	float			m_fDXVersion;

	RESOURCETYPE	m_RTYPECurrent;
	TEXTURE_CASE	*m_pCurrentCase;
	UINT			m_uCurrentCase;
	UINT			m_uCurrentFrame;
	UINT			m_uCurrentMipLevel;
	bool			m_bRenderEachMip;

	bool			m_bShowTopLevel;

	bool			m_bTexFilSupported[MAXFILTER];
	bool			m_bTexFilSupportedRef[MAXFILTER];
	bool			m_bMagTexFilSupported[MAXFILTER];
	bool			m_bMagTexFilSupportedRef[MAXFILTER];

	bool			m_bGuardband;

	std::vector <TEXTURE_CASE *>	m_listCases;

	virtual void DumpTestDetails() = 0;

public:
	CMipGenTest();
	~CMipGenTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool TestTerminate(void);
	virtual bool ParseCmdLine();
	bool ProcessFrame();

	// Helper functions
	bool WritePatternOnSurface(LPSURFACE pSurface, DWORD dwPattern, UINT uFace = 0);
	bool DrawTexture(float x, float y, UINT size, DWORD dwDevice = CD3D_ALL);
	bool DrawCubeTextureFace(float x, float y, UINT size, int face, DWORD dwDevice = CD3D_ALL);
	bool RenderTextureScene();
	bool RenderCubeTextureScene();
	bool CheckTextureCaps();
	bool CheckCubeTextureCaps();

	virtual void GenerateCaseList() = 0;
};

// Class for emulation techniques
class CMipGenEmulation : public CMipGenTest
{
protected:
	LPBASETEXTURES	m_pTextures;

	virtual void DumpTestDetails();

public:
	CMipGenEmulation();
	~CMipGenEmulation();

	// Framework functions
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
    virtual void UpdateStatus(void);
	virtual void CommandLineHelp();

	void GenerateCaseList();

	// Main functions
	virtual bool CreateBaseTexture() = 0;
	virtual bool CreatePatternSurface() = 0;
	virtual void ReleasePatternSurface() = 0;

	virtual bool RenderScene() = 0;
	virtual bool RenderMipLevels() = 0;
};

class CMipGenEmuTexture: public CMipGenEmulation
{
protected:
	LPTEXTURES		m_pPatternTexture;
	LPSURFACES		m_pPatternSurface;

	bool CreateBaseTexture();
	bool CreatePatternSurface();
	void ReleasePatternSurface();

	bool RenderMipLevels();
	bool RenderScene();
	bool SetAutoGenFilter();

public:
	CMipGenEmuTexture();
	bool CapsCheck();

};

class CMipGenEmuCubeTexture: public CMipGenEmulation
{
protected:
	LPCUBETEXTURES		m_pPatternTexture;
	LPSURFACES			m_pPatternSurface[6];

	bool CreateBaseTexture();
	bool CreatePatternSurface();
	void ReleasePatternSurface();

	bool RenderMipLevels();
	bool RenderScene();
	bool SetAutoGenFilter();

public:
	CMipGenEmuCubeTexture();
	bool CapsCheck();

};

// Class for hardware validation
class CMipGenHardware : public CMipGenTest
{
protected:
	LPBASETEXTURE		m_pSrcTexture;
	LPBASETEXTURE		m_pRefTexture;

	LPTEXTURE		m_pColorFilledTexture;
	LPSURFACE		m_pColorFilledSurface;

	bool			m_bCreatePatternFailed;
	bool			m_bColorFillTopLevelFailed;
	bool			m_bSetAutoGenFailed;

	bool CopyFromSurfAToSurfB(LPSURFACE pSrcSurf, LPSURFACE pDestSurf, bool bUseSubRect = false);
	bool CreateColorFilledSurface();

	virtual bool CreatePatternSurface() = 0;
	virtual void ReleasePatternSurface() = 0;
	virtual bool WritePatternOnRef() = 0;
	virtual bool ColorFillTopLevel() = 0;
	virtual bool SetAutoGenFilter() = 0;

	virtual bool MethodLock() = 0;
	virtual bool MethodUpdateSurface() = 0;
	virtual bool MethodUpdateTexture() = 0;
	virtual bool MethodStretchRect() = 0;
	virtual bool MethodGetDC() = 0;
	virtual bool MethodColorFill() = 0;
	virtual bool MethodSetRenderTarget() = 0;
	virtual bool MethodEvictManagedResources() = 0;
	virtual bool MethodLostDevice() = 0;

	virtual bool CreateBaseTexture() {return true;}

	virtual void DumpTestDetails();

public:
	CMipGenHardware();
	~CMipGenHardware();

	// Framework functions
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
	virtual bool CapsCheck();
    virtual void UpdateStatus(void);
	virtual void CommandLineHelp();

	virtual bool RenderRefMipLevels() = 0;
	virtual bool RenderScene() = 0;

	void GenerateCaseList();
};

class CMipGenHWTexture: public CMipGenHardware
{
protected:
	LPTEXTURE		m_pPatternTexture;
	LPSURFACE		m_pPatternSurface;

	bool CreateBaseTexture();
	bool CreatePatternSurface();
	void ReleasePatternSurface();

	bool RenderRefMipLevels();
	bool RenderScene();
	bool WritePatternOnRef();
	bool ColorFillTopLevel();
	bool SetAutoGenFilter();

	bool MethodLock();
	bool MethodUpdateSurface();
	bool MethodUpdateTexture();
	bool MethodStretchRect();
	bool MethodGetDC();
	bool MethodColorFill();
	bool MethodSetRenderTarget();
	bool MethodEvictManagedResources();
	bool MethodLostDevice();

public:
	CMipGenHWTexture();
	bool CapsCheck();

};

class CMipGenHWCubeTexture: public CMipGenHardware
{
protected:
	LPCUBETEXTURE		m_pPatternTexture;
	LPSURFACE			m_pPatternSurface[6];

	bool CreateBaseTexture();
	bool CreatePatternSurface();
	void ReleasePatternSurface();

	bool RenderRefMipLevels();
	bool RenderScene();
	bool WritePatternOnRef();
	bool ColorFillTopLevel();
	bool SetAutoGenFilter();

	bool MethodLock();
	bool MethodUpdateSurface();
	bool MethodUpdateTexture();
	bool MethodStretchRect();
	bool MethodGetDC();
	bool MethodColorFill();
	bool MethodSetRenderTarget();
	bool MethodEvictManagedResources();
	bool MethodLostDevice();

public:
	CMipGenHWCubeTexture();
	bool CapsCheck();

};

#endif
