#pragma once

#include "WGFCompute.h"

// CComputeAtomicityTest, base class for all atomic intrinsics
class CComputeAtomicityTest : public CComputeTest
{
protected:
    CComputeAtomicityTest()
        : m_bSubsetOperations(false)
        , m_SharedMemType(SHARED_MEMORY_TGSM)
        , m_pSharedUAVBuffer(NULL)
        , m_pSharedUAVBufferCopy(NULL)
        , m_pSharedUAV(NULL)
        , m_sharedUAVClearVal(0xffffffff)
        , m_skipTotalThreadCount(400)
    {
        ZeroStructMem(m_NumThreads);

        m_TargetUAVDimension = D3D11_UAV_DIMENSION_BUFFER;
        m_TargetUAVMiscFlags = static_cast<D3D11_RESOURCE_MISC_FLAG>(0);
    }

    virtual TEST_RESULT Setup();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual void CleanupTestCase();

    virtual void GetThreadGroupCount(UINT *x, UINT *y, UINT *z) = 0;
    virtual tstring GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES]) = 0;
    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData) = 0;

protected:
    UINT m_skipTotalThreadCount;
    UINT m_NumThreads[3];
    bool m_bSubsetOperations;
    SHARED_MEMORY_TYPE m_SharedMemType;
    UINT m_sharedUAVClearVal;
    ID3D11Buffer *m_pSharedUAVBuffer;
    ID3D11Buffer *m_pSharedUAVBufferCopy;
    ID3D11UnorderedAccessView *m_pSharedUAV;
};


// CAtomicSingleIntrinsicTest, base class for: add, min, max, and, or, xor
class CAtomicSingleIntrinsicTest : public CComputeAtomicityTest
{
protected:
    CAtomicSingleIntrinsicTest()
        : m_bOriginalValueVariant(false)
        , m_bSignedInt(false)
    {
    }

    virtual void InitTestParameters();

    virtual tstring GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES]);
    virtual tstring GetShaderValueString() = 0;
    virtual tstring GetIntrinsicName() = 0;
    virtual UINT GetTargetStartVal() = 0;

    bool m_bOriginalValueVariant;
    bool m_bSignedInt;
};


// CAtomicAddTest, tests InterlockedAdd
class CAtomicAddTest : public CAtomicSingleIntrinsicTest
{
public:
    CAtomicAddTest()
    {
    }

protected:
    virtual tstring GetIntrinsicName() { return "Add"; }
    virtual void GetThreadGroupCount(UINT *x, UINT *y, UINT *z)
    {
        // 10 groups of 1000 threads
        *x = 5;
        *y = 2;
        *z = 1;
    }
    virtual tstring GetShaderValueString();
    virtual UINT GetTargetStartVal();
    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData);
};


// CAtomicMinMaxTest, base class for min & max intrinsics
class CAtomicMinMaxTest : public CAtomicSingleIntrinsicTest
{
protected:
    CAtomicMinMaxTest()
    {
    }

    virtual void GetThreadGroupCount(UINT *x, UINT *y, UINT *z)
    {
        // 10 groups of 1000 threads
        *x = 5;
        *y = 2;
        *z = 1;
    }
    virtual tstring GetShaderValueString();
    virtual UINT GetTargetStartVal();
    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData);
    UINT GetThreadIterationValue(UINT n);
    UINT ModifyExpectedValue(UINT currVal, UINT iterationVal, const tstring& intrinsicName);
};


// CAtomicMinTest, tests InterlockedMin
class CAtomicMinTest : public CAtomicMinMaxTest
{
public:
    CAtomicMinTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "Min"; }
};


// CAtomicMaxTest, tests InterlockedMax
class CAtomicMaxTest : public CAtomicMinMaxTest
{
public:
    CAtomicMaxTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "Max"; }
};


// CAtomicBitwiseTest, base class for and, or, xor intrinsics
class CAtomicBitwiseTest : public CAtomicSingleIntrinsicTest
{
protected:
    CAtomicBitwiseTest()
    {
    }

    virtual void GetThreadGroupCount(UINT *x, UINT *y, UINT *z)
    {
        // 10 groups of 1000 threads
        *x = 5;
        *y = 2;
        *z = 1;
    }
    virtual tstring GetShaderValueString();
    virtual UINT GetTargetStartVal();
    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData);
    UINT GetThreadIterationValue(UINT n, const tstring& intrinsicName);
    UINT ModifyExpectedValue(UINT currVal, UINT iterationVal, const tstring& intrinsicName);
};


// CAtomicAndTest, tests InterlockedAnd
class CAtomicAndTest : public CAtomicBitwiseTest
{
public:
    CAtomicAndTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "And"; }
};


// CAtomicOrTest, tests InterlockedAnd
class CAtomicOrTest : public CAtomicBitwiseTest
{
public:
    CAtomicOrTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "Or"; }
};


// CAtomicXorTest, tests InterlockedAnd
class CAtomicXorTest : public CAtomicBitwiseTest
{
public:
    CAtomicXorTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "Xor"; }
};


// CAtomicStoreTest, base class for: CompareStore, Exchange, CompareExchange
class CAtomicStoreTest : public CComputeAtomicityTest
{
protected:
    CAtomicStoreTest()
    {
    }

    virtual void InitTestParameters();
    virtual tstring GetIntrinsicName() = 0;
    virtual void GetThreadGroupCount(UINT *x, UINT *y, UINT *z)
    {
        // 10 groups of 1000 threads
        *x = 5;
        *y = 2;
        *z = 1;
    }

    virtual tstring GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES]);
    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData);

private:
    struct WinnerInfo
    {
        UINT Thread;
        UINT Group;
        UINT Value;
        UINT Count;
        bool SharedUAVEqualsTargetUAV;
    };

private:
    TEST_RESULT VerifyWinner(WinnerInfo winner);

    TEST_RESULT VerifyCompareExchangeReturnValues(
        UINT iteration,
        WinnerInfo winner,
        UINT startGroup,
        UINT numberOfGroups,
        UINT numThreadsPerGroup,
        const D3D11_MAPPED_SUBRESOURCE* pBufferData);

    TEST_RESULT CAtomicStoreTest::VerifyExchangeReturnValues(
        UINT iteration,
        UINT startGroup,
        UINT numberOfGroups,
        UINT numThreadsPerGroup,
        const D3D11_MAPPED_SUBRESOURCE* pBufferData,
        __out UINT* pLastUpdate);

};


// CAtomicExchangeTest, tests InterlockedExchange
class CAtomicExchangeTest : public CAtomicStoreTest
{
public:
    CAtomicExchangeTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "Exchange"; }
};


// CAtomicCompareExchangeTest, tests InterlockedCompareExchange
class CAtomicCompareExchangeTest : public CAtomicStoreTest
{
public:
    CAtomicCompareExchangeTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "CompareExchange"; }
};


// CAtomicCompareStoreTest, tests InterlockedCompareStore
class CAtomicCompareStoreTest : public CAtomicStoreTest
{
public:
    CAtomicCompareStoreTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "CompareStore"; }
};


// CMultiAtomicCompareExchangeTest, tests InterlockedCompareExchange with InterlockedAdd
class CMultiAtomicTest : public CAtomicStoreTest
{
public:
    CMultiAtomicTest();
    
protected:
    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    
    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData);
    virtual TEST_RESULT VerifyReturnValues(
        UINT iteration,
        UINT startGroup,
        UINT numberOfGroups,
        UINT numThreadsPerGroup,
        bool replaceOccured,
        const D3D11_MAPPED_SUBRESOURCE* pBufferData);

protected:
    UINT m_ReplaceValue;
    UINT m_AddValue;
    UINT m_ClearVal;
    tstring m_Shader_GroupSharedMem;

};

class CMultiAtomicCompareExchangeTest : public CMultiAtomicTest
{
public:
    CMultiAtomicCompareExchangeTest()
    {
    }

protected:
    virtual tstring GetIntrinsicName() { return "CompareExchange"; }
    virtual tstring GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES]);

};


// CMultiAtomicCompareStoreTest, tests InterlockedCompareStore with InterlockedAdd
class CMultiAtomicCompareStoreTest : public CMultiAtomicTest
{
public:
    CMultiAtomicCompareStoreTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "CompareStore"; }
    virtual tstring GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES]);
};


// CAtomicMultipleIntrinsicTest
class CAtomicMultipleIntrinsicTest : public CComputeAtomicityTest
{
public:
    CAtomicMultipleIntrinsicTest()
    {
        m_sharedUAVClearVal = 0x40E380F0; // Note: Special value from zhizheng's d3d11 compute whql test spec
        m_skipTotalThreadCount = 100000;
		m_bInterGroup = false;
    }

protected:
	bool m_bInterGroup;
    virtual void GetThreadGroupCount(UINT *x, UINT *y, UINT *z)
    {
		if ( !m_bInterGroup)
		{
			// 2048 groups of 32 threads
			*x = 128;
			*y = 8;
			*z = 2;
		}
		else
		{
			// 32 groups of 1024 threads
			*x = 32;
			*y = 1;
			*z = 1;
		}
    }

    virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();

    virtual tstring GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES]);
    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData);

    virtual UINT GetNumberOfThreadsPerGroup() const
    {
        const UINT tx = m_NumThreads[0];
        const UINT ty = m_NumThreads[1];
        const UINT tz = m_NumThreads[2];
        return tx * ty * tz;
    }

    virtual UINT GetNumberOfGroupsDispatched()
    {
        UINT gx, gy, gz;
        GetThreadGroupCount( &gx, &gy, &gz );
        return gx * gy * gz;
    }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
class CAtomicityPSTest : public CComputeTest
{
public:
    CAtomicityPSTest():
      m_ViewWidth(0),
      m_ViewHeight(0),
      m_bSubsetOperations(false),
      m_pPixelShader(NULL),
      m_pUAVBuf(NULL),
      m_pUAVBufCopy(NULL),
      m_UAVClearVal(0xffffffff),
      m_BufferSize(0),
      m_bUseTargetUAV(true)
      {
          m_ppUAVs[0] =  m_ppUAVs[1] = NULL;
      };

      virtual TEST_RESULT Setup();
      virtual TEST_RESULT SetupTestCase();
      virtual TEST_RESULT ExecuteTestCase();
      virtual void CleanupTestCase();
      virtual void Cleanup();
      UINT GetPixelCount();

      virtual tstring GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES]) = 0;
      virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData) = 0;
      virtual UINT GetTargetStartVal() = 0;


protected:
    UINT m_ViewWidth;
    UINT m_ViewHeight;
    tstring m_TypeUAV;
    bool m_bSubsetOperations;
    bool m_bUseTargetUAV;

    ID3D11PixelShader *m_pPixelShader;

    // UAVs
    ID3D11UnorderedAccessView * m_ppUAVs[2];
    ID3D11Buffer * m_pUAVBuf;
    ID3D11Buffer * m_pUAVBufCopy;
    UINT m_BufferSize; // number of elements in the buffer
    UINT m_UAVClearVal;

    TEST_RESULT CreateUAVs();
    TEST_RESULT SetupPixelShader();
};


// CAtomicSingleIntrinsicTest, base class for: add, min, max, and, or, xor
class CAtomicSingleIntrinsicPSTest : public CAtomicityPSTest
{
public:
    CAtomicSingleIntrinsicPSTest()
        : m_bOriginalValueVariant(false)
    {
    }

    virtual void InitTestParameters();

    virtual tstring GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES]);
    virtual tstring GetShaderValueString() = 0;
    virtual tstring GetIntrinsicName() = 0;
    virtual UINT GetTargetStartVal() = 0;

    bool m_bOriginalValueVariant;
};

// CAtomicAddPSTest, tests InterlockedAdd
class CAtomicAddPSTest : public CAtomicSingleIntrinsicPSTest
{
public:
    CAtomicAddPSTest()
    {
    }

public:
    virtual tstring GetIntrinsicName() { return "Add"; }
    virtual tstring GetShaderValueString();
    virtual UINT GetTargetStartVal();
    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData);
};


// CAtomicMinMaxTest, base class for min & max intrinsics
class CAtomicMinMaxPSTest : public CAtomicSingleIntrinsicPSTest
{
protected:
    CAtomicMinMaxPSTest()
    {
    }

    virtual tstring GetShaderValueString();
    virtual UINT GetTargetStartVal();
    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData);
    UINT GetPixelIterationValue(UINT n);
    UINT ModifyExpectedValue(UINT currVal, UINT iterationVal, const tstring& intrinsicName);
};


// CAtomicMinTest, tests InterlockedMin
class CAtomicMinPSTest : public CAtomicMinMaxPSTest
{
public:
    CAtomicMinPSTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "Min"; }
};


// CAtomicMaxTest, tests InterlockedMax
class CAtomicMaxPSTest : public CAtomicMinMaxPSTest
{
public:
    CAtomicMaxPSTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "Max"; }
};



// CAtomicBitwisePSTest, base class for and, or, xor intrinsics
class CAtomicBitwisePSTest : public CAtomicSingleIntrinsicPSTest
{
protected:
    CAtomicBitwisePSTest()
    {
    }


    virtual tstring GetShaderValueString();
    virtual UINT GetTargetStartVal();
    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData);
    UINT GetPixelIterationValue(UINT n, const tstring& intrinsicName);
    UINT ModifyExpectedValue(UINT currVal, UINT iterationVal, const tstring& intrinsicName);
};


// CAtomicAndPSTest, tests InterlockedAnd
class CAtomicAndPSTest : public CAtomicBitwisePSTest
{
public:
    CAtomicAndPSTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "And"; }
};


// CAtomicOrPSTest, tests InterlockedAnd
class CAtomicOrPSTest : public CAtomicBitwisePSTest
{
public:
    CAtomicOrPSTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "Or"; }
};


// CAtomicXorPSTest, tests InterlockedAnd
class CAtomicXorPSTest : public CAtomicBitwisePSTest
{
public:
    CAtomicXorPSTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "Xor"; }
};


// CAtomicStorePSTest, base class for: CompareStore, Exchange, CompareExchange
class CAtomicStorePSTest : public CAtomicityPSTest
{
protected:
    CAtomicStorePSTest()
    {
    }

    virtual void InitTestParameters();
    virtual tstring GetIntrinsicName() = 0;
    virtual tstring GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES]);
    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData);
    virtual UINT GetTargetStartVal(){return m_UAVClearVal;}

private:
    struct WinnerInfo
    {
        UINT PID;
        UINT Value;
        UINT Count;
    };

private:
    TEST_RESULT VerifyWinner(WinnerInfo winner);

    TEST_RESULT VerifyCompareExchangeReturnValues(
        UINT iteration,
        WinnerInfo winner,
        UINT numPixels,
        const D3D11_MAPPED_SUBRESOURCE* pBufferData);

    TEST_RESULT CAtomicStorePSTest::VerifyExchangeReturnValues(
        UINT iteration,
        UINT numPixels,
        const D3D11_MAPPED_SUBRESOURCE* pBufferData,
        __out UINT* pLastUpdate);

};





// CAtomicExchangePSTest, tests InterlockedExchange
class CAtomicExchangePSTest : public CAtomicStorePSTest
{
public:
    CAtomicExchangePSTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "Exchange"; }
};


// CAtomicCompareExchangePSTest, tests InterlockedCompareExchange
class CAtomicCompareExchangePSTest : public CAtomicStorePSTest
{
public:
    CAtomicCompareExchangePSTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "CompareExchange"; }
};


// CAtomicCompareStorePSTest, tests InterlockedComparePSStore
class CAtomicCompareStorePSTest : public CAtomicStorePSTest
{
public:
    CAtomicCompareStorePSTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "CompareStore"; }
};


class CMultiAtomicPSTest : public CAtomicStorePSTest
{
public:
    CMultiAtomicPSTest();

protected:
    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();

    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData);
    virtual TEST_RESULT VerifyReturnValues(
        UINT iteration,
        UINT numPixels,
        bool replaceOccured,
        const D3D11_MAPPED_SUBRESOURCE* pBufferData);

protected:
    UINT m_ReplaceValue;
    UINT m_AddValue;
    UINT m_ClearVal;
    tstring m_Shader;

};

// CMultiAtomicCompareExchangePSTest, tests InterlockedCompareExchange with InterlockedAdd
class CMultiAtomicCompareExchangePSTest : public CMultiAtomicPSTest
{
public:
    CMultiAtomicCompareExchangePSTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "CompareExchange"; }
    virtual tstring GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES]);
};

// CMultiAtomicCompareStorePSTest, tests InterlockedCompareStore with InterlockedAdd
class CMultiAtomicCompareStorePSTest : public CMultiAtomicPSTest
{
public:
    CMultiAtomicCompareStorePSTest()
    {
    }
protected:
    virtual tstring GetIntrinsicName() { return "CompareStore"; }
    virtual tstring GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES]);
};


// CAtomicMultipleIntrinsicPSTest
class CAtomicMultipleIntrinsicPSTest : public CAtomicityPSTest
{
public:
    CAtomicMultipleIntrinsicPSTest()
    {
        m_UAVClearVal = 0x40E380F0; // Note: Special value from zhizheng's d3d11 compute whql test spec
    }

protected:

    virtual void InitTestParameters();

    virtual tstring GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES]);
    virtual TEST_RESULT VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData);
    virtual UINT GetTargetStartVal(){return m_UAVClearVal;}
};