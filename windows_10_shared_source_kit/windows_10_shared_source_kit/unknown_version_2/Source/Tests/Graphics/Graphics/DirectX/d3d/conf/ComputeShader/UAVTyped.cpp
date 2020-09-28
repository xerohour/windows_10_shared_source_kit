#include "d3dx11async.h"
#include "FormatHelper.h"

#include "UAV.h"

#define EPSILON 0.005f

TEST_RESULT CUAVTypedTest::Setup()
{
    if (g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0)
    {
        return RESULT_SKIP;
    }

    return CUAVTest::Setup();
}

TEST_RESULT CUAVTypedTest::SetupTestCase()
{
    // Test case logic for Typed UAV Load Additional Formats
    if (m_UAVOperation == UAV_OP_READ_AF)
    {
        // Skips
        if (!IsTypedUAVLoadFormatSupported(m_UAVFormat))
        {
            WriteToLog(_T("Skipping: Format is not supported for Typed UAV Load."));
            return RESULT_SKIP;
        }
        if (!IsFormatSupportedForDimension(m_UAVFormat, GetUAVDimension(m_UAVResourceType)))
        {
            WriteToLog(_T("Skipping: Format is not supported for Resource Type."));
            return RESULT_SKIP;
        }
        // Value tweaks
        m_ScaleFactor = 32;
        m_ScaleFactorString = _T("32");
    }
    else
    {
        m_ScaleFactor = m_ScaleFactorDefault;
        m_ScaleFactorString = m_ScaleFactorStringDefault;
    }

    return CUAVTest::SetupTestCase();
}

bool CUAVTypedTest::IsTypedUAVLoadFormatSupported(DXGI_FORMAT Format)
{
    D3D11_FEATURE_DATA_FORMAT_SUPPORT2 Support2Data = { Format, 0 };
    GetDevice()->CheckFeatureSupport(D3D11_FEATURE_FORMAT_SUPPORT2, &Support2Data, sizeof(Support2Data));
    return (Support2Data.OutFormatSupport2 & D3D11_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0;
}

bool CUAVTypedTest::IsFormatSupportedForDimension(DXGI_FORMAT Format, D3D11_UAV_DIMENSION Dimension)
{
    D3D11_FEATURE_DATA_FORMAT_SUPPORT SupportData = { Format, 0 };
    GetDevice()->CheckFeatureSupport(D3D11_FEATURE_FORMAT_SUPPORT, &SupportData, sizeof(SupportData));
    switch (Dimension)
    {
    case D3D11_UAV_DIMENSION_BUFFER:
        return (SupportData.OutFormatSupport & D3D11_FORMAT_SUPPORT_BUFFER) != 0;
    case D3D11_UAV_DIMENSION_TEXTURE1D:
    case D3D11_UAV_DIMENSION_TEXTURE1DARRAY:
        return (SupportData.OutFormatSupport & D3D11_FORMAT_SUPPORT_TEXTURE1D) != 0;
    case D3D11_UAV_DIMENSION_TEXTURE2D:
    case D3D11_UAV_DIMENSION_TEXTURE2DARRAY:
        return (SupportData.OutFormatSupport & D3D11_FORMAT_SUPPORT_TEXTURE2D) != 0;
    case D3D11_UAV_DIMENSION_TEXTURE3D:
        return (SupportData.OutFormatSupport & D3D11_FORMAT_SUPPORT_TEXTURE3D) != 0;
    }
    return false;
}

void CUAVTypedTest::InitTestParameters()
{
    CUserValueSet<RESOURCE_TYPE> *pUAVResourceValueSet = new CUserValueSet<RESOURCE_TYPE>();
    pUAVResourceValueSet->AddValue(RESOURCE_RWTEXTURE1D);
    pUAVResourceValueSet->AddValue(RESOURCE_RWTEXTURE1D_ARRAY);
    pUAVResourceValueSet->AddValue(RESOURCE_RWTEXTURE2D);
    pUAVResourceValueSet->AddValue(RESOURCE_RWTEXTURE2D_ARRAY);
    pUAVResourceValueSet->AddValue(RESOURCE_RWTEXTURE3D);

    CTestCaseParameter<RESOURCE_TYPE> *pUAVResourceParam = AddParameter<RESOURCE_TYPE>(_T("UAVResourceType"), &m_UAVResourceType);
    testfactor::RFactor rfUAVRWBuf = AddParameterValue<RESOURCE_TYPE>(_T("UAVResourceType"), RESOURCE_RWBUFFER);
    testfactor::RFactor rfUAVTex1D = AddParameterValue<RESOURCE_TYPE>(_T("UAVResourceType"), RESOURCE_RWTEXTURE1D);
    testfactor::RFactor rfUAVTex1DArray = AddParameterValue<RESOURCE_TYPE>(_T("UAVResourceType"), RESOURCE_RWTEXTURE1D_ARRAY);
    testfactor::RFactor rfUAVTex2D = AddParameterValue<RESOURCE_TYPE>(_T("UAVResourceType"), RESOURCE_RWTEXTURE2D);
    testfactor::RFactor rfUAVTex2DArray = AddParameterValue<RESOURCE_TYPE>(_T("UAVResourceType"), RESOURCE_RWTEXTURE2D_ARRAY);
    testfactor::RFactor rfUAVTex3D = AddParameterValue<RESOURCE_TYPE>(_T("UAVResourceType"), RESOURCE_RWTEXTURE3D);

    CUserValueSet<DXGI_FORMAT> *pUAVWriteFormatValueSet = new CUserValueSet<DXGI_FORMAT>();
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R32G32B32A32_FLOAT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R32G32B32A32_UINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R32G32B32A32_SINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16G16B16A16_FLOAT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16G16B16A16_UNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16G16B16A16_UINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16G16B16A16_SNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16G16B16A16_SINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R32G32_FLOAT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R32G32_UINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R32G32_SINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R10G10B10A2_UNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R10G10B10A2_UINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R11G11B10_FLOAT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R8G8B8A8_UNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R8G8B8A8_UINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R8G8B8A8_SNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R8G8B8A8_SINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16G16_FLOAT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16G16_UNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16G16_UINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16G16_SNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16G16_SINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R32_FLOAT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R32_UINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R32_SINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R8G8_UNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R8G8_UINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R8G8_SNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R8G8_SINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16_FLOAT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16_UNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16_UINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16_SNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R16_SINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R8_UNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R8_UINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R8_SNORM);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_R8_SINT);
    pUAVWriteFormatValueSet->AddValue(DXGI_FORMAT_A8_UNORM);

    CUserValueSet<DXGI_FORMAT> *pUAVReadFormatValueSet = new CUserValueSet<DXGI_FORMAT>();
    pUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8G8B8A8_TYPELESS);
    pUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R10G10B10A2_TYPELESS);
    pUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16G16_TYPELESS);
    pUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R32_FLOAT);
    pUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R32_UINT);
    pUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R32_SINT);
    pUAVReadFormatValueSet->AddValue(DXGI_FORMAT_B8G8R8A8_TYPELESS);
    pUAVReadFormatValueSet->AddValue(DXGI_FORMAT_B8G8R8X8_TYPELESS);

    // Typed UAV Load Additional Formats
    CUserValueSet<DXGI_FORMAT> *pExtendedUAVReadFormatValueSet = new CUserValueSet<DXGI_FORMAT>();
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R32G32B32A32_FLOAT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R32G32B32A32_UINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R32G32B32A32_SINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16G16B16A16_FLOAT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16G16B16A16_UNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16G16B16A16_UINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16G16B16A16_SNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16G16B16A16_SINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R32G32_FLOAT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R32G32_UINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R32G32_SINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R10G10B10A2_UNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R10G10B10A2_UINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R11G11B10_FLOAT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8G8B8A8_UNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8G8B8A8_UINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8G8B8A8_SNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8G8B8A8_SINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16G16_FLOAT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16G16_UNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16G16_UINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16G16_SNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16G16_SINT);
    //pExtendedUAVReadFormatValueSet->AddValue( DXGI_FORMAT_R32_FLOAT          ); // already tested in pUAVReadFormatValueSet
    //pExtendedUAVReadFormatValueSet->AddValue( DXGI_FORMAT_R32_UINT           );
    //pExtendedUAVReadFormatValueSet->AddValue( DXGI_FORMAT_R32_SINT           );
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8G8_UNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8G8_UINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8G8_SNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8G8_SINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16_FLOAT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16_UNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16_UINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16_SNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R16_SINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8_UNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8_UINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8_SNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_R8_SINT);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_A8_UNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_B5G6R5_UNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_B5G5R5A1_UNORM);
    pExtendedUAVReadFormatValueSet->AddValue(DXGI_FORMAT_B4G4R4A4_UNORM);

    testfactor::RFactor rfUAVWriteFormat = AddParameter<DXGI_FORMAT>(_T("UAVFormat"), &m_UAVFormat, pUAVWriteFormatValueSet);
    testfactor::RFactor rfUAVReadFormat = AddParameterValueSet<DXGI_FORMAT>(_T("UAVFormat"), pUAVReadFormatValueSet);
    testfactor::RFactor rfExtendedUAVReadFormat = AddParameterValueSet<DXGI_FORMAT>(_T("UAVFormat"), pExtendedUAVReadFormatValueSet); // Typed UAV Load Additional Formats

    CTestCaseParameter<UAV_OPERATION> *pOpParam = AddParameter<UAV_OPERATION>(_T("UAVOperation"), &m_UAVOperation);
    testfactor::RFactor rfUAVRead = AddParameterValue<UAV_OPERATION>(_T("UAVOperation"), UAV_OP_READ);
    testfactor::RFactor rfUAVReadOOB = AddParameterValue<UAV_OPERATION>(_T("UAVOperation"), UAV_OP_READ_OOB);
    testfactor::RFactor rfUAVReadAF = AddParameterValue<UAV_OPERATION>(_T("UAVOperation"), UAV_OP_READ_AF); // Typed UAV Load Additional Formats
    testfactor::RFactor rfUAVWrite = AddParameterValue<UAV_OPERATION>(_T("UAVOperation"), UAV_OP_WRITE);

    // resource creation factors
    // -------------------------

    // mip slice
    CTestCaseParameter<UINT> *pMipSliceParam = AddParameter<UINT>(_T("MipSlice"), &m_uMipSlice);

    CUserValueSet<UINT> *pMipSliceValues = new CUserValueSet<UINT>();
    pMipSliceValues->AddValue(0);
    pMipSliceValues->AddValue(1);
    pMipSliceValues->AddValue(RESOURCE_MIP_LEVELS - 1);

    testfactor::RFactor rfMipSlice = AddParameterValueSet(_T("MipSlice"), pMipSliceValues);
    testfactor::RFactor rfMipSliceDef = AddParameterValue(_T("MipSlice"), (UINT)0);

    // first array slice
    CTestCaseParameter<UINT> *pArraySliceParam = AddParameter<UINT>(_T("FirstArraySlice"), &m_uFirstArraySlice);

    CUserValueSet<UINT> *pArraySliceValues = new CUserValueSet<UINT>();
    pArraySliceValues->AddValue(RESOURCE_ARRAY_SIZE - 1);
    pArraySliceValues->AddValue(RESOURCE_ARRAY_SIZE - 2);
    pArraySliceValues->AddValue(0);

    testfactor::RFactor rfArraySlice = AddParameterValueSet(_T("FirstArraySlice"), pArraySliceValues);
    testfactor::RFactor rfArraySliceDef = AddParameterValue(_T("FirstArraySlice"), (UINT)0);

    // view array size
    CTestCaseParameter<UINT> *pArraySizeParam = AddParameter<UINT>(_T("ViewArraySize"), &m_uArraySize);

    CUserValueSet<UINT> *pArraySizeValues = new CUserValueSet<UINT>();
    pArraySizeValues->AddValue(1);
    pArraySizeValues->AddValue(2);
    pArraySizeValues->AddValue(RESOURCE_ARRAY_SIZE);

    testfactor::RFactor rfArraySize = AddParameterValueSet(_T("ViewArraySize"), pArraySizeValues);
    testfactor::RFactor rfArraySizeDef = AddParameterValue(_T("ViewArraySize"), (UINT)1);

    // width
    CTestCaseParameter<UINT> *pWidthParam = AddParameter<UINT>(_T("Width"), &m_uWidth);

    CUserValueSet<UINT> *pWidthValues = new CUserValueSet<UINT>();
    pWidthValues->AddValue(RESOURCE_WIDTH);
    pWidthValues->AddValue(RESOURCE_WIDTH - 1);

    testfactor::RFactor rfWidth = AddParameterValueSet(_T("Width"), pWidthValues);
    testfactor::RFactor rfWidthDef = AddParameterValue(_T("Width"), (UINT)RESOURCE_WIDTH);

    // height
    CTestCaseParameter<UINT> *pHeightParam = AddParameter<UINT>(_T("Height"), &m_uHeight);

    CUserValueSet<UINT> *pHeightValues = new CUserValueSet<UINT>();
    pHeightValues->AddValue(RESOURCE_HEIGHT);
    pHeightValues->AddValue(RESOURCE_HEIGHT - 1);

    testfactor::RFactor rfHeight = AddParameterValueSet(_T("Height"), pHeightValues);
    testfactor::RFactor rfHeightDef = AddParameterValue(_T("Height"), (UINT)1);
    testfactor::RFactor rfHeightCube = AddParameterValue(_T("Height"), (UINT)RESOURCE_WIDTH);

    // depth
    CTestCaseParameter<UINT> *pDepthParam = AddParameter<UINT>(_T("Depth"), &m_uDepth);

    CUserValueSet<UINT> *pDepthValues = new CUserValueSet<UINT>();
    pDepthValues->AddValue(RESOURCE_DEPTH);
    pDepthValues->AddValue(1);

    testfactor::RFactor rfDepth = AddParameterValueSet(_T("Depth"), pDepthValues);
    testfactor::RFactor rfDepthDef = AddParameterValue(_T("Depth"), (UINT)1);

    // depth slice (for PS test)
    CTestCaseParameter<UINT> *pDepthSliceParam = AddParameter<UINT>(_T("DepthSlice"), &m_uDepthSlice);

    CUserValueSet<UINT> *pDepthSliceValues = new CUserValueSet<UINT>();
    pDepthSliceValues->AddValue(RESOURCE_DEPTH - 1);
    pDepthSliceValues->AddValue(0);

    testfactor::RFactor rfDepthSlice = AddParameterValueSet(_T("DepthSlice"), pDepthSliceValues);
    testfactor::RFactor rfDepthSliceDef = AddParameterValue(_T("DepthSlice"), (UINT)0);

    // test cubemaps
    CTestCaseParameter<bool> *pCubeParam = AddParameter<bool>(_T("TestCube"), &m_bTestCube);
    testfactor::RFactor rfTestCubeTrue = AddParameterValue<bool>(_T("TestCube"), true);
    testfactor::RFactor rfTestCubeFalse = AddParameterValue<bool>(_T("TestCube"), false);

    // testing both "UAV with RTV" and "UAV only" cases for pixel shader
    testfactor::RFactor rfIsUAVOnly = AddParameter<bool>(_T("IsUAVOnly"), &m_bUAVOnly, new CBoolValueSet());


    testfactor::RFactor rfResourceCreateDef = rfWidthDef * rfHeightDef  * rfDepthDef * rfDepthSliceDef * rfMipSliceDef * rfArraySliceDef * rfArraySizeDef;
    testfactor::RFactor rfResourceCreateCubeDef = rfWidthDef * rfHeightCube * rfDepthDef * rfDepthSliceDef * rfMipSliceDef * rfArraySliceDef * rfArraySizeDef;
    testfactor::RFactor rfTex1DCreate = rfWidth    * rfHeightDef  * rfDepthDef * rfDepthSliceDef * rfMipSlice    * rfArraySliceDef * rfArraySizeDef;
    testfactor::RFactor rfTex1DArrayCreate = rfWidth    * rfHeightDef  * rfDepthDef * rfDepthSliceDef * rfMipSlice    * (rfArraySlice % rfArraySize);
    testfactor::RFactor rfTex2DCreate = rfWidth    * rfHeight     * rfDepthDef * rfDepthSliceDef * rfMipSlice    * rfArraySliceDef * rfArraySizeDef;
    testfactor::RFactor rfTex2DArrayCreate = rfWidth    * rfHeight     * rfDepthDef * rfDepthSliceDef * rfMipSlice    * (rfArraySlice % rfArraySize);
    testfactor::RFactor rfTexCubeCreate = rfWidthDef * rfHeightCube * rfDepthDef * rfDepthSliceDef * rfMipSlice    * (rfArraySlice % rfArraySize);
    testfactor::RFactor rfTex3DCreate = rfWidth    * rfHeight     * (rfDepth % rfDepthSlice) * rfMipSlice * rfArraySliceDef * rfArraySizeDef;

    testfactor::RFactor rfRoot =
        rfUAVRead * rfUAVTex1D      * rfUAVReadFormat * rfResourceCreateDef     * rfTestCubeFalse +
        rfUAVRead * rfUAVTex1DArray * rfUAVReadFormat * rfResourceCreateDef     * rfTestCubeFalse +
        rfUAVRead * rfUAVTex2D      * rfUAVReadFormat * rfResourceCreateDef     * rfTestCubeFalse +
        rfUAVRead * rfUAVTex2DArray * rfUAVReadFormat * rfResourceCreateDef     * rfTestCubeFalse +
        rfUAVRead * rfUAVTex2DArray * rfUAVReadFormat * rfResourceCreateCubeDef * rfTestCubeTrue +
        rfUAVRead * rfUAVTex3D      * rfUAVReadFormat * rfResourceCreateDef     * rfTestCubeFalse +

        rfUAVRead * rfUAVTex1D      * rfUAVReadFormat * rfTex1DCreate      * rfTestCubeFalse +
        rfUAVRead * rfUAVTex1DArray * rfUAVReadFormat * rfTex1DArrayCreate * rfTestCubeFalse +
        rfUAVRead * rfUAVTex2D      * rfUAVReadFormat * rfTex2DCreate      * rfTestCubeFalse +
        rfUAVRead * rfUAVTex2DArray * rfUAVReadFormat * rfTex2DArrayCreate * rfTestCubeFalse +
        rfUAVRead * rfUAVTex2DArray * rfUAVReadFormat * rfTexCubeCreate    * rfTestCubeTrue +
        rfUAVRead * rfUAVTex3D      * rfUAVReadFormat * rfTex3DCreate      * rfTestCubeFalse +

        // Additional Typed UAV Load Formats (Threshold Feature)
        rfUAVReadAF * rfUAVRWBuf      * rfExtendedUAVReadFormat * rfResourceCreateDef     * rfTestCubeFalse +
        rfUAVReadAF * rfUAVTex1D      * rfExtendedUAVReadFormat * rfResourceCreateDef     * rfTestCubeFalse +
        //rfUAVReadAF * rfUAVTex1DArray * rfExtendedUAVReadFormat * rfResourceCreateDef     * rfTestCubeFalse + // no need to test arrays for Format conversion differences
        rfUAVReadAF * rfUAVTex2D      * rfExtendedUAVReadFormat * rfResourceCreateDef     * rfTestCubeFalse +
        //rfUAVReadAF * rfUAVTex2DArray * rfExtendedUAVReadFormat * rfResourceCreateDef     * rfTestCubeFalse +
        //rfUAVReadAF * rfUAVTex2DArray * rfExtendedUAVReadFormat * rfResourceCreateCubeDef * rfTestCubeTrue  +
        rfUAVReadAF * rfUAVTex3D      * rfExtendedUAVReadFormat * rfResourceCreateDef     * rfTestCubeFalse +

        rfUAVReadOOB * rfUAVTex1D * rfUAVReadFormat * rfResourceCreateDef * rfTestCubeFalse +
        rfUAVReadOOB * rfUAVTex1DArray * rfUAVReadFormat * rfResourceCreateDef * rfTestCubeFalse +
        rfUAVReadOOB * rfUAVTex2D * rfUAVReadFormat * rfResourceCreateDef * rfTestCubeFalse +
        rfUAVReadOOB * rfUAVTex2DArray * rfUAVReadFormat * rfResourceCreateDef * rfTestCubeFalse +
        rfUAVReadOOB * rfUAVTex2DArray * rfUAVReadFormat * rfResourceCreateCubeDef * rfTestCubeTrue +
        rfUAVReadOOB * rfUAVTex3D * rfUAVReadFormat * rfResourceCreateDef * rfTestCubeFalse +

        rfUAVWrite * rfUAVTex1D * rfUAVWriteFormat * rfResourceCreateDef * rfTestCubeFalse +
        rfUAVWrite * rfUAVTex1DArray * rfUAVWriteFormat * rfResourceCreateDef * rfTestCubeFalse +
        rfUAVWrite * rfUAVTex2D * rfUAVWriteFormat * rfResourceCreateDef * rfTestCubeFalse +
        rfUAVWrite * rfUAVTex2DArray * rfUAVWriteFormat * rfResourceCreateDef * rfTestCubeFalse +
        rfUAVWrite * rfUAVTex2DArray * rfUAVWriteFormat * rfResourceCreateCubeDef * rfTestCubeTrue +
        rfUAVWrite * rfUAVTex3D * rfUAVWriteFormat * rfResourceCreateDef * rfTestCubeFalse +

        rfUAVWrite * rfUAVTex1D * rfUAVWriteFormat * rfTex1DCreate * rfTestCubeFalse +
        rfUAVWrite * rfUAVTex1DArray * rfUAVWriteFormat * rfTex1DArrayCreate * rfTestCubeFalse +
        rfUAVWrite * rfUAVTex2D * rfUAVWriteFormat * rfTex2DCreate * rfTestCubeFalse +
        rfUAVWrite * rfUAVTex2DArray * rfUAVWriteFormat * rfTex2DArrayCreate * rfTestCubeFalse +
        rfUAVWrite * rfUAVTex2DArray * rfUAVWriteFormat * rfTexCubeCreate * rfTestCubeTrue +
        rfUAVWrite * rfUAVTex3D * rfUAVWriteFormat * rfTex3DCreate * rfTestCubeFalse;

    if (m_bPixelShaderTest)
        SetRootTestFactor(rfRoot % rfIsUAVOnly);
    else
        SetRootTestFactor(rfRoot);

    // Adding priority filters
    CFilterHandle filterFormat = FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_R32G32B32A32_FLOAT) ||
        FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_R16G16B16A16_UINT) ||
        FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_R32G32_FLOAT) ||
        FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_R10G10B10A2_UINT) ||
        FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_R11G11B10_FLOAT) ||
        FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_R8G8B8A8_SINT) ||
        FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_R16G16_SNORM) ||
        FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_R32_UINT) ||
        FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_R8G8_UNORM) ||
        FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_R16_FLOAT) ||
        FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_A8_UNORM) ||
        FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_B8G8R8X8_TYPELESS) ||
        FilterEqual<DXGI_FORMAT>(_T("UAVFormat"), DXGI_FORMAT_R16G16_TYPELESS);


    AddPriorityPatternFilter(filterFormat, WeightFactorLarge);
    DeclarePriorityLevelByPriorityValue(1, 1.5f);
}

TEST_RESULT CUAVTypedTest::CreateUAV()
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC    bufDesc;
    D3D11_TEXTURE1D_DESC tex1DDesc;
    D3D11_TEXTURE2D_DESC tex2DDesc;
    D3D11_TEXTURE3D_DESC tex3DDesc;
    bool bArray = false;

    switch (m_UAVResourceType)
    {
    case RESOURCE_RWBUFFER:
        bufDesc.ByteWidth = m_uWidth * GetBitsPerElement(m_UAVFormat);
        bufDesc.StructureByteStride = 0;
        bufDesc.Usage = D3D11_USAGE_DEFAULT;
        bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        bufDesc.CPUAccessFlags = 0;
        bufDesc.MiscFlags = 0;

        if (FAILED(hr = GetDevice()->CreateBuffer(&bufDesc, NULL, (ID3D11Buffer**)&m_pUAVResource)))
        {
            WriteToLog(_T("CUAVTypedTest::CreateUAV() failed to create buffer, hr=%s"), D3DHResultToString(hr).c_str());
            return RESULT_FAIL;
        }

        bufDesc.BindFlags = 0;
        bufDesc.Usage = D3D11_USAGE_STAGING;
        bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        if (FAILED(hr = GetDevice()->CreateBuffer(&bufDesc, NULL, (ID3D11Buffer**)&m_pUAVResourceStaging)))
        {
            WriteToLog(_T("CUAVTypedTest::CreateUAV() failed to create buffer, hr=%s"), D3DHResultToString(hr).c_str());
            return RESULT_FAIL;
        }

        break;
    case RESOURCE_RWTEXTURE1D_ARRAY:
        bArray = true;
    case RESOURCE_RWTEXTURE1D:
        tex1DDesc.Width = m_uWidth;
        tex1DDesc.MipLevels = RESOURCE_MIP_LEVELS;
        tex1DDesc.ArraySize = bArray ? RESOURCE_ARRAY_SIZE : 1;
        tex1DDesc.Format = m_UAVFormat;
        tex1DDesc.Usage = D3D11_USAGE_DEFAULT;
        tex1DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        tex1DDesc.CPUAccessFlags = 0;
        tex1DDesc.MiscFlags = 0;

        if (FAILED(hr = GetDevice()->CreateTexture1D(&tex1DDesc, NULL, (ID3D11Texture1D**)&m_pUAVResource)))
        {
            WriteToLog(_T("CUAVTypedTest::CreateUAV() failed to create texture, hr=%s"), D3DHResultToString(hr).c_str());
            return RESULT_FAIL;
        }

        tex1DDesc.BindFlags = 0;
        tex1DDesc.Usage = D3D11_USAGE_STAGING;
        tex1DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        if (FAILED(hr = GetDevice()->CreateTexture1D(&tex1DDesc, NULL, (ID3D11Texture1D**)&m_pUAVResourceStaging)))
        {
            WriteToLog(_T("CUAVTypedTest::CreateUAV() failed to create texture, hr=%s"), D3DHResultToString(hr).c_str());
            return RESULT_FAIL;
        }

        break;
    case RESOURCE_RWTEXTURE2D_ARRAY:
        bArray = true;
    case RESOURCE_RWTEXTURE2D:
        tex2DDesc.Width = m_uWidth;
        tex2DDesc.Height = m_uHeight;
        tex2DDesc.MipLevels = RESOURCE_MIP_LEVELS;
        tex2DDesc.ArraySize = bArray || m_bTestCube ? RESOURCE_ARRAY_SIZE : 1;
        tex2DDesc.Format = m_UAVFormat;
        tex2DDesc.SampleDesc.Quality = 0;
        tex2DDesc.SampleDesc.Count = 1;
        tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
        tex2DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        tex2DDesc.CPUAccessFlags = 0;
        tex2DDesc.MiscFlags = m_bTestCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

        if (FAILED(hr = GetFramework()->CreateTexture2DTiledOptional(&tex2DDesc, NULL, (ID3D11Texture2D**)&m_pUAVResource)))
        {
            WriteToLog(_T("CUAVTypedTest::CreateUAV() failed to create texture, hr=%s"), D3DHResultToString(hr).c_str());
            return RESULT_FAIL;
        }

        tex2DDesc.BindFlags = 0;
        tex2DDesc.Usage = D3D11_USAGE_STAGING;
        tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        if (FAILED(hr = GetFramework()->CreateTexture2DTiledOptional(&tex2DDesc, NULL, (ID3D11Texture2D**)&m_pUAVResourceStaging)))
        {
            WriteToLog(_T("CUAVTypedTest::CreateUAV() failed to create texture, hr=%s"), D3DHResultToString(hr).c_str());
            return RESULT_FAIL;
        }

        break;
    case RESOURCE_RWTEXTURE3D:
        tex3DDesc.Width = m_uWidth;
        tex3DDesc.Height = m_uHeight;
        tex3DDesc.Depth = m_uDepth;
        tex3DDesc.MipLevels = RESOURCE_MIP_LEVELS;
        tex3DDesc.Format = m_UAVFormat;
        tex3DDesc.Usage = D3D11_USAGE_DEFAULT;
        tex3DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        tex3DDesc.CPUAccessFlags = 0;
        tex3DDesc.MiscFlags = 0;

        if (FAILED(hr = GetDevice()->CreateTexture3D(&tex3DDesc, NULL, (ID3D11Texture3D**)&m_pUAVResource)))
        {
            WriteToLog(_T("CUAVTypedTest::CreateUAV() failed to create texture, hr=%s"), D3DHResultToString(hr).c_str());
            return RESULT_FAIL;
        }

        tex3DDesc.BindFlags = 0;
        tex3DDesc.Usage = D3D11_USAGE_STAGING;
        tex3DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        if (FAILED(hr = GetDevice()->CreateTexture3D(&tex3DDesc, NULL, (ID3D11Texture3D**)&m_pUAVResourceStaging)))
        {
            WriteToLog(_T("CUAVTypedTest::CreateUAV() failed to create texture, hr=%s"), D3DHResultToString(hr).c_str());
            return RESULT_FAIL;
        }

        break;
    default:
        WriteToLog(_T("CUAVTypedTest::CreateUAV() failed, invalid resource type."));
        return RESULT_FAIL;
    }

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    D3D11_UAV_DIMENSION uavDimension = GetUAVDimension(m_UAVResourceType);
    uavDesc.ViewDimension = uavDimension;

    if (m_UAVOperation == UAV_OP_WRITE || m_UAVOperation == UAV_OP_READ_AF)
    {
        uavDesc.Format = m_UAVFormat;
    }
    else // UAV casting DCR
    {
        tstring fmtType = GetFormatType(m_UAVFormat);
        if (fmtType == _T("float"))
            uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
        else if (fmtType == _T("uint"))
            uavDesc.Format = DXGI_FORMAT_R32_UINT;
        else if (fmtType == _T("int"))
            uavDesc.Format = DXGI_FORMAT_R32_SINT;
        else
            uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    }

    switch (uavDimension)
    {
    case D3D11_UAV_DIMENSION_BUFFER:
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.Flags = 0;
        uavDesc.Buffer.NumElements = m_uWidth;
    case D3D11_UAV_DIMENSION_TEXTURE1D:
        uavDesc.Texture1D.MipSlice = m_uMipSlice;
        break;
    case D3D11_UAV_DIMENSION_TEXTURE1DARRAY:
        uavDesc.Texture1DArray.MipSlice = m_uMipSlice;
        uavDesc.Texture1DArray.FirstArraySlice = m_uFirstArraySlice;
        uavDesc.Texture1DArray.ArraySize = m_uArraySize;
        break;
    case D3D11_UAV_DIMENSION_TEXTURE2D:
        uavDesc.Texture2D.MipSlice = m_uMipSlice;
        break;
    case D3D11_UAV_DIMENSION_TEXTURE2DARRAY:
        uavDesc.Texture2DArray.MipSlice = m_uMipSlice;
        uavDesc.Texture2DArray.FirstArraySlice = m_uFirstArraySlice;
        uavDesc.Texture2DArray.ArraySize = m_uArraySize;
        break;
    case D3D11_UAV_DIMENSION_TEXTURE3D:
        uavDesc.Texture3D.MipSlice = m_uMipSlice;
        uavDesc.Texture3D.FirstWSlice = 0;
        uavDesc.Texture3D.WSize = -1;
        break;
    default:
        WriteToLog(_T("CUAVTypedTest::CreateUAV() failed, invalid resource type."));
        return RESULT_FAIL;
    }

    if (FAILED(hr = GetDevice()->CreateUnorderedAccessView(m_pUAVResource, &uavDesc, &m_pUAV)))
    {
        WriteToLog(_T("CUAVTypedTest::CreateUAV() failed to create UAV, hr=%s"), D3DHResultToString(hr).c_str());
        return RESULT_FAIL;
    }

    return RESULT_PASS;
}

tstring CUAVTypedTest::GetFormatType(DXGI_FORMAT format)
{
    tstring typeStr;

    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        typeStr = "int";
        break;
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        typeStr = "uint";
        break;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    default:
        typeStr = "float";
        break;
    }

    return typeStr;
}

TEST_RESULT CUAVTypedTest::CreateShader()
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr;
    ID3D10Blob *pShaderBlob = NULL;
    ID3D10Blob *pErrorBlob = NULL;

    UINT depth = m_uDepth;
    UINT height = m_uHeight;
    UINT width = m_uWidth;

    width >>= m_uMipSlice;
    if (m_UAVResourceType >= RESOURCE_RWTEXTURE2D)
        height >>= m_uMipSlice;
    if (m_UAVResourceType == RESOURCE_RWTEXTURE3D)
        depth >>= m_uMipSlice;

    depth = max(depth, 1);

    if (m_UAVResourceType == RESOURCE_RWTEXTURE1D_ARRAY)
        height = RESOURCE_ARRAY_SIZE;
    else if (m_UAVResourceType == RESOURCE_RWTEXTURE2D_ARRAY)
        depth = RESOURCE_ARRAY_SIZE;

    tstring uavRes;
    tstring uavOp = m_UAVOperation == UAV_OP_READ || m_UAVOperation == UAV_OP_READ_AF ? _T("UAV_READ") :
                    m_UAVOperation == UAV_OP_READ_OOB                                 ? _T("UAV_READ_OOB") :
                  /*m_UAVOperation == UAV_OP_WRITE*/                                    _T("UAV_WRITE");
    assert(m_UAVOperation == UAV_OP_READ     ||
           m_UAVOperation == UAV_OP_READ_AF  ||
           m_UAVOperation == UAV_OP_READ_OOB ||
           m_UAVOperation == UAV_OP_WRITE);  // Adding a new UAV Operation requires a mapping to shader #define
    tstring compType = GetFormatType(m_UAVFormat);
    tstring threadCountX = ToString(width);
    tstring threadCountY = ToString(height);
    tstring threadCountZ = ToString(depth);

    if (m_UAVOperation == UAV_OP_READ_AF)   // Typed UAV Load Additional Formats feature testing expands the number of channels tested
        compType += _T("4");                // Declare 4-channel types in the shaders and let the hardware fill in 0s when necessary

    if (m_UAVResourceType == RESOURCE_RWBUFFER)
        uavRes = _T("UAV_RWBUF");
    else if (m_UAVResourceType == RESOURCE_RWTEXTURE1D)
        uavRes = _T("UAV_TEXTURE1D");
    else if (m_UAVResourceType == RESOURCE_RWTEXTURE1D_ARRAY)
        uavRes = _T("UAV_TEXTURE1D_ARRAY");
    else if (m_UAVResourceType == RESOURCE_RWTEXTURE2D)
        uavRes = _T("UAV_TEXTURE2D");
    else if (m_UAVResourceType == RESOURCE_RWTEXTURE2D_ARRAY)
        uavRes = _T("UAV_TEXTURE2D_ARRAY");
    else
        uavRes = _T("UAV_TEXTURE3D");

    D3D10_SHADER_MACRO macros[] =
    {
        { uavRes.c_str(), "1" },
        { uavOp.c_str(), "1" },
        { "TYPE", compType.c_str() },
        { "THREADCOUNTX", threadCountX.c_str() },
        { "THREADCOUNTY", threadCountY.c_str() },
        { "THREADCOUNTZ", threadCountZ.c_str() },
        { "INTEGER_FORMAT", compType.compare(0, 5, "float") == 0 ? "0" : "1" },
        { "SCALEFACTOR", m_ScaleFactorString.c_str() },
        NULL
    };

    hr = D3DX11CompileFromResource(
        NULL,
        _T("RC_UAV_TYPED_FX"),
        _T("UAVTyped.fx"),
        macros,
        NULL,
        _T("CSMain"),
        _T("cs_5_0"),
        D3D10_SHADER_DEBUG,
        D3D10_SHADER_DEBUG,
        NULL,
        &pShaderBlob,
        &pErrorBlob,
        NULL
        );

    if (FAILED(hr) || pShaderBlob == NULL)
    {
        WriteToLog(_T("CUAVTypedTest::CreateShader() - Failed to compile shader, hr=%s, Errors=%s"),
            D3DHResultToString(hr).c_str(), pErrorBlob ? pErrorBlob->GetBufferPointer() : "");
        tr = RESULT_FAIL;
        goto Done;
    }

    hr = GetDevice()->CreateComputeShader(
        pShaderBlob->GetBufferPointer(),
        pShaderBlob->GetBufferSize(),
        NULL,
        &m_pComputeShader
        );

    if (FAILED(hr))
    {
        WriteToLog(_T("CUAVTypedTest::CreateShader() - Failed to create shader, hr=%s"), D3DHResultToString(hr).c_str());
        tr = RESULT_FAIL;
    }

Done:
    SAFE_RELEASE(pShaderBlob);
    SAFE_RELEASE(pErrorBlob);

    return tr;
}

TEST_RESULT CUAVTypedTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    UINT ifaces = 1;
    UINT clearVal = 0xffffffff;
    GetEffectiveContext()->CSSetUnorderedAccessViews(0, 1, &m_pUAV, &ifaces);
    GetEffectiveContext()->CSSetShader(m_pComputeShader, NULL, 0);
    GetEffectiveContext()->ClearUnorderedAccessViewUint(m_pUAV, &clearVal);

    GetEffectiveContext()->Dispatch(1, 1, 1);

    if (FAILED(ExecuteEffectiveContext()))
    {
        WriteToLog(_T("CUAVTypedTest::ExecuteTestCase() - Failed to execute deferred context"));
        return RESULT_FAIL;
    }

    GetImmediateContext()->CopyResource(m_pUAVResourceStaging, m_pUAVResource);

    UINT numSubresources = RESOURCE_MIP_LEVELS;
    if (m_UAVResourceType == RESOURCE_RWTEXTURE1D_ARRAY || m_UAVResourceType == RESOURCE_RWTEXTURE2D_ARRAY)
        numSubresources = RESOURCE_MIP_LEVELS * RESOURCE_ARRAY_SIZE;
    if (m_UAVResourceType == RESOURCE_RWBUFFER)
        numSubresources = 1;

    HRESULT hr;
    for (UINT i = 0; i < numSubresources && tr == RESULT_PASS; ++i)
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubres;
        hr = GetImmediateContext()->Map(m_pUAVResourceStaging, i, D3D11_MAP_READ, 0, &mappedSubres);
        if (FAILED(hr))
        {
            WriteToLog(_T("CUAVTypedTest::ExecuteTestCase() - Map failed, hr=%s"), D3DHResultToString(hr).c_str());
            tr = RESULT_FAIL;
            break;
        }

        tr = CompareResults(mappedSubres.pData, i, mappedSubres.RowPitch, mappedSubres.DepthPitch);

        GetImmediateContext()->Unmap(m_pUAVResourceStaging, i);
    }

    return tr;
}

TEST_RESULT CUAVTypedTest::CompareResults(void *pResult, UINT subresource, UINT rowPitch, UINT depthPitch)
{
    UINT depth = m_uDepth;
    UINT height = m_uHeight;
    UINT width = m_uWidth;
    UINT elementWidth = max(1, GetBitsPerElement(m_UAVFormat) / 8);
    UINT id = 0;

    UINT testedSubres = (m_uFirstArraySlice * RESOURCE_MIP_LEVELS) + m_uMipSlice;
    UINT mipSlice = subresource % RESOURCE_MIP_LEVELS;
    width >>= mipSlice;
    if (m_UAVResourceType >= RESOURCE_RWTEXTURE2D)
        height >>= mipSlice;
    if (m_UAVResourceType == RESOURCE_RWTEXTURE3D)
        depth >>= mipSlice;

    depth = max(depth, 1);
    UINT depthSlice = m_uDepthSlice;
    if (m_uDepthSlice != 0)
        depthSlice = depth - 1;

    tstring typeStr = GetFormatType(m_UAVFormat);
    DXGI_FORMAT viewFmt;
    if (typeStr == "int")
        viewFmt = DXGI_FORMAT_R32_SINT;
    else if (typeStr == "uint")
        viewFmt = DXGI_FORMAT_R32_UINT;
    else
        viewFmt = DXGI_FORMAT_R32_FLOAT;

    BYTE *pCurrentResult = (BYTE*)pResult;
    for (UINT z = 0; z < depth; ++z)
    {
        if (m_bPixelShaderTest && m_UAVResourceType == RESOURCE_RWTEXTURE3D)
        {
            if (z != depthSlice)
                continue;
            else
                id = 0;
        }

        for (UINT y = 0; y < height; ++y)
        {
            for (UINT x = 0; x < width; ++x)
            {
                BYTE *pElement = pCurrentResult + x * elementWidth;
                FLOAT_COLOR color;
                if (!CFormatHelper::ConvertPixels(CONV_FROM_FORMAT, m_UAVOperation == UAV_OP_WRITE || m_UAVOperation == UAV_OP_READ_AF ? m_UAVFormat : viewFmt, &color, 1, 1, pElement))
                {
                    WriteToLog(_T("CUAVTypedTest::CompareResults - CFormatHelper::ConvertPixels() failed"));
                    return RESULT_FAIL;
                }

                if (subresource != testedSubres)
                {
                    //if( *((UINT32*)pElement) != 0xffffffff )
                    //{
                    //	WriteToLog(_T("Test case failed: Subresource %d, location (%d,%d,%d) should not have been modified, expected = 0xffffffff, actual result = 0x%x"), subresource, x, y, z, *((UINT32*)pElement));
                    //	return RESULT_FAIL;
                    //}
                }
                else
                {
                    float div = typeStr == _T("float") ? m_ScaleFactor : 1.0f;
                    if (m_UAVOperation == UAV_OP_READ)
                    {
                        float expected = (float)((id + 1) % 128) / div;
                        if (abs(expected - color.r) > EPSILON)
                        {
                            WriteToLog(_T("Test case failed: Read for subresource %d, location (%d,%d,%d) should have returned %f, actual result = %f"), subresource, x, y, z, expected, color.r);
                            return RESULT_FAIL;
                        }
                    }
                    else if (m_UAVOperation == UAV_OP_READ_AF)
                    {
                        // Correct for ConvertPixels setting A-channel to 1.0 when it doesn't exist
                        if (CD3D10FormatHelper_Test::GetBitsPerComponent(m_UAVFormat, 3) == 0)
                            color.a = 0;

                        // Build expected color that:
                        // - skips 0-bit components (e.g. RG&B in A8_UNORM)
                        // - accounts for UNORM and SNORM formats saturating the index value when written
                        // - accounts for saturation that occurs when you assign values greater than the bit-depth can support
                        // - accounts for quantization that occurs in small bit-depth UNORMs
                        FLOAT_COLOR expectedColor = { 0 };
                        for (UINT i = 0; i < 4; ++i)
                        {
                            UINT bpc = CD3D10FormatHelper_Test::GetBitsPerComponent(m_UAVFormat, i);
                            if (bpc == 0)
                            {
                                expectedColor.comp[i] = 0;
                            }
                            else
                            {
                                // saturated index
                                if (CD3D10FormatHelper_Test::GetFormatComponentInterpretation(m_UAVFormat, i) == D3D10FCI_UNORM ||
                                    CD3D10FormatHelper_Test::GetFormatComponentInterpretation(m_UAVFormat, i) == D3D10FCI_SNORM)
                                    expectedColor.comp[i] = (float)((min(id, 1) + 1) % int(m_ScaleFactor)) / div;
                                else
                                    expectedColor.comp[i] = (float)((id + 1) % int(m_ScaleFactor)) / div;
                                // saturated final value
                                UINT64 saturatedValue = (UINT64(1) << bpc) - 1;
                                expectedColor.comp[i] = min(expectedColor.comp[i], static_cast<float>(saturatedValue));
                                // quantization
                                if (CD3D10FormatHelper_Test::GetFormatComponentInterpretation(m_UAVFormat, i) == D3D10FCI_UNORM)
                                {
                                    assert(expectedColor.comp[i] <= 1.0);
                                    int quantizedVal = int((expectedColor.comp[i] * saturatedValue) + 0.5); //Clamp to nearest
                                    float nearest = float(quantizedVal) / saturatedValue;
                                    expectedColor.comp[i] = nearest;
                                }
                            }
                        }

                        if (abs(expectedColor.r - color.r) > EPSILON ||
                            abs(expectedColor.g - color.g) > EPSILON ||
                            abs(expectedColor.b - color.b) > EPSILON ||
                            abs(expectedColor.a - color.a) > EPSILON)
                        {
                            WriteToLog(_T("Test case failed: Read for subresource %d, location (%d,%d,%d) should have returned (%f,%f,%f,%f), actual result = (%f,%f,%f,%f)"), subresource, x, y, z, expectedColor.r, expectedColor.g, expectedColor.b, expectedColor.a, color.r, color.g, color.b, color.a);
                            return RESULT_FAIL;
                        }
                    }
                    else if (m_UAVOperation == UAV_OP_READ_OOB)
                    {
                        if (*((UINT32*)pElement) != 0x0)
                        {
                            WriteToLog(_T("Test case failed: Out of bounds read for subresource %d, location (%d,%d,%d) should have returned 0, actual result = 0x%x"), subresource, x, y, z, *((UINT32*)pElement));
                            return RESULT_FAIL;
                        }
                    }
                    else if (m_UAVOperation == UAV_OP_WRITE)
                    {
                        if (m_UAVFormat == DXGI_FORMAT_A8_UNORM)
                            color.r = color.a;
                        float expected = (float)(id % int(m_ScaleFactor)) / div;
                        if (abs(expected - color.r) > EPSILON)
                        {
                            WriteToLog(_T("Test case failed: Write to subresource %d, location (%d,%d,%d) should have been %f, actual result = %f"), subresource, x, y, z, expected, color.r);
                            return RESULT_FAIL;
                        }
                    }

                    id++;
                }
            }

            pCurrentResult += rowPitch;
        }

        pCurrentResult = (BYTE*)pResult + (z + 1) * depthPitch;
    }

    return RESULT_PASS;
}

//--------------------------------------------------------------------------------

TEST_RESULT CUAVTypedPixelShaderTest::CreateShader()
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr;

    ID3D10Blob *pPixelShaderBlob = NULL;
    ID3D10Blob *pPixelErrorBlob = NULL;
    ID3D10Blob *pVertexShaderBlob = NULL;
    ID3D10Blob *pVertexErrorBlob = NULL;

    // Create input buffers
    float vertices[6][3];

    vertices[0][0] = -1.0f;	vertices[0][1] = -1.0f;	vertices[0][2] = 0.5f;
    vertices[1][0] = -1.0f;	vertices[1][1] = 1.0f;	vertices[1][2] = 0.5f;
    vertices[2][0] = 1.0f;	vertices[2][1] = -1.0f;	vertices[2][2] = 0.5f;
    vertices[3][0] = 1.0f;	vertices[3][1] = -1.0f;	vertices[3][2] = 0.5f;
    vertices[4][0] = -1.0f;	vertices[4][1] = 1.0f;	vertices[4][2] = 0.5f;
    vertices[5][0] = 1.0f;	vertices[5][1] = 1.0f;	vertices[5][2] = 0.5f;

    D3D11_BUFFER_DESC bd;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(float) * 6 * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = vertices;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;
    hr = GetFramework()->CreateBufferTiledOptional(&bd, &initData, &m_pVertexBuffer);
    if (FAILED(hr))
    {
        WriteToLog(_T("CUAVTypedPixelShaderTest::ExecuteTestCase() - CreateBuffer failed, hr=%s"), D3DHResultToString(hr).c_str());
        return RESULT_FAIL;
    }

    tstring uavRes;
    tstring uavOp = m_UAVOperation == UAV_OP_READ || m_UAVOperation == UAV_OP_READ_AF ? _T("UAV_READ") :
                    m_UAVOperation == UAV_OP_READ_OOB                                 ? _T("UAV_READ_OOB") :
                  /*m_UAVOperation == UAV_OP_WRITE*/                                    _T("UAV_WRITE");
    assert(m_UAVOperation == UAV_OP_READ     ||
           m_UAVOperation == UAV_OP_READ_AF  ||
           m_UAVOperation == UAV_OP_READ_OOB ||
           m_UAVOperation == UAV_OP_WRITE);  // Adding a new UAV Operation requires a mapping to shader #define
    tstring compType = GetFormatType(m_UAVFormat);

    if (m_UAVOperation == UAV_OP_READ_AF)   // Typed UAV Load Additional Formats feature testing expands the number of channels tested
        compType += _T("4");                // Declare 4-channel types in the shaders and let the hardware fill in 0s when necessary

    if (m_UAVResourceType == RESOURCE_RWBUFFER)
        uavRes = _T("UAV_RWBUF");
    else if (m_UAVResourceType == RESOURCE_RWTEXTURE1D)
        uavRes = _T("UAV_TEXTURE1D");
    else if (m_UAVResourceType == RESOURCE_RWTEXTURE1D_ARRAY)
        uavRes = _T("UAV_TEXTURE1D_ARRAY");
    else if (m_UAVResourceType == RESOURCE_RWTEXTURE2D)
        uavRes = _T("UAV_TEXTURE2D");
    else if (m_UAVResourceType == RESOURCE_RWTEXTURE2D_ARRAY)
        uavRes = _T("UAV_TEXTURE2D_ARRAY");
    else
        uavRes = _T("UAV_TEXTURE3D");

    UINT width = m_uWidth;
    width >>= m_uMipSlice;
    UINT depth = m_uDepth;
    depth >>= m_uMipSlice;
    depth = max(depth, 1);
    UINT depthSlice = 0;
    if (m_UAVResourceType == RESOURCE_RWTEXTURE3D)
    {
        depthSlice = m_uDepthSlice;
        if (depthSlice > 0)
            depthSlice = depth - 1;
    }

    tstring widthStr = ToString(width);
    tstring depthStr = ToString((float)depthSlice / (float)depth);

    tstring isUAVOnly;
    if (m_bUAVOnly)
        isUAVOnly = _T("1");
    else
        isUAVOnly = _T("0");

    D3D10_SHADER_MACRO macros[] =
    {
        { uavRes.c_str(), "1" },
        { uavOp.c_str(), "1" },
        { "VIEWPORT_WIDTH", widthStr.c_str() },
        { "TYPE", compType.c_str() },
        { "INTEGER_FORMAT", compType.compare(0, 5, "float") == 0 ? "0" : "1" },
        { "DEPTH_SLICE", depthStr.c_str() },
        { "PIXEL_SHADER_UAV_ONLY", isUAVOnly.c_str() },
        { "SCALEFACTOR", m_ScaleFactorString.c_str() },
        NULL
    };

    // Create passthrough VS
    hr = D3DX11CompileFromResource(
        NULL,
        _T("RC_UAV_TYPED_FX"),
        _T("UAVTyped.fx"),
        NULL,
        NULL,
        _T("VSMain"),
        _T("vs_5_0"),
        D3D10_SHADER_DEBUG,
        D3D10_SHADER_DEBUG,
        NULL,
        &pVertexShaderBlob,
        &pVertexErrorBlob,
        NULL
        );

    if (FAILED(hr) || pVertexShaderBlob == NULL)
    {
        WriteToLog(_T("CUAVTypedPixelShaderTest::CreateShader() - Failed to compile vertex shader, hr=%s, Errors=%s"),
            D3DHResultToString(hr).c_str(), pVertexErrorBlob ? pVertexErrorBlob->GetBufferPointer() : "");
        tr = RESULT_FAIL;
        goto Done;
    }

    hr = GetDevice()->CreateVertexShader(
        pVertexShaderBlob->GetBufferPointer(),
        pVertexShaderBlob->GetBufferSize(),
        NULL,
        &m_pVertexShader
        );

    if (FAILED(hr))
    {
        WriteToLog(_T("CUAVTypedPixelShaderTest::CreateShader() - Failed to create vertex shader, hr=%s"), D3DHResultToString(hr).c_str());
        tr = RESULT_FAIL;
        goto Done;
    }


    //Create input layout

    D3D11_INPUT_ELEMENT_DESC pIEDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = GetDevice()->CreateInputLayout(
        pIEDesc,
        1,
        pVertexShaderBlob->GetBufferPointer(),
        pVertexShaderBlob->GetBufferSize(),
        &m_pInputLayout);

    if (FAILED(hr))
    {
        WriteToLog(_T("CUAVTypedPixelShaderTest::CreateShader() - CreateInputLayout() failed"));
        tr = RESULT_FAIL;
        goto Done;
    }

    // Create pixel shader

    hr = D3DX11CompileFromResource(
        NULL,
        _T("RC_UAV_TYPED_FX"),
        _T("UAVTyped.fx"),
        macros,
        NULL,
        _T("PSMain"),
        _T("ps_5_0"),
        D3D10_SHADER_DEBUG,
        D3D10_SHADER_DEBUG,
        NULL,
        &pPixelShaderBlob,
        &pPixelErrorBlob,
        NULL
        );

    if (FAILED(hr) || pPixelShaderBlob == NULL)
    {
        WriteToLog(_T("CUAVTypedPixelShaderTest::CreateShader() - Failed to compile pixel shader, hr=%s, Errors=%s"),
            D3DHResultToString(hr).c_str(), pPixelErrorBlob ? pPixelErrorBlob->GetBufferPointer() : "");
        tr = RESULT_FAIL;
        goto Done;
    }

    hr = GetDevice()->CreatePixelShader(
        pPixelShaderBlob->GetBufferPointer(),
        pPixelShaderBlob->GetBufferSize(),
        NULL,
        &m_pPixelShader
        );

    if (FAILED(hr))
    {
        WriteToLog(_T("CUAVTypedPixelShaderTest::CreateShader() - Failed to create pixel shader, hr=%s"), D3DHResultToString(hr).c_str());
        tr = RESULT_FAIL;
    }

Done:
    SAFE_RELEASE(pVertexShaderBlob);
    SAFE_RELEASE(pVertexErrorBlob);
    SAFE_RELEASE(pPixelShaderBlob);
    SAFE_RELEASE(pPixelErrorBlob);

    return tr;
}

TEST_RESULT CUAVTypedPixelShaderTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    // Set vertex buffer
    UINT stride = sizeof(float) * 3;
    UINT offset = 0;
    GetEffectiveContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
    GetEffectiveContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    GetEffectiveContext()->IASetInputLayout(m_pInputLayout);

    // Setup the default viewport
    UINT depth = m_uDepth;
    UINT height = m_uHeight;
    UINT width = m_uWidth;

    width >>= m_uMipSlice;
    if (m_UAVResourceType >= RESOURCE_RWTEXTURE2D)
        height >>= m_uMipSlice;
    if (m_UAVResourceType == RESOURCE_RWTEXTURE3D)
        depth >>= m_uMipSlice;

    depth = max(depth, 1);

    if (m_UAVResourceType == RESOURCE_RWTEXTURE1D_ARRAY)
        height = RESOURCE_ARRAY_SIZE;
    else if (m_UAVResourceType == RESOURCE_RWTEXTURE2D_ARRAY)
        depth = RESOURCE_ARRAY_SIZE;


    D3D11_VIEWPORT vp;
    vp.Height = (float)height;
    vp.Width = (float)width;
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    GetEffectiveContext()->RSSetViewports(1, &vp);

    // Setup the UAV
    UINT ifaces = 1;
    if (m_bUAVOnly)
    {
        GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews(
            0,
            NULL,
            NULL,
            0,
            1,
            &m_pUAV,
            &ifaces);
    }
    else
    {
        // create a dummy rendertarget
        if (CComputeTest::SetupRTV(width, height) != RESULT_PASS)
        {
            WriteToLog(_T("CUAVTypedPixelShaderTest::ExecuteTestCase: CComputeTest::SetupRTV() failed "));
            return RESULT_FAIL;
        }
        GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews(
            1,
            &m_pRTView,
            NULL,
            1,
            1,
            &m_pUAV,
            &ifaces);
    }

    UINT clearVal = 0xffffffff;
    GetEffectiveContext()->ClearUnorderedAccessViewUint(m_pUAV, &clearVal);

    GetEffectiveContext()->VSSetShader(m_pVertexShader, NULL, 0);
    GetEffectiveContext()->PSSetShader(m_pPixelShader, NULL, 0);

    GetEffectiveContext()->Draw(6, 0);

    if (FAILED(ExecuteEffectiveContext()))
    {
        WriteToLog(_T("CUAVTypedTest::ExecuteTestCase() - Failed to execute deferred context"));
        return RESULT_FAIL;
    }

    GetImmediateContext()->CopyResource(m_pUAVResourceStaging, m_pUAVResource);

    UINT numSubresources = RESOURCE_MIP_LEVELS;
    if (m_UAVResourceType == RESOURCE_RWTEXTURE1D_ARRAY || m_UAVResourceType == RESOURCE_RWTEXTURE2D_ARRAY)
        numSubresources = RESOURCE_MIP_LEVELS * RESOURCE_ARRAY_SIZE;
    if (m_UAVResourceType == RESOURCE_RWBUFFER)
        numSubresources = 1;

    HRESULT hr;
    for (UINT i = 0; i < numSubresources && tr == RESULT_PASS; ++i)
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubres;
        hr = GetImmediateContext()->Map(m_pUAVResourceStaging, i, D3D11_MAP_READ, 0, &mappedSubres);
        if (FAILED(hr))
        {
            WriteToLog(_T("CUAVTypedPixelShaderTest::ExecuteTestCase() - Map failed, hr=%s"), D3DHResultToString(hr).c_str());
            tr = RESULT_FAIL;
            break;
        }

        tr = CompareResults(mappedSubres.pData, i, mappedSubres.RowPitch, mappedSubres.DepthPitch);

        GetImmediateContext()->Unmap(m_pUAVResourceStaging, i);
    }

    return tr;
}