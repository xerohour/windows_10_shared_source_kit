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

// Includes
#pragma warning ( disable : 4005 )	// Disable dupe definitions for ntstatus types
#include <ntstatus.h>
#include <Psapi.h>
#include <dxgiddi.h>

////////////////////////

class CSystemCheckApp : public CD3D11TestFramework
{
public:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	void InitOptionVariables();
	bool InitTestGroups();

	void SetMinimumFeatureLevel( const D3D_FEATURE_LEVEL &fl ) { m_minimumFeatureLevel = fl; }
	D3D_FEATURE_LEVEL GetMinimumFeatureLevel() const { return m_minimumFeatureLevel; }

protected:
	D3D_FEATURE_LEVEL m_minimumFeatureLevel;
};