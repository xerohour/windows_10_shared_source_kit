//
//  BltQueue.h
//

#ifndef __BltQueue_H__
#define __BltQueue_H__

#include "CD3DTest.h"

#define MAX_BLTS 20
#define BLT_WARN 3
#define BLT_MAX 7

typedef struct _BLTQUEUE_VERTEX
{
	float x,y,z,w;
	DWORD diffuse;
	float u,v;
} BLTQUEUE_VERTEX;
#define BLTQUEUE_FVF D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0)

// Known driver flags
#define KNOWN_LIGHTWEIGHT                   0x00000001      // Device can support lightweight surfaces
#define KNOWN_HWCURSOR                      0x00000002      // Device can support hardware cursors in Hi-Res
#define KNOWN_MIPPEDCUBEMAPS                0x00000004      // Device can support mipped cubemaps
#define KNOWN_ZSTENCILDEPTH                 0x00000010      // Device cannot support Z/Stencil depths different than the render target
#define KNOWN_HWCURSORLOWRES                0x00000020      // Device can support hardware cursors in LowRes
#define KNOWN_NOTAWINDOWEDBLTQUEUER         0x00000040      // Device has no drivers known to over-queue windowed presentation blts
#define KNOWN_D16_LOCKABLE                  0x00000080      // Device supports lockable D16 format correctly
#define KNOWN_RTTEXTURE_R5G6B5              0x00000100      // RT+Tex formats that are supported
#define KNOWN_RTTEXTURE_X8R8G8B8            0x00000200
#define KNOWN_RTTEXTURE_A8R8G8B8            0x00000400
#define KNOWN_RTTEXTURE_A1R5G5B5            0x00000800
#define KNOWN_RTTEXTURE_A4R4G4B4            0x00001000
#define KNOWN_RTTEXTURE_X1R5G5B5            0x00002000     

//
//  The BltQueue specification
//
class BltQueue : public CD3DTest
{
protected:
	bool m_bHadForceDriversOnKey;
	bool m_bHadForceDriversOffKey;
	DWORD m_dwOldForceDriversOnValue;
	DWORD m_dwOldForceDriversOffValue;
	LPSURFACE m_pBackbuffer;
	LPTEXTURE m_pTexture;
	LPVERTEXBUFFER m_pVB;
	bool SetDriverFlags(DWORD dwFlags, bool bValue);
	bool ResetDriverFlags( void );

public:
	BltQueue();
	bool ExecuteTest(UINT nTest);
	bool PreModeSetup(void);
	bool CapsCheck();
	UINT TestInitialize();
	bool TestTerminate();
	bool SetDefaultRenderStates();
	virtual LRESULT	OnSetCursor(WPARAM wParam, LPARAM lParam);
	UINT TestBltQueue(UINT nBlts);
	UINT RunBltQueueSequence(UINT nBlts);
	bool DrawSprite(float center_x, float center_y, float width, float height, float theta);
};

#endif

