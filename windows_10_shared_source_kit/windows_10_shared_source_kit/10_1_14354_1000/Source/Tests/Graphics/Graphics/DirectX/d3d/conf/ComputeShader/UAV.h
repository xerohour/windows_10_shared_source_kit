//////////////////////////////////////////////////////////////////////////
//  WGFCompute.h
//  created: 2009/03/23
//
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "WGFCompute.h"
#include "FormatHelper.h"
#include "FormatDesc.hpp"

using namespace std;

#define DEFAULT_BUFFER_SIZE 16 * 16 * 4
#define DEFAULT_BUFFER_SIZE_STRING "16 * 16 * 4"
#define DEFAULT_BUFFER_STRIDE 4
#define DEFAULT_BUFFER_ELEMENT_WIDTH 4

#define UAV_BUFFER_RW_TEST_BUFFER_SIZE 16 * 18 * 4
#define UAV_BUFFER_RW_TEST_BUFFER_SIZE_STRING "16 * 18 * 4"

#define UAV_BUFFER_SRC_TEST_BLANK_BUFFER_BYTE_COUNT 400
#define UAV_BUFFER_SRC_TEST_PATTERN_FLOAT 1.0, 0.25, 0.75, 1.0
#define UAV_BUFFER_SRC_TEST_PATTERN_FLOAT_STRING "1.0, 0.25, 0.75, 1.0"
#define UAV_BUFFER_SRC_TEST_PATTERN_SINT -10, 25, -75, 100 
#define UAV_BUFFER_SRC_TEST_PATTERN_SINT_STRING "-10, 25, -75, 100" 
#define UAV_BUFFER_SRC_TEST_PATTERN_UINT 10, 25, 75, 100 
#define UAV_BUFFER_SRC_TEST_PATTERN_UINT_STRING "10, 25, 75, 100" 
#define UAV_BUFFER_SRC_TEST_PATTERN_MIXGROUP true, -12.5f, 15.3f, 13.5f, 12355.2f, true, false, true, true, -40, 1000, 1, 22, 1, 2, 3
#define UAV_BUFFER_SRC_TEST_PATTERN_MIXGROUP_STRING "true, -12.5, 15.3, 13.5, 12355.2, true, false, true, true, -40, 1000, 1, 22, 1, 2, 3"
#define UAV_BUFFER_SRC_TEST_PATTERN_UINTGROUP 10, 25, 75, 100 
#define UAV_BUFFER_SRC_TEST_PATTERN_UINTGROUP_STRING "10, 25, 75, 100"
#define UAV_BUFFER_SRC_TEST_PATTERN_SINTGROUP 12230, -12325
#define UAV_BUFFER_SRC_TEST_PATTERN_SINTGROUP_STRING "12230, -12325"
#define UAV_BUFFER_SRC_TEST_PATTERN_FLOATGROUP 32.5
#define UAV_BUFFER_SRC_TEST_PATTERN_FLOATGROUP_STRING "32.5"

#define UAV_BUFFER_TYPED_TEST_NUMBER_OF_ELEMENT_PER_THREAD 100
#define UAV_BUFFER_TYPED_TEST_NUMBER_OF_ELEMENT_PER_THREAD_STRING "100"
#define UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_1_UNORM 0.5
#define UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_1_SNORM -0.5
#define UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_1_FLOAT 2.0
#define UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_1_UINT 1
#define UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_1_SINT -1
#define UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_2_UNORM 1.0
#define UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_2_SNORM 1.0
#define UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_2_FLOAT 1.0
#define UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_2_UINT 2
#define UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_2_SINT 2
#define UAV_BUFFER_TYPED_TEST_FLOAT_TOLERANCE 0.01

#define RESOURCE_ARRAY_SIZE 6
#define RESOURCE_MIP_LEVELS 3
#define RESOURCE_WIDTH      8
#define RESOURCE_HEIGHT     8
#define RESOURCE_DEPTH      RESOURCE_ARRAY_SIZE

#define MAX_RESULT_SIZE     RESOURCE_WIDTH * RESOURCE_HEIGHT * RESOURCE_ARRAY_SIZE * RESOURCE_MIP_LEVELS * 16    

enum RESOURCE_TYPE
{
    RESOURCE_BYTEADDRESSBUFFER,
    RESOURCE_STRUCTUREDBUFFER,
    RESOURCE_STRUCTUREDBUFFER_1FLOAT,
    RESOURCE_STRUCTUREDBUFFER_4UINT,
    RESOURCE_STRUCTUREDBUFFER_2SINT,
    RESOURCE_STRUCTUREDBUFFER_MIX,
    RESOURCE_RWBUFFER,
    RESOURCE_RWBUFFER_SINT,
    RESOURCE_RWBUFFER_UINT,
    RESOURCE_RWBUFFER_UINT2,
    RESOURCE_RWBUFFER_FLOAT,
    RESOURCE_RWBUFFER_FLOAT4,
    RESOURCE_RWBYTEADDRESSBUFFER,
    RESOURCE_RWSTRUCTUREDBUFFER,
    RESOURCE_RWSTRUCTUREDBUFFER_1FLOAT,
    RESOURCE_RWSTRUCTUREDBUFFER_4UINT,
    RESOURCE_RWSTRUCTUREDBUFFER_2SINT,
    RESOURCE_RWSTRUCTUREDBUFFER_MIX,
    RESOURCE_RWTEXTURE1D,
    RESOURCE_RWTEXTURE1D_ARRAY,
    RESOURCE_RWTEXTURE2D,
    RESOURCE_RWTEXTURE2D_ARRAY,
    RESOURCE_RWTEXTURE3D
};

DECLARE_NAMED_VALUES(RESOURCE_TYPE);

enum UAV_OPERATION
{
    UAV_OP_READ,
    UAV_OP_READ_OOB,
    UAV_OP_WRITE,
    UAV_OP_READ_AF, // Typed UAV Load Additional Formats (Threshold Feature)
};

DECLARE_NAMED_VALUES(UAV_OPERATION);

//------------------------------------------------------------------------------------------------

struct UIntGroup
{
    UINT a;
    UINT b;
    UINT c;
    UINT d;

    UIntGroup(UINT i1, UINT i2, UINT i3, UINT i4) : a(i1), b(i2), c(i3), d(i4) {}
};

struct SIntGroup
{
    int a;
    int b;

    SIntGroup(UINT i1, UINT i2) : a(i1), b(i2) {}

};

struct FloatGroup
{
    float a;

    FloatGroup(float f) : a(f) {}
};

struct MixGroup
{
    int b1; // should be a bool but compiler making it < 4 byte
    float f1;
    float f2;
    float f3;
    float f4;
    int b2; // should be a bool but compiler making it < 4 byte
    int b3; // should be a bool but compiler making it < 4 byte
    int b4; // should be a bool but compiler making it < 4 byte
    int b5; // should be a bool but compiler making it < 4 byte
    int i1;
    UINT u1;
    UINT u2;
    int i2;
    int i3;
    int i4;
    int i5;

    MixGroup() {}

    MixGroup(bool b1Val, float f1Val, float f2Val, float f3Val, float f4Val, bool b2Val, bool b3Val,
        bool b4Val, bool b5Val, int i1Val, UINT u1Val, UINT u2Val, int i2Val, int i3Val, int i4Val, int i5Val)
        : b1((int)b1Val), f1(f1Val), f2(f2Val), f3(f3Val), f4(f4Val), b2((int)b2Val),
        b3((int)b3Val), b4((int)b4Val), b5((int)b5Val), i1(i1Val), u1(u1Val), u2(u2Val),
        i2(i2Val), i3(i3Val), i4(i4Val), i5(i5Val) {}
};

//------------------------------------------------------------------------------------------------

class CUAVTest : public CComputeTest
{
protected:
    CUAVTest() :
        m_pSRV(NULL),
        m_pUAV(NULL),
        m_pSrcBuffer(NULL),
        m_pUAVResource(NULL),
        m_pUAVResourceStaging(NULL),
        m_pComputeShader(NULL),
        m_pInputLayout(NULL),
        m_pPixelShader(NULL),
        m_pVertexShader(NULL),
        m_pVertexBuffer(NULL),

        m_SrcResourceType(RESOURCE_BYTEADDRESSBUFFER),
        m_uBufferSize(DEFAULT_BUFFER_SIZE),
        m_uBufferStride(DEFAULT_BUFFER_STRIDE),
        m_uBufferElementWidth(DEFAULT_BUFFER_ELEMENT_WIDTH)
    {
    }

    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase() { return RESULT_PASS; }
    virtual void Cleanup() { }
    virtual void CleanupTestCase();

    virtual TEST_RESULT CreateShaderResource();
    virtual void CreateInitialData() = 0;
    virtual TEST_RESULT CreateUAV() = 0;
    virtual TEST_RESULT CreateShader() = 0;
    D3D11_SRV_DIMENSION GetSRVDimension(RESOURCE_TYPE resType);
    D3D11_UAV_DIMENSION GetUAVDimension(RESOURCE_TYPE resType);

    RESOURCE_TYPE             m_SrcResourceType;
    RESOURCE_TYPE             m_UAVResourceType;
    UAV_OPERATION             m_UAVOperation;
    UINT                      m_uBufferSize;
    UINT                      m_uBufferStride;
    UINT                      m_uBufferNumElements;
    UINT                      m_uBufferElementWidth;
    DXGI_FORMAT               m_srcFormat;
    DXGI_FORMAT               m_UAVFormat;

    UINT                      m_threadCountX;
    UINT                      m_threadCountY;
    UINT                      m_threadCountZ;

    ID3D11ShaderResourceView  *m_pSRV;
    ID3D11UnorderedAccessView *m_pUAV;
    ID3D11Buffer              *m_pSrcBuffer;
    ID3D11Resource            *m_pUAVResource;
    ID3D11Resource            *m_pUAVResourceStaging;
    ID3D11ComputeShader       *m_pComputeShader;
    ID3D11InputLayout         *m_pInputLayout;
    ID3D11PixelShader         *m_pPixelShader;
    ID3D11VertexShader        *m_pVertexShader;
    ID3D11Buffer              *m_pVertexBuffer;

    BYTE                      *m_pInitialData[MAX_RESULT_SIZE];
    BYTE                      *m_pExpectedResults[MAX_RESULT_SIZE];
    BYTE                      *m_pComputedResults[MAX_RESULT_SIZE];
};

class CUAVBufferTest : public CUAVTest
{

protected:
    virtual bool EqualWithinTolerance(float r, float g, float b, float a, FLOAT_COLOR f, DXGI_FORMAT format);
    virtual UINT GetByteSizePerColor(DXGI_FORMAT format);
    virtual bool HasAlphaChannel(DXGI_FORMAT format);
    virtual bool HasRedChannel(DXGI_FORMAT format);
    virtual bool HasGreenChannel(DXGI_FORMAT format);
    virtual bool HasBlueChannel(DXGI_FORMAT format);

    // for CS test
    UINT m_dispatchCountX;
    UINT m_dispatchCountY;
    UINT m_dispatchCountZ;

    // for PS test
    UINT m_viewportWidth;
    UINT m_viewportHeight;

};

class CUAVBufferSrcTest : public CUAVBufferTest
{
public:
    CUAVBufferSrcTest() :
        m_pSrcBufferBlank(NULL),
        m_pSRVBlank(NULL)
    {
    }

    virtual void InitTestParameters() = 0;
    virtual void CreateInitialData();
    virtual TEST_RESULT CreateUAV();

protected:
    virtual TEST_RESULT CreateShaderResource();
    virtual TEST_RESULT CompareResults(void *pResult);
    void CleanupTestCase();

    UINT m_uTotalThreadCount;
    ID3D11ShaderResourceView *m_pSRVBlank;
    ID3D11Buffer *m_pSrcBufferBlank;

};

class CUAVBufferSrcCSTest : public CUAVBufferSrcTest
{
public:
    void InitTestParameters();
    void CreateInitialData();
    TEST_RESULT CreateShader();
    TEST_RESULT ExecuteTestCase();
};

class CUAVBufferSrcPSTest : public CUAVBufferSrcTest
{
public:
    void InitTestParameters();
    TEST_RESULT SetupTestCase();
    void CreateInitialData();
    TEST_RESULT CreateShader();
    TEST_RESULT ExecuteTestCase();

    // pixel shader with UAV only, or with both UAV and RTVs
    bool m_bUAVOnly;
};

class CUAVBufferRWTest : public CUAVBufferTest
{
public:
    CUAVBufferRWTest() :
        m_uThreadByteWidth(UAV_BUFFER_RW_TEST_BUFFER_SIZE)
    {
    }

    void CreateInitialData();
    TEST_RESULT CreateUAV();

protected:
    TEST_RESULT CompareResults(void *pResult);

    UINT m_uByteWidth; // total size of UAV
    UINT m_uThreadByteWidth; // size of portion of UAV allocated to each thread

    // test pattern to preinitalize UAV with
    UINT     m_uintTestPattern[4];
    int      m_sintTestPattern[4];
    float    m_floatTestPattern[4];
    MixGroup m_mixTestPattern;
};

class CUAVBufferRWCSTest : public CUAVBufferRWTest
{
public:
    CUAVBufferRWCSTest()
    {
    }

    void InitTestParameters();
    void CreateInitialData();
    TEST_RESULT CreateShader();
    TEST_RESULT ExecuteTestCase();
};

class CUAVBufferRWPSTest : public CUAVBufferRWTest
{
public:
    CUAVBufferRWPSTest()
    {
    }

    void InitTestParameters();
    TEST_RESULT SetupTestCase();
    void CreateInitialData();
    TEST_RESULT CreateShader();
    TEST_RESULT ExecuteTestCase();

    // pixel shader with UAV only, or with both UAV and RTVs
    bool m_bUAVOnly;
};

class CUAVBufferTypedTest : public CUAVBufferTest
{
public:
    CUAVBufferTypedTest() {}

    virtual TEST_RESULT CreateUAV();
    virtual TEST_RESULT SetupTestCase();

protected:
    virtual TEST_RESULT CompareResults(void *pResult);
    virtual float GetExpectedValue1(DXGI_FORMAT format);
    virtual float GetExpectedValue2(DXGI_FORMAT format);

    UINT     m_uByteWidth;
    UINT     m_uThreadByteWidth;

    UINT     m_uintTestPattern[4];
    int      m_sintTestPattern[4];
    float    m_floatTestPattern[4];
    MixGroup m_mixTestPattern;
};

class CUAVBufferTypedCSTest : public CUAVBufferTypedTest
{
public:
    CUAVBufferTypedCSTest() {}

    void InitTestParameters();
    void CreateInitialData();
    TEST_RESULT CreateShader();
    TEST_RESULT ExecuteTestCase();
};

class CUAVBufferTypedPSTest : public CUAVBufferTypedTest
{
public:
    CUAVBufferTypedPSTest() {}

    void InitTestParameters();
    void CreateInitialData();
    TEST_RESULT CreateShader();
    TEST_RESULT ExecuteTestCase();

    // pixel shader with UAV only, or with both UAV and RTVs
    bool m_bUAVOnly;
};

class CUAVTypedTest : public CUAVTest
{
public:
    CUAVTypedTest() :
        m_uWidth(RESOURCE_WIDTH),
        m_uHeight(RESOURCE_HEIGHT),
        m_uDepth(RESOURCE_DEPTH),
        m_uMipSlice(0),
        m_uFirstArraySlice(0),
        m_uArraySize(RESOURCE_ARRAY_SIZE),
        m_bPixelShaderTest(FALSE),
        m_bUAVOnly(FALSE),
        m_ScaleFactor(128.0f),
        m_ScaleFactorString(_T("128")),
        m_ScaleFactorDefault(128.0f),
        m_ScaleFactorStringDefault(_T("128"))
    {
    }

    TEST_RESULT Setup();
    TEST_RESULT SetupTestCase();
    void        InitTestParameters();
    void        CreateInitialData() { };
    TEST_RESULT CreateUAV();
    TEST_RESULT CreateShader();
    TEST_RESULT ExecuteTestCase();

protected:
    bool        IsTypedUAVLoadFormatSupported(DXGI_FORMAT Format);
    bool        IsFormatSupportedForDimension(DXGI_FORMAT Format, D3D11_UAV_DIMENSION Dimension);
    TEST_RESULT CompareResults(void *pResult, UINT subresource, UINT rowPitch, UINT depthPitch);
    tstring     GetFormatType(DXGI_FORMAT format);

    UINT  m_uWidth;
    UINT  m_uHeight;
    UINT  m_uDepth;
    UINT  m_uMipSlice;
    UINT  m_uFirstArraySlice;
    UINT  m_uArraySize;
    UINT  m_uDepthSlice;
    bool  m_bTestCube;
    bool  m_bPixelShaderTest;
    // pixel shader with UAV only, or with both UAV and RTVs
    bool  m_bUAVOnly;
    float   m_ScaleFactor;
    tstring m_ScaleFactorString;
    const float   m_ScaleFactorDefault;
    const tstring m_ScaleFactorStringDefault;
};

class CUAVTypedPixelShaderTest : public CUAVTypedTest
{
public:
    CUAVTypedPixelShaderTest() { m_bPixelShaderTest = TRUE; }
    TEST_RESULT CreateShader();
    TEST_RESULT ExecuteTestCase();
};
