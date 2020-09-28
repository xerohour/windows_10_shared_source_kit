#ifndef GETADAPTERIDENTIFIER_INCLUDED
#define GETADAPTERIDENTIFIER_INCLUDED

#include <d3d9.h>
#include "Framework.h"

class CGetAdapterIdentifierMain : public CD3DTest
{
private:
	// Private data members
	static CONST UINT cuiNoOfTestCases;
	static CONST UINT cuiNoOfTimedCallsWHQL;
	static CONST UINT cuiNoOfTimedCallsDriverVersion;
	static const float fMinEnumToNoEnumTimeRatio;

public:
	// Public data members
	UINT uiSrcDeviceAdapterOrdinal;

private:
	// Private Methods
	void ReportInvalidTCID(UINT uiTestCaseId)
	{
		WriteToLog("Unable to interpret test case identifier: %d\n", uiTestCaseId);
		SkipTestRange(uiTestCaseId, uiTestCaseId);
	}
	DWORD ExtractBitsRange(DWORD dwSourceBits, UINT uiLSB, UINT uiMSB)
	{
		return (dwSourceBits >> uiLSB) & (DWORD)((UINT)((DWORD)1 << (uiMSB - uiLSB + 1)) - 1);
	}
	bool ValidateWHQLLevelValue(DWORD dwTestValue);
	bool EnumWHQLLevel();
	bool DoNotEnumWHQLLevel();
	bool CompareTimesWHQL();
	bool CompareTimesDriverVersion();

public:
	// Public Methods
	CGetAdapterIdentifierMain();
	~CGetAdapterIdentifierMain();
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uiTestCaseId);
	bool TestTerminate(void);
	//bool DisplayFrame(void);
};

#endif
