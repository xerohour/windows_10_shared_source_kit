//////////////////////////////////////////////////////////////////////////
//  Swizzle.cpp
//  created:	2005/05/11
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "Swizzle.h"

class CMaskValueSet : public CValueSet< std::string >
{
	size_t GetSize() const { return 15; }
	bool GetValue( size_t nIndex, std::string *pValue ) const
	{
		size_t v = nIndex + 1;
		const char values[] = "xyzw";
		
		for( size_t n = 0; n < 4; ++n )
		{
			if( v & ( (size_t) 1 << n ) )
				*pValue += values[n];
		}
		
		return true;
	};
};

class CSwizzleValueSet : public CValueSet< std::string >
{
	size_t GetSize() const { return 2 << 6; }
	bool GetValue( size_t nIndex, std::string *pValue ) const
	{
		if( nIndex >= GetSize() )
			return false;
			
		const char values[] = "xyzw";

		pValue->erase();
		*pValue += values[nIndex & 0x03];
		nIndex >>= 2;
		*pValue += values[nIndex & 0x03];
		nIndex >>= 2;
		*pValue += values[nIndex & 0x03];
		nIndex >>= 2;
		*pValue += values[nIndex & 0x03];
		
		return true;
	}
};

TEST_RESULT CShader5xTest_MaskSwizzle::Setup()
{
	TEST_RESULT tr = CShader5xTest::Setup();
	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTBufferView, NULL );
	return tr;
}

void CShader5xTest_MaskSwizzle::InitTestParameters()
{		
	m_dwValues[0] = 0;
	m_dwValues[1] = (DWORD) -1;
	m_dwValues[2] = 0xbaadf00d;
	m_dwValues[3] = 0xdeadbeef;
	m_dwValues[4] = 0x7f7f7f7f;
	m_dwValues[5] = FloatMaker::DENORMp();
	m_dwValues[6] = FloatMaker::DENORMn();
	m_dwValues[7] = FloatMaker::INFp();
	m_dwValues[8] = FloatMaker::INFn();
	m_dwValues[9] = FloatMaker::IND();
	m_dwValues[10] = FloatMaker::SNANp();
	m_dwValues[11] = FloatMaker::SNANn();
	m_dwValues[12] = FloatMaker::QNANp();
	m_dwValues[13] = FloatMaker::QNANn();
	m_dwValues[14] = FloatMaker::FLOATMAXp();
	m_dwValues[15] = FloatMaker::FLOATMAXn();

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	
	testfactor::RFactor swizzle = AddParameter( _T( "Swizzle" ), &m_strSwizzle, new CSwizzleValueSet() );
	testfactor::RFactor mask = AddParameter( _T( "Mask" ), &m_strMask, new CMaskValueSet() );
	
	testfactor::RFactor datax = AddParameter( _T( "DataX" ), &m_dwData[0], new CTableValueSet<DWORD>( &m_dwValues[0], sizeof( DWORD ), sizeof( m_dwValues ) / ( 4 * sizeof( m_dwValues[0] ) ) ) );
	testfactor::RFactor datay = AddParameter( _T( "DataY" ), &m_dwData[1], new CTableValueSet<DWORD>( &m_dwValues[1], sizeof( DWORD ), sizeof( m_dwValues ) / ( 4 * sizeof( m_dwValues[0] ) ) ) );
	testfactor::RFactor dataz = AddParameter( _T( "DataZ" ), &m_dwData[2], new CTableValueSet<DWORD>( &m_dwValues[2], sizeof( DWORD ), sizeof( m_dwValues ) / ( 4 * sizeof( m_dwValues[0] ) ) ) );
	testfactor::RFactor dataw = AddParameter( _T( "DataW" ), &m_dwData[3], new CTableValueSet<DWORD>( &m_dwValues[3], sizeof( DWORD ), sizeof( m_dwValues ) / ( 4 * sizeof( m_dwValues[0] ) ) ) );

	// Utilize pairwise to reduce the number of testcases
	testfactor::RFactor data = ( datax % datay % dataz % dataw );

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{data, 1, NULL, 0},
		{mask, 1, NULL, 0},
		{swizzle, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "Pairwise", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}	


TEST_RESULT CShader5xTest_MaskSwizzle::SetupTestCase()
{
	// Call the base SetupTestCase to determine if should skip
	TEST_RESULT tRes = CShader5xTest::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;
	
	return BuildShaders();
}

TEST_RESULT CShader5xTest_MaskSwizzle::ExecuteTestCase()
{
	HRESULT hr;
	// Bind the output stream to the pipeline.
	ID3D11Buffer *buffers[] = { m_pStreamOutputBuffer };
	UINT offsets[] = { 0 };
	GetEffectiveContext()->SOSetTargets( 1, buffers, offsets );
	
	// Set the constant buffer data
	DWORD pData[4];
	pData[0] = m_dwData[0];
	pData[1] = m_dwData[1];
	pData[2] = m_dwData[2];
	pData[3] = m_dwData[3];
	if (FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, pData) ) )
	{
		WriteToLog( _T( "ID3D11Buffer->Map() failed mapping m_pConstBuffer, hr=%s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	FLOAT ClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	
	
	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->ClearUnorderedAccessViewFloat(m_pRTUABufferView, ClearColor);
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{
		GetEffectiveContext()->ClearRenderTargetView( m_pRTBufferView, ClearColor );
		GetEffectiveContext()->Draw( 1, 0 );
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	TEST_RESULT tr = RESULT_PASS;

	if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY ) 
	{
		ResultType pSO[2];
		D3D11_BOX box;
		box.left = 0;
		box.right = 4 * 4 * sizeof( DWORD );
		box.top = 0;
		box.bottom = 1;
		box.front = 0;
		box.back = 1;
		GetImmediateContext()->CopySubresourceRegion( m_pStreamOutputBufferCopy, 0, 0, 0, 0, m_pStreamOutputBuffer, 0, &box );
		
		if (FAILED( hr = MapAndRead( GetImmediateContext(), m_pStreamOutputBufferCopy, pSO) ) )
		{
			WriteToLog( _T( "ID3D11Buffer->Map() failed mapping result buffer." ) );
			return RESULT_FAIL;
		}

		if (!CompareResult( pSO[0] ))
		{
			tr = RESULT_FAIL;
			WriteToLog(_T("CompareResult () failed for shader stage: %s"),ToString(m_ShaderStage).c_str());
		}
	}

	else
	{	
		ResultType RTData;
		D3D11_BOX box;
		box.left = 0;
		box.right =  4 * sizeof( DWORD );
		box.top = 0;
		box.bottom = 1;
		box.front = 0;
		box.back = 1;

		GetImmediateContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, &box );
		
		// Lock the render target and examine the results
		if (FAILED( hr = MapAndRead( GetImmediateContext(), m_pRTBufferCopy, RTData) ) )
		{
			WriteToLog( _T( "ID3D11Buffer->Map() failed mapping result buffer." ) );
			return RESULT_FAIL;
		}

		if ( !CompareResult( RTData ) )
		{
			tr = RESULT_FAIL;
			WriteToLog(_T("CompareResult () failed for shader stage: %s"),ToString(m_ShaderStage).c_str());
		}
	}
	
	return tr;	
}

bool CShader5xTest_MaskSwizzle::CompareResult( ResultType &result )
{
	bool bResult = true;
	tstring strSwizzle = ToString( m_strSwizzle );

	bool bMaskUsed[4] = { false, false, false, false };
	
	for( int i = m_strMask.size() - 1; i != size_t( -1 ); --i )
	{
		switch( m_strMask[i] )
		{
			case 'x':
				bMaskUsed[0] = true;
				break;
			case 'y':
				bMaskUsed[1] = true;
				break;
			case 'z':
				bMaskUsed[2] = true;
				break;
			case 'w':
				bMaskUsed[3] = true;
				break;
		}
	}
	
	for( int i = 0; i < 4; ++i )
	{
		DWORD dwValue;
		TCHAR nSwizzle = strSwizzle[i];
		
		if( !bMaskUsed[i] )
		{
			if( CLEAR_VALUE != result[i] )
			{
				WriteToLog(_T("Error: clear color(%d) does not match result[%d]=%d with mask[%d]."), CLEAR_VALUE, i,result[i], i);
				bResult = false;	
			}
		}
		else
		{
			switch( nSwizzle )
			{
				case _T( 'x' ):
					dwValue = m_dwData[0];
					break;
				case _T( 'y' ):
					dwValue = m_dwData[1];
					break;
				case _T( 'z' ):
					dwValue = m_dwData[2];
					break;
				case _T( 'w' ):
					dwValue = m_dwData[3];
					break;
			}
			
			if( dwValue != result[i] )
			{
				WriteToLog( _T( "Error: %c = result[%d]=0x%x, Expected value =0x%x." ), nSwizzle, i, result[i], dwValue);
				bResult = false;
			}
		}						
	}
	
	return bResult;
}

void CShader5xTest_MaskSwizzle::CleanupTestCase()
{
}


TEST_RESULT CShader5xTest_MaskSwizzle::BuildShaders()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	//
	// Generate and compile the shader source
	//
	char szSourceBuffer[4096];
	memset(szSourceBuffer, 0, 4096);
	
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;

	
	// clear out pipeline state for shaders
	GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->HSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->DSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->CSSetShader( NULL, NULL, 0 );

	//
	//  Need at least 100 temps because r99 is used by SetupMovToRawOutputFromTemp and SetOutputRegMaskRaw
	//
	AssemblyHelper asmHelper(1, 100);
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY )
	{
		asmHelper.SetOutputRegMask(1, AssemblyHelper::xyzw);
		asmHelper.SetUserInputRegMask(0, AssemblyHelper::xyzw);
	}
	
	asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	
	// insert target instructions
	_snprintf(
		szSourceBuffer,
		sizeof( szSourceBuffer ) - 1,
		"mov r0, vec4( %s, %s, %s, %s )\n"
		"mov r0.%s, c0[0].%s\n",
		str_CLEAR_VALUE, str_CLEAR_VALUE, str_CLEAR_VALUE, str_CLEAR_VALUE,
		m_strMask.c_str(),
		m_strSwizzle.c_str()
		);

	szSourceBuffer[sizeof(szSourceBuffer) - 1] = 0; // Fix for OACR error - MISSING_ZERO_TERMINATION1
	tstring instructionCode(szSourceBuffer);
	
	if ( m_ShaderStage != D3D_SHADER_STAGE_GEOMETRY ) 
	{
		hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
			m_ShaderStage, g_TestApp.m_D3DOptions, 
			instructionCode, &m_pRTBufferView, &m_pRTUABufferView, 1, &pShaderBuf,
			&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS);

		if ( FAILED(hr) )
		{
			WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			if( g_bDumpDissasm && pShaderBuf != NULL )
				MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );
			goto setupDone;
		}
	}

	else if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY )
	{
		GetEffectiveContext()->VSSetShader( m_pPassThroughVS, NULL, 0 );

		instructionCode += "mov o[1], v[0][0]\n";

		tstring shaderSource = asmHelper.MakeAssemblyShader(
			m_ShaderStage,  GetFramework()->m_D3DOptions.FeatureLevel, 
			g_TestApp.m_D3DOptions, instructionCode
			);
		if( !AssembleShader( shaderSource.c_str(), &pShaderBuf, &pErrorBuf ) )
		{
			if (pErrorBuf != NULL)
			{
				WriteToLog( _T( "AssembleShader() failed assembling GS: %s" ), pErrorBuf->GetBufferPointer() );
			}
			tRes = RESULT_FAIL;
			goto setupDone;
		}
		const D3D11_SO_DECLARATION_ENTRY streamDecl[] =
		{
			{ 0, "0_", 0, 0, 4, 0 },
			{ 0, "1_", 0, 0, 4, 0 },
		};

		UINT stride0 = 2 * 4 * sizeof( DWORD );
		
		if( FAILED( GetDevice()->CreateGeometryShaderWithStreamOutput( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), streamDecl, ARRAYSIZE(streamDecl), &stride0, 1, 0, NULL, &m_pGS ) ) )
		{
			WriteToLog( _T( "CDevice::CreatePixelShader() failed creating GS" ) );
			tRes = RESULT_FAIL;
			goto setupDone;
		}
		
		GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
		
		if( g_bDumpDissasm )
		MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );

		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
	}	

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
		goto setupDone;
	}

setupDone:
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );
	
	return tRes;
}
