//
//  LockDepth.h
//

#ifndef __LockDepth_H__
#define __LockDepth_H__

#define STRICT
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>
#include <windows.h>
#include "TestUtils.h"
#include "CD3DTest.h"

#include <vector>
using namespace std;

// Known driver flags
#define KNOWN_D16_LOCKABLE                  0x00000080      // Device supports lockable D16 format correctly

typedef struct
{
	FORMAT		fmtDepthStencil;
	LPSURFACES	pSurfaceDepthStencil;
} DEPTHSURFACE;

//
//  The LockDepth specification
//
class CLockDepth : public CD3DTest
{
public:
	CLockDepth();
	bool PreModeSetup(void);
	bool SetDefaultRenderStates();
	bool SetDefaultMatrices();
	UINT TestInitialize();
	bool ExecuteTest(UINT nTest);
	bool TestTerminate();
	bool ProcessFrame();
	bool CapsCheck();

private:
	LPVERTEXBUFFERS  m_pVB;
	WORD			m_wTolerance;
	UINT			m_uMaxFailsLog;
	LPTEXTURES		m_pTextures;
	vector<DEPTHSURFACE>	m_vecDepthStencil;		// all the depth stencil lockable formats and surfaces
	int				m_iLastDepthStencilSurface;		// remembers the last selected depth stencil surface
	
	bool IsDepthStencilFormatSupported( FORMAT fmtDepthStencil );	// verifies if the device can support this depth stencil format
	bool InitZBuffers( FORMAT fmtDepthStencil, LPSURFACES *ppDepthStencilSurface );
	bool PrepareZBuffers( UINT nTest );
	bool Render( UINT nTest );
	bool RenderClearFrame( UINT nTest );
	bool ValidateZBuffers( FORMAT fmtDepthStencil );
	bool CompareZBuffers();
	WORD GetZDepthWord(UINT nTest, float x, float y);
	FLOAT GetZDepthFloat(UINT nTest, float x, float y);
	
	bool			m_bHadForceDriversOnKey;
	bool			m_bHadForceDriversOffKey;
	DWORD			m_dwOldForceDriversOnValue;
	DWORD			m_dwOldForceDriversOffValue;
	bool SetDriverFlags(DWORD dwFlags, bool bValue);
	bool ResetDriverFlags();

	LPTEXTURES CreateNoisyTextures();
};


#endif

