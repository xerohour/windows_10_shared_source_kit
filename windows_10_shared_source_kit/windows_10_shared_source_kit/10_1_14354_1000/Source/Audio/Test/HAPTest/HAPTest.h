// Copyright(C) Microsoft.All rights reserved.

// haptest.h

class CHapTest : public WEX::TestClass<CHapTest>
{
public:
    BEGIN_TEST_CLASS(CHapTest)
    
        // Setup and cleanup run elevated.
        // Unfortunately everything runs inproc because we're using TAEF's device support,
        // but this will at least trigger TAEF to give a meaningful error if the test is run non-elevated.
        //
        // To wit:
        //
        // Error: TAEF: [HRESULT: 0x80070545] Failed to initialize InProc test execution
        // (RunAs:Elevated requires that Te.exe is running elevated when used with the /inproc setting).
        TEST_CLASS_PROPERTY(L"RunFixtureAs", L"Elevated")
        
        // TAEF will call CoInitialize for us
        TEST_CLASS_PROPERTY(L"ThreadingModel", L"MTA")
    
    END_TEST_CLASS()

    TEST_CLASS_SETUP(SetupClass);
    TEST_CLASS_CLEANUP(CleanupClass);

    BEGIN_TEST_METHOD(OffloadFormatControl)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'RenderOffload'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(BufferSize)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'RenderOffload'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(UnderrunGlitchReport)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Reliability")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'RenderOffload'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(DuplicateWritePositionGlitchReport)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Reliability")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'RenderOffload'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(VolumeImmediate)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'RenderOffload'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(Mix)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'RenderOffload'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(ForbiddenFormats)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"IDX")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'RenderOffload'")
    END_TEST_METHOD()
   
    BEGIN_TEST_METHOD(EOSEvent)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'RenderOffload'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(Raw)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(CommunicationCaptureDefault)
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Capture'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(CommunicationRenderRaw)
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(LoopbackBeforeRender)
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(APORegistration)
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render' OR @Type = 'Capture'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(SupportedFormatsPcm)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"IDX")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'RenderOffload'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(LoopbackProtectMute)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"IDX")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'RenderOffload'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(LoopbackProtectDisable)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"IDX")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'RenderOffload'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(MixOffloadDifferentFormat)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'RenderOffload'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(HardwareVolume)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"IDX")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render' OR @Type = 'Capture'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(HardwareMute)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"IDX")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render' OR @Type = 'Capture'")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(HardwarePeakMeter)
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"IDX")
        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render' OR @Type = 'Capture'")
    END_TEST_METHOD()

private:
    void RunMixTest(BOOL bDifferentFormats = FALSE);
};

_Success_(return != 0)
bool DeviceUnderTest(_In_ IBasicLog *pLogger, _COM_Outptr_ IMMDevice **ppMMDevice, _Outptr_opt_ bool *pbSysvad = NULL);

class IOffloadPath;
interface IKsControl;

HRESULT GetOffloadPath(_In_ IBasicLog *pLogger, _In_ IMMDevice *pMMDevice, _COM_Outptr_ IOffloadPath **ppOffloadPath);

HRESULT GetAdapterKsControl(_In_ IBasicLog *pLogger, _In_ IOffloadPath *pOffloadPath, _COM_Outptr_ IKsControl **ppKsControl);

#define NO_SUCH_DEVICE L"None"

#define DEFAULT_FUZZ_DURATION (60*60) // 1 hour

template <typename T>
class CStopOnExit {
public:
    CStopOnExit() {}
    CStopOnExit(T *p) : m_p(p) {}
    ~CStopOnExit() { if (m_p) { VERIFY_SUCCEEDED(m_p->Stop()); } }
    void Update(T *p) { m_p = p; }
private:
    CComPtr<T> m_p;
};

class CNormalizeVolumeAndRestoreOnExit {
public:
    CNormalizeVolumeAndRestoreOnExit(IMMDevice *pMMDevice);
    ~CNormalizeVolumeAndRestoreOnExit();
    
private:
    CComPtr<IAudioEndpointVolume> m_pAudioEndpointVolume;
    float m_volume;
    bool m_restore;
};

#define BITS_PER_BYTE 8
#define HNS_PER_MS 10000
#define MS_PER_SECOND 1000

double DbFromMagnitude(double m);
double MagnitudeFromDb(double dB);
double DbFromSineAmp(double a);
double SineAmpFromDb(double dB);
LONG KsVolumeFromDb(double dB);

// needs a filter IKsControl
HRESULT GetModes(IKsControl *pKsControl, UINT pin, GUID **ppModes, UINT *nModes);

// needs a stream IKsControl
HRESULT SetVolume(IKsControl *pKsControl, UINT node, double dB);
HRESULT SetVolumeRamp(IKsControl *pKsControl, UINT node, double dB, double seconds);

void LogTime(IBasicLog *pLogger, LPCWSTR szMessage);
LPCWSTR GetDeviceState(DWORD dwDeviceState);

struct ChannelInfo {
    WORD nChannels;
    DWORD dwChannelMask;
    LPCWSTR szName;
    
    ChannelInfo(WORD n, DWORD dw, LPCWSTR sz)
        : nChannels(n)
        , dwChannelMask(dw)
        , szName(sz)
        {}
};
WORD OnBits(UINT32 mask);
#define CHANNELINFO(mask, name) ChannelInfo(OnBits(mask), mask, name)

// 44100 => "44.1 kHz"
CString StringSampleRate(int r);

struct AudioClientPropertiesWin8 {
    UINT32                  cbSize;
    BOOL                    bIsOffload;
    AUDIO_STREAM_CATEGORY   eCategory;
};

struct AudioClientPropertiesWin81 {
    UINT32                  cbSize;
    BOOL                    bIsOffload;
    AUDIO_STREAM_CATEGORY   eCategory;
    AUDCLNT_STREAMOPTIONS   Options;
};

#define VERIFY_ASCENDING_INCLUSIVE(x, y, z) VerifyAscendingInclusive(pLogger, x, L ## #x, y, L ## #y, z, L ## #z)

bool VerifyAscendingInclusive(
    IBasicLog *pLogger,
    double x_value, LPCWSTR x_label,
    double y_value, LPCWSTR y_label,
    double z_value, LPCWSTR z_label
);

struct Tolerance {
    double Below;
    double Above;
    
    Tolerance(double b, double a) : Below(b), Above(a) {}
};
