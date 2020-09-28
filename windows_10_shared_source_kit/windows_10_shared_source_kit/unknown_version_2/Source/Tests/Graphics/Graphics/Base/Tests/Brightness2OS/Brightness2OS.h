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

    class Brightness2OS: public WEX::TestClass<Brightness2OS>
    {
        IDXGIAdapter1*      m_pAdapter;
        AdapterDescriptor*  m_pAd;

        D3DKMT_HANDLE       m_hAdapter;

        HINSTANCE           m_hGdi;
        PFND3DKMT_ESCAPE    m_pfnEscape;

        bool                m_IsAdaptiveBrightness;

        bool Cleanup();
        HRESULT CallEscape(__inout D3DKMT_ESCAPE* Escape);
        bool ControlBrightnessManualMode(bool bStartTrue_EndFalse);

    public:
        // Declare this class as a TestClass, and supply metadata if necessary.
        TEST_CLASS(Brightness2OS)

        TEST_METHOD_SETUP(SetupBrightness2OS)
        TEST_METHOD_CLEANUP(CleanupBrightness2OS)

        // Declare the tests within this class
        TEST_METHOD(TestAdaptiveBrightnessPresence)
        TEST_METHOD(TestAC2DCTransition)
        TEST_METHOD(TestDC2ACTransition)
        TEST_METHOD(TestFullsreenVideoSenario)
        TEST_METHOD(TestDynamicDimmedTrasition)

    };

} /* namespace DevX */ } /* namespace GRFX */ } /* namespace BaseVideo */
