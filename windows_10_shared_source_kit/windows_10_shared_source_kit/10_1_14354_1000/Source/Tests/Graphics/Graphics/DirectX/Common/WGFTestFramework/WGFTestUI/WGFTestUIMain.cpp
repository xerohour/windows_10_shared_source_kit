#include "pch.h"

#include "WGFTestUIShell.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	CWGFTestUIShell shell;
	return GetFramework()->Execute( &shell );
}
