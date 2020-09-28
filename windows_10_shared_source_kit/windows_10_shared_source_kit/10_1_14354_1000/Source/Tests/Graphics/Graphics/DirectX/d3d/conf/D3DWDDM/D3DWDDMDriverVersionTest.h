#pragma once

// Includes
#include "D3DWDDM.h"
#include "d3d11internaltest.h"

////////////////////////

inline tstring __stdcall ToString( D3D_DRIVER_DDI_VERSION in )
{
    tstring ret;
    switch( in )
    {
    case D3D_DRIVER_DDI_VERSION_2_1	: ret = _T("D3D_DRIVER_DDI_VERSION_2_1"); break;
    case D3D_DRIVER_DDI_VERSION_2_0	: ret = _T("D3D_DRIVER_DDI_VERSION_2_0"); break;
    case D3D_DRIVER_DDI_VERSION_1_3	: ret = _T("D3D_DRIVER_DDI_VERSION_1_3"); break;
    case D3D_DRIVER_DDI_VERSION_1_2	: ret = _T("D3D_DRIVER_DDI_VERSION_1_2"); break;
    case D3D_DRIVER_DDI_VERSION_1_1	: ret = _T("D3D_DRIVER_DDI_VERSION_1_1"); break;
    case D3D_DRIVER_DDI_VERSION_1_0	: ret = _T("D3D_DRIVER_DDI_VERSION_1_0"); break;
	default							: ret = _T("UNKNOWN"); break; 
    }

    return ret;
}

class CD3DWDDMDriverVersionTest : public CD3D11Test
{
public:
	CD3DWDDMDriverVersionTest(void);
	void InitTestParameters();
	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	void Cleanup();

protected:
	ATL::CComPtr<ID3D11Device> m_pDevice;
	ATL::CComPtr<ID3D11DeviceContext> m_pDeviceContext;
	ATL::CComPtr<ID3D11DeviceTest> m_pDevTest;
	DevX::GRFX::AdapterDescriptorCore *m_pAdapterDescriptor;
	D3D_FEATURE_LEVEL m_featureLevel;
	D3DKMT_DRIVERVERSION m_kmtDriverVersion;
};

