#ifndef __MFTCompSessionTests_H__
#define __MFTCompSessionTests_H__

#include "MFTDiagTestDLLCommon.h"
#include "XMLConfig.h"

HRESULT RunTranscodeTest(const DWORD dwTestID, ILogger* pLogger, CXmlRootConfig* pXMLConfig, IMFActivate* pActivate);
HRESULT InitializeTranscodeTestTable(CXmlRootConfig** ppXMLRootConfig);

#endif