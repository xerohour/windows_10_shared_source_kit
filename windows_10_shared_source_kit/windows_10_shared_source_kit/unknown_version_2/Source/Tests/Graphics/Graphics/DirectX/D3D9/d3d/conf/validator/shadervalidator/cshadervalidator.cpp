#include "CShaderValidator.h"


BOOL CALLBACK EnumResNameProc( HMODULE, LPCTSTR, LPTSTR, LONG_PTR );

//-------------------------------------------------------------------------------------------------------------------------

CShaderValidator::CShaderValidator()
{
    m_pIncludeHandler_File     = NULL;
    m_pIncludeHandler_Resource = NULL;

	SetTestType( TESTTYPE_CONF );
    m_szTestName = _T("ShaderValidator");
	m_szCommandKey = _T("ShaderValidator");

	if( KeySet( _T( "alldevices" ) ) )
	{
		m_Options.D3DOptions.dwDevTypes    = DEVICETYPE_HAL | DEVICETYPE_REF;
		m_Options.D3DOptions.dwBehaviors   = DEVICECREATE_ALL;
		m_szLogName    = "ShaderValidator_AllDevices";
	}
		
    m_Options.D3DOptions.fMinDXVersion = 9.0f;
    //m_Options.D3DOptions.fMaxDXVersion = 9.0f;    
    m_Options.D3DOptions.bReference    = false;

    if ( KeySet( _T("DeleteLog") ) )
    {
        if ( KeySet( _T("LogDriveC") ) )  { DeleteFile( _T("C:\\Windows\\DxLogs\\ShaderValidator.log") ); }
        else                              { DeleteFile( _T("D:\\Windows\\DxLogs\\ShaderValidator.log") ); }
    }
}

//-------------------------------------------------------------------------------------------------------------------------

UINT CShaderValidator::TestInitialize()
{
	/*TCHAR pBuffer[80];
	
	while(ReadString(_T("file"), pBuffer, ARRAYSIZE(pBuffer), true))
	{
		SetIndices(pBuffer);
	}*/

	if ( !KeySet("debug_unmute") )
		D3DXDebugMute( true );

    m_nTestTemplateCount   = 0;
    m_nCurrentTestTemplate = 0;

    if ( !SetTestRange( 1, GetTestCaseCount() ) )
    {
        WriteToLog( 1, _T("TestInitialize() -- SetTestRange() failed\n") );
        return D3DTESTINIT_ABORT;
    }

    if ( NULL == ( m_pDev = GetDevice9() ) )
    {
        WriteToLog( 1, _T("TestInitialize() -- GetDevice9() == NULL\n") );
        return D3DTESTINIT_ABORT;
    }

    D3DDEVICE_CREATION_PARAMETERS m_CreationParams;

    if ( FAILED( m_pDev->GetCreationParameters( &m_CreationParams ) ) )
    {
        WriteToLog( 1, _T("TestInitialize() -- m_pDev->GetCreationParameters() failed\n") );
        return D3DTESTINIT_ABORT;
    }

    m_bIsREF  = ( m_CreationParams.DeviceType == D3DDEVTYPE_REF );
    m_bIsSWVP = m_pDev->GetSoftwareVertexProcessing();
    m_bIsMixedVP = ( m_pSrcDevice->GetBehaviors() & DEVICECREATE_MIXED );

    // If the device is created as HAL MIXED HWVP, temporarily set to SWVP mode so that the following
    // GetDeviceCaps call gets the caps associated with SWVP.  This is needed for the test to work.
    if ( !m_bIsREF && m_bIsMixedVP && !m_bIsSWVP )
    {
        m_pDev->SetSoftwareVertexProcessing( TRUE );
    }

    if ( FAILED( m_pDev->GetDeviceCaps( &m_Caps ) ) )
    {
        WriteToLog( 1, _T("TestInitialize() -- m_pDev->GetDeviceCaps() failed\n") );
        return D3DTESTINIT_ABORT;
    }

    // If the device is created as HAL MIXED HWVP, set back to HWVP mode so that we can test if the runtime
    // validator behaves correctly if device is in HWVP mode.
    if ( !m_bIsREF && m_bIsMixedVP && !m_bIsSWVP )
    {
        m_pDev->SetSoftwareVertexProcessing( FALSE );
    }

	return D3DTESTINIT_RUN;
}

//-------------------------------------------------------------------------------------------------------------------------

bool CShaderValidator::ExecuteTest( UINT )
{
    if ( !GetNextTestCase() )
    {
        WriteToLog( 1, _T("ExecuteTest() -- GetNextTestCase() failed\n") );
        Fail();
        return false;
    }

    TCHAR szTestInfo[ 500 ];

	_stprintf( szTestInfo, _T("%s : #%03d"), m_pszTestTemplateNameList[ m_nCurrentTestTemplate ], m_uCurrentTestCase );
	_stprintf( szTestInfo, _T("%s : %s"), szTestInfo, m_szTestCaseDesc_Desc );
    BeginTestCase( szTestInfo, m_uTestNumber );

    //m_pD3D->SetDebugMute( true );
	HRESULT hr_Debug = D3DXAssembleShader9( m_pShader, strlen( m_pShader ), NULL, NULL, D3DXSHADER_DEBUG, &m_pShaderBuf, &m_pErrorBuf_Shader );
	
	RELEASE( m_pShaderBuf );
    RELEASE( m_pErrorBuf_Shader );

	HRESULT hr = D3DXAssembleShader9( m_pShader, strlen( m_pShader ), NULL, NULL, 0, &m_pShaderBuf, &m_pErrorBuf_Shader );
    BOOL    bPassCreation = TRUE;

	
    if ( m_bVSTest )
    {
        if( m_VS_Criteria.fMinDXVersion != 0 && m_pD3D->GetRuntimeVersion( ) < m_VS_Criteria.fMinDXVersion )
		{
			WriteToLog( 1, _T( "ExecuteTest() -- Test case should only run on DX version %f and above\n"), m_VS_Criteria.fMinDXVersion );
			Skip( );
			return false;
		}

		if ( m_VS_Criteria.bPassAssembler )
        {
            if ( FAILED( hr ) )
            {
                WriteToLog( 1, _T("ExecuteTest() -- D3DXAssembleShader9() should succeed but failed\n%s"), (CHAR*)(m_pErrorBuf_Shader->GetBufferPointer()) );
                Fail();
                return false;
            }
            
            if ( FAILED( hr_Debug ) )
            {
                WriteToLog( 1, _T("ExecuteTest() -- D3DXAssembleShader9( DEBUG ) should succeed but failed\n") );
                Fail();
                return false;
            }
        }
        else if ( !(m_VS_Criteria.bPassAssembler) )
        {
            if ( SUCCEEDED( hr ) )
            {
                WriteToLog( 1, _T("ExecuteTest() -- D3DXAssembleShader9() should fail but succeeded\n") );
                Fail();
                return false;
            }

            if ( SUCCEEDED( hr_Debug ) )
            {
                WriteToLog( 1, _T("ExecuteTest() -- D3DXAssembleShader9( DEBUG ) should fail but succeeded\n") );
                Fail();
                return false;
            }
        }

        if ( m_VS_Criteria.bPassAssembler && SUCCEEDED( hr ) )
        {
            LPDIRECT3DVERTEXSHADER9 pVS = NULL;

            hr = m_pDev->CreateVertexShader( (DWORD *)(m_pShaderBuf->GetBufferPointer()), &pVS );

            RELEASE( pVS );

            if ( m_VS_Criteria.nVertexShaderVersion > (INT)(m_Caps.VertexShaderVersion & 0xffff) )
            { bPassCreation = FALSE; }
			
            if ( !m_bIsREF && !m_bIsSWVP && !m_bIsMixedVP )
            {
                if ( m_VS_Criteria.bRequireSWVP )
                { bPassCreation = FALSE; }

                if ( m_VS_Criteria.nMaxVertexShaderConst != -1 &&
                     m_VS_Criteria.nMaxVertexShaderConst > (INT)m_Caps.MaxVertexShaderConst )
                { bPassCreation = FALSE; }

                if ( m_VS_Criteria.nDynamicFlowControlDepth != -1 &&
                     m_VS_Criteria.nDynamicFlowControlDepth > (INT)m_Caps.VS20Caps.DynamicFlowControlDepth )
                { bPassCreation = FALSE; }

                if ( m_VS_Criteria.nNumTemps != -1 &&
                     m_VS_Criteria.nNumTemps > (INT)m_Caps.VS20Caps.NumTemps )
                { bPassCreation = FALSE; }

                if ( m_VS_Criteria.nStaticFlowControlDepth != -1 &&
                     m_VS_Criteria.nStaticFlowControlDepth > (INT)m_Caps.VS20Caps.StaticFlowControlDepth )
                { bPassCreation = FALSE; }

                if ( m_VS_Criteria.nMaxVertexShader30InstructionSlots != -1 &&
                     m_VS_Criteria.nMaxVertexShader30InstructionSlots > (INT)m_Caps.MaxVertexShader30InstructionSlots )
                { bPassCreation = FALSE; }

                if (  ( m_VS_Criteria.nCaps & CAPS_PREDICATION ) &&
                     !( m_Caps.VS20Caps.Caps & D3DVS20CAPS_PREDICATION ) )
                { bPassCreation = FALSE; }
            }

            if ( bPassCreation && FAILED( hr ) )
            {
                WriteToLog( 1, _T("ExecuteTest() -- m_pDev->CreateVertexShader9() should succeed but failed\n") );
                Fail();
                return false;
            }
            else if ( !bPassCreation && SUCCEEDED( hr ) )
            {
                WriteToLog( 1, _T("ExecuteTest() -- m_pDev->CreateVertexShader9() should fail but succeeded\n") );
                Fail();
                return false;
            }
        }
    }
    else
    {
	  	if( m_PS_Criteria.fMinDXVersion != 0 && m_pD3D->GetRuntimeVersion( ) < m_PS_Criteria.fMinDXVersion )
		{
			WriteToLog( 1, _T( "ExecuteTest() -- Test case should only run on DX version %f and above\n"), m_PS_Criteria.fMinDXVersion );
			Skip( );
			return false;
		}

		if ( m_PS_Criteria.bPassAssembler )
        {
            if ( FAILED( hr ) )
            {
                WriteToLog( 1, _T("ExecuteTest() -- D3DXAssembleShader9() should succeed but failed\n%s"), (CHAR*)(m_pErrorBuf_Shader->GetBufferPointer()) );
                Fail();
                return false;
            }

            if ( FAILED( hr_Debug ) )
            {
                WriteToLog( 1, _T("ExecuteTest() -- D3DXAssembleShader9( DEBUG ) should succeed but failed\n") );
                Fail();
                return false;
            }
        }
        else if ( !(m_PS_Criteria.bPassAssembler) )
        {
            if ( SUCCEEDED( hr ) )
            {
                WriteToLog( 1, _T("ExecuteTest() -- D3DXAssembleShader9() should fail but succeeded\n") );
                Fail();
                return false;
            }

            if ( SUCCEEDED( hr_Debug ) )
            {
                WriteToLog( 1, _T("ExecuteTest() -- D3DXAssembleShader9( DEBUG ) should fail but succeeded\n") );
                Fail();
                return false;
            }
        }

        if ( m_PS_Criteria.bPassAssembler && SUCCEEDED( hr ) )
        {
            LPDIRECT3DPIXELSHADER9 pPS = NULL;

            hr = m_pDev->CreatePixelShader( (DWORD *)(m_pShaderBuf->GetBufferPointer()), &pPS );

            RELEASE( pPS );

            if ( m_PS_Criteria.nPixelShaderVersion > (INT)(m_Caps.PixelShaderVersion & 0xffff) )
            { bPassCreation = FALSE; }
			
			if ( !m_bIsREF )
            {
                if ( m_PS_Criteria.bRequireREF )
                { bPassCreation = FALSE; }

                if ( m_PS_Criteria.nDynamicFlowControlDepth != -1 &&
                     m_PS_Criteria.nDynamicFlowControlDepth > (INT)m_Caps.PS20Caps.DynamicFlowControlDepth )
                { bPassCreation = FALSE; }

                if ( m_PS_Criteria.nNumTemps != -1 &&
                     m_PS_Criteria.nNumTemps > (INT)m_Caps.PS20Caps.NumTemps )
                { bPassCreation = FALSE; }

                if ( m_PS_Criteria.nStaticFlowControlDepth != -1 &&
                     m_PS_Criteria.nStaticFlowControlDepth > (INT)m_Caps.PS20Caps.StaticFlowControlDepth )
                { bPassCreation = FALSE; }

                if ( m_PS_Criteria.nNumInstructionSlots != -1 &&
                     m_PS_Criteria.nNumInstructionSlots > (INT)m_Caps.PS20Caps.NumInstructionSlots )
                { bPassCreation = FALSE; }

                if ( m_PS_Criteria.nMaxPixelShader30InstructionSlots != -1 &&
                     m_PS_Criteria.nMaxPixelShader30InstructionSlots > (INT)m_Caps.MaxPixelShader30InstructionSlots )
                { bPassCreation = FALSE; }

                if (  ( m_PS_Criteria.nCaps & CAPS_ARBITRARYSWIZZLE ) &&
                     !( m_Caps.PS20Caps.Caps & D3DPS20CAPS_ARBITRARYSWIZZLE ) )
                { bPassCreation = FALSE; }

                if (  ( m_PS_Criteria.nCaps & CAPS_GRADIENTINSTRUCTIONS ) &&
                     !( m_Caps.PS20Caps.Caps & D3DPS20CAPS_GRADIENTINSTRUCTIONS ) )
                { bPassCreation = FALSE; }

                if (  ( m_PS_Criteria.nCaps & CAPS_PREDICATION ) &&
                     !( m_Caps.PS20Caps.Caps & D3DPS20CAPS_PREDICATION ) )
                { bPassCreation = FALSE; }

                if (  ( m_PS_Criteria.nCaps & CAPS_NODEPENDENTREADLIMIT ) &&
                     !( m_Caps.PS20Caps.Caps & D3DPS20CAPS_NODEPENDENTREADLIMIT ) )
                { bPassCreation = FALSE; }

                if (  ( m_PS_Criteria.nCaps & CAPS_NOTEXINSTRUCTIONLIMIT ) &&
                     !( m_Caps.PS20Caps.Caps & D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT ) )
                { bPassCreation = FALSE; }
            }

            if ( bPassCreation && FAILED( hr ) )
            {
                WriteToLog( 1, _T("ExecuteTest() -- m_pDev->CreatePixelShader9() should succeed but failed\n") );
                Fail();
                return false;
            }
            else if ( !bPassCreation && SUCCEEDED( hr ) )
            {
                WriteToLog( 1, _T("ExecuteTest() -- m_pDev->CreatePixelShader9() should fail but succeeded\n") );
                Fail();
                return false;
            }
        }
    }

    Pass();

    return false;
}

//-------------------------------------------------------------------------------------------------------------------------

void CShaderValidator::UpdateStatus()
{
    TCHAR szStatus[ 256 ];

    _stprintf( szStatus, _T("  $pPass: $g%d    $pFail: $r%d"), GetPass(), GetFail() );
    WriteStatus( "$wResults", szStatus );

    _stprintf( szStatus, _T("  $c%s"), m_pTestTemplateDesc );
    WriteStatus( "$wTempl.", szStatus );

    _stprintf( szStatus, _T("  $r%03d"), m_uCurrentTestCase );
    WriteStatus( "$wTest #", szStatus );

    _stprintf( szStatus, _T("  $y%s%s%s"), m_szTestCaseDesc_ShaderVersion,
                                           m_szTestCaseDesc_Desc[ 0 ] == '\0' ? "\0" : " : $g",
                                           m_szTestCaseDesc_Desc[ 0 ] == '\0' ? "\0" : m_szTestCaseDesc_Desc );
    WriteStatus( "$wDesc.", szStatus );

    RELEASE( m_pShaderBuf );
    RELEASE( m_pErrorBuf_Shader );
}

//-------------------------------------------------------------------------------------------------------------------------

bool CShaderValidator::TestTerminate()
{
    RELEASE( m_pEffect );
    RELEASE( m_pShaderBuf );
    RELEASE( m_pErrorBuf_Effect );
    RELEASE( m_pErrorBuf_Shader );

    SAFEDELETE( m_pIncludeHandler_File );
    SAFEDELETE( m_pIncludeHandler_Resource );

    return true;
}

//-------------------------------------------------------------------------------------------------------------------------

BOOL CShaderValidator::GetNextTestCase()
{
    if ( m_uTestNumber > m_puTestTemplateAccumulatedSizeList[ m_nCurrentTestTemplate ] || m_uTestNumber == 1 || (m_pConstraints && m_pConstraints->uStart == m_uTestNumber) )
    {
        if ( m_pConstraints )
		{
			if ( m_uTestNumber > m_pConstraints->uStart )
			{
				++m_nCurrentTestTemplate;
			}
		}
		else if ( m_uTestNumber > 1 )
        {
            ++m_nCurrentTestTemplate;
        }
		

        RELEASE( m_pEffect );
        RELEASE( m_pErrorBuf_Effect );

        if ( m_bLoadFromFile )
        {
            SAFEDELETE( m_pIncludeHandler_File );

            m_pIncludeHandler_File = new CIncludeFromFile;

            if ( FAILED( D3DXCreateEffectFromFile( m_pDev, m_pszTestTemplateNameList[ 0 ], NULL, m_pIncludeHandler_File, 0, NULL, &m_pEffect, &m_pErrorBuf_Effect ) ) )
            {
                WriteToLog( 1, _T("    D3DXCreateEffectFromFile( %s ) failed:\n        %s\n"),
                               m_pszTestTemplateNameList[ 0 ],
                               (CHAR *)(m_pErrorBuf_Effect->GetBufferPointer()) );
                return FALSE;
            }
        }
        else
        {
            SAFEDELETE( m_pIncludeHandler_Resource );

            m_pIncludeHandler_Resource = new CIncludeFromResource;

            if ( FAILED( D3DXCreateEffectFromResource( m_pDev, NULL, m_pszTestTemplateNameList[ m_nCurrentTestTemplate ], NULL, m_pIncludeHandler_Resource, 0, NULL, &m_pEffect, &m_pErrorBuf_Effect ) ) )
            {
                WriteToLog( 1, _T("    D3DXCreateEffectFromResource( %s ) failed:\n        %s\n"),
                               m_pszTestTemplateNameList[ m_nCurrentTestTemplate ],
                               (CHAR *)(m_pErrorBuf_Effect->GetBufferPointer()) );
                return FALSE;
            }
        }

        D3DXHANDLE hDummy, hDummy_1;

        if ( NULL == ( hDummy = m_pEffect->GetParameterByName( NULL, "TestInfo" ) ) )
        {
            WriteToLog( 1, _T("    m_pEffect->GetParameterByName( \"TestInfo\" ) failed\n") );
            return FALSE;
        }

        if ( FAILED( m_pEffect->GetString( hDummy, &m_pTestTemplateDesc ) ) )
        {
            WriteToLog( 1, _T("    m_pEffect->GetString( \"TestInfo\" ) failed\n") );
            return FALSE;
        }

        if ( NULL == ( hDummy_1 = m_pEffect->GetAnnotationByName( hDummy, "TestType" ) ) )
        {
            WriteToLog( 1, _T("    m_pEffect->GetAnnotationByName( \"TestInfo <TestType>\" ) failed\n") );
            return FALSE;
        }

        LPCSTR pTestType;

        if ( FAILED( m_pEffect->GetString( hDummy_1, &pTestType ) ) )
        {
            WriteToLog( 1, _T("    m_pEffect->GetString( \"TestInfo <TestType>\" ) failed\n") );
            return FALSE;
        }

        m_bVSTest = strcmp( pTestType, "PS" );
    }

    if ( m_bVSTest )
    {
        if ( !GetTestCase_VS() )
        {
            WriteToLog( 1, _T("    GetTestCase_VS() failed\n") );
            return FALSE;
        }
    }
    else
    {
        if ( !GetTestCase_PS() )
        {
            WriteToLog( 1, _T("    GetTestCase_PS() failed\n") );
            return FALSE;
        }
    }

    return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------------

BOOL CShaderValidator::GetTestCase_VS()
{
    CHAR       szInfo[ 256 ];
    D3DXHANDLE hDummy, hDummy_1;

    m_uCurrentTestCase = m_uTestNumber - ( m_nCurrentTestTemplate == 0 ? 0 : m_puTestTemplateAccumulatedSizeList[ m_nCurrentTestTemplate - 1 ] );

    sprintf( szInfo, "VS_%03d_Desc", m_uCurrentTestCase );

    if ( NULL == ( hDummy = m_pEffect->GetParameterByName( NULL, szInfo ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameterByName( \"%s\" ) failed\n"), szInfo );
        return FALSE;
    }

    LPCSTR pTestCaseDesc;

    if ( FAILED( m_pEffect->GetString( hDummy, &pTestCaseDesc ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetString( \"%s\" ) failed\n"), szInfo );
        return FALSE;
    }

    if ( strlen( pTestCaseDesc ) == 6 )
    {
        strncpy( m_szTestCaseDesc_ShaderVersion, pTestCaseDesc, 6 );
        m_szTestCaseDesc_ShaderVersion[ 6 ] = '\0';
        m_szTestCaseDesc_Desc[ 0 ] = '\0';
    }
    else if ( strlen( pTestCaseDesc ) == 7 )
    {
        strncpy( m_szTestCaseDesc_ShaderVersion, pTestCaseDesc, 7 );
        m_szTestCaseDesc_ShaderVersion[ 7 ] = '\0';
        m_szTestCaseDesc_Desc[ 0 ] = '\0';
    }
    else
    {
        strncpy( m_szTestCaseDesc_ShaderVersion, pTestCaseDesc, 7 );

        if ( m_szTestCaseDesc_ShaderVersion[ 6 ] == ' ' )
        {
            m_szTestCaseDesc_ShaderVersion[ 6 ] = '\0';

            if ( strlen( pTestCaseDesc ) )
            pTestCaseDesc += 9;
        }
        else
        {
            m_szTestCaseDesc_ShaderVersion[ 7 ] = '\0';
            pTestCaseDesc += 10;
        }

        strcpy( m_szTestCaseDesc_Desc, pTestCaseDesc );
    }

    sprintf( szInfo, "VS_%03d_Criteria", m_uCurrentTestCase );

    if ( NULL == ( hDummy = m_pEffect->GetParameterByName( NULL, szInfo ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameterByName( \"%s\" ) failed\n"), szInfo );
        return FALSE;
    }

    // bPassAssembler

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 0 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : bPassAssembler ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetBool( hDummy_1, &m_VS_Criteria.bPassAssembler ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetBool( \"%s\" : bPassAssembler ) failed\n"), szInfo );
        return FALSE;
    }

    // bRequireSWVP

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 1 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : bRequireSWVP ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetBool( hDummy_1, &m_VS_Criteria.bRequireSWVP ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetBool( \"%s\" : bRequireSWVP ) failed\n"), szInfo );
        return FALSE;
    }

    // nVertexShaderVersion

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 2 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nVertexShaderVersion ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_VS_Criteria.nVertexShaderVersion ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nVertexShaderVersion ) failed\n"), szInfo );
        return FALSE;
    }

    // nDynamicFlowControlDepth

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 3 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nDynamicFlowControlDepth ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_VS_Criteria.nDynamicFlowControlDepth ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nDynamicFlowControlDepth ) failed\n"), szInfo );
        return FALSE;
    }

    // nNumTemps

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 4 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nNumTemps ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_VS_Criteria.nNumTemps ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nNumTemps ) failed\n"), szInfo );
        return FALSE;
    }

    // nStaticFlowControlDepth

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 5 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nStaticFlowControlDepth ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_VS_Criteria.nStaticFlowControlDepth ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nStaticFlowControlDepth ) failed\n"), szInfo );
        return FALSE;
    }

	// nMaxVertexShaderConst

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 6 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nMaxVertexShaderConst ) failed\n"), szInfo );
        return FALSE;
    }

	if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_VS_Criteria.nMaxVertexShaderConst ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nMaxVertexShaderConst ) failed\n"), szInfo );
        return FALSE;
    }

    // nCaps

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 7 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nCaps ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_VS_Criteria.nCaps ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nCaps ) failed\n"), szInfo );
        return FALSE;
    }

    // nMaxVertexShader30InstructionSlots

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 8 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nMaxVertexShader30InstructionSlots failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_VS_Criteria.nMaxVertexShader30InstructionSlots ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nMaxVertexShader30InstructionSlots ) failed\n"), szInfo );
        return FALSE;
    }

	// fMinDXVersion

	if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 9 ) ) )
	{
		WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : fMinDXVersion failed\n"), szInfo );
		return FALSE;
	}

	if ( FAILED( m_pEffect->GetFloat( hDummy_1, &m_VS_Criteria.fMinDXVersion ) ) )
	{
		WriteToLog( 1, _T("    m_pEffect->GetFloat( \"%s\" : fMinDXVersion ) failed\n"), szInfo );
		return FALSE;
	}

    // assembly shader

    sprintf( szInfo, "VS_%03d", m_uCurrentTestCase );

    if ( NULL == ( hDummy = m_pEffect->GetParameterByName( NULL, szInfo ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameterByName( \"%s\" ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetString( hDummy, &m_pShader ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetString( \"%s\" ) failed\n"), szInfo );
        return FALSE;
    }

    return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------------

BOOL CShaderValidator::GetTestCase_PS()
{
    CHAR       szInfo[ 256 ];
    D3DXHANDLE hDummy, hDummy_1;

    m_uCurrentTestCase = m_uTestNumber - ( m_nCurrentTestTemplate == 0 ? 0 : m_puTestTemplateAccumulatedSizeList[ m_nCurrentTestTemplate - 1 ] );

    sprintf( szInfo, "PS_%03d_Desc", m_uCurrentTestCase );

    if ( NULL == ( hDummy = m_pEffect->GetParameterByName( NULL, szInfo ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameterByName( \"%s\" ) failed\n"), szInfo );
        return FALSE;
    }

    LPCSTR pTestCaseDesc;

    if ( FAILED( m_pEffect->GetString( hDummy, &pTestCaseDesc ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetString( \"%s\" ) failed\n"), szInfo );
        return FALSE;
    }

    if ( strlen( pTestCaseDesc ) == 6 )
    {
        strncpy( m_szTestCaseDesc_ShaderVersion, pTestCaseDesc, 6 );
        m_szTestCaseDesc_ShaderVersion[ 6 ] = '\0';
        m_szTestCaseDesc_Desc[ 0 ] = '\0';
    }
    else if ( strlen( pTestCaseDesc ) == 7 )
    {
        strncpy( m_szTestCaseDesc_ShaderVersion, pTestCaseDesc, 7 );
        m_szTestCaseDesc_ShaderVersion[ 7 ] = '\0';
        m_szTestCaseDesc_Desc[ 0 ] = '\0';
    }
    else
    {
        strncpy( m_szTestCaseDesc_ShaderVersion, pTestCaseDesc, 7 );

        if ( m_szTestCaseDesc_ShaderVersion[ 6 ] == ' ' )
        {
            m_szTestCaseDesc_ShaderVersion[ 6 ] = '\0';

            if ( strlen( pTestCaseDesc ) )
            pTestCaseDesc += 9;
        }
        else
        {
            m_szTestCaseDesc_ShaderVersion[ 7 ] = '\0';
            pTestCaseDesc += 10;
        }

        strcpy( m_szTestCaseDesc_Desc, pTestCaseDesc );
    }

    sprintf( szInfo, "PS_%03d_Criteria", m_uCurrentTestCase );

    if ( NULL == ( hDummy = m_pEffect->GetParameterByName( NULL, szInfo ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameterByName( \"%s\" ) failed\n"), szInfo );
        return FALSE;
    }

    // bPassAssembler

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 0 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : bPassAssembler ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetBool( hDummy_1, &m_PS_Criteria.bPassAssembler ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetBool( \"%s\" : bPassAssembler ) failed\n"), szInfo );
        return FALSE;
    }

    // bRequireREF

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 1 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : bRequireREF ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetBool( hDummy_1, &m_PS_Criteria.bRequireREF ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetBool( \"%s\" : bRequireREF ) failed\n"), szInfo );
        return FALSE;
    }

    // nPixelShaderVersion

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 2 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nPixelShaderVersion ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_PS_Criteria.nPixelShaderVersion ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nPixelShaderVersion ) failed\n"), szInfo );
        return FALSE;
    }

    // nDynamicFlowControlDepth

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 3 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nDynamicFlowControlDepth ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_PS_Criteria.nDynamicFlowControlDepth ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nDynamicFlowControlDepth ) failed\n"), szInfo );
        return FALSE;
    }

    // nNumTemps

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 4 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nNumTemps ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_PS_Criteria.nNumTemps ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nNumTemps ) failed\n"), szInfo );
        return FALSE;
    }

    // nStaticFlowControlDepth

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 5 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nStaticFlowControlDepth ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_PS_Criteria.nStaticFlowControlDepth ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nStaticFlowControlDepth ) failed\n"), szInfo );
        return FALSE;
    }

    // nNumInstructionSlots

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 6 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nNumInstructionSlots ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_PS_Criteria.nNumInstructionSlots ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nNumInstructionSlots ) failed\n"), szInfo );
        return FALSE;
    }

    // nCaps

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 7 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nCaps ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_PS_Criteria.nCaps ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nCaps ) failed\n"), szInfo );
        return FALSE;
    }

    // nMaxPixelShader30InstructionSlots

    if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 8 ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : nMaxPixelShader30InstructionSlots failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetInt( hDummy_1, &m_PS_Criteria.nMaxPixelShader30InstructionSlots ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetInt( \"%s\" : nMaxPixelShader30InstructionSlots ) failed\n"), szInfo );
        return FALSE;
    }

	// fMinDXVersion

	if ( NULL == ( hDummy_1 = m_pEffect->GetParameter( hDummy, 9 ) ) )
	{
		WriteToLog( 1, _T("    m_pEffect->GetParameter( \"%s\" : fMinDXVersion failed\n"), szInfo );
		return FALSE;
	}

	if ( FAILED( m_pEffect->GetFloat( hDummy_1, &m_PS_Criteria.fMinDXVersion ) ) )
	{
		WriteToLog( 1, _T("    m_pEffect->GetFloat( \"%s\" : fMinDXVersion ) failed\n"), szInfo );
		return FALSE;
	}

    // assembly shader

    sprintf( szInfo, "PS_%03d", m_uCurrentTestCase );

    if ( NULL == ( hDummy = m_pEffect->GetParameterByName( NULL, szInfo ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetParameterByName( \"%s\" ) failed\n"), szInfo );
        return FALSE;
    }

    if ( FAILED( m_pEffect->GetString( hDummy, &m_pShader ) ) )
    {
        WriteToLog( 1, _T("    m_pEffect->GetString( \"%s\" ) failed\n"), szInfo );
        return FALSE;
    }

    return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------------

UINT CShaderValidator::GetTestCaseCount()
{
    if ( !GetTestTemplateNameList() )
    {
        WriteToLog( 1, _T("    GetTestTemplateNameList() failed\n") );
        return 0;
    }

    CHAR * pData;
    CHAR * pPtr;
    DWORD  dwBytesRead = 0;
	int k;

    for ( int i = 0; i < m_nTestTemplateCount; ++i )
    {
        if ( m_bLoadFromFile )
        {
            FILE * pFile;

            if ( NULL == ( pFile = fopen( m_pszTestTemplateNameList[ 0 ], "rt" ) ) )
            {
                WriteToLog( 1, _T("    fopen( %s ) failed\n"), m_pszTestTemplateNameList[ 0 ] );
                return 0;
            }

            if ( 0 == (dwBytesRead = fread( (void *)m_szTestCaseFromFile, sizeof( CHAR ), MAX_TEST_CASE_FROM_FILE_SIZE, pFile )) )
            {
                WriteToLog( 1, _T("    fread( %s ) failed\n"), m_pszTestTemplateNameList[ 0 ] );
                fclose( pFile );
                return 0;
            }

            fclose( pFile );

            pData = m_szTestCaseFromFile;
        }
        else
        {
            if ( !LoadFromResource( m_pszTestTemplateNameList[ i ], (LPVOID *)&pData, &dwBytesRead ) )
            {
                WriteToLog( 1, _T("    LoadFromResource( %s ) failed\n"), m_pszTestTemplateNameList[ i ] );
                return 0;
            }
        }

        if ( NULL == ( pPtr = _tcsstr( pData, _T("Test_Count") ) ) || pPtr > pData + strlen(pData) )
        {
            WriteToLog( 1, _T("    missing statement \"int Test_Case = #\" in resource: %s\n"), m_pszTestTemplateNameList[ i ] );
            return 0;
        }

        while ( !_istdigit( (unsigned char)(*pPtr) ) )  { ++pPtr; }

        #pragma prefast ( suppress: 6201 )
        m_puTestTemplateAccumulatedSizeList[ i >= 0 ? i : 0 ] = (UINT) _ttoi( pPtr )
                                                   + ( ( i > 0 ) ? m_puTestTemplateAccumulatedSizeList[ i - 1 ] : 0 );
    }

    return m_puTestTemplateAccumulatedSizeList[ m_nTestTemplateCount - 1 ];
}

//-------------------------------------------------------------------------------------------------------------------------

BOOL CShaderValidator::GetTestTemplateNameList()
{
    ReadString( _T("load"), _T("\0 "), m_pszTestTemplateNameList[ 0 ], ARRAYSIZE(m_pszTestTemplateNameList[ 0 ]) );

    if ( m_pszTestTemplateNameList[ 0 ][ 0 ] != '\0' )
    {
        m_bLoadFromFile = TRUE;
        m_nTestTemplateCount = 1;
        return TRUE;
    }

    if ( 0 == EnumResourceNames( NULL, RT_RCDATA, (ENUMRESNAMEPROC)EnumResNameProc, (LONG_PTR)this ) )
    {
        WriteToLog( 1, _T("    EnumResourceNames() failed\n") );
        return FALSE;
    }

    return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------------

void CShaderValidator::CommandLineHelp( )
{
	WriteCommandLineHelp( "-alldevices:\tRuns test on all 6 device types." );
}

//-------------------------------------------------------------------------------------------------------------------------

BOOL CALLBACK EnumResNameProc
(
    HMODULE,
    LPCTSTR,
    LPTSTR   _lpszName,
    LONG_PTR _lParam
)
{
    if ( 0 != strcmp( _lpszName, "TEST_INCLUDE.FX" ) )
    {
        int * nCount = &(((CShaderValidator *)_lParam)->m_nTestTemplateCount);

        _tcscpy( ((CShaderValidator *)_lParam)->m_pszTestTemplateNameList[ *nCount ], _lpszName );
        ++(*nCount);
    }

    return TRUE;
}

/*bool CShaderValidator::SetIndices(TCHAR *file_name)
{
	FILE *input, *output;
	bool  result = true;
	TCHAR str[120], str0[4] = "PS_", tempstr[50];
	TCHAR *pstr;
	UINT index = 3;

	input  = fopen(file_name, "r");
	strcpy(tempstr, file_name);
	strcat(tempstr, ".fx");
	output = fopen(tempstr, "w");

	if (NULL == input || NULL == output)
	{
		WriteToLog("Input file is NULL.\n");
		return false;
	}

	//get string from file
	while (NULL != fgets(str, 120, input))
	{
		//find indices and change them 
		pstr = str;
		while(true)//pstr != NULL)
		{
			if (NULL == (pstr = strstr(pstr, str0)))
			//if (pstr == NULL)
				break;
			pstr += strlen(str0);
			if(strlen(pstr) >= 3 &&
			   pstr[0] >= '0' && pstr[0] <= '9' &&
			   pstr[1] >= '0' && pstr[1] <= '9' &&
			   pstr[2] >= '0' && pstr[2] <= '9'   )
			{
				_stprintf(pstr, _T("%.3d%s"), (index++)/3, pstr+3);
			}
		}

		fputs(str, output);
	}

	fcloseall();

	return true;
}*/