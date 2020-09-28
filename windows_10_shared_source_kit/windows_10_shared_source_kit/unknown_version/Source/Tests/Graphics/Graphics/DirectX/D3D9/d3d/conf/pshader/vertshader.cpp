// FILE:        VertShader.cpp
// DESC:        Test pixel shaders with software vertex shaders
// AUTHOR:      Anthony Clarke

#include <ddraw.h>
#include <d3d8.h>
#include <d3dx8.h>
#include "PShader.h"

extern MYTLVERTEX g_FourColorVertices[];

CPSVertShader::CPSVertShader()
{
    m_szTestName = "Vertex Shader/Pixel Shader combination";
    m_szCommandKey = "VertShader";

	m_bBump = false;
	m_bCube = false;
	m_bVol = false;
}

CPSVertShader_Bump::CPSVertShader_Bump()
{
    m_szTestName = "Vertex Shader/Pixel Shader combination (Bump)";
    m_szCommandKey = "VertShader_Bump";

	m_bBump = true;
}

CPSVertShader_Cube::CPSVertShader_Cube()
{
    m_szTestName = "Vertex Shader/Pixel Shader combination (Cube)";
    m_szCommandKey = "VertShader_Cube";

	m_bCube = true;
}

CPSVertShader_Vol::CPSVertShader_Vol()
{
    m_szTestName = "Vertex Shader/Pixel Shader combination (Volume)";
    m_szCommandKey = "VertShader_Volume";

	m_bVol = true;
}

CPSVertShader_BumpCube::CPSVertShader_BumpCube()
{
    m_szTestName = "Vertex Shader/Pixel Shader combination (BumpCube)";
    m_szCommandKey = "VertShader_BumpCube";

	m_bBump = true;
	m_bCube = true;
}

UINT CPSVertShader::TestInitialize()
{
    UINT uResult = CPSAddress::TestInitialize();

    if ( D3DTESTINIT_RUN != uResult )
		return uResult;

    if (m_Flags.Check(VBHARDWARE) && !m_Flags.Check(HWVERTSHADER))
    {
        WriteToLog(_T("Hardware vertex processing but vertex shaders are not supported.") );
        //Default Test range that is set so that D3DTESTINIT_SKIPALL will work. 
		//This should be reset by each test
        SetTestRange(1, 1);
        return D3DTESTINIT_SKIPALL;
    }

	ChangeVertexShaderType();

    return D3DTESTINIT_RUN;
}

void CPSVertShader::CommandLineHelp(void)
{
	WriteCommandLineHelp("$ySetStages: $wSet Texture co-ords only for MaxSimultaneousTextures");
    WriteCommandLineHelp("$ySetDefStages: $wSet Position and Texture co-ords defaults upto MaxSimultaneousTextures");
    WriteCommandLineHelp("$ySetDefMaxStages: $wSet Position and Texture co-ords defaults all possible D3D Texture stages");
	CPixelShader::CommandLineHelp();
	return;
}

void CPSVertShader::ChangeVertexShaderType()
{
	DWORD rgdwDec[MAX_FVF_DECL_SIZE] = {0};
	TCHAR sztCode[MAX_CODES+1];
	HRESULT hr;
	DWORD dwFlags = 0;
	LPD3DXBUFFER pXCode = NULL;
    LPD3DXBUFFER pXError = NULL;
	DWORD dwNumVerts, dwVBUsage;
	float *pfX, *pfY;
    float rgVertConst[16] = {   0.0f, -1.0f, 0.0f, 0.0f,
                                1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f};

	for
	(
		SHADERVECTOR::iterator iShader = m_vShaders.begin();
		iShader != m_vShaders.end();
		++iShader
	)
	{	        
		if (!GetVertexShaderCode(*iShader, sztCode, sizeof(sztCode), rgdwDec))
		{
			WriteToLog(_T("Failed to create vertex shader code. (%08X)\n"), hr);
			continue;
		}

        //vertex shaders to be created in the appropriate location.
        if (m_Flags.Check(VBHARDWARE))
            dwVBUsage = 0;
        else
            dwVBUsage = D3DUSAGE_SOFTWAREPROCESSING;
         
		if (SHADE_NEEDZ == (*iShader)->Type)
		{
			dwNumVerts = 8;
		}
		else
		{
			dwNumVerts = 4;
		}

		//This converts the screen coordinate position data back to clipping
		//space coords -1 to 1 range. Z is in th 0 to 1 range.
		for (int i = 0; i < dwNumVerts; ++i)
		{	
			pfX = (float*) ((BYTE*) (*iShader)->pVertices + (i * (*iShader)->dwVertexFormatSize));
			*pfX = (*pfX / m_pCurrentMode->nBackBufferWidth - 0.5f) * 2.0f;
			pfY = (float*) ( (BYTE*) (*iShader)->pVertices + ( (i * (*iShader)->dwVertexFormatSize) + sizeof(float) ) );
			*pfY = ( (-*pfY) / m_pCurrentMode->nBackBufferHeight + 0.5f) * 2.0f;
		}

		hr = D3DXAssembleShader(sztCode, strlen(sztCode), dwFlags, NULL, &pXCode, &pXError);
		if (FAILED(hr))
		{
			WriteToLog(_T("Vertex Shader failed to assemble. (%08X)\n"), hr);

            RELEASE(pXCode);
            RELEASE(pXError);
			continue;
		}

		if (!CreateVertexShader( rgdwDec, (DWORD*)pXCode->GetBufferPointer(), &(*iShader)->dwVSHandle, dwVBUsage))
		{
			WriteToLog(_T("Vertex Shader failed to create. (%08X)\n"), hr);

            RELEASE(pXCode);
            RELEASE(pXError);
			continue;
		}

		(*iShader)->vstPrimType = VST_PROG;

		ReCreateShaderAndBuffers(*iShader);

        RELEASE(pXCode);
        RELEASE(pXError);
	}

    //At the moment only one set of vertex shader constants is being used 
    //so this will be fine
    if (!SetVertexShaderConstant(0, rgVertConst, 4))
    {
        WriteToLog(_T("Failed to set vertex shader constants.") );
    }

   	return;
}

bool CPSVertShader::GetVertexShaderCode(MYSHADER * pShader, TCHAR *sztCode, DWORD dwSize, DWORD *rgdwDeclarFromFVF)
{
	HRESULT hr;
	int nVersion = (m_dwVersion < 0x0900) ? 0 : 1; // VS 1.0 is only available on Dx8
    DWORD dwNumTextureStages = D3DDP_MAXTEXCOORD;
	int i, nNumCoords = 0;

    //Make sure that the end of the string will be null terminated.
	dwSize--;    

	hr = DeclaratorFromFVF(pShader->dwVertexFormat, rgdwDeclarFromFVF);
	if (FAILED(hr))
	{
		WriteToLog(_T("Failed to convert FVF to Declaration. (%08X)\n"), hr);
		pShader++;
		return false;
	}
	
    //Change the stream
	rgdwDeclarFromFVF[0] = D3DVSD_STREAM(DEFAULT_VSSTREAM);
    
    if ( m_Flags.Check(SETSTAGES) || m_Flags.Check(SETDEFSTAGES) )
    {
        dwNumTextureStages = m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures;
    }

    if ( dwNumTextureStages < 4 )
    {
        WriteToLog ( _T("Too few texture stages! Must have a minimum of 4.") );
        return false;
    }    
    
    //D3DFVF_TEX0 = 0x000 D3DFVF_TEX8 = 0x800
    nNumCoords = (pShader->dwVertexFormat & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;

    if ( m_Flags.Check(SETDEFSTAGES) || m_Flags.Check(SETDEFMAXSTAGES) )
    {
       	_sntprintf(sztCode, dwSize, "vs.1.%d\n"                                
								"mov oPos, c3\n" //This should be initialized by the driver see man bug #52149
								"m3x3 oPos, v0, c0\n"
								"mov oD0, v5\n"
								"mov oD1, v6\n",nVersion); 

        for ( i = 0; i < dwNumTextureStages; ++i)
        {            
            if ( pShader->rgnCoordsForStage[i] < nNumCoords )
            {
		        _sntprintf( sztCode, dwSize,
				        _T("%smov oT%d, v%d\n"),
				        sztCode, i, D3DVSDE_TEXCOORD0 + pShader->rgnCoordsForStage[i]);

                if ( ( pShader->dwVertexFormat & D3DFVF_TEXCOORDSIZE1(pShader->rgnCoordsForStage[i]) ) ==  D3DFVF_TEXCOORDSIZE1(pShader->rgnCoordsForStage[i]) )
                {
                    _sntprintf( sztCode, dwSize, _T("%smov oT%d.yzw, c3\n"), sztCode, i );
                }
                else if ( ( pShader->dwVertexFormat & D3DFVF_TEXCOORDSIZE3(pShader->rgnCoordsForStage[i]) ) ==  D3DFVF_TEXCOORDSIZE3(pShader->rgnCoordsForStage[i]) )
                {
                    _sntprintf( sztCode, dwSize, _T("%smov oT%d.w, c3\n"), sztCode, i );
                }
                else if ( ( pShader->dwVertexFormat & D3DFVF_TEXCOORDSIZE4(pShader->rgnCoordsForStage[i]) ) !=  D3DFVF_TEXCOORDSIZE4(pShader->rgnCoordsForStage[i]) )
                {
                    _sntprintf( sztCode, dwSize, _T("%smov oT%d.zw, c3\n"), sztCode, i );
                }           
            }
            else
            {
                _sntprintf( sztCode, dwSize,
				        _T("%smov oT%d, c3\n"),
				        sztCode, i);
            }
        }
    }
    else
    {
        _sntprintf(sztCode, dwSize, "vs.1.%d\n"
                                "mov oPos, c3\n" //This should be initialized by the driver see man bug #52149
                                "m3x3 oPos, v0, c0\n"
								"mov oD0, v5\n"
    							"mov oD1, v6\n",nVersion);

        for ( i = 0; i < dwNumTextureStages; ++i)
        {
            if ( pShader->rgnCoordsForStage[i] < nNumCoords )
            {
		        _sntprintf( sztCode, dwSize,
				    _T("%smov oT%d, v%d\n"),
				    sztCode, i, D3DVSDE_TEXCOORD0 + pShader->rgnCoordsForStage[i]);
            }
            else
            {
                //You must output continuous co-ordinates from a shader.
                //Once one set of co-ords cannot be written out no set higher
                //can be written.
                break;
            }
        }
    }
    //Make sure that the end of the string will be null terminated.
    sztCode[dwSize] = '\0';
    int num = _tcslen(sztCode);
    if ( _tcslen(sztCode) == dwSize)
    {
        WriteToLog( _T("Out of string space for vertex shader") );
        return false;
    }
	return true;
}

void CPSVertShader::ProcessArgs(void)
{
    CPixelShader::ProcessArgs();
    if (KeySet("WHQL"))
    {		
        return;
    }
    if (KeySet("SETDEFSTAGES"))
    {
        m_Flags.Set(SETDEFSTAGES);
    }
    if (KeySet("SETDEFMAXSTAGES"))
    {
        m_Flags.Set(SETDEFMAXSTAGES);
    }
    if (KeySet("SETSTAGES"))
    {
        m_Flags.Set(SETSTAGES);
    }
    return;
}

HRESULT CPSVertShader::DeclaratorFromFVF(DWORD dwFVF, DWORD Declarator[MAX_FVF_DECL_SIZE])
{
	HRESULT hrResult = D3D_OK;
    DWORD iTemp;

    iTemp = 0;
    Declarator[iTemp] = D3DVSD_STREAM(0);
    iTemp++;
   
    if (dwFVF & D3DFVF_POSITION_MASK)
    {
        if ((dwFVF & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW)
            Declarator[iTemp] = D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT4);
        else
            Declarator[iTemp] = D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT3);

        iTemp++;
    }

	if (dwFVF & D3DFVF_NORMAL)
    {
        Declarator[iTemp] = D3DVSD_REG( D3DVSDE_NORMAL,  D3DVSDT_FLOAT3);
        iTemp++;
    }

    if (dwFVF & D3DFVF_PSIZE)
    {
        Declarator[iTemp] = D3DVSD_REG( D3DVSDE_PSIZE,  D3DVSDT_FLOAT1);
        iTemp++;
    }

	if (dwFVF & D3DFVF_DIFFUSE)
    {
        Declarator[iTemp] = D3DVSD_REG( D3DVSDE_DIFFUSE,  D3DVSDT_D3DCOLOR);
        iTemp++;
    }

	if (dwFVF & D3DFVF_SPECULAR)
    {
        Declarator[iTemp] = D3DVSD_REG( D3DVSDE_SPECULAR,  D3DVSDT_D3DCOLOR);
        iTemp++;
    }

    if (dwFVF & D3DFVF_TEXCOUNT_MASK)
    {
	    DWORD iTexCoord;
        DWORD dwNumTexCoords = ((dwFVF & D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
		const DWORD rgdwTexCoordSize[] = { 2 * sizeof(float), 3 * sizeof(float), 4 * sizeof(float), sizeof(float)};

        if(dwNumTexCoords > 8)
            dwNumTexCoords = 8;
	
		if (dwNumTexCoords > 0)
        {
            for (iTexCoord = 0; iTexCoord < dwNumTexCoords; iTexCoord++)
            {
				// D3DVSDT_FLOAT1 = 0, D3DVSDT_FLOAT4 = 3
				Declarator[iTemp] = D3DVSD_REG( D3DVSDE_TEXCOORD0 + iTexCoord,  rgdwTexCoordSize[(dwFVF >> (16 + iTexCoord * 2)) & 0x3] / sizeof(float) - 1);
				iTemp++;
            }
        }
    }

    Declarator[iTemp] = D3DVSD_END();

	return hrResult;
}
