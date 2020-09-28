#include "StreamOutPatches.h"
#include <sstream>
#define IFR(x) if( FAILED(x) ) { WriteToLog( _T("%s:%d , %s"), __FILE__, __LINE__, D3DHResultToString(x).c_str() ); return RESULT_FAIL; }
#define IFRM(x,m) if( FAILED(x) ) { WriteToLog( _T("%s:%d , %s, %s"), __FILE__, __LINE__, m, D3DHResultToString(x).c_str() ); return RESULT_FAIL; }

static const D3D11_PRIMITIVE_TOPOLOGY s_PatchTypes[] = 
{
	D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST,
	D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST
};

void CStreamOutPatches::SubmitLayout( unsigned int layout[4], bool ignore[4], unsigned int layoutCount )
{
	unsigned int start = m_DataLayout.size();
	bool ignoreAll = true;
	for( unsigned int i(0); i < layoutCount; ++i )
	{
		ignoreAll = ignoreAll && ignore[i];
	}
	if( ignoreAll )
	{
		return;
	}
	for( unsigned int i(0); i < layoutCount; ++i )
	{
		m_DataLayout.push_back( DataUnit( layout[i], ignore[i] ) );
	}
	m_LayoutWindows.push_back( LayoutWindow( start, m_DataLayout.size() - start ) );
}

void CStreamOutPatches::BuildLayout( unsigned int layout[4], bool ignore[4], unsigned int layoutCount, unsigned int layoutCompCount )
{
	for( unsigned int i(1); i < 5; ++i )
	{
		if( i + layoutCompCount > 4 )
		{
			break;
		}
		layout[layoutCount] = i;
		ignore[layoutCount] = false;
		if( i + layoutCompCount == 4 )
		{
			SubmitLayout( layout, ignore, layoutCount + 1 );
			ignore[layoutCount] = true;
			SubmitLayout( layout, ignore, layoutCount + 1 );
		}
		else
		{
			BuildLayout( layout, ignore, layoutCount + 1, layoutCompCount + i );
			ignore[layoutCount] = true;
			BuildLayout( layout, ignore, layoutCount + 1, layoutCompCount + i );
		}
	}
}

void CStreamOutPatches::InitTestParameters()
{

	CTestCaseParameter<D3D11_PRIMITIVE_TOPOLOGY>* pInputTop = AddParameter<D3D11_PRIMITIVE_TOPOLOGY>( "PrimitiveTopology", &m_inputTopology );
	CTestCaseParameter<unsigned int>* pLayoutIndex = AddParameter<unsigned int>( "LayoutIndex", &m_LayoutIndex );
	CTestCaseParameter<unsigned int>* pBufferCount = AddParameter<unsigned int>( "StreamOutBufferCount", &m_StreamOutBufferCount );
	CTestCaseParameter<unsigned int>* pPatchDrawCount = AddParameter<unsigned int>( "PatchesDrawn", &m_NumberOfPatchesToDraw );
	CTestCaseParameter<unsigned int>* pDrawCalls = AddParameter<unsigned int>( "DrawCalls", &m_NumberOfDrawCalls );
	
	// setup random data layouts for a 4 component element size
	{
		unsigned int layout[4];
		bool ignore[4];
		BuildLayout( layout, ignore, 0, 0 );
	}
	const unsigned int distribution[] = { 1, 2, 3, 4, 2, 3, 4, 1, 4 };
	// setup random data layouts for a 128 component (32 elements) of data.
	const unsigned int numberOfSingleElementLayouts = m_LayoutWindows.size();
	for( unsigned int i(0); i < NUMBER_OF_32_ELEMENT_LAYOUTS; ++i )
	{
		unsigned int start = m_DataLayout.size();
		unsigned int componentCount = 0;
		unsigned int inputCount = 0;
		while( componentCount < 128 )
		{
			unsigned int compCount = distribution[rand() % 9];
			if( componentCount + compCount > 128 )
			{
				continue;
			}
			// do we have to buffer this vertex data?
			if( componentCount % 4 + compCount > 4 )
			{
				unsigned int width = 4 - componentCount % 4;
				m_DataLayout.push_back( DataUnit( width, true ) );
				componentCount += width;
				if( componentCount >= 128 )
				{
					continue;
				}
				if( ++inputCount >= 32 )
				{
					break;
				}
			}
			if( componentCount )
			{
				m_DataLayout.push_back( DataUnit( compCount, rand() % 6 == 0 ) );
			}
			else
			{
				m_DataLayout.push_back( DataUnit( compCount, false ) );
			}
			componentCount += compCount;
			if( ++inputCount >= 32 )
			{
				break;
			}
		}
		m_LayoutWindows.push_back( LayoutWindow( start, m_DataLayout.size() - start ) );
	}

	
	testfactor::RFactor primitiveTypeFactor = AddParameterValueSet<D3D11_PRIMITIVE_TOPOLOGY>( pInputTop, 
		new CTableValueSet<D3D11_PRIMITIVE_TOPOLOGY>( s_PatchTypes, sizeof(D3D11_PRIMITIVE_TOPOLOGY), sizeof(s_PatchTypes) / sizeof(D3D11_PRIMITIVE_TOPOLOGY) )
		);

	testfactor::RFactor layoutRange = AddParameterValueSet( pLayoutIndex, new CRangeValueSet<unsigned int>( 0, numberOfSingleElementLayouts + NUMBER_OF_32_ELEMENT_LAYOUTS-1, 1 ) );
	
	testfactor::RFactor bufferCount = AddParameterValueSet( pBufferCount, new CRangeValueSet<unsigned int>( 1, 4, 1 ) );
	testfactor::RFactor VertexCount = AddParameterValueSet( pPatchDrawCount, new CRangeValueSet<unsigned int>( 1, 3, 1 ) );
	testfactor::RFactor DrawCalls = AddParameterValueSet( pDrawCalls, new CRangeValueSet<unsigned int>( 1, 2, 1 ) );

	testfactor::XFACTOR pairwise[] = 
	{
		{ layoutRange, 0, NULL, 0 },
		{ primitiveTypeFactor, 0, NULL, 0 },
		{ bufferCount, 0, NULL, 0 },
		{ VertexCount, 0, NULL, 0 },
		{ DrawCalls, 0, NULL, 0 }
	};

	SetRootTestFactor( testfactor::NewCrossFactor( pairwise, sizeof(pairwise) / sizeof(testfactor::XFACTOR) ) );
}

TEST_RESULT CStreamOutPatches::Setup()
{
	// setup static test patch data
	for( unsigned int i(0); i < D3D11_IA_PATCH_MAX_CONTROL_POINT_COUNT * MAX_NUMBER_OF_PATCHES; ++i )
	{
		for( unsigned int j(0); j < D3D11_GS_INPUT_REGISTER_COMPONENTS * D3D11_GS_INPUT_REGISTER_COUNT; j += 4 )
		{
			s_pPatchVertexData[i].m_Data[j] = (i + 1) * (j + 1);
			s_pPatchVertexData[i].m_Data[j+1] = (j + 1) * 7;
			s_pPatchVertexData[i].m_Data[j+2] = (i + 1) * 5;
			s_pPatchVertexData[i].m_Data[j+3] = j;
		}
	}
	return RESULT_PASS;
}


TEST_RESULT CStreamOutPatches::SetupTestCase()
{
	{
		TEST_RESULT tr = SetupSOClearObjects();
		if( tr != RESULT_PASS )
		{
			return tr;
		}
		tr = SetupRasterizer();
		if( tr != RESULT_PASS )
		{
			return tr;
		}
	}

	std::stringstream ss;
	std::vector<D3D11_INPUT_ELEMENT_DESC> elementDesc;
	{
		memset( m_soBufferDesc, 0, sizeof(m_soBufferDesc) );
		for( unsigned int i(0); i < m_StreamOutBufferCount; ++i )
		{
			m_soBufferDesc[i].BindFlags = D3D11_BIND_STREAM_OUTPUT;
		}
		const DXGI_FORMAT formats[] = { DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32A32_UINT };
		unsigned int index(0);
		for( unsigned int i(m_LayoutWindows[m_LayoutIndex].m_StartIndex);
			i < m_LayoutWindows[m_LayoutIndex].m_StartIndex + m_LayoutWindows[m_LayoutIndex].m_WindowCount;
			++i )
		{
			ss << "uint" << m_DataLayout[i].m_NumCompotents << " var" << i << " : Data" << index << ";";

			const D3D11_INPUT_ELEMENT_DESC e = 
			{
				"Data",
				index,
 				formats[m_DataLayout[i].m_NumCompotents-1],
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			};

			elementDesc.push_back(e);

			if( m_DataLayout[i].m_IgnoredDataGap )
			{
				++index;
				continue;
			}

			const unsigned int bufferIndex = rand() % m_StreamOutBufferCount;
			const unsigned int startComp = rand() % m_DataLayout[i].m_NumCompotents;
			const unsigned int compCount = rand() % (m_DataLayout[i].m_NumCompotents - startComp) + 1;
			 
			const D3D11_SO_DECLARATION_ENTRY entry = 
			{
				0,
				"Data",
				index++,
				(BYTE)startComp,
				(BYTE)compCount,
				(BYTE)bufferIndex
			};

			m_SOBufferDecls.push_back( entry );

			m_soBufferDesc[bufferIndex].ByteWidth += compCount * 4;
		}
	}

	{
		TCOMPtr<ID3D10Blob*> rShader;
		TCOMPtr<ID3D10Blob*> rError;
		TCOMPtr<ID3D10Blob*> rPreProc;
		{
			std::string layout = ss.str();
			const D3D10_SHADER_MACRO macros[] = 
			{
				{ "DEFINES_SET", "1" },
				{ "LAYOUT", layout.c_str() },
				NULL
			};

			HRESULT hr = GetFramework()->CompileShaderFromResource( 
				NULL, 
				"PassThroughVS.fx", 
				"main", 
				D3D_SHADER_STAGE_VERTEX,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_11_0,
				0,
				rShader,
				rError,
				macros,
				rPreProc );
			if( FAILED(hr) )
			{
				if( *rError )
				{
					WriteToLog( _T("%s"), rError->GetBufferPointer() );
					return RESULT_FAIL;
				}
				IFR(hr);
			}
			
			if( GetFramework()->LogVerbose() )
			{
				WriteMessage( _T("%s"), rPreProc->GetBufferPointer() );
			}
			
			IFR(GetDevice()->CreateVertexShader(
				rShader->GetBufferPointer(),
				rShader->GetBufferSize(),
				NULL,
				&m_pVS ));

			IFR(GetDevice()->CreateInputLayout(
				&elementDesc[0],
				elementDesc.size(),
				rShader->GetBufferPointer(),
				rShader->GetBufferSize(),
				&m_pInputLayout));
		}
			
		{
			const unsigned int strides[] = { m_soBufferDesc[0].ByteWidth, m_soBufferDesc[1].ByteWidth, m_soBufferDesc[2].ByteWidth, m_soBufferDesc[3].ByteWidth };
			IFR(GetDevice()->CreateGeometryShaderWithStreamOutput(
				rShader->GetBufferPointer(),
				rShader->GetBufferSize(),
				&m_SOBufferDecls[0],
				m_SOBufferDecls.size(),
				strides,
				m_StreamOutBufferCount,
				D3D11_SO_NO_RASTERIZED_STREAM,
				NULL,
				&m_pGS ));
		}
	}

	for( unsigned int i(0); i < m_StreamOutBufferCount; ++i )
	{
		m_soBufferDesc[i].ByteWidth *= (m_inputTopology - 32) * m_NumberOfPatchesToDraw * m_NumberOfDrawCalls;
	}

	{
		TEST_RESULT tr = CreateSOBuffers( m_soBuffers, m_soBufferDesc );
		if( tr != RESULT_PASS )
		{
			return tr;
		}
	}

	// Make the buffers for mapping back the data.
	for( unsigned int i(0); i < m_StreamOutBufferCount; ++i )
	{
		const D3D11_BUFFER_DESC desc = 
		{
			m_soBufferDesc[i].ByteWidth,
			D3D11_USAGE_STAGING,
			0,
			D3D11_CPU_ACCESS_READ,
			0,
			0
		};

		if( desc.ByteWidth > 0 )
		{
			IFR(GetDevice()->CreateBuffer( &desc, NULL, m_rBufferCopies[i] ));
		}
	}

	{
		const D3D11_BUFFER_DESC desc = 
		{
			sizeof(s_pPatchVertexData),
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_VERTEX_BUFFER,
			0,
			0,
			0
		};

		const D3D11_SUBRESOURCE_DATA data = 
		{
			s_pPatchVertexData,
			0,
			0
		};

		IFR(GetDevice()->CreateBuffer( &desc, &data, &m_pVB ));
	}
	return RESULT_PASS;
}

TEST_RESULT CStreamOutPatches::ExecuteTestCase()
{
	for( unsigned int i(0); i < sizeof(m_soBufferSize) / sizeof(UINT); ++i )
	{
		TEST_RESULT tr = ClearSOBuffer( m_soBuffers[i], m_soBufferDesc[i].ByteWidth );
		if( tr != RESULT_PASS )
		{
			return tr;
		}
	}

	GetEffectiveContext()->ClearState();

	GetEffectiveContext()->RSSetState( m_pRS );
	GetEffectiveContext()->OMSetDepthStencilState( m_pDSDisable, 0u );
	
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );
	GetEffectiveContext()->IASetPrimitiveTopology( m_inputTopology );
	{
		const UINT stride = D3D11_GS_INPUT_REGISTER_COMPONENTS * D3D11_GS_INPUT_REGISTER_COUNT * sizeof(unsigned int);
		const UINT offset = 0;
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVB, &stride, &offset );
	}

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );

	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	{
		const UINT offsets[] = { 0, 0, 0, 0 };
		GetEffectiveContext()->SOSetTargets( m_StreamOutBufferCount, m_soBuffers, offsets );
	}

	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );

	for( unsigned int i(0); i < m_NumberOfDrawCalls; ++i )
	{
		if( i > 0 )
		{
			const UINT offsets[] = { -1, -1, -1, -1 };
			ID3D11Buffer* pBuffers[] = { NULL, NULL, NULL, NULL };
			GetEffectiveContext()->SOSetTargets( m_StreamOutBufferCount, pBuffers, offsets );
			GetEffectiveContext()->SOSetTargets( m_StreamOutBufferCount, m_soBuffers, offsets );
		}
		GetEffectiveContext()->Draw( (static_cast<unsigned int>(m_inputTopology) - 32u) * m_NumberOfPatchesToDraw, 0 );
	}

	for( unsigned int i(0); i < m_StreamOutBufferCount; ++i )
	{
		if( m_soBufferDesc[i].ByteWidth > 0 )
		{
			GetEffectiveContext()->CopyResource( *m_rBufferCopies[i], m_soBuffers[i] );
		}
	}

	ExecuteEffectiveContext();

	D3D11_MAPPED_SUBRESOURCE mapped[4];
	for( unsigned int i(0); i < m_StreamOutBufferCount; ++i )
	{
		if( m_soBufferDesc[i].ByteWidth > 0 )
		{
			IFR(GetImmediateContext()->Map( 
				*m_rBufferCopies[i],
				0,
				D3D11_MAP_READ,
				0,
				mapped + i ));
		}
	}

	TEST_RESULT tr = RESULT_PASS;
	{
		unsigned int bufferComponentIndex[] = { 0, 0, 0, 0 };
		const LayoutWindow lw = m_LayoutWindows[m_LayoutIndex];

		for( unsigned int drawCall(0); drawCall < m_NumberOfDrawCalls; ++drawCall ) 
		{
			for( unsigned int primCount(0); primCount < m_NumberOfPatchesToDraw; ++primCount )
			{
				for( unsigned int controlPointIndex(0); controlPointIndex < static_cast<unsigned int>(m_inputTopology) - 32u; ++controlPointIndex )
				{
					// we're always starting from vertex 0, so draw call doesn't factor into this.
					const unsigned int srcDataOffset = primCount * (static_cast<unsigned int>(m_inputTopology) - 32u) + controlPointIndex;
					unsigned int soBufferDeclIndex(0);
					unsigned int vertexDataOffset(0);
					for( unsigned int dataLayoutIndex(lw.m_StartIndex); dataLayoutIndex < lw.m_StartIndex + lw.m_WindowCount; ++dataLayoutIndex )
					{
						const DataUnit du = m_DataLayout[dataLayoutIndex];
						if( du.m_IgnoredDataGap == false ) // src data gap
						{							
							unsigned int const* srcData = s_pPatchVertexData[srcDataOffset].m_Data; // source vertex
							srcData += vertexDataOffset; // source vertex location
							srcData += m_SOBufferDecls[soBufferDeclIndex].StartComponent; // start location of the copied vertex data
							unsigned int const* resData = static_cast<unsigned int const*>(mapped[m_SOBufferDecls[soBufferDeclIndex].OutputSlot].pData); // start location of the result buffer
							resData += bufferComponentIndex[m_SOBufferDecls[soBufferDeclIndex].OutputSlot]; // current offset in the result buffer

							if( memcmp( srcData, resData, m_SOBufferDecls[soBufferDeclIndex].ComponentCount * 4 ) != 0 )
							{
								WriteToLog( _T("src data does not match result data, buffer: %d, semantic index: %d, StartComp: %d, CompCount: %d, controlPoint: %d, Primitive: %d, DrawCall: %d"),
									m_SOBufferDecls[soBufferDeclIndex].OutputSlot,
									m_SOBufferDecls[soBufferDeclIndex].SemanticIndex,
									m_SOBufferDecls[soBufferDeclIndex].StartComponent,
									m_SOBufferDecls[soBufferDeclIndex].ComponentCount,
									controlPointIndex,
									primCount,
									drawCall );
								tr = RESULT_FAIL;
								break;
							}

							bufferComponentIndex[m_SOBufferDecls[soBufferDeclIndex].OutputSlot] += m_SOBufferDecls[soBufferDeclIndex].ComponentCount;
							++soBufferDeclIndex;
						}
						vertexDataOffset += du.m_NumCompotents;
					}
					if( tr != RESULT_PASS )
					{
						break;
					}
				}
				if( tr != RESULT_PASS )
				{
					break;
				}
			}
			if( tr != RESULT_PASS )
			{
				break;
			}
		}
	}

	for( unsigned int i(0); i < m_StreamOutBufferCount; ++i )
	{
		if( m_soBufferDesc[i].ByteWidth > 0 )
		{
			GetImmediateContext()->Unmap( *m_rBufferCopies[i], 0 );
		}
	}

	return tr;
}

void CStreamOutPatches::CleanupTestCase()
{
	CleanupSOClearObjects();
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pGS);
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pRS );
	SAFE_RELEASE( m_pDSDisable );
	
	for( unsigned int i(0); i < sizeof(m_soBufferSize) / sizeof(UINT); ++i )
	{
		SAFE_RELEASE(m_soBuffers[i]);
	}

	m_SOBufferDecls.clear();

} 

void CStreamOutPatches::Cleanup()
{	
}

CStreamOutPatches::Vertex CStreamOutPatches::s_pPatchVertexData[D3D11_IA_PATCH_MAX_CONTROL_POINT_COUNT * MAX_NUMBER_OF_PATCHES]; 
