////////////////////////////////////////
// DXGIMultiPresent.h

#pragma once

// ETW-related headers
#include <etwConsumersRequire.h>
#include "DxgKernelEventHelper.h"
#include <lddmcoreeventdefs.h>
#include "d3dkmthk.h"
#include "d3dkmthk_p.h"
#include "dxmini.h"
#include "d3dkmddi.h"
#include "d3dukmdt.h"
#include "dmmdiag.h"
#include "dxetwevt.hxx"
#include "DxgKernelConsumer.h"
#include <TFrameworkEventHelper.h>
#include "TFrameworkConsumer.h"
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include "Controller.h"
#include "ETWProvider.h"
#include "tlhelp32.h"
#include "AdapterUtil.h"

#include "D3D11Test.h"

// Internal interfaces for CompositionBuffers
#include "d3d11partner.h"

////////////////////////////////////////////////////////////////////////////////
//CMultiPresentApp
////////////////////////////////////////
class CMultiPresentApp : public CD3D11TestFramework
{
	virtual TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	virtual bool InitTestGroups();
	virtual void InitOptionVariables();
};

////////////////////////////////////////////////////////////////////////////////
// CMultiPresentTest
////////////////////////////////////////

class CMultiPresentTest : public CD3D11Test, public CDxgKernelConsumer
{
public:
	CMultiPresentTest();

	void InitTestParameters();
	TEST_RESULT Setup();
	TEST_RESULT ExecuteTestCase();
	void Cleanup();
	bool IsElevated();

// ETW consumer
public:
	void Present_Handler(PEVENT_RECORD pEvent);
    
protected:
	ID3D11Device*			m_pDevice;
	ID3D11PartnerDevice*	m_pPartnerDevice;
	UINT					m_PresentCount;
	UINT					m_numTextures;
};