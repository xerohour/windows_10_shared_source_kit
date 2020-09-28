
// Auto-Generated pch file from the following files:
// winmain.cpp, cd3dwindowframework.cpp

#define _WIN32_WINNT_BAK _WIN32_WINNT
#define NTDDI_VERSION_BAK NTDDI_VERSION

#if (_WIN32_WINNT < _WIN32_WINNT_LONGHORN)
#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_LONGHORN
#endif

#if (_NTDDI_VERSION < _NTDDI_LONGHORN)
#undef NTDDI_VERSION
#define NTDDI_VERSION _NTDDI_LONGHORN
#endif

#if !defined(_NTSTATUS_)
typedef long NTSTATUS;
#endif

#include <windows.h>

#define D3DKMDT_SPECIAL_MULTIPLATFORM_TOOL
#include <d3dkmthk.h>

#include "CD3DWindowFramework.h"		// referenced 1 times
#include "Framework.h"		// referenced 1 times
#include <RichEdit.h>		// referenced 1 times
#include <math.h>		// referenced 1 times
#include <crtdbg.h>		// referenced 1 times
#include "FrameworkResource.h"		// referenced 1 times

#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT _WIN32_WINNT_BAK

#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_VERSION_BAK
