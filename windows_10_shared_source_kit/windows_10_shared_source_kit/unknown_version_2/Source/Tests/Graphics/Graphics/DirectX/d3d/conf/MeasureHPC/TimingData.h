////////////////////////////////////////////////////////////////////////////////
// TimingData.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "d3d11_2.h"
#include "D3D11Test.h"
#include <D3DSampleLocation.h> // for TRException

#include "d3d11events.h"
#include "lddmcoreeventdefs.h"
#include "XperfHelper.h"
#include "LegacyDxEtwHelper.h"
#include "TimingDataHelper.h"

#include <string>
#include <vector>
using namespace std;

#include "Direct3DFailureCategoryStrings.h"

#define ETL_FILE		    L"WGF11TimingData.etl"
#define ETL_TEAPOT          L"WGF11TimingData_Teapot.etl"
#define RESULTS_FILE		L"WGF11TimingData.csv"
#define LOGSESSION_NAME		L"DxTest"
#define MARKER_NAME_START	L"TimingData_Start"
#define MARKER_UID		1337

#define STR_SIZE		256

extern string g_szPassthroughVS; 
extern string g_szPassthroughPS;
extern string g_szPassthroughPS_Alt;
extern string g_szBasicCS;
extern string g_szBasicCS_Alt;

////////////////////////////////////////////////////////////////////////////////
//
// CTimingDataApp
//
////////////////////////////////////////////////////////////////////////////////
class CTimingDataApp : public CD3D11TestFramework
{
public:
    CTimingDataApp();

    TESTTYPE GetTestType() const { return TESTTYPE_CONF; }

private:
    bool InitTestGroups();
    void InitOptionVariables();
};

////////////////////////////////////////////////////////////////////////////////
//
// CTimingDataTest
//
////////////////////////////////////////////////////////////////////////////////
class CTimingDataTest : public CD3D11Test
{
public:
    CTimingDataTest();
    virtual TEST_RESULT Setup();
    virtual TEST_RESULT SetupTestCase();
    virtual void CleanupTestCase();
    virtual void Cleanup();

    void TogglePS();
    void ToggleCS();
    virtual TEST_RESULT SetupPipeline();
    virtual void CleanupPipeline();

    TEST_RESULT EnableETWTracing();
    TEST_RESULT StartETWTracing();
    TEST_RESULT StopETWTracing();
    TEST_RESULT FormSequenceNumbers(vector<UINT64> *pResults,
            const UINT32 &high,
            const vector<UINT32> &vLow0,
            const vector<UINT32> &vLow1);
    TEST_RESULT GetMarkerBegin(UINT64 *pMarkerBegin);
	TEST_RESULT FilterHistoryBufferEvents(vector<CEvent_HistoryBuffer*> *pHistoryBufferEvents, bool fCheckOrder = false);
    TEST_RESULT FilterCalibrateEvents(vector<CEvent_CalibrateGpuClock*> *pCalibrateEvents);
    TEST_RESULT FilterCommandBufferEvents(vector<CEvent_CommandBufferSubmission*> *pCommandBufferEvents);
    TEST_RESULT FilterDroppedCallEvents(vector<CEvent_RuntimeDroppedCall*> *pDroppedCallEvents);
    TEST_RESULT FilterCommandListEvents(vector<CEvent_CommandList*> *pCommandListEvents);
    TEST_RESULT CorrelateTimestamps(vector<CEvent_HistoryBuffer*> *pHistoryBufferEvents, vector<UINT64> *pTimestamps, vector<CEvent_HistoryBuffer*> *pFilteredEvents = nullptr, vector<UINT64> *pElapsedTime=nullptr);
    TEST_RESULT CorrelateTimestampsToApiSequenceNumbers(vector<CEvent_HistoryBuffer*> *pHistoryBufferEvents, vector<UINT64> *pTimestamps, vector<UINT64> *pElapsedTime, vector<UINT64> apiSequenceNumbers, vector<CEvent_HistoryBuffer*> *pFilteredEvents = nullptr);
protected:
    BOOL CTimingDataTest::SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile);
    void CTimingDataTest::SaveViewToBmp(ID3D11DeviceContext2 *m_pDeviceContext, ID3D11Texture2D *pStagingTex, ID3D11Texture2D *pRTTex, LPCTSTR bmpfile);
    //
    // Overridable
    //
    const D3D11_INPUT_ELEMENT_DESC * m_layout;
    int m_layout_count;
    const void *m_vertices;
    size_t m_vertices_size;
    size_t m_vertices_count;
    UINT m_width;
    UINT m_height;
    enum D3D_PRIMITIVE_TOPOLOGY m_topology;
    bool m_display;  //display
    UINT m_stride;
    UINT m_offset;
    ID3D11Texture2D *m_pStagingTex;
    string m_szVS;
    string m_szPS;
    string m_szPS_Alt;
    string m_szCS;
    string m_szCS_Alt;
    UINT m_RTWidth;
    UINT m_RTHeight;
    UINT m_UAWidth;
    UINT m_UAHeight;
    UINT m_SRWidth;
    UINT m_SRHeight;

    ID3D11Buffer *m_pVB;
    ID3D11Buffer *m_pIB;
    ID3D11RasterizerState *m_pRS;
    ID3D11VertexShader *m_pVS;
    ID3D11PixelShader *m_pPS;
    ID3D11PixelShader *m_pPS_0;
    ID3D11PixelShader *m_pPS_1;
    ID3D11ComputeShader *m_pCS;
    ID3D11ComputeShader *m_pCS_0;
    ID3D11ComputeShader *m_pCS_1;
    ID3D11InputLayout *m_pInputLayout;
    ID3D11Texture2D *m_pRTTex;
    ID3D11Texture2D *m_pSRTex;
    ID3D11Texture2D	*m_pUATex;
    ID3D11RenderTargetView *m_pRTView;
    ID3D11ShaderResourceView *m_pSRView;
    ID3D11UnorderedAccessView *m_pUAView;

    ID3D11Device2 *m_pDevice;
    ID3D11DeviceContext2 *m_pDeviceContext;

    // ETW
    CTimingDataHelper *m_pTimingDataHelper;
    XperfHelper *m_pXperfHelper;
    WCHAR m_wszProcessOption[STR_SIZE];
    PWCHAR m_ETL_file;
    bool m_bTracing;
};

