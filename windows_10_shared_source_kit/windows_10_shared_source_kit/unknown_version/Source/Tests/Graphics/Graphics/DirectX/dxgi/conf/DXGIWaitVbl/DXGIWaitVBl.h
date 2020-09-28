#ifndef DXGIWAITVBL_H_INCLUDED
#define DXGIWAITVBL_H_INCLUDED


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
// STL headers
//

#include <vector>


//
// Framework headers
//

#include <WGFTestCore.h>
#include <WGFTestUI.h>
#include <DXGITest.h>


//
// Project headers
//

#include "DXGIBBFMT.H"
#include "DXGIValueSets.h"
#include "DXGIWndClass.h"

#include "D3D10RefUtil.h"
#include "D3D10SwapChain.h"

#include "QPC.h"


#define MAX_PERCENT_DIFFERENCE 0.33f
#define NO_OF_TESTED_INTERVALS 100
#define MAX_FAILING_INTERVALS 25
#define MAX_ADAPTERS 4
#define MAX_OUTPUTS 3
#define DEFAULT_WINDOW_WIDTH 256
#define DEFAULT_WINDOW_HEIGHT 256

typedef struct _MODEPRIORITYPATTERN
{
    UINT    uiModeWidth;
    UINT    uiModeHeight;
} MODEPRIORITYPATTERN;

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
		bool GetSwapChainPtr(IDXGISwapChain **m_pSwapChainPointer);
};


class CDXGIWaitVBl
:
	public CDXGITest,
	public clsBasicWindowClass,
	public clsD3D10Ref
{
	//
	// Construction/destruction
	//

	public:
		CDXGIWaitVBl();

		virtual ~CDXGIWaitVBl();

        static const MODEPRIORITYPATTERN  m_ModePriorityPatterns[];

	protected:
		UINT m_uNumAdapters;
		UINT m_vecNumOutputs[MAX_ADAPTERS];
		UINT m_uAdapter, m_uOutput, m_uModeIndex;
        UINT m_FailedCases;   

		LARGE_INTEGER m_liFrequency;
		std::vector<DXGI_MODE_DESC> m_vecTestedModes[MAX_ADAPTERS][MAX_OUTPUTS];
		int m_uiModeIndex;

		clsTestableDevice PrimaryAdapterDevice;
		ID3D10Device *m_pD3D10Device;
		HWND hwndFocusWindow;
		HWND hwndDeviceWindow;
		IDXGIFactory *m_pFactory;
		clsTestableDXGISwapChain TestedSwapChain;
		IDXGISwapChain *m_pSwapChain;
		IDXGIOutput *m_pDXGIOutput;
		float m_fRefreshRateInHz;
        float m_fMeasuredIntervals[NO_OF_TESTED_INTERVALS];

		bool bRefDevice;

		UINT EnumerateModes();
		void ReleaseTestCaseObjects();
		bool UpdateOutputWindowInformation();
		bool SetupDevSCFS(int m_uiModeIndexArg);
		HRESULT WaitAndPumpMessages(DWORD duration);


	//
	// Framework override methods
	//

	public:
		virtual TEST_RESULT Setup();

		virtual void InitTestParameters();

		virtual TEST_RESULT ExecuteTestCase();

		virtual void Cleanup();
};

#endif
