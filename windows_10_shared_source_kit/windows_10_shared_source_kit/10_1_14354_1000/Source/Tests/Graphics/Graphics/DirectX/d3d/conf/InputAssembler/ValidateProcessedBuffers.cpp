//This is to validate simple cases that should work

//Make sure that indices are respected for IB
//Make sure that buffer bound as a view works
//Make sure that VB works
//Cycle through all valid buffer usages and some sizes
//Try huge buffers
//Bind max number of buffers, with each buffer of a different usage/pool/cpuaccess
//Draw maximum vertices, index and instance count

#include "InputAssembler.h"

void CBufferTest::InitTestParameters()
{
	//Initialize root factors
	CInputAssemblerTest::InitTestParameters();

	//Create the parameters

	CTestCaseParameter< D3D11_USAGE > *pSrcUsage = AddParameter< D3D11_USAGE >( _T("SrcUsage"), &m_SrcResourceFlags.Usage);
	CTestCaseParameter< D3D11_BIND_FLAG > *pSrcBindFlags = AddParameter< D3D11_BIND_FLAG >( _T("SrcBindFlags"), (D3D11_BIND_FLAG*)&m_SrcResourceFlags.BindFlags);
	CTestCaseParameter< D3D11_CPU_ACCESS_FLAG > *pSrcCPUAccessFlags = AddParameter< D3D11_CPU_ACCESS_FLAG >( _T("SrcCPUAccessFlags"), (D3D11_CPU_ACCESS_FLAG*)&m_SrcResourceFlags.CPUAccessFlags);
	CTestCaseParameter< D3D11_RESOURCE_MISC_FLAG > *pSrcMiscFlags = AddParameter< D3D11_RESOURCE_MISC_FLAG >( _T("SrcMiscFlags"), (D3D11_RESOURCE_MISC_FLAG*)&m_SrcResourceFlags.MiscFlags);
	CTestCaseParameter< D3D11_MAP > *pSrcMap = AddParameter< D3D11_MAP >( _T("SrcMap"), &m_SrcResourceFlags.Map);

	CTestCaseParameter< D3D11_USAGE > *pDestUsage = AddParameter< D3D11_USAGE >( _T("DestUsage"), &m_DestResourceFlags.Usage);
	CTestCaseParameter< D3D11_BIND_FLAG > *pDestBindFlags = AddParameter< D3D11_BIND_FLAG >( _T("DestBindFlags"), (D3D11_BIND_FLAG*)&m_DestResourceFlags.BindFlags);
	CTestCaseParameter< D3D11_CPU_ACCESS_FLAG > *pDestCPUAccessFlags = AddParameter< D3D11_CPU_ACCESS_FLAG >( _T("DestCPUAccessFlags"), (D3D11_CPU_ACCESS_FLAG*)&m_DestResourceFlags.CPUAccessFlags);
	CTestCaseParameter< D3D11_RESOURCE_MISC_FLAG > *pDestMiscFlags = AddParameter< D3D11_RESOURCE_MISC_FLAG >( _T("DestMiscFlags"), (D3D11_RESOURCE_MISC_FLAG*)&m_DestResourceFlags.MiscFlags);
	CTestCaseParameter< D3D11_MAP > *pDestMap = AddParameter< D3D11_MAP >( _T("DestMap"), &m_DestResourceFlags.Map);

	CTestCaseParameter< RESOURCE_ACCESS_METHOD > *pAccessMethod = AddParameter( _T("AccessMethod"), &m_AccessMethod);
	CTestCaseParameter< bool > *pDrawSrcBuffer = AddParameter( _T("DrawWithSrc"), &m_bDrawSrcBuffer);
	CTestCaseParameter< bool > *pDrawDestBuffer = AddParameter( _T("DrawWithDest"), &m_bDrawDestBuffer);
	CTestCaseParameter< bool > *pUnbindBuffer = AddParameter( _T("UnbindBuffer"), &m_bUnbindBuffer);

	//Create the value sets

	D3DResourceFlags excludeFlags;
	D3DResourceFlags includeFlags;


	excludeFlags.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
	includeFlags.BindFlags = D3D11_BIND_INDEX_BUFFER | D3D11_BIND_VERTEX_BUFFER;

	m_pVBIBCopySrcValueSet = new CD3DResourceFlagsValueSet(this, D3D11_RESOURCE_DIMENSION_BUFFER, true, false, &includeFlags, &excludeFlags, RAC_COPY_SRC);
	m_pVBIBCopyDestValueSet = new CD3DResourceFlagsValueSet(this, D3D11_RESOURCE_DIMENSION_BUFFER, true, false, &includeFlags, &excludeFlags, RAC_COPY_DEST);
	m_pVBIBCopyRegionSrcValueSet = new CD3DResourceFlagsValueSet(this, D3D11_RESOURCE_DIMENSION_BUFFER, true, false, &includeFlags, &excludeFlags, RAC_COPYREGION_SRC);
	m_pVBIBCopyRegionDestValueSet = new CD3DResourceFlagsValueSet(this, D3D11_RESOURCE_DIMENSION_BUFFER, true, false, &includeFlags, &excludeFlags, RAC_COPYREGION_DEST);
	m_pVBIBUpdateDestValueSet = new CD3DResourceFlagsValueSet(this, D3D11_RESOURCE_DIMENSION_BUFFER, true, false, &includeFlags, &excludeFlags, RAC_UPDATE_DEST);

	excludeFlags.BindFlags = D3D11_BIND_INDEX_BUFFER | D3D11_BIND_VERTEX_BUFFER;
	includeFlags.BindFlags = DO_NOT_FILTER_FLAG;

	m_pCopySrcValueSet = new CD3DResourceFlagsValueSet(this, D3D11_RESOURCE_DIMENSION_BUFFER, true, false, &includeFlags, &excludeFlags, RAC_COPY_SRC);
	m_pCopyDestValueSet = new CD3DResourceFlagsValueSet(this, D3D11_RESOURCE_DIMENSION_BUFFER, true, false, &includeFlags, &excludeFlags, RAC_COPY_DEST);
	m_pCopyRegionSrcValueSet = new CD3DResourceFlagsValueSet(this, D3D11_RESOURCE_DIMENSION_BUFFER, true, false, &includeFlags, &excludeFlags, RAC_COPYREGION_SRC);
	m_pCopyRegionDestValueSet = new CD3DResourceFlagsValueSet(this, D3D11_RESOURCE_DIMENSION_BUFFER, true, false, &includeFlags, &excludeFlags, RAC_COPYREGION_DEST);
	m_pUpdateDestValueSet = new CD3DResourceFlagsValueSet(this, D3D11_RESOURCE_DIMENSION_BUFFER, true, false, &includeFlags, &excludeFlags, RAC_UPDATE_DEST);

	testfactor::RFactor rAccessCopySrc = AddParameterValue(pAccessMethod, RAC_COPY_SRC);
	testfactor::RFactor rAccessCopyDest = AddParameterValue(pAccessMethod, RAC_COPY_DEST);
	testfactor::RFactor rAccessCopyRegionSrc = AddParameterValue(pAccessMethod, RAC_COPYREGION_SRC);
	testfactor::RFactor rAccessCopyRegionDest = AddParameterValue(pAccessMethod, RAC_COPYREGION_DEST);
	testfactor::RFactor rAccessUpdateDest = AddParameterValue(pAccessMethod, RAC_UPDATE_DEST);

	testfactor::RFactor rVBIBCopySrc = m_pVBIBCopySrcValueSet->CreateFactor( pSrcUsage, pSrcBindFlags, pSrcCPUAccessFlags, pSrcMiscFlags, pSrcMap );
	testfactor::RFactor rVBIBCopyDest = m_pVBIBCopyDestValueSet->CreateFactor( pDestUsage, pDestBindFlags, pDestCPUAccessFlags, pDestMiscFlags, pDestMap );
	testfactor::RFactor rVBIBCopyRegionSrc = m_pVBIBCopyRegionSrcValueSet->CreateFactor( pSrcUsage, pSrcBindFlags, pSrcCPUAccessFlags, pSrcMiscFlags, pSrcMap );
	testfactor::RFactor rVBIBCopyRegionDest = m_pVBIBCopyRegionDestValueSet->CreateFactor( pDestUsage, pDestBindFlags, pDestCPUAccessFlags, pDestMiscFlags, pDestMap );
	testfactor::RFactor rVBIBUpdateDest = m_pVBIBUpdateDestValueSet->CreateFactor( pDestUsage, pDestBindFlags, pDestCPUAccessFlags, pDestMiscFlags, pDestMap );

	testfactor::RFactor rBufferCopySrc = m_pCopySrcValueSet->CreateFactor( pSrcUsage, pSrcBindFlags, pSrcCPUAccessFlags, pSrcMiscFlags, pSrcMap );
	testfactor::RFactor rBufferCopyDest = m_pCopyDestValueSet->CreateFactor( pDestUsage, pDestBindFlags, pDestCPUAccessFlags, pDestMiscFlags, pDestMap );
	testfactor::RFactor rBufferCopyRegionSrc = m_pCopyRegionSrcValueSet->CreateFactor( pSrcUsage, pSrcBindFlags, pSrcCPUAccessFlags, pSrcMiscFlags, pSrcMap );
	testfactor::RFactor rBufferCopyRegionDest = m_pCopyRegionDestValueSet->CreateFactor( pDestUsage, pDestBindFlags, pDestCPUAccessFlags, pDestMiscFlags, pDestMap );
	testfactor::RFactor rBufferUpdateDest = m_pUpdateDestValueSet->CreateFactor( pDestUsage, pDestBindFlags, pDestCPUAccessFlags, pDestMiscFlags, pDestMap );

	testfactor::RFactor rCopySrc = rAccessCopySrc * rVBIBCopySrc * rBufferCopyDest;
	testfactor::RFactor rCopyDest = rAccessCopyDest * ((rVBIBCopyDest * rBufferCopySrc) + (rVBIBCopySrc * rVBIBCopyDest));
	testfactor::RFactor rCopyRegionSrc = rAccessCopyRegionSrc * rVBIBCopyRegionSrc * rBufferCopyRegionDest;
	testfactor::RFactor rCopyRegionDest = rAccessCopyRegionDest * ((rVBIBCopyRegionDest * rBufferCopyRegionSrc) + (rVBIBCopyRegionSrc * rVBIBCopyRegionDest));
	//testfactor::RFactor rUpdateSrc = rAccessUpdateSrc * rVBIBUpdateSrc * rBufferUpdateDest;
	testfactor::RFactor rUpdateDest = rAccessUpdateDest * rVBIBUpdateDest;// * rBufferUpdateSrc;//Always use static buffer for src

	testfactor::RFactor rDrawWithSrcTrue = AddParameterValue< bool >( pDrawSrcBuffer, true );
	testfactor::RFactor rDrawWithSrcFalse = AddParameterValue< bool >( pDrawSrcBuffer, false );
	testfactor::RFactor rDrawWithDestTrue = AddParameterValue< bool >( pDrawDestBuffer, true );
	testfactor::RFactor rDrawWithDestFalse = AddParameterValue< bool >( pDrawDestBuffer, false );
	testfactor::RFactor rUnbindBuffer = AddParameterValueSet< bool >( pUnbindBuffer, BoolValueSet() );
	testfactor::RFactor rUnbindBufferFalse = AddParameterValue< bool >( pUnbindBuffer, false );
	testfactor::RFactor rBeforeAccess = (rDrawWithSrcTrue * rDrawWithDestFalse * rUnbindBuffer) +
										(rDrawWithSrcFalse * rDrawWithDestTrue * rUnbindBuffer) +
										(rDrawWithSrcTrue * rDrawWithDestTrue * rUnbindBuffer) +
										(rDrawWithSrcFalse * rDrawWithDestFalse * rUnbindBufferFalse);

	testfactor::RFactor rBeforeAccessNoSrc = (rDrawWithSrcFalse * rDrawWithDestTrue * rUnbindBuffer) +
											(rDrawWithSrcFalse * rDrawWithDestFalse * rUnbindBufferFalse);

	SetParameterDefaultValue< bool >( pDrawSrcBuffer, false );
	SetParameterDefaultValue< bool >( pDrawDestBuffer, false );
	SetParameterDefaultValue< bool >( pUnbindBuffer, false );

	//TODO: Maps
	//TODO: Regions and partial/full access

	testfactor::RFactor rAccessMethod = (rCopySrc + rCopyDest + rCopyRegionSrc + rCopyRegionDest) * rBeforeAccess;
	testfactor::RFactor rAccessMethodNoSrc = (/*rUpdateSrc +*/ rUpdateDest) * rBeforeAccessNoSrc;

	SetRootTestFactor( rAccessMethod + rAccessMethodNoSrc );

	// Use priority patterns to "chip away" non priority 1 test cases
	AddPriorityPatternFilter(	FilterNotEqual<D3D11_USAGE>(pSrcUsage, D3D11_USAGE_DEFAULT) && 
								FilterNotEqual<D3D11_USAGE>(pSrcUsage, D3D11_USAGE_DYNAMIC), 0.5f);

	AddPriorityPatternFilter(	FilterNotEqual<D3D11_USAGE>(pDestUsage, D3D11_USAGE_DEFAULT) && 
								FilterNotEqual<D3D11_USAGE>(pDestUsage, D3D11_USAGE_DYNAMIC), 0.5f);

	AddPriorityPatternFilter(	FilterNotEqual<D3D11_BIND_FLAG>(pSrcBindFlags, D3D11_BIND_VERTEX_BUFFER) &&
								FilterNotEqual<D3D11_BIND_FLAG>(pSrcBindFlags, D3D11_BIND_INDEX_BUFFER) &&
								FilterNotEqual<D3D11_BIND_FLAG>(pSrcBindFlags, (D3D11_BIND_FLAG)(D3D11_BIND_RENDER_TARGET | D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER)) &&
								FilterNotEqual<D3D11_BIND_FLAG>(pSrcBindFlags, (D3D11_BIND_FLAG)(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)), 0.5f);

	 AddPriorityPatternFilter(	FilterNotEqual<D3D11_BIND_FLAG>(pDestBindFlags, D3D11_BIND_VERTEX_BUFFER) &&
								FilterNotEqual<D3D11_BIND_FLAG>(pDestBindFlags, D3D11_BIND_INDEX_BUFFER) &&
								FilterNotEqual<D3D11_BIND_FLAG>(pDestBindFlags, (D3D11_BIND_FLAG)(D3D11_BIND_RENDER_TARGET | D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER)) &&
								FilterNotEqual<D3D11_BIND_FLAG>(pDestBindFlags, (D3D11_BIND_FLAG)(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)), 0.5f);

	DeclarePriorityLevelByPriorityValue( 1, 1.0f );
}

const WORD g_SmallIndices[288] = {	1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15, 
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15,
									1, 0, 2, 1, 3, 5, 4, 7, 8, 9, 6, 10, 12, 4, 11, 15 };

TEST_RESULT CBufferTest::Setup()
{
	TEST_RESULT tr = CInputAssemblerTest::Setup();
	if (tr != RESULT_PASS)
		return tr;

	HRESULT hr;

	m_bVertexID = false;
	m_bInstanceID = false;

	//Set input elements
	m_NumElements = 1;
	ZeroMemory( m_InputElements, sizeof(m_InputElements) );
	m_InputElements[0].InputSlot = 0;
	m_InputElements[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	m_InputElements[0].SemanticName = szSemantic;
	m_InputElements[0].SemanticIndex = 0;
	m_InputElements[0].InstanceDataStepRate = 0;
	m_InputElements[0].AlignedByteOffset = 0;
	m_InputElements[0].Format = DXGI_FORMAT_R32_UINT;

	m_LayoutInfo.InitializeInputElements(m_InputElements, m_NumElements);

	//Set the stride
	m_LayoutInfo.SetLayoutData(m_bVertexID, m_bInstanceID, 16);

	m_LayoutInfo.IBBoundSize = 16;
	m_LayoutInfo.IBOffset = 0;
	m_LayoutInfo.IBStride = sizeof(WORD);
	m_LayoutInfo.slots[0].VBBoundSize = 16 * 16 * 2;
	m_LayoutInfo.slots[0].VBOffset = 0;

	//Setup the shaders and inputlayout
	if (RESULT_PASS != SetupPipeline(false))
	{
		tr = RESULT_FAIL;
		goto TESTFAIL;   
	}
	
	D3D11_BUFFER_DESC bufferDesc;

	//Default VB
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = 16 * 16 * 2;

	if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL, &m_pDefaultVB ) ) )
	{
		WriteToLog( "CreateBuffer() VB failed" );
		tr = RESULT_FAIL;
		goto TESTFAIL;   
	}

	//Default IB
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(WORD) * 16;

	if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL, &m_pDefaultIB ) ) )
	{
		WriteToLog( "CreateBuffer() IB failed" );
		tr = RESULT_FAIL;
		goto TESTFAIL;
	}

	//Result buffer
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.BindFlags = 0;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = 16 * 16 * 2;

	if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL, &m_pResultBuffer ) ) )
	{
		WriteToLog( "CreateBuffer() ResultBuffer failed" );
		tr = RESULT_FAIL;
		goto TESTFAIL;
	}

	tr = RESULT_PASS;
TESTFAIL:

	return tr;
}

void CBufferTest::Cleanup()
{
	CInputAssemblerTest::Cleanup();	

	SAFE_RELEASE( m_pDefaultVB );
	SAFE_RELEASE( m_pDefaultIB );
	SAFE_RELEASE( m_pResultBuffer );
}

void CBufferTest::SetVBIB(ID3D11Buffer *pBuffer, UINT BindFlags)
{
	const UINT stride = 4;
	const UINT offset = 0;

	if ((BindFlags & D3D11_BIND_VERTEX_BUFFER) && (BindFlags & D3D11_BIND_INDEX_BUFFER))
	{
		//use same buffer as VB and IB with different offset
		m_pDeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &stride, &offset);
		m_pDeviceContext->IASetIndexBuffer(pBuffer, DXGI_FORMAT_R16_UINT, 16 * 16 );
	}
	else if (BindFlags & D3D11_BIND_VERTEX_BUFFER)
	{
		m_pDeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &stride, &offset);
		m_pDeviceContext->IASetIndexBuffer(m_pDefaultIB, DXGI_FORMAT_R16_UINT, 0);
	}
	else if (BindFlags & D3D11_BIND_INDEX_BUFFER)
	{
		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pDefaultVB, &stride, &offset);
		m_pDeviceContext->IASetIndexBuffer(pBuffer, DXGI_FORMAT_R16_UINT, 0);
	}
	else
	{
		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pDefaultVB, &stride, &offset);
		m_pDeviceContext->IASetIndexBuffer(m_pDefaultIB, DXGI_FORMAT_R16_UINT, 0);
	}
}

TEST_RESULT CBufferTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;
	UINT ZeroOffsets[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	ID3D11Buffer *pNullBuffers[] = { NULL };
	const UINT stride = 4;
	const UINT offset = 0;
	BYTE *pVBData = m_BufferData[0] + (GetFramework()->GetCurrentTestCase() % 4);
	BYTE *pIBData = (BYTE*)(g_SmallIndices + (GetFramework()->GetCurrentTestCase() % 4));
	BYTE pVBIBData[16 * 16 * 2];
	BYTE *pInitialData = pVBData;

	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA subData;

	m_LayoutInfo.slots[0].pVBData = pVBData;
	m_LayoutInfo.pIBData = pIBData;
	
	bool bSrcUsed = m_AccessMethod != RAC_UPDATE_DEST;

	memcpy(pVBIBData, pVBData, 16 * 16);
	memcpy(pVBIBData + 16*16, pIBData, 16 * 16);

	//Update default VB and IB data
	BYTE* pData = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	GetImmediateContext()->Map( m_pDefaultVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	memcpy(mappedRes.pData, pVBData, 16*16*2);
	GetImmediateContext()->Unmap(m_pDefaultVB,0);

	GetImmediateContext()->Map( m_pDefaultIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	memcpy(mappedRes.pData, pIBData, 16*sizeof(WORD));
	GetImmediateContext()->Unmap( m_pDefaultIB, 0 );


	//UpdateDest doesn't need a src buffer
	if (bSrcUsed)
	{
		//Source buffer
		bufferDesc.Usage = m_SrcResourceFlags.Usage;
		bufferDesc.BindFlags = m_SrcResourceFlags.BindFlags;
		bufferDesc.CPUAccessFlags = m_SrcResourceFlags.CPUAccessFlags;
		bufferDesc.MiscFlags = m_SrcResourceFlags.MiscFlags;
		bufferDesc.ByteWidth = 16 * 16 * 2;

		subData.SysMemPitch = 0;
		subData.SysMemSlicePitch = 0;

		if (m_SrcResourceFlags.BindFlags & D3D11_BIND_INDEX_BUFFER && m_SrcResourceFlags.BindFlags & D3D11_BIND_VERTEX_BUFFER)
		{
			pInitialData = pVBIBData;
		}
		else if (m_SrcResourceFlags.BindFlags & D3D11_BIND_INDEX_BUFFER)
		{
			pInitialData = (BYTE*)pIBData;
		}

		subData.pSysMem = (void *)pInitialData;

		if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, &subData, &m_pSrcBuffer ) ) )
		{
			WriteToLog( "CreateBuffer() src failed" );
			tRes = RESULT_FAIL;
			goto TESTFAIL;
		}
	}

	//Dest buffer
	bufferDesc.Usage = m_DestResourceFlags.Usage;
	bufferDesc.BindFlags = m_DestResourceFlags.BindFlags;
	bufferDesc.CPUAccessFlags = m_DestResourceFlags.CPUAccessFlags;
	bufferDesc.MiscFlags = m_DestResourceFlags.MiscFlags;
	bufferDesc.ByteWidth = 16 * 16 * 2;

	if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL, &m_pDestBuffer ) ) )
	{
		WriteToLog( "CreateBuffer() dest failed" );
		tRes = RESULT_FAIL;
		goto TESTFAIL;
	}

	//Need to draw with the resource first
	//Maybe cycle on this so we try without drawing first?
	//Should we unbind the buffers before copying? Cycle on this?
	//Should with draw with the dest resource?  Cycle?

	//Draw with Source buffer
	if (m_bDrawSrcBuffer)
	{
		SetVBIB(m_pSrcBuffer, m_SrcResourceFlags.BindFlags);
		m_pDeviceContext->DrawIndexed(16, 0, 0);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto TESTFAIL;
		}
	}

	//Draw with Dest buffer
	if (m_bDrawDestBuffer)
	{
		SetVBIB(m_pDestBuffer, m_DestResourceFlags.BindFlags);
		m_pDeviceContext->DrawIndexed(16, 0, 0);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto TESTFAIL;
		}

	}

	//Unbind buffers
	if (m_bUnbindBuffer)
	{
		m_pDeviceContext->IASetVertexBuffers(0, 1, pNullBuffers, &stride, &offset);
		m_pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto TESTFAIL;
		}

	}

	//Set SO Buffer.  This resets the writing offset in the SO buffer to the begining
	m_pDeviceContext->SOSetTargets(1, &m_pSOBuffer, ZeroOffsets);

	switch (m_AccessMethod)
	{
		case RAC_UPDATE_SRC:
			//nothing to test, this would be the same as map test
			break;

		case RAC_UPDATE_DEST:
			m_pDeviceContext->UpdateSubresource( m_pDestBuffer, 0, NULL, m_BufferData[0] + (GetFramework()->GetCurrentTestCase() % 4), bufferDesc.ByteWidth, 0 );
			break;

		case RAC_COPY_SRC:
		case RAC_COPY_DEST:
			m_pDeviceContext->CopyResource( m_pDestBuffer, m_pSrcBuffer );
			break;

		case RAC_COPYREGION_SRC:
		case RAC_COPYREGION_DEST:
			m_pDeviceContext->CopySubresourceRegion( m_pDestBuffer, 0, 0, 0, 0, m_pSrcBuffer, 0, NULL );
			break;

	}
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto TESTFAIL;
	}

	if (bSrcUsed && m_SrcResourceFlags.BindFlags & D3D11_BIND_INDEX_BUFFER && m_SrcResourceFlags.BindFlags & D3D11_BIND_VERTEX_BUFFER)
	{
		if ((m_DestResourceFlags.BindFlags & D3D11_BIND_INDEX_BUFFER) && !(m_DestResourceFlags.BindFlags & D3D11_BIND_VERTEX_BUFFER))
		{
			m_LayoutInfo.pIBData = pVBIBData;
			m_LayoutInfo.slots[0].pVBData = pVBData;
		}
		else
			m_LayoutInfo.slots[0].pVBData = pVBIBData;
	}
	else if (bSrcUsed && m_SrcResourceFlags.BindFlags & D3D11_BIND_INDEX_BUFFER)
	{
		m_LayoutInfo.pIBData = pIBData;
		if ((m_DestResourceFlags.BindFlags & D3D11_BIND_INDEX_BUFFER) && !(m_DestResourceFlags.BindFlags & D3D11_BIND_VERTEX_BUFFER))
			m_LayoutInfo.slots[0].pVBData = pVBData;
		else
			m_LayoutInfo.slots[0].pVBData = pIBData;
	}
	else if (m_DestResourceFlags.BindFlags & D3D11_BIND_INDEX_BUFFER)
	{
		m_LayoutInfo.pIBData = pVBData;
	}

	if ((m_DestResourceFlags.BindFlags & D3D11_BIND_VERTEX_BUFFER) || (m_DestResourceFlags.BindFlags & D3D11_BIND_INDEX_BUFFER))
	{
		SetVBIB(m_pDestBuffer, m_DestResourceFlags.BindFlags);

		//Start the query for pipeline stats.  It's ended in VerifyDraw*Results.
		GetEffectiveContext()->Begin(m_pQuery);

		m_pDeviceContext->DrawIndexed(16, 0, 0);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto TESTFAIL;
		}
		if (!VerifyDrawIndexedResults(16, 0, 0))
		{
			tRes = RESULT_FAIL;
			goto TESTFAIL;
		}
	}

	//need to verify the entire buffer
	m_pDeviceContext->CopyResource( m_pResultBuffer, m_pDestBuffer );
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto TESTFAIL;
	}

	BYTE* pResult = NULL;
	if( FAILED( hr = GetImmediateContext()->Map( m_pResultBuffer, 0, D3D11_MAP_READ, 0, &mappedRes ) ) )
	{
		WriteToLog( _T("Map call on dest buffer failed. hr=%s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto TESTFAIL;
	}
	
	pResult = (BYTE*) mappedRes.pData;

	if (0 != memcmp( pInitialData, pResult, 16 * 16 * 2 ))
	{
		WriteToLog( "Data in dest buffer after Access method is not as expected" );
		GetImmediateContext()->Unmap(m_pResultBuffer,0);
		tRes = RESULT_FAIL;
		goto TESTFAIL;
	}
	GetImmediateContext()->Unmap(m_pResultBuffer,0);
	
TESTFAIL:

	m_pDeviceContext->IASetVertexBuffers(0, 1, pNullBuffers, &stride, &offset);
	m_pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);

	SAFE_RELEASE( m_pSrcBuffer );
	SAFE_RELEASE( m_pDestBuffer );

	return tRes;
}
