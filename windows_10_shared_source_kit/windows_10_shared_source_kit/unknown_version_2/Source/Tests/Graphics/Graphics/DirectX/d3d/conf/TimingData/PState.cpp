////////////////////////////////////////////////////////////////////////////////
// PState.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "PState.h"

////////////////////////////////////////////////////////////////////////////////
// CPStateTest
////////////////////////////////////////////////////////////////////////////////

#define DRAW_COUNT_SLOW         10
#define DRAW_COUNT_MODERATE     30
#define DRAW_COUNT_FAST         100

#define DRAW_INTERVAL_SLOW      1000
#define DRAW_INTERVAL_MODERATE  200
#define DRAW_INTERVAL_FAST      1

#define PSTATE_ERROR_TOLERANCE      0.000200
#define PSTATE_ERROR_TOLERANCE_ADJ  0.000100

CPStateTest::CPStateTest()
{
}

TEST_RESULT CPStateTest::SetupTestCase()
{
	TEST_RESULT tr = CTimingDataTest::SetupTestCase();
	HRESULT hr = S_OK;
	if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0)
	{
		return RESULT_SKIP;
	}
	if (tr != RESULT_PASS)
	{
		return tr;
	}

	tr = SetupPipeline();

	return tr;
}

TEST_RESULT CPStateTest::ExecuteTestCase()
{
	const UINT drawCounts[] = { DRAW_COUNT_SLOW, DRAW_COUNT_MODERATE, DRAW_COUNT_FAST };
	const UINT drawCountsCount = (sizeof(drawCounts) / sizeof(drawCounts[0]));
	const UINT drawIntervals[] = { DRAW_INTERVAL_SLOW, DRAW_INTERVAL_MODERATE, DRAW_INTERVAL_FAST };
	static_assert(drawCountsCount == (sizeof(drawIntervals) / sizeof(drawIntervals[0])),
		"drawCounts[] and drawIntervals[] must have the same number of elements.");
	const UINT expectedDraws = DRAW_COUNT_SLOW + DRAW_COUNT_MODERATE + DRAW_COUNT_FAST;

	HRESULT hr = E_FAIL;
	float timestampDeltas[DRAW_COUNT_SLOW + DRAW_COUNT_MODERATE + DRAW_COUNT_FAST - 1];
	vector<CEvent_CalibrateGpuClock*> calibrateEvents;

	TEST_RESULT tr = StartETWTracing();
	if (tr != RESULT_PASS)
	{
		return tr;
	}

	// draw from long intervals to shorter ones to force p-state transitions
	for (UINT p = 0; p < drawCountsCount; ++p)
	{
		for (UINT count = 0; count < drawCounts[p]; ++count)
		{
			TogglePS();

			m_pDeviceContext->Draw(4, 0);

			if (FAILED(hr = ExecuteEffectiveContext()))
			{
				LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
					hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute);
				StopETWTracing();
				return tr;
			}

			Sleep(drawIntervals[p]);
		}
	}

	tr = StopETWTracing();
	if (tr != RESULT_PASS)
	{
		return tr;
	}
	//
	// validate...
	//
	if (GetFramework()->IsCompat() || GetFramework()->Is11On12())
	{
		tr = FilterCalibrateEvents(&calibrateEvents);
		if (tr != RESULT_PASS)
		{
			return tr;
		}
		if ((UINT32)calibrateEvents.size() < 2)
		{
			LogError(__FILEW__, __LINE__, L"Not enough calibrate events present.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
			return RESULT_FAIL;
		}
		LARGE_INTEGER cpuFrequency;

		if (QueryPerformanceFrequency(&cpuFrequency) == FALSE)
		{
			LogError(__FILEW__, __LINE__, L"Failure to query for performance frequency.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
			return RESULT_FAIL;
		}
		const UINT64 gpuFrequencyReported = calibrateEvents[0]->m_GpuFrequency;
		UINT i = 1;
		for (; i < calibrateEvents.size(); ++i)
		{
			if (calibrateEvents[i]->m_GpuFrequency != gpuFrequencyReported)
				break;
		}
		if (i >= calibrateEvents.size())
		{
			const double gpuFrequencyTolerance = 3;
			const UINT64 cpuDeltaOverall = calibrateEvents[calibrateEvents.size() - 1]->m_CpuClock - calibrateEvents[0]->m_CpuClock;
			const UINT64 gpuDeltaOverall = calibrateEvents[calibrateEvents.size() - 1]->m_GpuClock - calibrateEvents[0]->m_GpuClock;
			const double cpuSecElapsed = (double)cpuDeltaOverall / (double)cpuFrequency.QuadPart;
			const UINT64 gpuFrequency = (UINT64)((double)gpuDeltaOverall / cpuSecElapsed);
			INT64 diff = gpuFrequency - gpuFrequencyReported;
			if (diff < 0)
			{
				diff = -diff;
			}
			double parts = (double)diff * 1000 / (double)gpuFrequency;
			if (parts > gpuFrequencyTolerance)
			{
				LogError(__FILEW__, __LINE__, L"Failure PState parts is greater than tolerance", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
				return RESULT_FAIL;
			}
		}
	}
	else
	{
		if (m_pXperfHelper == NULL)
		{
			m_pXperfHelper = new XperfHelper(NORMAL, true, LARGE, FALSE);
			m_pXperfHelper->SetMergedFile(ETLFileName());
		}
		hr = m_pXperfHelper->Action(L"gputiming -test 1024 -noheader", RESULTS_FILE, L"perf_gputiming.dll");
		hr = S_OK;              //return code from Action() currently unreliable - todo
		if (!SUCCEEDED(hr))
		{
			tr = RESULT_FAIL;
		}
		if (tr == RESULT_PASS)
		{
			tr = ReadResultFile();
		}
	}
	if (tr == RESULT_FAIL)
	{
		LogError(__FILEW__, __LINE__, L"Failure PState", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
		return RESULT_FAIL;
	}

	return tr;
}

void CPStateTest::CleanupTestCase()
{
	CTimingDataTest::CleanupTestCase();

	CleanupPipeline();
}
