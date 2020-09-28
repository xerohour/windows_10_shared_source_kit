#include "filter.h"

BEGIN_NAMED_VALUES( CResourceMinLODScenario::eUpdateMethod )
NAMED_VALUE( _T( "UpdateSubresource" ), CResourceMinLODScenario::UpdateMethod_UpdateSubresource )
NAMED_VALUE( _T( "CopySubresource" ),   CResourceMinLODScenario::UpdateMethod_CopySubresource )
END_NAMED_VALUES( CResourceMinLODScenario::eUpdateMethod )

struct SRCDATA
{
	std::vector<BYTE> vPixels;
	UINT              RowPitch;
	UINT              SlicePitch;
};

// Fills a SRCDATA structure for a specific mip surface size with a solid color
static void GetSrcData(SRCDATA* pOut, const UINT uMipDimension[3], const BYTE Color[16], UINT uBPP)
{
	UINT uRowPitch          = uBPP        * uMipDimension[0];
	UINT uSlicePitch        = uRowPitch   * uMipDimension[1];
	UINT uTotalBytesNeeded  = uSlicePitch * uMipDimension[2];
	
	// Allocate memory for the pixels
	pOut->vPixels.resize( uTotalBytesNeeded );

	for( UINT z = 0; z < uMipDimension[2]; z++ )
	{
		BYTE* pDstSlice = &(pOut->vPixels[z * uSlicePitch]);

		for( UINT y = 0; y < uMipDimension[1]; y++ )
		{
			BYTE* pDstRow = pDstSlice + (y * uRowPitch);

			for( UINT x = 0; x < uMipDimension[0]; x++ )
			{
				BYTE* pDstPixel = pDstRow + (x * uBPP);

				memcpy( pDstPixel, Color, uBPP );
			}
		}
	}

	pOut->RowPitch   = uRowPitch;
	pOut->SlicePitch = uSlicePitch;
}

class CAsyncTextureUpdateQueue
{
public:
	struct CMD
	{
		ID3D11Resource*                         pTex;
		ID3D11Resource*                         pStaging; 
		UINT                                    uMip;
		UINT                                    uMipDimension[3];
		UINT                                    uBPP;
		CResourceMinLODScenario::eUpdateMethod  UpdateMethod;
		BYTE                                    bColor[16];
	};

	CAsyncTextureUpdateQueue();
	~CAsyncTextureUpdateQueue();

	HRESULT Init( ID3D11DeviceContext* pContext );
	void Enqueue( const CMD* Cmd );
	HRESULT DoOneUpdate( const CMD* Cmd );
	HRESULT WaitForMip( UINT uMip, ID3D11DeviceContext* pImmediateContext );
	void WaitForAll( ID3D11DeviceContext* pImmediateContext );

	void ThreadProc();

private:
	void Lock()   { EnterCriticalSection( &m_CS ); m_uLockCount++; }
	void Unlock() { assert( m_uLockCount > 0 ); m_uLockCount--; LeaveCriticalSection( &m_CS ); }

	D3D11_DEVICE_CONTEXT_TYPE GetContextType() { return m_ContextType; }

	typedef std::list< CMD > WORKQUEUE;
	WORKQUEUE                   m_WorkQueue;

	typedef std::list< ID3D11CommandList* > COMMANDLISTQUEUE;
	COMMANDLISTQUEUE            m_CommandListQueue;

	CRITICAL_SECTION            m_CS;
	D3D11_DEVICE_CONTEXT_TYPE   m_ContextType;
	UINT                        m_uLockCount;
	ID3D11DeviceContext*        m_pContext;
	HRESULT                     m_AsyncStatus;
	HANDLE                      m_hThread;
	bool                        m_bThreadShouldExit;
};

static DWORD TextureUpdateThreadProc( LPVOID pParam )
{
	((CAsyncTextureUpdateQueue*)pParam)->ThreadProc();
	return 0;
}

CAsyncTextureUpdateQueue::CAsyncTextureUpdateQueue()
{
	InitializeCriticalSection( &m_CS );
	m_ContextType = D3D11_DEVICE_CONTEXT_IMMEDIATE;
	m_uLockCount = 0;
	m_pContext = NULL;
	m_AsyncStatus = S_OK;
	m_hThread = NULL;
	m_bThreadShouldExit = false;
}

CAsyncTextureUpdateQueue::~CAsyncTextureUpdateQueue()
{
	// Ensure all threads have exited before continuing
	if( m_hThread )
	{
		assert( D3D11_DEVICE_CONTEXT_DEFERRED == GetContextType() );

		m_bThreadShouldExit = true;

		WaitForSingleObject( m_hThread, INFINITE );
		CloseHandle( m_hThread );
		m_hThread = NULL;
	}

	while( m_uLockCount )
	{
		Unlock();
	}

	for( COMMANDLISTQUEUE::iterator it = m_CommandListQueue.begin();
		 it != m_CommandListQueue.end();
		 ++it )
	{
		(*it)->Release();
	}

	DeleteCriticalSection( &m_CS );

}

HRESULT CAsyncTextureUpdateQueue::Init( ID3D11DeviceContext* pContext )
{
	HRESULT hr = S_OK;

	assert( !m_pContext ); // Init should only be called once
	assert( !m_hThread );
	assert( 0 == m_WorkQueue.size() );
	assert( 0 == m_CommandListQueue.size() );

	m_pContext = pContext;

	m_ContextType = pContext->GetType();

	// when using a deferred context, create a thread that will call into the deferred context
	if( D3D11_DEVICE_CONTEXT_DEFERRED == GetContextType() )
	{
		m_hThread = CreateThread( NULL, 0, TextureUpdateThreadProc, this, 0, NULL );
		if( !m_hThread )
		{
			hr = E_FAIL;
		}
	}

	return hr;
}

void CAsyncTextureUpdateQueue::Enqueue(const CMD* Cmd)
{
	assert( Cmd->uMip < D3D11_REQ_MIP_LEVELS ); // enables simple implementation of WaitForAll

	Lock();
	assert( 1 == m_uLockCount );
	m_WorkQueue.push_back( *Cmd );
	Unlock();
}

HRESULT CAsyncTextureUpdateQueue::WaitForMip(UINT uMip, ID3D11DeviceContext* pImmediateContext)
{
	HRESULT hr = S_OK;

	assert( D3D11_DEVICE_CONTEXT_IMMEDIATE == pImmediateContext->GetType() );

	// While there are still outstanding commands referencing uMip
	// and updating has been successful
	while( SUCCEEDED(hr) )
	{
		bool bPendingCommand = false;

		Lock();
		assert( 1 == m_uLockCount );

		for( WORKQUEUE::iterator it = m_WorkQueue.begin();
			it != m_WorkQueue.end();
			++it )
		{
			CMD& cmd = *it;

			if( cmd.uMip == uMip )
			{
				bPendingCommand = true;
			}
		}

		hr = m_AsyncStatus;

		Unlock();

		if( bPendingCommand )
		{
			if( D3D11_DEVICE_CONTEXT_DEFERRED == GetContextType() )
			{
				assert( m_hThread );
				Sleep( 0 ); // wait for the worker thread to do some work
			}
			else
			{
				assert( !m_hThread );

				CMD cmd = m_WorkQueue.front();
				m_WorkQueue.pop_front();

				m_AsyncStatus = DoOneUpdate( &cmd ); // do the update on this thread
			}
		}
		else
		{
			break;
		}
	}

	// Now execute all pending command lists on the immediate context
	COMMANDLISTQUEUE tempCommandListQueue;

	Lock();
	assert( 1 == m_uLockCount );
	tempCommandListQueue = m_CommandListQueue;
	m_CommandListQueue.clear();
	Unlock();

	for( COMMANDLISTQUEUE::iterator it = tempCommandListQueue.begin();
		 it != tempCommandListQueue.end();
		 ++it )
	{
		// Only deferred contexts should generate command lists
		assert( D3D11_DEVICE_CONTEXT_DEFERRED == GetContextType() );

		ID3D11CommandList* pCL = *it;

		pImmediateContext->ExecuteCommandList( pCL, TRUE );

		pCL->Release();
	}

	return hr;
}

void CAsyncTextureUpdateQueue::WaitForAll(ID3D11DeviceContext* pImmediateContext)
{
	if( m_hThread || !m_WorkQueue.empty() )
	{
		for( UINT i = 0; i < D3D11_REQ_MIP_LEVELS; i++ )
		{
			WaitForMip( i, pImmediateContext );
		}
	}
}

void CAsyncTextureUpdateQueue::ThreadProc()
{
	bool bThreadShouldExit = false;

	while( !bThreadShouldExit )
	{
		// Find work to do
		CMD cmd;
		bool bFoundCommand = false;

		Lock();
		assert( 1 == m_uLockCount );

		if( !m_WorkQueue.empty() )
		{
			cmd = m_WorkQueue.front();
			bFoundCommand = true;
		}

		// Check if the thread should exit
		bThreadShouldExit = m_bThreadShouldExit;

		assert( 1 == m_uLockCount );
		Unlock();

		// Do the work if any was found
		if( bFoundCommand )
		{
			HRESULT hr = DoOneUpdate( &cmd );

			// Remove the completed work item from the front of the queue
			Lock();
			assert( 1 == m_uLockCount );

			m_WorkQueue.pop_front();

			if( FAILED(hr) )
			{
				m_AsyncStatus = hr;
			}

			assert( 1 == m_uLockCount );
			Unlock();
		}

		Sleep(0); // Enable other threads to run
	}
}

HRESULT CAsyncTextureUpdateQueue::DoOneUpdate( const CMD* pCmd )
{
	HRESULT hr = S_OK;
   
	// Allocate memory for pixels
	SRCDATA srcData;
	GetSrcData( &srcData, pCmd->uMipDimension, pCmd->bColor, pCmd->uBPP );

	const BYTE* pPixels = &(srcData.vPixels[0]);

	switch( pCmd->UpdateMethod )
	{
	case CResourceMinLODScenario::UpdateMethod_UpdateSubresource:
		{
			m_pContext->UpdateSubresource( pCmd->pTex, pCmd->uMip, NULL, pPixels, srcData.RowPitch, srcData.SlicePitch );
		}
		break;

	case CResourceMinLODScenario::UpdateMethod_CopySubresource:
		{
			D3D11_MAPPED_SUBRESOURCE MappedSR;
			hr = m_pContext->Map( pCmd->pStaging, pCmd->uMip, D3D11_MAP_WRITE, 0, &MappedSR );

			if(SUCCEEDED(hr))
			{
				for( UINT z = 0; z < pCmd->uMipDimension[2]; z++ )
				{
					BYTE* pDstSlice = ((BYTE*)MappedSR.pData) + (MappedSR.DepthPitch * z);
					const BYTE* pSrcSlice = pPixels + (srcData.SlicePitch * z);

					for( UINT y = 0; y < pCmd->uMipDimension[1]; y++ )
					{
						BYTE* pDstScan = pDstSlice + (MappedSR.RowPitch * y);
						const BYTE* pSrcScan = pSrcSlice + (srcData.RowPitch * y);

						memcpy(pDstScan, pSrcScan, srcData.RowPitch);
					}
				}

				m_pContext->Unmap( pCmd->pStaging, pCmd->uMip );

				m_pContext->CopySubresourceRegion( pCmd->pTex, pCmd->uMip, 0, 0, 0, pCmd->pStaging, pCmd->uMip, NULL );
			}
		}
		break;

	default:
		assert( false ); // unknown update method
	}

	// Create command list if necessary
	if( SUCCEEDED(hr) &&
		(D3D11_DEVICE_CONTEXT_DEFERRED == GetContextType()))
	{
		ID3D11CommandList* pCL = NULL;

		hr = m_pContext->FinishCommandList( TRUE, &pCL );

		if( SUCCEEDED(hr) )
		{
			Lock();
			assert( 1 == m_uLockCount );

			m_CommandListQueue.push_back( pCL );

			assert( 1 == m_uLockCount );
			Unlock();
		}
	}

	return hr;
}

CResourceMinLODScenario::CResourceMinLODScenario()
{
	m_pSamplerState = NULL;
	m_pConstBuff = NULL;
	m_pVS = NULL;
	m_pGS = NULL;

	ZeroMemory( m_pPS, sizeof( m_pPS ) );
}

static UINT TEX_DIMENSIONS_XY[] = 
{
	256,
	512,
	1024,
	2048,
};

static UINT TEX_DIMENSIONS_Z[] = 
{
	1,
	2,
	4,
};

// The test assumes that the texture can has RGBA channels
static DXGI_FORMAT RGBA_FORMATS[] = 
{
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_B8G8R8A8_UNORM,
};

// Each lod step represents a different call to SetResourceMinLOD
// and an array slice in the output render target
CONST UINT RT_WIDTH         = 16;
CONST UINT RT_HEIGHT        = 16;
CONST DXGI_FORMAT RT_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

void CResourceMinLODScenario::InitTestParameters()
{
	AddParameter<bool>          ( _T("UseInitialData"),    &m_bUseInitialData );
	AddParameter<bool>          ( _T("UpdateDuringRender"),&m_bUpdateDuringRender );
	AddParameter<bool>          ( _T("ClampBeforeUpdate"), &m_bClampBeforeUpdate );
	AddParameter<eUpdateMethod> ( _T("UpdateMethod"),      &m_UpdateMethod );
	AddParameter<DXGI_FORMAT>   ( _T("Format"),            &m_TexFormat );
	AddParameter<UINT>          ( _T("Width"),             &m_TextureSize[0] );
	AddParameter<UINT>          ( _T("Height"),            &m_TextureSize[1] );
	AddParameter<UINT>          ( _T("Depth"),             &m_TextureSize[2] );
	AddParameter<UINT>          ( _T("Dimension"),         &m_TextureDimension );
	AddParameter<D3D11_USAGE>   ( _T("Usage"),             &m_TextureUsage );

	SetParameterDefaultValue<bool>          ( _T("UseInitialData"),    false );
	SetParameterDefaultValue<bool>          ( _T("UpdateDuringRender"),false );
	SetParameterDefaultValue<bool>          ( _T("ClampBeforeUpdate"), false );
	SetParameterDefaultValue<eUpdateMethod> ( _T("UpdateMethod"),      UpdateMethod_UpdateSubresource );
	SetParameterDefaultValue<DXGI_FORMAT>   ( _T("Format"),            DXGI_FORMAT_R16G16B16A16_FLOAT );
	SetParameterDefaultValue<UINT>          ( _T("Width"),             1 );
	SetParameterDefaultValue<UINT>          ( _T("Height"),            1 );
	SetParameterDefaultValue<UINT>          ( _T("Depth"),             1 );
	SetParameterDefaultValue<UINT>          ( _T("Dimension"),         1 );
	SetParameterDefaultValue<D3D11_USAGE>   ( _T("Usage"),             D3D11_USAGE_DEFAULT );

	testfactor::RFactor rUseInitialDataF = AddParameterValue<bool>( _T("UseInitialData"), false );
	testfactor::RFactor rUseInitialDataT = AddParameterValue<bool>( _T("UseInitialData"), true );
	testfactor::RFactor rUseInitialDataAll = rUseInitialDataF + rUseInitialDataT;

	testfactor::RFactor rUpdateDuringRenderF = AddParameterValue<bool>( _T("UpdateDuringRender"), false );
	testfactor::RFactor rUpdateDuringRenderT = AddParameterValue<bool>( _T("UpdateDuringRender"), true );
	testfactor::RFactor rUpdateDuringRenderAll = rUpdateDuringRenderF + rUpdateDuringRenderT;

	testfactor::RFactor rClampBeforeUpdateF = AddParameterValue<bool>( _T("ClampBeforeUpdate"), false );
	testfactor::RFactor rClampBeforeUpdateT = AddParameterValue<bool>( _T("ClampBeforeUpdate"), true );
	testfactor::RFactor rClampBeforeUpdateAll = rClampBeforeUpdateF + rClampBeforeUpdateT;

	testfactor::RFactor rUpdateMethodsDefault;
	
	// deferred contexts do not support mapping of staging buffers
	if( GetFramework()->IsDeferredContextUsed() )
	{
		rUpdateMethodsDefault = 
			AddParameterValue<CResourceMinLODScenario::eUpdateMethod>( _T("UpdateMethod"), UpdateMethod_UpdateSubresource );
	}
	else
	{
		rUpdateMethodsDefault = 
			AddParameterValue<CResourceMinLODScenario::eUpdateMethod>( _T("UpdateMethod"), UpdateMethod_UpdateSubresource )
			+ AddParameterValue<CResourceMinLODScenario::eUpdateMethod>( _T("UpdateMethod"), UpdateMethod_CopySubresource );
	}

	testfactor::RFactor rTexFormats = 
		AddParameterValueSet( _T("Format"), new CTableValueSet<DXGI_FORMAT>( RGBA_FORMATS, sizeof( DXGI_FORMAT ), ARRAY_SIZE( RGBA_FORMATS ) ) );

	testfactor::RFactor rWidthAll = 
		AddParameterValueSet( _T("Width"), new CTableValueSet<UINT>( TEX_DIMENSIONS_XY, sizeof( UINT ), ARRAY_SIZE( TEX_DIMENSIONS_XY ) ) );

	testfactor::RFactor rHeightAll = 
		AddParameterValueSet( _T("Height"), new CTableValueSet<UINT>( TEX_DIMENSIONS_XY, sizeof( UINT ), ARRAY_SIZE( TEX_DIMENSIONS_XY ) ) );

	testfactor::RFactor rDepthAll = 
		AddParameterValueSet( _T("Depth"), new CTableValueSet<UINT>( TEX_DIMENSIONS_Z, sizeof( UINT ), ARRAY_SIZE( TEX_DIMENSIONS_Z ) ) );
	
	testfactor::RFactor rDimension1 = AddParameterValue<UINT>( _T("Dimension"), 1 );
	testfactor::RFactor rDimension2 = AddParameterValue<UINT>( _T("Dimension"), 2 );
	testfactor::RFactor rDimension3 = AddParameterValue<UINT>( _T("Dimension"), 3 );

	testfactor::RFactor rUsageDefault   = AddParameterValue<D3D11_USAGE>( _T("Usage"), D3D11_USAGE_DEFAULT );
	testfactor::RFactor rUsageImmutable = AddParameterValue<D3D11_USAGE>( _T("Usage"), D3D11_USAGE_IMMUTABLE );

	testfactor::RFactor rDimensionAndSize = (rDimension1 * rWidthAll) 
										  + (rDimension2 * rWidthAll % rHeightAll) 
										  + (rDimension3 * rWidthAll % (rHeightAll * rDepthAll));

	
	testfactor::RFactor rDefault = rUsageDefault * rUseInitialDataAll * rUpdateDuringRenderAll * rClampBeforeUpdateAll * rUpdateMethodsDefault;
	// immutable resources can't be updated
	testfactor::RFactor rImmutable = rUsageImmutable * rUseInitialDataT * rUpdateDuringRenderF * rClampBeforeUpdateT;

	SetRootTestFactor( rDimensionAndSize * rTexFormats * (rDefault + rImmutable) );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Width"), 512), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
	
	// Skip test cases where both of these parameters = false
	// in these test cases, the texture would not be initialized before being sampled from
	SkipMatching( FilterEqual<bool>(_T("UpdateDuringRender"), false) && 
				  FilterEqual<bool>(_T("UseInitialData"), false) );
}

TEST_RESULT CResourceMinLODScenario::Setup()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = S_OK;

	// Constant buffer
	{
		D3D11_BUFFER_DESC cbDesc = CD3D11_BUFFER_DESC( sizeof(UINT) * 4, D3D11_BIND_CONSTANT_BUFFER );
		hr = GetFramework()->CreateBufferTiledOptional( &cbDesc, NULL, &m_pConstBuff );
		if( FAILED(hr) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Failed to create constant buffer.  hr = %s"), D3DHResultToString(hr).c_str() );
			goto testDone;
		}
	}

	// Shaders
	{
		struct
		{
			const char*             entryPoint;
			D3D_SHADER_STAGE        stage;
			ID3D11VertexShader**    ppVS;
			ID3D11GeometryShader**  ppGS;
			ID3D11PixelShader**     ppPS;
		} shaderInfo[] = 
		{
			{ "VSEntry",    D3D_SHADER_STAGE_VERTEX,    &m_pVS, NULL,   NULL },
			{ "GSEntry",    D3D_SHADER_STAGE_GEOMETRY,  NULL,   &m_pGS, NULL },
			{ "PSEntry1D",  D3D_SHADER_STAGE_PIXEL,     NULL,   NULL,   &m_pPS[0] },
			{ "PSEntry2D",  D3D_SHADER_STAGE_PIXEL,     NULL,   NULL,   &m_pPS[1] },
			{ "PSEntry3D",  D3D_SHADER_STAGE_PIXEL,     NULL,   NULL,   &m_pPS[2] },
		};

		for( UINT i = 0; i < ARRAY_SIZE(shaderInfo); i++ )
		{
			hr = CompileShader( this, 
								_T("minlod.sh"),
								shaderInfo[i].entryPoint,
								shaderInfo[i].stage,
								shaderInfo[i].ppVS,
								shaderInfo[i].ppGS,
								shaderInfo[i].ppPS,
								NULL );

			if( FAILED(hr) )
			{   
				tRes = RESULT_FAIL;
				WriteToLog( _T("Failed to create a shader.  hr = %s"), D3DHResultToString(hr).c_str() );
				goto testDone;
			}
		}
	}

	// Sampler 
	{
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC( D3D11_DEFAULT );
		assert(D3D11_FILTER_MIN_MAG_MIP_LINEAR == samplerDesc.Filter); // GetExpectedColor assumes this

		hr = GetDevice()->CreateSamplerState( &samplerDesc, &m_pSamplerState );
		if( FAILED(hr) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Failed to create a sampler state.  hr = %s"), D3DHResultToString(hr).c_str() );
			goto testDone;
		}
	}

testDone:
	return tRes;
}

void CResourceMinLODScenario::Cleanup()
{
	SAFE_RELEASE( m_pSamplerState );
	SAFE_RELEASE( m_pConstBuff );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );

	for( UINT i = 0; i < ARRAY_SIZE(m_pPS); i++ )
	{
		SAFE_RELEASE( m_pPS[i] );
	}
}

static UINT GetMipCountOneDimension(UINT i)
{
	UINT uResult = 0;

	while(i)
	{
		uResult++;
		i >>= 1;
	}

	return uResult;
}

UINT CResourceMinLODScenario::GetMipCount()
{
	UINT uMipCount = 1;

	for (UINT i = 0; i < m_TextureDimension; i++)
	{
		uMipCount = max(uMipCount, GetMipCountOneDimension(m_TextureSize[i]));
	}

	return uMipCount;
}

float GetResourceMinLOD(UINT uLodStep, UINT uMipCount)
{
	// if SetResourceMinLOD is called with a value larger than this
	// then out-of-bounds behavior will be invoked
	// which this test case does not cover
	const float fMaxMinLOD = (float)uMipCount - 1;

	float fResult = fMaxMinLOD - ((uLodStep * fMaxMinLOD) / (float)CResourceMinLODScenario::NUM_LOD_STEPS);

	assert( fResult >= 0.0f );
	assert( fResult <= fMaxMinLOD );

	return fResult;
}

// Convert a R8G8B8A8_UNORM value to "format"
static bool ConvertColor(DXGI_FORMAT format, UINT colorIn, BYTE colorOut[16])
{
	// From R8G8B8A8_UNORM to FLOAT_COLOR
	FLOAT_COLOR floatColor;
	bool bRes = CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, DXGI_FORMAT_R8G8B8A8_UNORM, &floatColor, 1, 1, &colorIn );
	assert( bRes ); // can only fail because of a bug in this code
	if ( !bRes )
	{
		return false;
	}

	// From FLOAT_COLOR to format
	bRes = CFormatHelper::ConvertPixels( CONV_TO_FORMAT, format, &floatColor, 1, 1, colorOut );
	assert( bRes );
	if ( !bRes )
	{
		return false;
	}

	return true;
}

TEST_RESULT CResourceMinLODScenario::ExecuteTestCase()
{
	HRESULT hr = S_OK;
	TEST_RESULT tRes = RESULT_PASS;

	CAsyncTextureUpdateQueue updateQueue;
	m_uNextColorIndex = 0;
	ZeroMemory( m_ExpectedColor, sizeof( m_ExpectedColor ) );
	ZeroMemory( m_LastGeneratedColor, sizeof( m_LastGeneratedColor ) );

	ID3D11Resource* pTexture = NULL;
	ID3D11Resource* pTextureStaging = NULL;
	ID3D11ShaderResourceView* pSRV = NULL;
	ID3D11Texture2D* pRenderTarget = NULL;
	ID3D11RenderTargetView* pRTV = NULL;

	// If none of these are set, then the device will render from an unintialized surface
	assert( m_bUpdateDuringRender || m_bUseInitialData );

	// Initialize asynchronous texture update queue
	ID3D11DeviceContext* pContext = GetEffectiveContext();

	// Compute number of mip levels based on m_TextureSize[]
	UINT uMipCount = GetMipCount();
	
	// Create texture & SRV
	tRes = CreateTexture(&pTexture, &pTextureStaging, &pSRV);
	if( tRes != RESULT_PASS )
	{
		tRes = RESULT_SKIP; // out-of-memory errors are expected
		goto testDone;
	}

	// Create render target
	tRes = CreateRenderTarget(NUM_LOD_STEPS, &pRenderTarget, &pRTV);
	if (tRes != RESULT_PASS)
		goto testDone;
	
	hr = ExecuteEffectiveContext();
	if( FAILED(hr) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("Failed to execute deferred context. hr = %s"), D3DHResultToString(hr).c_str() );
		goto testDone;
	}
	
	hr = updateQueue.Init( pContext );
	if( FAILED(hr) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("Failed to initialize asynchronous texture update queue.  hr = %s"), D3DHResultToString(hr).c_str() );
		goto testDone;
	}

	// Create texture
	if( m_bUpdateDuringRender )
	{
		assert( D3D11_USAGE_IMMUTABLE != m_TextureUsage );
		
		// Update the texture contents (possibly asynchronously)
		for (UINT i = 0; i < uMipCount; i++)
		{
			UINT uMipIdx = uMipCount - i - 1; // start with the smallest mip level first
			
			UINT uColor = GetMipColor(uMipIdx);

			CAsyncTextureUpdateQueue::CMD Cmd;
			Cmd.pTex = pTexture;
			Cmd.pStaging = pTextureStaging;
			Cmd.uMip = uMipIdx;
			Cmd.uBPP = GetBPP();
			Cmd.UpdateMethod = m_UpdateMethod;
			GetMipDimension( uMipIdx, Cmd.uMipDimension );

			// Convert uColor to m_TexFormat and store in Cmd.bColor
			if ( !ConvertColor( m_TexFormat, uColor, Cmd.bColor ))
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T("CResourceMinLODScenario::ExecuteTestCase() - ConvertColor() failed") );
				goto testDone;
			}
			
			updateQueue.Enqueue( &Cmd );
		}
	}

	for (UINT uLodStep = 0; uLodStep < NUM_LOD_STEPS; uLodStep++)
	{
		const float fResourceMinLOD = GetResourceMinLOD( uLodStep, uMipCount );

		UINT uLowestMipToUse = (UINT)fResourceMinLOD;
		assert( uLowestMipToUse < uMipCount );
		assert( uLowestMipToUse < ARRAY_SIZE( m_LastGeneratedColor ) );

		if( m_bClampBeforeUpdate )
		{
			GetImmediateContext()->SetResourceMinLOD( pTexture, fResourceMinLOD );
		}

		// Wait all updates to mip-map levels coressponding uLowestMipToUse and higher to be completed
		hr = updateQueue.WaitForMip( uLowestMipToUse, GetImmediateContext() );
		if( FAILED(hr) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Asynchronous texture update failure.  hr = %s"), D3DHResultToString(hr).c_str() );
			goto testDone;
		}

		if( !m_bClampBeforeUpdate )
		{
			GetImmediateContext()->SetResourceMinLOD( pTexture, fResourceMinLOD );
		}
		
		Render( uLodStep, pRTV, pSRV );

		// Remember the color of the mip-map level that should have been sampled for this rendering operation
		m_ExpectedColor[ uLodStep ] = GetExpectedColor( uLowestMipToUse, fResourceMinLOD - floor( fResourceMinLOD ) );
	}

	tRes = CheckResults( pRenderTarget );

testDone:
	updateQueue.WaitForAll( GetImmediateContext() ); // ensure all threads have completed before returning

	SAFE_RELEASE( pTexture );
	SAFE_RELEASE( pTextureStaging );
	SAFE_RELEASE( pSRV );
	SAFE_RELEASE( pRenderTarget );
	SAFE_RELEASE( pRTV );

	return tRes;
}


TEST_RESULT CResourceMinLODScenario::CreateRenderTarget(UINT uArraySize, ID3D11Texture2D** ppRT, ID3D11RenderTargetView** ppRTV)
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = S_OK;

	ID3D11Texture2D* pTex = NULL;
	ID3D11RenderTargetView* pRTV = NULL;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory( &texDesc, sizeof(texDesc) );

	texDesc.Width               = RT_WIDTH;
	texDesc.Height              = RT_HEIGHT;
	texDesc.MipLevels           = 1;
	texDesc.ArraySize           = uArraySize;
	texDesc.Format              = RT_FORMAT;
	texDesc.SampleDesc.Count    = 1;
	texDesc.SampleDesc.Quality  = 0;
	texDesc.Usage               = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags           = D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags      = 0;
	texDesc.MiscFlags           = 0;

	hr = GetFramework()->CreateTexture2DTiledOptional( &texDesc, NULL, &pTex );
	if (FAILED(hr))
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("CreateTexture2D failed for ender target.  hr = %s"), D3DHResultToString(hr).c_str() );
		goto testDone;
	}

	hr = GetDevice()->CreateRenderTargetView( pTex, NULL, &pRTV );
	if (FAILED(hr))
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("CreateRenderTargetView failed.  hr = %s"), D3DHResultToString(hr).c_str() );
		goto testDone;
	}

testDone:
	if (RESULT_PASS == tRes)
	{
		*ppRT  = pTex;
		*ppRTV = pRTV;

		pTex->AddRef();
		pRTV->AddRef();
	}
	
	SAFE_RELEASE(pTex);
	SAFE_RELEASE(pRTV);

	return tRes;
}

UINT CResourceMinLODScenario::GetBPP()
{
	UINT uBlockWidth;
	UINT uBlockHeight;
	UINT uBlockBytes;

	CFormatHelper::GetBlockSize( m_TexFormat, &uBlockWidth, &uBlockHeight, &uBlockBytes );
	assert( 1 == uBlockWidth ); // compressed formats are not supported by this test
	assert( 1 == uBlockHeight );
	assert( uBlockBytes <= 16 ); // 16 is hard-coded in this test in a few places 

	return uBlockBytes;
}

TEST_RESULT CResourceMinLODScenario::CreateTexture(ID3D11Resource** ppResource, ID3D11Resource** ppStaging, ID3D11ShaderResourceView** ppSRV)
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = S_OK;

	ID3D11Resource* pResource = NULL;
	ID3D11Resource* pStaging = NULL;
	ID3D11ShaderResourceView* pSRV = NULL;
	
	UINT uMipCount = GetMipCount();

	D3D11_SUBRESOURCE_DATA* pSRD = NULL;

	assert( uMipCount < D3D11_REQ_MIP_LEVELS );

	D3D11_SUBRESOURCE_DATA SRD[ D3D11_REQ_MIP_LEVELS ];
	SRCDATA SrcData[ D3D11_REQ_MIP_LEVELS ];
	
	if( m_bUseInitialData )
	{
		for( UINT uMip = 0; uMip < uMipCount; uMip++ )
		{
			// Determine mip color (in m_TexFormat)
			UINT uColor = GetMipColor( uMip );
			BYTE bColor[16];
			if ( !ConvertColor( m_TexFormat, uColor, bColor ))
			{
				tRes = RESULT_FAIL;
				WriteToLog(_T("CCResourceMinLODScenario::CreateTexture - ConvertColor() failed"));
				goto testDone;
			}

			// Determine mip dimensions
			UINT uMipDim[3];
			GetMipDimension( uMip, uMipDim );

			// Determine bytes per pixel
			UINT uBPP = GetBPP();

			// Allocate memory of initial data
			GetSrcData( &SrcData[uMip], uMipDim, bColor, uBPP );

			SRD[uMip].pSysMem          = &(SrcData[uMip].vPixels[0]);
			SRD[uMip].SysMemPitch      = SrcData[uMip].RowPitch;
			SRD[uMip].SysMemSlicePitch = SrcData[uMip].SlicePitch;
		}

		pSRD = SRD;
	}

	const UINT uCPUAccessFlags = (m_TextureUsage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;

	switch(m_TextureDimension)
	{
	case 1:
		{
			ID3D11Texture1D* pTex1D = NULL;

			D3D11_TEXTURE1D_DESC texDesc;
			ZeroMemory( &texDesc, sizeof(texDesc) );

			texDesc.Width           = m_TextureSize[0];
			texDesc.MipLevels       = uMipCount;
			texDesc.ArraySize       = 1;
			texDesc.Format          = m_TexFormat;
			texDesc.Usage           = m_TextureUsage;
			texDesc.BindFlags       = D3D11_BIND_SHADER_RESOURCE;
			texDesc.CPUAccessFlags  = uCPUAccessFlags;
			texDesc.MiscFlags       = D3D11_RESOURCE_MISC_RESOURCE_CLAMP;

			hr = GetDevice()->CreateTexture1D( &texDesc, pSRD, &pTex1D );
			if(FAILED(hr))
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T("CreateTexture1D failed.  hr = %s"), D3DHResultToString(hr).c_str() );
				goto testDone;
			}

			pResource = pTex1D;
			pTex1D = NULL;

			texDesc.Usage           = D3D11_USAGE_STAGING;
			texDesc.BindFlags       = 0;
			texDesc.CPUAccessFlags  = D3D11_CPU_ACCESS_WRITE;
			texDesc.MiscFlags       = 0;

			hr = GetDevice()->CreateTexture1D( &texDesc, NULL, &pTex1D );
			if(FAILED(hr))
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T("CreateTexture1D (staging) failed.  hr = %s"), D3DHResultToString(hr).c_str() );
				goto testDone;
			}

			pStaging = pTex1D;
			pTex1D = NULL;
		}
		break;

	case 2:
		{
			ID3D11Texture2D* pTex2D = NULL;

			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory( &texDesc, sizeof(texDesc) );

			texDesc.Width               = m_TextureSize[0];
			texDesc.Height              = m_TextureSize[1];
			texDesc.MipLevels           = uMipCount;
			texDesc.ArraySize           = 1;
			texDesc.Format              = m_TexFormat;
			texDesc.SampleDesc.Count    = 1;
			texDesc.SampleDesc.Quality  = 0;
			texDesc.Usage               = m_TextureUsage;
			texDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
			texDesc.CPUAccessFlags      = uCPUAccessFlags;
			texDesc.MiscFlags           = D3D11_RESOURCE_MISC_RESOURCE_CLAMP;

			hr = GetFramework()->CreateTexture2DTiledOptional( &texDesc, pSRD, &pTex2D );
			if(FAILED(hr))
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T("CreateTexture2D failed.  hr = %s"), D3DHResultToString(hr).c_str() );
				goto testDone;
			}

			pResource = pTex2D;
			pTex2D = NULL;

			texDesc.Usage           = D3D11_USAGE_STAGING;
			texDesc.BindFlags       = 0;
			texDesc.CPUAccessFlags  = D3D11_CPU_ACCESS_WRITE;
			texDesc.MiscFlags       = 0;

			hr = GetFramework()->CreateTexture2DTiledOptional( &texDesc, NULL, &pTex2D );
			if(FAILED(hr))
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T("CreateTexture2D (staging) failed.  hr = %s"), D3DHResultToString(hr).c_str() );
				goto testDone;
			}

			pStaging = pTex2D;
			pTex2D = NULL;
		}
		break;

	case 3:
		{
			ID3D11Texture3D* pTex3D = NULL;

			D3D11_TEXTURE3D_DESC texDesc;
			ZeroMemory( &texDesc, sizeof(texDesc) );

			texDesc.Width               = m_TextureSize[0];
			texDesc.Height              = m_TextureSize[1];
			texDesc.Depth               = m_TextureSize[2];
			texDesc.MipLevels           = uMipCount;
			texDesc.Format              = m_TexFormat;
			texDesc.Usage               = m_TextureUsage;
			texDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
			texDesc.CPUAccessFlags      = uCPUAccessFlags;
			texDesc.MiscFlags           = D3D11_RESOURCE_MISC_RESOURCE_CLAMP;

			hr = GetDevice()->CreateTexture3D( &texDesc, pSRD, &pTex3D );
			if(FAILED(hr))
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T("CreateTexture3D failed.  hr = %s"), D3DHResultToString(hr).c_str() );
				goto testDone;
			}

			pResource = pTex3D;
			pTex3D = NULL;

			texDesc.Usage           = D3D11_USAGE_STAGING;
			texDesc.BindFlags       = 0;
			texDesc.CPUAccessFlags  = D3D11_CPU_ACCESS_WRITE;
			texDesc.MiscFlags       = 0;

			hr = GetDevice()->CreateTexture3D( &texDesc, NULL, &pTex3D );
			if(FAILED(hr))
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T("CreateTexture3D (staging) failed.  hr = %s"), D3DHResultToString(hr).c_str() );
				goto testDone;
			}

			pStaging = pTex3D;
			pTex3D = NULL;
		}
		break;

	default:
		assert( false ); // unknown texture dimension
	}

	assert(RESULT_PASS == tRes);
	hr = GetDevice()->CreateShaderResourceView( pResource, NULL, &pSRV );
	if(FAILED(hr))
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("CreateShaderResourceView failed.  hr = %s"), D3DHResultToString(hr).c_str() );
		goto testDone;
	}

testDone:
	if(RESULT_PASS == tRes)
	{
		*ppResource = pResource;
		*ppStaging = pStaging;
		*ppSRV = pSRV;

		pResource->AddRef();
		pStaging->AddRef();
		pSRV->AddRef();
	}

	SAFE_RELEASE(pResource);
	SAFE_RELEASE(pStaging);
	SAFE_RELEASE(pSRV);
	return tRes;
}

// The set of R8G8B8A8_UNORM colors used to color mip levels
static const UINT COLORS[] = 
{
	0x00FFFF33,
	0x003300AA,
	0x003300FF,
	0x0000FFFF,
	0x00330000,
	0x00330033,
	0x00003300,
	0x00FFFFAA,
	0x00000033,
	0x000000AA,
	0x000000FF,
	0x00AA33FF,
	0x00AA33AA,
	0x00FFAAFF,
	0x00FFFF00,
	0x0000FF00,
	0x0000AA00,
	0x00AA3300,
	0x00AA3333,
	0x0000AAFF,
	0x0000FF33,
	0x0000FFAA,
	0x000033AA,
	0x000033FF,
	0x0000AA33,
	0x0000AAAA,
	0x00FFAA00,
	0x00FFAA33,
	0x00FFAAAA,
	0x00003333,
};

void CResourceMinLODScenario::GetMipDimension(UINT uMip, UINT uMipDimension[3])
{
	for( UINT i = 0; i < 3; i++ )
	{
		uMipDimension[i] = m_TextureSize[i];
	}

	for( UINT u = 0; u < uMip; u++ )
	{
		for( UINT i = 0; i < 3; i++ )
		{
			uMipDimension[i] >>= 1;

			if( uMipDimension[i] == 0 )
			{
				uMipDimension[i] = 1;
			}
		}
	}
}

UINT CResourceMinLODScenario::GetMipColor(UINT uMip)
{
	assert( uMip < ARRAY_SIZE( m_LastGeneratedColor ) );

	// Fill in the pixels with the color specified by m_uColorIndex;
	UINT uColor = COLORS[ m_uNextColorIndex % (ARRAY_SIZE(COLORS)) ];
	m_uNextColorIndex++; // incremented so that next time a different color will be chosen

	// Remember the last color that was placed into this mip level
	m_LastGeneratedColor[ uMip ] = uColor;

	return uColor;
}

void CResourceMinLODScenario::Render(UINT uArraySlice, ID3D11RenderTargetView* pRTV, ID3D11ShaderResourceView* pSRV)
{
	// Bind resources
	D3D11_VIEWPORT vp = { 0, 0, RT_WIDTH, RT_HEIGHT, 0, 1 };
	GetImmediateContext()->RSSetViewports( 1, &vp );

	GetImmediateContext()->VSSetShader( m_pVS, NULL, 0 );
	GetImmediateContext()->GSSetShader( m_pGS, NULL, 0 );
	GetImmediateContext()->GSSetConstantBuffers( 0, 1, &m_pConstBuff );
	GetImmediateContext()->VSSetSamplers( 0, 1, &m_pSamplerState );
	GetImmediateContext()->GSSetSamplers( 0, 1, &m_pSamplerState );
	GetImmediateContext()->PSSetSamplers( 0, 1, &m_pSamplerState );
	GetImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	
	assert( m_TextureDimension > 0 );
	assert( m_TextureDimension < 4 );
	GetImmediateContext()->PSSetShader( m_pPS[ m_TextureDimension-1 ], NULL, 0 );

	GetImmediateContext()->OMSetRenderTargets( 1, &pRTV, NULL );
	GetImmediateContext()->VSSetShaderResources( 0, 1, &pSRV );
	GetImmediateContext()->GSSetShaderResources( 0, 1, &pSRV );
	GetImmediateContext()->PSSetShaderResources( 0, 1, &pSRV );

	// Put uArraySlice into the constant buffer
	UINT cbData[4] = { uArraySlice, 0, 0, 0 };
	GetImmediateContext()->UpdateSubresource(m_pConstBuff, 0, NULL, cbData, 0, 0);

	// Render
	GetImmediateContext()->Draw(1, 0);

	//
	// Flush after every 3 draw calls to 
	// cause the driver to generate a new allocation list
	// this helps to test that the driver puts the correct allocations into the allocation list
	//
	if( 0 == (uArraySlice % 3) )
	{
		GetImmediateContext()->Flush();
	}
}

TEST_RESULT CResourceMinLODScenario::CheckResults(ID3D11Texture2D* pRT)
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = S_OK;

	ID3D11Texture2D* pStaging = NULL;

	// Create a staging buffer
	D3D11_TEXTURE2D_DESC stagingDesc;
	pRT->GetDesc( &stagingDesc );

	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	stagingDesc.MiscFlags = 0;

	hr = GetFramework()->CreateTexture2DTiledOptional( &stagingDesc, NULL, &pStaging );
	if( FAILED(hr) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("Failed to create a staging buffer.  hr = %s"), D3DHResultToString(hr).c_str() );
		goto testDone;
	}
	
	hr = ExecuteEffectiveContext();
	if( FAILED(hr) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("Failed to execute deferred context. hr = %s"), D3DHResultToString(hr).c_str() );
		goto testDone;
	}

	// Copy render target contents to staging buffer
	GetImmediateContext()->CopyResource( pStaging, pRT );

	// Verify contents of staging buffer
	for (UINT uLodStep = 0; uLodStep < NUM_LOD_STEPS; uLodStep++)
	{
		UINT uSR = D3D11CalcSubresource( 0, uLodStep, 1 );

		D3D11_MAPPED_SUBRESOURCE mappedSR;
		hr = GetImmediateContext()->Map( pStaging, uSR, D3D11_MAP_READ, 0, &mappedSR );
		if( FAILED(hr) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Failed to map a staging buffer.  hr = %s"), D3DHResultToString(hr).c_str() );
			goto testDone;
		}

		// The sampler should have choosen the most detailed mip level that was allowed by fResourceMinLOD
		float fResourceMinLOD = GetResourceMinLOD( uLodStep, GetMipCount() );
		UINT uMip = (UINT)floor( fResourceMinLOD );
		assert( uMip < GetMipCount() );

		FLOAT_COLOR expectedColor = m_ExpectedColor[ uLodStep ];

		FLOAT_COLOR actualColor;
		bool bRes = CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, RT_FORMAT, &actualColor, 1, 1, mappedSR.pData );
		assert( bRes ); // can only fail because of a bug in this code

		bool bCompareResult = true;

		for( UINT i = 0; i < 4; i++ )
		{
			bCompareResult = bCompareResult && CompareFloatEpsilon( expectedColor.comp[i], actualColor.comp[i], 1.0f / 32.0f );
		}

		GetImmediateContext()->Unmap( pStaging, uSR );

		if( !bCompareResult )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Image comparison failed.  Expected (%f, %f, %f, %f).  Found (%f, %f, %f, %f).  ResourceMinLOD = %f, MipIndex = %u"),
						expectedColor.r, expectedColor.g, expectedColor.b, expectedColor.a,
						actualColor.r, actualColor.g, actualColor.b, actualColor.a,
						fResourceMinLOD,
						uMip );
			goto testDone;
		}

	}

testDone:
	SAFE_RELEASE( pStaging );
	return tRes;
}

// Given a mip level, determine the color that the hardware sampler should produce
FLOAT_COLOR CResourceMinLODScenario::GetExpectedColor(UINT uMipFloor, float fFrc)
{
	UINT uMipCount = GetMipCount();

	assert( fFrc < 1.0f );
	assert( uMipFloor < uMipCount );

	UINT uMipCeil = uMipFloor + 1;
	if( uMipCeil >= uMipCount )
	{
		uMipCeil = uMipFloor;
	}

	assert( uMipCeil < ARRAY_SIZE(m_LastGeneratedColor) );
	assert( uMipFloor < ARRAY_SIZE(m_LastGeneratedColor) );

	FLOAT_COLOR colorFloor;
	FLOAT_COLOR colorCeil;

	bool bRes = CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, 
											  DXGI_FORMAT_R8G8B8A8_UNORM, 
											  &colorFloor, 
											  1, 
											  1, 
											  &m_LastGeneratedColor[ uMipFloor ] );
	assert( bRes ); // can only fail because of a bug in this code

	CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, 
								  DXGI_FORMAT_R8G8B8A8_UNORM, 
								  &colorCeil, 
								  1, 
								  1, 
								  &m_LastGeneratedColor[ uMipCeil ] );
	assert( bRes );

	FLOAT_COLOR result;

	for( UINT i = 0; i < 4; i++ )
	{
		result.comp[i] = colorFloor.comp[i] + (colorCeil.comp[i] - colorFloor.comp[i]) * fFrc;
	}

	return result;
}