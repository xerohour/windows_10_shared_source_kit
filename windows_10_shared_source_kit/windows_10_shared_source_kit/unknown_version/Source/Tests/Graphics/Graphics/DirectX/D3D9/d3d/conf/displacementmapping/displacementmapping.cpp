#define D3D_OVERLOADS
//#include "3dmath.h"
#include "DisplacementMapping.h"
#include "CImage.h"
#include "cd3dx.h"		//for D3DXAssembleShader9
//#include "d3d9.h"       //for D3DSHADER_VERSION_MAJOR and D3DSHADER_VERSION_MINOR

// Global Test and App instance
CD3DWindowFramework App;
//CBasicTest          BasicTest;
CMipMapTest         MipMapTest;
CMaxMipLevelTest    MaxMipLevelTest;
CBiasTest           BiasTest;
CAddressModesTest   AddressModesTest;

DisplacementMapping::DisplacementMapping()
{
	m_szTestName = "DisplacementMapping";
	m_szCommandKey = "DisMap";
	m_szLogName = "DisplacementMapping";
	SetTestType(TESTTYPE_CONF);
	m_Options.D3DOptions.fMinDXVersion = 9.f;
	m_Options.D3DOptions.dwBehaviors = DEVICECREATE_HWVP | DEVICECREATE_STANDARD;// | DEVICECREATE_PURE;
    m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_HWVP | DEVICECREATE_STANDARD;
	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	//m_Options.ModeOptions.dwTexInclude  = PF_DMAP;
	m_Options.ModeOptions.dwTexExclude  = PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;

	LookupTypes[0] = LOOKUP;
    LookupTypes[1] = IMPLICIT;
	LookupTypes[2] = PRESAMPLED;
	LookupStr  [0] = "LOOKUP";
    LookupStr  [1] = "IMPLICIT";
	LookupStr  [2] = "PRESAMPLED";
	Filters    [0] = D3DTEXF_NONE;
	Filters    [1] = D3DTEXF_POINT;
	Filters    [2] = D3DTEXF_LINEAR;
	FiltersStr [0] = "NONE";
	FiltersStr [1] = "POINT";
	FiltersStr [2] = "LINEAR";

	m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.10f);
	m_pFramework->GetImageCmp()->SetPassRequired(0.90f);

	m_Presampled = m_Lookup = true;
	m_MaxTess = 0.f;
	m_nShaders = 7;
	for(int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i] = NULL;
		m_pDecls[i] = NULL;
	}
	if(KeySet(_T("NotAdapTess")))
		m_AdapTess = false;
	else
		m_AdapTess = true;
	
	m_Tex = false;
	m_DMap = true;
	/*m_Tex = true;
	m_DMap = true;
	if(KeySet(_T("dmap")))
		m_Tex = false;
	if(KeySet(_T("tex")))
		m_DMap = false;*/

}

UINT DisplacementMapping::TestInitialize(void)
{
	if (!GenerateVertexBuffer())
        return D3DTESTINIT_ABORT;
	
	if (!GenerateVertexShader())
		return D3DTESTINIT_ABORT;

	return D3DTESTINIT_RUN;
}

bool DisplacementMapping::GenerateTextures(bool isSolid, bool isSmaller)
{
	HRESULT  hr;
	CImage **ppImages = NULL;
	CImage  *pDMImage = NULL;
	CImage  *pImage   = NULL;
	FColor  *pDMData  = NULL;
	FColor  *pData    = NULL;
	FORMAT   Format;
    DWORD    Stripes[4];
	UINT     uDMWidth = 0, uDMHeight = 0;
	UINT     uWidth, uHeight;
	UINT     uLevels;
	bool     result = true;

	if ( isSmaller )
	{
		uWidth = uHeight = 64;
	}
	else
	{
		uWidth = uHeight = 128;
	}

	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps); 
	if (!(caps.TextureCaps  & D3DPTEXTURECAPS_MIPMAP) || m_isPreswizzled ||
		(m_pD3D->GetRuntimeVersion() >= 9.029f && FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), 
														  m_pCurrentMode->pDisplay->Format, USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, Format)) ))
		uLevels = 1;
	else
		uLevels = 0;

	Format = m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

	//size of presampled texture depends on tessellation
	if (m_isPreswizzled)
	{
		float n = m_MaxTess / (isSmaller ? 7.f : 8.f);
		for (int i = 0; i < (isSmaller ? 7 : 8); i++)
		{
			uDMWidth += ( (UINT)(n*(i+1) + 1) )*( (UINT)(n*(i+1) + 2) );
		}
		uDMWidth  = (UINT) ceil(sqrt((float)uDMWidth));
		UINT pow1 = (UINT) ceil( log((float)uDMWidth)/log(2.f) );
		uDMWidth  = 1 << pow1; 
		uDMHeight = uDMWidth;
	}
	else
	{
		uDMWidth  = uWidth;
		uDMHeight = uHeight;
	}
	
	//create and fill out standard texture which contant is goingto be used tofill out presampled DM texture

	if(!CreateTexture(uWidth, uHeight, uLevels, 0, FMT_A8R8G8B8, POOL_MANAGED, &m_pTexture))
	{
		WriteToLog("GenerateTextures: Create m_pTexture failed.\n");
		result = false;
		goto Exit;
	}

	if(m_pTexture->IsPalettized())
		m_pTexture->SetPalette(NULL);

	uLevels = m_pTexture->GetLevelCount();
	
	ppImages = new CImage*[uLevels];
	if (ppImages == NULL)
	{
		WriteToLog("GenerateTextures: out of memory on allocating ppImages.\n");
		result = false;
		goto Exit;
	}

	ZeroMemory(ppImages, sizeof(CImage *) * uLevels);

	UINT uW = uWidth, uH = uHeight;
	for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
	{
		ppImages[LevelIndex] = new CImage;
		if (ppImages[LevelIndex] == NULL)
		{
			WriteToLog("GenerateTextures: out of memory on allocating ppImages[%d] \n", LevelIndex);
			result = false;
			goto Exit;
		}
		
		WORD c = 0xff;// - LevelIndex*15;
		Stripes[(0 + LevelIndex)%4] = RGBA_MAKE(c, 0, 0, 0xff);
		Stripes[(1 + LevelIndex)%4] = RGBA_MAKE(0, c, 0, 0xff);
		Stripes[(2 + LevelIndex)%4] = RGBA_MAKE(0, 0, c, 0xff);
		Stripes[(3 + LevelIndex)%4] = RGBA_MAKE(c, c, c, 0xff);

		if (!ppImages[LevelIndex]->LoadStripes( uW, uH, isSolid ? 1:4, Stripes, (LevelIndex%2 ? true : false), LevelIndex%3==1?true:false ))
		{
			WriteToLog("GenerateTexture: LoadStripes() on Image failed for m_pTexture on level %d \n", LevelIndex);
			result = false;
			goto Exit;
		}

		uW >>= 1;
		uH >>= 1;
	}

	if ( !m_pTexture->LoadImage( 0, ppImages, TEXLOAD_DEFAULT))
	{
		WriteToLog("GenerateTexture: LoadImage to m_pTexture failed with HResult = %s.\n", m_pD3D->HResultToString(GetLastError()));
		result = false;
		goto Exit;
	}

	//create displacement mapping texture

	if(!CreateTexture(uDMWidth, uDMHeight, uLevels, USAGE_DMAP, Format, POOL_MANAGED, &m_pDMTexture))
	{
		WriteToLog("GenerateTextures: Create DMTexture failed.\n");
		result = false;
		goto Exit;
	}

	if(m_pDMTexture->IsPalettized())
		m_pDMTexture->SetPalette(NULL);

	//copy image from standard picture to presampled displacement mapping texture

	if (m_isPreswizzled)
	{
		float n = m_MaxTess / (isSmaller ? 7.f : 8.f);
		
		pDMImage = new CImage[uDMWidth*uDMHeight]; 
		if (NULL == pDMImage)
		{
			WriteToLog("GenerateTextures - out of memory allocating pDMImage.\n");
			result = false;
			goto Exit;
		}

		pImage = new CImage[uWidth*uHeight];
		if (NULL == pDMImage)
		{
			WriteToLog("GenerateTextures - out of memory allocating pImage.\n");
			result = false;
			goto Exit;
		}

		pDMData = new FColor[uDMWidth*uDMHeight];
		if (NULL == pDMData)
		{
			WriteToLog("GenerateTextures - out of memory allocating pDMData.\n");
			result = false;
			goto Exit;
		}

		pData = new FColor[uWidth*uHeight];
		if (NULL == pData)
		{
			WriteToLog("GenerateTextures - out of memory allocating pData.\n");
			result = false;
			goto Exit;
		}
		
		UINT offset = 0;

		for (UINT i = 0; i < (isSmaller ? 7 : 8); i++)
		{
			if (isSolid)
			{
				WORD c = 0xff - i*25;
				Stripes[0] = Stripes[1] = Stripes[2] = Stripes[3] = RGBA_MAKE(c, c, c, 0xff);
			}
			else
			{
				Stripes[(0/* + i*/)%4] = 0xff646464; //100
				Stripes[(1/* + i*/)%4] = 0xff969696; //150
				Stripes[(2/* + i*/)%4] = 0xffc8c8c8; //200
				Stripes[(3/* + i*/)%4] = 0xffffffff; //255
			}

			if ( !pImage->LoadStripes(uWidth, uHeight, 4, Stripes, true/*(i%2 ? false : true)*/, false/*i%3==2?true:false*/) )
			{
				WriteToLog("GenerateTextures -- LoadStripes on pImage failed\n");
				result = false;
				goto Exit;
			}

			pImage->GetData(*(DWORD*)&uWidth, *(DWORD*)&uHeight, pData);

			float TessLevel = (float)( (int)(n*(i+1)) ); 
			DWORD index1 = ((( (int)(n*(i+1)+1) )*( (int)(n*(i+1)+2) ))>>1) - 1;//((width*uDMHeight)>>1)-1;//(uDMWidth>>1)-1;
			DWORD index2 = index1+1;//(uDMWidth*uDMHeight)>>1;
			float fW = uWidth / TessLevel;
			
			for(int y = 0; y <= TessLevel; y++)
			{
                                int x;
                                for(x = y; x >= 0; x--)
				{
					// Determine which color we will be using.
					int x1 = x == 0 ? 0 : (int)(fW*x) - 1;//(int)w*x-1;
					int y1 = y == 0 ? 0 : (int)(fW*y) - 1;//((int)w*y-1);
					
					if (x1 > uWidth || y1 > uHeight)
						x1 = y1 = 0;

					pDMData[offset+index1--] = pData[x1+y1*uWidth/*(int)(w*(x+y*uWidth))*/];
				}
				//for(x = y; x <= m_TessLevel; x++)
				for( x = (int)TessLevel; x >= y; x--) 
				{
					// Determine which color we will be using.
					int x1 = x == 0 ? 0 : (int)(fW*x) - 1;//(int)w*x-1;
					int y1 = y == 0 ? 0 : (int)(fW*y) - 1;//((int)w*y-1);
					
					if (x1 > uWidth || y1 > uHeight)
						x1 = y1 = 0;

					pDMData[offset+index2++] = pData[x1+y1*uWidth/*(int)(w*(x+y*uWidth))*/];
				}
			}
			offset += (UINT)( (TessLevel+1)*(TessLevel+2) );
		}

		//fill out unused image
		for(UINT i = offset; i < uDMWidth*uDMHeight; i++)
			pDMData[i] = (UINT32)0x00000000;

		if(!pDMImage->SetData(uDMWidth, uDMHeight, pDMData))
		{
			WriteToLog("GenerateTextures -- Failed to set data in pDMImage.\n");
			result = false;
			goto Exit;
		}

		if ( !m_pDMTexture->LoadImage(1, &pDMImage))
		{
			WriteToLog("GenerateTextrures: LoadImage failed on m_pDMTexture.\n");
			result = false;
			goto Exit;
		}
	}
	else
	{	
		for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
		{
			if ( KeySet(_T("address")) )
			{
				//R * 0.2125f + G * 0.7154f + B * 0.0721f;
				WORD count = 256/uLevels;
				WORD r = (WORD)((float)(LevelIndex*count+count/2)/(3.f*0.2125f));
				WORD g = (WORD)((float)(LevelIndex*count+count/2)/(3.f*0.7154f));
				WORD b = (WORD)((float)(LevelIndex*count+count/2)/(3.f*0.0721f));
				Stripes[0] = Stripes[1] = Stripes[2] = Stripes[3] = RGBA_MAKE(r, g, b, 0);
			}
			else
			{
				if (isSolid)
				{
					WORD c = 0xff - LevelIndex*25;
					Stripes[0] = Stripes[1] = Stripes[2] = Stripes[3] = RGBA_MAKE(c, c, c, 0xff); 
				}
				else
				{
					WORD c = 0xff - LevelIndex*25;
					Stripes[(0 + LevelIndex)%4] = RGBA_MAKE(0x64, 0x64, 0x64, 0xff); 
					Stripes[(1 + LevelIndex)%4] = RGBA_MAKE(0x96, 0x96, 0x96, 0xff); 
					Stripes[(2 + LevelIndex)%4] = RGBA_MAKE(0xc8, 0xc8, 0xc8, 0xff); 
					Stripes[(3 + LevelIndex)%4] = RGBA_MAKE(0xff, 0xff, 0xff, 0xff); 
				}
			}
			
			if (!m_pDMTexture->LoadStripes(LevelIndex+1, uDMWidth, uDMHeight, 4, Stripes, (LevelIndex%2 ? false : true), false/*LevelIndex%3==2?true:false*/ ))
			{
				WriteToLog("GenerateTextrures: LoadStripes failed on m_pDMTexture.\n");
				return false;
			}

			uDMWidth  >>= 1;
			uDMHeight >>= 1;
		}
	}
	
Exit:
	if (!result)
	{
		RELEASE(m_pTexture);
		RELEASE(m_pDMTexture);
		Fail();
	}

	delete [] pDMImage;
	delete [] pImage;
	delete [] pDMData;
	delete [] pData;

	if (NULL != ppImages)
	for(UINT i = 0; i < uLevels; i++)
		SAFEDELETE(ppImages[i]);
	SAFEDELETEA(ppImages);

	return result;
}

bool DisplacementMapping::GenerateVertexBuffer(bool isLarger)
{
	UINT uWidth  = m_Options.D3DOptions.nBackBufferWidth;
	UINT uHeight = m_Options.D3DOptions.nBackBufferHeight;
	VOID* pV     = NULL;
	HRESULT hr;
	UINT m_nVertices = 6;
	VERTEX v1[6];
	
	float v = isLarger ? 1.99f : 0.99f;
	v1[0]  = VERTEX(D3DXVECTOR3( -1.f, -1.f, 1.f), D3DXVECTOR3( 0.f, 0.f, 1.f), 0xffffff, 0.01f,  0.01f);
	v1[1]  = VERTEX(D3DXVECTOR3( -1.f,  1.f, 1.f), D3DXVECTOR3( 0.f, 0.f, 1.f), 0xffffff, 0.01f,  v);
	v1[2]  = VERTEX(D3DXVECTOR3(  1.f, -1.f, 1.f), D3DXVECTOR3( 0.f, 0.f, 1.f), 0xffffff, v,  0.01f);
	v1[3]  = VERTEX(D3DXVECTOR3(  1.f,  1.f, 1.f), D3DXVECTOR3( 0.f, 0.f, 1.f), 0xffffff, v,  v);
	v1[4]  = VERTEX(D3DXVECTOR3(  1.f, -1.f, 1.f), D3DXVECTOR3( 0.f, 0.f, 1.f), 0xffffff, v,  0.01f);
	v1[5]  = VERTEX(D3DXVECTOR3( -1.f,  1.f, 1.f), D3DXVECTOR3( 0.f, 0.f, 1.f), 0xffffff, 0.01f,  v);

	if( !(CreateVertexBuffer( m_nVertices*sizeof(VERTEX), USAGE_NPATCHES, 0, POOL_MANAGED, &m_pVB1)))
	    return false;
	
	if (!m_pVB1->Lock( 0, sizeof(v1), (BYTE**)&pV, 0) || NULL ==pV)
	{
		WriteToLog( "Failed to lock m_pVertexBuffer.\n" );
		return false;
	}

	memcpy( pV, v1, sizeof(v1) );

	if (!m_pVB1->Unlock())
	{
		WriteToLog( "Failed to unlock m_pVertexBuffer.\n" );
		return false;
	}

	float texWidth  = 128.f;//256.f;
	float texHeight = 128.f;//256.f;
	float w = (2.f*texWidth ) / uWidth;
	float h = (2.f*texHeight) / uHeight;
	UINT  uLevels = 8;
	VERTEX *v2 = new VERTEX[4*uLevels];

	for (UINT li = 0; li < uLevels; li++)
	{
		v2[4*li+0]  = VERTEX(D3DXVECTOR3( 0.f, 0.f, 1.f), D3DXVECTOR3( 0.f, 0.f, 1.f), 0xffffff, 0.01f,  0.01f);
		v2[4*li+1]  = VERTEX(D3DXVECTOR3( 0.f,   h, 1.f), D3DXVECTOR3( 0.f, 0.f, 1.f), 0xffffff, 0.01f,  v);
		v2[4*li+2]  = VERTEX(D3DXVECTOR3(   w, 0.f, 1.f), D3DXVECTOR3( 0.f, 0.f, 1.f), 0xffffff, v,  0.01f);
		v2[4*li+3]  = VERTEX(D3DXVECTOR3(   w,   h, 1.f), D3DXVECTOR3( 0.f, 0.f, 1.f), 0xffffff, v,  v);
		w /= 2.f;
		h /= 2.f;
	}

	if( !(CreateVertexBuffer( 4*uLevels*sizeof(VERTEX), USAGE_NPATCHES,  0, POOL_MANAGED, &m_pVB2)))
	{
		delete [] v2;
	    return false;
	}
	
	if (!m_pVB2->Lock( 0, sizeof(VERTEX)*4*uLevels, (BYTE**)&pV, 0) || NULL == pV)
	{
		WriteToLog( "Failed to lock m_pVertexBuffer.\n" );
		delete [] v2;
		return false;
	}
	memcpy( pV, v2, sizeof(VERTEX)*4*uLevels );
	if (!m_pVB2->Unlock())
	{
		WriteToLog( "Failed to unlock m_pVertexBuffer.\n" );
		delete [] v2;
		return false;
	}

	delete [] v2;

	return true;
}

bool DisplacementMapping::GenerateVertexShader()
{
	HRESULT hr;
	LPD3DXBUFFER pShader = NULL;

	float color[4] = {0.f, 0.f, 0.f, 1.f };
	SetVertexShaderConstant(4, color, 1);
	
	if (m_Lookup)
	{
		VERTEXELEMENT decl0[] = 
			{
				{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
				{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
				//{0, 24, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,    0},
				{0, 28, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
				{0, 28, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_LOOKUP,  DECLUSAGE_SAMPLE,   0},
				DECL_END()
			};

		if(!CreateVertexDeclaration(decl0, &(m_pDecls[0])))
		{
			WriteToLog("DisplacementMapping::GenerateVertexShader - CreateVertexDeclaration() failed.\n");
			return false;
		}

		if ( KeySet(_T("address")) )
		{
			float colors[8][4] = {
				1.f, 1.f, 1.f, 0.f,
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 0.f, 0.f,
				0.f, 0.f, 1.f, 0.f,
				1.f, 1.f, 0.f, 0.f,
				1.f, 0.f, 1.f, 0.f,
				0.f, 1.f, 1.f, 0.f,
				0.5f, 0.5f, 0.5f, 0.f
			};
			SetVertexShaderConstant(8, colors, 8);
			float c[] = {8.f, 0.f, 0.f, 0.f};
			SetVertexShaderConstant(16, c, 1);

			TCHAR *szShader = _T(
				"vs.1.1\n"
				"dcl_position  v0   \n"
				"dcl_normal    v1   \n"
				//"dcl_color     v2   \n"
				"dcl_sample    v3   \n"
				"dcl_texcoord1 v4   \n"

				"m4x4 oPos, v0, c0   \n"
				"mul r0, v3, c16.x   \n"
				"mov a0.x,  r0.x     \n"
				"mov oD0,  c[8+a0.x] \n"
				"mov oT0,  v4        \n"
			);
			hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
			if (FAILED(hr))
			{
				WriteToLog(_T("DisplacementMapping::GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
							m_pD3D->HResultToString(hr));
				RELEASE(m_pDecls[0]);
				return false;
			}
		}
		else
		{
			TCHAR *szShader = _T(
				"vs.1.1\n"
				"dcl_position  v0   \n"
				"dcl_normal    v1   \n"
				//"dcl_color     v2   \n"
				"dcl_sample    v3   \n"
				"dcl_texcoord1 v4   \n"

				"m4x4 oPos, v0, c0  \n"
				/*"mov r0.y,  v3.x    \n" //-> green color
				"mov r0.xz, v3.x    \n"//"mov r0.xz, c4.xz    \n"
				"mov r0.w,  c4.w    \n"*/
				"mov oD0,  v3       \n"
				"mov oT0,  v4       \n"
			);
			hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
			if (FAILED(hr))
			{
				WriteToLog(_T("DisplacementMapping::GenerateVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
							m_pD3D->HResultToString(hr));
				RELEASE(m_pDecls[0]);
				return false;
			}
		}
		if (!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShaders[0])))
		{
			WriteToLog("DisplacementMapping::GenerateVertexShader -- CreateVertexShader() failed.\n");
			pShader->Release();
			RELEASE(m_pDecls[0]);
			return false;
		}
		pShader->Release();

		VERTEXELEMENT decl1[] = 
			{
				{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
				{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
				//{0, 24, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,    0},
				{0, 28, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_LOOKUP,  DECLUSAGE_SAMPLE,   0},
				DECL_END()
			};

		if(!CreateVertexDeclaration(decl1, &(m_pDecls[1])))
		{
			WriteToLog("DisplacementMapping::GenerateVertexShader - CreateVertexDeclaration1() failed.\n");
			return false;
		}
		
		TCHAR *szShader = _T(
			"vs.1.1\n"
			"dcl_position v0   \n"
			"dcl_normal   v1   \n"
			//"dcl_color    v2   \n"
			"dcl_sample   v3   \n"

			"m4x4 oPos, v0, c0 \n"
			/*"mov r0.z, v3.x    \n" //->blue
			"mov r0.x, c4.x    \n"
			"mov r0.y, c4.y    \n"
			"mov r0.w, c4.w    \n"*/
			"mov oD0,  v3      \n"
			//"mov oT0, v3 \n"
		);
		pShader = NULL;
		hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
		if (FAILED(hr))
		{
			WriteToLog(_T("DisplacementMapping::GenerateVertexShader -- D3DXAssembleShader1() failed, hr=%s\n"),
						m_pD3D->HResultToString(hr));
			RELEASE(m_pDecls[1]);
			return false;
		}
		if (!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShaders[1])))
		{
			WriteToLog("DisplacementMapping::GenerateVertexShader -- CreateVertexShader1() failed.\n");
			pShader->Release();
			RELEASE(m_pDecls[1]);
			return false;
		}
		pShader->Release();

		VERTEXELEMENT decl4[] = 
			{
				{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
				{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
				//{0, 24, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,    0},
				{0, 28, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
				{0, 28, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_LOOKUP,  DECLUSAGE_SAMPLE,   0},
				DECL_END()
			};
		if(!CreateVertexDeclaration(decl4, &(m_pDecls[4])))
		{
			WriteToLog("DisplacementMapping::GenerateVertexShader - CreateVertexDeclaration4() failed.\n");
			return false;
		}
		szShader = _T(
			"vs.1.1            \n"
			"dcl_position  v0  \n"
			"dcl_normal    v1  \n"
			//"dcl_color     v2  \n"
			"dcl_sample    v3  \n"
			"dcl_texcoord1 v4  \n"

			"mov oPos,   v0    \n"
			/*"mov r0.x,   v3.x  \n"
			"mov r0.y,  v3.x   \n"//->green
			"mov r0.xz, v3.x   \n"//"mov r0.xz, c4.xz   \n"
			"mov r0.w,  c4.w   \n"*/
			"mov oD0,    v3    \n"
			"mov oT0,    v4    \n"
			);
		hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
		if (FAILED(hr))
		{
			WriteToLog(_T("DisplacementMapping::GenerateVertexShader -- D3DXAssembleShader4() failed, hr=%s\n"),
						m_pD3D->HResultToString(hr));
			RELEASE(m_pDecls[4]);
			return false;
		}
		if (!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShaders[4])))
		{
			WriteToLog("DisplacementMapping::GenerateVertexShader -- CreateVertexShader4() failed.\n");
			pShader->Release();
			RELEASE(m_pDecls[4]);
			return false;
		}
		pShader->Release();

		VERTEXELEMENT decl5[] = 
			{
				{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
				{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
				//{0, 24, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,    0},
				{0, 28, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_LOOKUP,  DECLUSAGE_SAMPLE,   0},
				DECL_END()
			};

		if(!CreateVertexDeclaration(decl5, &(m_pDecls[5])))
		{
			WriteToLog("DisplacementMapping::GenerateVertexShader - CreateVertexDeclaration5() failed.\n");
			return false;
		}
		//float color[4] = {0.f, 0.f, 0.f, 0.f };//1.f, 1.f, 1.f};//0.5f, 0.5f, 0.5f};//1.f, 1.f, 1.f};
		//SetVertexShaderConstant(4, &color, 1);

		szShader = _T(
			"vs.1.1           \n"
			"dcl_position v0  \n"
			"dcl_normal   v1  \n"
			//"dcl_color    v2  \n"
			"dcl_sample   v3  \n"

			"mov oPos, v0     \n"
			/*"mov r0.z, v3.x   \n" //->blue
			"mov r0.xyw, c4.xyw   \n"*/
			"mov oD0,  v3     \n"
			//"mov oT0, v3 \n"
			);

		hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
		if (FAILED(hr))
		{
			WriteToLog(_T("DisplacementMapping::GenerateVertexShader -- D3DXAssembleShader5() failed, hr=%s\n"),
						m_pD3D->HResultToString(hr));
			RELEASE(m_pDecls[5]);
			return false;
		}
		if (!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShaders[5])))
		{
			WriteToLog("DisplacementMapping::GenerateVertexShader -- CreateVertexShader5() failed.\n");
			pShader->Release();
			RELEASE(m_pDecls[5]);
			return false;
		}
		pShader->Release();
	}

	if ( m_Presampled )
	{
		VERTEXELEMENT decl2[] = 
			{
				{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
				{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
				//{0, 24, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,    0},
				{0,  0, (DECLTYPE)D3DDECLTYPE_UNUSED,   DECLMETHOD_LOOKUPPRESAMPLED, DECLUSAGE_SAMPLE, 0},
				DECL_END()
			};

		if(!CreateVertexDeclaration(decl2, &(m_pDecls[2])))
		{
			WriteToLog("DisplacementMapping::GenerateVertexShader - CreateVertexDeclaration2() failed.\n");
			return false;
		}
		TCHAR *szShader = _T(
			"vs.1.1\n"
			"dcl_position v0   \n"
			"dcl_normal   v1   \n"
			//"dcl_color    v2   \n"
			"dcl_sample   v3   \n"

			"m4x4 oPos, v0, c0 \n"
			/*"mov r0.x, v3.x    \n"
			"mov r0.y, c4.y    \n"
			"mov r0.z, c4.z    \n"
			"mov r0.w, c4.w    \n"*/
			"mov oD0,  v3      \n"
			//"mov oT0, v3 \n"
			);
		hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
		if (FAILED(hr))
		{
			WriteToLog(_T("DisplacementMapping::GenerateVertexShader -- D3DXAssembleShader2() failed, hr=%s\n"),
						m_pD3D->HResultToString(hr));
			RELEASE(m_pDecls[2]);
			return false;
		}
		if (!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShaders[2])))
		{
			WriteToLog("DisplacementMapping::GenerateVertexShader -- CreateVertexShader2() failed.\n");
			pShader->Release();
			RELEASE(m_pDecls[2]);
			return false;
		}
		pShader->Release();
	}

	VERTEXELEMENT decl3[] = 
		{
			{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3,   DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
			//{0, 24, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,    0},
			{0, 28, (DECLTYPE)D3DDECLTYPE_FLOAT2,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
			DECL_END()
		};

	if(!CreateVertexDeclaration(decl3, &(m_pDecls[3])))
	{
		WriteToLog("DisplacementMapping::GenerateVertexShader - CreateVertexDeclaration3() failed.\n");
		return false;
	}

	TCHAR *szShader = _T(
		"vs.1.1\n"
		"dcl_position v0   \n"
		"dcl_normal   v1   \n"
		//"dcl_color    v2   \n"
		"dcl_texcoord v3   \n"

		"m4x4 oPos, v0, c0 \n"
		"mov oT0,   v3     \n"  
		//"mov oD0, v2     \n"
		);
	pShader = NULL;
	hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
	if (FAILED(hr))
	{
		WriteToLog(_T("DisplacementMapping::GenerateVertexShader -- D3DXAssembleShader3() failed, hr=%s\n"),
					m_pD3D->HResultToString(hr));
		RELEASE(m_pDecls[3]);
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
		"dcl_position v0  \n"
		"dcl_normal   v1  \n"
		//"dcl_color    v2  \n"
		"dcl_texcoord v3  \n"

		"mov oPos, v0     \n"
		"mov oT0,  v3     \n"
		);
	hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
	if (FAILED(hr))
	{
		WriteToLog(_T("DisplacementMapping::GenerateVertexShader -- D3DXAssembleShader6() failed, hr=%s\n"),
					m_pD3D->HResultToString(hr));
		RELEASE(m_pDecls[6]);
		return false;
	}
	if (!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShaders[6])))
	{
		WriteToLog("DisplacementMapping::GenerateVertexShader -- CreateVertexShader6() failed.\n");
		pShader->Release();
		RELEASE(m_pDecls[6]);
		return false;
	}
	pShader->Release();

	return true;
}

bool DisplacementMapping::CheckFilterSupport(D3DTEXTUREFILTERTYPE Filter, DWORD FilterCaps, WORD id)
{
	HRESULT hr;
	FORMAT Format;
	Format = m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
		
	//check if any filters but point are valid for current texture format
	int nAdapter = m_pSrcDevice->GetAdapterID();
	DISPLAYMODE Mode;
	hr = m_pD3D->GetAdapterDisplayMode( m_pSrcDevice->GetAdapterID(), &Mode );
	if( FAILED(hr) )
	{
		WriteToLog("CheckFilterSupport: GetAdapterDisplayMode() on src device returned %s (0x%x)\n", m_pD3D->HResultToString(hr), hr );
		return false;
	}
	hr = m_pD3D->CheckDeviceFormat(nAdapter, m_pSrcDevice->GetDevType(), Mode.Format, D3DUSAGE_QUERY_FILTER, RTYPE_TEXTURE, Format);
	
	switch (Filter)
	{
		case D3DTEXF_NONE:
		{
			switch (id)
			{
				case 0: case 1: case 2:
					return true;
				default:
					WriteToLog("Unknow filter ID\n");
					return false;
			}
		}
		case D3DTEXF_ANISOTROPIC:
		{
			switch (id)
			{
				case 0:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC))
						return false;
					break;
				case 1:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC))
						return false;
					break;
				default:
					WriteToLog("Unknow filter ID\n");
					return false;
			}
			//if hr fails that means that any filtering but point is not supported for this format
			if (FAILED(hr))
			{
				//WriteToLog("Current format can not be used with Linear Filter.\n");
				return false;
			}
			else
				return true;
		}
		case D3DTEXF_LINEAR:
		{
			switch (id)
			{
				case 0:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))
						return false;
					break;
				case 1:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MINFLINEAR))
						return false;
					break;
				case 2:
					if (!(FilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
						return false;
					break;
				default:
					WriteToLog("Unknow filter ID\n");
					return false;
			}
			//if hr fails that means that any filtering but point is not supported for this format
			if (FAILED(hr))
			{
				//WriteToLog("Current format can not be used with Linear Filter.\n");
				return false;
			}
			else
				return true;
		}
		case D3DTEXF_POINT:
		{
			switch (id)
			{
				case 0:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MAGFPOINT))
						return false;
					else 
						return true;
				case 1:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MINFPOINT))
						return false;
					else 
						return true;
				case 2:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MIPFPOINT))
						return false;
					else 
						return true;
				default:
					WriteToLog("Unknow filter ID\n");
					return false;
			}
		}
		default:
		{
			WriteToLog("Unknown filter type\n");
			return false;
		}
	}
}
	
