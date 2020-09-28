#pragma once

#include <D3DX11Async.h>
#include "D3D11Test.h"
#include "WGFTestCore.h"
#include "D3DResourceFlagsValueSet.h"
#include <string>

//Order of operations to run most of these tests
// - Set input elements
// - m_LayoutInfo.InitializeInputElements()
// - Set Aligned Byte Offsets
// - m_LayoutInfo.SetLayoutData()
// - SetupPipeline()
// - Fill m_LayoutInfo members like IB and VB
// - GetImmediateContext()->Begin(m_pQuery)
// - DrawXXX
// - VerifyDrawXXX


const D3D11_PRIMITIVE_TOPOLOGY DefaultPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
const UINT MaxInputClassLayouts = 32;
const UINT MaxInputElements = 32;
const UINT MaxSOElements = 16;
const UINT MaxSOBufferSize = 0x4100000;
const UINT MaxBufferSize = 0x800000;
const char szSemantic[] = "data\0";
const UINT nMaxUAVs = 5;

struct DRAW_PARAMS
{
	UINT VertexCount;
	UINT StartVertexLocation;
};

struct DRAWINSTANCED_PARAMS
{
	UINT VertexCountPerInstance;
	UINT InstanceCount;
	UINT StartVertexLocation;
	UINT StartInstanceLocation;
};

struct DRAWINDEXED_PARAMS
{
	UINT IndexCount;
	UINT StartIndexLocation;
	INT BaseVertexLocation;
};

struct DRAWINDEXEDINSTANCED_PARAMS
{
	UINT IndexCountPerInstance;
	UINT InstanceCount;
	UINT StartIndexLocation;
	INT BaseVertexLocation;
	UINT StartInstanceLocation;
};

enum DrawMethod
{
	Draw,
	DrawInstanced,
	DrawInstancedIndirect,
	DrawIndexed,
	DrawIndexedInstanced,
	DrawIndexedInstancedIndirect,
	DrawAuto
};

enum DrawIndirectType
{
	UpdateSubresource,
	CopyStructureCount, 
	StreamOutputBuffer,
	RenderTargetBuffer
};

enum BufferSizeCase
{
	MaxBufferSizeCase,
	SmallSizeCase,
	SizeRequiredByDrawCase
};

enum VBOffsetsCase
{
	ZeroOffsets,
	SmallOffsets,
	BigOffsets
};

enum InputClassCase
{
	ICC_AllVertex,
	ICC_AllInstance,
	ICC_AlternateByVertex,
	ICC_AlternateByInstance,
	ICC_AlternateBy2Vertex,
	ICC_AlternateBy2Instance,
	ICC_AlternateByRandomVertex,
	ICC_AlternateByRandomInstance,
	ICC_NumCases
};

enum InputSlotCase
{
	ISC_Sequential,
	ISC_Random3,
	ISC_NoZero,
	ISC_SkipSlots,
	ISC_NumCases
};

//TODO: Add case zero
enum AlignedByteOffsetCase
{
	ABOC_MatchFormat,
	ABOC_MatchFormatOutOfOrder,
	ABOC_BiggerThanFormat,
	ABOC_BiggerThanFormatOutOfOrder,
	ABOC_SameOffsets,
	ABOC_NumCases
};

enum InstanceDataStepRateCase
{
	IDSRC_Zero,
	IDSRC_One,
	IDSRC_Random,
	IDSRC_NumCases
};

struct BufferSlot
{
	UINT numElements;
	UINT stride;
	UINT maxFormatStride;
	UINT VBBoundSize; //This is the size of the buffer bound.  0 means unbound.
	UINT VBOffset;
	const BYTE *pVBData;
	UINT firstElement; //index of the first element for this slot
	UINT lastElement; //index of the last element for this slot
	UINT elementsIndex[MaxInputElements];
};

struct LayoutInfo
{
	UINT numElements;
	UINT numOutElements;
	D3D11_INPUT_ELEMENT_DESC *pInputElements;
	UINT numBuffers;
	UINT maxSlot;
	UINT numVertexElements;
	UINT numInstanceElements;
	UINT IBBoundSize;
	UINT IBOffset;
	UINT IBStride;
	const void *pIBData;
	UINT SOStride;
	UINT SOSizeRequired;
	UINT numSOElements;
	UINT numComponents;
	bool bVertexId;
	bool bInstanceId;
	BufferSlot slots[32];

	LayoutInfo()
	{
		ZeroMemory(slots, sizeof(BufferSlot) * 32);
	}

	void  InitializeInputElements(D3D11_INPUT_ELEMENT_DESC *pInputLayoutDesc, UINT numElements);
	void  SetLayoutData(bool bVertexId, bool bInstanceId, UINT numDrawElements);
	std::string  CreateVSCode();
	std::string  CreateCSCode();
	void  CreateStreamOutputDecl(D3D11_SO_DECLARATION_ENTRY *pSODecl);
};

DECLARE_NAMED_VALUES( VBOffsetsCase )
DECLARE_NAMED_VALUES( BufferSizeCase )
DECLARE_NAMED_VALUES( InputClassCase )
DECLARE_NAMED_VALUES( DrawMethod )
DECLARE_NAMED_VALUES( DrawIndirectType )

extern const D3D11_INPUT_CLASSIFICATION InputClassLayouts[ICC_NumCases][MaxInputElements];
extern const UINT InputSlotElements[ISC_NumCases][MaxInputElements];
extern const UINT InstanceDataStepRateElements[IDSRC_NumCases][MaxInputElements];

//Helper functions
extern void ShuffleElements(UINT seed, UINT numElements, UINT *pArray);
extern DWORD GetFormatType(DXGI_FORMAT format);
extern std::string GetShaderTypeFromFormat(DXGI_FORMAT format);
extern UINT GetNumComponentsForFormat(DXGI_FORMAT format);
extern UINT GetFormatStrideInBytes(DXGI_FORMAT format);
extern tstring GetHexOfMemory(const BYTE* pMemory, UINT stride);
extern void AddFormatValues(DWORD formatType, UINT numComponents, void* pData1, void* pData2, void* pResult);


class CInputAssemblerTest : public CD3D11Test
{
public:
	CInputAssemblerTest();
	virtual TEST_RESULT Setup();
	virtual void Cleanup();
	virtual void CleanupTestCase(){};
	virtual void InitTestParameters();

	TEST_RESULT SetupPipeline(bool bNullInputLayout);

	bool VerifyDrawResults(UINT VertexCount, UINT StartVertexLocation);
	bool VerifyDrawInstancedResults(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation);
	bool VerifyDrawIndexedResults(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
	bool VerifyDrawIndexedInstancedResults(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation);
	bool VerifyDrawAutoResults(UINT VertexCount);
	virtual bool IsNoVBBound() { return false; };	// By default, a VB is always bound

protected:
	bool ShouldCheckCounters(  UINT InstanceCount );

	ID3D11Device			   *m_pDevice;
	ID3D11DeviceContext		   *m_pDeviceContext;
	ID3D11Debug				   *m_pDebug;
	ID3D11InfoQueue			   *m_pInfoQueue;

	bool				        m_bSetupBuffers;

	ID3D11DepthStencilState    *m_pDSState;
	ID3D11Buffer			   *m_pSOBuffer;
	ID3D11Buffer			   *m_pSOBufferZero;
	ID3D11Buffer			   *m_pSOBufferCopy;
	ID3D11Buffer			   *m_pSOVBBuffer;
	ID3D11Buffer			   *m_pSOArgBuffer;
	ID3D11Buffer			   *m_pVertexBuffers[MaxInputElements];
	ID3D11Buffer			   *m_pIndexBufferBig;
	ID3D11Buffer			   *m_pIndexBufferSmall;
	ID3D11Buffer			   *m_pDrawIndirectArgumentBuffer; 
	ID3D11InputLayout		   *m_pInputLayout;
	ID3D11Query				   *m_pQuery;
	ID3D11GeometryShader	   *m_pGS;
	ID3D11VertexShader		   *m_pVS;

	BYTE				        m_BufferData[MaxInputElements][MaxBufferSize];
	WORD				        m_IndexBufferData[MaxBufferSize];

	DrawMethod			        m_DrawMethod;
	DrawIndirectType	        m_DrawIndirectType;
	UINT                        m_nBufferUAVFlag;

	D3D11_INPUT_ELEMENT_DESC	m_InputElements[MaxInputElements];
	LayoutInfo				    m_LayoutInfo;
	bool					    m_bVertexID;
	bool					    m_bInstanceID;
	UINT					    m_NumElements;
	UINT					    m_MaxNumElements;
};

//
class ID3D11InputLayoutsTest : public CInputAssemblerTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	InputClassCase				m_InputClassCase;
	InputSlotCase				m_InputSlotCase;
	AlignedByteOffsetCase		m_AlignedByteOffsetCase;
	InstanceDataStepRateCase	m_InstanceDataStepRateCase;

	DXGI_FORMAT		m_Format;
	UINT			m_InputSlot;
	UINT			m_AlignedByteOffset;
	UINT			m_InputSlotClass;
	UINT			m_InstanceDataStepRate;
	DrawMethod		m_DrawMethod;
};

class CGeneratedValuesTest : public CInputAssemblerTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	InputClassCase	m_InputClassCase;

	DrawMethod		m_DrawMethod;

	UINT			m_VertexCount;
	UINT			m_StartVertexLocation;
	UINT			m_IndexCount;
	UINT			m_StartIndexLocation;
	INT				m_BaseVertexLocation;
	UINT			m_InstanceCount;
	UINT			m_StartInstanceLocation;

	bool			m_NullInputLayout;
	bool			m_bIBBound;
	bool			m_bIBBigEnough;
};


class CUnboundBuffersTest : public CInputAssemblerTest
{
public:
	CUnboundBuffersTest()
	{
		for(UINT i=0; i<MaxInputElements; i++)
		{
			m_pSmallVertexBuffers[i] = NULL;
		}
	}
	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void Cleanup();
	virtual bool IsNoVBBound() { return m_bNoVBBound; };

protected:
	InputClassCase				m_InputClassCase;
	InputSlotCase				m_InputSlotCase;
	AlignedByteOffsetCase		m_AlignedByteOffsetCase;
	InstanceDataStepRateCase	m_InstanceDataStepRateCase;

	DXGI_FORMAT		m_Format;
	UINT			m_InputSlot;
	UINT			m_AlignedByteOffset;
	UINT			m_InputSlotClass;
	UINT			m_InstanceDataStepRate;
	DrawMethod		m_DrawMethod;
	bool			m_bUseOffsetInDraw;
	bool			m_bNoVBBound;

	ID3D11Buffer			*m_pSmallVertexBuffers[MaxInputElements];

	bool					m_bIBBound;
};

class CDrawTest : public CInputAssemblerTest
{
public:
	CDrawTest() :
		m_pDrawSizeIndexBuffer(NULL),
		m_pUAVSrcBuf(NULL),
		m_pStaging(NULL),
		m_nAlignedByteOffsetForArgs(0)
	{
		for(UINT i=0; i<MaxInputElements; i++)
		{
			m_pSmallVertexBuffers[i] = NULL;
			m_pDrawSizeVertexBuffers[i] = NULL;
		}

		for ( UINT i = 0; i < nMaxUAVs; i++ )
		{
			m_pUnorderedAccessView[i] = NULL;
		}
	}

	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();
	virtual void Cleanup();
	void InitCommonTestParameters();
	virtual bool IsNoVBBound();

protected:

	virtual HRESULT  SetupArgumentBufferForDrawIndirect();
	virtual HRESULT  SetupUnorderedAccessView( UINT* pData, UINT nElements );
	virtual HRESULT  ValidateArgumentBuffer( UINT* pData, UINT nElements );


	InputClassCase		m_InputClassCase;
	BufferSizeCase		m_VBSizesCase;
	VBOffsetsCase		m_VBOffsetsCase;
	BufferSizeCase		m_IBSizeCase;
	VBOffsetsCase		m_IBOffsetCase;

	UINT				m_VertexCount;
	UINT				m_StartVertexLocation;
	UINT				m_IndexCount;
	UINT				m_StartIndexLocation;
	INT					m_BaseVertexLocation;
	UINT				m_InstanceCount;
	UINT				m_StartInstanceLocation;

	ID3D11Buffer			   *m_pSmallVertexBuffers[MaxInputElements];
	ID3D11Buffer			   *m_pDrawSizeVertexBuffers[MaxInputElements];
	ID3D11Buffer			   *m_pDrawSizeIndexBuffer;
	ID3D11Buffer	           *m_pUAVSrcBuf;
	ID3D11UnorderedAccessView  *m_pUnorderedAccessView[nMaxUAVs];
	ID3D11Buffer               *m_pStaging;

	UINT						m_nAlignedByteOffsetForArgs;

	// common test parameters
	testfactor::RFactor m_rVBSizesCase;
	testfactor::RFactor m_rIBSizeCase;
	testfactor::RFactor m_rVBOffsetsCase;
	testfactor::RFactor m_rIBOffsetCase;
	testfactor::RFactor m_rInstancedInputLayout;
	testfactor::RFactor m_rNonInstancedInputLayout;
};

class CDrawIndirectTest : public CDrawTest
{
	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
};
 
//
class CBufferTest : public CInputAssemblerTest
{
public:
	CBufferTest() : m_pSrcBuffer( NULL ),
					m_pDestBuffer( NULL ),
					m_pDefaultIB( NULL ),
					m_pDefaultVB( NULL ),
					m_pResultBuffer( NULL )
		{};

	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void Cleanup();

protected:

	DrawMethod		m_DrawMethod;

	D3DResourceFlags		m_SrcResourceFlags;
	D3DResourceFlags		m_DestResourceFlags;
	RESOURCE_ACCESS_METHOD	m_AccessMethod;
	bool					m_bDrawSrcBuffer;
	bool					m_bDrawDestBuffer;
	bool					m_bUnbindBuffer;

	CD3DResourceFlagsValueSet *m_pVBIBCopySrcValueSet;
	CD3DResourceFlagsValueSet *m_pVBIBCopyDestValueSet;
	CD3DResourceFlagsValueSet *m_pVBIBCopyRegionSrcValueSet;
	CD3DResourceFlagsValueSet *m_pVBIBCopyRegionDestValueSet;
	CD3DResourceFlagsValueSet *m_pVBIBUpdateDestValueSet;

	CD3DResourceFlagsValueSet *m_pCopySrcValueSet;
	CD3DResourceFlagsValueSet *m_pCopyDestValueSet;
	CD3DResourceFlagsValueSet *m_pCopyRegionSrcValueSet;
	CD3DResourceFlagsValueSet *m_pCopyRegionDestValueSet;
	CD3DResourceFlagsValueSet *m_pUpdateDestValueSet;

	ID3D11Buffer			*m_pSrcBuffer;
	ID3D11Buffer			*m_pDestBuffer;
	ID3D11Buffer			*m_pDefaultIB;
	ID3D11Buffer			*m_pDefaultVB;
	ID3D11Buffer			*m_pResultBuffer;

	void SetVBIB(ID3D11Buffer* pBuffer, UINT BindFlags);
};

class CLimitsTest : public CInputAssemblerTest
{
public:
	CLimitsTest()
	{
		m_pInitialData = NULL;
	}

	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void Cleanup();

protected:

	tstring GetSRVCheckCode(UINT buffer, UINT index);
	bool			m_bFirstIf;

	//DrawMethod		m_DrawMethod;

	//D3D11_BIND_FLAGS m_BindFlags;
	//DXGI_FORMAT		m_Format;
	UINT			m_NumBuffers;
	UINT			m_NumIB;
	UINT			m_NumVBs;
	UINT			m_NumSRVBuffers;

	BYTE			*m_pInitialData;
	BYTE			m_pDefaultVBData[10 * 4 * 2];
	ID3D11Buffer			*m_pDefaultVB;

};

//////////////////////////////////////////////////////////////////////////////////////////////////

class CInputAssemblerApp : public CD3D11TestFramework
{
public:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	bool InitTestGroups();
	void InitOptionVariables();

	bool					m_bDebugHelper;
};
extern CInputAssemblerApp  g_App;
	