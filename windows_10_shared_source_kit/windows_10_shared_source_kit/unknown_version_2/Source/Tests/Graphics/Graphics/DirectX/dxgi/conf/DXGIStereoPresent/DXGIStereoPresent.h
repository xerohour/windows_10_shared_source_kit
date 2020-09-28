#pragma once

#include <windows.h>
#include <d3d9.h>
#include <d3d11.h>
#include <dxgi1_2.h>

#include <D3D11Test.h>

#include <vector>

#include "FormatHelper.h"
#include "MinMaxRange.h"

#include <DwmSlicer.h>
#include <DCOMPIsolation.h>

using namespace ImagePipeline;

// User Private
#include <winuserp.h>

// DWM Private
#include <dwmapip.h>

using namespace std;

#define WINDOW_X			0
#define WINDOW_Y			0
#define WINDOW_WIDTH		800
#define WINDOW_HEIGHT		600
#define WINDOW_NAME			"FocusWindow"

#define NUM_FORMATS			2
#define MAX_FORMATMODES 	2
#define PRESENT_ATTEMPTS	1
#define NUM_TEST_ELEMENTS	6

#define POST_PRESENT_SLEEP_TIME				2000
#define FULLSCREEN_TRANSITION_SLEEP_TIME	100
#define CAPTURE_TIME						2000

extern DXGI_FORMAT g_pFormatsToTest[];
extern FLOAT_COLOR g_pEyeColors[];

extern RECT g_pDirtyRects[];

extern bool operator ==(const DXGI_MODE_DESC1 &mode1, const DXGI_MODE_DESC1 &mode2);
extern bool operator !=(const DXGI_MODE_DESC1 &mode1, const DXGI_MODE_DESC1 &mode2);

enum DIRTY_RECT_TEST
{
	DIRTY_RECT_TOP_LEFT,
	DIRTY_RECT_BOTTOM_RIGHT,
	DIRTY_RECT_INTERNAL,
	DIRTY_RECT_PIXEL,
	DIRTY_RECT_FULL,
	DIRTY_RECT_MULTIPLE,
	DIRTY_RECT_SCROLL_BOTTOM_RIGHT,
	DIRTY_RECT_SCROLL_TOP_LEFT
};

DECLARE_NAMED_VALUES(DIRTY_RECT_TEST);

class CDXGIStereoPresent : public CD3D11Test
{
public:
	CDXGIStereoPresent();
	virtual ~CDXGIStereoPresent() { }

	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	
	virtual TEST_RESULT ExecuteTestCase();

	virtual void CleanupTestCase();
	virtual void Cleanup();
	
	HRESULT CreateTestWindow();
	HRESULT InitializeStereoBackbuffer(const bool &bForDirtyRectPresent = false);
	void LogMode();
	void ProcessMessages();

	virtual HRESULT CreateStereoSwapChain() = 0;
	virtual TEST_RESULT ValidateResults() = 0;

protected:
	HWND					m_hWnd;
	HINSTANCE				m_hInstance;

	vector<DXGI_MODE_DESC1>	m_supportedModes;
	IDXGIFactory2			*m_pFactory;
	IDXGISwapChain1			*m_pSwapChain;
	IDXGIOutput1			*m_pOutput;
	ID3D11Texture2D			*m_pStagingBB;

	// DWM Slicer-related members
	DwmSlicer						*m_pSlicer;
	UINT							m_framesCaptured;
	UINT							m_capturableFrames;
	SmartArray<SmartPtr<Texture>>	m_captureAtlas;
	SmartPtr<Texture>				m_frame;
	
	CDCompIsolation			m_DCompIsolation;
	
	BOOL					m_bWindowed;
	BOOL					m_bTestDComp;
	UINT					m_currentMode;
	DXGI_SWAP_EFFECT		m_SwapEffect;
	UINT					m_BufferCount;
	UINT					m_PresentFlags;
	UINT					m_SyncInterval;
	BOOL					m_bTestDirtyRects;
	DIRTY_RECT_TEST			m_DirtyRectTest;
	DXGI_PRESENT_PARAMETERS m_PresentParams;
	RECT					m_Rects[2];
	POINT					m_ScrollOffset;
};

class CDXGIStereoPresentWin : public CDXGIStereoPresent
{
public:
	CDXGIStereoPresentWin();

	void InitTestParameters();
	TEST_RESULT Setup();

	HRESULT CreateStereoSwapChain();
	TEST_RESULT ValidateResults();

protected:
	RECT					m_currentRect;
};

class CDXGIStereoPresentFS : public CDXGIStereoPresent
{
public:
	CDXGIStereoPresentFS();

	void InitTestParameters();
	TEST_RESULT Setup();

	HRESULT CreateStereoSwapChain();
	TEST_RESULT ValidateResults();
};

class CDXGIStereoPresentDComp : public CDXGIStereoPresentWin
{
public:
	CDXGIStereoPresentDComp() { m_bTestDComp = TRUE; }
};
