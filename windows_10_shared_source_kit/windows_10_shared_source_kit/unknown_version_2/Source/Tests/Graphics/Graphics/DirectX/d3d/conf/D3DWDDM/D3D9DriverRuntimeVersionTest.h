#pragma once

// Includes
#include "D3DWDDM.h"

///////////////////////////////////////////////////////
// Enums and Structs
///////////////////////////////////////////////////////
enum TEST_DDI_VERSION
{
	TEST_WDDM_2_0	= D3D_UMD_INTERFACE_VERSION_WDDM2_0,
	TEST_WDDM_1_3	= D3D_UMD_INTERFACE_VERSION_WDDM1_3,
	TEST_WIN8		= D3D_UMD_INTERFACE_VERSION_WIN8,
	TEST_WIN7		= D3D_UMD_INTERFACE_VERSION_WIN7,
	TEST_INVALID	= 0xee000
};
DECLARE_NAMED_VALUES( TEST_DDI_VERSION )

////////////////////////

class CD3D9DriverRuntimeVersionTest : public CD3D11Test
{
public:
	CD3D9DriverRuntimeVersionTest(void);
	void InitTestParameters();
	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	void Cleanup();

protected:
	DevX::GRFX::AdapterDescriptorCore *m_pAdapterDescriptor;
	TEST_DDI_VERSION m_RuntimeVersion;
	TEST_DDI_VERSION m_OSCorrespondingWDDMVersion;
	D3DKMT_DRIVERVERSION m_kmtDriverVersion;
};

