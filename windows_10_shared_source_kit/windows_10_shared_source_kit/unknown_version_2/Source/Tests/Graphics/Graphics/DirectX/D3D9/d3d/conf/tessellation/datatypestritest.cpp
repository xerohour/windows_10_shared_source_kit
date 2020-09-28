#define D3D_OVERLOADS

#include "Tessellation.h"
#include "VertexStructDefs.h"
#include "cd3dx.h"		//for D3DXAssembleShader9

D3DDECLTYPE DataTypesTri[] = {
	//D3DDECLTYPE_FLOAT1,
    D3DDECLTYPE_FLOAT2,
    D3DDECLTYPE_FLOAT3,  
    D3DDECLTYPE_FLOAT4,  
    D3DDECLTYPE_D3DCOLOR,  
    D3DDECLTYPE_UBYTE4,  
    D3DDECLTYPE_SHORT2, 
    D3DDECLTYPE_SHORT4, 
};

TCHAR*  DataTypesTriStr[] = {
	//"FLOAT1",
    "FLOAT2",
    "FLOAT3",  
    "FLOAT4",  
    "D3DCOLOR",  
    "UBYTE4",  
	"SHORT2", 
    "SHORT4"
};

D3DDECLMETHOD MethodsTri[] = {
    D3DDECLMETHOD_DEFAULT,
    D3DDECLMETHOD_CROSSUV,    
};

TCHAR* MethodsTriStr[] = {
    "DEFAULT",
    "CROSSUV",  
};

CDataTypesTriTest::CDataTypesTriTest()
{
	m_szTestName = "Data Types Tri Test";
	m_szCommandKey = "DataTypesTri";

	m_pVB = NULL;
	m_pShader = NULL;
	m_pDecl = NULL;
}

UINT CDataTypesTriTest::TestInitialize(void)
{
	HRESULT hr;
	CAPS    caps;
	m_pSrcDevice->GetDeviceCaps(&caps);
  	m_nDataTypes = sizeof(DataTypesTri) / sizeof(D3DDECLTYPE);
	m_nMethods = sizeof(MethodsTri) / sizeof(D3DDECLMETHOD);
	m_nBasises = sizeof(Basises) / sizeof(D3DBASISTYPE);
	m_nDegrees = sizeof(Degrees) / sizeof(D3DDEGREETYPE);
	m_nTessLevels = 3;//sizeof(DataTypesTriTessLevels) / (3*sizeof(float));
	m_nPatchesPerRow = 0;
	m_nVertices = 0;
	m_uPatchWidth = 0;
	m_uPatchHeight = 0;
	m_uBufferWidth = 0;
	m_uBufferHeight = 0;
	m_StreamSize = 0;

	tcCount.View = m_nViewMatrices; 
	tcCount.TessLevels = m_nTessLevels;
	tcCount.Degree = m_nDegrees;
	tcCount.Basis = m_nBasises;
	tcCount.NumPatches = 6; 
	tcCount.DataType = m_nDataTypes;
	tcCount.Method = m_nMethods;

    DATATESTCASE tc;
	tc.nTest = 0;

	ClearRangeList();
	
	for( tc.DataType = 0; tc.DataType < tcCount.DataType; tc.DataType++ )
	{
		WORD countMethods = 0;
		switch ( DataTypesTri[tc.DataType] )
		{
			case D3DDECLTYPE_FLOAT3:  
			case D3DDECLTYPE_FLOAT4:
			case D3DDECLTYPE_D3DCOLOR:
			case D3DDECLTYPE_UBYTE4:
			case D3DDECLTYPE_SHORT4:
				countMethods = 2;
				break;
			default:
				countMethods = 1;
		}
		if ( DataTypesTri[tc.DataType] == D3DDECLTYPE_UBYTE4 && !(caps.DeclTypes & D3DDTCAPS_UBYTE4) )
			continue;

		for ( tc.Method     = 0; tc.Method     < countMethods;       tc.Method++     )
		for ( tc.TessLevels = 0; tc.TessLevels < tcCount.TessLevels; tc.TessLevels++ )
		for ( tc.NumPatches = 0; tc.NumPatches < tcCount.NumPatches; tc.NumPatches++ )
		for ( tc.Basis      = 0; tc.Basis      < tcCount.Basis;      tc.Basis++      )
		{
			//only bezier patches are used as tripatches
			if(Basises[tc.Basis] != D3DBASIS_BEZIER) 
				continue;

			for( tc.Degree = 0; tc.Degree < tcCount.Degree; tc.Degree++ )
			{
				//quadratic degree isn't supported for rtpatches
				if (Degrees[tc.Degree] == D3DDEGREE_QUADRATIC)
					continue;
				tc.View = 0;
				AddTestRange( (tc.nTest + 1), (tc.nTest + tcCount.View) );
			}
		}
	}

	return D3DTESTINIT_RUN;
}

bool CDataTypesTriTest::ExecuteTest(UINT iTest)
{
	DATATESTCASE tc;
	tc.nTest = iTest-1;
	DWORD NumPatches = 1 << tc.NumPatches;
	if(	tc.NumPatches >= tcCount.NumPatches ||
		tc.View       >= tcCount.View       ||
        tc.Degree     >= tcCount.Degree     ||
        tc.Basis      >= tcCount.Basis      ||
		tc.TessLevels >= tcCount.TessLevels ||
		tc.DataType   >= tcCount.DataType    )
    {
        WriteToLog( "No such test (%d)\n", iTest);
		return false;
    }

	//create and set verex shader
	if ( !(tc.View || tc.Degree || tc.Basis || tc.NumPatches || tc.TessLevels)  || m_pShader == NULL )
	{
		for( UINT i = 0; i < m_numPatches; i++ )
		{
			DeletePatch(SURFACEHANDLEBASE + i);
		}
		m_numPatches = 0;
		RELEASE(m_pShader);
		RELEASE(m_pDecl);
		if (!GenerateVertexShader(tc))
		{
			WriteToLog("Failed to create vertex shader.\n");
			DATATESTCASE tc1 = tc;
			tc1.DataType = tc.DataType+1;
			SkipTestRange(iTest, tc1.nTest);
			return false;
		}
		if ( !SetVertexDeclaration(m_pDecl) )
		{
			WriteToLog("Failed to set vertex declaration.\n");
			DATATESTCASE tc1 = tc;
			tc1.DataType = tc.DataType+1;
			SkipTestRange(iTest, tc1.nTest);
			return false;
		}
		if ( !SetVertexShader(m_pShader) )
		{
			WriteToLog("Failed to set vertex shader.\n");
			DATATESTCASE tc1 = tc;
			tc1.DataType = tc.DataType+1;
			SkipTestRange(iTest, tc1.nTest);
			return false;
		}
	}
	
	//create new patches
	if ( tc.View == 0 || iTest == m_pRangeList->uStart || (m_pConstraints != NULL && iTest == m_pConstraints->uStart))
	{
		for( UINT i = 0; i < m_numPatches; i++ )
		{
			DeletePatch(SURFACEHANDLEBASE + i);
		}
		m_numPatches = 0;
		RELEASE(m_pVB);
		if (!GenerateBuffers(tc))
		{
			WriteToLog("%d Failed to create the vertex buffer.\n", iTest);
			SkipTestRange(iTest, iTest+4);
			return false;
		}
		sprintf( msgString, TEXT("%s %s %d %s %s patches, Tess =(%.2f, %.2f, %.2f)"),
			MethodsTriStr[tc.Method], DataTypesTriStr[tc.DataType],
			NumPatches, BasisStr[tc.Basis], DegreeStr[tc.Degree], 
			m_TessLevels[tc.TessLevels][0], m_TessLevels[tc.TessLevels][1], 
			m_TessLevels[tc.TessLevels][2]
		);
	}

	BeginTestCase( msgString, iTest);

	float AdaptiveTess_x;
	float AdaptiveTess_y;
	float AdaptiveTess_z;
	float AdaptiveTess_w;
	switch (tc.View)
	{
		case 0:
			AdaptiveTess_x = 0.f;
			AdaptiveTess_y = 0.f;
			AdaptiveTess_z = 2.f;
			AdaptiveTess_w = 1.f;
			break;
		case 1:
			AdaptiveTess_x = -1.f;
			AdaptiveTess_y = 0.f;
			AdaptiveTess_z = 0.f;
			AdaptiveTess_w = 2.f;
			break;
		case 2:
			AdaptiveTess_x = 0.f;
			AdaptiveTess_y = -1.f;
			AdaptiveTess_z = 0.f;
			AdaptiveTess_w = 2.f;
			break;
		case 3:
			AdaptiveTess_x = 1.f;
			AdaptiveTess_y = 0.f;
			AdaptiveTess_z = 0.f;
			AdaptiveTess_w = 2.f;
			break;
		case 5:
			AdaptiveTess_x = 0.f;
			AdaptiveTess_y = 1.f;
			AdaptiveTess_z = 0.f;
			AdaptiveTess_w = 2.f;
			break;
	}
	SetRenderState(D3DRS_ADAPTIVETESS_X, *(DWORD*)&AdaptiveTess_x);
	SetRenderState(D3DRS_ADAPTIVETESS_Y, *(DWORD*)&AdaptiveTess_y);
	SetRenderState(D3DRS_ADAPTIVETESS_Z, *(DWORD*)&AdaptiveTess_z);
	SetRenderState(D3DRS_ADAPTIVETESS_W, *(DWORD*)&AdaptiveTess_w);

	D3DTRIPATCH_INFO TriInfo;
	float f[3];
	f[0]=m_TessLevels[tc.TessLevels][0]; 
	f[1]=m_TessLevels[tc.TessLevels][1]; 
	f[2]=m_TessLevels[tc.TessLevels][2];  

	//set stransformation matrices
	D3DXMATRIX mat;
	D3DXMatrixMultiply( &mat, &m_ViewMatrices[tc.View], &m_ProjMatrix); 
	D3DXMatrixTranspose( &mat, &mat );   
	SetVertexShaderConstant(0, &mat, 4); 
	if ( !SetStreamSource(0, m_pVB, m_StreamSize) )
		WriteToLog("%d Set stream source failed.\n", iTest);
	    
	BeginScene();

	if (tc.View == 0)
	{
		m_numPatches = NumPatches;
		for( int i = 0; i < m_numPatches; i++ ) 
		{
			SetNewMaterial(i);
			TriInfo.StartVertexOffset = i*m_nVertices;
			TriInfo.NumVertices = m_nVertices; 
			TriInfo.Basis = Basises[tc.Basis]; 
			TriInfo.Degree = Degrees[tc.Degree];
			DrawTriPatch(SURFACEHANDLEBASE+i, f, &TriInfo);
		}
	}
	else
	{
		for( int i = 0; i < m_numPatches; i++ ) 
		{
			SetNewMaterial(i);
			DrawTriPatch(SURFACEHANDLEBASE+i, f, NULL);
		}
	}

	EndScene();
	return true;
}

bool CDataTypesTriTest::GenerateBuffers(DATATESTCASE tc)
{
	VOID *pV = NULL;
	UINT  buffer_size = 0;
	float fX, fY, fZ, fNX, fNY ,fNZ, fU, fV;
	FLOAT_VERTEX *VBarrayFloat = NULL;
	DWORD_VERTEX *VBarrayDword = NULL;
	SHORT_VERTEX *VBarrayShort = NULL;
	
	m_nPatchesPerRow = ((UINT)ceil(sqrt((1<<tc.NumPatches)/2.f))) << 1;
	m_nVertices = ((Degrees[tc.Degree]+1)*(Degrees[tc.Degree]+2)) >> 1;
	m_uPatchHeight = m_uPatchWidth = Degrees[tc.Degree] + 1;
	m_uBufferWidth = m_uPatchWidth  + (m_uPatchWidth - 1)*((m_nPatchesPerRow>>1) - 1);
	m_uBufferHeight =m_uPatchHeight + (m_uPatchHeight -1)*((m_nPatchesPerRow>>1) - 1);

	buffer_size = m_nVertices*m_nPatchesPerRow*(m_nPatchesPerRow>>1);
	ASSERT(buffer_size > 0);
	
	//find out buffer's size
	switch ( DataTypesTri[tc.DataType] ) 
	{
		case D3DDECLTYPE_FLOAT1:
		case D3DDECLTYPE_FLOAT2:  
		case D3DDECLTYPE_FLOAT3:
		case D3DDECLTYPE_FLOAT4:
		{
			m_StreamSize = sizeof(FLOAT_VERTEX);
			VBarrayFloat = new FLOAT_VERTEX[buffer_size];
			break;
		}
		case D3DDECLTYPE_D3DCOLOR: 
		case D3DDECLTYPE_UBYTE4:
		{
			m_StreamSize = sizeof(DWORD_VERTEX);
			VBarrayDword = new DWORD_VERTEX[buffer_size];
			break;
		}
		case D3DDECLTYPE_SHORT2:
		case D3DDECLTYPE_SHORT4:
		{
			m_StreamSize = sizeof(SHORT_VERTEX);
			VBarrayShort = new SHORT_VERTEX[buffer_size];
			break;
		}
		default:
		{
			WriteToLog("GenerateBuffers - Invalid data type %d.\n", tc.DataType);
			return false;
		}
	}
	
	if( !CreateVertexBuffer( buffer_size*m_StreamSize, D3DUSAGE_RTPATCHES, 0, POOL_MANAGED, &m_pVB )) 
	{
		WriteToLog("%d: Can't create vertex buffer.\n", tc.nTest);
		return false;
	}
	if (!m_pVB->Lock( 0, buffer_size*m_StreamSize, (BYTE**)&pV, 0))
	{
		WriteToLog("%d: Failed to lock m_pVB.\n", tc.nTest);
		return false;
	}

	int tempWidth, X_Offset = 0, Y_Offset = 0, index = 0;
	for (UINT PatchInd = 0; PatchInd < ((m_nPatchesPerRow>>1)*(m_nPatchesPerRow>>1))<<1; PatchInd += 2)
	{
		tempWidth = 1;
		for (int j = Y_Offset; j < Y_Offset + (int)m_uPatchHeight; j++)
		{
			fY = 1.f-2.f*((float)j/(m_uBufferHeight-1.f));
			for (int i = X_Offset; i < X_Offset + tempWidth; i++)
			{
				if (KeySet(_T("straight")))
				{
					fX = -1.f + 2.f*((float)i/(m_uBufferWidth-1.f));
					fZ = 0.f;
					fNX = fNY = 0.0f;
					fNZ = -1.0f;
				}
				else
				{
					fX = cos( (m_uBufferWidth-1.f-i) * (D3DX_PI/(m_uBufferWidth-1.f)) );
					fZ = sqrt( 1.f - fX*fX );
					fNX = -fX;
					fNY = 0.0f;
					fNZ = -fZ;
				}
				
				fU = (1.f+fX)/2.f;
				fV = (1.f-fY)/2.f;
				
				SHORT  sX = (SHORT)( fX*0x7fff );
				SHORT  sY = (SHORT)( fY*0x7fff );
				SHORT  sZ = (SHORT)( fZ*0x7fff );
				SHORT sNX = (SHORT)( fNX*0x7fff );
				SHORT sNY = (SHORT)( fNY*0x7fff );
				SHORT sNZ = (SHORT)( fNZ*0x7fff );
				SHORT  sU = (SHORT)( fU*0x7fff );
				SHORT  sV = (SHORT)( fV*0x7fff );
				BYTE   bX = (BYTE)( (fX+1.f)*(0xff>>1) );
				BYTE   bY = (BYTE)( (fY+1.f)*(0xff>>1) );
				BYTE   bZ = (BYTE)( (fZ+1.f)*(0xff>>1) );
				BYTE  bNX = (BYTE)( (fNX+1.f)*(0xff>>1) );
				BYTE  bNY = (BYTE)( (fNY+1.f)*(0xff>>1) );
				BYTE  bNZ = (BYTE)( (fNZ+1.f)*(0xff>>1) );
				BYTE   bU = (BYTE)( fU*0xff );
				BYTE   bV = (BYTE)( fV*0xff );
						
				switch ( DataTypesTri[tc.DataType] ) 
				{
					case D3DDECLTYPE_FLOAT1: 
						VBarrayFloat[index++] = FLOAT_VERTEX( fX, fY, fZ, 0.f, fNX, fNY, fNZ, 0.f, fU, 0.f, fV, 0.f);
						break;
					case D3DDECLTYPE_FLOAT2:  
						VBarrayFloat[index++] = FLOAT_VERTEX( fX, fY, fZ, 0.f, fNX, fNY, fNZ, 0.f, fU, fV, 0.f, 0.f);
						break;
					case D3DDECLTYPE_FLOAT3:  
						VBarrayFloat[index++] = FLOAT_VERTEX( fX, fY, fZ, 0.f, fNX, fNY, fNZ, 0.f, 0.f, fU, fV, 0.f);
						break;
					case D3DDECLTYPE_FLOAT4:  
						VBarrayFloat[index++] = FLOAT_VERTEX( fX, fY, fZ, 1.f, fNX, fNY, fNZ, 0.f, 0.f, 0.f, fU, fV);
						break;
					case D3DDECLTYPE_D3DCOLOR:  
						VBarrayDword[index++] = DWORD_VERTEX( RGBA_MAKE(bX, bY, bZ, 0xff), RGBA_MAKE(bNX, bNY, bNZ, 0), RGBA_MAKE(bU, bV, 0, 0));
						break;
					case D3DDECLTYPE_UBYTE4: 
						VBarrayDword[index++] = DWORD_VERTEX( (bX | (bY<<8) | (bZ<<16) | (0xff<<24)), (bNX | (bNY<<8) | (bNZ<<16)), (bU | (bV<<8)) );
						break;
					case D3DDECLTYPE_SHORT2: 
						VBarrayShort[index++] = SHORT_VERTEX( sX, sY, sZ, sNX, sNY, sNZ, sU, sV, 0, 0);
						break;
					case D3DDECLTYPE_SHORT4: 
						VBarrayShort[index++] = SHORT_VERTEX( sX, sY, sZ, sNX, sNY, sNZ, 0, 0, sU, sV);
						break;
				}
			}
			tempWidth++;
		}
		tempWidth = 1;
		for (int j = Y_Offset + (int)m_uPatchHeight - 1; j >= Y_Offset; j--)
		{
			fY = 1.f-2.f*((float)j/(m_uBufferHeight-1.f));
			for (int i = X_Offset + (int)m_uPatchWidth - 1; i >= X_Offset + (int)m_uPatchWidth - tempWidth; i--)
			{
				if (KeySet(_T("straight")))
				{
					fX = -1.f + 2.f*((float)i/(m_uBufferWidth-1.f));
					fZ = 0.f;
				}
				else
				{
					fX = cos( (m_uBufferWidth-1.f-i) * (D3DX_PI/(m_uBufferWidth-1.f)) );
					fZ = sqrt( 1.f - fX*fX );
				}
					
				fNX = -fX;
				fNY = 0.0f;
				fNZ = -fZ;

				fU = (1.f+fX)/2.f;
				fV = (1.f-fY)/2.f;
			
				SHORT  sX = (SHORT)( fX*0x7fff );
				SHORT  sY = (SHORT)( fY*0x7fff );
				SHORT  sZ = (SHORT)( fZ*0x7fff );
				SHORT sNX = (SHORT)( fNX*0x7fff );
				SHORT sNY = (SHORT)( fNY*0x7fff );
				SHORT sNZ = (SHORT)( fNZ*0x7fff );
				SHORT  sU = (SHORT)( fU*0x7fff );
				SHORT  sV = (SHORT)( fV*0x7fff );
				BYTE   bX = (BYTE)( (fX+1.f)*(0xff>>1) );
				BYTE   bY = (BYTE)( (fY+1.f)*(0xff>>1) );
				BYTE   bZ = (BYTE)( (fZ+1.f)*(0xff>>1) );
				BYTE  bNX = (BYTE)( (fNX+1.f)*(0xff>>1) );
				BYTE  bNY = (BYTE)( (fNY+1.f)*(0xff>>1) );
				BYTE  bNZ = (BYTE)( (fNZ+1.f)*(0xff>>1) );
				BYTE   bU = (BYTE)( fU*0xff );
				BYTE   bV = (BYTE)( fV*0xff );
						
				switch ( DataTypesTri[tc.DataType] ) 
				{
					case D3DDECLTYPE_FLOAT1: 
						VBarrayFloat[index++] = FLOAT_VERTEX( fX, fY, fZ, 0.f, fNX, fNY, fNZ, 0.f, fU, 0.f, fV, 0.f);
						break;
					case D3DDECLTYPE_FLOAT2:  
						VBarrayFloat[index++] = FLOAT_VERTEX( fX, fY, fZ, 0.f, fNX, fNY, fNZ, 0.f, fU, fV, 0.f, 0.f);
						break;
					case D3DDECLTYPE_FLOAT3:  
						VBarrayFloat[index++] = FLOAT_VERTEX( fX, fY, fZ, 0.f, fNX, fNY, fNZ, 0.f, 0.f, fU, fV, 0.f);
						break;
					case D3DDECLTYPE_FLOAT4:  
						VBarrayFloat[index++] = FLOAT_VERTEX( fX, fY, fZ, 1.f, fNX, fNY, fNZ, 0.f, 0.f, 0.f, fU, fV);
						break;
					case D3DDECLTYPE_D3DCOLOR:  
						VBarrayDword[index++] = DWORD_VERTEX( RGBA_MAKE(bX, bY, bZ, 0xff), RGBA_MAKE(bNX, bNY, bNZ, 0), RGBA_MAKE(bU, bV, 0, 0));
						break;
					case D3DDECLTYPE_UBYTE4: 
						VBarrayDword[index++] = DWORD_VERTEX( (bX | (bY<<8) | (bZ<<16) | (0xff<<24)), (bNX | (bNY<<8) | (bNZ<<16)), (bU | (bV<<8)) );
						break;
					case D3DDECLTYPE_SHORT2: 
						VBarrayShort[index++] = SHORT_VERTEX( sX, sY, sZ, sNX, sNY, sNZ, sU, sV, 0, 0);
						break;
					case D3DDECLTYPE_SHORT4: 
						VBarrayShort[index++] = SHORT_VERTEX( sX, sY, sZ, sNX, sNY, sNZ, 0, 0, sU, sV);
						break;
				}		
			}
			tempWidth++;
		}
		X_Offset += m_uPatchWidth-1;
		if (X_Offset >= m_uBufferWidth-1)
		{
			X_Offset = 0;
			Y_Offset += m_uPatchHeight-1;
		}
	}
	m_uBufferWidth  = m_nVertices*m_nPatchesPerRow;
	m_uBufferHeight = m_nPatchesPerRow >> 1;


	switch ( DataTypesTri[tc.DataType] ) 
	{
		case D3DDECLTYPE_FLOAT1:
		case D3DDECLTYPE_FLOAT2:  
		case D3DDECLTYPE_FLOAT3:
		case D3DDECLTYPE_FLOAT4:
		{
			memcpy( pV, VBarrayFloat, buffer_size*sizeof(FLOAT_VERTEX));
			break;
		}
		case D3DDECLTYPE_D3DCOLOR:
		case D3DDECLTYPE_UBYTE4:
		{
			memcpy( pV, VBarrayDword, buffer_size*sizeof(DWORD_VERTEX));
			break;
		}
		case D3DDECLTYPE_SHORT2:
		case D3DDECLTYPE_SHORT4:
		{
			memcpy( pV, VBarrayShort, buffer_size*sizeof(SHORT_VERTEX));
			break;
		}
	}
	if (!m_pVB->Unlock())
	{
		WriteToLog("%d: Failed to unlock m_pVB.\n", tc.nTest);
		return false;
	}
	delete [] VBarrayFloat;
	delete [] VBarrayDword;
	delete [] VBarrayShort;
	
	return true;
}

bool CDataTypesTriTest::GenerateVertexShader(DATATESTCASE tc)
{
	HRESULT hr;
	LPD3DXBUFFER pShader = NULL;
	
	switch ( DataTypesTri[tc.DataType] )
	{
		case D3DDECLTYPE_FLOAT1:
		{
			VERTEXELEMENT decl[] = 
			{
				{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT1,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
				{0,  4, (DECLTYPE)D3DDECLTYPE_FLOAT1,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 1},
				{0,  8, (DECLTYPE)D3DDECLTYPE_FLOAT1,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 2},
				{0, 16, (DECLTYPE)D3DDECLTYPE_FLOAT1,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
				{0, 20, (DECLTYPE)D3DDECLTYPE_FLOAT1,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   1},
				{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT1,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   2},
				{0, 32, (DECLTYPE)D3DDECLTYPE_FLOAT1,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
				{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT1,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
				{0, 40, (DECLTYPE)D3DDECLTYPE_FLOAT1,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 2},
				//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
				DECL_END()
			};
			if(!CreateVertexDeclaration( decl, &(m_pDecl)))
			{
				WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
				return false;
			}

			TCHAR *szShader = _T(
				"vs.1.1            \n"
				"dcl_position  v0   \n"
				"dcl_position1 v1   \n"
				"dcl_position2 v2   \n"
				"dcl_normal    v3   \n"
				"dcl_normal1   v4   \n"
				"dcl_normal2   v5   \n"
				"dcl_texcoord  v6   \n"
				"dcl_texcoord1 v7   \n"
				"dcl_texcoord2 v8   \n"
				//"dcl_diffuse  v3   \n"
				
				"mov  r0.xw, v0.xw     \n"
				"mov  r0.y, v1.x       \n"
				"mov  r0.z, v2.x       \n"
				"m4x4 oPos,   r0,  c0  \n"
				"mov  r1.xw, v3.xw     \n"
				"mov  r1.y, v4.x       \n"
				"mov  r1.z, v5.x       \n"
				"dp3  r2.x,   r1, c7  \n"    // perform lighting N dot L calculation in world space
				                              // calculate pixel color from light intensity and
				"mul    r3, r2.x,  c4  \n"	  // interpolated diffuse vertex color 
				"add   oD0,   r3,  c6  \n"    //add ambient color -> final pixel color
				"mov  r4.xzw, v6.xzw   \n"
				"mov  r4.y, v8.x       \n"
				"mov   oT0, r4.xy      \n"
			);
			hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
			if (FAILED(hr))
			{
				WriteToLog(_T("GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
					m_pD3D->HResultToString(hr));
					RELEASE(m_pDecl);
				return false;
			}
			break;
		}
		case D3DDECLTYPE_FLOAT2:
		{
			/*VERTEXELEMENT decl[] = 
			{
				{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
				{0,  8, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 1},
				{0, 16, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
				{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   1},
				{0, 32, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
				//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
				DECL_END()
			};*/
			VERTEXELEMENT decl[] = 
			{
				{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
				{0, 16, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
				{0, 32, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
				//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
				DECL_END()
			};
			if(!CreateVertexDeclaration( decl, &(m_pDecl)))
			{
				WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
				return false;
			}

			TCHAR *szShader = _T(
				"vs.1.1             \n"
				"dcl_position  v0   \n"
				//"dcl_position1 v1   \n"
				"dcl_normal    v2   \n"
				//"dcl_normal1   v3   \n"
				"dcl_texcoord  v4   \n"
				//"dcl_diffuse  v3   \n"
				
				"mov   r0,    v0       \n"
				//"mov r0.z,  v1.x       \n"
				"m4x4 oPos,   r0,  c0  \n"
				"mov   r1,    v2       \n"
				//"mov r1.z,  v3.x       \n"
				"dp3  r2.x,   r1, c7  \n"    // perform lighting N dot L calculation in world space
				                              // calculate pixel color from light intensity and
				"mul    r3, r2.x,  c4  \n"	  // interpolated diffuse vertex color 
				"add   oD0,   r3,  c6  \n"    //add ambient color -> final pixel color
				"mov   oT0,   v4       \n"
			);
			hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
			if (FAILED(hr))
			{
				WriteToLog(_T("GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
					m_pD3D->HResultToString(hr));
					RELEASE(m_pDecl);
				return false;
			}
			break;
		}
		case D3DDECLTYPE_FLOAT3:
		{
			switch ( MethodsTri[tc.Method] )
			{
				case D3DDECLMETHOD_DEFAULT:
				{
					VERTEXELEMENT decl[] = 
					{
						{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
						{0, 16, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
						{0, 32, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
						//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
						DECL_END()
					};
					if(!CreateVertexDeclaration( decl, &(m_pDecl)))
					{
						WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
						return false;
					}
					break;
				}
				case D3DDECLMETHOD_CROSSUV:
				{
					VERTEXELEMENT decl[] = 
					{
						{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
						{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL,   0},
						{0, 16, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   1},
						{0, 32, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
						//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
						DECL_END()
					};
					if(!CreateVertexDeclaration( decl, &(m_pDecl)))
					{
						WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
						return false;
					}
					break;
				}
				default:
				{
					WriteToLog("Unknown D3DECLMETHOD %d\n", tc.Method);
					return false;
				}
			}

			TCHAR *szShader = _T(
				"vs.1.1            \n"
				"dcl_position v0   \n"
				"dcl_normal   v1   \n"
				"dcl_texcoord v2   \n"
				//"dcl_diffuse  v3   \n"

				"m4x4 oPos,   v0,  c0  \n"
				"dp3  r0.x,   v1, c7  \n"    // perform lighting N dot L calculation in world space
	 										// calculate pixel color from light intensity and
				"mul    r1, r0.x,  c4  \n"	  // interpolated diffuse vertex color 
				"add   oD0,   r1,  c6  \n"    //add ambient color -> final pixel color
				"mov   oT0,   v2.yz    \n"
			);
			hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
			if (FAILED(hr))
			{
				WriteToLog(_T("GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
					m_pD3D->HResultToString(hr));
					RELEASE(m_pDecl);
				return false;
			}
			break;
		}
		case D3DDECLTYPE_FLOAT4:
		{
			switch ( MethodsTri[tc.Method] )
			{
				case D3DDECLMETHOD_DEFAULT:
				{
					VERTEXELEMENT decl[] = 
					{
						{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
						{0, 16, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
						{0, 32, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
						//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
						DECL_END()
					};
					if(!CreateVertexDeclaration( decl, &(m_pDecl)))
					{
						WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
						return false;
					}
					break;
				}
				case D3DDECLMETHOD_CROSSUV:
				{
					VERTEXELEMENT decl[] = 
					{
						{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
						{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL,   0},
						{0, 16, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   1},
						{0, 32, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
						//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
						DECL_END()
					};
					if(!CreateVertexDeclaration( decl, &(m_pDecl)))
					{
						WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
						return false;
					}
					break;
				}
				default:
				{
					WriteToLog("Unknown D3DECLMETHOD %d\n", tc.Method);
					return false;
				}
			}

			TCHAR *szShader = _T(
				"vs.1.1            \n"
				"dcl_position v0   \n"
				"dcl_normal   v1   \n"
				"dcl_texcoord v2   \n"
				//"dcl_diffuse  v3   \n"

				"m4x4 oPos,   v0,  c0  \n"
				"dp3  r0.x,   v1, c7  \n"    // perform lighting N dot L calculation in world space
											// calculate pixel color from light intensity and
				"mul    r1, r0.x,  c4  \n"	  // interpolated diffuse vertex color 
				"add   oD0,   r1,  c6  \n"    //add ambient color -> final pixel color
				"mov   oT0,   v2.zw    \n"
			);
			hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
			if (FAILED(hr))
			{
				WriteToLog(_T("GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
					m_pD3D->HResultToString(hr));
					RELEASE(m_pDecl);
				return false;
			}
			break;
		}
		case D3DDECLTYPE_D3DCOLOR:
		{
			float value[4] = { 2.f, -1.f, 0.f, 0.f };
			//float value[4] = { 2.f/0xff, -1.f, 1.f/0xff, 0.f };
			if ( !SetVertexShaderConstantF( 8, value, 1 ))
				WriteToLog("GenerateBuffers - SetVertexShaderConstantF: Failed to set new value for c8.\n");

			switch ( MethodsTri[tc.Method] )
			{
				case D3DDECLMETHOD_DEFAULT:
				{
					VERTEXELEMENT decl[] = 
					{
						{0, 0, (DECLTYPE)D3DDECLTYPE_D3DCOLOR,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
						{0, 4, (DECLTYPE)D3DDECLTYPE_D3DCOLOR,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
						{0, 8, (DECLTYPE)D3DDECLTYPE_D3DCOLOR,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
						//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
						DECL_END()
					};
					if(!CreateVertexDeclaration( decl, &(m_pDecl)))
					{
						WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
						return false;
					}
					TCHAR *szShader = _T(
						"vs.1.1            \n"
						"dcl_position v0   \n"
						"dcl_normal   v1   \n"
						"dcl_texcoord v2   \n"
						//"dcl_diffuse  v3   \n"

						"mul    r0,  c8.x, v0 \n"
						"add    r1,  c8.y, r0 \n"
						"m4x4 oPos,  r1,  c0   \n"
						"mul    r2,  v1, c8.x \n"
						"add    r3,  r2, c8.y \n"
						"dp3  r4.x,  r3, c7   \n"    // perform lighting N dot L calculation in world space
													// calculate pixel color from light intensity and
						"mul    r5, r4.x,  c4  \n"	  // interpolated diffuse vertex color 
						"add   oD0,   r5,  c6  \n"    //add ambient color -> final pixel color
						"mov   oT0,   v2.xy    \n"
					);
					hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
					if (FAILED(hr))
					{
						WriteToLog(_T("GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
							m_pD3D->HResultToString(hr));
							RELEASE(m_pDecl);
						return false;
					}
					break;
				}
				case D3DDECLMETHOD_CROSSUV:
				{
					VERTEXELEMENT decl[] = 
					{
						{0, 0, (DECLTYPE)D3DDECLTYPE_D3DCOLOR,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
						{0, 0, (DECLTYPE)D3DDECLTYPE_D3DCOLOR,   DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL,   0},
						{0, 4, (DECLTYPE)D3DDECLTYPE_D3DCOLOR,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   1},
						{0, 8, (DECLTYPE)D3DDECLTYPE_D3DCOLOR,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
						//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
						DECL_END()
					};
					if(!CreateVertexDeclaration( decl, &(m_pDecl)))
					{
						WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
						return false;
					}
					TCHAR *szShader = _T(
						"vs.1.1            \n"
						"dcl_position v0   \n"
						"dcl_normal   v1   \n"
						"dcl_texcoord v2   \n"
						//"dcl_diffuse  v3   \n"

						"mul    r0,  c8.x, v0 \n"
						"add    r1,  c8.y, r0 \n"
						"m4x4 oPos,   r1,  c0  \n"
						"dp3  r2.x,   v1, c7  \n"    // perform lighting N dot L calculation in world space
													// calculate pixel color from light intensity and
						"mul    r3, r2.x,  c4  \n"	  // interpolated diffuse vertex color 
						"add   oD0,   r3,  c6  \n"    //add ambient color -> final pixel color
						"mov   oT0,   v2       \n"
					);
					hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
					if (FAILED(hr))
					{
						WriteToLog(_T("GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
							m_pD3D->HResultToString(hr));
							RELEASE(m_pDecl);
						return false;
					}
					break;
				}
				default:
				{
					WriteToLog("Unknown D3DECLMETHOD %d\n", tc.Method);
					return false;
				}
			}
			break;
		}
		case D3DDECLTYPE_UBYTE4:
		{
			float value[4] = { 2.f/0xff, -1.f, 1.f/0xff, 0.f };
			if ( !SetVertexShaderConstantF( 8, value, 1 ))
				WriteToLog("GenerateBuffers - SetVertexShaderConstantF: Failed to set new value for c8.\n");

			switch ( MethodsTri[tc.Method] )
			{
				case D3DDECLMETHOD_DEFAULT:
				{
					VERTEXELEMENT decl[] = 
					{
						{0, 0, (DECLTYPE)D3DDECLTYPE_UBYTE4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
						{0, 4, (DECLTYPE)D3DDECLTYPE_UBYTE4,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
						{0, 8, (DECLTYPE)D3DDECLTYPE_UBYTE4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
						//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
						DECL_END()
					};
					if(!CreateVertexDeclaration( decl, &(m_pDecl)))
					{
						WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
						return false;
					}
					TCHAR *szShader = _T(
						"vs.1.1            \n"
						"dcl_position v0   \n"
						"dcl_normal   v1   \n"
						"dcl_texcoord v2   \n"
						//"dcl_diffuse  v3   \n"

						"mul    r0,  c8.x, v0 \n"
						"add    r1,  c8.y, r0 \n"
						"m4x4 oPos,  r1,  c0   \n"
						"mul    r2,  v1, c8.x \n"
						"add    r3,  r2, c8.y \n"
						"dp3  r4.x,  r3, c7   \n"    // perform lighting N dot L calculation in world space
													// calculate pixel color from light intensity and
						"mul    r5, r4.x,  c4  \n"	  // interpolated diffuse vertex color 
						"add   oD0,   r5,  c6  \n"    //add ambient color -> final pixel color
						"mul    r6,  v2, c8.z \n"
						"mov   oT0,   r6.xy    \n"
					);
					hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
					if (FAILED(hr))
					{
						WriteToLog(_T("GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
							m_pD3D->HResultToString(hr));
							RELEASE(m_pDecl);
						return false;
					}
					break;
				}
				case D3DDECLMETHOD_CROSSUV:
				{
					VERTEXELEMENT decl[] = 
					{
						{0, 0, (DECLTYPE)D3DDECLTYPE_UBYTE4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
						{0, 0, (DECLTYPE)D3DDECLTYPE_UBYTE4,   DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL,   0},
						{0, 4, (DECLTYPE)D3DDECLTYPE_UBYTE4,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   1},
						{0, 8, (DECLTYPE)D3DDECLTYPE_UBYTE4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
						//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
						DECL_END()
					};
					if(!CreateVertexDeclaration( decl, &(m_pDecl)))
					{
						WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
						return false;
					}
					TCHAR *szShader = _T(
						"vs.1.1            \n"
						"dcl_position v0   \n"
						"dcl_normal   v1   \n"
						"dcl_texcoord v2   \n"
						//"dcl_diffuse  v3   \n"

						"mul    r0,  c8.x, v0 \n"
						"add    r1,  c8.y, r0 \n"
						"m4x4 oPos,   r1,  c0  \n"
						"dp3  r2.x,   v1, c7  \n"    // perform lighting N dot L calculation in world space
													// calculate pixel color from light intensity and
						"mul    r3, r2.x,  c4  \n"	  // interpolated diffuse vertex color 
						"add   oD0,   r3,  c6  \n"    //add ambient color -> final pixel color
						"mul    r4,  v2, c8.z \n"
						"mov   oT0,   r4.xy    \n"
					);
					hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
					if (FAILED(hr))
					{
						WriteToLog(_T("GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
							m_pD3D->HResultToString(hr));
							RELEASE(m_pDecl);
						return false;
					}
					break;
				}
				default:
				{
					WriteToLog("Unknown D3DECLMETHOD %d\n", tc.Method);
					return false;
				}
			}
			break;
		}
		case D3DDECLTYPE_SHORT2:
		{
			float value[4] = { 1.f/0x7fff, 0.f, 0.f, 0.f };
			if ( !SetVertexShaderConstantF( 8, value, 1 ))
				WriteToLog("GenerateBuffers - SetVertexShaderConstantF: Failed to set new value for c8.\n");
			
			/*VERTEXELEMENT decl[] = 
			{
				{0,  0, (DECLTYPE)D3DDECLTYPE_SHORT2,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
				{0,  4, (DECLTYPE)D3DDECLTYPE_SHORT2,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 1},
				{0,  8, (DECLTYPE)D3DDECLTYPE_SHORT2,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
				{0, 12, (DECLTYPE)D3DDECLTYPE_SHORT2,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   1},
				{0, 16, (DECLTYPE)D3DDECLTYPE_SHORT2,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
				//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
				DECL_END()
			};*/
			VERTEXELEMENT decl[] = 
			{
				{0,  0, (DECLTYPE)D3DDECLTYPE_SHORT4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
				{0,  8, (DECLTYPE)D3DDECLTYPE_SHORT4,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
				{0, 16, (DECLTYPE)D3DDECLTYPE_SHORT2,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
				//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
				DECL_END()
			};
			if(!CreateVertexDeclaration( decl, &(m_pDecl)))
			{
				WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
				return false;
			}

			TCHAR *szShader = _T(
				"vs.1.1             \n"
				"dcl_position  v0   \n"
				//"dcl_position1 v1   \n"
				"dcl_normal    v2   \n"
				//"dcl_normal1   v3   \n"
				"dcl_texcoord  v4   \n"
				//"dcl_diffuse  v3   \n"

				"mov r0, v0 \n"
				//"mov r0.zw, v1.xy \n"
				"mul    r1, c8.x,  r0 \n"
				"m4x4 oPos,    r1,  c0 \n"
				"mov r2, v2 \n"
				//"mov r2.zw, v3.xy \n"
				"mul    r3, c8.x,  r2 \n"
				"dp3  r4.x,    r3, c7 \n"    // perform lighting N dot L calculation in world space
				                              // calculate pixel color from light intensity and
				"mul    r5,  r4.x,  c4 \n"	  // interpolated diffuse vertex color 
				"add   oD0,    r5,  c6 \n"    //add ambient color -> final pixel color
				"mul    r6, c8.x,  v4 \n"
				"mov   oT0,    r6      \n"
			);
			hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
			if (FAILED(hr))
			{
				WriteToLog(_T("GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
					m_pD3D->HResultToString(hr));
					RELEASE(m_pDecl);
				return false;
			}
			break;
		}
		case D3DDECLTYPE_SHORT4:
		{
			float value[4] = { 1.f/0x7fff, 0.f, 0.f, 0.f };
			if ( !SetVertexShaderConstantF( 8, value, 1 ))
				WriteToLog("GenerateBuffers - SetVertexShaderConstantF: Failed to set new value for c8.\n");

			switch ( MethodsTri[tc.Method] )
			{
				case D3DDECLMETHOD_DEFAULT:
				{
					VERTEXELEMENT decl[] = 
					{
						{0, 0, (DECLTYPE)D3DDECLTYPE_SHORT4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
						{0, 8, (DECLTYPE)D3DDECLTYPE_SHORT4,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
						{0, 16, (DECLTYPE)D3DDECLTYPE_SHORT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
						//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
						DECL_END()
					};
					if(!CreateVertexDeclaration( decl, &(m_pDecl)))
					{
						WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
						return false;
					}
					TCHAR *szShader = _T(
						"vs.1.1            \n"
						"dcl_position v0   \n"
						"dcl_normal   v1   \n"
						"dcl_texcoord v2   \n"
						//"dcl_diffuse  v3   \n"

						"mul    r0, c8.x,  v0 \n"
						"m4x4 oPos,    r0,  c0 \n"
						"mul    r1, c8.x,  v1 \n"
						"dp3  r2.x,    r1, c7 \n"    // perform lighting N dot L calculation in world space
													// calculate pixel color from light intensity and
						"mul    r3,  r2.x,  c4 \n"	  // interpolated diffuse vertex color 
						"add   oD0,    r3,  c6 \n"    //add ambient color -> final pixel color
						"mul    r4, c8.x,  v2 \n"
						"mov   oT0,  r4.zw     \n"
					);
					hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
					if (FAILED(hr))
					{
						WriteToLog(_T("GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
							m_pD3D->HResultToString(hr));
							RELEASE(m_pDecl);
						return false;
					}
					break;
				}
				case D3DDECLMETHOD_CROSSUV:
				{
					VERTEXELEMENT decl[] = 
					{
						{0, 0, (DECLTYPE)D3DDECLTYPE_SHORT4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
						{0, 0, (DECLTYPE)D3DDECLTYPE_SHORT4,   DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL,   0},
						{0, 8, (DECLTYPE)D3DDECLTYPE_SHORT4,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   1},
						{0, 16, (DECLTYPE)D3DDECLTYPE_SHORT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
						//{0, 32, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_DIFFUSE,  0},
						DECL_END()
					};
					if(!CreateVertexDeclaration( decl, &(m_pDecl)))
					{
						WriteToLog("GenerateVertexShader - CreateVertexDeclaration() failed.\n");
						return false;
					}
					TCHAR *szShader = _T(
						"vs.1.1            \n"
						"dcl_position v0   \n"
						"dcl_normal   v1   \n"
						"dcl_texcoord v2   \n"
						//"dcl_diffuse  v3   \n"

						"mul    r0,  c8.x, v0 \n"
						"m4x4 oPos,   r0,  c0  \n"
						"dp3  r2.x,   v1, c7  \n"    // perform lighting N dot L calculation in world space
													// calculate pixel color from light intensity and
						"mul    r3, r2.x,  c4  \n"	  // interpolated diffuse vertex color 
						"add   oD0,   r3,  c6  \n"    //add ambient color -> final pixel color
						"mul    r4,  v2, c8.x \n"
						"mov   oT0,   r4.zw    \n"
					);
					hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
					if (FAILED(hr))
					{
						WriteToLog(_T("GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
							m_pD3D->HResultToString(hr));
							RELEASE(m_pDecl);
						return false;
					}
					break;
				}
				default:
				{
					WriteToLog("Unknown D3DECLMETHOD %d\n", tc.Method);
					return false;
				}
			}
			break;
		}
		default:
		{
			WriteToLog("Unknown D3DDECLTYPE %d\n", tc.DataType);
			return false;
		}
	}
	if (!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShader)))
	{
		WriteToLog("GenerateVertexShader -- CreateVertexShader() failed.\n");
		pShader->Release();
		RELEASE(m_pDecl);
		return false;
	}
	pShader->Release();
	
	return true;
}

bool CDataTypesTriTest::CapsCheck()
{
	Tessellation::CapsCheck();
	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps);
	//if tri patches aren't supported -> skip
	if(!(caps.DevCaps & D3DDEVCAPS_RTPATCHES))
	{
        WriteToLog("CapsCheck: tri patches are not supported.\n");
        return false;
    } 
	//if use adaptive tesselation but it's not supported for this kind of patches -> skip
	if (m_AdapTess && !(caps.DevCaps2 & D3DDEVCAPS2_ADAPTIVETESSRTPATCH))
	{
        WriteToLog("CapsCheck: adaptive tesselation is not supported for tri patches.\n");
        return false;
    }
	if( caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
	{
		WriteToLog("CDataTypesTriTest: vertex shaders version 1.1 or higher are not supported\n");
		return false;
	}
	
	m_TessLevels[0][0] = 1.f;
	m_TessLevels[0][1] = 1.f;
	m_TessLevels[0][2] = 1.f;
	m_TessLevels[1][0] = m_MaxTess-1.f;
	m_TessLevels[1][1] = m_MaxTess/4.f+1.f;
	m_TessLevels[1][2] = m_MaxTess/2.f;
	m_TessLevels[2][0] = m_MaxTess/2.f-1;
	m_TessLevels[2][1] = m_MaxTess-2.f;
	m_TessLevels[2][2] = m_MaxTess/4.f;

	return true;
}

bool CDataTypesTriTest::Setup()
{
	Tessellation::Setup();
	
	return true;
}

void CDataTypesTriTest::Cleanup()
{
	Tessellation::Cleanup();
	
	RELEASE(m_pVB);
	RELEASE(m_pShader);
	RELEASE(m_pDecl);

	for( UINT i = 0; i < m_numPatches; i++ ) 
        DeletePatch(SURFACEHANDLEBASE + i);
	m_numPatches = 0;
}

