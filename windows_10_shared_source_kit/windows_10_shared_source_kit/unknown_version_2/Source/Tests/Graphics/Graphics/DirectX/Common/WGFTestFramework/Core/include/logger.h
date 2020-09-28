#pragma once

#include <windows.h>
#include <time.h>
#include <WTTLogger.h>
#include "DXGLogger.h"


class CDXGTestLogger : public CDXGLogger
{
	public:
		CDXGTestLogger();


	public:
		// Text Logging
		virtual PCTSTR GenerateCmdline
		(
			__out_ecount( MAX_PATH ) PTSTR sBuffer
		);


	protected:
		virtual HRESULT TraceToWindow
		(
			UINT uLevel,
			__in const PCSTR sMsg
		);

		virtual HRESULT TraceToWindow
		(
			UINT uLevel,
			__in const PCWSTR sMsg
		);
};
