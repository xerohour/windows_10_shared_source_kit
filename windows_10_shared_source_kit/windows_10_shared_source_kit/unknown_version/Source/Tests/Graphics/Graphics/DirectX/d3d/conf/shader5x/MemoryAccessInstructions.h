//////////////////////////////////////////////////////////////////////////
//  memoryaccess.h
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"
#include <D3DSampleLocation.h> // for TRException

//---------------------------------------------------------------------------------------------------------------------------------
namespace MemoryAccessInstructions
{

typedef enum {Out_Low, Out_High, Out_PartialLow, Out_PartialHigh, In_Start, In_End, In_Middle, NoAddressType} AddressType;
typedef enum {UAV, TGSM} MemoryType;
typedef enum {Raw, Structured, Typed} ViewType;
typedef enum {SelectX=0, SelectY=1, SelectZ=2, SelectW=3}   SelectComponent;
typedef enum {Decrement, Underflow, Increment, Overflow}   CounterOperation;
typedef enum {NotACounter, Append, Count}   CounterUAVType;

DECLARE_NAMED_VALUES( MemoryAccessInstructions::AddressType )
DECLARE_NAMED_VALUES( MemoryAccessInstructions::MemoryType )
DECLARE_NAMED_VALUES( MemoryAccessInstructions::ViewType )
DECLARE_NAMED_VALUES( MemoryAccessInstructions::SelectComponent )
DECLARE_NAMED_VALUES( MemoryAccessInstructions::CounterOperation )
DECLARE_NAMED_VALUES( MemoryAccessInstructions::CounterUAVType )

class CMemoryAccessShader5xTest : public CShader5xTest
{
public:
protected:
	CMemoryAccessShader5xTest( DXGI_FORMAT RTFormat = DXGI_FORMAT_R8G8B8A8_UINT ) :  CShader5xTest( RTFormat ) {}
	void                                    CreateTestFactors();
	void                                    CreateResourceBasedOnFactors(UINT32 ByteSize, UINT32 StructureByteStride);
	void                                    CleanupResource();
    UINT32 *                                PointToComponent( UINT32 *, SelectComponent );
    int                                     AddressTypeToOffset( AddressType a);
    bool                                    IsOutOfBoundsOffset( AddressType a);
    std::string                             SelectComponentToString( SelectComponent c );
    
protected:
	AddressType                             m_AddressType;
	MemoryType                              m_MemoryType;
	ViewType                                m_ViewType;
	DXGI_FORMAT                             m_UAVFormat;
	SelectComponent                         m_SelectSourceComponent;
	SelectComponent                         m_SelectSource2Component;
	UINT                                    m_NumComponents;
	CounterOperation                        m_CounterOperation;
	CounterUAVType                          m_CounterUAVType;
	SelectComponent                         m_DestMask;

	//These resources are created by CreateResourceBasedOnFactors based on the current parameters as driven by the test factors
	WindowsTest::CHandle< ID3D11Buffer >                 m_pStagingBuffer;
	WindowsTest::CHandle< ID3D11Texture3D >              m_pStagingTex3D;
	WindowsTest::CHandle< ID3D11Resource >               m_pTestResource;
	WindowsTest::CHandle< ID3D11UnorderedAccessView >    m_pTestUAV;

    static const UINT                       m_BufferSize = sizeof(UINT32)*3;

	//These factors are created by CreateTestFactors, and is available to be set as Root, or used in further factor expressions

	//appropriate for instructions that operate on SINT:
	testfactor::RFactor                     m_SINTFactors; 
	testfactor::RFactor                     m_SINTFactors2Sources; 

	//appropriate for instructions that operate on UINT:
	testfactor::RFactor                     m_UINTFactors;
	testfactor::RFactor                     m_UINTFactors2Sources;

	//for append and consume:
	testfactor::RFactor                     m_AppendFactor; 
	testfactor::RFactor                     m_ConsumeFactor; 
};

//---------------------------------------------------------------------------------------------------------------------------------
class CShader5xTest_Atomic_instruction : public MemoryAccessInstructions::CMemoryAccessShader5xTest
{
public:
    CShader5xTest_Atomic_instruction(); 

	TEST_RESULT Setup();
	void CleanupTest();

	TEST_RESULT ExecuteTestCase();
	void InitTestParameters();

	TEST_RESULT SetupTestCase();
	void CleanupTestCase();

protected:
    WindowsTest::CHandle< ID3D11Buffer >                 m_pReporterBuffer;
    WindowsTest::CHandle< ID3D11Buffer >                 m_pReporterStagingBuffer;
    WindowsTest::CHandle< ID3D11UnorderedAccessView >    m_pReporterUAV;

    static UINT32                           m_TestValues[];
    tstring                                 m_MadeShader;

protected:
    virtual UINT32  CalcResult(UINT32 Src0, UINT32 Src1, UINT32 & DestMemory)=0;
    virtual tstring GetInstruction()=0;
    virtual bool    CanOperateOnSINT() { return false; }
    virtual bool    CanOperateOnUINT() { return false; }
    virtual UINT    NumSources()       { return 1; }
    virtual bool    ReturnsValue()=0;
};

class CImmAtomicAllocConsume : public MemoryAccessInstructions::CMemoryAccessShader5xTest
{
public:
	CImmAtomicAllocConsume();
	void InitTestParameters();

    virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();

private:
	UINT m_StructCount;
	UINT m_StructureComponentCount;
};


//---------------------------------------------------------------------------------------------------------------------------------
#define DEFINE_ATOMIC_TEST( Name, NumSrcs, bSINT, bUINT, bImmediate, calc ) \
class CShader5xTest_Atomic_##Name: public CShader5xTest_Atomic_instruction              \
{                                                                                       \
protected:                                                                              \
    UINT32  CalcResult(UINT32 Src0, UINT32 Src1, UINT32 & DestMemory) { calc }          \
    tstring GetInstruction() { return #Name; }                                          \
    bool    CanOperateOnSINT() { return bSINT; }                                        \
    bool    CanOperateOnUINT() { return bUINT; }                                        \
    bool    ReturnsValue() { return bImmediate; }                                       \
    UINT    NumSources()       { return NumSrcs; }                                      \
};

#include "AtomicTestCases.h"

#undef DEFINE_ATOMIC_TEST

}; // namespace MemoryAccessInstructions


