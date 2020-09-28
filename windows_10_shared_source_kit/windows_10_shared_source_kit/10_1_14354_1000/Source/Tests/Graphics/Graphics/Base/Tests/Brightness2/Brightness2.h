    /*
* Copyright (c) Microsoft Corporation. All rights reserved.
*
* This File was generated using the VisualTAEF C++ Project Wizard.
* Class Name: SpbSoftGpuAcpi
*/

#include "WexTestClass.h"

#include "AdapterUtil.h"


namespace DevX { namespace GRFX { namespace BaseVideo
{
    // The test requires special setup with SoftGPU running on top of ACPI simulated device

    class Brightness2: public WEX::TestClass<Brightness2>
    {
        IDXGIAdapter1*      m_pAdapter;
        AdapterDescriptor*  m_pAd;

        D3DKMT_HANDLE       m_hAdapter;

        HINSTANCE           m_hGdi;
        PFND3DKMT_ESCAPE    m_pfnEscape;

        UCHAR               m_LevelsCnt;
        UCHAR               m_BrightnessMax;
        UCHAR               m_BrightnessMin;
        bool                m_IsBrightness2;

        D3DKMT_DRIVERVERSION m_DriverVersion;

        bool Cleanup();
        HRESULT CallEscape(__inout D3DKMT_ESCAPE* Escape);

    public:
        // Declare this class as a TestClass, and supply metadata if necessary.
        TEST_CLASS(Brightness2)

        TEST_METHOD_SETUP(SetupBrightness2)
        TEST_METHOD_CLEANUP(CleanupBrightness2)

        // Declare the tests within this class
        TEST_METHOD(TestBrightness2Presence)
        TEST_METHOD(TestSmoothBrightnessControl)
        TEST_METHOD(TestAdaptiveBrightnessControl)
    };

} /* namespace DevX */ } /* namespace GRFX */ } /* namespace BaseVideo */
