#pragma once

//////////////////////////////////////////////////////////////////////////
//  create.h
//  Created:	10/03/2007
//
//  Purpose: 
//////////////////////////////////////////////////////////////////////////

// Includes
#include <vector>
#include <map>
#include "testapp.h"
#include <d3d9.h>
#include <d3d10_1.h>
#include <d3d11.h>
#include <d3dcommon.h>

class CCreateTest : public CTest
{
public:
	CCreateTest();
	void InitTestParameters();
	TEST_RESULT Setup();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	void Cleanup();

protected:
	UINT m_AdapterNum;

	std::map<UINT, D3D_FEATURE_LEVEL> m_uMaxCreatedLevel;
	DXGI_ADAPTER_DESC m_AdapterDesc;
	LARGE_INTEGER m_UserModeDriverVersion;

	struct DeviceInfo
	{
		tstring pnpId;
		tstring adapterName;
		tstring featureLevel;
		tstring driverVersion;
		tstring machineName;
	};
	// map from adapter name to device info
	typedef std::map<tstring, DeviceInfo> DeviceMap_t;
	bool ReadDeviceInfoFromMasterFile(tstring fileName, DeviceMap_t& deviceMap);
	bool WriteDeviceInfoToApprovalFile(tstring fileName, DeviceInfo newDevice);
};


// to and from string for feature level
//tstring __stdcall ToString( D3D10_FEATURE_LEVEL1 level );
//bool __stdcall FromString( D3D10_FEATURE_LEVEL1 *pLevel, const tstring &str );