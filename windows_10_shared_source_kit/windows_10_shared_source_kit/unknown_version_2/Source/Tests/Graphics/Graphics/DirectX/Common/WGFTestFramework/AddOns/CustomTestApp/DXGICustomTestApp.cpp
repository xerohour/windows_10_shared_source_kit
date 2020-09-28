m_pDecls[3]);
		return false;
	}
	if (!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShaders[3])))
	{
		WriteToLog("DisplacementMapping::GenerateVertexShader -- CreateVertexShader3() failed.\n");
		pShader->Release();
		RELEASE(m_pDecls[3]);
		return false;
	}
	pShader->Release();

	VERTEXELEMENT decl6[] = 
		{
			{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
			//{0, 24, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,    0},
			{0, 28, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
			DECL_END()
		};
	if(!CreateVertexDeclaration(decl6, &(m_pDecls[6])))
	{
		WriteToLog("DisplacementMapping::GenerateVertexShader - CreateVertexDeclaration6() failed.\n");
		return false;
	}
	szShader = _T(
		"vs.1.1           \n"
		"dcl_po