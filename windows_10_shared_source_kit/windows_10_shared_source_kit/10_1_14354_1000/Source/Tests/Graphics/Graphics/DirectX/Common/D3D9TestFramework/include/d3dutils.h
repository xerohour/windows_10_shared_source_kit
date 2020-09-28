
#ifndef _D3DUTILS_H_
#define _D3DUTILS_H_

#include "windows.h"
#include "CD3DDef.h"
#include "testutils.h"

    // Code from tstutils.h in legacy

// Teststates for the conformance tests.
#define TSTSTATE_HARDWARE 0
#define TSTSTATE_SOFTWARE 1
#define TSTSTATE_COMPARE  2

// ============================================================================
//
// FUNCTIONS
//
// ============================================================================

void SetVertex(D3DVERTEX *lpVrt,
              float x, float y, float z,
              float nx, float ny, float nz);

void SetVertex(D3DTLVERTEX *lpVrt, float x, float y, float z);

// This function (overloaded for D3DVERTEX and D3DTLVERTEX types) assigns
// values to a vertex structure.

// ----------------------------------------------------------------------------

void SetTexture(D3DTLVERTEX *lpVrt, float u, float v, int nColor);

// Sets Texture colors for a D3DTLVERTEX

// ----------------------------------------------------------------------------

long UseConformanceRasterizer(int bUseCRaster);

// Accepts a BOOL value.  A value of TRUE sets up the conformance
// rasterizer for use by the tests.  A value of FALSE removes all registry
// keys set by the setup.  The return value will 

// ----------------------------------------------------------------------------

// GetFogModeString
//

char * GetFogModeString(D3DFOGMODE fogMode);

// Returns a character string that describes the fog mode specified
// by the input.

// ----------------------------------------------------------------------------

HRESULT BltSurface(HWND hWnd, LPDIRECTDRAWSURFACE pSource,
                   LPDIRECTDRAWSURFACE pDest, BOOL bFullscreen);

HRESULT BltSurface(HWND hWnd, LPDIRECTDRAWSURFACE pSource,
                   LPDIRECTDRAWSURFACE pDest, BOOL bFullscreen, DWORD dwROP);

// BltSurface blits the source surface onto the destination surface.  It's
// overloaded to accept a Windows GDI ROP code.

// ----------------------------------------------------------------------------

int GetStrictLevel(void);

// Parses the command line and returns the strict level.
// Returns -1 if level was not found.

// ----------------------------------------------------------------------------

long DrawCircle(LPDIRECT3DDEVICE2 pDevice, LPD3DVECTOR pCenter, float Radius, 
                int nDivisions, D3DCOLOR Color, D3DCOLOR Specular);

// DrawCircle draws the requested circle using TL vertices and DrawPrimitive().
// This function is ONLY available on DX5 or later releases.

// ----------------------------------------------------------------------------

long MakeCircle(D3DHVERTEX * pVertices, D3DVECTOR *dvecCenter, float Radius,
                int nDivisions);

// Takes a center, radius, and number of divisions, then plots a circle and
// returns the points in an array of D3DHVERTEX structures..

// ----------------------------------------------------------------------------

unsigned long GetRefCount(LPVOID pInterface);

// Returns the reference count of a COM interface by calling AddRef and Release
// on the interface, and returning the refcount returned by the Release call.

// ----------------------------------------------------------------------------

int GetTexFmtDesc( const PIXELFORMAT * pDDPF, PTSTR ptcsDesc, int nChars );


#endif // #ifndef _D3DUTILS_H_
