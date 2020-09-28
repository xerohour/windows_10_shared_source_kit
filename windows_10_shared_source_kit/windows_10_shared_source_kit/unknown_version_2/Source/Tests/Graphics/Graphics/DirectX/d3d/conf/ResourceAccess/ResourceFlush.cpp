#include "ResourceFlush.h"

TEST_RESULT CResourceFlushTest::SetupTestCase()
{
	{
		ID3D11Query* pQ = NULL;
		HRESULT hr = GetDevice()->CreateQuery( &CD3D11_QUERY_DESC( D3D11_QUERY_EVENT ), &pQ );
		if (FAILED( hr ))
		{
			WriteToLog( _T( "CResourceFlushTest::SetupTestCase() - Failed to create event query" ) );
			return RESULT_FAIL;
		}

		GetImmediateContext()->ClearState();
		GetImmediateContext()->End( pQ );
		GetImmediateContext()->Flush();
		hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), pQ, NULL, 0, 0 );
		if (hr != S_OK)
		{
			WriteToLog( _T( "CResourceFlushTest::SetupTestCase() - Event query failed or timed out" ) );
			return RESULT_FAIL;
		}
		SAFE_RELEASE( pQ );
		GetImmediateContext()->Flush();
	}
	
	D3D11_BUFFER_DESC BufDesc = CD3D11_BUFFER_DESC( UINT( -1 ), D3D11_BIND_SHADER_RESOURCE );

	// Find largest resource that can be made:
	HRESULT hr;
	ID3D11Buffer* pRes = NULL;
	for (hr = GetFramework()->CreateBufferTiledOptional( &BufDesc, NULL, &pRes );
		FAILED( hr );
		hr = GetFramework()->CreateBufferTiledOptional( &BufDesc, NULL, &pRes ))
	{
		if (1 == BufDesc.ByteWidth)
		{
			WriteToLog( _T( "CResourceFlushTest::SetupTestCase() - Couldn't create any resources" ) );
			return RESULT_FAIL;
		}
		BufDesc.ByteWidth /= 2;
	}

	// Create as many of them as possible:
	try
	{
		const BYTE SrcData = 0;
		const D3D11_BOX DstBox = CD3D11_BOX( 0, 0, 0, 1, 1, 1 ); // Single Byte Update
		do
		{
			// Reference the object:
			GetImmediateContext()->UpdateSubresource( pRes, 0, &DstBox, &SrcData, sizeof( SrcData ), sizeof( SrcData ) );
			GetEffectiveContext()->UpdateSubresource( pRes, 0, &DstBox, &SrcData, sizeof( SrcData ), sizeof( SrcData ) );
			
			m_vBufs.push_back( pRes ); // throw( bad_alloc )
			pRes = NULL;
			hr = GetFramework()->CreateBufferTiledOptional( &BufDesc, NULL, &pRes );
		} while (SUCCEEDED( hr ));

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog(_T("ExecuteEffectiveContext() failed"));
			return RESULT_FAIL;
		}

		return RESULT_PASS;
	}
	catch (bad_alloc&)
	{
		SAFE_RELEASE( pRes );
		WriteToLog( _T( "CResourceFlushTest::SetupTestCase() - Failed to grow vector" ) );
		return RESULT_FAIL;
	}
}

TEST_RESULT CResourceFlushTest::ExecuteTestCase()
{
	D3D11_BUFFER_DESC BufDesc;
	(m_vBufs.back())->GetDesc( &BufDesc ); 

	// Destroy last resource:
	SAFE_RELEASE( m_vBufs.back() );

	// Wouldn't expect the re-creation to succeed without the flush:
	GetImmediateContext()->Flush();

	ID3D11Buffer* pRes = NULL;
	HRESULT hr = GetFramework()->CreateBufferTiledOptional( &BufDesc, NULL, &pRes );
	SAFE_RELEASE( pRes );
	if (FAILED( hr ))
	{
		WriteToLog( _T( "CResourceFlushTest::ExecuteTestCase() - Flush did not allow re-creation of resource" ) );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

void CResourceFlushTest::CleanupTestCase()
{
	for (vector< ID3D11Buffer* >::iterator itCur = m_vBufs.begin(); itCur != m_vBufs.end(); ++itCur)
	{
		SAFE_RELEASE( (*itCur) );
	}
	m_vBufs.clear();

	GetImmediateContext()->Flush();
}

