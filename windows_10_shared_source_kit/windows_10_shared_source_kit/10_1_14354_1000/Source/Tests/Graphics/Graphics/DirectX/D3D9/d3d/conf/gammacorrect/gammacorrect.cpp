#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "GammaCorrect.h"

// Global Test and App instance
CD3DWindowFramework App;
GammaCorrect        TestReadTexDefault(D3DUSAGE_QUERY_SRGBREAD, RTYPE_TEXTURE, D3DPOOL_DEFAULT );
GammaCorrect        TestWriteTexDefault(D3DUSAGE_QUERY_SRGBWRITE, RTYPE_TEXTURE, D3DPOOL_DEFAULT  );
GammaCorrect        TestReadWriteTexDefault(D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE, RTYPE_TEXTURE, D3DPOOL_DEFAULT );
GammaCorrect        TestReadCubeDefault(D3DUSAGE_QUERY_SRGBREAD, RTYPE_CUBETEXTURE, D3DPOOL_DEFAULT);
GammaCorrect        TestWriteCubeDefault(D3DUSAGE_QUERY_SRGBWRITE, RTYPE_CUBETEXTURE, D3DPOOL_DEFAULT);
GammaCorrect        TestReadWriteCubeDefault(D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE, RTYPE_CUBETEXTURE, D3DPOOL_DEFAULT);
GammaCorrect        TestReadVolDefault(D3DUSAGE_QUERY_SRGBREAD, RTYPE_VOLUMETEXTURE, D3DPOOL_DEFAULT);
GammaCorrect        TestWriteVolDefault(D3DUSAGE_QUERY_SRGBWRITE, RTYPE_VOLUMETEXTURE, D3DPOOL_DEFAULT);
GammaCorrect        TestReadWriteVolDefault(D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE, RTYPE_VOLUMETEXTURE, D3DPOOL_DEFAULT);
GammaCorrect        TestReadTexManaged(D3DUSAGE_QUERY_SRGBREAD, RTYPE_TEXTURE, D3DPOOL_MANAGED );
GammaCorrect        TestWriteTexManaged(D3DUSAGE_QUERY_SRGBWRITE, RTYPE_TEXTURE, D3DPOOL_MANAGED );
GammaCorrect        TestReadWriteTexManaged(D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE, RTYPE_TEXTURE, D3DPOOL_MANAGED );
GammaCorrect        TestReadCubeManaged(D3DUSAGE_QUERY_SRGBREAD, RTYPE_CUBETEXTURE, D3DPOOL_MANAGED);
GammaCorrect        TestWriteCubeManaged(D3DUSAGE_QUERY_SRGBWRITE, RTYPE_CUBETEXTURE, D3DPOOL_MANAGED);
GammaCorrect        TestReadWriteCubeManaged(D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE, RTYPE_CUBETEXTURE, D3DPOOL_MANAGED);
GammaCorrect        TestReadVolManaged(D3DUSAGE_QUERY_SRGBREAD, RTYPE_VOLUMETEXTURE, D3DPOOL_MANAGED);
GammaCorrect        TestWriteVolManaged(D3DUSAGE_QUERY_SRGBWRITE, RTYPE_VOLUMETEXTURE, D3DPOOL_MANAGED);
GammaCorrect        TestReadWriteVolManaged(D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE, RTYPE_VOLUMETEXTURE, D3DPOOL_MANAGED);
GammaCorrect        TestReadTexSystem(D3DUSAGE_QUERY_SRGBREAD, RTYPE_TEXTURE, D3DPOOL_SYSTEMMEM );
GammaCorrect        TestWriteTexSystem(D3DUSAGE_QUERY_SRGBWRITE, RTYPE_TEXTURE, D3DPOOL_SYSTEMMEM  );
GammaCorrect        TestReadWriteTexSystem(D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE, RTYPE_TEXTURE, D3DPOOL_SYSTEMMEM );
GammaCorrect        TestReadCubeSystem(D3DUSAGE_QUERY_SRGBREAD, RTYPE_CUBETEXTURE, D3DPOOL_SYSTEMMEM);
GammaCorrect        TestWriteCubeSystem(D3DUSAGE_QUERY_SRGBWRITE, RTYPE_CUBETEXTURE, D3DPOOL_SYSTEMMEM);
GammaCorrect        TestReadWriteCubeSystem(D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE, RTYPE_CUBETEXTURE, D3DPOOL_SYSTEMMEM);
GammaCorrect        TestReadVolSystem(D3DUSAGE_QUERY_SRGBREAD, RTYPE_VOLUMETEXTURE, D3DPOOL_SYSTEMMEM);
GammaCorrect        TestWriteVolSystem(D3DUSAGE_QUERY_SRGBWRITE, RTYPE_VOLUMETEXTURE, D3DPOOL_SYSTEMMEM);
GammaCorrect        TestReadWriteVolSystem(D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE, RTYPE_VOLUMETEXTURE, D3DPOOL_SYSTEMMEM);


DWORD g_TextureOp[] = {
    D3DTOP_SELECTARG1,      // the default
    D3DTOP_SELECTARG2,
	// Modulate
    D3DTOP_MODULATE,      // multiply args together
    D3DTOP_MODULATE2X,    // multiply and  1 bit
    D3DTOP_MODULATE4X,    // multiply and  2 bits
    // Add
    D3DTOP_ADD,           // add arguments together
    D3DTOP_ADDSIGNED,     // add with -0.5 bias
    D3DTOP_ADDSIGNED2X,   // as above but left  1 bit
    D3DTOP_SUBTRACT,      // Arg1 - Arg2, with no saturation
    D3DTOP_ADDSMOOTH,     // add 2 args, subtract product
};

TCHAR *g_TextureOpNames[] = {
	"D3DTOP_SELECTARG1",    
    "D3DTOP_SELECTARG2",
	// Modulate
    "D3DTOP_MODULATE",      
    "D3DTOP_MODULATE2X",
    "D3DTOP_MODULATE4X",
    // Add
    "D3DTOP_ADD",
    "D3DTOP_ADDSIGNED",
    "D3DTOP_ADDSIGNED2X",
    "D3DTOP_SUBTRACT",
    "D3DTOP_ADDSMOOTH",
};

//-----------------------------------------------------------------------------

GammaCorrect::GammaCorrect(DWORD option, RESOURCETYPE RType, D3DPOOL pool)
{
	SetTestType(TESTTYPE_CONF);

	m_Options.D3DOptions.fMinDXVersion = 9.f;

	m_Options.ModeOptions.dwTexExclude   = PF_BUMPDUDV | PF_BUMPLUMINANCE;
	m_Options.ModeOptions.dwCubeExclude  = PF_BUMPDUDV | PF_BUMPLUMINANCE;
	m_Options.ModeOptions.dwVolExclude   = PF_BUMPDUDV | PF_BUMPLUMINANCE;
	
	m_pFramework->GetImageCmp()->SetTolPerPixel(0.05f);
	m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.05f);
	m_pFramework->GetImageCmp()->SetPassRequired(0.95f);
	m_pFramework->GetImageCmp()->SetMinPixels(0);

	//m_Options.ModeOptions.dwTexInclude = PF_FOURCC;
	//m_Options.ModeOptions.dwCubeInclude = PF_FOURCC;
	//m_Options.ModeOptions.dwVolInclude = PF_FOURCC;

	m_isRead     = false;
	m_isSrcRead  = false;
	m_isRefRead  = false;
	m_isWrite    = false;
	m_isSrcWrite = false;
	m_isRefWrite = false;

	m_RType = RType;
	m_Pool = pool;

	m_pVertexBuffer = NULL;
	m_pTexture1     = NULL;
	m_pTexture2     = NULL;
	
	if ( option == (D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE) )
	{
		if(KeySet(_T("SrcRead")))
		m_isSrcRead = true;
		else if(KeySet(_T("RefRead")))
			m_isRefRead = true;
		else if(KeySet(_T("NotRead")))
			m_isRead = false;
		else
			m_isRead = true;

		if (KeySet(_T("SrcWrite")))
			m_isSrcWrite = true;
		else if(KeySet(_T("RefWrite")))
			m_isRefWrite = true;
		else if(KeySet(_T("NotWrite")))
			m_isWrite = false;
		else
			m_isWrite = true;
		
		strcpy(szTestName, "READ-WRITE ");
		strcpy(szCommandKey, "rw");
	}
	else if ( option == D3DUSAGE_QUERY_SRGBREAD )
	{
		if(KeySet(_T("SrcRead")))
		m_isSrcRead = true;
		else if(KeySet(_T("RefRead")))
			m_isRefRead = true;
		else if(KeySet(_T("NotRead")))
			m_isRead = false;
		else
			m_isRead = true;

		strcpy(szTestName, "READ ");
		strcpy(szCommandKey, "r");
	}
	else 
	{
		if(KeySet(_T("SrcWrite")))
		m_isSrcWrite = true;
		else if(KeySet(_T("RefWrite")))
			m_isRefWrite = true;
		else if(KeySet(_T("NotWrite")))
			m_isWrite = false;
		else
			m_isWrite = true;

		strcpy(szTestName, "WRITE ");
		strcpy(szCommandKey, "w");
	}

	//choose pool
	switch (pool)
	{
		case D3DPOOL_DEFAULT:
			sprintf(szTestName, "%sDEFAULT ", szTestName);
			sprintf(szCommandKey, "%sdef", szCommandKey);
			break;
		case D3DPOOL_MANAGED:
			sprintf(szTestName, "%sMANAGED ", szTestName);
			sprintf(szCommandKey, "%sman", szCommandKey);
			break;
		case D3DPOOL_SYSTEMMEM:
			sprintf(szTestName, "%sSYSTEMMEM ", szTestName);
			sprintf(szCommandKey, "%ssys", szCommandKey);
			break;
	}

	//choose texture type
	switch (RType)
	{
		case RTYPE_TEXTURE:
		{
			m_Options.ModeOptions.TextureFmt = FMT_ALL;
			sprintf(szTestName, "%sTEXTURE", szTestName);
			sprintf(szCommandKey, "%stex", szCommandKey);
			break;
		}
		case RTYPE_CUBETEXTURE:
		{
			m_Options.ModeOptions.CubeFmt = FMT_ALL;
			sprintf(szTestName, "%sCUBE TEXTURE", szTestName);
			sprintf(szCommandKey, "%scube", szCommandKey);
			break;
		}
		case RTYPE_VOLUMETEXTURE:
		{
			m_Options.ModeOptions.VolumeFmt = FMT_ALL;
			sprintf(szTestName, "%sVOLUME TEXTURE", szTestName);
			sprintf(szCommandKey, "%svol", szCommandKey);
			break;
		}
	}

	m_szTestName = _T(szTestName);
	m_szCommandKey = _T(szCommandKey);
}

//-----------------------------------------------------------------------------

bool GammaCorrect::PreModeSetup()
{
	HKEY hRegKey;
	TCHAR ShaderRange[MAX_PATH];

	m_bCapsFileExisted       = false;
	m_szCapsFileValue[ 0 ]   = '\0';

	if ( DRIVERMODEL_LDDM == m_pD3D->GetDriverModel() )
		m_isLDDMDriver = true;
	else
		m_isLDDMDriver = false;
	
	//this is only for blending cases for dx9.L
	if ( m_isLDDMDriver && !KeySet("noregkey"))
	{
		if ( !(m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps & D3DPMISCCAPS_POSTBLENDSRGBCONVERT) )
		{
			DWORD dwType, dwValue, dwSize;
			TCHAR szValue[ 300 ];

			//Now we need to create ini file which will tell REF to turn off this cap
			FILE *ini_file = NULL;
			TCHAR szPath[ MAX_PATH ];
			szPath[ 0 ] = '\0';
			GetExpandedPath(_T("%USERPROFILE%\\SuckySRGBSupport.ini"), szPath );
			ini_file = fopen(szPath, "w");
			fprintf( ini_file, "[RefCapDisable]\n" );
			fprintf( ini_file, "PrimitiveMiscCaps=D3DPMISCCAPS_POSTBLENDSRGBCONVERT");
			fclose(ini_file);

			//set REG_SZ regkey SOFTWARE\Microsoft\Direct3D\ReferenceDevice\CapsFile to SuckySRGBSupport.ini
			if (SUCCEEDED ( RegCreateKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Direct3D\\ReferenceDevice"),
											0, NULL, 0,
											KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY,
											NULL, &hRegKey, NULL) ) )
			{
				//check if CapsFile key is already set
				LONG lResult = RegQueryValueEx(hRegKey, _T("CapsFile"), 0, &dwType, (BYTE *)&szValue, &dwSize);
	
				if(ERROR_SUCCESS != lResult)
				{
					// CapsFile key was not set -> we must create one.
					m_bCapsFileExisted = false;

					dwSize = (_tcsclen(szPath) + 1) * sizeof(TCHAR);
					if ( FAILED ( RegSetValueEx(hRegKey, 
												_T("CapsFile"), 
												0, 
												REG_SZ,
												(BYTE *) szPath,
												dwSize ) ))
					{
						WriteToLog(_T("GammaCorrect: Unable to set ref rast reg key CapsFile.\n***ABORT THIS TEST RUN***\n"));
						RegCloseKey(hRegKey);
						return false;
					}
				}
				else
				{
					// CapsFile key was set -> save previous value and change it if nessessary
					m_bCapsFileExisted = true;
					_tcscpy(m_szCapsFileValue, szValue);
					
					dwSize = (_tcsclen(szPath) + 1) * sizeof(TCHAR);
					if ( FAILED ( RegSetValueEx(hRegKey, 
												_T("CapsFile"), 
												0, 
												REG_SZ,
												(BYTE *) szPath,
												dwSize ) ))
					{
						WriteToLog(_T("GammaCorrect: Unable to set ref rast reg key CapsFile.\n***ABORT THIS TEST RUN***\n"));
						RegCloseKey(hRegKey);
						return false;
					}					
				}
				
				RegCloseKey(hRegKey);
			}
			else
			{
				WriteToLog(_T("GammaCorrect: Unable to open LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Direct3D\\ReferenceDevice reg keys.\n***ABORT THIS TEST RUN***\n"));
				return false;
			}
		}
	}

	return CD3DTest::PreModeSetup();
}

//-----------------------------------------------------------------------------

bool GammaCorrect::Setup()
{
	SetDefaultMatrices();

	SetDefaultRenderStates();

	switch (m_RType)
	{
		case RTYPE_TEXTURE:
		{
			if ( !GenerateTextures() || !GenerateVertexBuffer() )
				return false;
			break;
		}
		case RTYPE_CUBETEXTURE:
		{	
			if ( !GenerateCubeTextures() || !GenerateCubeVertexBuffer() )
				return false;
			break;
		}
		case RTYPE_VOLUMETEXTURE:
		{	
			if ( !GenerateVolTextures()  || !GenerateVolVertexBuffer() )
				return false;
			break;
		}
	} 

	return true;
}

//-----------------------------------------------------------------------------

void GammaCorrect::Cleanup()
{
	RELEASE(m_pVertexBuffer);
	RELEASE(m_pTexture1);
	RELEASE(m_pTexture2);

	//this is only for blending cases for dx9.L
	if ( m_isLDDMDriver  && !KeySet("noregkey"))
	{
		if ( !(m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps & D3DPMISCCAPS_POSTBLENDSRGBCONVERT) )
		{
            HKEY hRegKey;
			DWORD dwType, dwValue, dwSize;
			TCHAR szValue[ 300 ];

			//restore CapsFile value
			if (SUCCEEDED ( RegCreateKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Direct3D\\ReferenceDevice"),
											0, NULL, 0,
											KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY,
											NULL, &hRegKey, NULL) ) )										// handle to open key
			{
				if ( !m_bCapsFileExisted )
				{
					//if CapsFile reg key didn't exist -> remove it
					if ( FAILED( RegDeleteValue(hRegKey, _T("CapsFile") )))
					{
						WriteToLog(_T("GammaCorrect: Unable to delete CapsFile reg key\n"));
					}
				}
				else
				{
					//restore original CapsFile value
					szValue[ 0 ] = '\0';
					_tcscpy(szValue, m_szCapsFileValue);
					dwSize = (_tcsclen(szValue) + 1) * sizeof(TCHAR);

					if ( FAILED ( RegSetValueEx(hRegKey, _T("CapsFile"), 0, REG_SZ, (BYTE *) szValue, dwSize ) ))
					{
						WriteToLog(_T("GammaCorrect: Unable to restore value of CapsFile reg key.\n"));
					}		
				}
			
				RegCloseKey(hRegKey);
			}
			else
			{
				WriteToLog(_T("GammaCorrect: Unable to open LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Direct3D\\ReferenceDevice reg keys.\n"));
			}
		}
	}
}

//-----------------------------------------------------------------------------

UINT GammaCorrect::TestInitialize(void)
{
	HRESULT hr;

	m_nTextureOp  = sizeof(g_TextureOp) / sizeof(DWORD);
	m_curTextureOp = -1;

	switch (m_RType)
	{
        case RTYPE_TEXTURE:
            if (m_uLevels == 1 || m_pCurrentMode->nTextureFormat % (m_nTextureOp+1) == 0)
				m_curTextureOp = -1;
			else
			{
				m_curTextureOp = m_pCurrentMode->nTextureFormat % (m_nTextureOp+1)-1;
				m_nTextureOp = 1;
			}
			break;
		
		case RTYPE_CUBETEXTURE:
			if (m_uLevels == 1 || m_pCurrentMode->nCubeFormat % (m_nTextureOp+1) == 0)
				m_curTextureOp = -1;
			else
			{
				m_curTextureOp = m_pCurrentMode->nCubeFormat % (m_nTextureOp+1)-1;
				m_nTextureOp = 1;
			}
			break;
		
		case RTYPE_VOLUMETEXTURE:
			if (m_uLevels == 1 || m_pCurrentMode->nVolumeFormat % (m_nTextureOp+1) == 0)
				m_curTextureOp = -1;
			else
			{
				m_curTextureOp = m_pCurrentMode->nVolumeFormat % (m_nTextureOp+1)-1;
				m_nTextureOp = 1;
			}
			break;
	} 
    

	if ( (FMT)m_Format.d3dfFormat == FMT_A8 )
	{
		m_curTextureOp = 0;
		m_nTextureOp = 1;
	}	

#ifdef FAST_TEST
    SetTestRange(1, m_uLevels);
#else
    SetTestRange(1, m_uLevels*m_nTextureOp);
#endif
	
	return D3DTESTINIT_RUN;
}

#ifdef TEST_FAST
bool GammaCorrect::ExecuteTest(UINT iTest)
{
    m_LevelIndex = (iTest - 1) % m_uLevels;

    UINT numTextureOpsToTest = m_nTextureOp;
    if (m_LevelIndex == 0 || iTest == m_pRangeList->uStart || (m_pConstraints != NULL && iTest == m_pConstraints->uStart))
    {
        m_LevelIndex = (iTest - 1) % m_uLevels;

        if (m_curTextureOp != -1)
            numTextureOpsToTest = 1;

    }

    BeginTestCase(_T(msgString), iTest);

    BeginScene();

    UINT offset = 0;

    SetSamplerState(0, SAMP_MAXMIPLEVEL, m_LevelIndex);
    SetSamplerState(1, SAMP_MAXMIPLEVEL, m_LevelIndex);
    const DWORD width = m_Options.D3DOptions.nBackBufferWidth;
    const DWORD height = m_Options.D3DOptions.nBackBufferHeight;
    const DWORD numRows = 2;
    const DWORD numColumns = (numTextureOpsToTest - 1) / numRows + 1;
    const DWORD widthPerOp = width / numColumns;
    const DWORD heightPerOp = height / numRows;

    if (m_curTextureOp == -1)
    {
        m_curTextureOp = 0;
    }

    UINT startOffset = offset;
    for (; m_curTextureOp < numTextureOpsToTest; m_curTextureOp++)
    {
        offset = startOffset;
        SetTextureStageState((DWORD)1, D3DTSS_COLOROP, (DWORD)g_TextureOp[m_curTextureOp]);
        //this is only for blending cases for dx9.L
        if (m_isLDDMDriver)
        {
            if (m_curTextureOp % 2 && !KeySet("noblend"))
                SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
            else
                SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
        }

        VIEWPORT viewport;
        viewport.X = (m_curTextureOp % numColumns) * widthPerOp;
        viewport.Y = (m_curTextureOp / numColumns) * heightPerOp;
        viewport.Width = widthPerOp;
        viewport.Height = heightPerOp;
        viewport.MinZ = 0.0;
        viewport.MaxZ = 1.0;

        SetViewport(&viewport);

        switch (m_RType)
        {
        case RTYPE_TEXTURE:
        {
            DrawPrimitive(D3DPT_TRIANGLESTRIP, offset, 2);
            break;
        }
        case RTYPE_CUBETEXTURE:
        {
            for (int i = 0; i < 6; i++)
            {
                DrawPrimitive(D3DPT_TRIANGLESTRIP, offset, 2);
                offset += 4;
            }
            break;
        }
        case RTYPE_VOLUMETEXTURE:
        {
            DWORD Depth = max(1, 16 / (1 << m_LevelIndex));
            for (int j = 0; j < m_LevelIndex && 16 / (1 << j) > 1; j++)
                offset += 4 * max(1, 16 / (1 << j));
            for (int i = 0; i < Depth; i++)
            {
                DrawPrimitive(D3DPT_TRIANGLESTRIP, offset, 2);
                offset += 4;
            }
            break;
        }
        }
    }

    EndScene();

    return true;
}
#else
//-----------------------------------------------------------------------------
bool GammaCorrect::ExecuteTest(UINT iTest)
{
    m_LevelIndex = (iTest - 1) % m_uLevels;

    UINT numTextureOpsToTest = m_nTextureOp;
    if (m_LevelIndex == 0 || iTest == m_pRangeList->uStart || (m_pConstraints != NULL && iTest == m_pConstraints->uStart))
    {
        m_LevelIndex = (iTest - 1) % m_uLevels;

        if (m_curTextureOp == -1)
            m_curTextureOp = (iTest - 1) / m_uLevels;
        SetTextureStageState((DWORD)1, D3DTSS_COLOROP, (DWORD)g_TextureOp[m_curTextureOp++]);

        //this is only for blending cases for dx9.L
        if (m_isLDDMDriver)
        {
            if (m_curTextureOp % 2 && !KeySet("noblend"))
                SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
            else
                SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
        }
    }

    msgString[0] = '\0';
    //this is only for blending cases for dx9.L
    if (m_isLDDMDriver)
        sprintf(msgString, "%s %d Level, %s%s", msgString, m_LevelIndex, (m_curTextureOp % 2 && !KeySet("noblend")) ? "SRCCOLOR_INVSRCCOLOR blend, " : "", g_TextureOpNames[m_curTextureOp - 1]);
    else
        sprintf(msgString, "%s %d Level, %s", msgString, m_LevelIndex, g_TextureOpNames[m_curTextureOp - 1]);

    BeginTestCase(_T(msgString), iTest);

    BeginScene();

    UINT offset = 0;

    SetSamplerState(0, SAMP_MAXMIPLEVEL, m_LevelIndex);
    SetSamplerState(1, SAMP_MAXMIPLEVEL, m_LevelIndex);
    switch (m_RType)
    {
    case RTYPE_TEXTURE:
    {
        DrawPrimitive(D3DPT_TRIANGLESTRIP, offset, 2);
        break;
    }
    case RTYPE_CUBETEXTURE:
    {
        for (int i = 0; i < 6; i++)
        {
            DrawPrimitive(D3DPT_TRIANGLESTRIP, offset, 2);
            offset += 4;
        }
        break;
    }
    case RTYPE_VOLUMETEXTURE:
    {
        DWORD Depth = max(1, 16 / (1 << m_LevelIndex));
        for (int j = 0; j < m_LevelIndex && 16 / (1 << j) > 1; j++)
            offset += 4 * max(1, 16 / (1 << j));
        for (int i = 0; i < Depth; i++)
        {
            DrawPrimitive(D3DPT_TRIANGLESTRIP, offset, 2);
            offset += 4;
        }
        break;
    }
    }

    EndScene();

    return true;
}
#endif

//-----------------------------------------------------------------------------

void GammaCorrect::CommandLineHelp(void)
{
	//these command lines are only valid for test groups using D3DUSAGE_SRGBREAD
	if (m_szCommandKey[0] == 'r' || m_szCommandKey[0] == 'R')
	{
		WriteCommandLineHelp(_T("$ySrcRead  \t$gUse D3DUSAGE_SRGBREAD textures only on src device."));
		WriteCommandLineHelp(_T("$yRefRead \t$gUse D3DUSAGE_SRGBREAD textures only on ref device."));
		WriteCommandLineHelp(_T("$yNotRead \t$gDo not use D3DUSAGE_SRGBREAD textures."));
	}

	//these command lines are only valid for test groups using D3DUSAGE_SRGBWRITE
	if (m_szCommandKey[0] == 'w' || m_szCommandKey[0] == 'W' ||
		m_szCommandKey[1] == 'w' || m_szCommandKey[1] == 'W'  )
	{
		WriteCommandLineHelp(_T("$ySrcWrite \t$gUse D3DUSAGE_SRGBWRITE render-targets only on src device."));
		WriteCommandLineHelp(_T("$yRefWrite\t$gUse D3DUSAGE_SRGBWRITE render-targets only on ref device."));
		WriteCommandLineHelp(_T("$yNotWrite\t$gDo not use D3DUSAGE_SRGBWRITE render-targets.\n"));
	}
	
	WriteCommandLineHelp(_T("$ynoMipMap\t$gDo not use mipmapping."));
}

//-----------------------------------------------------------------------------

bool GammaCorrect::ClearFrame(void)
{
	DPF(4, _T("GammaCorrect::ClearFrame\n"));

	if ( (FMT)m_Format.d3dfFormat == FMT_A8 )
		return Clear(0, NULL, FMWK_DEFAULT, 0xffffffff, 1.f, 0);
	else
		return Clear(0, NULL, FMWK_DEFAULT, 0x0000007f, 1.f, 0);
}

#ifdef FAST_TEST
bool GammaCorrect::AddFormatModes(DISPLAYMODE *pMode, bool bWindowed)
{
    UINT uTexture = 0;
    UINT uZBuffer = 0;
    UINT uCube = 0;
    UINT uVolume = 0;
    UINT uMS = 0;
    UINT uBB = 0;

    // Convert the formats into indexes into the common lists
    if (!GetFormatIndex(m_Options.ModeOptions.TextureFmt, m_pCommonTextureFormats, &uTexture) ||
        !GetZBufferFormatIndex(m_Options.ModeOptions.ZBufferFmt, m_pCommonZBufferFormats, &(pMode->Format), &uZBuffer) ||
        !GetFormatIndex(m_Options.ModeOptions.CubeFmt, m_pCommonCubeFormats, &uCube) ||
        !GetFormatIndex(m_Options.ModeOptions.VolumeFmt, m_pCommonVolumeFormats, &uVolume) ||
        !GetFormatIndex(m_Options.ModeOptions.MultisampType, m_pCommonMultiSampleFormats, m_uCommonMultiSampleFormats, &uMS) ||
        !GetBackBufferFormatIndex(m_Options.ModeOptions.BackBufferFmt, m_pCommonBackBufferFormats, &(pMode->Format), &uBB) ||
        // We need to validate MS here.  If the device doens't support this MS type, we don't want to add it as a mode
        (m_pCommonMultiSampleFormats && m_pCommonZBufferFormats &&
        (FAILED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, bWindowed, m_pCommonMultiSampleFormats[uMS], NULL)) ||
        FAILED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonZBufferFormats[uZBuffer], bWindowed, m_pCommonMultiSampleFormats[uMS], NULL)))))
    {
        DPF(2, _T("Requested fmt not supported by device.\n"));
        return false;
    }

    // If only one mode was requested, add it and return
    if (m_Options.ModeOptions.TextureFmt != FMT_ALL &&
        m_Options.ModeOptions.ZBufferFmt != FMT_ALL &&
        m_Options.ModeOptions.CubeFmt != FMT_ALL &&
        m_Options.ModeOptions.VolumeFmt != FMT_ALL &&
        m_Options.ModeOptions.BackBufferFmt != FMT_ALL &&
        m_Options.ModeOptions.MultisampType != MULTISAMPLE_ALL)
    {
        return AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB);
    }

    if (m_Options.ModeOptions.BackBufferFmt == FMT_ALL)
    {
        for (UINT nBB = 0; nBB < m_uCommonBackBufferFormats; nBB++)
        {
            // Filter out unsupported BB modes based on ModeOptions
            if (m_pD3D->GetVersion() < 8.f)
            {
                if (pMode->Format.d3dfFormat != m_pCommonBackBufferFormats[nBB].d3dfFormat)
                    continue;
            }
            else if (FAILED(m_pD3D->CheckDeviceType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format,
                m_pCommonBackBufferFormats[nBB], bWindowed)))
                continue;

            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, uVolume, uMS, 0, nBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.TextureFmt == FMT_ALL)
    {
        for (UINT nTexture = 0; nTexture < m_uCommonTextureFormats; nTexture++)
        {
            if (m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_A8R8G8B8 ||
                m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_DXT1)
            {
                if (!AddMode(pMode, bWindowed, nTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
            }
        }
    }

    if (m_Options.ModeOptions.ZBufferFmt == FMT_ALL)
    {
        for (UINT nZBuffer = 0; nZBuffer < m_uCommonZBufferFormats; nZBuffer++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, nZBuffer, uCube, uVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.CubeFmt == FMT_ALL)
    {
        for (UINT nCube = 0; nCube < m_uCommonCubeFormats; nCube++)
        {
            if (m_pCommonTextureFormats[nCube].d3dfFormat == FMT_A8R8G8B8 ||
                m_pCommonTextureFormats[nCube].d3dfFormat == FMT_DXT1)
            {
                if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, nCube, uVolume, uMS, 0, uBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
            }
        }
    }

    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
    {
        for (UINT nVolume = 0; nVolume < m_uCommonVolumeFormats; nVolume++)
        {
            if (m_pCommonTextureFormats[nVolume].d3dfFormat == FMT_A8R8G8B8 ||
                m_pCommonTextureFormats[nVolume].d3dfFormat == FMT_DXT1)
            {
                if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, nVolume, uMS, 0, uBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
            }
        }
    }

    if (m_Options.ModeOptions.MultisampType == FMT_ALL)
    {
        DWORD dwMSQuality = 0;

        for (UINT nMS = 0; nMS < m_uCommonMultiSampleFormats; nMS++)
        {
            int nValidBB = -1, nValidZBuffer = -1;

            // For each MS type, find a BB and ZBuffer that support that type
            for (UINT nBB = 0; nBB < m_uCommonBackBufferFormats; nBB++)
                for (UINT nZBuffer = 0; nZBuffer < m_uCommonZBufferFormats; nZBuffer++)
                {
                    // If specific modes are requested, only allow those modes to be matched.
                    if (m_Options.ModeOptions.BackBufferFmt != FMT_ALL && m_Options.ModeOptions.BackBufferFmt != FMT_ANY &&
                        nBB != uBB)
                        continue;
                    if (m_Options.ModeOptions.ZBufferFmt != FMT_ALL && m_Options.ModeOptions.ZBufferFmt != FMT_ANY &&
                        nZBuffer != uZBuffer)
                        continue;

                    // Filter out unsupported BB modes based on ModeOptions
                    if (SUCCEEDED(m_pD3D->CheckDeviceType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, m_pCommonBackBufferFormats[nBB], bWindowed)) &&
                        SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonBackBufferFormats[nBB], bWindowed, m_pCommonMultiSampleFormats[nMS], &dwMSQuality)) &&
                        SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonZBufferFormats[nZBuffer], bWindowed, m_pCommonMultiSampleFormats[nMS], &dwMSQuality)))
                    {
                        nValidBB = nBB;
                        nValidZBuffer = nZBuffer;
                        break;
                    }
                }

            if (nValidBB == -1 || nValidZBuffer == -1)
                continue;

            for (UINT nMSQ = 0; nMSQ < dwMSQuality; nMSQ++)
            {
                if (!AddMode(pMode, bWindowed, uTexture, nValidZBuffer, uCube, uVolume, nMS, nMSQ, nValidBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
            }
        }
    }

    return true;
}
#endif



