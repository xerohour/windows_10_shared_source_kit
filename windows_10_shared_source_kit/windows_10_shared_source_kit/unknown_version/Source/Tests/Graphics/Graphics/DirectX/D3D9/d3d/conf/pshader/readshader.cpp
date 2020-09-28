// FILE:        ReadShader.cpp
// DESC:        Read Shader is designed to read in pixel shaders
//				from and text file resource. This allows know problamatic
//				shaders to be tested easily.
// AUTHOR:      Anthony Clarke

#include <ddraw.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "PShader.h"
#include "CResFile.h"

extern MYTLVERTEX g_FourColorVertices[];
extern MYTLVERTEXTEX4 g_FourColorVerticesTex4[];
extern MYTLVERTEXTEX6 g_FourColorVerticesTex6[];
extern MYCUBETLVERTEX g_TexmZtests[];
extern MYTLVERTEX3 g_Texm3Vertices[];

CReadShader::CReadShader()
{
	m_szTestName = _T("Read Shader");
    m_szCommandKey = _T("ReadShader");

    m_pStripesTex = NULL;
	m_pGradientTex = NULL;
    m_pSineBumpTex = NULL;
	return;
}

CReadShader::~CReadShader()
{
	TestTerminate();
	return;
}

void CReadShader::CommandLineHelp(void)
{
	WriteCommandLineHelp("$yfilename: $wfilename to read shaders from");
	CPixelShader::CommandLineHelp();
	return;
}

UINT CReadShader::TestInitialize(void)
{
	m_byFormats = TEXFORMAT | BUMPFORMAT;
	UINT uResult = CPixelShader::TestInitialize();
    if ( D3DTESTINIT_RUN != uResult )
		return uResult;
	
	ProcessArgs();
    PopulateShaderInfo();

	DWORD dwShaders = m_vShaders.size();

	if (dwShaders == 0)
	{
		SetTestRange(1, 1);
		return D3DTESTINIT_SKIPALL;
	}

	SetTestRange(1, dwShaders * m_uTotalFormats * m_uNumShaderVersions);
    return D3DTESTINIT_RUN;
}

HRESULT CReadShader::AddShader(TCHAR *sztType, TCHAR *sztName, TCHAR *sztCodes, DWORD dwVersion)
{
	// Based on type we should load different texture for the shader.
	if ( _tcsstr(sztType, _T("tex")) )
	{
		// Don't use defaults anymore for the unused channels
		if (!strcmp("texcrd re-use reg", sztName))
			AddShaderDef(sztName, sztCodes, MYCUBETLVERTEX_FVF, g_TexmZtests, true, SHADE_PLAIN, dwVersion);
		else
		{
			if ( _tcsstr(sztType, _T("tex14")) )
				AddShaderDef(sztName, sztCodes, MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, true, SHADE_PLAIN, dwVersion);
			else
				AddShaderDef(sztName, sztCodes, MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, true, SHADE_PLAIN, dwVersion);
		}

		AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);		
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
		if ( _tcsstr(sztType, _T("tex14")) )
		{
			AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
			AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
		}

		if (!strcmp("Re-use the texop source", sztName))
			SetLastShaderWrapAndMip();
	}
	else if ( _tcsstr(sztType, _T("bump")) )
	{
		AddShaderDef(sztName, sztCodes, MYTLVERTEX_FVF, g_FourColorVertices, SHADE_BUMP, dwVersion);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pSineBumpTex);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
		if ( _tcsstr(sztType, _T("bump2")) )
		{
			AddBumpStageForLastShaderDef(2);
			AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex, 2);

			if (!strcmp("Re-use the texop source", sztName))
				SetLastShaderWrapAndMip();
		}
        SetLastShaderTexFormats(TEXFORMAT | BUMPFORMAT);
	}
	else if ( _tcsstr(sztType, _T("needz")) )
	{
		AddShaderDef(sztName, sztCodes, MYCUBETLVERTEX_FVF, g_TexmZtests, SHADE_NEEDZ, dwVersion);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
		SetCoordsForLastPSStages(2, 1);
		SetCoordsForLastPSStages(3, 2);
	}
	else if ( _tcsstr(sztType, _T("proj")) )
	{
		AddShaderDef(sztName, sztCodes, MYTLVERTEX3_FVF, g_Texm3Vertices, true, SHADE_PROJ, dwVersion);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);		
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);		
		if ( _tcsstr(sztType, _T("proj14")) )
		{
			AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
			AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

			if (!strcmp("re-use coords proj _dw", sztName) || !strcmp("re-use coords proj _dz", sztName))
				SetLastShaderWrapAndMip();
		}
	}
	else
	{
		AddShaderDef(sztName, sztCodes, MYTLVERTEX_FVF, g_FourColorVertices, SHADE_PLAIN, dwVersion);
		//Default setup.
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);		
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);
		AddTextureToLastShaderDef((CnBaseTexture**)&m_pStripesTex);
	}
	return S_OK;
}

HRESULT CReadShader::PopulateShaderInfo()
{
	CResFile ResFile;
	TCHAR sztBuffer[BUFFER_LENGTH+1] = _T("");
	TCHAR sztType[LINE_LENGTH] = _T("");
	TCHAR sztName[LINE_LENGTH] = _T("");
	TCHAR sztLine[LINE_LENGTH*2+1] = _T("");
	TCHAR sztShaderIns[MAX_CODES] = _T("");
	TCHAR * ptcTemp = NULL;
	TCHAR * ptcRemainder = NULL;
	DWORD dwBytesRead = 0;
	DWORD dwShaderVersion = 0;
	int nMinorVersion = 0;
	int nMajorVersion = 0;
	bool bInShader = false;

	//Opens a resource first if it exists then opens a file.
	if (!ResFile.Open(m_sztFilename, CREITHER, FALSE, NULL, "SHADERS") )
	{
		WriteToLog( _T("Failed to open shader file\n") );
		return E_INVALIDARG;
	}
	sztLine[0] = _T('\0');
	sztShaderIns[0] = _T('\0');
	memset(sztBuffer,0,sizeof(sztBuffer));
	
	// File reading
	while (ResFile.Read((void*)sztBuffer, sizeof(sztBuffer) - sizeof(TCHAR), &dwBytesRead))
	{
		//Buffer is null terminated as we copy size - 1 TCHARS.
		ptcRemainder = sztBuffer;

		while (ptcRemainder)
		{
			//This is the getline function the should be added to ResFile

			if ( _T('\n') == *ptcRemainder)
				ptcRemainder++;
			_tcsncat(sztLine, ptcRemainder, LINE_LENGTH);
			//Just a little not lines are terminated with \r\n
			// carridge return and newline.
			if ( ptcTemp = _tcsstr(sztLine, _T("\r")) )
			{
				// Null terminate the line
				ptcTemp[0] = _T('\0');
				// Increament the remainder pointer
				ptcRemainder = _tcsstr(ptcRemainder, _T("\n"));
				if (NULL != ptcRemainder)
				{					
					ptcRemainder++;
				}

				if ( ptcTemp = _tcsstr(sztLine, _T("end:")) )
				{
					bInShader = false;

					if (!strcmp("cmp >= 0.0", sztName) ||
						!strcmp("Neg, bx2 and bias of constant", sztName) ||
						!strcmp("Re-use the texop source 2 x texbem", sztName))
					{
						if (m_dwVersion < 0x0900)
						{
							//Create the shader
							AddShader(sztType, sztName, sztShaderIns, dwShaderVersion);
						}
					}
					else
					{
						//Create the shader
						AddShader(sztType, sztName, sztShaderIns, dwShaderVersion);
					}

					//reset shader codes.
					sztShaderIns[0] = _T('\0');
				}
				else if (bInShader)
				{						
					_tcscat(sztShaderIns, sztLine);					
					_tcscat(&sztShaderIns[_tcslen(sztShaderIns)], _T("\n\0"));
				}
				else if ( ptcTemp = _tcsstr(sztLine, _T("type:")) )
				{
					_tcscpy(sztType,  &ptcTemp[5]);						
				}
				else if ( ptcTemp = _tcsstr(sztLine, _T("name:")) )
				{
					_tcscpy(sztName,  &ptcTemp[5]);
				}
				else if ( ptcTemp = _tcsstr(sztLine, _T("ps.")) )
				{
					nMajorVersion = atoi(&ptcTemp[3]);
					ptcTemp = _tcsstr(&ptcTemp[3], _T("."));
					nMinorVersion = atoi(&ptcTemp[1]);
					dwShaderVersion = PSVERSION( nMajorVersion, nMinorVersion );

					if (dwShaderVersion < m_dwMinPSVersion)
						dwShaderVersion = m_dwMinPSVersion;

					//Now we can get all the shader code.
					bInShader = true;
				}
				//reset line
				sztLine[0] = _T('\0');
			}
			else if ( _tcslen(sztLine) <= LINE_LENGTH &&  !_tcscmp(ptcRemainder,sztLine) )
			{
				//need to read some more buffer
				ptcRemainder = NULL;
			}
			else
			{
				//bad news file is bad.
				WriteToLog(_T("Text file is bad. Line length may have been exceeded.") );
				return E_UNEXPECTED;
			}
		}
		//Clear the buffer
		if (dwBytesRead > 0)
			memset(sztBuffer,0,sizeof(sztBuffer));
	}

	ResFile.Close();
	return S_OK;
}

bool CReadShader::ExecuteTest(UINT uiTest)
{
	return CPixelShader::ExecuteTest(uiTest);
}

bool CReadShader::TestTerminate(void)
{		    
    return CPixelShader::TestTerminate();
}

void CReadShader::ReleaseTextures()
{
    RELEASE(m_pStripesTex);
    RELEASE(m_pSineBumpTex);
	RELEASE(m_pGradientTex);
	CPixelShader::ReleaseTextures();
    return;
}

bool CReadShader::GenerateTextureData()
{
	//Stripes
	D3DCOLOR pdwColors[] = {
        D3DRGBA(1, 1, 1, 1),
        D3DRGBA(1, 0, 0, 1),
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(0, 0, 1, 1)
    };
	DWORD dwCount = sizeof(pdwColors)/sizeof(D3DCOLOR);
	if ( !m_StripesImage.LoadStripes(LARGE_TEXSIZE, LARGE_TEXSIZE, dwCount, pdwColors, true, true) )
    {
        WriteToLog( _T("(ERROR) Failed to generate stripes.  Aborting.\n") );
        return false;
    }

	// Gradient
	DWORD dwColors[4] =
    {
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(1, 1, 1, 0),
        D3DRGBA(1, 1, 1, 0)
    };
    if ( !m_GradientImage.LoadGradient(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwColors) )
    {
        WriteToLog( _T("(ERROR) Failed to create gradient data.  Aborting.\n") );
        return false;
    }

	//Now bump data
	//Sine Texture.
	if ( !GenerateBumpMap(&m_SineBumpImage) )
	{
		WriteToLog( _T("(ERROR) Failed to generate sine data.  Aborting.\n") );
		return false;
	}

	return CPixelShader::GenerateTextureData();
}


bool CReadShader::LoadTextures()
{
	if (!SetTextureFormat(TEXFORMAT))
		return false;

    DWORD dwLevels = (DWORD) 1;
    DWORD dwPool = POOL_DEFAULT;
    DWORD dwUsage = (DWORD) 0;
    FORMAT *pFormat = &m_pTexFormats[m_pCurrentMode->nTextureFormat];

	//Stripes
    if (!CreateTexture(LARGE_TEXSIZE, LARGE_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pStripesTex))
    {
        WriteToLog(_T("(ERROR) Failed to create Stripes texture.  Aborting.\n"));
        return false;
    }	
    if (!m_pStripesTex->LoadImage(dwLevels, &m_StripesImage))
    {
        WriteToLog(_T("Unable to load stripes texture.\n"));
        return false;
    }
	
	//Gradient
    if (!CreateTexture(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pGradientTex))
    {
        WriteToLog(_T("(ERROR) Failed to create gradient texture.  Aborting.\n"));
        return false;
    }
    if (!m_pGradientTex->LoadImage(dwLevels, &m_GradientImage))
    {
        WriteToLog(_T("Unable to load gradient texture.\n"));
        return false;
    }

	// Now do the bump map textures    
    if (!SetTextureFormat(BUMPFORMAT))
    {
        WriteToLog("(WARNING) No common BumpDuDv formats.\n");
		return false;
    }
    else
    {		
        pFormat = &m_pBumpFormats[m_pCurrentMode->nTextureFormat];

        if (!CreateTexture(LARGE_TEXSIZE, LARGE_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pSineBumpTex))
        {
            WriteToLog(_T("(ERROR) Failed to create bump environment map sine texture.  Aborting.\n"));
            return false; 
        }
        if (!m_pSineBumpTex->LoadImage(dwLevels, &m_SineBumpImage))
        {
            WriteToLog(_T("Unable to load bump sine texture.\n"));
            return false;
        }
		// Reset to current non bump format
		SetTextureFormat(TEXFORMAT);
    }
	return CPixelShader::LoadTextures();
}


void CReadShader::ProcessArgs(void)
{
    CPixelShader::ProcessArgs();
    if (KeySet("WHQL"))
    {
        _tcscpy(m_sztFilename, _T("SHADER1"));
        return;
    }

    //Get shader file
    //default:  SHADER1
	ReadString(_T("filename"), _T("SHADER1"), m_sztFilename, ARRAYSIZE(m_sztFilename));    
	return;
}
