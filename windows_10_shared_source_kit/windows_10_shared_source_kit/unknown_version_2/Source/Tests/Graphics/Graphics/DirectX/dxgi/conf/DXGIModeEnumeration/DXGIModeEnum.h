#ifndef DXGIPRESENT_H_INCLUDED
#define DXGIPRESENT_H_INCLUDED


//
// Win32 headers
//

#include <windows.h>
#include <basetsd.h>
#include <mmsystem.h>


//
// Direct3D headers
//

#include <d3d10_1.h>
#include <dxgi.h>
#include <d3dx10.h>


//
// Framework headers
//

#include <WGFTestCore.h>
#include <WGFTestUI.h>
#include <DXGITest.h>


//
// Project headers
//

#include "common.hpp"
#include "DXGIBBFMT.H"
#include "DXGIFMT.H"
#include "DXGIWndClass.h"
#include "DXGISurfUtils.h"
#include "DXGISwapEffects.h"
#include "DXGIValueSets.h"

#include "D3D10RefUtil.h"
#include "D3D10SwapChain.h"

#include "ModeSet.h"


//
// StrSafe.h has to come last, because it deprecates functions
// that it doesn't like.
//

#include <StrSafe.h>


#undef MSG_LEN
#define MSG_LEN     1024

#undef BACK_BUFFER_COUNT
#define BACK_BUFFER_COUNT  2

#define MAX_DXGI_FORMATS        7

#define TRY_TIMES       2
#define TRY_INTERVAL    4000

struct TEST_MODE
{
	UINT AdapterOrdinal;
	UINT OutputOrdinal;
	DXGI_OUTPUT_DESC OutputDescriptor;
	DXGI_MODE_DESC Mode;
	DXGI_MODE_ROTATION Rotation;
};

typedef struct _RELATIVEGETDSDDIMS
{
	float fWidthFactor;
	float fHeightFactor;
} RELATIVEGETDSDDIMS;

typedef struct _PRIORITYPATTERN
{
    UINT    uiModeWidth;
    UINT    uiModeHeight;
} PRIORITYPATTERN;

class CFormatsToTest
{
	public:
		static const DXGI_FORMAT FormatList[ MAX_DXGI_FORMATS ];

		static bool IsFormatInList( DXGI_FORMAT Format );
};


class clsTestableDevice
:
	public clsD3D10Device
{
	//
	// Construction/destruction
	//

	public:
		clsTestableDevice(CTest *pInitLoggingGroup)
		:
			clsD3D10Device(pInitLoggingGroup)
		{
		};

		virtual ~clsTestableDevice();


	//
	// Public methods
	//

	public:
		bool GetDevicePtr(ID3D10Device **pDevicePointer);
};


class clsTestableDXGISwapChain
:
	public clsDXGISwapChain
{
	//
	// Construction/destruction
	//

	public:
		clsTestableDXGISwapChain(CTest *pInitLoggingGroup, clsD3D10Device *pInitParentD3D10Device)
		:
			clsDXGISwapChain(pInitLoggingGroup, pInitParentD3D10Device)
		{
		};

		virtual ~clsTestableDXGISwapChain();


	//
	// Public methods
	//

	public:
		bool GetSwapChainPtr(IDXGISwapChain **pSwapChainPointer);
};


class CBaseModeEnumerationTestGroup
:
	public CDXGITest
{
	//
	// Construction/destruction
	//

	public:
		CBaseModeEnumerationTestGroup();

		virtual ~CBaseModeEnumerationTestGroup();

        static const PRIORITYPATTERN  m_ModePriorityPatterns[];

	//
	// Helper methods
	//

	public:
        LONG ChangeDisplayMode
        (
            __in DEVMODEW   *pDevModeW,
            __in LPWSTR     pswzDeviceName           
        );

		HRESULT EnumerateModes();

		HRESULT EnumerateAdapterModes
		(
        	__in IDXGIAdapter   *pDXGIAdapterEnumMode,
	        __in UINT           uiAdapterOrdinal
		);

		TEST_RESULT CheckForDuplicateModes();
        bool IsStereoCompatibleFormat(DXGI_FORMAT format);
		bool IsModeADupeExceptForRefreshRate(std::vector <TEST_MODE> &ModeList, TEST_MODE ModeToCheck);

	//
	// Member data
	//

	protected:
		std::vector< TEST_MODE > TestedModes;
		bool m_bTestStereoModes;
};


class CValidateModeListTestGroup
:
	public CBaseModeEnumerationTestGroup
{
	//
	// Construction/destruction
	//

	public:
		CValidateModeListTestGroup();

		virtual ~CValidateModeListTestGroup();


	//
	// Framework override methods
	//

	protected:
		virtual void InitTestParameters();

		virtual TEST_RESULT ExecuteTestCase();


	//
	// Member data
	//

	protected:
		UINT m_iCurrentScenario;
};


class CDXGIModeEnum
:
	public CBaseModeEnumerationTestGroup,
	public clsBasicWindowClass
	//public CTestDXGISurfaceUtils,
	//public clsD3D10Ref 
{
	//
	// Construction/destruction
	//

	public:
		CDXGIModeEnum();

		virtual ~CDXGIModeEnum();

	//
	// Framework override methods
	//

	protected:
		virtual void InitTestParameters();


		virtual TEST_RESULT Setup();

		virtual void Cleanup();


		virtual TEST_RESULT SetupTestCase();

		virtual void CleanupTestCase();


		virtual TEST_RESULT ExecuteTestCase();


	//
	// Helper methods
	//

	public:
		HRESULT SetDisplayMode();

		bool UpdateOutputWindowInformation();

		BOOL ReleaseTestCaseObjects();

		TEST_RESULT PresentAndValidate();

		void GetFocusForFullscreen();

		BOOL CompareSwapChainDesc
		(
			__in DXGI_SWAP_CHAIN_DESC & swapChainDesc1,
			__in DXGI_SWAP_CHAIN_DESC & swapChainDesc2
		);

		BOOL CompareSwapChainDesc1
		(
			__in DXGI_SWAP_CHAIN_DESC1 & swapChainDesc1,
			__in DXGI_SWAP_CHAIN_DESC1 & swapChainDesc2
		);

                BOOL CompareFullscreenDesc(
                        __in DXGI_SWAP_CHAIN_FULLSCREEN_DESC & fullscreenDesc1,
                        __in DXGI_SWAP_CHAIN_FULLSCREEN_DESC & fullscreenDesc2
                );

		TEST_RESULT SetupFullScreenSwapChain();

		TEST_RESULT ValidateMode();


	//
	// Member data
	//

	protected:
		IDXGIFactory *m_pFactory;
		IDXGIAdapter *m_pAdapter;

		clsTestableDevice PrimaryAdapterDevice;
		ID3D10Device * m_pD3D10Device;

		clsTestableDXGISwapChain TestedSwapChain;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc1;
                DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
		IDXGISwapChain * pSwapChain;
		IDXGISwapChain1 * pSwapChain1;

		HWND hwndFocusWindow;
		HWND hwndDeviceWindow;

		// Simplified information about the output and the relationship between the device
		// window and the output.
		//

		IDXGIOutput * pDeviceWindowOutput;
		UINT uiDeviceOutputWidth;
		UINT uiDeviceOutputHeight;
		RECT rectDeviceWindow;

		int iModeIndex;
		int iSwapEffectIndex;

		int iDSDFmtIndex;
		int iDSDDimIndex;
};

class CDXGIStereoModeEnum
:
	public CDXGIModeEnum
{
public:
	CDXGIStereoModeEnum() { m_bTestStereoModes = true; }
};

class CValidateStereoModeListTestGroup
:
	public CValidateModeListTestGroup
{
public:
	CValidateStereoModeListTestGroup() { m_bTestStereoModes = true; }
};

int __cdecl CompareModes
(
	__in const void *pv1,
	__in const void *pv2
);

int __cdecl CompareModes
(
	__in const TEST_MODE *pModeDesc1,
	__in const TEST_MODE *pModeDesc2
);

#endif
