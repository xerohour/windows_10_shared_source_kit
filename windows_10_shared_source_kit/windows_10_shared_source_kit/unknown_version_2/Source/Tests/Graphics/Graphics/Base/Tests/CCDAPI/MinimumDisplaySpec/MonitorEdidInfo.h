#pragma once
#include "TestResult.h"
#include "Logging.h"

namespace DEVX { namespace GRFX {

	union Resolution
	{
		struct
		{
			USHORT y;
			USHORT x;
		};
		ULONG value;
	};

	std::ostream& operator << ( std::ostream& s, Resolution const& r );

	struct ResCmp
	{
		bool operator ()( Resolution const& l, Resolution const& r );
	};

	struct RefreshSupport
	{
		RefreshSupport();

		bool m_SupportStatus;
		std::set<UINT32> m_VerticalRefreshSet;
	};

	typedef std::map<Resolution, RefreshSupport, ResCmp> ResMapFreqSet;

    TestResult::Type GetMonitorEdidModeList( LoggerWrapper* m_pLogger, DISPLAY_DEVICE const* pMonitor, ResMapFreqSet* pEdidResSet );
    HRESULT EnumEdid( LoggerWrapper* m_pLogger, DISPLAY_DEVICE const* pMonitor, UCHAR edid[128], UCHAR block );
    HRESULT LogMonitorDetails( LoggerWrapper* m_pLogger, DISPLAY_DEVICE const* pMonitor );

} }
