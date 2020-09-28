#include "CD3DWindowFramework.h"
#include "GetAdapterIdentifierMain.h"

#include <stdio.h>
#include <mmsystem.h>

extern CD3DWindowFramework TestApp;

////////////////////////////////////////////////////////////////////////////////
// CUpdateSurfaceTestMain methods

CONST UINT CGetAdapterIdentifierMain::cuiNoOfTestCases = 3;
CONST UINT CGetAdapterIdentifierMain::cuiNoOfTimedCallsWHQL = 10;
CONST UINT CGetAdapterIdentifierMain::cuiNoOfTimedCallsDriverVersion = 1000;
const float CGetAdapterIdentifierMain::fMinEnumToNoEnumTimeRatio = 1.2f;

CGetAdapterIdentifierMain::CGetAdapterIdentifierMain()
{
	SetTestType(TESTTYPE_CONF);

	m_szCommandKey= _T("GetAdapterIdentifier");
	m_szLogName = _T("GetAdapterIdentifier");
	m_szTestName= _T("GetAdapterIdentifier");

	m_Options.D3DOptions.fMinDXVersion = 9.f;
	//m_Options.D3DOptions.fMaxDXVersion = 9.f;
	m_Options.TestOptions.bCompareSrcRef = false;
}

CGetAdapterIdentifierMain::~CGetAdapterIdentifierMain()
{
}

UINT CGetAdapterIdentifierMain::TestInitialize(void)
{
	HRESULT hresult;
	DEVICE_CREATION_PARAMETERS dcpSrcDevice;

	// Determine the adapter ordinal of the Direct3D device.
	if (SUCCEEDED((hresult = m_pSrcDevice -> GetCreationParameters(&dcpSrcDevice))))
	{
		uiSrcDeviceAdapterOrdinal = dcpSrcDevice.AdapterOrdinal;
		SetTestRange(1, cuiNoOfTestCases);
		return D3DTESTINIT_RUN;
	}

	return D3DTESTINIT_ABORT;
}

bool CGetAdapterIdentifierMain::ExecuteTest(UINT uiTestCaseId)
{
	bool bTestCaseExecuted = false;

	// Record commencement of a test case.
	BeginTestCase("Test Case", uiTestCaseId);

	switch (uiTestCaseId)
	{
	case 1:
		bTestCaseExecuted = EnumWHQLLevel();
		break;
	case 2:
		bTestCaseExecuted = DoNotEnumWHQLLevel();
		break;
	case 3:
		bTestCaseExecuted = CompareTimesWHQL();
		break;
	case 4:
		bTestCaseExecuted = CompareTimesDriverVersion();
		break;
	/*
	case 5:
		bTestCaseExecuted = ();
		break;
	*/
	default:
		ReportInvalidTCID(uiTestCaseId);
		break;
	}

	return bTestCaseExecuted;
}

bool CGetAdapterIdentifierMain::TestTerminate(void)
{
	return true;
}

bool CGetAdapterIdentifierMain::ValidateWHQLLevelValue(DWORD dwTestValue)
{
	UINT uiYear, uiMonth, uiDay;
	bool bReturn = true;

	if ((dwTestValue == 0) || (dwTestValue == 1))
	// The values of 0 and 1 are valid, denoting "not certified" and "certified without a date",
	// respectively.
		return true;

	// Extract the date components from the field and verify that each lies within the
	// expected range.
	uiYear = (UINT)(ExtractBitsRange(dwTestValue, 16, 31));
	if (uiYear < 1999)
	{
		WriteToLog(_T("Invalid Year Component: %d.  (Expected >= 1999.)\n"), uiYear);
		bReturn = false;
	}

	uiMonth = (UINT)(ExtractBitsRange(dwTestValue, 8, 15));
	if ((!uiMonth) || (uiMonth >= 13))
	{
		WriteToLog(_T("Invalid Month Component: %d.  (Expected 1--12.)\n"), uiMonth);
		bReturn = false;
	}

	uiDay = (UINT)(ExtractBitsRange(dwTestValue, 0, 7));
	if ((!uiDay) || (uiDay >= 32))
	{
		WriteToLog(_T("Invalid Day Component: %d.  (Expected 1--31.)\n"), uiDay);
		bReturn = false;
	}

	return bReturn;
}

bool CGetAdapterIdentifierMain::EnumWHQLLevel()
{
	HRESULT hresult;
	D3DADAPTER_IDENTIFIER9 d3dai9;

	if (FAILED((hresult = m_pD3D -> GetAdapterIdentifier(uiSrcDeviceAdapterOrdinal, 0, (ADAPTER_IDENTIFIER*)(&d3dai9)))))
	{
		WriteToLog(_T("GetAdapterIdentifier failed in CGetAdapterIdentifierMain::EnumWHQLLevel.\n"));
		Fail();
	}

	if (!ValidateWHQLLevelValue(d3dai9.WHQLLevel))
		Fail();

	return true;
}

bool CGetAdapterIdentifierMain::DoNotEnumWHQLLevel()
{
	HRESULT hresult;
	D3DADAPTER_IDENTIFIER9 d3dai9;

	// Assign the WHQLLevel field a non-zero value to verify that the GetAdapterIdentifier call
	// actually clears the field.
	d3dai9.WHQLLevel = 1;

	if (FAILED((hresult = m_pD3D -> GetAdapterIdentifier(uiSrcDeviceAdapterOrdinal, 0, (ADAPTER_IDENTIFIER*)(&d3dai9)))))
	{
		WriteToLog(_T("GetAdapterIdentifier failed in CGetAdapterIdentifierMain::DoNotEnumWHQLLevel.\n"));
		Fail();
	}

	if (d3dai9.WHQLLevel)
	{
		WriteToLog(_T("GetAdapterIdentifier inserted a non-zero value into the WHQLLevel field when called without the D3DENUM_WHQL_LEVEL flag.\n"));
		Fail();
	}

	return true;
}

bool CGetAdapterIdentifierMain::CompareTimesWHQL()
{
    //commented out since d3d9 doesn't support this function on Vista.
	return true;
}

bool CGetAdapterIdentifierMain::CompareTimesDriverVersion()
{
	HRESULT hresult;
	D3DADAPTER_IDENTIFIER9 d3dai9;
	UINT uiCount, uiTimeWithDriverVersion, uiTimeWithoutDriverVersion;
	DWORD dwStartTime, dwEndTime;
	float fEnumToNoEnumTimeRatio;

	// Time calls with driver version.
	dwStartTime = timeGetTime();
	for (uiCount = 0; uiCount < cuiNoOfTimedCallsDriverVersion; ++uiCount)
	{
		hresult = m_pD3D -> GetAdapterIdentifier(uiSrcDeviceAdapterOrdinal, 0, (ADAPTER_IDENTIFIER*)(&d3dai9));
	}
	dwEndTime = timeGetTime();
	uiTimeWithDriverVersion = (UINT)(dwEndTime - dwStartTime);
	// Note: If the system timer has "wrapped around" so that dwEndTime < dwStartTime,
	// uiElapsedTimeInMilliseconds is still assigned the correct value owing to the loss
	// of the sign bit when the DWORD is cast into an unsigned integer.

	d3dai9.WHQLLevel = 0;

	// Time calls without driver version.
	dwStartTime = timeGetTime();
	for (uiCount = 0; uiCount < cuiNoOfTimedCallsDriverVersion; ++uiCount)
	{
		hresult = m_pD3D -> GetAdapterIdentifier(uiSrcDeviceAdapterOrdinal, 0x4, (ADAPTER_IDENTIFIER*)(&d3dai9));
	}
	dwEndTime = timeGetTime();
	uiTimeWithoutDriverVersion = (UINT)(dwEndTime - dwStartTime);

	if (FAILED(hresult))
		WriteToLog("Call to GetAdapterIdentifier with 0x4 flag failed.\n");

	if (d3dai9.WHQLLevel)
		WriteToLog("0x4 caused WHQL enumeration!\n");

	if ((fEnumToNoEnumTimeRatio = (float)uiTimeWithDriverVersion / (float)uiTimeWithoutDriverVersion) < fMinEnumToNoEnumTimeRatio)
	{
		WriteToLog(_T("GetAdapterIdentifier without driver version retrieval was not significantly faster than GetAdapterIdentifier with driver version retrieval.\n"));
		Fail();
	}

	WriteToLog("Ratio: %f\n", fEnumToNoEnumTimeRatio);

	//Fail();

	return true;
}
