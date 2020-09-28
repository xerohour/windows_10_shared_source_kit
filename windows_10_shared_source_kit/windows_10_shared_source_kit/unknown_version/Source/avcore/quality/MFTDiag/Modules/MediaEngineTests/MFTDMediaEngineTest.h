#pragma once

#pragma prefast(push)
#pragma prefast(disable:6001, "disabling warning for test code")
#pragma prefast(disable:6011, "disabling warning for test code")
#pragma prefast(disable:6031, "disabling warning for test code")
#pragma prefast(disable:6054, "disabling warning for test code")
#pragma prefast(disable:6101, "disabling warning for test code")
#pragma prefast(disable:6262, "disabling warning for test code")
#pragma prefast(disable:6284, "disabling warning for test code")
#pragma prefast(disable:6258, "disabling warning for test code")
#pragma prefast(disable:6388, "disabling warning for test code")
#pragma prefast(disable:26015, "disabling warning for test code")
#pragma prefast(disable:26018, "disabling warning for test code")
#pragma prefast(disable:26019, "disabling warning for test code")
#pragma prefast(disable:26071, "disabling warning for test code")
#pragma prefast(disable:26135, "disabling warning for test code")
#pragma prefast(disable:28159, "disabling warning for test code")
#pragma prefast(disable:28251, "disabling warning for test code")
#pragma prefast(disable:28252, "disabling warning for test code")
#pragma prefast(disable:28718, "disabling warning for test code")
#pragma prefast(disable:28931, "disabling warning for test code")

#include "BaseMFTTest.h"
#include "stdafx.h"
#include "CDWMETWCallback.h"
#include "CHWDRMETWCallback.h"
#include "MFSampleCallbackMFT.h"
#include "MFTDiagTestDLLCommon.h"
#include "MFTDTaefBase.h"

// D3D Headers
#include <D3D11_1.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgidwm.h>

// D2D headers
#include <d2d1_1.h>

#include "utils.h"
#include "SmartDxvadumpmft.h"
#include "HEVESSource_Alloc.h"
#include "HEVESByteStreamHandler.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>

#ifndef BUILD_MINWIN
#include "pch.h"
#include <windowsx.h>
#include <initguid.h>
#include "utilreg.h"
#include "mfinternal.h"
#include "mediatypeutil.h"
#include "wmcodecdsp.h"
#include "mfidl.h"
#include <buttonclient.h>
#include <PowerBase.h>
#endif
#include "cpm.h"

#define CANPLAYTIMEOUT 20000 //wait for 20 seconds max to start the video playback
#define MFDCONNECTED_STANDBY_TEST_TIMEOUT   1000
#define MAX_TEST_DEVICE_ROTATIONS 100
#define DEFAULT_DEVICE_ROTATION_INTERVAL 5

#define DESKTOPMPO_REGKEYLOC L"SOFTWARE\\Microsoft\\Windows\\DWM"
#define DESKTOPMPO_REGKEYNAME L"EnableDesktopOverlays"
#define MAX_FILES 10
#define MAX_FILE_LENGTH 1024
DWORD   g_dwLastTestID = 0;
DBTestInfo  *g_pDBTestInfo = NULL;

typedef enum {
    notMPOCapable,
    rgbMPOCapable,
    yuvMPOCapable
} MPOCapablities;

#ifndef BUILD_MINWIN
HRESULT MediaEngineTestEntryPoint(LPCTSTR pszInput, LPCTSTR pszOutput, LPCTSTR pszParams, LPCTSTR pszTestType,
    DWORD dwTestID, LPCTSTR pszTestTitle, LPCTSTR pszTestDLLName, LPCTSTR pszKID, LPCTSTR pszContentKey, LogType eLogType);
#endif

HRESULT MediaEngineTestEntryPointMobile(LPCTSTR pszInput, LPCTSTR pszOutput, LPCTSTR pszParams, LPCTSTR pszTestType,
    DWORD dwTestID, LPCTSTR pszTestTitle, LPCTSTR pszTestDLLName, LPCTSTR pszKID, LPCTSTR pszContentKey, LogType eLogType);


HRESULT GetOverlayCaps(DXGI_MULTIPLANE_OVERLAY_CAPS* pOverlayCaps, DWORD* pdwLeft, DWORD* pdwTop, DWORD* pdwWidth, DWORD* pdwHeight, CSmartLogger logger);
HRESULT MakeProcessDpiAware( CSmartLogger logger );

typedef enum {
    Playback_None        = 0,
    Playback_HWND        = 1,
    Playback_DComp       = 2,
    Playback_FrameServer = 4,
    Playback_MPOVV       = 5,
    Playback_MPOVV_DXVAHD= 6,
    Playback_MPOVV_Soft  = 7
} PlaybackMode;


class CMediaEngineTestSampleCallbackMFT : public CMFSampleCallbackMFT
{
public:
    CMediaEngineTestSampleCallbackMFT();
    ~CMediaEngineTestSampleCallbackMFT();

    HRESULT CheckMediaType(
        IMFMediaType*   pMT
        );

    STDMETHODIMP GetAttributes(
        IMFAttributes** ppAttributes
        );

    STDMETHODIMP GetInputAvailableType(
        DWORD           dwInputStreamID,
        DWORD           dwTypeIndex,
        IMFMediaType**  ppType
        );
};

struct DXCompareInfo
{
    DWORD dwWidth;
    DWORD dwHeight;

    char ppszFrameFileNames[MAX_FILES][MAX_FILE_LENGTH];
    DWORD dwFileCount;

    GUID subType;
};

class CMediaEngineTest : public CBaseTest, public IMFMediaEngineNotify, public PlaybackCallback, public IMFSampleCallbackMFT_Callback
{

    //data members
public:	
    HANDLE m_hPBCompleted;
    HANDLE m_hFrameStepCompleted;
    HRESULT m_hr;
    HANDLE m_hErrorSignal;
    HANDLE m_hEventCanPlay;
    HANDLE m_hSeekCompleted;
    DWORD  m_dwwidth;
    DWORD  m_dwheight;

private:
    long m_cRef;
    IMFMediaEngine* m_pMediaEngine ;
    HWND *m_pHwnd;
    CMFPlaybackWindow* m_pPlaybackWindow;
    PlaybackMode m_Mode;
    IMFDXGIDeviceManager* m_pDXGIManager;
    bool m_DXVA;    
    bool m_protectedPlay;
    bool m_bHWDRMEnabled;
    UINT m_ResetToken;
    IMFMediaEngineEx*           m_pMediaEngineEx;    
    ID3D11Device*               m_pDX11Device;
    ID3D11DeviceContext*        m_pDevContent;
    IDCompositionVisual*        m_pCompVisual; 
    IDCompositionDevice*        m_pCompDevice;
    IDCompositionTarget*        m_pCompTarget ;

    //MPO
    bool                        m_bMPOTest;
    MPOCapablities              m_isMPOCapable;
    char                        *m_pszFileName;
    char                        *m_pszDXVAFileName;
    DXGI_MULTIPLANE_OVERLAY_CAPS    m_overlayCaps;

    BOOL m_bFullScreen;
    char                         m_ppszDXVAFileNamesWithDecoration[MAX_FILES][MAX_FILE_LENGTH];
    char                         m_ppszNonDXVAFileNamesWithDecoration[MAX_FILES][MAX_FILE_LENGTH];

    //DRM
    WCHAR                       m_wszLAURL[MAX_PATH];
    WCHAR                       m_wszKID[MAX_PATH];

    DWORD                           m_dwLeft;
    DWORD                           m_dwTop;
    DWORD                           m_dwWidth;
    DWORD                           m_dwHeight;

    TCHAR                           m_pszTempOutFile[MAX_PATH];
    char                           *m_pszTempOutFile2; 
    CCPKETWController               m_etwController;
    CCPKETWConsumer                 m_etwConsumer;
    COVERLAY_PRESENTETWCallback*    m_pMPOStatusCallback;
    CHWDRM_DEVICECALLETWCallback*   m_pDRMStatusCallback;
    BOOL                            m_bStopUpdateThread;
    HANDLE                          m_hUpdateLogThread;
    BOOL                            m_bETWInitialized;

    BOOL                            m_fD3DAWARE;

    WCHAR                           m_pszPrevFileName[MAX_PATH];
    WCHAR                           m_pszCurrentFileName[MAX_PATH];

    DWORD                           m_dwCSTimeout;
    DWORD                           m_dwCSIntervalTime;

    bool                            m_bMPOCS;

    bool                            m_bUpdateOverlayInformation;

    bool                            m_bFirstPass;

    ULARGE_INTEGER                  m_uliNextCSTime;

    BOOL                            m_bInterlaced;

    CMediaEngineTestSampleCallbackMFT* m_pSampleCallback;

    ULONGLONG                       m_ullLastCount;

    DWORD                           m_dwRotationCurrent;
    DWORD                           m_dwRotationCount;
    DWORD                           m_dwRotationCurrentCount;
    DWORD                           m_pdwRotationTarget[MAX_TEST_DEVICE_ROTATIONS];
    DWORD                           m_dwRotationIntervalSeconds;
    ULARGE_INTEGER                  m_uliNextRotationTime;
    DWORD                           m_dwPSNRThreshold;

    WCHAR*                          m_pwszContentFileName;
    WCHAR*                          m_pwszContentSourcePath;

    //Frame server mode
    BOOL                         m_fPlaying;
    BOOL                         m_fSeeking;
    BOOL                         m_fLoop;
    BOOL                         m_fEOS;
    BOOL                         m_fStopFrameStep;
    BOOL                         m_fVisVal;
    RECT                         m_rcTarget;
    int                          m_fd;
    int                          m_dwFileCount;
    DWORD m_nFrames;
    float m_flStartPos;
    float m_flSeek;
    double m_fDuration;
    float m_flRate;
    BOOL m_bSeek;
    HANDLE                          m_FrameStepThreadHandle;
    CComPtr <ID3D11Texture2D >      m_spD3D11Texture2D;
    CComPtr<IDXGIOutput>            m_spDXGIOutput;
    DWORD  *m_dwWidths;
    DWORD  *m_dwHeights;
    DXCompareInfo m_ComparisonInfo;

    BOOL m_bDumpFrame;

    //functions

public:

    //PlaybackCallback 
    HRESULT HandlePlaybackEvent(UINT message);	

    //IMFMediaEngineNotify 
    STDMETHODIMP QueryInterface(__in REFIID riid, __out LPVOID* ppvObject); 
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    STDMETHODIMP EventNotify(DWORD event, DWORD_PTR param1, DWORD param2);

    CMediaEngineTest();
    ~CMediaEngineTest();	
    
    HRESULT SetGenericTestSettings(LPCTSTR pszInputFileName, LPCTSTR pszOutputFileName, LPCTSTR pszConfig, LPCTSTR pszTestType, DWORD dwTestID, LPCTSTR pszTestTitle, LPCTSTR pszTestDLL, LogType eLogType);
    HRESULT SetDRMSettings(LPCTSTR pszKID, LPCTSTR pszContentKey);
    HRESULT SetMPOSettings(DXGI_MULTIPLANE_OVERLAY_CAPS overlayCaps, MPOCapablities isMPOCapable, BOOL bFullScreen, DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight);
    
    BOOL    ShouldStopUpdateThread(void);
    HRESULT UpdateEventsInformation(void);

    HRESULT SetProperties(LPCTSTR pszPropertyName, LPCTSTR pszPropertyValues);

    HRESULT RotateDevice();

    void SetTestStatus(HRESULT hr);

    STDMETHODIMP CallbackEntry(
        IMFSample*  pSample
        );
    
    virtual HRESULT CTBSetupTest(void);
    virtual HRESULT CTBExecuteTest(void);

    BOOL IsVisValTest(void);
    HRESULT SetKeepfiles(bool bKeepFiles);
    HRESULT SetDXVA(bool fDxva);
    HRESULT SetStrictTypeCheck(bool fCheck);
    bool GetDXVA();
    HRESULT GetDXComparionInfo(DXCompareInfo& comparisonInfo);

    void FrameStep(void);

protected:
    DWORD   m_dwFrames;
    DWORD   m_dwFrameInterval;
    HRESULT OnCanPlay();
    HRESULT OnFirstFrameReady();
    HRESULT OnEnded();
    HRESULT OnError();
    HRESULT OnLoaded();
    HRESULT SignalEndTest();
    HRESULT InitializeMediaEngine();
    HRESULT DesktopMPOTestSetup();
    HRESULT WINAPI MFCreateMediaEngine(__in DWORD dwCreationFlags,__in IMFAttributes* spAttrib);
    HRESULT CreateMediaEngineAttribute(__in_opt IUnknown* pExtension,__in DXGI_FORMAT d3dRenderTarget,
        __in IMFMediaEngineNotify* pAppCallback,__deref_out IMFAttributes** spAttrib);
    HRESULT CreateDX11Device(__out D3D_FEATURE_LEVEL* pFeatureLevel);
    HRESULT CreateCompVisual(IDCompositionVisual **ppCompVisual);	
    HRESULT CreatePlaybackWindow(void);

    //CBaseTest
    virtual HRESULT InitializeInputParams();
    HRESULT SetDRMAttributes(IPropertyStore **pSourcePS);

    //DRM
    void SetLAURLandKID(LPCTSTR pszContentKey, LPCTSTR pszKID);

    //MPO
    HRESULT EnableEventMonitor(void);
    HRESULT StartUpdateThread(void);
    HRESULT StopUpdateThread(void);
    HRESULT InitializeETW();
    HRESULT RGBOverlayExpected(bool* pbRGB);
    
    //Frame Server
    HRESULT OnFrameSteppingEvent();
    HRESULT OnTransferFrame();
    HRESULT WriteDXVAFrameToFile(PBYTE pData, int nPitch, DWORD dwHeight, DX_FORMAT subtype);
    HRESULT GenerateNewFileName(BOOL bDXVA);
    HRESULT NewFile();
    HRESULT StartFrameStep();
    HRESULT StopFrameStep();
   

    // Seeking and duration.
    void GetDuration(double *pDuration, BOOL *pbCanSeek);
    HRESULT SetPlaybackPosition(double pos); 
    double  GetPlaybackRate();
    HRESULT SetPlaybackRate(float rate);
    double  GetPlaybackPosition();    
    BOOL    IsSeeking();
    HRESULT Seek();
    HRESULT OnSeekEvent();
    HRESULT CreateTexture(ID3D11Device *ppD3D11Device, ID3D11Texture2D **ppTexture2D,BOOL bStaging, BOOL bRead);
};

