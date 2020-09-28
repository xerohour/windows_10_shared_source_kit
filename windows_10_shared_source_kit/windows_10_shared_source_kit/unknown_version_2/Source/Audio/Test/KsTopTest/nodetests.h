.99f));
	g_cubeV[13] = VOLVERTEX(D3DXVECTOR3(  0.33f, -1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f,-0.99f, -0.99f));
	g_cubeV[14] = VOLVERTEX(D3DXVECTOR3(  1.00f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f,-0.99f,  0.99f));
	g_cubeV[15] = VOLVERTEX(D3DXVECTOR3(  1.00f, -1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f,-0.99f, -0.99f));
	g_cubeV[16] = VOLVERTEX(D3DXVECTOR3( -0.33f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f,  0.99f,  0.99f));
	g_cubeV[17] = VOLVERTEX(D3DXVECTOR3( -0.33f, -1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f, -0.99f,  0.99f));
	g_cubeV[18] = VOLVERTEX(D3DXVECTOR3(  0.33f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f,  0.99f,  0.99f));
	g_cubeV[19] = VOLVERTEX(D3DXVECTOR3(  0.33f, -1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f, -0.99f,  0.99f));
	g_cubeV[20] = VOLVERTEX(D3DXVECTOR3( -1.00f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f,  0.99f, -0.99f));
	g_cubeV[21] = VOLVERTEX(D3DXVECTOR3( -1.00f, -1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( -0.99f, -0.99f, -0.99f));
	g_cubeV[22] = VOLVERTEX(D3DXVECTOR3( -0.33f,  0.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f,  0.99f, -0.99f));
	g_cubeV[23] = VOLVERTEX(D3DXVECTOR3( -0.33f, -1.00f, 0.f), D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3(  0.99f, -0.99f, -0.99f));

	if ( !CreateVertexBuffer( 24*sizeof(VOLVERTEX), 0, FVF_VOLVERTEX, POOL_MANAGED, &m_pVertexBuffer ) )
	{
		Fail();
		WriteToLog("GenerateCubeVertexBuffer: failed to create vertex buffer with result %s.\n", m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	VOID* pV;
	if ( !m_pVertexBuffer->Lock( 0, 24*sizeof(VOLVERTEX), (BYTE**)&pV, 0) )
	{
		Fail();
		WriteToLog( "GenerateCubeVertexBuffer: failed to lock buffer with result %s.\n", m_pD3D->HResultToString(GetLastError()) );
		RELEASE(m_pVertexBuffer);
		return false;
	}

	memcpy( pV, g_cubeV, 24*sizeof(VOLVERTEX));

	if ( !m_pVertexBuffer->Unlock() )
	{
		
		WriteToLog( "GenerateCubeVertexBuffer: failed to unlock buffer with result %s.\n", m_pD3D->HResultToString(GetLastError()) );
		RELEASE(m_pVertexBuffer);
		return false;
	}

	SetStreamSource(0, m_pVertexBuffer, sizeof(VOLVERTEX));
	SetVertexShader(FVF_VOLVERTEX);

	return true;
}
	
bool GammaCorrect::GenerateVolVertexBuffer()
{
	HRESULT hr;
	UINT offset = 0;
	DWORD texDepth = 16;
	UINT NumberOfSlices = 16+8+4+2+1;
	
	VOLVERTEX *g_V = new VOLVERTEX[4*NumberOfSlices];
	
	for (UINT index = texDepth; index >= 1; index /= 2)
	{
		DWORD NumPerRow = (DWORD)ce