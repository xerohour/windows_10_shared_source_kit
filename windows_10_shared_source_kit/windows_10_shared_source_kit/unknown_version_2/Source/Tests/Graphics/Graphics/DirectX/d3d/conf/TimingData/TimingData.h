////////////////////////////////////////////////////////////////////////////////
// TimingData.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "d3d11_2.h"
#include "D3D11Test.h"

#include "d3d11events.h"
#include "d3d12events.h"
#include "lddmcoreeventdefs.h"
#include "XperfHelper.h"
#include "LegacyDxEtwHelper.h"
#include "TimingDataHelper.h"

#include <string>
#include <vector>
using namespace std;

#include "Direct3DFailureCategoryStrings.h"

#define ETL_FILE            L"WGF11TimingData.etl"
#define RESULTS_FILE        L"WGF11TimingData.csv"
#define LOGSESSION_NAME     L"DxTest"
#define MARKER_NAME_START   L"TimingData_Start"
#define MARKER_UID      1337

#define STR_SIZE        256

extern string g_szPassthroughVS; 
extern string g_szPassthroughPS;
extern string g_szPassthroughPS_Alt;
extern string g_szBasicCS;
extern string g_szBasicCS_Alt;


//
// TRException
//
struct TRException
{
	TRException(tstring error, TEST_RESULT tr = RESULT_FAIL)
		: sError( error ),
		  tRes( tr )
	{}

	void Prepend( tstring str ) { sError = str + sError; }
	tstring GetError() { return sError + str2tstr( "\n" ); }
	TEST_RESULT GetResult() { return tRes; }

private:
	tstring sError;
	TEST_RESULT tRes;
};

struct EventDxgContext
{
    UINT64 hContext;
    UINT64 ContextHandle;
    UINT64 hDevice;
    UINT32 NodeOrdinal;
    UINT32 EngineAffinity;
    UINT32 DmaBufferSize;
    UINT32 DmaBufferSegmentSet;
    UINT32 DmaBufferPrivateDataSize;
    UINT32 AllocationListSize;
    UINT32 PatchLocationListSize;
    UINT32 ContextFlags;
    UINT64 pAdapter;
};

struct DxgkDeviceD3dContext
{
    UINT64 DxgiDevice;
    UINT64 ContextHandle;
    UINT64 D3dDevice;
};

#pragma pack(push, r1, 1)
    struct EventBatch
    {
        UINT32 QPCLow;
        UINT8 ThreadIDOrdinal;
        UINT8 MarkerAPI;
    };
#pragma pack(pop, r1)

struct EventBatch12
{
    UINT32 QPCLow;
    UINT8 ThreadIDOrdinal;
    UINT8 MarkerAPI;
};

struct EventRuntimeMarkerData
{
public:
    UINT64 m_Context;
    UINT64 m_ApiSeqNum;
    UINT32 m_MarkerApi;
    UINT32 m_ThreadId;
    UINT64 m_HistoryBuff;
    UINT64 m_DxgiDev;
    UINT64 QPC;
    UINT64 CpuFreq;
    bool m_fSeen;
    bool fSorted = false;

    bool operator<(const EventRuntimeMarkerData &B) const
    {
        if(m_HistoryBuff == B.m_HistoryBuff)
        {
            return QPC < B.QPC;
        }
        return m_HistoryBuff < B.m_HistoryBuff;
    }
};

struct EventHistoryBuff
{
    CEvent_HistoryBuffer* EventData;
    UINT64 m_GpuClock;
    UINT64 m_CpuClock;
    UINT64 m_GpuFrequency;
};

struct EventDxgDevice
{
    UINT64 hProcessId;
    UINT64 pDxgAdapter;
    UINT32 ClientType;
    UINT64 hDevice;
};

struct EventExecuteCommandList
{
    UINT64 pCommandList;
    UINT64 pCommandQueue;
    UINT64 CommandListSequenceNum; // used only for filtering
};

struct EventResults
{
    UINT64 Draws;
    UINT64 m_Pid;
    UINT64 Task;
    bool m_Is11On12 = false;
    list<vector<BYTE>> EventMemory;
    vector<EventDxgContext> EventDxgContext;
    vector<DxgkDeviceD3dContext> EventDxgDevCon;
    vector<CEvent_RuntimeDroppedCall*> DroppedVector;
    vector<CEvent_CommandList*> CommandListVector;
    vector<CEvent_CalibrateGpuClock*> CalibrateClockVector;
    vector<CEvent_CommandBufferSubmission*> CommandBufferVector;
    vector<CEvent_MarkerInt*> MarkerIntVector;
    vector<CEvent_HistoryBuffer*> HistoryVector;
    vector<CEvent_CustomDriverMarker*> CustomVector;
    vector<EventRuntimeMarkerData*> RuntimeMarkerVector;
    vector<EventHistoryBuff> ValidHistoryVector;
    vector<EventDxgDevice> ValidDxgkDeviceVector;
    vector<CEvent_CalibrateGpuClock*> ValidCalibrateVector;
    vector<EventRuntimeMarkerData*> PrempRuntimeMarkerVector;
    vector<EventExecuteCommandList*> ValidExecuteVector;
};


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
    bool IsSetRedBlueCS()
    {
        return m_bRedBlueCS;
    }
    bool Is11On12()
    {
        return m_D3DOptions.Force11On12;;
    }
    bool IsCompat()
    {
        return m_MobileCompat || this->IsMobile();
    }

private:
    bool m_bRedBlueCS;          //Wrap Api/Flush in Critical Section in Red/Blue, for testing
    bool m_MobileCompat;
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
    CTimingDataApp *GetFramework() const { return dynamic_cast< CTimingDataApp *>(::GetFramework()); }
    CTimingDataTest(bool fEnableETW = true, PWCHAR ETL_file = ETL_FILE);
    virtual TEST_RESULT Setup();
    virtual TEST_RESULT SetupTestCase();
    virtual void CleanupTestCase();
    virtual void Cleanup();

    void TogglePS();
    void ToggleCS();
    virtual TEST_RESULT SetupPipeline();
    virtual void CleanupPipeline();

    TEST_RESULT EnableETWTracing(PWCHAR EtlFile);
    TEST_RESULT StartETWTracing(bool fDraw=false, PWCHAR EtlFile=NULL);
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
    HRESULT FilterAndSortRuntimeMarkers(vector<EventRuntimeMarkerData> *SortedRuntimeMarkers, UINT64 StartTime);
    TEST_RESULT FilterRuntimeMarkers(vector<EventRuntimeMarkerData> *RuntimeMarkers);
    TEST_RESULT ReadResultFile();
    void GatherSpecifiedEvents();
    EventResults m_Results;
protected:
    BOOL SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile);
    void SaveViewToBmp(ID3D11DeviceContext2 *m_pDeviceContext, ID3D11Texture2D *pStagingTex, ID3D11Texture2D *pRTTex, LPCTSTR bmpfile);
    void SaveMapToBmp(D3D11_MAPPED_SUBRESOURCE *pMapData, UINT width, UINT height, LPCTSTR bmpfile, bool invert = true);

    //
    // Overridable
    //
    const D3D11_INPUT_ELEMENT_DESC * m_layout;
    int m_layout_count;
    const void *m_vertices;
    size_t m_vertices_size;
    size_t m_vertices_count;
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
    UINT m_width;
    UINT m_height;
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
    ID3D11Texture2D *m_pUATex;
    ID3D11RenderTargetView *m_pRTView;
    ID3D11ShaderResourceView *m_pSRView;
    ID3D11UnorderedAccessView *m_pUAView;
    ID3D11SamplerState* m_pSampler;

    ID3D11Device2 *m_pDevice;
    ID3D11DeviceContext2 *m_pDeviceContext;

    // ETW
    CTimingDataHelper *m_pTimingDataHelper;
    XperfHelper *m_pXperfHelper;
    WCHAR m_wszProcessOption[STR_SIZE];
    bool m_bTracing;
    PWCHAR ETLFileName()
    {
        return m_ETL_file;
    }

private:
    PWCHAR m_ETL_file;
    DWORD m_Pid;
};

