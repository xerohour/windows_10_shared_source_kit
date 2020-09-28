//////////////////////////////////////////////////////////////////////////
//  Registers.cpp
//  created:	2006/09/22
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include <float.h>
#include "registers.h"

const char szInitVS[] =
	"vs_4_0"							"\n"
	;
	
const char szFinishVS[] =
	"\n"
	//"mov o[0].x, r5.x"				"\n"
	;

const char szInitGS[] =
	"gs_4_0"							"\n"
	"dcl_inputPrimitive point"			"\n"
	"dcl_outputTopology pointlist"		"\n"
	"dcl_maxOutputVertexCount 1"		"\n"
	//"<inst> o[0], r7, r8"		 // Test instruction here
	;
	
const char szFinishGS[] =
	"emit"								"\n"
	;
	
const char szInitPS[] =
	"ps_4_0"							"\n"
	;

const char szFinishPS[] =
	""				"\n"
	;


TEST_RESULT CShader5xTest_Registers::Setup()
{
	TEST_RESULT tr = CShader5xTest::Setup();
	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTBufferView, NULL );
	return tr;
}

TEST_RESULT CShader5xTest_Registers::CreateVertexShader(tstring szShaderBody)
{
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	tstring szShader;

	szShader = tstring(szInitVS) + szShaderBody + tstring(szFinishVS);

	if( !AssembleShader( szShader.c_str(), &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T( "AssembleShader() failed assembling VS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		return RESULT_FAIL;
	}
	
	if( FAILED( GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pVS ) ) )
	{
		WriteToLog( _T( "CDevice::CreateVertexShader() failed creating VS" ) );
		return RESULT_FAIL;
	}
	
	if( g_bDumpDissasm )
		MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );

	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	return RESULT_PASS;
}

TEST_RESULT CShader5xTest_Registers::CreateGeometryShader(tstring szShaderBody, bool bCreateWithStreamOutput)
{
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	tstring szShader;

	// GS
	szShader = tstring(szInitGS) + szShaderBody + tstring(szFinishGS);
		
	if( !AssembleShader( szShader.c_str(), &pShaderBuf, &pErrorBuf ) )
	{
		WriteToLog( _T( "AssembleShader() failed assembling GS: %s" ), pErrorBuf->GetBufferPointer() );
		return RESULT_FAIL;
	}
	
	if (bCreateWithStreamOutput)
	{
		const D3D11_SO_DECLARATION_ENTRY pDecl[] =
		{
			{ 0, "0_", 0, 0, 4, 0 },
		};

		UINT stride0 = 1 * 4 * sizeof( DWORD );

		if( FAILED( GetDevice()->CreateGeometryShaderWithStreamOutput( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), pDecl, 1, &stride0, 1, 0, NULL, &m_pGS ) ) )
		{
			WriteToLog( _T( "CDevice::CreateGeometryShaderWithStreamOutput() failed creating GS StreamOutput" ) );
			return RESULT_FAIL;
		}
	}
	else //no SO
	{
		if( FAILED( GetDevice()->CreateGeometryShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pGS ) ) )
		{
			WriteToLog( _T( "CDevice::CreateGeometryShader() failed creating GS" ) );
			return RESULT_FAIL;
		}
	}

	if( g_bDumpDissasm )
		MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );

	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	return RESULT_PASS;
}

TEST_RESULT CShader5xTest_Registers::CreatePixelShader(tstring szShaderBody)
{
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	tstring szShader;

	// PS
	szShader = tstring(szInitPS) + szShaderBody + tstring(szFinishPS);
		
	if( !AssembleShader( szShader.c_str(), &pShaderBuf, &pErrorBuf ) )
	{
		WriteToLog( _T( "AssembleShader() failed assembling PS: %s" ), pErrorBuf->GetBufferPointer() );
		return RESULT_FAIL;
	}
	
	if( FAILED( GetDevice()->CreatePixelShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pPS ) ) )
	{
		WriteToLog( _T( "CDevice::CreatePixelShader() failed creating PS" ) );
		return RESULT_FAIL;
	}
	
	if( g_bDumpDissasm )
		MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );

	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );	
	
	return RESULT_PASS;
}

TEST_RESULT CShader5xTest_Registers::SetPipeline(tstring szShaderBody)
{
	char szSourceBuffer[4096];
	
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	
	tstring szShader;
	if (m_ShaderStage == D3D_SHADER_STAGE_VERTEX)
	{
		ClearStreamOutputBuffer( 4 );

		if ( RESULT_FAIL == CreateVertexShader(szShaderBody) )
			return RESULT_FAIL;

		GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );

		GetEffectiveContext()->GSSetShader( m_pNullGSStreamOutput, NULL, 0 );
		UINT offsets[] = { 0 };
		GetEffectiveContext()->SOSetTargets( 1, &m_pStreamOutputBuffer, offsets );

		GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );

		GetEffectiveContext()->RSSetState( m_pRSDisabled );
	}
	else if (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
	{
		ClearStreamOutputBuffer( 4 );

		GetEffectiveContext()->VSSetShader( m_pPassThroughVS, NULL, 0 );

		if ( RESULT_FAIL == CreateGeometryShader(szShaderBody, true) )
			return RESULT_FAIL;

		GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );

		UINT offsets[] = { 0 };
		GetEffectiveContext()->SOSetTargets( 1, &m_pStreamOutputBuffer, offsets );

		GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->RSSetState( m_pRSDisabled );
	}
	else if (m_ShaderStage == D3D_SHADER_STAGE_PIXEL)
	{
		//First Clear RT
		DWORD dwColor = CLEAR_VALUE;
		float color = *( (float*) &dwColor );
		FLOAT colors[4] = { color, color, color, color };
		GetEffectiveContext()->ClearRenderTargetView( m_pRTBufferView, colors );


		GetEffectiveContext()->VSSetShader( m_pPassThroughVS, NULL, 0 );

		ID3D11Buffer *pNullBuffers[] = { NULL };
		UINT offsets[] = { 0 };
		GetEffectiveContext()->SOSetTargets( 1, pNullBuffers, offsets );
		GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );


		if ( RESULT_FAIL == CreatePixelShader(szShaderBody) )
			return RESULT_FAIL;

	
		GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
		
		GetEffectiveContext()->RSSetState( m_pRSEnabled );
	}

	return RESULT_PASS;
}


void CShader5xTest_Registers::CleanupTestCase()
{
	GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
	}

	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
}


//////////////////////////////////////////////////////////////////////////
// temp registers
//////////////////////////////////////////////////////////////////////////

void CShader5xTest_Registers_LimitsTemp::InitTestParameters()
{
	CUserValueSet< D3D_SHADER_STAGE > *pShaderStageValues = new CUserValueSet< D3D_SHADER_STAGE >();
	pShaderStageValues->AddValue( D3D_SHADER_STAGE_VERTEX );
	pShaderStageValues->AddValue( D3D_SHADER_STAGE_GEOMETRY );
	pShaderStageValues->AddValue( D3D_SHADER_STAGE_PIXEL );

	CTestCaseParameter< UINT > *pTemp = AddParameter< UINT >( _T("Temp"), &m_TempCount);
	CTestCaseParameter< UINT > *pIndexableTemp = AddParameter< UINT >( _T("IndexableTemp"), &m_IndexableTempCount);
	CTestCaseParameter< UINT > *pNumIndexableRegisters = AddParameter< UINT >( _T("NumIndexableRegisters"), &m_NumIndexableRegisters);

	testfactor::RFactor rNumTemp0 = AddParameterValue< UINT >( pTemp, 0 );
	testfactor::RFactor rNumTemp2048 = AddParameterValue< UINT >( pTemp, 2048 );
	testfactor::RFactor rNumTemp4096 = AddParameterValue< UINT >( pTemp, 4096 );

	testfactor::RFactor rNumIndexableRegisters0 = AddParameterValue< UINT >( pNumIndexableRegisters, 0 );
	testfactor::RFactor rNumIndexableRegisters1 = AddParameterValue< UINT >( pNumIndexableRegisters, 1 );
	testfactor::RFactor rNumIndexableRegisters3 = AddParameterValue< UINT >( pNumIndexableRegisters, 3 );
	testfactor::RFactor rNumIndexableRegisters2048 = AddParameterValue< UINT >( pNumIndexableRegisters, 2048 );
	testfactor::RFactor rNumIndexableRegisters4096 = AddParameterValue< UINT >( pNumIndexableRegisters, 4096 );

	testfactor::RFactor rNumIndexTemp0 = AddParameterValue< UINT >( pIndexableTemp, 0 );
	testfactor::RFactor rNumIndexTemp2048 = AddParameterValue< UINT >( pIndexableTemp, 2048 );
	testfactor::RFactor rNumIndexTemp4096 = AddParameterValue< UINT >( pIndexableTemp, 4096 );

	testfactor::RFactor rTotalIndexTemp0 = rNumIndexTemp0 * rNumIndexableRegisters0;
	testfactor::RFactor rTotalIndexTemp2048 = rNumIndexTemp2048 * (rNumIndexableRegisters1 + rNumIndexableRegisters3 + rNumIndexableRegisters2048);
	testfactor::RFactor rTotalIndexTemp4096 = rNumIndexTemp4096 * (rNumIndexableRegisters1 + rNumIndexableRegisters3 + rNumIndexableRegisters4096);

	testfactor::RFactor rMaxTemp =	(rNumTemp0 * rTotalIndexTemp4096) +
									(rNumTemp2048 * rTotalIndexTemp2048) +
									(rNumTemp4096 * rTotalIndexTemp0); 

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	testfactor::RFactor shaderStage = AddParameterValueSet( _T(m_shaderStageParameterStr), pShaderStageValues );

	SetRootTestFactor( rMaxTemp * shaderStage );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumIndexableRegisters"), 1), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumIndexableRegisters"), 4096), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

UINT CShader5xTest_Registers_LimitsTemp::GetNumElementsForIndexableTemp(UINT indexableTemp)
{
	UINT numElements = 0;
	if (m_NumIndexableRegisters == m_IndexableTempCount)
		numElements = 1;
	else if (m_NumIndexableRegisters == 1)
		numElements = m_IndexableTempCount;
	else if (m_NumIndexableRegisters == 3)
	{
		//In this case, let's do [10, 100, rest].  This is to make sure they don't use the numElements of x0 for all the other x#
		switch(indexableTemp)
		{
			case 0:
				numElements = 10;
				break;
			case 1:
				numElements = 100;
				break;
			case 2:
				numElements = m_IndexableTempCount - 100 - 10;
				break;
		}
	}

	return numElements;
}

TEST_RESULT CShader5xTest_Registers_LimitsTemp::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;

	char szBuffer[4096];

	//Create the declarations of the temp registers
	tstring szDecl = tstring("dcl_output o[0].xyzw\n");

	_snprintf( szBuffer, sizeof( szBuffer ) - 1, "dcl_temps %u\n", m_TempCount );

	// Zero terminate szBuffer to make PreFast happy
	szBuffer[sizeof(szBuffer) - 1] = _T('\0');

	szDecl += tstring(szBuffer);

	for (UINT i=0; i<m_NumIndexableRegisters; i++)
	{
		UINT numElements = GetNumElementsForIndexableTemp(i);

		_snprintf( szBuffer, sizeof( szBuffer ) - 1, "dcl_indexableTemp x%u[%u], 4\n", i, numElements );

		// Zero terminate szBuffer to make PreFast happy
		szBuffer[sizeof(szBuffer) - 1] = _T('\0');

		szDecl += tstring(szBuffer);
	}
	
	//Move data to all the temp registers
	tstring szMoveData = "";

	for (UINT i=0; i<m_TempCount; i++)
	{
		_snprintf( szBuffer, sizeof( szBuffer ) - 1, "mov r%u, %u\n", i, i * 2 );

		// Zero terminate szBuffer to make PreFast happy
		szBuffer[sizeof(szBuffer) - 1] = _T('\0');

		szMoveData += tstring(szBuffer);
	}

	for (UINT i=0; i<m_NumIndexableRegisters; i++)
	{
		UINT numElements = GetNumElementsForIndexableTemp(i);

		for (UINT iElement=0; iElement<numElements; iElement++)
		{
			_snprintf( szBuffer, sizeof( szBuffer ) - 1, "mov x%u[%u], %u\n", i, iElement, 666 + i * 100 + iElement );

			// Zero terminate szBuffer to make PreFast happy
			szBuffer[sizeof(szBuffer) - 1] = _T('\0');

			szMoveData += tstring(szBuffer);
		}
	}

	//Do some operations with most of the temp registers and put the result in r0
	tstring szOperations = "mov o[0], 0\n";

	for (UINT i=1; i<m_TempCount; i++)
	{
		_snprintf( szBuffer, sizeof( szBuffer ) - 1, "iadd r0.x, r0.x, r%u.x\n", i );

		// Zero terminate szBuffer to make PreFast happy
		szBuffer[sizeof(szBuffer) - 1] = _T('\0');

		szOperations += tstring(szBuffer);
	}

	//Move result to o[0]
	if (m_TempCount > 0)
		szOperations += "mov o[0].x, r0.x\n";

	for (UINT i=0; i<m_NumIndexableRegisters; i++)
	{
		UINT numElements = GetNumElementsForIndexableTemp(i);

		for (UINT iElement=0; iElement<numElements; iElement++)
		{
			if( i == 0 && iElement == 0 )
				continue;

			_snprintf( szBuffer, sizeof( szBuffer ) - 1, "iadd x0[0], x0[0], x%u[%u]\n", i, iElement );

			// Zero terminate szBuffer to make PreFast happy
			szBuffer[sizeof(szBuffer) - 1] = _T('\0');

			szOperations += tstring(szBuffer);
		}
	}

	//Move result to o[0]
	if (m_NumIndexableRegisters > 0)
		szOperations += "mov o[0].y, x0[0]\n";


	//Construct full shader
	tstring szShaderBody = szDecl + szMoveData + szOperations;

	if (RESULT_FAIL == SetPipeline(szShaderBody))
		return RESULT_FAIL;

	GetEffectiveContext()->Draw( 1, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		return RESULT_FAIL;
	}

	UINT result[2];
	if ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL )
	{
		GetImmediateContext()->CopyResource( m_pRTBufferCopy, m_pRTBuffer );

		// Lock the render target and examine the results
		if (FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, result) ) )
		{
			WriteToLog( _T("Failed to map m_pRTBufferCopy, hr=%s."), D3DHResultToString(hr).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}
	}
	else
	{
		// Lock the output stream and examine the results
		GetImmediateContext()->CopyResource( m_pStreamOutputBufferCopy, m_pStreamOutputBuffer );
		if (FAILED( hr = MapAndRead(GetImmediateContext(), m_pStreamOutputBufferCopy, result) ) )
		{
			WriteToLog( _T("Failed to map m_pStreamOutputBufferCopy, hr=%s."), D3DHResultToString(hr).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}
	}

	{
		UINT calcResultTemp = 0;
		for (UINT i=0; i<m_TempCount; i++)
		{
			calcResultTemp += i * 2;
		}

		UINT calcResultIndexTemp = 0;
		for (UINT i=0; i<m_NumIndexableRegisters; i++)
		{
			UINT numElements = GetNumElementsForIndexableTemp(i);

			for (UINT iElement=0; iElement<numElements; iElement++)
			{
				calcResultIndexTemp += 666 + i * 100 + iElement;
			}
		}

		if ( calcResultTemp != result[0] )
		{
			WriteToLog( 1, _T("Expected temp %x : result %x."), calcResultTemp, result[0] );
			tr = RESULT_FAIL;
		}
		if ( calcResultIndexTemp != result[1] )
		{
			WriteToLog( 1, _T("Expected indexTemp %x : result %x."), calcResultIndexTemp, result[1] );
			tr = RESULT_FAIL;
		}
	}

testDone:
	return tr;
}

//////////////////////////////////////////////////////////////////////////
// input and output registers
//////////////////////////////////////////////////////////////////////////

const UINT g_NonSequentialReg16[16] = 
{ 14, 6, 2, 0, 9, 11, 3, 15, 1, 8, 5, 10, 4, 13, 7, 12 };

const UINT g_NonSequentialReg32[32] = 
{ 21, 18, 14, 30, 6, 16, 2, 0, 27, 9, 22, 11, 31, 3, 17, 15, 20, 1, 24, 8, 25, 5, 10, 29, 19, 4, 26, 13, 7, 23, 12 };


void CShader5xTest_Registers_LimitsInputOutput::InitTestParameters()
{
	CTestCaseParameter< UINT > *pNumInput = AddParameter< UINT >( _T("NumInput"), &m_NumInput);
	CTestCaseParameter< UINT > *pNumOutput = AddParameter< UINT >( _T("NumOutput"), &m_NumOutput);
	CTestCaseParameter< SHADER_STAGE_TYPE > *pOutputStage = AddParameter< SHADER_STAGE_TYPE >( _T("OutputStage"), &m_OutputStage);
	CTestCaseParameter< SHADER_STAGE_TYPE > *pInputStage = AddParameter< SHADER_STAGE_TYPE >( _T("InputStage"), &m_InputStage);
	CTestCaseParameter< bool > *pSequentialRegisters = AddParameter< bool >( _T("SequentialReg"), &m_SequentialRegisters);
	CTestCaseParameter< tstring > *pInputMask = AddParameter< tstring >( _T("InputMask"), &m_InputMask);
	CTestCaseParameter< tstring > *pOutputMask = AddParameter< tstring >( _T("OutputMask"), &m_OutputMask);

	testfactor::RFactor rOutputVS = AddParameterValue< SHADER_STAGE_TYPE >( pOutputStage, SHADER_STAGE_VERTEX );
	testfactor::RFactor rOutputGS = AddParameterValue< SHADER_STAGE_TYPE >( pOutputStage, SHADER_STAGE_GEOMETRY );

	testfactor::RFactor rInputGS = AddParameterValue< SHADER_STAGE_TYPE >( pInputStage, SHADER_STAGE_GEOMETRY );
	testfactor::RFactor rInputPS = AddParameterValue< SHADER_STAGE_TYPE >( pInputStage, SHADER_STAGE_PIXEL );

	testfactor::RFactor rSequentialReg = AddParameterValue< bool >( pSequentialRegisters, true );
	testfactor::RFactor rNonSequentialReg = AddParameterValue< bool >( pSequentialRegisters, false );

	CUserValueSet< UINT > *p1InputValues = new CUserValueSet< UINT >();
	p1InputValues->AddValue( 0 );
	p1InputValues->AddValue( 1 );
	testfactor::RFactor r1Input = AddParameterValueSet< UINT >( pNumInput, p1InputValues );
	testfactor::RFactor r1Output = AddParameterValue< UINT >( pNumOutput, 1 );

	CUserValueSet< UINT > *p2InputValues = new CUserValueSet< UINT >();
	p2InputValues->AddValue( 0 );
	p2InputValues->AddValue( 1 );
	p2InputValues->AddValue( 2 );
	testfactor::RFactor r2Input = AddParameterValueSet< UINT >( pNumInput, p2InputValues );
	testfactor::RFactor r2Output = AddParameterValue< UINT >( pNumOutput, 2 );

	CUserValueSet< UINT > *p15InputValues = new CUserValueSet< UINT >();
	p15InputValues->AddValue( 0 );
	p15InputValues->AddValue( 1 );
	p15InputValues->AddValue( 8 );
	p15InputValues->AddValue( 15 );
	testfactor::RFactor r15Input = AddParameterValueSet< UINT >( pNumInput, p15InputValues );
	testfactor::RFactor r15Output = AddParameterValue< UINT >( pNumOutput, 15 );

	CUserValueSet< UINT > *p16InputValues = new CUserValueSet< UINT >();
	p16InputValues->AddValue( 0 );
	p16InputValues->AddValue( 1 );
	p16InputValues->AddValue( 8 );
	p16InputValues->AddValue( 16 );
	testfactor::RFactor r16Input = AddParameterValueSet< UINT >( pNumInput, p16InputValues );
	testfactor::RFactor r16Output = AddParameterValue< UINT >( pNumOutput, 16 );

	CUserValueSet< UINT > *p32InputValues = new CUserValueSet< UINT >();
	p32InputValues->AddValue( 0 );
	p32InputValues->AddValue( 1 );
	p32InputValues->AddValue( 8 );
	p32InputValues->AddValue( 16 );
	p32InputValues->AddValue( 31 );
	p32InputValues->AddValue( 32 );
	testfactor::RFactor r32Input = AddParameterValueSet< UINT >( pNumInput, p32InputValues );
	testfactor::RFactor r32Output = AddParameterValue< UINT >( pNumOutput, 32 );

	CUserValueSet< tstring > *pXYZWInputMaskValues = new CUserValueSet< tstring >();
	pXYZWInputMaskValues->AddValue( tstring("xyzw") );
	pXYZWInputMaskValues->AddValue( tstring("xyz") );
	pXYZWInputMaskValues->AddValue( tstring("xw") );
	pXYZWInputMaskValues->AddValue( tstring("x") );
	pXYZWInputMaskValues->AddValue( tstring("y") );
	testfactor::RFactor rXYZWInputMask = AddParameterValueSet< tstring >( pInputMask, pXYZWInputMaskValues );
	testfactor::RFactor rXYZWOutputMask = AddParameterValue< tstring >( pOutputMask, tstring("xyzw") );

	CUserValueSet< tstring > *pXWInputMaskValues = new CUserValueSet< tstring >();
	pXWInputMaskValues->AddValue( tstring("xw") );
	pXWInputMaskValues->AddValue( tstring("x") );
	pXWInputMaskValues->AddValue( tstring("w") );
	testfactor::RFactor rXWInputMask = AddParameterValueSet< tstring >( pInputMask, pXWInputMaskValues );
	testfactor::RFactor rXWOutputMask = AddParameterValue< tstring >( pOutputMask, tstring("xw") );

	CUserValueSet< tstring > *pYInputMaskValues = new CUserValueSet< tstring >();
	pYInputMaskValues->AddValue( tstring("y") );
	testfactor::RFactor rYInputMask = AddParameterValueSet< tstring >( pInputMask, pYInputMaskValues );
	testfactor::RFactor rYOutputMask = AddParameterValue< tstring >( pOutputMask, tstring("y") );

	testfactor::RFactor rMasks =	(rXYZWInputMask * rXYZWOutputMask) +
									(rXWInputMask * rXWOutputMask) +
									(rYInputMask * rYOutputMask);

	testfactor::RFactor r15Link =	(r1Input * r1Output) +
									(r2Input * r2Output) +
									(r15Input * r15Output);

	testfactor::RFactor r16Link =	(r1Input * r1Output) +
									(r2Input * r2Output) +
									(r15Input * r15Output) +
									(r16Input * r16Output);

	testfactor::RFactor r32Link =	(r1Input * r1Output) +
									(r2Input * r2Output) +
									(r15Input * r15Output) +
									(r16Input * r16Output) +
									(r32Input * r32Output);

	testfactor::RFactor rOutputVSLinks;
	if (GetFramework()->m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0)
		rOutputVSLinks = (r16Link * rSequentialReg) + (r15Link * rNonSequentialReg);
	else if (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
		rOutputVSLinks = (r32Link * rSequentialReg) + (r16Link * rNonSequentialReg);
	testfactor::RFactor rOutputGSLinks = (r32Link * rSequentialReg) + (r16Link * rNonSequentialReg);

	testfactor::RFactor rVSToGS = rOutputVS * rInputGS * rOutputVSLinks;
	testfactor::RFactor rVSToPS = rOutputVS * rInputPS * rOutputVSLinks;
	testfactor::RFactor rGSToPS = rOutputGS * rInputPS * rOutputGSLinks;

	//GS To SO linkage is already tested in StreamOut test.

	//TODO: Could reduce when 0 input registers to use only 1 input mask


	SetRootTestFactor( (rVSToGS + rVSToPS + rGSToPS) * rMasks );
}

TEST_RESULT CShader5xTest_Registers_LimitsInputOutput::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;

	char szBuffer[4096];

	//Start by clearing the SO buffer and the RT if needed
	ClearStreamOutputBuffer( 4 );

	if ( m_InputStage == SHADER_STAGE_PIXEL )
	{
		//Clear RT
		DWORD dwColor = CLEAR_VALUE;
		float color = *( (float*) &dwColor );
		FLOAT colors[4] = { color, color, color, color };
		GetEffectiveContext()->ClearRenderTargetView( m_pRTBufferView, colors );
	}

	//Create the declarations for output stage
	tstring szDeclOutput = "";

	UINT index = 0;
	for (UINT i=0; i<m_NumOutput; i++)
	{
		index = m_SequentialRegisters ? i : m_OutputStage == SHADER_STAGE_GEOMETRY ? g_NonSequentialReg32[i] : g_NonSequentialReg16[i];

		if ( (m_InputStage == SHADER_STAGE_PIXEL) && (i == m_NumOutput - 1) )
			_snprintf( szBuffer, sizeof( szBuffer ) - 1, "dcl_output_sv o[%u].xyzw, position\n", index );
		else
			_snprintf( szBuffer, sizeof( szBuffer ) - 1, "dcl_output o[%u].%s\n", index, m_OutputMask.c_str() );

		// Zero terminate szBuffer to make PreFast happy
		szBuffer[sizeof(szBuffer) - 1] = _T('\0');

		szDeclOutput += tstring(szBuffer);
	}
	
	//Move data to all the outputs
	tstring szMoveDataOutput = "";

	for (UINT i=0; i<m_NumOutput; i++)
	{
		index = m_SequentialRegisters ? i : m_OutputStage == SHADER_STAGE_GEOMETRY ? g_NonSequentialReg32[i] : g_NonSequentialReg16[i];

		if ( (m_InputStage == SHADER_STAGE_PIXEL) && (i == m_NumOutput - 1) )
			_snprintf( szBuffer, sizeof( szBuffer ) - 1, "mov o[%u].xyzw, vec4( 0.0, 0.0, 0.0, 1.0 )\n", index );
		else
			_snprintf( szBuffer, sizeof( szBuffer ) - 1, "mov o[%u].%s, vec4(%u, %u, %u, %u)\n", index, m_OutputMask.c_str(), i * 4 + 1, i * 4 + 2, i * 4 + 3, i * 4 + 4 );

		// Zero terminate szBuffer to make PreFast happy
		szBuffer[sizeof(szBuffer) - 1] = _T('\0');

		szMoveDataOutput += tstring(szBuffer);
	}

	//Construct full shader for output stage
	tstring szShaderBodyOutput = szDeclOutput + szMoveDataOutput;

	if ( m_OutputStage == SHADER_STAGE_VERTEX )
	{
		if ( RESULT_FAIL == CreateVertexShader(szShaderBodyOutput) )
			return RESULT_FAIL;
	}
	else //stage geometry
	{
		if ( RESULT_FAIL == CreateGeometryShader(szShaderBodyOutput, false) )
			return RESULT_FAIL;
	}

	//Create the declarations for input stage
	tstring szDeclInput = "dcl_temps 2\n" "dcl_output o[0].xyzw\n";

	if ( m_NumInput > 0 )
	{
		UINT emptySlots = m_NumOutput - m_NumInput;
		UINT useEveryXSlots = m_NumOutput / m_NumInput;
		UINT slotsUsed = 0;
		for (UINT i=0; i<m_NumOutput; i++)
		{
			if (i % useEveryXSlots != 0 || slotsUsed == m_NumInput)
				continue;

			index = m_SequentialRegisters ? i : m_OutputStage == SHADER_STAGE_GEOMETRY ? g_NonSequentialReg32[i] : g_NonSequentialReg16[i];

			if (m_InputStage == SHADER_STAGE_GEOMETRY)
			{
				_snprintf( szBuffer, sizeof( szBuffer ) - 1, "dcl_input v[1][%u].%s\n", index, m_InputMask.c_str() );

				// Zero terminate szBuffer to make PreFast happy
				szBuffer[sizeof(szBuffer) - 1] = _T('\0');
			}
			else //pixel
			{
				if (i == m_NumOutput - 1)
					_snprintf( szBuffer, sizeof( szBuffer ) - 1, "dcl_input_ps_sv v[%u].xyzw, linear_noperspective, position\n", index );
				else
					_snprintf( szBuffer, sizeof( szBuffer ) - 1, "dcl_input_ps v[%u].%s, constant\n", index, m_InputMask.c_str() );
			}

			// Zero terminate szBuffer to make PreFast happy
			szBuffer[sizeof(szBuffer) - 1] = _T('\0');

			szDeclInput += tstring(szBuffer);
			slotsUsed ++;
		}

		assert( slotsUsed == (m_NumOutput - emptySlots) );
	}

	_snprintf( szBuffer, sizeof( szBuffer ) - 1, "mov o[0].xyzw, 255\n", m_OutputMask.c_str() );

	// Zero terminate szBuffer to make PreFast happy
	szBuffer[sizeof(szBuffer) - 1] = _T('\0');

	szDeclInput += tstring(szBuffer);

	//Verify all inputs are usable.
	
	if ( m_NumInput > 0 )
	{
		UINT emptySlots = m_NumOutput - m_NumInput;
		UINT useEveryXSlots = m_NumOutput / m_NumInput;
		UINT slotsUsed = 0;
		for (UINT i=0; i<m_NumOutput; i++)
		{
			if ( i % useEveryXSlots != 0 || slotsUsed == m_NumInput )//&& i == m_NumOutput - 1 )
				continue;

			//No need to check the position value for PS.
			if ( i == m_NumOutput - 1 && m_InputStage == SHADER_STAGE_PIXEL )
				continue;

			index = m_SequentialRegisters ? i : m_OutputStage == SHADER_STAGE_GEOMETRY ? g_NonSequentialReg32[i] : g_NonSequentialReg16[i];

			if (m_InputStage == SHADER_STAGE_GEOMETRY)
			{
				_snprintf( szBuffer, sizeof( szBuffer ) - 1, "ieq r0.%s, v[0][%u].%s, vec4(%u, %u, %u, %u)\n", m_InputMask.c_str(), index, m_InputMask.c_str(), i * 4 + 1, i * 4 + 2, i * 4 + 3, i * 4 + 4 );
			}
			else //pixel
			{
				_snprintf( szBuffer, sizeof( szBuffer ) - 1, "ieq r0.%s, v[%u].%s, vec4(%u, %u, %u, %u)\n", m_InputMask.c_str(), index, m_InputMask.c_str(), i * 4 + 1, i * 4 + 2, i * 4 + 3, i * 4 + 4 );
			}

			// Zero terminate szBuffer to make PreFast happy
			szBuffer[sizeof(szBuffer) - 1] = _T('\0');

			szDeclInput += tstring(szBuffer);
			//Make sure each used component are not 0
			szDeclInput += "mov r1.x, 0xffffffff\n";
			for (UINT c = 0; c<m_InputMask.size(); c++)
			{
				_snprintf( szBuffer, sizeof( szBuffer ) - 1, "and r1.x, r1.x, r0.%c\n", m_InputMask[c] );

				// Zero terminate szBuffer to make PreFast happy
				szBuffer[sizeof(szBuffer) - 1] = _T('\0');

				szDeclInput += tstring(szBuffer);
			}
			szDeclInput += "if_z r1.x\n";
			szDeclInput += "    mov o[0].x, 80\n";
			_snprintf( szBuffer, sizeof( szBuffer ) - 1, "    mov o[0].y, %u\n", index );

			// Zero terminate szBuffer to make PreFast happy
			szBuffer[sizeof(szBuffer) - 1] = _T('\0');

			szDeclInput += tstring(szBuffer);
			if (m_InputStage == SHADER_STAGE_GEOMETRY)
				szDeclInput += "    emit\n";
			szDeclInput += "    ret\n";
			szDeclInput += "endif\n";
			slotsUsed ++;
		}
	}

	//Construct full shader for input stage
	tstring szShaderBodyInput = szDeclInput;// + szMoveDataOutput;

	if ( m_InputStage == SHADER_STAGE_GEOMETRY )
	{
		if ( RESULT_FAIL == CreateGeometryShader(szShaderBodyInput, true) )
			return RESULT_FAIL;
	}
	else //stage pixel
	{
		if ( RESULT_FAIL == CreatePixelShader(szShaderBodyInput) )
			return RESULT_FAIL;
	}

	//Set the pipeline

	if ( m_OutputStage == SHADER_STAGE_VERTEX )
	{
		GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );

		if ( m_InputStage == SHADER_STAGE_GEOMETRY )
		{
			GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );

			UINT offsets[] = { 0 };
			GetEffectiveContext()->SOSetTargets( 1, &m_pStreamOutputBuffer, offsets );

			GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
			GetEffectiveContext()->RSSetState( m_pRSDisabled );
		}
		else
		{
			ID3D11Buffer *pNullBuffers[] = { NULL };
			UINT offsets[] = { 0 };
			GetEffectiveContext()->SOSetTargets( 1, pNullBuffers, offsets );
			GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
			GetEffectiveContext()->RSSetState( m_pRSEnabled );
			GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
		}
	}
	else
	{
		ID3D11Buffer *pNullBuffers[] = { NULL };
		UINT offsets[] = { 0 };
		GetEffectiveContext()->SOSetTargets( 1, pNullBuffers, offsets );

		GetEffectiveContext()->VSSetShader( m_pPassThroughVS, NULL, 0 );
		GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
		GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

		GetEffectiveContext()->RSSetState( m_pRSEnabled );
	}

	GetEffectiveContext()->Draw( 1, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		return RESULT_FAIL;
	}

	UINT result[2];
	if ( m_InputStage == SHADER_STAGE_PIXEL )
	{
		GetImmediateContext()->CopyResource( m_pRTBufferCopy, m_pRTBuffer );

		// Lock the render target and examine the results
		if (FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, result) ) )
		{
			WriteToLog( _T("Failed to map m_pRTBufferCopy, hr=%s."), D3DHResultToString(hr).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}
	}
	else
	{
		// Lock the output stream and examine the results
		GetImmediateContext()->CopyResource( m_pStreamOutputBufferCopy, m_pStreamOutputBuffer );
		if (FAILED( hr = MapAndRead(GetImmediateContext(), m_pStreamOutputBufferCopy, result) ) )
		{
			WriteToLog( _T("Failed to map m_pStreamOutputBufferCopy, hr=%s."), D3DHResultToString(hr).c_str() );
			tr = RESULT_FAIL;
			goto testDone;
		}
	}

	if ( 255 != result[0] )
	{
		WriteToLog( 1, _T("Expected success value is 255 but result is %u for index %u."), result[0], result[1] );
		tr = RESULT_FAIL;
	}

testDone:
	return tr;
}


