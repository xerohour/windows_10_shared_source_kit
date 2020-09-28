
#include "LostDev.h"

#include <tlhelp32.h>
#include <Shlobj.h>

// ----------------------------------------------------------------------------

BOOL GetProcessModule (DWORD dwPID, DWORD dwModuleID, 
     LPMODULEENTRY32 lpMe32, DWORD cbMe32) 
{ 
    BOOL          bRet        = FALSE; 
    BOOL          bFound      = FALSE; 
    HANDLE        hModuleSnap = NULL; 
    MODULEENTRY32 me32        = {0}; 
 
    // Take a snapshot of all modules in the specified process. 

    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
    if (hModuleSnap == INVALID_HANDLE_VALUE) 
        return (FALSE); 
 
    // Fill the size of the structure before using it. 

    me32.dwSize = sizeof(MODULEENTRY32); 
 
    // Walk the module list of the process, and find the module of 
    // interest. Then copy the information to the buffer pointed 
    // to by lpMe32 so that it can be returned to the caller. 

    if (Module32First(hModuleSnap, &me32)) 
    { 
        do 
        { 
            if (me32.th32ModuleID == dwModuleID) 
            { 
                CopyMemory (lpMe32, &me32, cbMe32); 
                bFound = TRUE; 
            } 
        } 
        while (!bFound && Module32Next(hModuleSnap, &me32)); 
 
        bRet = bFound;   // if this sets bRet to FALSE, dwModuleID 
                         // no longer exists in specified process 
    } 
    else 
        bRet = FALSE;           // could not walk module list 
 
    // Do not forget to clean up the snapshot object. 

    CloseHandle (hModuleSnap); 
 
    return (bRet); 
} 

bool IsIEProcessRunning () 
{ 
    HANDLE         hProcessSnap = NULL; 
    bool           bRet      = FALSE; 
    PROCESSENTRY32 pe32      = {0}; 
 
    //  Take a snapshot of all processes in the system. 

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

    if (hProcessSnap == INVALID_HANDLE_VALUE) 
        return (FALSE); 
 
    //  Fill in the size of the structure before using it. 

    pe32.dwSize = sizeof(PROCESSENTRY32); 
 
    //  Walk the snapshot of the processes, and for each process, 
    //  display information. 

    if (Process32First(hProcessSnap, &pe32)) 
    { 
        DWORD         dwPriorityClass; 
        BOOL          bGotModule = FALSE; 
        MODULEENTRY32 me32       = {0}; 
 
        do 
        { 
            bGotModule = GetProcessModule(pe32.th32ProcessID, 
                pe32.th32ModuleID, &me32, sizeof(MODULEENTRY32)); 

            if (bGotModule) 
            { 
                HANDLE hProcess; 
 
                // Get the actual priority class. 
                hProcess = OpenProcess (PROCESS_ALL_ACCESS, 
                    FALSE, pe32.th32ProcessID); 
                dwPriorityClass = GetPriorityClass (hProcess); 
                CloseHandle (hProcess); 

				//Check if this is IE
				if (!strcmp("IEXPLORE.EXE", me32.szModule) )
				{
					bRet = TRUE;
				}
            } 
        } 
        while (Process32Next(hProcessSnap, &pe32)); 
    } 
    else 
	{
        bRet = FALSE;    // could not walk the list of processes 
	}
 
    // Do not forget to clean up the snapshot object. 

    CloseHandle (hProcessSnap); 
    return (bRet); 
}

bool IsWindowsExplorerRunning()
{
	HWND hExplorer = FindWindow("CabinetWClass", NULL);

	if (hExplorer != NULL)
		return true;

	hExplorer = FindWindow("ExploreWClass", NULL);

	if (hExplorer != NULL)
		return true;

	return false;
}

bool IsInternetExplorerRunning()
{
	HWND hIE = FindWindow("IEFrame", NULL);

	if ( (hIE != NULL) || IsIEProcessRunning() )
		return true;

	return false;
}

bool IsActiveDesktopEnabled()
{
	// Is Active Desktop on or off?
	SHELLFLAGSTATE shfs;
	SHGetSettings(&shfs,SSF_DESKTOPHTML);

	return (shfs.fDesktopHTML != 0);
}

bool CLostDev::AreTherePossibleGDILeaks()
{
	OSVERSIONINFO VersionInfo;
	memset(&VersionInfo, 0, sizeof(OSVERSIONINFO));
	VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&VersionInfo);

	//
	// find out what platform we're on
	//

	if(VersionInfo.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS) // Windows 9x Variants
	{
		return false;
	}

	if (IsActiveDesktopEnabled() ||
		IsWindowsExplorerRunning() ||
		IsInternetExplorerRunning() )
	{
		return true;
	}

	return false;
}

bool CLostDev::CopyFromAToB(LOCKED_RECT *pLockRectA, LOCKED_RECT *pLockRectB, SURFACEDESC	*pDescA)
{
	//Copy A to B
	DWORD   dwY=0;
	DWORD   dwX;
	PBYTE   pSrc=(PBYTE)pLockRectA->pBits;
	PBYTE	pDest=(PBYTE)pLockRectB->pBits;
	DWORD	WidthSize = pDescA->dwWidth * pDescA->Format.ddpfPixelFormat.dwRGBBitCount / 8;

	do
	{
		if ( !memcpy(pDest, pSrc, WidthSize) )
		{
			return false;
		}

		// Increment to the next line.

		pSrc += pLockRectA->iPitch;
		pDest += pLockRectB->iPitch;

		dwY++;
	}
	while (dwY < pDescA->dwHeight);

	return true;
}

// ----------------------------------------------------------------------------

bool CLostDev::FillSurface(SURFACEDESC *pDesc, LOCKED_RECT *pLockedRect, bool bUseSubRectLock)
{
	PBYTE ptr;
	ptr = (PBYTE)pLockedRect->pBits;
	int PixelSize = pDesc->Format.ddpfPixelFormat.dwRGBBitCount / 8;
	UINT offset;

	//DPF(1, "P: %i, W: %i, Pitch: %i\n", PixelSize, Desc.dwWidth, m_LockedRect[iLevel].iPitch);
	
	DWORD dwHeight = pDesc->dwHeight;
	DWORD dwWidth = pDesc->dwWidth;
	if (bUseSubRectLock)
	{
		dwHeight -= pDesc->dwHeight - (pDesc->dwHeight / 4);
		dwWidth -= pDesc->dwWidth - (pDesc->dwWidth / 4);
	}

	int x, y;

	__try
	{
		for (y=0; y<dwHeight; y++)
		{
			for (x=0; x<dwWidth; x++)
			{
				for (int k=0; k<PixelSize; k++)
				{
					offset=pLockedRect->iPitch*y+x*PixelSize+k;
					*(ptr+offset)=(BYTE)((y*dwWidth + x*PixelSize+k)%255) + 1;
				}
			}
		}
	}
	__except
	(
		ExceptionFilter
		(
			GetExceptionCode(),
			GetExceptionInformation()
		)
	)
	{
		DPF(1, _T("EXCEPTION occured on fill surface on pixel %d,%d"), x, y);
		return false;
	}

	/*	UINT bpp = Desc.Format.ddpfPixelFormat.dwRGBBitCount;

		PBYTE   pBuf;
		PBYTE   pLine = (PBYTE)pLockedRect.pBits;
		BYTE	color;

		for (DWORD dwY = 0; dwY < Desc.dwHeight; dwY++)
		{
			pBuf = pLine;
			pLine += m_LockedRect[iLevel].iPitch;
			for (DWORD dwX = 0; dwX < Desc.dwWidth; dwX++)
			{
				color = (dwX + dwY) % 255;
				switch(bpp)
				{
					case 8 :
						break;

					case 16 :
						*(WORD*)pBuf = (WORD)((color << 8) + color);
						pBuf += 2;
						break;

					case 24 :
						break;

					case 32:
						*(DWORD*)pBuf = (DWORD)((color << 16) + (color << 8) + color);
						pBuf += 4;
						break;
				}
			}
		}
	*/
	/*    DWORD   dwY=0;
		DWORD   dwX;
		PBYTE   pSrc;
		PBYTE   pSrcMove;
		PDWORD 	pDWSrc;
		PWORD	pWSrc;

		bool b16bits = (16 == Desc.Format.ddpfPixelFormat.dwRGBBitCount);

		pSrc=(PBYTE)pLockedRect.pBits;

		do
		{
			dwX = 0;

			if (b16bits)
				pWSrc = (PWORD) pSrc;
			else
				pDWSrc = (PDWORD) pSrc;

			pSrcMove = pSrc;
			do
			{
				if (b16bits)
					pWSrc = (PWORD) pSrc;
				else
					pDWSrc = (PDWORD) pSrc;

				if (b16bits)
				{
					*pWSrc = (dwX + dwY) % 255;
				}
				else
				{
					*pDWSrc = (dwX + dwY) % 255;
				}

				pSrcMove ++;
				dwX++;
			}
			while (dwX < Desc.dwWidth);

			// Increment to the next line.

			pSrc += m_LockedRect[iLevel].iPitch;

			dwY++;
		}
		while (dwY < Desc.dwHeight);
	*/

	return true;
}
