#include "WindowPCH.h"

#include "Framework.h"
#include "RichEdit.h"

// --------------------------------------------------------------------------------------

CLogWindow::CLogWindow()
{
    m_nLogLevel 	= 1;
	m_nLogAlloc		= 0;
	m_nLogLength	= 0;
	m_sLog			= NULL;
	m_nGrowSize		= 512;
    m_nLineAlloc 	= 0;
	m_sLineBuffer 	= NULL;
	m_sRTFHeader	= _T("{\\rtf\\ansi\\deff0"
		"{\\fonttbl{\\f0\\froman\\fprq2\\fcharset0 Times New Roman;}}"
		"{\\colortbl;"
		"\\red255\\green0\\blue0;"			// $r
		"\\red0\\green255\\blue0;"			// $g
		"\\red0\\green0\\blue255;"			// $b
		"\\red0\\green255\\blue255;"		// $c
		"\\red255\\green255\\blue0;"		// $y
		"\\red255\\green255\\blue255;"		// $w
        "\\red0\\green255\\blue255;"		// $a
		"\\red153\\green102\\blue204;"		// $p
		"\\red255\\green153\\blue0;}"		// $o
		"\\pard\\f0\\fs20\\cf6 ");
}

// --------------------------------------------------------------------------------------

CLogWindow::~CLogWindow()
{
    ClearEntries();

    m_nLineAlloc = 0;
	m_nLogAlloc  = 0;
	m_nLogLength = 0;

    SAFEDELETEA(m_sLog);
	SAFEDELETEA(m_sLineBuffer);
}

// --------------------------------------------------------------------------------------

void CLogWindow::ClearEntries(void)
{
    if(NULL != m_sLog)
	{
        UINT uLen = _tcslen(m_sRTFHeader);
		m_sLog[uLen] = _T('\0');
        m_nLogLength = uLen;
	}
	else	
		m_nLogLength = 0;
}

// --------------------------------------------------------------------------------------

bool CLogWindow::InitializeWindow(DWORD dwStyleEx, LPTSTR pClassName, LPTSTR pName, HMENU hMenu, HICON hIcon, DWORD dwStyle, int nPosX, int nPosY, int nWidth, int nHeight, HWND hParent)
{
	if( CWindow::InitializeWindow( dwStyleEx, pClassName, pName, hMenu, hIcon, dwStyle, nPosX, nPosY, nWidth, nHeight, hParent ) )
	{
		m_nLineAlloc  = 256;
		m_sLineBuffer = new TCHAR[m_nLineAlloc];

		if(NULL == m_sLineBuffer)
		{
			DPF(1, _T("CLogWindow::Init() - Out of memory.\n"));
			return false;
		}

		memset(m_sLineBuffer, 0, sizeof(TCHAR) * m_nLineAlloc);

		AppendToLog(m_sRTFHeader);

		SendMessage(m_hWindow, EM_SETBKGNDCOLOR, 0, RGB(0,0,0));
		SendMessage(m_hWindow, EM_SETLIMITTEXT, 0x7FFFFFFE, 0);
		
		return true;
	}
	
	return false;
}

// --------------------------------------------------------------------------------------

int CLogWindow::GetLogLevel(void)
{
    return m_nLogLevel;
}

// --------------------------------------------------------------------------------------

void CLogWindow::SetLogLevel(int nNewLevel)
{
    m_nLogLevel = nNewLevel;
}

// --------------------------------------------------------------------------------------

bool CLogWindow::WriteToLog(const TCHAR *ptcsFmt, ...)
{
	bool bRes = false;
	TCHAR *sOutput = NULL;
	va_list	argList;

	va_start(argList, ptcsFmt);
	sOutput = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

	bRes = WriteToLogWhole(1, sOutput);

	SAFEDELETEA(sOutput);

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CLogWindow::WriteToLog(int nLevel, const TCHAR *ptcsFmt, ...)
{
    bool bRes = false;
	TCHAR *sOutput = NULL;      
	va_list	argList;

	va_start(argList, ptcsFmt);
	sOutput = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

    bRes = WriteToLogWhole(nLevel, sOutput);

	SAFEDELETEA(sOutput);

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CLogWindow::AppendToLog(TCHAR *sEntry)
{
    UINT nEntryLen = 0;

    nEntryLen = _tcslen(sEntry);

	// Grow the log as needed
	if(nEntryLen + m_nLogLength + 1 >= m_nLogAlloc)
	{
		// Calculate the new log allocation
		m_nLogAlloc += m_nGrowSize + nEntryLen;

		TCHAR *pTemp = new TCHAR[m_nLogAlloc];
		if(NULL == pTemp)
		{
			DPF(1, _T("CLogWindow::WriteToLogWhole() - Out of memory.\n"));
			return false;
		}
		memset(pTemp, 0, sizeof(TCHAR) * (m_nLogAlloc));

		if(m_sLog)
		{
			_tcscpy(pTemp, m_sLog);
			SAFEDELETEA(m_sLog);
		}

		m_sLog = pTemp;
        m_nGrowSize *= 2;
	}

	m_nLogLength += nEntryLen;
    _tcscat(m_sLog, sEntry);

	return true;
}

// --------------------------------------------------------------------------------------

bool CLogWindow::WriteToLogWhole(int nLevel, const TCHAR *sEntry)
{
	if(sEntry == NULL)
		return false;
    if(nLevel > m_nLogLevel || NULL == *sEntry) 
		return true;

	// Convert the string to RTF format
    m_sLineBuffer[0] = _T('\0');
	ConvertRTF((TCHAR*)sEntry, &m_sLineBuffer, m_nLineAlloc);
	
	return AppendToLog(m_sLineBuffer);
}

// --------------------------------------------------------------------------------------

bool CLogWindow::WriteStatus(const TCHAR *ptcsKey, const TCHAR *ptcsFmt, ...)
{
	bool bRes = false;
	TCHAR *sOutput = NULL;
	va_list	argList;

	va_start(argList, ptcsFmt);
	sOutput = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

	bRes = WriteStatusWhole(1, ptcsKey, sOutput);

	SAFEDELETEA(sOutput);

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CLogWindow::WriteStatus(int nLevel, const TCHAR *ptcsKey, const TCHAR *ptcsFmt,...)
{
    bool bRes = false;
	TCHAR *sOutput = NULL;      
	va_list	argList;

	va_start(argList, ptcsFmt);
	sOutput = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

	bRes = WriteStatusWhole(nLevel, ptcsKey, sOutput);

	SAFEDELETEA(sOutput);

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CLogWindow::WriteStatusWhole(int nLevel, const TCHAR *ptcsKey, const TCHAR *ptcsBuffer)
{
	if(!ptcsKey || !ptcsBuffer)	return false;
	if(nLevel > m_nLogLevel) return true;

    TCHAR sWhole[128] = {0};
	memset(sWhole, 0, sizeof(sWhole));
	_sntprintf_s(sWhole, (sizeof sWhole / sizeof sWhole[0]) - 2, _T("@b%s@b0\t%s"), ptcsKey, ptcsBuffer);
	_tcscat(sWhole, _T("\n"));
	
	return WriteToLogWhole(nLevel, sWhole);
}

// --------------------------------------------------------------------------------------

void CLogWindow::Append(TCHAR *sAppend, TCHAR **sOut, UINT &nLen, UINT &nAlloc)
{
	UINT nAppendLen = _tcslen(sAppend);
	if(nLen+nAppendLen >= nAlloc)
	{
		nAlloc += _tcslen(sAppend) * 2;
		TCHAR *sTemp = new TCHAR[nAlloc];
		if(NULL == sTemp)
		{
			DPF(1, _T("CLogWindow::Append() - Out of memory.\n"));
			return;
		}
		_tcscpy(sTemp, *sOut);
		SAFEDELETEA(*sOut);
		*sOut = sTemp;
	}
	//_tcscat(*sOut, sAppend);
	// This is better than _tcscat since we know the length and do not need to call _tcslen again:
	_tcscpy((*sOut)+nLen, sAppend);
	nLen += nAppendLen;
}

// --------------------------------------------------------------------------------------

void CLogWindow::Append(TCHAR cAppend, TCHAR **sOut, UINT &nLen, UINT &nAlloc)
{
	if(nLen+1 >= nAlloc)
	{
		nAlloc *= 2;
		TCHAR *sTemp = new TCHAR[nAlloc];
		if(NULL == sTemp)
		{
			DPF(1, _T("CLogWindow::Append() - Out of memory.\n"));
			return;
		}
		_tcscpy(sTemp, *sOut);
		SAFEDELETEA(*sOut);
		*sOut = sTemp;
	}
	(*sOut)[nLen] = cAppend;
	(*sOut)[nLen+1] = _T('\0');
	nLen += 1;
}

// --------------------------------------------------------------------------------------

void CLogWindow::ConvertRTF(TCHAR *sIn, TCHAR **sOut, UINT &nAlloc)
{
	TCHAR *sIndex = sIn;
	UINT nLen = _tcslen(*sOut);

	if(NULL == sIn || NULL == *sIn)
		return;

	while(*sIndex != _T('\0'))
	{
		switch(*sIndex)
		{
		case _T('\\'):
			Append(_T("\\\\"), sOut, nLen, nAlloc);
			sIndex++;
			break;
		case _T('\n'):
			Append(_T("\\line "), sOut, nLen, nAlloc);
			sIndex++;
			break;
		case _T('\t'):
			Append(_T("\\tab "), sOut, nLen, nAlloc);
			sIndex++;
			break;
		case _T('{'):
			Append(_T("\\{ "), sOut, nLen, nAlloc);
			sIndex++;
			break;
		case _T('}'):
			Append(_T("\\} "), sOut, nLen, nAlloc);
			sIndex++;
			break;
		case _T('$'):
			switch(sIndex[1])
			{
				case _T('$'):
					Append(_T('$'), sOut, nLen, nAlloc);
					sIndex++;
					break;
				case _T('r'):
				case _T('R'):
					Append(_T("\\cf1 "), sOut, nLen, nAlloc);
					sIndex+=2;
					break;
				case _T('g'):
				case _T('G'):
					Append(_T("\\cf2 "), sOut, nLen, nAlloc);
					sIndex+=2;
					break;
				case _T('b'):
				case _T('B'):
					Append(_T("\\cf3 "), sOut, nLen, nAlloc);
					sIndex+=2;
					break;
				case _T('c'):
				case _T('C'):
					Append(_T("\\cf4 "), sOut, nLen, nAlloc);
					sIndex+=2;
					break;
				case _T('y'):
				case _T('Y'):
					Append(_T("\\cf5 "), sOut, nLen, nAlloc);
					sIndex+=2;
					break;
				case _T('w'):
				case _T('W'):
					Append(_T("\\cf6 "), sOut, nLen, nAlloc);
					sIndex+=2;
					break;
           		case _T('a'):
				case _T('A'):
					Append(_T("\\cf7 "), sOut, nLen, nAlloc);
					sIndex+=2;
					break;
           		case _T('p'):
				case _T('P'):
					Append(_T("\\cf8 "), sOut, nLen, nAlloc);
					sIndex+=2;
					break;
				case _T('o'):
				case _T('O'):
					Append(_T("\\cf9 "), sOut, nLen, nAlloc);
					sIndex+=2;
					break;
				default:
					Append(*sIndex, sOut, nLen, nAlloc);
					sIndex++;
					break;
			}
			break;
		case _T('@'):
            {
				if(0 == _tcsnicmp(sIndex+1, _T("b0"), 2))
				{
					Append(_T("\\b0 "), sOut, nLen, nAlloc);
					sIndex+=3;
				}
				else if(0 == _tcsnicmp(sIndex+1, _T("i0"), 2))
				{
					Append(_T("\\i0 "), sOut, nLen, nAlloc);
					sIndex+=3;
				}
                else if(0 == _tcsnicmp(sIndex+1, _T("u0"), 2))
				{
					Append(_T("\\ul0 "), sOut, nLen, nAlloc);
					sIndex+=3;
				}
				else if(0 == _tcsnicmp(sIndex+1, _T("b"), 1))
				{
					Append(_T("\\b "), sOut, nLen, nAlloc);
					sIndex+=2;
				}
				else if(0 == _tcsnicmp(sIndex+1, _T("i"), 1))
				{
					Append(_T("\\i "), sOut, nLen, nAlloc);
					sIndex+=2;
				}
                else if(0 == _tcsnicmp(sIndex+1, _T("u"), 1))
				{
					Append(_T("\\ul "), sOut, nLen, nAlloc);
					sIndex+=2;
				}
				else
				{
					Append(*sIndex, sOut, nLen, nAlloc);
					sIndex+=1;
				}
			}
			break;
		default:
			// Small optimization to remove per-char function overhead for most characters written:
			if(nLen+1 < nAlloc)
			{
				(*sOut)[nLen++] = *sIndex;
				(*sOut)[nLen] = _T('\0');
			}
			else
			{
				Append(*sIndex, sOut, nLen, nAlloc);
			}
			sIndex++;
			break;
		}
	}
	Append(_T('\0'), sOut, nLen, nAlloc);
}

// --------------------------------------------------------------------------------------
// NOTE: This only supports shrinking string operations, not growing.
//

void Replace(TCHAR *sSrc, TCHAR *sSearch, TCHAR *sReplace)
{
	TCHAR *sRead = sSrc, *sWrite = NULL, *sTemp = NULL, *sWildSearch = NULL;
	UINT nAdvance, uLen;
	TCHAR cTemp;

	if(NULL == sSrc || NULL == sSearch || NULL == sReplace)
		return;

	uLen = _tcslen(sSrc);

	sWrite = new TCHAR[uLen + 1];
	sWildSearch = new TCHAR[_tcslen(sSearch) + 1];
    if(NULL == sWrite || NULL == sWildSearch)
		goto Exit;

	sWrite[0] = _T('\0');
	_tcscpy(sWildSearch, sSearch);

	// Handle wildcards
	sTemp = _tcschr(sWildSearch, _T('*'));
	if(NULL != sTemp)
	{
		nAdvance = 0;
		*sTemp = _T('\0');
	}
	else
	{
		nAdvance = _tcslen(sWildSearch);
	}

	while(NULL != sRead)
	{		
		sTemp = _tcsstr(sRead, sWildSearch);
		if(NULL == sTemp)
		{
			_tcscat(sWrite, sRead);
			sRead = NULL;
		}
		else
		{
			cTemp = *sTemp;
			*sTemp = _T('\0');
            _tcscat(sWrite, sRead);
			_tcscat(sWrite, sReplace);
			*sTemp = cTemp;

			if(0 == nAdvance)
				while((sTemp - sSrc + 1) < uLen && *(++sTemp) != _T(' '));
			sRead = sTemp + nAdvance;
		}		
	}
	_tcscpy(sSrc, sWrite);

Exit:
	SAFEDELETEA(sWrite);
	SAFEDELETEA(sWildSearch);
}

// --------------------------------------------------------------------------------------

void CLogWindow::StripRTF(TCHAR *sString)
{
	if(NULL == sString)
	{
		DPF(1, _T("CLogWindow::StripRTF() - invalid params.\n"));
		return;
	}

	Replace(sString, _T("\\tab "), _T("\t"));
	Replace(sString, _T("\\b "), _T(""));
	Replace(sString, _T("\\i "), _T(""));
	Replace(sString, _T("\\ul "), _T(""));
	Replace(sString, _T("\\b0 "), _T(""));
	Replace(sString, _T("\\i0 "), _T(""));
	Replace(sString, _T("\\ul0 "), _T(""));
	Replace(sString, _T("\\cf*"), _T(""));
}

// --------------------------------------------------------------------------------------

void CLogWindow::Update()
{
	if(NULL != m_hWindow)
		SendMessage(m_hWindow, WM_SETTEXT, 0, (LPARAM)m_sLog);
}

// --------------------------------------------------------------------------------------

bool CLogWindow::GetLogLine(UINT nLine, TCHAR *sBuffer, UINT nSize)
{
	UINT i=0;
	TCHAR *sLine = NULL, *sLog = NULL;
	TCHAR cTemp;

	if(NULL == sBuffer || NULL == m_sLog)
	{
		DPF(1, _T("CLogWindow::GetLogLine() - invalid params.\n"));
		return false;
	}

	sLog = m_sLog + _tcslen(m_sRTFHeader);

	while(NULL != sLog)
	{
		if(i == nLine)
		{
			sLine = _tcsstr(sLog, _T("\\line "));

			if(sLine)
			{
				cTemp = *sLine;
				*sLine = _T('\0');
				_tcsncpy(sBuffer, sLog, nSize-1);
				sBuffer[_tcslen(sLog)+1] = _T('\0');
				*sLine = cTemp;
			}
			else
			{
				_tcsncpy(sBuffer, sLog, nSize-1);
				sBuffer[_tcslen(sLog)+1] = _T('\0');
			}
			StripRTF(sBuffer);
			break;
		}
		
        sLine = _tcsstr(sLog, _T("\\line "));
		sLog = (sLine == NULL) ? NULL : sLine + 6;
		i++;
	}

	return i == nLine;
}

// --------------------------------------------------------------------------------------

UINT CLogWindow::GetNumLines()
{
	TCHAR *sTemp = m_sLog;
	UINT nCount = 0;

	while(sTemp = _tcsstr(sTemp, _T("\\line ")))
	{
		sTemp += 6;
		nCount++;
	}

	return nCount;
}

