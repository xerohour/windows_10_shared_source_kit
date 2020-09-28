/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include "main.hpp"

bool HybridPerf::Setup()
{
    // Step 1: Force the app off the d-list to ensure that we are testing the right thing
    HMODULE hDXGI = GetModuleHandle(L"dxgi.dll");
    if (!hDXGI)
    {
        return false;
    }
    typedef void ( WINAPI * PFNSetAppCompatStringPointer) ( SIZE_T, const char* );
    PFNSetAppCompatStringPointer pfnShim = (PFNSetAppCompatStringPointer)GetProcAddress(hDXGI, "SetAppCompatStringPointer");
    const char* szShim = "Hybrid=3";
    pfnShim(strlen(szShim) + 1, szShim);

    // Step 2: Make sure that we are on a system with 2 GPUs
    CComPtr<IDXGIFactory2> spFactory;
    CComPtr<IDXGIAdapter1> spAdapters[2];
    VERIFY_SUCCEEDED(CreateDXGIFactory2(0, __uuidof(spFactory.p), (void**)&spFactory));
    VERIFY_SUCCEEDED(spFactory->EnumAdapters1(0, &spAdapters[0])); // always at least basic render
    HRESULT hr = spFactory->EnumAdapters1(1, &spAdapters[1]);
    
    if (FAILED(hr))
    {
        Log::Result(TestResults::Skipped, L"No hardware adapters in the system.");
        return true;
    } 

    DXGI_ADAPTER_DESC1 desc;
    VERIFY_SUCCEEDED(spAdapters[1]->GetDesc1(&desc));
    if (desc.VendorId == BASIC_RENDER_VENDOR && desc.DeviceId == BASIC_RENDER_DEVICE)
    {
        Log::Result(TestResults::Skipped, L"Only one hardware adapter in the system.");
        return true;
    }

    return true;
}

ULONGLONG GetResult(CMetric* pMetric)
{
    return (*pMetric->List())[0]->ullValue();
}

void HybridPerf::RunTest(UINT numShaderLoops, UINT numPrimitives, wchar_t* testName)
{
    CProvider etwProvider;
    wchar_t* ETLFile = L"merged.etl";
    ULONGLONG results[2];

    for (UINT i = 0; i < 2; ++i)
    {
        std::vector<CMetric*> MetricList;
        GraphicsWorker worker;
        worker.SetFeatureLevel(D3D_FEATURE_LEVEL_11_0);
        worker.SelectAdapter(i);
        worker.SetWindowless();
        worker.SetPixelsPerTriangle(1024*512);
        worker.SetLoopsInShader(numShaderLoops);
        worker.SetTrianglesPerPrimitive(20);
        worker.SetPrimitivesPerFrame(numPrimitives);
        worker.SetNonInteractive();
        worker.SetTime(0);
        worker.SetBlend(numShaderLoops == 1);
        worker.SetWaitForWork(true);
        worker.SetFlushRate(100);

        XperfHelper xperf(LITE, false);
        xperf.Start();

        EventWriteLogMarkerStart(1,L"StartTest");
        HRESULT hr = worker.Run();
        EventWriteLogMarkerStop(1, L"EndTest");

        xperf.Stop();
        VERIFY_SUCCEEDED(hr);

        
        ULONGLONG ullRangeStart = 0;
        ULONGLONG ullRangeStop = 0;
        ULONG ulProcessId = 0;
        ULONGLONG ullRangeTotal = 0;

        CTestMarksHelper TestMarks(ETLFile);
        TestMarks.GetTestRange(1,&ullRangeStart,&ullRangeStop,&ulProcessId,&ullRangeTotal);

        //------------------------------------------------------------------
        // This block designs the search to use over the Engine Data.
        CDMAEngineHelper dmaEngineHelper(ETLFile);
        dmaEngineHelper.SetEcho(true);
        WCHAR wszFilter[MAX_PATH];
        StringCchPrintf(wszFilter,MAX_PATH,L"-range %I64d %I64d -process %d -active",
                        ullRangeStart,ullRangeStop,ulProcessId);
        dmaEngineHelper.GetUsage(L"results.csv", testName, wszFilter, ullRangeTotal, &MetricList);

        results[i] = 0;
        for (UINT j = 1; j < MetricList.size(); j += 3)
        {
            results[i] = max(results[i], GetResult(MetricList[j]));
        }
    }
    
    // dGPU time spent should be less than or equal to iGPU time spent
    VERIFY_IS_LESS_THAN_OR_EQUAL(results[1], results[0]);
}

#if 0
// Disabling as part of bug 2242775
void HybridPerf::FillRate()
{
    RunTest(1, 3000, L"FillRate");
}

void HybridPerf::ShaderThroughput()
{
    RunTest(30, 1000, L"ShaderThroughput");
}
#endif