#pragma once

#include <windows.h>
#include <time.h>
#include <WTTLogger.h>
#include "DXGLogger.h"


class CLogWindow;


class CDXG9Logger : public CDXGLogger
{
	public:
		CDXG9Logger();


	public:
		// Text Logging
		virtual PCTSTR GenerateCmdline
		(
			DWORD nBuffer,
			__inout_ecount(nBuffer) const PTSTR sBuffer
		);


	protected:
		virtual HRESULT TraceToWindow
		(
			UINT uLevel,
			__in const PCTSTR sMsg
		);


	public:
		CLogWindow * pLogWindow;
};
