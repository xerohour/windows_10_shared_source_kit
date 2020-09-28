#include "BasicD3D10.h"

#include "windows.h"

const char * const CTestBasicD3D10::szD3D10Path = "D3D10.DLL";

HMODULE CTestBasicD3D10::hmoduleD3D10 = NULL;
PFND3D10CREATEDEVICE CTestBasicD3D10::pfnD3D10CreateDevice = NULL;
bool CTestBasicD3D10::bD3D10Loaded = false;

CTestBasicD3D10::CTestBasicD3D10()
{
}

CTestBasicD3D10::~CTestBasicD3D10()
{
}

bool CTestBasicD3D10::Initialize_D3D10()
{
	if (!bD3D10Loaded)
	{
		if (!hmoduleD3D10)
			if (!(hmoduleD3D10 = LoadLibraryEx(szD3D10Path, NULL, 0)))
			{
				GetFramework()->WriteToLog("Failed to load D3D10.DLL.\n");
				return false;
			}

		if
		(
			!
			(
				pfnD3D10CreateDevice =
				(PFND3D10CREATEDEVICE)(GetProcAddress(hmoduleD3D10, "D3D10CreateDevice"))
			)
		)
		{
			GetFramework()->WriteToLog("GetProcAddress failed for D3D10CreateDevice.\n");
			return false;
		}

		bD3D10Loaded = true;
	}

	return true;
}
