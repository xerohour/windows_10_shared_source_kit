//-----------------------------------------------------------------------------
// File: DXGIFrameStats.h
//
// Desc: DXGI GetFrameStatistics tests.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
// Win32 headers
#include <dxgi.h>

// C runtime headers
#include <stdio.h>
#include <tchar.h>

// Framework headers
#include "WGFTestCore.h"
#include "WGFTestUI.h"
#include <d3d10test.h>
#include "DCOMPIsolation.h" // Isolates Win8 DCOMP code so we can still compile the test to run on Win7.

#include <strsafe.h>

// DWM
#include <dwmapi.h>
#include <dwmapip.h>

using namespace testfactor;

//-----------------------------------------------------------------------------
// Enumerations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// enum: RESCHK_TEST_NAMES
// Desc: Resource types that can be tested
//-----------------------------------------------------------------------------
enum TEST_RESOURCE
{
    D3D10VERTEXBUFFER,
    DXGISURFACE,
};

//-----------------------------------------------------------------------------
// enum: TESTS_RESOURCETYPE
// Desc: Resource types that can be tested
//-----------------------------------------------------------------------------
enum TESTS_RESOURCETYPE
{
    DXGIOUTPUT,
    DXGISWAPCHAIN,
};

//-----------------------------------------------------------------------------
// To/FromString
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function: ToString
// Desc: Get string from TESTS_RESOURCETYPE
//-----------------------------------------------------------------------------
tstring __stdcall ToString(TESTS_RESOURCETYPE type)
{
    switch(type)
    {
        case DXGIOUTPUT:
            return tstring( _T("DXGIOutput") );
            break;

        case DXGISWAPCHAIN:
            return tstring( _T("DXGISwapChain") );
            break;

        default:
            break;
    }
    return tstring( _T( "" ) );
};
//-----------------------------------------------------------------------------
// Function: FromString
// Desc: Get TESTS_RESOURCETYPE from string
//-----------------------------------------------------------------------------
bool __stdcall FromString(TESTS_RESOURCETYPE *pType, const tstring &str)
{
    if( tcistring(str) == _T("DXGIOutput") )
    {
        *pType = DXGIOUTPUT;
        return true;
    }

    if( tcistring(str) == _T("DXGISwapChain"))
    {
        *pType = DXGISWAPCHAIN;
        return true;
    }

    return false;
};

//-----------------------------------------------------------------------------
// Function: ToString
// Desc: Get string from TEST_RESOURCE
//-----------------------------------------------------------------------------
tstring __stdcall ToString(TEST_RESOURCE resource)
{
    switch(resource)
    {
        case D3D10VERTEXBUFFER:
            return tstring( _T("D3D10VertexBuffer") );
            break;

        case DXGISURFACE:
            return tstring( _T("DXGISURFACE") );
            break;

        default:
            break;
    }
    return tstring( _T( "" ) );
};
//-----------------------------------------------------------------------------
// Function: FromString
// Desc: Get TEST_RESOURCE from string
//-----------------------------------------------------------------------------
bool __stdcall FromString(TEST_RESOURCE *pType, const tstring &str)
{
    if( tcistring(str) == _T("D3D10VertexBuffer") )
    {
        *pType = D3D10VERTEXBUFFER;
        return true;
    }

    if( tcistring(str) == _T("DXGISURFACE") )
    {
        *pType = DXGISURFACE;
        return true;
    }

    return false;
};

//-----------------------------------------------------------------------------
// DWMAPI typedefs
//-----------------------------------------------------------------------------
// Find if DWM composition is enabled
typedef HRESULT (WINAPI *PFDWMISCOMPOSITIONENABLED)(BOOL* pfEnabled);

// Get DWM timming info
typedef HRESULT (WINAPI *PFNDWMGETCOMPOSITIONTIMINGINFO)(HWND hWnd, DWM_TIMING_INFO* pTimingInfo);

//-----------------------------------------------------------------------------
// Test helper classes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Class: CSwapChainTargetWindow
// Desc: Hellper class to create a window for the swap chain
//-----------------------------------------------------------------------------
class CDXGISwapChainWindow
{
    public:
        // Construction
        CDXGISwapChainWindow() {m_hWnd = NULL;};

        // Destruction
        ~CDXGISwapChainWindow() { if(NULL != m_hWnd) DestroyWindow(m_hWnd); };

        // Create the test window
        HRESULT CreateTestWindow(RECT* pRect = NULL);

        // Get test windows HWND
        HWND GetTestWindow() {return m_hWnd;};

    private:
        // Window handle
        HWND m_hWnd;

        // Window message proc
        static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
};

//-----------------------------------------------------------------------------
// Class: CTestResource
// Desc: Base class for resources to test
//-----------------------------------------------------------------------------
class CTestResource
{
    public:
        // Construction
        CTestResource() 
        { 
            m_pDXGIResource = NULL; 
            m_bIsShared = FALSE;
            m_UsageFlags = D3D10_USAGE_DEFAULT;
        };

        // Destruction
        virtual ~CTestResource() { ReleaseResource(); };

    // Methods
    public:
        // Create the resource
        virtual HRESULT CreateResource(ID3D10Device* pDevice, CD3D10TestFramework* pTestFramework ) = 0;

        // Release the resource
        virtual void ReleaseResource() { SAFE_RELEASE(m_pDXGIResource); };

        // Use the resource
        virtual BOOL UseResource(ID3D10Device* pDevice) { return FAIL;};

    // Test helper methods
    public:
        // Get a pointer to the resource
        virtual IDXGIResource* GetDXGIResource() { return m_pDXGIResource; };

        // Can the resource be sharable
        virtual BOOL IsShared() { return m_bIsShared;};

        // Get CPU access Flags
        virtual D3D10_USAGE GetUsageFlags() { return m_UsageFlags; };

        // Set CPU access Flags
        virtual BOOL SetUsageFlags(D3D10_USAGE Flags) { m_UsageFlags = Flags; return TRUE;};

        // Get the IDXGIResource from an IUnknown interface
        virtual HRESULT SetResourceFromInterface(IUnknown** pIF)
        {
            HRESULT hr = E_FAIL;

            if( NULL == pIF )
                return E_FAIL;

            if( NULL == *pIF )
                return E_FAIL;

            return (*pIF)->QueryInterface(__uuidof(IDXGIResource), (void**)&m_pDXGIResource);;
        };

    // Data members   
    protected:
        // The DXGI Resource
        IDXGIResource* m_pDXGIResource;

        // If the resource can be shared
        BOOL m_bIsShared;

        // CPU Access Flags for the resource to use
        D3D10_USAGE m_UsageFlags;
};

//-----------------------------------------------------------------------------
// Class: CFrameStatsTest
// Desc: Baseclass for DXGIGetFrameStats test
//-----------------------------------------------------------------------------
class CDXGIFrameStatsTest : public CD3D10Test
{
    public:
        // Construction
        CDXGIFrameStatsTest()
        {
            m_bOwnByFullScreen = TRUE;
            m_ResourceType = DXGIOUTPUT;
            m_tp_bDCOMP = false;
            m_bOwnedResource = FALSE;
            m_hWnd = NULL;
            m_CurrentTestResource = D3D10VERTEXBUFFER;
            m_pTestResource = NULL;
            m_pRTView = NULL;
            m_bDWMCompositionEnabled = FALSE;
            SetupDWMStatus();

            // Give our DCOMP isolation object a means to log.
        	m_DCOMPIsolation.SetLogger(GetFramework()->GetLogWrapper());
        };

    // Test framework overrides
    public:
        // Per-testgroup setup
        virtual TEST_RESULT Setup() {return CD3D10Test::Setup();};

        // Per-testgroup cleanup
        virtual void Cleanup() {CD3D10Test::Cleanup();};

        // Init the test parameters
        virtual void InitTestParameters();
        
        // Execute the test
        TEST_RESULT ExecuteTestCase();

    // Methods a test case must override
    public:
        // Run the test case
        virtual TEST_RESULT RunCase(IDXGISwapChain* pDXGISwapChain, IDXGIOutput* pDXGIOutput) = 0;

    // Test helper methods
    public:

        // Alternate test parameter initialization for simple negative testing.
        void InitNegativeTestParameters();

        // Converts HRESULT to string
        virtual LPCTSTR HResultToString(IN HRESULT hResult) const;

        // Logs a message and HRESULT on failure
        virtual HRESULT LogOnFailure(HRESULT hResult, const TCHAR *szText);

        // GetFrameStatistics for current tested resource type
        virtual HRESULT GetFrameStatistics(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput, DXGI_FRAME_STATISTICS* pFrameStats);

		// GetLastPresentCount for current tested resource type
		virtual HRESULT GetLastPresentCount(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput, UINT* pLastPresentCount);

        // Increments the present count for pDXGISwapChain
        virtual HRESULT IncrementPresentCount(IDXGISwapChain* pDXGISwapChain);

        // Takes ownership over the output
        virtual HRESULT TakeOwnership(IDXGISwapChain* pDXGISwapChain, IDXGIOutput* pDXGIOutput = NULL);

        // Release ownership over the output
        virtual HRESULT ReleaseOwnership(IDXGISwapChain* pDXGISwapChain, IDXGIOutput* pDXGIOutput = NULL);

        // IDXGISwapChain->GetOutput wrapper
        HRESULT GetSwapChainOutput(IDXGISwapChain* pDXGISwapChain, IDXGIOutput** ppDXGIOutput);

        // Get the current resource type being tested
        TESTS_RESOURCETYPE GetCurrentResourceType() {return m_ResourceType;};

        // If taking ownership with SetFullscreenState()
        BOOL GetOwnByFullScreen() {return m_bOwnByFullScreen;};

        // Get the test winows hwnd
        HWND GetTestWindow() {return m_hWnd;};

        // Set the test winows hwnd
        void SetTestWindow(HWND hwnd) {m_hWnd = hwnd;};

        // Create the test resource
        HRESULT CreateTestResource();

        // Release the test resource
        void ReleaseTestResource();

        // Setup DWM status flags 
        BOOL SetupDWMStatus();

        // Is composition Enabled
        BOOL IsDWMCompositionEnabled() {return m_bDWMCompositionEnabled;};

    // Data members
    private:
        // Resource type to test
        TESTS_RESOURCETYPE m_ResourceType;

        // Take ownership with IDXGISwapChain->SetFullscreenState() or 
        // IDXGIOutput->TakeOwnership()
        BOOL m_bOwnByFullScreen;        

        // True if we taken ownership
        BOOL m_bOwnedResource;

        // TRUE for windowless (DCOMP) Presents, FALSE for regular Windowed presents.
        BOOL m_tp_bDCOMP;

        CDCompIsolation m_DCOMPIsolation;

        // Test window
        HWND m_hWnd;

        // Resource type we are currnety using
        TEST_RESOURCE m_CurrentTestResource;

        // The resource we are using
        CTestResource* m_pTestResource;

        // Render target view
        ID3D10RenderTargetView* m_pRTView;

        // flag for DWM compositon is enabled
        BOOL m_bDWMCompositionEnabled;
};

//-----------------------------------------------------------------------------
// Test case classes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Class: CDXGIIncrementPresentCountTest
// Desc: Test group to check that DXGI_FRAME_STATISTICS.PresentCount gets
// incremented after a present.
//-----------------------------------------------------------------------------
class CDXGIIncrementPresentCountTest: public CDXGIFrameStatsTest
{
    // Test case overrides
    public:
        // Runs the test case
        TEST_RESULT RunCase(IDXGISwapChain* pDXGISwapChain, IDXGIOutput* pDXGIOutput);
};

//-----------------------------------------------------------------------------
// Class: CDXGIGetLastPresentCountIncrementTest
// Desc: Test group to check that GetLastPresentCount gets incremented after 
// a present.
//-----------------------------------------------------------------------------
class CDXGIGetLastPresentCountIncrementTest: public CDXGIFrameStatsTest
{
    // Test case overrides
    public:
        // Runs the test case
        TEST_RESULT RunCase(IDXGISwapChain* pDXGISwapChain, IDXGIOutput* pDXGIOutput);
};

//-----------------------------------------------------------------------------
// Class: CDXGIWithoutOwnershipTest
// Desc: Test group to verify GetFrameStats fail when called outside of take
// ownership.
//-----------------------------------------------------------------------------
class CDXGIWithoutOwnershipTest: public CDXGIFrameStatsTest
{
    // Test case overrides
    public:
        // Runs the test case
        TEST_RESULT RunCase(IDXGISwapChain* pDXGISwapChain, IDXGIOutput* pDXGIOutput);
        void InitTestParameters() {InitNegativeTestParameters();};
};

//-----------------------------------------------------------------------------
// Class: CDXGINULLStatsTest
// Desc: GetFrameStats(NULL)
//-----------------------------------------------------------------------------
class CDXGINULLStatsTest: public CDXGIFrameStatsTest
{
    // Test case overrides
    public:
        // Runs the test case
        TEST_RESULT RunCase(IDXGISwapChain* pDXGISwapChain, IDXGIOutput* pDXGIOutput);
        //void InitTestParameters() {AddResTypesTestParameters();};
};

//-----------------------------------------------------------------------------
// Class: CDXGIGetLastPresentCountNULLTest
// Desc: GetLastPresentCount(NULL)
//-----------------------------------------------------------------------------
class CDXGIGetLastPresentCountNULLTest: public CDXGIFrameStatsTest
{
    // Test case overrides
    public:
        // Runs the test case
        TEST_RESULT RunCase(IDXGISwapChain* pDXGISwapChain, IDXGIOutput* pDXGIOutput);
        void InitTestParameters() {InitNegativeTestParameters();};
};

//-----------------------------------------------------------------------------
// Class: CDXGIGlitchTest
// Desc: Verify that that a glitch can be detected.
//-----------------------------------------------------------------------------
class CDXGIGlitchTest: public CDXGIFrameStatsTest
{
    // Test case overrides
    public:
        // Runs the test case
        TEST_RESULT RunCase(IDXGISwapChain* pDXGISwapChain, IDXGIOutput* pDXGIOutput);
        //void InitTestParameters() {AddResTypesTestParameters();};
};

//-----------------------------------------------------------------------------
// Class: CDXGIQueuedFrameTest
// Desc: Try to detect a queued frame
//-----------------------------------------------------------------------------
class CDXGIQueuedFrameTest: public CDXGIFrameStatsTest
{
    // Test case overrides
    public:
        // Runs the test case
        TEST_RESULT RunCase(IDXGISwapChain* pDXGISwapChain, IDXGIOutput* pDXGIOutput);
        //void InitTestParameters() {AddResTypesTestParameters();};
};


//-----------------------------------------------------------------------------
// Class: CResChk_D3D10VB
// Desc: Wrapped D3D10 Buffer
//-----------------------------------------------------------------------------
class CDXGIResource_D3D10VB : public CTestResource
{
    public:
        // Construction
        CDXGIResource_D3D10VB() 
        { 
            m_pVS = NULL;
            m_pPS = NULL;
            m_pInputLayout = NULL;
            m_pD3D10VB = NULL; 
        };

        ~CDXGIResource_D3D10VB() { ReleaseResource(); };

        // Create the resource
        virtual HRESULT CreateResource(ID3D10Device* pDevice, CD3D10TestFramework* pTestFramework );

        // Release the resource
        virtual void ReleaseResource() 
        {
            CTestResource::ReleaseResource();
            SAFE_RELEASE(m_pVS);
            SAFE_RELEASE(m_pPS);
            SAFE_RELEASE(m_pInputLayout);
            SAFE_RELEASE(m_pD3D10VB);
        };

        // Use resource
        virtual BOOL UseResource(ID3D10Device* pDevice);

    // Data members
    private:
        // The D3D10 Vertex Buffer
        ID3D10Buffer* m_pD3D10VB;

        // Shaders
        ID3D10VertexShader* m_pVS;
        ID3D10PixelShader* m_pPS;
        ID3D10InputLayout* m_pInputLayout;

        // XYZW Vertex
        struct XYZW_VERTEX
        {
            float x;
            float y;
            float z;
            float w;
        };
};

//-----------------------------------------------------------------------------
// The test app
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Class: CMyTestApp
// Desc: Test application object for WGF framework.
//-----------------------------------------------------------------------------
class CMyTestApp : public CD3D10TestFramework
{	
    public:
        // Construction
        CMyTestApp() {;};

    // Test framework method overrides
    public:
        // Returns the default type for all test groups in the app
        virtual TESTTYPE GetTestType() const { return TESTTYPE_CONF; };

		void InitOptionVariables()
		{
			CD3D10TestFramework::InitOptionVariables();
		}

        // Registers test groups
        bool InitTestGroups()
        {
            // Register test class with the group manager
            if( !ADD_TEST(_T("GetFrameStatistics\\IncrementPresentCount"), CDXGIIncrementPresentCountTest) ) return false;
            if( !ADD_TEST(_T("Glitch\\DetectGlitch"), CDXGIGlitchTest) ) return false;
            if( !ADD_TEST(_T("GetFrameStatistics\\IV\\WithoutOwnership"), CDXGIWithoutOwnershipTest) ) return false;
            if( !ADD_TEST(_T("GetFrameStatistics\\IV\\NULL_Input"), CDXGINULLStatsTest) ) return false;
			if( !ADD_TEST(_T("GetLastPresentCount\\IncrementPresentCount"), CDXGIGetLastPresentCountIncrementTest) ) return false;
			if( !ADD_TEST(_T("GetLastPresentCount\\IV\\NULL_Input"), CDXGIGetLastPresentCountNULLTest) ) return false;
            if( !ADD_TEST(_T("QueuedFrames\\1_QueuedFrame"), CDXGIQueuedFrameTest) ) return false;
            
            return true;
        };
};