#include "CCommandLine.h"
#include <stdlib.h>

CCommandLine::CCommandLine()
{
    szBuffer=szTempBuffer;
    dwBufferSize = sizeof(szTempBuffer);
    szParam = "";

    Refresh();
}

CCommandLine::~CCommandLine()
{
    if (szBuffer != szTempBuffer)
        free(szBuffer);
}

bool CCommandLine::ATOI(LPTSTR szBuffer, int * piValue)
{
    bool    bNeg=false;
    int     iValueTable[]={0,1,2,3,4,5,6,7,8,9};

    *piValue = 0;

    // Is this a digit?

    if ('-' == *szBuffer)
    {
        bNeg = true;
        szBuffer++;
    }

    if ('0' > *szBuffer || '9' < *szBuffer)
        return false;

    // Walk through as long as digits are available.

    while ('0' <= *szBuffer && '9' >= *szBuffer)
    {
        *piValue *= 10;
        *piValue += iValueTable[*szBuffer - '0'];
        szBuffer++;
    }

    // Make sure things like 100abc are not accepted.

    if (' ' != *szBuffer && 0 != *szBuffer)
        return false;

    // If negative, negate value

    if (bNeg)
        *piValue *= -1;

    // Successful conversion.

    return true;
}

LPTSTR CCommandLine::FindKey(LPCTSTR pKey, LPTSTR * pszStart)
{
    int     iKeyLength=lstrlen(pKey);
    TCHAR * szWalk = szBuffer;

    while (0 != *szWalk)
    {
        if ('-' == *szWalk || '/' == *szWalk)
        {
            TCHAR   szTemp[MAX_PATH+1];
            TCHAR * szTerminate;

            szWalk ++;
            
            szTerminate = szWalk;

            while (0 != *szTerminate)
            {
                // Accept format: -p 50, -p=50, -p:50

                if (' ' == *szTerminate || '=' == *szTerminate || ':' == *szTerminate)
                    break;

                szTerminate++;
            }

            strncpy(szTemp,szWalk,(szTerminate - szWalk));
			
			// Append a NULL char
			if((szTerminate - szWalk) < MAX_PATH)
				szTemp[szTerminate - szWalk] = _T('\0');

			// PreFast is complaining that szTemp may still not be terminated
			szTemp[MAX_PATH] = _T('\0');

            // Determine if this is the key.

            if (lstrcmpi(szTemp,pKey) == 0)
            {
                *pszStart = szWalk-1;

                szWalk += iKeyLength+1; // +1 to move past terminator

                // Find the end of the key.

                while (' ' == *szWalk)
                    szWalk++;

                return szWalk;
            }

            szWalk=szTerminate;
        }

        szWalk++;
    }

    return NULL;
}

bool CCommandLine::KeySet(LPCTSTR pKey, bool bRemove)
{
    LPTSTR  szStop;
    LPTSTR  szStart;

    szStop = FindKey(pKey,&szStart);

    if (NULL == szStop)
        return false;

    // The key must be a full key. /D:50 doesn't qualify for /D

    if (':' == * (szStop-1))
        return false;

    if (bRemove)
        Remove(szStart,szStop-1);

    return true;
}

bool CCommandLine::ReadInteger(LPCTSTR pKey, __out int * pInt,bool bRemove)
{
    LPTSTR  szValue;
    LPTSTR  szStart;
    bool    bResult;

    szValue = FindKey(pKey,&szStart);

    if (NULL == szValue)
        return false;

    bResult = ATOI(szValue,pInt);

    while (0 != *szValue)
    {
        if (' ' == *szValue)
            break;

        szValue++;
    }

    if (bRemove)
        Remove(szStart,szValue);

    return true;
}

void CCommandLine::ReadInteger(LPCTSTR pKey, int iDefault, __out int * szValue, bool bRemove)
{
    if (ReadInteger(pKey,szValue,bRemove))
        return;

    *szValue = iDefault;
}

bool CCommandLine::ReadFloat(LPCTSTR pKey, __out float * pFloat,bool bRemove)
{
    LPTSTR  szValue;
    LPTSTR  szStart;

    szValue = FindKey(pKey,&szStart);

    if (NULL == szValue)
        return false;

    *pFloat = (float)atof(szValue);

    while (0 != *szValue)
    {
        if (' ' == *szValue)
            break;

        szValue++;
    }

    if (bRemove)
        Remove(szStart,szValue);

    return true;
}

void CCommandLine::ReadFloat(LPCTSTR pKey, float fDefault, __out float * pFloat, bool bRemove)
{
    if (ReadFloat(pKey,pFloat,bRemove))
        return;

    *pFloat = fDefault;
}

bool CCommandLine::ReadString(LPCTSTR pKey, __out_ecount(nBuffer) LPTSTR szBuffer, int nBuffer, bool bRemove)
{
    LPTSTR  szValue;
    LPTSTR  szTerminate;
    LPTSTR  szStart;
    bool    bQuote=false;

    szValue = FindKey(pKey,&szStart);

    if (NULL == szValue)
        return false;

    if (('-' == *szValue || '/' == *szValue || 0 == *szValue) && ':' != *(szValue-1))
        return false;

    if ('"' == *szValue)
    {
        bQuote=true;
        szValue++;
    }

    szTerminate = szValue;

    while (0 != *szTerminate)
    {
        if (' ' == *szTerminate && !bQuote)
            break;

        if ('"' == *szTerminate && bQuote)
            break;

        szTerminate++;
    }

    strncpy(szBuffer,szValue,(szTerminate - szValue));
	
	// Append a NULL char
	szBuffer[szTerminate - szValue] = _T('\0');

    if (bRemove)
        Remove(szStart,szTerminate);

    return true;
}

void CCommandLine::ReadString(LPCTSTR szKey, LPCTSTR szDefault, __out_ecount(nBuffer) LPTSTR szBuffer, int nBuffer, bool bRemove)
{
    if (!ReadString(szKey,szBuffer,nBuffer - 1,bRemove))
	    _tcsncpy(szBuffer,szDefault,nBuffer - 1);

	// Zero terminiate szBuffer to make PreFast happy
	szBuffer[nBuffer - 1] = _T('\0');
}

void CCommandLine::Refresh(void)
{
    if (NULL == szBuffer)
    {
        szBuffer = szTempBuffer;
        dwBufferSize = sizeof(szTempBuffer);
    }

    ZeroMemory(szBuffer,dwBufferSize);

    lstrcpy(szBuffer,GetCommandLine());

	// Get the params
	szParam = _tcsrchr(szBuffer, _T('"'));
	if(szParam)
	{
		while(*szParam == _T(' ') || *szParam == _T('"'))
			szParam++;
	}
	else
	{
		// No quotes, look for last "\"
		szParam = _tcsrchr(szBuffer, '\\');
		if (0L == szParam) {
			// Found no quotes not a path separator char, so start
			// from beginning of the buffer
			szParam = szBuffer;
		}
		while(*szParam != _T(' ') && *szParam != _T('\0'))
			szParam++;
		while(*szParam == _T(' '))
			szParam++;
	}
}

void CCommandLine::Remove(LPTSTR szStart, LPTSTR szStop)
{
    // Remove the key from the string
	_tcscpy(szStart, szStop);
}

/*
bool CCommandLine::CreateCommandWindow(HWND hWnd, LPTSTR pTitle, bool bAttached)
{
    bool    bResult;

    if (NULL != hWnd && bAttached)
    {
        bResult = cLogWin.Create(hWnd);

        if (bResult)
        {
            RECT    rRect;

            // By default, expand to fill parent.

            GetClientRect(hWnd,&rRect);
            MoveWindow(cLogWin.hLogWnd,0,0,rRect.right,rRect.bottom,false);
        }
    }
    else
        bResult = cLogWin.Create(hWnd,WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,640,480,false);

    cLogWin.SetAutoClose(bAttached);

    if (NULL != pTitle)
        SetWindowText(cLogWin.hLogWnd,pTitle);

    return bResult;
}

void CCommandLine::WriteCommandOption(LPTSTR pFormat, ...)
{
    va_list list;
    TCHAR   Buffer[MAX_PATH];
    int     TabStops[]={50,250,300,600};

    cLogWin.SetTabStops(TabStops,sizeof(TabStops)/sizeof(int));

    va_start(list,pFormat);

    wvsprintf(Buffer,pFormat,list);

    cLogWin.LogWin(Buffer);
}

void CCommandLine::ClearCommandWindow(void)
{
    cLogWin.ClearAllEntries();
}

*/

void CCommandLine::AddKeys(LPCTSTR pKey)
{
    if (NULL == szBuffer)
        szBuffer = szTempBuffer;

    lstrcat(szBuffer," ");
    lstrcat(szBuffer,pKey);
}

