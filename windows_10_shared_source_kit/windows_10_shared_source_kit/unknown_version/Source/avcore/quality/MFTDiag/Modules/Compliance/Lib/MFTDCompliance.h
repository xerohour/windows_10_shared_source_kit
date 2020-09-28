#ifndef __MFTDCompliance_H__
#define __MFTDCompliance_H__

#include "MFTDiagTestDLLCommon.h"
#include "XMLConfig.h"

HRESULT RunComplianceTest(const DWORD dwTestID, ILogger* pLogger, CXmlRootConfig* pXMLConfig, const GUID* pGUIDToTest, 
                          const WCHAR* pszHWURL);
HRESULT InitializeComplianceTestTable(CXmlRootConfig** ppXMLRootConfig);

#endif