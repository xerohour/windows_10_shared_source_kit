//////////////////////////////////////////////////////////////////////////
//  MemoryInstructions.cpp
//  created:	2009/06/11
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////


#include <float.h>
#include "MemoryAccessInstructions.h"
#include <strsafe.h>

using namespace MemoryAccessInstructions;

//---------------------------------------------------------------------------------------------------------------------------------
void ThrowTRFailure( HRESULT hr, const char * Message)
{
	if (FAILED(hr))
	{
		TRException exc(Message);
		char hrMsg[1000];
		StringCchPrintf(hrMsg, ARRAYSIZE(hrMsg), " hr = %08x, " );
		exc.Prepend(hrMsg);
		throw exc;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------
class CMapResource
{
public:
    CMapResource( CD3D11Test * pTest, ID3D11Resource * pResource, UINT Subresource = 0 ) :
        m_Subresource( Subresource ),
        m_pTest( pTest ),
        m_pResource( pResource )
    {
		ThrowTRFailure( m_pTest->ExecuteEffectiveContext(), 
            "Failed to execute effective context before map");

        ThrowTRFailure( m_pTest->GetImmediateContext()->Map( m_pResource, m_Subresource, D3D11_MAP_READ_WRITE, 0, &m_mappedResource ), 
            "Couldn't map output staging");
    }
    ~CMapResource(  )
    {
        m_pTest->GetImmediateContext()->Unmap( m_pResource, m_Subresource );
    }
    UINT32 *    Uint32( UINT x, UINT y = 0, UINT z = 0 )
    { 
        BYTE * pb = reinterpret_cast<BYTE*>(m_mappedResource.pData);
        return reinterpret_cast<UINT32*>( 
            pb + 
            x * sizeof(UINT32) +
            y * m_mappedResource.RowPitch + 
            z * m_mappedResource.DepthPitch  );
    }

protected:
	WindowsTest::CHandle<ID3D11Resource> m_pResource; 
    UINT             m_Subresource;
    CD3D11Test     * m_pTest; 
    D3D11_MAPPED_SUBRESOURCE m_mappedResource;
};


//---------------------------------------------------------------------------------------------------------------------------------
namespace MemoryAccessInstructions
{
BEGIN_NAMED_VALUES( MemoryAccessInstructions::AddressType )
	NAMED_VALUE( _T( "Out_Low" ), Out_Low )
	NAMED_VALUE( _T( "Out_High" ), Out_High )
	NAMED_VALUE( _T( "Out_PartialLow" ), Out_PartialLow )
	NAMED_VALUE( _T( "Out_PartialHigh" ), Out_PartialHigh )
	NAMED_VALUE( _T( "In_Start" ), In_Start )
	NAMED_VALUE( _T( "In_End" ), In_End )
	NAMED_VALUE( _T( "In_Middle" ), In_Middle )
	NAMED_VALUE( _T( "NoAddressType" ), NoAddressType )
END_NAMED_VALUES( MemoryAccessInstructions::AddressType )

BEGIN_NAMED_VALUES( MemoryType )
	NAMED_VALUE( _T( "UAV" ), UAV )
	NAMED_VALUE( _T( "TGSM" ), TGSM )
END_NAMED_VALUES( MemoryType )

BEGIN_NAMED_VALUES( ViewType )
	NAMED_VALUE( _T( "Raw" ), Raw )
	NAMED_VALUE( _T( "Structured" ), Structured )
	NAMED_VALUE( _T( "Typed" ), Typed )
END_NAMED_VALUES( ViewType )

BEGIN_NAMED_VALUES( SelectComponent )
	NAMED_VALUE( _T( "SelectX" ), SelectX )
	NAMED_VALUE( _T( "SelectY" ), SelectY )
	NAMED_VALUE( _T( "SelectZ" ), SelectZ )
	NAMED_VALUE( _T( "SelectW" ), SelectW )
END_NAMED_VALUES( SelectComponent )

BEGIN_NAMED_VALUES( CounterOperation )
	NAMED_VALUE( _T( "Decrement" ), Decrement )
	NAMED_VALUE( _T( "Underflow" ), Underflow )
	NAMED_VALUE( _T( "Increment" ), Increment )
	NAMED_VALUE( _T( "Overflow" ), Overflow )
END_NAMED_VALUES( CounterOperation )

BEGIN_NAMED_VALUES( CounterUAVType )
	NAMED_VALUE( _T( "NotACounter" ), NotACounter )
	NAMED_VALUE( _T( "Append" ), Append )
	NAMED_VALUE( _T( "Count" ), Count )
END_NAMED_VALUES( CounterUAVType )
};

//---------------------------------------------------------------------------------------------------------------------------------
static const TCHAR* SelectComponentToMaskString(SelectComponent cmp)
{
	switch(cmp)
	{
	case SelectX:
		return _T("x");

	case SelectY:
		return _T("y");

	case SelectZ:
		return _T("z");

	case SelectW:
		return _T("w");

	default:
		return _T("");
	}
}

//---------------------------------------------------------------------------------------------------------------------------------
void CMemoryAccessShader5xTest::CreateTestFactors()
{
	AddParameter< AddressType       >( _T("AddressType"),       &m_AddressType );
	AddParameter< MemoryType        >( _T("MemoryType"),        &m_MemoryType );
	AddParameter< ViewType          >( _T("ViewType"),          &m_ViewType );
	AddParameter< SelectComponent   >( _T("SelectSourceComponent"),   &m_SelectSourceComponent );
	AddParameter< SelectComponent   >( _T("SelectSource2Component"),  &m_SelectSource2Component );
	AddParameter< SelectComponent   >( _T("DestMask"),          &m_DestMask );
	AddParameter< D3D_SHADER_STAGE  >( _T("SHADER_STAGE_TYPE"), &m_ShaderStage );
	AddParameter< DXGI_FORMAT       >( _T("UAVFormat"),         &m_UAVFormat );
	AddParameter< UINT              >( _T("NumComponents"),     &m_NumComponents );
	AddParameter< CounterOperation  >( _T("CounterOperation"),  &m_CounterOperation );
	AddParameter< CounterUAVType    >( _T("CounterUAVType"),    &m_CounterUAVType );

	testfactor::RFactor rNotACounterUAV= 
		AddParameterValue( _T("CounterUAVType"), NotACounter );
	testfactor::RFactor rAppendUAVType = 
		AddParameterValue( _T("CounterUAVType"), Append );
	testfactor::RFactor rCountUAVType = 
		AddParameterValue( _T("CounterUAVType"), Count );

	testfactor::RFactor rDestMaskX = 
		AddParameterValue( _T("DestMask"), SelectX );
	testfactor::RFactor rDestMaskY = 
		AddParameterValue( _T("DestMask"), SelectY );
	testfactor::RFactor rDestMaskZ = 
		AddParameterValue( _T("DestMask"), SelectZ );
	testfactor::RFactor rDestMaskW = 
		AddParameterValue( _T("DestMask"), SelectW );

	testfactor::RFactor rDecUnder = 
		AddParameterValue( _T("CounterOperation"), Decrement ) +
		AddParameterValue( _T("CounterOperation"), Underflow );

	testfactor::RFactor rIncOver = 
		AddParameterValue( _T("CounterOperation"), Increment ) +
		AddParameterValue( _T("CounterOperation"), Overflow );

	testfactor::RFactor r1Component = 
		AddParameterValue( _T("NumComponents"), (UINT) 1 );
	testfactor::RFactor r4Components = 
		AddParameterValue( _T("NumComponents"), (UINT) 4 );

	testfactor::RFactor rPS = 
		AddParameterValue( _T("SHADER_STAGE_TYPE"), D3D_SHADER_STAGE_PIXEL );

	testfactor::RFactor rCS = 
		AddParameterValue( _T("SHADER_STAGE_TYPE"), D3D_SHADER_STAGE_COMPUTE );

	testfactor::RFactor rVS = 
		AddParameterValue( _T("SHADER_STAGE_TYPE"), D3D_SHADER_STAGE_VERTEX );

	testfactor::RFactor rGS = 
		AddParameterValue( _T("SHADER_STAGE_TYPE"), D3D_SHADER_STAGE_GEOMETRY );

	testfactor::RFactor rHS = 
		AddParameterValue( _T("SHADER_STAGE_TYPE"), D3D_SHADER_STAGE_HULL );

	testfactor::RFactor rDS = 
		AddParameterValue( _T("SHADER_STAGE_TYPE"), D3D_SHADER_STAGE_DOMAIN );

	// Pixel shader is the only rendering shader supported below feature level 11.1
	testfactor::RFactor rNonCSShaderStages = rPS;
	if( g_TestApp.IsFL11_1OrGreater() )
	{
		rNonCSShaderStages = rNonCSShaderStages + rVS + rGS + rHS + rDS;
	}

	testfactor::RFactor rAddressOut = 
		AddParameterValue( _T("AddressType"), Out_Low ) +
		AddParameterValue( _T("AddressType"), Out_High ) +
		AddParameterValue( _T("AddressType"), Out_PartialLow ) +
		AddParameterValue( _T("AddressType"), Out_PartialHigh );

	testfactor::RFactor rAddressMiddle = 
		AddParameterValue( _T("AddressType"), In_Middle );

	testfactor::RFactor rAddressInButNotMiddle = 
		AddParameterValue( _T("AddressType"), In_Start ) +
		AddParameterValue( _T("AddressType"), In_End );

	testfactor::RFactor rNoAddressType = 
		AddParameterValue( _T("AddressType"), NoAddressType );

	testfactor::RFactor rUAV = 
		AddParameterValue( _T("MemoryType"), UAV );

	testfactor::RFactor rTGSM = 
		AddParameterValue( _T("MemoryType"), TGSM );

	testfactor::RFactor FormatFactorUINT = 
		AddParameterValue( _T("UAVFormat"), DXGI_FORMAT_R32_UINT );
	testfactor::RFactor FormatFactorSINT = 
		AddParameterValue( _T("UAVFormat"), DXGI_FORMAT_R32_SINT );
	testfactor::RFactor FormatFactorFloat = 
		AddParameterValue( _T("UAVFormat"), DXGI_FORMAT_R32_FLOAT );

	testfactor::RFactor rSelectSourceComponentX = 
		AddParameterValue( _T("SelectSourceComponent"), SelectX );

	testfactor::RFactor rSelectSourceComponentFactor = 
        rSelectSourceComponentX +
		AddParameterValue( _T("SelectSourceComponent"), SelectY ) +
		AddParameterValue( _T("SelectSourceComponent"), SelectZ ) +
		AddParameterValue( _T("SelectSourceComponent"), SelectW );

	testfactor::RFactor rRaw = 
		AddParameterValue( _T("ViewType"), Raw );
	testfactor::RFactor rStructured = 
		AddParameterValue( _T("ViewType"), Structured );
	testfactor::RFactor rTyped = 
		AddParameterValue( _T("ViewType"), Typed );

	//-----------------------------------------------------------------------------------------------------------------------------
//	m_SelectSource2 = 
	testfactor::RFactor rSelectSource2X = 
		AddParameterValue( _T("SelectSource2Component"), SelectX );
	testfactor::RFactor rSelectSource2 = 
        rSelectSource2X +
		AddParameterValue( _T("SelectSource2Component"), SelectY ) +
		AddParameterValue( _T("SelectSource2Component"), SelectZ ) +
		AddParameterValue( _T("SelectSource2Component"), SelectW );
	//-----------------------------------------------------------------------------------------------------------------------------
	testfactor::RFactor rRaw_X_Structured = rRaw + rStructured;

	testfactor::RFactor rDestMask1 = 
		rDestMaskX + rDestMaskY + rDestMaskZ + rDestMaskW;

	testfactor::RFactor rAllMaskCombos = 
        rDestMask1 * rSelectSourceComponentFactor;

	testfactor::RFactor rSimpleMasks = 
        rDestMaskX * rSelectSourceComponentX;

	testfactor::RFactor rAllMaskCombos_2Sources = 
        rDestMask1 * rSelectSourceComponentFactor * rSelectSource2;

	testfactor::RFactor rSimpleMasks_2Sources = 
        rDestMaskX * rSelectSourceComponentX * rSelectSource2X;

    //Only "in middle" is given all possible combos of source and dest masks
	testfactor::RFactor rComplexForInMiddle_SimpleForOtherIns = 
        (rAddressInButNotMiddle + rAddressOut) * rSimpleMasks + rAddressMiddle * rAllMaskCombos;

	testfactor::RFactor rComplexForInMiddle_SimpleForOtherIns_2Sources = 
        (rAddressInButNotMiddle + rAddressOut) * rSimpleMasks_2Sources + rAddressMiddle * rAllMaskCombos_2Sources;


	testfactor::RFactor UINTUAVFactors = 
		( ( rRaw_X_Structured * r1Component ) + ( rTyped * r4Components * FormatFactorUINT ) ) * rUAV;
	testfactor::RFactor SINTUAVFactors = 
		( ( rRaw_X_Structured * r1Component ) + ( rTyped * r4Components * FormatFactorSINT ) ) * rUAV;

	testfactor::RFactor TGSMFactors = rTGSM * rRaw_X_Structured * r1Component * FormatFactorUINT; //spec doesn't specify to multiply by num components (as it does for UAV)

	//-----------------------------------------------------------------------------------------------------------------------------
	m_UINTFactors = 
		( (UINTUAVFactors * rComplexForInMiddle_SimpleForOtherIns * rNotACounterUAV) % rNonCSShaderStages ) + 
		( ((UINTUAVFactors + TGSMFactors) * rComplexForInMiddle_SimpleForOtherIns * rNotACounterUAV) % rCS );

	m_SINTFactors = 
		( (SINTUAVFactors * rComplexForInMiddle_SimpleForOtherIns * rNotACounterUAV) % rNonCSShaderStages ) + 
		( ((SINTUAVFactors + TGSMFactors) * rComplexForInMiddle_SimpleForOtherIns * rNotACounterUAV) % rCS );

	m_UINTFactors2Sources = 
		( (UINTUAVFactors * rComplexForInMiddle_SimpleForOtherIns_2Sources * rNotACounterUAV) % rNonCSShaderStages ) +
		( ((UINTUAVFactors + TGSMFactors) * rComplexForInMiddle_SimpleForOtherIns_2Sources * rNotACounterUAV) % rCS );

	m_SINTFactors2Sources = 
		( (SINTUAVFactors * rComplexForInMiddle_SimpleForOtherIns_2Sources * rNotACounterUAV) % rNonCSShaderStages ) +
		( ((SINTUAVFactors + TGSMFactors) * rComplexForInMiddle_SimpleForOtherIns_2Sources * rNotACounterUAV) % rCS );

#if 0
    m_AddressOut = rAddressOut * 
        ( ((rUAV  * rTyped * r4Components * FormatFactorUINT) % rNonCSShaderStages) + 
          ( rCS * rTGSM * rRaw   * r1Component  * FormatFactorUINT) ) *
        rNotACounterUAV * 
        rSimpleMasks);
#endif

	//-----------------------------------------------------------------------------------------------------------------------------
	testfactor::RFactor rCounters =
		(rStructured * rDestMask1 * r1Component);

	//-----------------------------------------------------------------------------------------------------------------------------
	testfactor::RFactor rAppendConsume =
		rCounters * (rAppendUAVType + rCountUAVType) * rUAV * rNoAddressType;

	m_AppendFactor = (rAppendConsume * rIncOver) % (rNonCSShaderStages + rCS);
	m_ConsumeFactor = (rAppendConsume * rDecUnder) % (rNonCSShaderStages + rCS);

}

//---------------------------------------------------------------------------------------------------------------------------------
void CMemoryAccessShader5xTest::CreateResourceBasedOnFactors(UINT32 ByteSize, UINT32 StructureByteStride)
{
	CleanupResource();

	if ( m_NumComponents == 1 )
	{
        //THis buffer is created even when the memory under test is TGSM, in order to have a mechanism to 
        //place initial data in the TGSM and to retrieve the TGSM as modified by the instruction under test.
        MemoryAccessInstructions::ViewType MyViewType = m_ViewType; 
        if ( m_MemoryType == TGSM )
            MyViewType = Typed;

		D3D11_BUFFER_DESC DescBuffer;

		DescBuffer.ByteWidth            = ByteSize;
		DescBuffer.Usage                = D3D11_USAGE_DEFAULT;
		DescBuffer.BindFlags            = D3D11_BIND_UNORDERED_ACCESS;
		DescBuffer.CPUAccessFlags       = 0;
		DescBuffer.MiscFlags            = 0;
		DescBuffer.MiscFlags           |= MyViewType == Structured ? D3D11_RESOURCE_MISC_BUFFER_STRUCTURED      : 0;
		DescBuffer.MiscFlags           |= MyViewType == Raw        ? D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS : 0;
		DescBuffer.StructureByteStride  = MyViewType == Structured ? StructureByteStride                        : 0;

		ThrowTRFailure( GetFramework()->GetDevice()->CreateBuffer(&DescBuffer, 0, reinterpret_cast<ID3D11Buffer**>(&m_pTestResource) ),
			"Couldn't create default test buffer");

		DescBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ; //this is read also, because of the CS_4_0 case. See Validate or CSUAVRead for details.
		DescBuffer.Usage = D3D11_USAGE_STAGING;
		DescBuffer.BindFlags = 0;
		DescBuffer.MiscFlags = 0;

		ThrowTRFailure( GetFramework()->GetDevice()->CreateBuffer(&DescBuffer, 0, &m_pStagingBuffer),
			"Couldn't create staging test buffer");

        {
		    D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;

		    uavd.ViewDimension          = D3D11_UAV_DIMENSION_BUFFER;
		    uavd.Buffer.FirstElement    = 0;
    		
            //If testing TGSM, then the uav is for transporting test data, and we choose "Typed"
            switch( MyViewType )
		    {
		    case Typed:         uavd.Buffer.Flags = 0;                         uavd.Format = m_UAVFormat;              break;
		    case Raw:           uavd.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW; uavd.Format = DXGI_FORMAT_R32_TYPELESS; break;
		    case Structured:    uavd.Buffer.Flags = 0;                         uavd.Format = DXGI_FORMAT_UNKNOWN;      break;
		    default : assert( false ); break;
		    }

		    UINT elementByteSize = DXGI_FORMAT_UNKNOWN == uavd.Format ? StructureByteStride : GetBitsPerElement( uavd.Format ) / 8;

		    uavd.Buffer.NumElements = ByteSize / elementByteSize;

		    switch( m_CounterUAVType )
		    {
		    case Append: uavd.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_APPEND;  break;
		    case Count:  uavd.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_COUNTER; break;
		    }

		    ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pTestResource, &uavd, & m_pTestUAV),
			    "Couldn't create UAV ");
        }
	}
	else if ( m_NumComponents == 4 )
	{
		assert( m_ViewType == Typed );

		D3D11_TEXTURE3D_DESC Tex3ddesc;
		Tex3ddesc.Width = ByteSize/sizeof(UINT32);
		Tex3ddesc.Height = ByteSize/sizeof(UINT32);
		Tex3ddesc.Depth = ByteSize/sizeof(UINT32);
		Tex3ddesc.MipLevels = 1;
		Tex3ddesc.Format = m_UAVFormat;
		Tex3ddesc.Usage = D3D11_USAGE_DEFAULT;
		Tex3ddesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		Tex3ddesc.CPUAccessFlags = 0;
		Tex3ddesc.MiscFlags = 0;

		ThrowTRFailure( GetFramework()->GetDevice()->CreateTexture3D(&Tex3ddesc, 0, reinterpret_cast<ID3D11Texture3D**>(&m_pTestResource) ),
			"Couldn't create default test tex3d");

		Tex3ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ; 
		Tex3ddesc.Usage = D3D11_USAGE_STAGING;
		Tex3ddesc.BindFlags = 0;
		Tex3ddesc.MiscFlags = 0;

		ThrowTRFailure( GetFramework()->GetDevice()->CreateTexture3D(&Tex3ddesc, 0, &m_pStagingTex3D),
			"Couldn't create staging test tex3d");

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;

		uavd.Format                 = m_UAVFormat; 
		uavd.ViewDimension          = D3D11_UAV_DIMENSION_TEXTURE3D;
		uavd.Texture3D.MipSlice     = 0;
		uavd.Texture3D.FirstWSlice  = 0;
		uavd.Texture3D.WSize        = Tex3ddesc.Depth;

		ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pTestResource, &uavd, & m_pTestUAV),
			"Couldn't create UAV ");
	}
	else
	{
		assert(false);
	}
}

//---------------------------------------------------------------------------------------------------------------------------------
void CMemoryAccessShader5xTest::CleanupResource()
{
	m_pTestResource = 0; //releases
	m_pStagingBuffer = 0; //releases
	m_pStagingTex3D = 0; //releases
	m_pTestUAV = 0; //releases
}

//---------------------------------------------------------------------------------------------------------------------------------
UINT32 * CMemoryAccessShader5xTest::PointToComponent( UINT32 *p, SelectComponent c )
{
    return p + (UINT32) c;
}

//---------------------------------------------------------------------------------------------------------------------------------
int CMemoryAccessShader5xTest::AddressTypeToOffset( AddressType a )
{
    int offset = 0;
    switch(a)
    {
    case Out_PartialLow:    offset = -4;                             break;
	// Note this is +4. This means it is not really testing "partially out". However the test was written in a way that makes this very difficult to control.
    case Out_PartialHigh:   offset = m_BufferSize+4;                 break;	

    case Out_Low:           offset = -20;                            break;
    case Out_High:          offset = m_BufferSize*2;                 break;
    case In_Start:          offset = 0;                              break;
    case In_End:            offset = m_BufferSize-(sizeof(UINT32));  break;
    case In_Middle:         offset = sizeof(UINT32);                 break;
//    case In_MiddleUnaligned:offset = m_BufferSize/2+1;               break;

    default : assert(false); return 0;
    }

    //locations are 
    return offset;
}

//---------------------------------------------------------------------------------------------------------------------------------
bool CMemoryAccessShader5xTest::IsOutOfBoundsOffset( AddressType a)
{
    switch(a)
    {
    case Out_Low:           
    case Out_High:          
    case Out_PartialLow:    
    case Out_PartialHigh:   
//    case In_MiddleUnaligned:
        return true;
    default : return false;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------
// Base class for everything but append/consume
//---------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------
CShader5xTest_Atomic_instruction::CShader5xTest_Atomic_instruction() :
      MemoryAccessInstructions::CMemoryAccessShader5xTest( DXGI_FORMAT_R32G32B32A32_UINT )
{
}

      
//---------------------------------------------------------------------------------------------------------------------------------
void CShader5xTest_Atomic_instruction::InitTestParameters()
{
    CreateTestFactors();
    testfactor::RFactor myFactors;
    testfactor::RFactor mySINTFactors;
    testfactor::RFactor myUINTFactors;

    if ( 2 == NumSources() )
    {
        mySINTFactors = m_SINTFactors2Sources;
        myUINTFactors = m_UINTFactors2Sources;
    }
    else
    {
        mySINTFactors = m_SINTFactors;
        myUINTFactors = m_UINTFactors;
    }

    if ( CanOperateOnSINT() && CanOperateOnUINT() )
    {
        myFactors  = mySINTFactors + myUINTFactors;
    }
    else if ( CanOperateOnUINT()  )
    {
        myFactors = myUINTFactors;
    }
    else if ( CanOperateOnSINT() )
    {
        myFactors = mySINTFactors;
    }
    else
    {
        assert(false && "Please override one or both of CanOperateOnSINT and CanOperateOnUINT");
    }

    SetRootTestFactor( myFactors ); 

/*
switch( NumSources() )
    {
    case 1 : SetRootTestFactor( SetRootTestFactormyFactors + m_AddressOut ); break;
    case 2 : SetRootTestFactor( ( myFactors + m_AddressOut ) * m_SelectSource2 ); break;
    default: assert(false); break;
    }
    */

	AddPriorityPatternFilter(FilterEqual<AddressType>(_T("AddressType"), In_Middle), WeightFactorVeryLarge);

	// Out-of-bounds addressing is not pri 1
	AddPriorityPatternFilter(FilterEqual<AddressType>(_T("AddressType"), Out_Low), WeightFactorVerySmall);
	AddPriorityPatternFilter(FilterEqual<AddressType>(_T("AddressType"), Out_High), WeightFactorVerySmall);
	AddPriorityPatternFilter(FilterEqual<AddressType>(_T("AddressType"), Out_PartialLow), WeightFactorVerySmall);
	AddPriorityPatternFilter(FilterEqual<AddressType>(_T("AddressType"), Out_PartialHigh), WeightFactorVerySmall);

	// Swizzling and masks are not pri 1
	AddPriorityPatternFilter(FilterEqual<SelectComponent>(_T("SelectSourceComponent"), SelectX), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SelectComponent>(_T("SelectSource2Component"), SelectX), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SelectComponent>(_T("DestMask"), SelectX), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<ViewType>(_T("ViewType"), Structured), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<ViewType>(_T("ViewType"), Raw), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<ViewType>(_T("ViewType"), Typed), WeightFactorNormal);	// Not as important as structured and raw
	AddPriorityPatternFilter(FilterEqual<CounterUAVType>(_T("CounterUAVType"), Append), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<CounterUAVType>(_T("CounterUAVType"), Count), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumComponents"), 4), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumComponents"), 1), WeightFactorSmall);

	if( GetInstruction() == _T("atomic_cmp_store") ||
		GetInstruction() == _T("imm_atomic_cmp_exch"))
	{
		DeclarePriorityLevelByPriorityValue( 1, 6.75f );
		DeclarePriorityLevelByPriorityValue( 2, 6.0f );
	}
	else
	{
		DeclarePriorityLevelByPriorityValue( 1, 4.5f );
		DeclarePriorityLevelByPriorityValue( 2, 3.0f );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_Atomic_instruction::Setup()
{
	if ( GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
    {
        return RESULT_SKIP;
    }

    TEST_RESULT tRes = RESULT_PASS;
    try
    {
        tRes = CShader5xTest::Setup();
    }
    catch(TRException &exc)
    {
        CleanupTestCase();

	    tRes = exc.GetResult();
	    exc.Prepend("[CInteractionBufferTest::Setup()] ");
	    if (tRes != RESULT_PASS)
        {
		    WriteToLog( exc.GetError().c_str() );
            return tRes;
        }
    }

    return tRes;
}

//---------------------------------------------------------------------------------------------------------------------------------
void CShader5xTest_Atomic_instruction::CleanupTestCase()
{
    CleanupResource();
}

//---------------------------------------------------------------------------------------------------------------------------------
std::string  CMemoryAccessShader5xTest::SelectComponentToString( SelectComponent c )
{
    switch ( c )
    {
    case SelectX: return "x"; 
    case SelectY: return "y"; 
    case SelectZ: return "z"; 
    case SelectW: return "w"; 
    default : assert(false);
    }
    return "";
}

//---------------------------------------------------------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_Atomic_instruction::SetupTestCase()
{
    TEST_RESULT tRes = RESULT_PASS;
    try
    {
        CreateResourceBasedOnFactors(m_BufferSize,sizeof(UINT32));
        // build create and set shader

        //            struct CBLayout
        //            {
        //                UINT32 Source0[4];
        //                UINT32 Source1[4];
        //                UINT32 Dest[4];
        //                UINT32 Offset;
        //            };
        // mov r0,cb.0
        // mov r1,cb.1
        // <instruction> <dest reg r2> u0/g0, <destaddress>.swizzle, r0.select, <r1.select>
        // mov u1+1, r2 //output return value
        // mov u1+0, u0/g0 //output modified value
        // reg content
        // r0   source 0
        // r1   source 1
        // r2   return value
        // r3   temp for setting up dest address/offset
	    AssemblyHelper asmHelper(8, 6);

        tstring MemoryType;
        tstring ViewType;
        tstring TargetRegister;
        tstring StructuredByteStrideOrRawByteCount;

        //-------------------------------------------------------------------------------------------------------------------------
        //decl for test UAV (register 1 always)
        switch( m_MemoryType )
        {
        case UAV:  MemoryType = "uav";  TargetRegister = "u1"; break;
        case TGSM: MemoryType = "tgsm"; TargetRegister = "g0"; break;
        default:
            ThrowTRFailure(E_FAIL,"Unrecognized memory type enum");
        }

        switch( m_ViewType )
        {
        case Raw:
            ViewType = "raw";
            if ( m_MemoryType == TGSM )
            {   
                StructuredByteStrideOrRawByteCount = FormatString(", %d", m_BufferSize);   
            }
            break;
        case Structured:
            ViewType = "structured";   
            if ( m_MemoryType == TGSM )
            {
                StructuredByteStrideOrRawByteCount = ", 4, 3";    //stride, struct count
            }
            else
            {
                StructuredByteStrideOrRawByteCount = ", 4";    //stride
            }
            break;
        case Typed:
            if ( m_NumComponents == 4 )
            {
                if ( m_UAVFormat == DXGI_FORMAT_R32_UINT )
                    ViewType = "typed_texture3d (uint,uint,uint,uint) "; 
                else if ( m_UAVFormat == DXGI_FORMAT_R32_SINT )
                    ViewType = "typed_texture3d (sint,sint,sint,sint) "; 
                else
                    assert(false);

            }
            else
            {
                if ( m_UAVFormat == DXGI_FORMAT_R32_UINT )
                    ViewType = "typed_buffer (uint,uint,uint,uint) "; 
                else if ( m_UAVFormat == DXGI_FORMAT_R32_SINT )
                    ViewType = "typed_buffer (sint,sint,sint,sint) "; 
                else
                    assert(false);
            }
            TargetRegister = "u1";  
            break;
        default:
            ThrowTRFailure(E_FAIL,"Unrecognized view type enum");
        }

        asmHelper.SetCustomDecl( 0, FormatString( "dcl_%s_%s %s%s", 
            MemoryType.c_str(),
            ViewType.c_str(),
            TargetRegister.c_str(),
            StructuredByteStrideOrRawByteCount.c_str()
            ) );

        //-------------------------------------------------------------------------------------------------------------------------
        // move from CBs to temps r0 and r1 and r2
        tstring src1;
        AssemblyHelper::Swizzle AllX = {AssemblyHelper::x,AssemblyHelper::x,AssemblyHelper::x,AssemblyHelper::x};
        AssemblyHelper::Swizzle AllY = {AssemblyHelper::y,AssemblyHelper::y,AssemblyHelper::y,AssemblyHelper::y};

	    asmHelper.SetupMovToTempFromCB(0, AssemblyHelper::xyzw, 0, AllX);
        if ( NumSources() > 1 )
        {
	        asmHelper.SetupMovToTempFromCB(1, AssemblyHelper::xyzw, 0, AllY);
            src1 = ", r1." + SelectComponentToString(m_SelectSource2Component);
        }

        //-------------------------------------------------------------------------------------------------------------------------
        // IN the UAV case, initial data is supplied by CopyResource. In the TGSM case, the data in that UAV, which is also bound
        // in the 
        tstring SetupDestValue;
        tstring ReturnDestValue;
        if ( !IsOutOfBoundsOffset( m_AddressType ) )
        {
            if ( m_MemoryType == TGSM )
            {
                asmHelper.SetCustomDecl( 1, "dcl_uav_typed_buffer (uint,uint,uint,uint) u1");
                SetupDestValue = 
                    "ld_uav_typed r3.x, l(0,0,0,0), u1.x\n"
                    "ld_uav_typed r3.y, l(1,1,1,1), u1.x\n"
                    "ld_uav_typed r3.z, l(2,2,2,2), u1.x\n";
                switch( m_ViewType )
                {
                case Raw:
                    SetupDestValue = SetupDestValue + 
                        "store_raw g0.xyz, l(0,0,0,0), r3\n";
                    ReturnDestValue = 
                        "\nld_raw r3.xyz, l(0,0,0,0), g0\n";
                    break;
                case Structured:  
                    SetupDestValue = SetupDestValue + 
                        "store_structured g0, l(0,0,0,0), l(0,0,0,0), r3.x\n"
                        "store_structured g0, l(1,1,1,1), l(0,0,0,0), r3.y\n"
                        "store_structured g0, l(2,2,2,2), l(0,0,0,0), r3.z\n";
                    ReturnDestValue = 
                        "\nld_structured r3.x, l(0,0,0,0), l(0,0,0,0), g0.x\n"
                        "ld_structured r3.y, l(1,1,1,1), l(0,0,0,0), g0.x\n"
                        "ld_structured r3.z, l(2,2,2,2), l(0,0,0,0), g0.x\n";
                    break;
                }
                ReturnDestValue = ReturnDestValue +
                    "store_uav_typed u1, l(0,0,0,0), r3.x\n"
                    "store_uav_typed u1, l(1,1,1,1), r3.y\n"
                    "store_uav_typed u1, l(2,2,2,2), r3.z\n";
            }
        }

        //-------------------------------------------------------------------------------------------------------------------------
        // The dest displacement from the beginning of the target memory is either
        //      - a byte address for raw 
        //      - an element coordinate for typed
        //      - a structure index and an offset for structured. 
        // For buffers, the address is in the first component and the offset, if needed, is in the second. 
        // For Tex3d, there are three components of address and there is no offset

        //AddressTypeToOffset returns a BYTE offset. This has to be converted to one of the above (address, coordinate or index)
        //Since the structure size is the same as the typed size (by design of this test), there are only two cases: address or coord

        //Offset will only be used if Structured (has no meaning otherwise). Offset is set to "unused" by setting to zero:
        int Address = AddressTypeToOffset(m_AddressType );
        int Offset = (m_ViewType == Structured) ? 
            (AddressTypeToOffset(m_AddressType ) % sizeof(UINT32) ) :       
            0;

        if ( m_ViewType != Raw )
        {
            //convert byte address to coordinate for UINT32 typed data:
            Address /= sizeof(UINT32);
        }

        tstring DestAddress = FormatString("l(%u,%u,%u,%u)", 
            Address,
            m_NumComponents == 1 ? Offset : Address,
            Address,
            0
            );

        tstring DestRegister;
        if ( ReturnsValue() )
        {
            DestRegister = FormatString("r2.%s, ",SelectComponentToString(m_DestMask).c_str() );
        }

		tstring instructionCode = "";
		if( m_ShaderStage == D3D_SHADER_STAGE_DOMAIN )
		{
			// The domain shader will execute twice for 1 point due to the isoline domain. Thus we only perform the operation the second time by checking if the domain.x != 0f. It will be 1.0f the second time.
			// execute the instruction
    		instructionCode = FormatString(
				"%s\n" //setup dest value
				"mov r2,l(0,0,0,0)\n" //initialize dest register

				"if_nz vDomain.x\n"	// conditional

				"%s " //instruction
				"%s" //dest register
				"%s, " //dst memory
				"%s, " //dstAddress
				"r0.%s " //src0.swizzle
				"%s\n"  // <, src1.swizzle>

				"endif\n"	// end conditional

				"%s", //return dest value
				SetupDestValue.c_str(),
				GetInstruction().c_str(),
				DestRegister.c_str(),
				TargetRegister.c_str(),
				DestAddress.c_str(),
				SelectComponentToString(m_SelectSourceComponent).c_str(),
				src1.c_str(),
				ReturnDestValue.c_str());
		}
		else
		{
			// execute the instruction
    		instructionCode = FormatString(
				"%s\n" //setup dest value
				"mov r2,l(0,0,0,0)\n" //initialize dest register
				"%s " //instruction
				"%s" //dest register
				"%s, " //dst memory
				"%s, " //dstAddress
				"r0.%s " //src0.swizzle
				"%s\n"  // <, src1.swizzle>
				"%s", //return dest value
				SetupDestValue.c_str(),
				GetInstruction().c_str(),
				DestRegister.c_str(),
				TargetRegister.c_str(),
				DestAddress.c_str(),
				SelectComponentToString(m_SelectSourceComponent).c_str(),
				src1.c_str(),
				ReturnDestValue.c_str());
		}

        //-------------------------------------------------------------------------------------------------------------------------
        // report return value

        UINT OutputReg = 0;// m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ? 0 : 2;
        asmHelper.SetOutputRegMask( OutputReg, AssemblyHelper::xyzw );
        asmHelper.SetupMovToOutputFromTemp(OutputReg /*outputRegIdx*/, AssemblyHelper::xyzw, 2 /* tempRegIdx */, AssemblyHelper::swizXYZW); //(m_SelectSourceComponent));

		WindowsTest::CHandle<ID3D10Blob> pShaderBuf;

        ThrowTRFailure( asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
		    m_ShaderStage, g_TestApp.m_D3DOptions, 
		    instructionCode, &m_pRTView, &m_pRTUAView, 1, &pShaderBuf,
		    &m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS), "Couldn't set up assembly helper.");

        m_MadeShader = asmHelper.RetrieveMadeShader();


        ID3D11UnorderedAccessView * pNulls[] = { 0,0,0,0};

        switch(m_ShaderStage)
        {
        case D3D_SHADER_STAGE_COMPUTE:
            GetEffectiveContext()->CSSetUnorderedAccessViews( 1, 1, &m_pTestUAV, NULL );
            break;
        case D3D_SHADER_STAGE_PIXEL:
		case D3D_SHADER_STAGE_VERTEX:
		case D3D_SHADER_STAGE_GEOMETRY:
		case D3D_SHADER_STAGE_HULL:
		case D3D_SHADER_STAGE_DOMAIN:
            GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, NULL, NULL, 1, 1, &m_pTestUAV, NULL);
            break;
        }
    }
    catch(TRException &exc)
    {
        CleanupTestCase();

	    tRes = exc.GetResult();
	    exc.Prepend("[CInteractionBufferTest::Setup()] ");
	    if (tRes != RESULT_PASS)
        {
		    WriteToLog( exc.GetError().c_str() );
            return tRes;
        }
    }
    return tRes;
}


//---------------------------------------------------------------------------------------------------------------------------------
UINT32 CShader5xTest_Atomic_instruction::m_TestValues[] = 
{
    0, 1, 2, 3,
    -4, -3, -2, -1,
    0xaaaaaaaa, 0x55555555, 0xabadcafe, 0xbadf00d
};


//---------------------------------------------------------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_Atomic_instruction::ExecuteTestCase()
{
    TEST_RESULT tRes = RESULT_PASS;
	bool undefinedUAV = false;
	bool undefinedTGSM = false;
    try
    {
        for(UINT destVal = 0; destVal < ARRAYSIZE(m_TestValues); ++destVal)
        {
            for(UINT src0Val = 0; src0Val < ARRAYSIZE(m_TestValues); ++src0Val)
            {
                UINT Source1Count = (NumSources() < 2)? 1 : ARRAYSIZE(m_TestValues);

                for(UINT src1Val = 0; src1Val < Source1Count ; ++src1Val)
                {
                    UINT32 ClearColorU[4] = {0xdeadbeef, 0xdeadbeef, 0xdeadbeef, 0xdeadbeef};
                    if ( m_MemoryType == UAV )
                    {
	                    GetEffectiveContext()->ClearUnorderedAccessViewUint(m_pTestUAV, ClearColorU);
                    }

                    //initialize entirety of test buffer/tex3d to starting value:
                    if ( 1 == m_NumComponents )
                    {
                        {
                            CMapResource map( this, m_pStagingBuffer );
    
                            for( UINT iDWORD = 0; iDWORD < m_BufferSize / sizeof(UINT32); ++ iDWORD )
                            {
                                * map.Uint32( iDWORD ) = m_TestValues[destVal];
                            }
                        }
    	                GetEffectiveContext()->CopyResource( m_pTestResource, m_pStagingBuffer );
                    }
                    else
                    {
                        assert( m_MemoryType == UAV );
                        {
                            CMapResource map( this, m_pStagingTex3D );

                            for( UINT iDWORDx = 0; iDWORDx < m_BufferSize / sizeof(UINT32); ++ iDWORDx )
                            for( UINT iDWORDy = 0; iDWORDy < m_BufferSize / sizeof(UINT32); ++ iDWORDy )
                            for( UINT iDWORDz = 0; iDWORDz < m_BufferSize / sizeof(UINT32); ++ iDWORDz )
                            {
                                * map.Uint32( iDWORDx, iDWORDy, iDWORDz ) = m_TestValues[destVal];
                            }
                        }
    	                GetEffectiveContext()->CopyResource( m_pTestResource, m_pStagingTex3D );
                    }

                    // place values into CB
		            D3D11_MAPPED_SUBRESOURCE mappedResource;
                    ThrowTRFailure( GetEffectiveContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ), 
                        "Couldn't map const buffer to write test values");

                    struct CBLayout
                    {
                        UINT32 Source0;
                        UINT32 Source1;
                        UINT32 Offset;
                    };
    
                    CBLayout * pCB = reinterpret_cast<CBLayout*>(mappedResource.pData);
                    
                    // source 0 goes at element 0, source 1 goes in at element 1
		            memcpy( & pCB->Source0 , m_TestValues + src0Val, sizeof(pCB->Source0));
		            memcpy( & pCB->Source1 , m_TestValues + src1Val, sizeof(pCB->Source1));

                    pCB->Offset = AddressTypeToOffset( m_AddressType );

		            GetEffectiveContext()->Unmap( m_pConstBuffer, 0 );

                    // execute shader
	                if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	                {
		                GetEffectiveContext()->Dispatch( 1, 1, 1 );
	                }
	                else
	                {
	                    FLOAT ClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		                GetEffectiveContext()->Draw( 1, 0 );
	                }

	                ThrowTRFailure( ExecuteEffectiveContext(), "ExecuteEffectiveContext() failed");

                    //      execute sim code
                    UINT32 Dest = m_TestValues[destVal];
                    if ( IsOutOfBoundsOffset(m_AddressType) )
                    {
                        Dest = 0;
                    }
                    UINT32 Source0 = m_TestValues[src0Val];
                    UINT32 Source1 = 0;
                    if ( NumSources() == 2 )
                    {
                        Source1 = m_TestValues[src1Val];
                    }
                    UINT32 SimulatedReg = CalcResult( Source0, Source1, Dest );

                    //If this was an out-of-bounds tests, then all we have to check is that the entire dest resource is unaffected.
                    //We can exploit the same code to check if any value other than the expected value was altered.

					// If the UAV is structured and the byte offset into causes an out-of-bounds, the UAV is undefined.
					if( m_MemoryType == UAV && m_ViewType == Structured )
					{
						// These are the calculations used in SetupTestCase()
						// Note: Since the structureStride of the test is only one 32-bit UINT it cannot test out-of-bounds from the stride. This is a test limitation.
						// The test will need to be updated to fix this (Ideally, the offset should be a test parameter that is cycled when using a structured buffer).
						const int structureStride = sizeof(UINT32); // Each struct is one 32-bit UINT
						const int index = AddressTypeToOffset( m_AddressType ) / structureStride;
						const int offset = AddressTypeToOffset( m_AddressType ) % structureStride; // Note: The offset here will NEVER be out-of-bounds. This is a test limitation.
						const int numStructElementsInBuffer = m_BufferSize / structureStride;	
						const bool indexIsInBounds = index >= 0 && index < numStructElementsInBuffer;
						const bool offsetIsInBounds = offset >= 0 && offset < structureStride;

						if( indexIsInBounds && !offsetIsInBounds )
						{
							undefinedUAV = true;
						}
					}

					if( m_MemoryType == TGSM && IsOutOfBoundsOffset(m_AddressType))
					{
						undefinedTGSM = true;
					}

					// Only check the UAV if the memory is not undefined
					HRESULT hrBadMemory = S_OK;
					if( !undefinedUAV && !undefinedTGSM )
					{
						if ( 1 == m_NumComponents )
						{
    						GetEffectiveContext()->CopyResource( m_pStagingBuffer, m_pTestResource );
							CMapResource map( this, m_pStagingBuffer );

							for( UINT iDWORD = 0; iDWORD < m_BufferSize / sizeof(UINT32); ++ iDWORD )
							{
								UINT32 Actual = * map.Uint32( iDWORD );

								if ((!IsOutOfBoundsOffset( m_AddressType )) && 
									iDWORD * sizeof(UINT32) == AddressTypeToOffset( m_AddressType ) )
								{
									if ( Actual != Dest )
									{
										hrBadMemory = E_FAIL;
										WriteToLog("ERROR: The instruction did not write into the UAV at the expected location (offset %d). "
											"Expected %08x, found %08x", AddressTypeToOffset( m_AddressType ), Dest, Actual );
										WriteToLog("--Instruction is %s, dest memory starts as 0x%08x(%d), with src0 = 0x%08x(%d), src1 = 0x%08x(%d)",
											GetInstruction().c_str(),
											m_TestValues[destVal],
											m_TestValues[destVal],
											Source0, Source0,
											Source1, Source1);
									}
								}
								else if ( Actual != m_TestValues[destVal] )
								{
									hrBadMemory = E_FAIL;
									WriteToLog("ERROR: The instruction (or some other part of the shader under test) erroneously wrote an unintended location. UINT32 offset %d was modified\n"
										"--The modified data is 0x%08x(%d).",  iDWORD , Actual, Actual);
								}
							}
						}
						else if ( 4 == m_NumComponents )
						{
    						GetEffectiveContext()->CopyResource( m_pStagingTex3D, m_pTestResource );
							CMapResource map( this, m_pStagingTex3D );

							for( UINT iDWORDx = 0; iDWORDx < m_BufferSize / sizeof(UINT32); ++ iDWORDx )
							for( UINT iDWORDy = 0; iDWORDy < m_BufferSize / sizeof(UINT32); ++ iDWORDy )
							for( UINT iDWORDz = 0; iDWORDz < m_BufferSize / sizeof(UINT32); ++ iDWORDz )
							{
								UINT32 Actual = * map.Uint32( iDWORDx, iDWORDy, iDWORDz );

								if ( m_MemoryType == UAV && 
									(!IsOutOfBoundsOffset( m_AddressType )) && 
									iDWORDx == iDWORDy &&
									iDWORDy == iDWORDz &&
									iDWORDx * sizeof(UINT32) == AddressTypeToOffset( m_AddressType ) )
								{
									if ( Actual != Dest )
									{
										hrBadMemory = E_FAIL;
										WriteToLog("ERROR: The instruction did not write into the UAV at the expected location (%d,%d,%d). "
											"Expected %08x, found %08x", iDWORDx, iDWORDy, iDWORDz, Dest, Actual );
										WriteToLog("--Instruction is %s, dest memory starts as 0x%08x(%d), with src0 = 0x%08x(%d), src1 = 0x%08x(%d)",
											GetInstruction().c_str(),
											m_TestValues[destVal],
											m_TestValues[destVal],
											Source0, Source0,
											Source1, Source1);
									}
								}
								else if ( Actual != m_TestValues[destVal] )
								{
									hrBadMemory = E_FAIL;
									WriteToLog("ERROR: The instruction (or some other part of the shader under test) erroneously wrote an unintended location. location (%d,%d,%d) was modified\n"
										"--The modified data is 0x%08x(%d).",  iDWORDx, iDWORDy, iDWORDz , Actual, Actual);
								}
							}
						}
						else
						{
							assert(false && "1 or 4 components please.");
						}
					}

                    //      compare and fail
                    GetImmediateContext()->CopyResource( m_pRTTexture2DCopy, m_pRTTexture2D);
	                UINT32 ActualRegElement[4];
                	ThrowTRFailure( MapAndRead(GetImmediateContext(), m_pRTTexture2DCopy, ActualRegElement ) , "Couldn't retrieve return value");

                    UINT32 ActualReg = * PointToComponent( ActualRegElement, m_DestMask) ;

                    HRESULT hrBadReturnValue = S_OK;
#if 0
                    if ( ActualDestMem != Dest )
                    {
                        hrBadReturnValue = E_FAIL;
                        WriteToLog("ERROR: Failed write to memory. Expected %08x, found %08x\n", Dest, ActualDestMem);
                        if (ActualDestMem == ClearColorU[0] )
                        {
                            WriteToLog("--This value indicates that the shader didn't write its output to the intermediate resource used "
                            "to read back the computed result. There is no information about the success or failure of the instruction under test. ");
                        }
                        if (ActualDestMem == m_TestValues[destVal/4] )
                        {
                            WriteToLog("--This value indicates that the instruction under test didn't write the expected memory value at all.");
                        }
                    }
#endif
                    if ( ReturnsValue() && (ActualReg != SimulatedReg) && !IsOutOfBoundsOffset( m_AddressType ))
                    {
						// This check should only occur if the address is not out-of-bounds. 
						// Out of bounds addressing on u# or g# causes an undefined result to be returned to the shader in dst0.

                        hrBadReturnValue = E_FAIL;
                        WriteToLog("ERROR: Failed instruction return value. Expected 0x%08x(%d), found 0x%08x(%d)\n", SimulatedReg, SimulatedReg, ActualReg, ActualReg);
                        if (ActualReg == ClearColorU[0] )
                        {
                            WriteToLog("--This value indicates that the shader didn't write the instruction's return value to the intermediate resource used "
                            "to read back the computed result. There is no information about the success or failure of the instruction under test. ");
                        }
                    }
                    if ( FAILED( hrBadMemory ) || FAILED( hrBadReturnValue ) )
                    {
                        WriteToLog("--CB 0 was loaded with {%08x(%d), %08x(%d), *, *} in the zeroth element, dest memory was 0x%08x(%d)",
                            m_TestValues[src0Val], m_TestValues[src0Val],
                            m_TestValues[src1Val], m_TestValues[src1Val],
                            m_TestValues[destVal],m_TestValues[destVal]
                            );
                        WriteToLog("--Shader was:\n%s", m_MadeShader.c_str());
                    }
	                ThrowTRFailure( hrBadMemory, "Corrupted buffer.");
	                ThrowTRFailure( hrBadReturnValue, "Bad written or returned value from instruction (or elsewhere in shader).");
                }
            }
        }
    }
    catch(TRException &exc)
    {
	    tRes = exc.GetResult();
	    exc.Prepend("[CInteractionBufferTest::Setup()] ");
	    if (tRes != RESULT_PASS)
        {
		    WriteToLog( exc.GetError().c_str() );
            return tRes;
        }
    }
    return tRes;
}

//---------------------------------------------------------------------------------------------------------------------------------
// Alloc/Consume test
//---------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------
CImmAtomicAllocConsume::CImmAtomicAllocConsume() : CMemoryAccessShader5xTest( DXGI_FORMAT_R32G32B32A32_UINT )
{
}

//---------------------------------------------------------------------------------------------------------------------------------
TEST_RESULT CImmAtomicAllocConsume::Setup()
{
	if ( GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
    {
        return RESULT_SKIP;
    }
    return CMemoryAccessShader5xTest::Setup();
}

//---------------------------------------------------------------------------------------------------------------------------------
void CImmAtomicAllocConsume::InitTestParameters()
{
	AddParameter< UINT >( _T("StructureCount"), &m_StructCount );
	AddParameter< UINT >( _T("StructureComponentCount"),  &m_StructureComponentCount );
	
	testfactor::RFactor rStructureComponentCount = AddParameterValue< UINT >( _T("StructureComponentCount"), 1 ) + 
												   AddParameterValue< UINT >( _T("StructureComponentCount"), 2 ) + 
												   AddParameterValue< UINT >( _T("StructureComponentCount"), 3 ) + 
												   AddParameterValue< UINT >( _T("StructureComponentCount"), 4 );

	testfactor::RFactor rStructureCount = AddParameterValue< UINT >( _T("StructureCount"), 15 ) + 
										  AddParameterValue< UINT >( _T("StructureCount"), 257 );

	CreateTestFactors();

	SetRootTestFactor( (m_AppendFactor + m_ConsumeFactor) * rStructureComponentCount * rStructureCount );

	AddPriorityPatternFilter(FilterEqual<CounterOperation>(_T("CounterOperation"), Decrement), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<CounterOperation>(_T("CounterOperation"), Increment), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

//---------------------------------------------------------------------------------------------------------------------------------
TEST_RESULT CImmAtomicAllocConsume::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		const UINT structureSize = sizeof(UINT) * m_StructureComponentCount;
		const UINT bufferByteSize = structureSize * m_StructCount;

		CreateResourceBasedOnFactors(bufferByteSize, structureSize);
	}
	catch(TRException &exc)
	{
        CleanupResource();
		tRes = exc.GetResult();
		exc.Prepend("[CImmAtomicAllocConsume::SetupTestCase()] ");
		if (tRes != RESULT_PASS)
		{
			WriteToLog( exc.GetError().c_str() );
		}
	}

	return tRes;
}

//---------------------------------------------------------------------------------------------------------------------------------
void CImmAtomicAllocConsume::CleanupTestCase()
{
	CleanupResource();
}

//---------------------------------------------------------------------------------------------------------------------------------
TEST_RESULT CImmAtomicAllocConsume::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;

	ID3D10Blob* pShaderBuf = NULL;

	UINT initialCount = 0;

	UINT expectedResults[4] = {0,0,0,0};
	UINT expectedWriteIndex = 0;

	tstring instructionCode;

	AssemblyHelper asmHelper( 0, 1 );

	const TCHAR* opcode = _T("");

	switch( m_DestMask ) 
	{
	case SelectX: expectedWriteIndex = 0; break;
	case SelectY: expectedWriteIndex = 1; break;
	case SelectZ: expectedWriteIndex = 2; break;
	case SelectW: expectedWriteIndex = 3; break;
	default:
		WriteToLog( _T("Unknown dest mask in ExecuteTestCase") );
		tr = RESULT_FAIL;
		goto testDone;
	}

	switch( m_CounterOperation ) 
	{
	case Decrement:
		initialCount = 27;
		expectedResults[ expectedWriteIndex ] = initialCount - 3; // imm_atomic_consume returns the "new" value
		opcode = _T("imm_atomic_consume");
		break;

	case Increment:
		initialCount = 27;
		expectedResults[ expectedWriteIndex ] = initialCount + 2; // imm_atomic_alloc returns the "original" value
		opcode = _T("imm_atomic_alloc");
		break;

	case Underflow:
		initialCount = 0;
		expectedResults[ expectedWriteIndex ] = initialCount - 3; // imm_atomic_consume returns the "new" value
		opcode = _T("imm_atomic_consume");
		break;

	case Overflow:
		initialCount = UINT_MAX - 1;
		expectedResults[ expectedWriteIndex ] = initialCount + 2; // imm_atomic_alloc returns the "original" value
		opcode = _T("imm_atomic_alloc");
		break;

	default:
		WriteToLog( _T("Unknown counter operation in ExecuteTestCase") );
		tr = RESULT_FAIL;
		goto testDone;
	}

	// Create shader
	const TCHAR* dclSuffix = (Count == m_CounterUAVType) ? _T("_opc") : _T(""); // COUNT UAVs require the _opc flag

	asmHelper.SetCustomUAVDecl(1, FormatString( _T("dcl_uav_structured%s u%d, %d"), dclSuffix, 1, sizeof(UINT) * m_StructureComponentCount ) );
	asmHelper.SetOutputRegMask( 0, AssemblyHelper::xyzw );
	asmHelper.SetupMovToTempFromLiteral( 0, AssemblyHelper::xyzw, tstring("l( 0, 0, 0, 0 )") );
	asmHelper.SetupMovToOutputFromTemp( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	
	instructionCode = "";
	if( m_ShaderStage == D3D_SHADER_STAGE_DOMAIN )
	{
		// The domain shader will execute twice due to the isoline domain. So we only perform the operation when one the last exection when vDomain.x = 1.0f
		instructionCode += "if_nz vDomain.x\n";
	}

	// Execute the instruction 3 times to ensure that the changed counter value is read, and overflow works
	instructionCode += FormatString( _T("%s r0.%s, u1\n%s r0.%s, u1\n%s r0.%s, u1"), 
									opcode,
									SelectComponentToMaskString( m_DestMask ),
									opcode,
									SelectComponentToMaskString( m_DestMask ),
									opcode,
									SelectComponentToMaskString( m_DestMask ));

	if( m_ShaderStage == D3D_SHADER_STAGE_DOMAIN )
	{
		instructionCode += "\nendif\n";
	}

	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(),
											   GetEffectiveContext(),
											   this,
											   m_ShaderStage,
											   g_TestApp.m_D3DOptions,
											   instructionCode,
											   &m_pRTView,
											   &m_pRTUAView,
											   1,
											   &pShaderBuf,
											   &m_pVS,
											   &m_pGS,
											   &m_pPS,
											   &m_pHS,
											   &m_pDS,
											   &m_pCS);
	if( FAILED( hr ) )
	{
		WriteToLog( _T("SetupPipelineWithShaderCode() failed, hr = 0x%s"), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto testDone;
	}
											   
	// Bind UAV to pipeline & execute the shader
	switch( m_ShaderStage )
	{
	case D3D_SHADER_STAGE_PIXEL:
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
		GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews(1, &m_pRTView, NULL, 1, 1, &m_pTestUAV, &initialCount);
		GetEffectiveContext()->Draw( 1, 0 );    
		break;

	case D3D_SHADER_STAGE_COMPUTE:
		{
			UINT initialCounts[2] = {0, initialCount};

			ID3D11UnorderedAccessView* pUAVArray[2] = { m_pRTUAView, m_pTestUAV };
			GetEffectiveContext()->CSSetUnorderedAccessViews(0, 2, pUAVArray, initialCounts);
			GetEffectiveContext()->Dispatch( 1, 1, 1 ); 
		}
		break;

	default:
		WriteToLog( _T("Unknown shader stage in ExecuteTestCase") );
		tr = RESULT_FAIL;
		goto testDone;
	}
		
	GetEffectiveContext()->CopyResource( m_pRTTexture2DCopy, m_pRTTexture2D );

	hr = ExecuteEffectiveContext();
	if( FAILED( hr ) )
	{
		WriteToLog( _T("ExecuteEffectiveContext() failed, hr = 0x%s"), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto testDone;
	}

	// Verify executed results
	UINT results[4];
	hr = MapAndRead( GetImmediateContext(), m_pRTTexture2DCopy, results );
	if( FAILED ( hr ) )
	{
		WriteToLog( _T("Failed to map staging buffer for read, hr = 0x%s"), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto testDone;
	}

	for( UINT i = 0; i < 4; i++ )
	{
		UINT expected = expectedResults[i];
		UINT actual = results[i];

		if( expected != actual )
		{
			WriteToLog( _T("Unexpected result found.  Component = %u, expected = %u, actual = %u\n"), i, expected, actual );
			tr = RESULT_FAIL;
			goto testDone;
		}
	}

testDone:
	SAFE_RELEASE( pShaderBuf );

	return tr;
}
