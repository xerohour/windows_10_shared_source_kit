isabled );
	SAFE_RELEASE( m_pRSEnabled );
	SAFE_RELEASE( m_pSampler );
	SAFE_RELEASE( m_pVertexBuffer );
	
	if( m_pAdapterDescriptor )
	{
		delete m_pAdapterDescriptor;
		m_pAdapterDescriptor = nullptr;
	}
}

//---------------------------------------------------------------

void CRenderTargetsTest::CleanupTestCase()
{
	ID3D11ShaderResourceView *pNullSRV[ NUM_RESOURCES ] = { NULL, NULL, NULL };
	ID3D11RenderTargetView *pNullRTV[ NUM_RESOURCES ] = { NULL, NULL, NULL };
	GetEffectiveContext()->PSSetShaderResources( 0, NUM_RESOURCES, pNullSRV );
	GetEffectiveContext()->OMSetRenderTargets( 3, pNullRTV, NULL );
	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteE