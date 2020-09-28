#ifndef __MFTVisValTests_H__
#define __MFTVisValTests_H__

#include "MFTDiagTestDLLCommon.h"
#include "XMLConfig.h"

HRESULT RunVisValTest(const DWORD dwTestID, ILogger* pLogger, CXmlRootConfig* pXMLConfig, IMFActivate* pActivate);
HRESULT InitializeVisValTestTable(CXmlRootConfig** ppXMLRootConfig);

extern BOOL g_bUseCabacForQualityTests;
extern BOOL g_bDisableMPEG4pt2DecoderPostProcessing;

#endif