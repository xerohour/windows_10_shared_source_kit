#include "dlist.h"
#include "WMIHelper.h"

#define BRIGHTNESS_TEST 0
#define BRIGHTNESS_CAPI_TEST 1
#define LEGACY_BRIGHT_NESS_TEST 2

class CBrightnessTest
{
protected:
    MONITOR_LIST_ENTRY Monitors;
    BOOL m_bInitialized;
    CVidWttLog* m_pLog;
    BOOL m_bBrightnessSupported;
    enum TEST_TYPE m_Type;
    
public:
    CBrightnessTest();
    ~CBrightnessTest();
    virtual HRESULT Init(CVidWttLog* pLog)
    {
        //Initilize the head of the list that will contains the monitors.
        InitializeListHeadM(&Monitors);
        return S_OK;
    };

    void SetSupport(BOOL bSupported){m_bBrightnessSupported = bSupported;};
    BOOL GetSupport(){return m_bBrightnessSupported;};
        
    TEST_TYPE GetType() { return m_Type;};
    HRESULT GetMonitorCount(LPDWORD pdwCount)
    {
        *pdwCount = GetItemCount(&Monitors);
        return S_OK;
    }
    void Cleanup();
    virtual HRESULT GetCurrentBrightness(DWORD dwMonitorIndex, BYTE* pbLevel){return S_OK;};
    virtual HRESULT GetSupportedLevels(DWORD dwMonitorIndex, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels){return S_OK;};
    virtual HRESULT SetLevel(DWORD dwMonitorIndex, BYTE bLevel){return S_OK;};
    virtual HRESULT SetALSLevel(DWORD dwMonitorIndex, BYTE bLevel){return S_OK;};
    virtual HRESULT SetALSState(DWORD dwMonitorIndex, BOOLEAN bState){return S_OK;};
    virtual HRESULT RevertToPolicyBrightness(DWORD dwMonitorIndex){return S_OK;};
};

class CWMIBrightnessTest : public CBrightnessTest
{
private:
    HRESULT CreateNewMonitor(PMONITOR_LIST_ENTRY* ppMonitor, VARIANT vInstance);
    HRESULT GetCurrentBrightness(LPCSTR szInstanceName, BYTE* pbLevel);
    HRESULT GetSupportedLevels(LPCSTR szInstanceName, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels);
    HRESULT SetLevel(LPCSTR szInstanceName, BYTE bLevel);
    HRESULT SetALSLevel(LPCSTR szInstanceName, BYTE bLevel);
    HRESULT SetALSState(LPCSTR szInstanceName, BOOLEAN bState);
    HRESULT RevertToPolicyBrightness(LPCSTR szInstanceName);
public:
    CWMIBrightnessTest();
    ~CWMIBrightnessTest();
    virtual HRESULT Init(CVidWttLog* pLog);
    virtual HRESULT GetCurrentBrightness(DWORD dwMonitorIndex, BYTE* pbLevel);
    virtual HRESULT GetSupportedLevels(DWORD dwMonitorIndex, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels);
    virtual HRESULT SetLevel(DWORD dwMonitorIndex, BYTE bLevel);
    virtual HRESULT SetALSLevel(DWORD dwMonitorIndex, BYTE bLevel);
    virtual HRESULT SetALSState(DWORD dwMonitorIndex, BOOLEAN bState);
    virtual HRESULT RevertToPolicyBrightness(DWORD dwMonitorIndex);
    BOOL ReportsBrightnessLevels(VARIANT* pvValue, VARIANT* pvPath);
    
protected:
    CWMIHelper m_WMIHelper;
        
};

class CWMIBrightnessCAPITest : public CBrightnessTest
{
private:
    HRESULT GetCurrentBrightness(LPCSTR szInstanceName, BYTE* pbLevel);
    HRESULT GetSupportedLevels(LPCSTR szInstanceName, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels);
    HRESULT SetLevel(LPCSTR szInstanceName, BYTE bLevel);
    HRESULT SetALSLevel(LPCSTR szInstanceName, BYTE bLevel);
    HRESULT SetALSState(LPCSTR szInstanceName, BOOLEAN bState);
    HRESULT RevertToPolicyBrightness(LPCSTR szInstanceName);
public:
    CWMIBrightnessCAPITest();
    ~CWMIBrightnessCAPITest();
    virtual HRESULT Init(CVidWttLog* pLog);
    string_vector GetInstanceNames(const GUID* Guid);
    virtual HRESULT GetCurrentBrightness(DWORD dwMonitorIndex, BYTE* pbLevel);
    virtual HRESULT GetSupportedLevels(DWORD dwMonitorIndex, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels);
    virtual HRESULT SetLevel(DWORD dwMonitorIndex, BYTE bLevel);
    virtual HRESULT SetALSLevel(DWORD dwMonitorIndex, BYTE bLevel);	
    virtual HRESULT SetALSState(DWORD dwMonitorIndex, BOOLEAN bState);
    virtual HRESULT RevertToPolicyBrightness(DWORD dwMonitorIndex);
    HRESULT TestBrightnessEvent(DWORD dwMonitorIndex);
    

};

class CLegacyBrightnessTest : public CBrightnessTest
{
private:
    HRESULT GetCurrentBrightness(HANDLE hMonitor, BYTE* pbLevel);
    HRESULT GetSupportedLevels(HANDLE hMonitor, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels);
    HRESULT SetLevel(HANDLE hMonitor, BYTE bLevel);
public:
    CLegacyBrightnessTest();
    ~CLegacyBrightnessTest();
    virtual HRESULT Init(CVidWttLog* pLog);
    virtual HRESULT GetCurrentBrightness(DWORD dwMonitorIndex, BYTE* pbLevel);
    virtual HRESULT GetSupportedLevels(DWORD dwMonitorIndex, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels);
    virtual HRESULT SetLevel(DWORD dwMonitorIndex, BYTE bLevel);

};