#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include "DXG9Logger.h"
#include "CWindow.h"
#include "TestUtils.h"


CDXG9Logger::CDXG9Logger()
{
	pLogWindow = NULL;
}

// --------------------------------------------------------------------------------------

HRESULT CDXG9Logger::TraceToWindow
(
	UINT uLevel,
	__in const PCTSTR sMsg
)
{
	if ( NULL != pLogWindow )
	{
		pLogWindow->WriteToLogWhole(uLevel, sMsg);
	};

	return S_OK;
};


// --------------------------------------------------------------------------------------

PCTSTR CDXG9Logger::GenerateCmdline
(
	DWORD nBuffer,
	__inout_ecount(nBuffer) const PTSTR sBuffer
)
{
	TCHAR *sPtr1 = NULL;
	TCHAR *sPtr2 = NULL;

	bool bPure = false;
	bool bSWVP = false;

	TCHAR sToken[MAX_PATH] = {0};

	ASSERT(NULL != sBuffer);
	ASSERT(NULL != m_DeviceInfo.sDevice);
	ASSERT(NULL != m_DeviceInfo.sResolution);
	ASSERT(NULL != m_LogInfo.sCmdLine);

    
	// -DXN
	sPtr1 = sPtr2 = _tcsstr(m_DeviceInfo.sDevice, _T("DX"));
	if(sPtr1)
	{
		while(*sPtr2 != _T(' ')) sPtr2++;
		_tcscat(sBuffer, _T(" -"));
		_tcsncat(sBuffer, sPtr1, sPtr2 - sPtr1);
	}

	// -SRC:PUREREF
	bPure = NULL != _tcsstr(m_DeviceInfo.sDevice, _T("Pure"));
	bSWVP = NULL == _tcsstr(m_DeviceInfo.sDevice, _T("HW VP"));

	if(_tcsstr(m_DeviceInfo.sDevice, _T("HAL")))
	{
		_tcscat(sBuffer, _T(" -SRC:"));
		if(bPure)
			_tcscat(sBuffer, _T("PURE"));
		else if(bSWVP)
			_tcscat(sBuffer, _T("SVP"));
		else
			_tcscat(sBuffer, _T("HVP"));

		_tcscat(sBuffer, _T("HAL"));
	}
	else if(_tcsstr(m_DeviceInfo.sDevice, _T("REF")))
	{
		_tcscat(sBuffer, _T(" -SRC:"));
		if(bPure)
			_tcscat(sBuffer, _T("PURE"));
		else if(bSWVP)
			_tcscat(sBuffer, _T("SVP"));
		else
			_tcscat(sBuffer, _T("HVP"));
		_tcscat(sBuffer, _T("REF"));
	}
	else if(_tcsstr(m_DeviceInfo.sDevice, _T("SW")))
	{
		_tcscat(sBuffer, _T(" -src:"));
		if(bPure)
			_tcscat(sBuffer, _T("PURE"));
		else if(bSWVP)
			_tcscat(sBuffer, _T("SVP"));
		else
			_tcscat(sBuffer, _T("HVP"));
		_tcscat(sBuffer, _T("SW"));
	}

	// -DisplayMode:?x?xR5G6B5
	if(NULL != (sPtr1 = _tcschr(m_DeviceInfo.sResolution, _T(' '))))
	{
		_tcscat(sBuffer, _T(" -DisplayMode:"));
		_tcsncat(sBuffer, m_DeviceInfo.sResolution, sPtr1 - m_DeviceInfo.sResolution);
	}

	// Append any other cmdline tokens on the end
	sPtr1 = m_LogInfo.sCmdLine + _tcslen(m_LogInfo.sExeName);
	while(*sPtr1 != _T('\0'))
	{
		// Advance past the whitespace
		while(*sPtr1 == _T(' ')) sPtr1++;
		sPtr2 = sPtr1;

		// Isolate the token
		while(*sPtr1 != _T(' ') && *sPtr1 != _T('\0'))
			sPtr1++;
		_tcsncpy(sToken, sPtr2, sPtr1 - sPtr2);
		sToken[sPtr1 - sPtr2] = _T('\0');
		_tcslwr(sToken);

		// Filter out tokens that we're already specifying
		if(*sToken != _T('\0') &&
		   0 != _tcsicmp(sToken, _T("-dx9")) &&
		   0 != _tcsicmp(sToken, _T("-dx9.1")) &&
		   0 != _tcsicmp(sToken, _T("-dx9.0")) &&
		   0 != _tcsicmp(sToken, _T("-dx8")) &&
		   0 != _tcsicmp(sToken, _T("-dx8.0")) &&
		   0 != _tcsicmp(sToken, _T("-dx8.1")) &&
		   0 != _tcsicmp(sToken, _T("-dx7")) &&
		   0 != _tcsicmp(sToken, _T("-dx7.0")) &&
		   0 != _tcsicmp(sToken, _T("-dx6")) &&
		   0 != _tcsicmp(sToken, _T("-dx6.0")) &&
		   0 != _tcsicmp(sToken, _T("-configure")) &&
		   NULL == _tcsstr(sToken, _T("-src:")) &&
		   NULL == _tcsstr(sToken, _T("-displaymode:")))
		{
			_tcscat(sBuffer, _T(" "));
			_tcsncat(sBuffer, sPtr2, sPtr1 - sPtr2);
		}
	}


	return sBuffer;
}
