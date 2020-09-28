#pragma once

// Includes
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <dxmini.h>
#include <d3dkmddi.h>
#include <d3dkmthk.h>
#include <atlbase.h>
#include "AdapterUtil.h"
#include "D3D11Test.h"
#include "Direct3DFailureCategoryStrings.h"

class CD3DWDDMApp : public CD3D11TestFramework
{
public:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	void InitOptionVariables();
	bool InitTestGroups();

	// Helpers
	bool RunOnAllWDDMVersions() const { return m_bRunOnAllWDDMVersions; }

protected:
	bool m_bRunOnAllWDDMVersions;
};