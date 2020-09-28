
// Auto-Generated pch file from the following files:
// cd3d.cpp, cd3d6.cpp, cd3d7.cpp, cd3d8.cpp, cd3d9.cpp, cd3dx.cpp

#define _WIN32_WINNT_BAK _WIN32_WINNT
#define NTDDI_VERSION_BAK NTDDI_VERSION

#if (_WIN32_WINNT < _WIN32_WINNT_LONGHORN)
#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_LONGHORN
#endif

#if (NTDDI_VERSION < NTDDI_LONGHORN)
#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_LONGHORN
#endif

#if !defined(_NTSTATUS_)
typedef long NTSTATUS;
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

#include <windows.h>

#define D3DKMDT_SPECIAL_MULTIPLATFORM_TOOL
#include <d3dkmthk.h>

#include "Framework.h"		// referenced 3 times
#include "CSurface.h"		// referenced 2 times
#include "CD3DX.h"		// referenced 2 times
//#include <d3d8rgbrast.h>		// referenced 2 times
//#include "D3D.h"		// referenced 2 times
#include "CD3D.h"		// referenced 2 times
#include <math.h>		// referenced 1 times
//#include <d3dx8.h>		// referenced 1 times
//#include "D3DX9.h"		// referenced 1 times
//#include "D3DX.h"		// referenced 1 times

#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT _WIN32_WINNT_BAK

#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_VERSION_BAK
