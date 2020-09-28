//
// CResFile.cpp
//

#include <windows.h>
#include "CResFile.h"

#ifdef _DEBUG
	#include <stdio.h>
	#define DEBUGFUNC(f, s, c) { CHAR __macTempStr[256]; sprintf(__macTempStr, "%s:%d[%s] - %s (%08X)\n",__FILE__,__LINE__, f, s, c); OutputDebugString(__macTempStr); }
#else
	#define DEBUGFUNC(f, s, c)
#endif

CResFile::CResFile()
{
	m_eType = CRCLOSED;
	m_dwFilePtr = 0;
	m_dwFileSize = 0;
}

CResFile::~CResFile()
{
	if(m_eType != CRCLOSED)
		Close();
}

BOOL
CResFile::Open(LPCSTR		lpFile,
			   enum CRESFILETYPE eFileType,
			   BOOL			bWrite,
			   HMODULE		hModule,
			   LPCSTR		lpType)
{
	if(m_eType != CRCLOSED)
	{
		DEBUGFUNC("CResFile::Open", "File already open.", 0);
		return FALSE;
	}

	if(NULL == lpFile)
	{
		DEBUGFUNC("CResFile::Open", "No file specified.", 0);
		return FALSE;
	}

	if((TRUE == bWrite) && (eFileType != CRFILE))
	{
		DEBUGFUNC("CResFile::Open", "Must specify type CRFILE for write access.", 0);
		return FALSE;
	}

	switch(eFileType)
	{
	case CREITHER:
		// First try to open as a resource, then try as a file.
		if(FALSE == Open(lpFile, CRRESOURCE, bWrite, hModule, lpType))
			return Open(lpFile, CRFILE, bWrite, hModule, lpType);
		return TRUE;
	case CRFILE:
		// Open the named item as a file
		m_hFile = CreateFile(lpFile,
						   GENERIC_READ | (bWrite ? GENERIC_WRITE : 0),
						   FILE_SHARE_READ,
						   NULL,
						   OPEN_EXISTING,
						   FILE_ATTRIBUTE_NORMAL,
						   NULL);
		if(INVALID_HANDLE_VALUE == (LPVOID)m_hFile)
		{
			DEBUGFUNC("CResFile::Open", "Unable to open file.", 0);
			return FALSE;
		}
		m_eType = CRFILE;
		m_dwFileSize = GetFileSize((HANDLE)m_hFile, NULL) - 1;
		break;
	case CRRESOURCE:
		// Open the named item as a resource
		m_hResource = FindResource(hModule, lpFile, (NULL == lpType ? "BINARY" : lpType));
		if(NULL == m_hResource)
		{
			DEBUGFUNC("CResFile::Open", "Unable to find resource.", 0);
			return FALSE;
		}
		m_dwFileSize = SizeofResource(hModule, m_hResource) - 1;
		// Retrieve the resource handle
		m_hResource = (HRSRC)LoadResource(hModule, m_hResource);
		if(NULL == m_hResource)
		{
			DEBUGFUNC("CResFile::Open", "Unable to load resource.", 0);
			return FALSE;
		}
		// Lock the handle to obtain a pointer to the resource data
		m_lpResData = (LPBYTE)LockResource(m_hResource);
		if(NULL == m_lpResData)
		{
			DEBUGFUNC("CResFile::Open", "Unable to lock resource.", 0);
			return FALSE;
		}
		m_eType = CRRESOURCE;
		
		break;
	default:
		DEBUGFUNC("CResFile::Open", "Invalid file type specified.", 0);
		return FALSE;
	}

	m_dwFilePtr = 0;
	return TRUE;
}

BOOL
CResFile::Close()
{
	switch(m_eType)
	{
	case CRCLOSED:
		return TRUE;
	case CRFILE:
		CloseHandle((HANDLE)m_hFile);
		m_eType = CRCLOSED;
		return TRUE;
	case CRRESOURCE:
		m_eType = CRCLOSED;
		return TRUE;
	}

	// If we get here, something got messed up internally

	DEBUGFUNC("CResFile::Close", "Invalid type in eType.", 0);
	return FALSE;
}

BOOL
CResFile::Read(LPVOID lpBuffer, DWORD dwBytesToRead, DWORD *lpdwBytesRead)
{
	BOOL bSucceeded;
	DWORD dwBytesRead;

	if(0 == dwBytesToRead)
	{
		if(lpdwBytesRead)
			*lpdwBytesRead = 0;
		return TRUE;
	}

	if(NULL == lpBuffer)
	{
		DEBUGFUNC("CResFile::Read", "Read buffer is NULL", 0);
		return FALSE;
	}


	switch(m_eType)
	{
	case CRCLOSED:
		DEBUGFUNC("CResFile::Read", "File is closed.", 0);
		return FALSE;
	case CRFILE:
		bSucceeded = ReadFile((HANDLE)m_hFile, lpBuffer, dwBytesToRead, &dwBytesRead, NULL);
		if(bSucceeded)
		{
			m_dwFilePtr += dwBytesRead;
			if(lpdwBytesRead)
				*lpdwBytesRead = dwBytesRead;
			return TRUE;
		}
	
		DEBUGFUNC("CResFile::Read", "ReadFile() failed.", 0);
		return FALSE;
	case CRRESOURCE:
		dwBytesToRead = ( ((m_dwFileSize - m_dwFilePtr) > dwBytesToRead) ? dwBytesToRead : (m_dwFileSize - m_dwFilePtr) );
		if(0 == dwBytesToRead)
		{
			DEBUGFUNC("CResFile::Read", "No more resource left to read.", 0);
			return FALSE;
		}

		CopyMemory(lpBuffer, m_lpResData + m_dwFilePtr, dwBytesToRead);
		m_dwFilePtr += dwBytesToRead;
		if(lpdwBytesRead)
			*lpdwBytesRead = dwBytesToRead;
		return TRUE;
	}

	// If we get here, something got messed up internally

	DEBUGFUNC("CResFile::Read", "Invalid type in m_eType.", m_eType);
	return FALSE;
}

BOOL
CResFile::Write(LPVOID lpBuffer, DWORD dwBytesToWrite, DWORD *lpdwBytesWritten)
{
	BOOL bSucceeded;
	DWORD dwBytesWritten;

	if(0 == dwBytesToWrite)
	{
		if(lpdwBytesWritten)
			lpdwBytesWritten = 0;
		return TRUE;
	}

	if(NULL == lpBuffer)
	{
		DEBUGFUNC("CResFile::Write", "Write buffer is NULL", 0);
		return FALSE;
	}

	switch(m_eType)
	{
	case CRCLOSED:
		DEBUGFUNC("CResFile::Write", "File is closed.", 0);
		return FALSE;
	case CRRESOURCE:
		DEBUGFUNC("CResFile::Write", "Can't write to a resource.", 0);
		return FALSE;
	case CRFILE:
		bSucceeded = WriteFile((HANDLE)m_hFile, lpBuffer, dwBytesToWrite, &dwBytesWritten, NULL);
		if(bSucceeded)
		{
			m_dwFilePtr += dwBytesWritten;
			if(lpdwBytesWritten)
				*lpdwBytesWritten = dwBytesWritten;
			m_dwFileSize = GetFileSize((HANDLE)m_hFile, NULL) - 1;
			return TRUE;
		}

		DEBUGFUNC("CResFile::Write", "WriteFile() failed.", 0);
		return FALSE;
	}

	DEBUGFUNC("CResFile::Write", "Invalid type in m_eType.", m_eType);
	return FALSE;
}

BOOL
CResFile::Seek(DWORD dwPos)
{

	if(CRCLOSED == m_eType)
	{
		DEBUGFUNC("CResFile::Seek", "File is closed.", 0);
		return FALSE;
	}

	if(dwPos > m_dwFileSize)
	{
		DEBUGFUNC("CResFile::Seek", "Specified position is beyond the end of the file.", dwPos);
		return FALSE;
	}

	switch(m_eType)
	{
	case CRRESOURCE:
		m_dwFilePtr = dwPos;
		return TRUE;
	case CRFILE:
		dwPos = SetFilePointer((HANDLE)m_hFile, dwPos, NULL, FILE_BEGIN);
		if(0xFFFFFFFF == dwPos)
		{
			DEBUGFUNC("CResFile::Seek", "SetFilePointer() failed.", 0);
			return FALSE;
		}
		m_dwFilePtr = dwPos;
		return TRUE;
	}

	// If we get here, something got messed up internally

	DEBUGFUNC("CResFile::Seek", "Invalid type in eType.", 0);
	return FALSE;
}

BOOL
CResFile::Eof()
{
	if(m_eType == CRCLOSED)
	{
		DEBUGFUNC("CResFile::Eof", "File is closed.", 0);
		return FALSE;
	}

	if(m_dwFilePtr == m_dwFileSize)
		return TRUE;

	return FALSE;
}
