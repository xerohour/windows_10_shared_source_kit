//#include "TestUtilsPCH.h"
#include <CShaderDebugger.h>

#include <tchar.h>


//HANDLE   CShaderDebugger::m_hNotifyPipe = NULL;
//HANDLE   CShaderDebugger::m_hDebugPipe  = NULL;

DWORD    CShaderDebugger::m_pdwEventData[ MAX_EVENT_DWORD_SIZE ];

VS_STATE CShaderDebugger::m_pVS_State[ MAX_VS_SAMPLES ];
PS_STATE CShaderDebugger::m_pPS_State[ MAX_PS_SAMPLES ];

DWORD    CShaderDebugger::m_dwSampleInterval_VS = 0xffffffff;
DWORD    CShaderDebugger::m_dwSampleInterval_PS = 0xffffffff;
DWORD    CShaderDebugger::m_dwSampleIndex_VS    = 0;
DWORD    CShaderDebugger::m_dwSampleIndex_PS    = 0;
DWORD    CShaderDebugger::m_dwOpCount_VS        = 0;
DWORD    CShaderDebugger::m_dwOpCount_PS        = 0;

bool     CShaderDebugger::m_bEnableDebugSpew = false;
bool     CShaderDebugger::m_bVSBPOnce        = false;
bool     CShaderDebugger::m_bPSBPOnce        = false;

HANDLE   CShaderDebugger::m_hSyncEvent = NULL;

DWORD WINAPI CShaderDebugger::ShaderDebuggerThread( LPVOID )
{
    HANDLE hNotifyPipe = CreateNamedPipe( TEXT("\\\\.\\pipe\\ShaderDebuggerNotifyPipe"),
                                          PIPE_ACCESS_DUPLEX,        // GENERIS_READ for server, GENERIC_WRITE for client
                                          PIPE_TYPE_MESSAGE          // MESSAGE mode
                                          | PIPE_READMODE_MESSAGE    // data is read from this pipe as a stream of message
                                          | PIPE_WAIT,               // blocking mode
                                          PIPE_UNLIMITED_INSTANCES,  // max of 1 instance of this pipe
                                          4096,                      // outbound buffer size
                                          4096,                      // inbound buffer size
                                          1000,                      // default time-out in milliseconds
                                          NULL );                    // default security descriptor

    if ( 0 == SetEvent( m_hSyncEvent ) )
    {
        DS( "CShaderDebugger::ShaderDebuggerThread() -- SetEvent() failed!", true );
        return false;
    }

    if ( hNotifyPipe == INVALID_HANDLE_VALUE )
    {
        DS( "CShaderDebugger::Start() -- CreateNamedPipe( NOTIFY_PIPE ) failed!", true );
        return false;
    }

    DS( "CShaderDebugger::ShaderDebuggerThread() -- NOTIFY_PIPE created!", false );

    D3DDM_Notify Notify;
    DWORD dwBytesRead, dwBytesWritten;
    do
    {
        if ( 0 == ConnectNamedPipe( hNotifyPipe, NULL ) )
        {
            if ( ERROR_PIPE_CONNECTED != GetLastError() )
            {
                DS( "CShaderDebugger::ShaderDebuggerThread() -- ConnectNamedPipe( NOTIFY_PIPE ) failed!", true );
                return false;
            }
        }

        if( 0 == ReadFile( hNotifyPipe, &Notify, sizeof( Notify ), &dwBytesRead, NULL ) )
        {
            DS( "CShaderDebugger::ShaderDebuggerThread() -- ReadFile( NOTIFY_PIPE ) failed!", true );
            return 1;
        }

        if( dwBytesRead != sizeof( Notify ) )
        {
            DS( "CShaderDebugger::ShaderDebuggerThread() -- ReadFile( NOTIFY_PIPE ) data read mismatched!", false );
            return 1;
        }

        if ( Notify.dwPID != GetCurrentProcessId() )
        {
            if ( 0 == WriteFile( hNotifyPipe, &Notify, 0, &dwBytesWritten, NULL ) )
            {
                DS( "CShaderDebugger::ShaderDebuggerThread() -- WriteFile( NOTIFY_PIPE_WRONG_PID : ACK ) failed!", true );
                return 1;
            }

            if ( 0 == FlushFileBuffers( hNotifyPipe ) )
            {
                DS( "CShaderDebugger::ShaderDebuggerThread() -- FlushFileBuffers( NOTIFY_PIPE_WRONG_PID ) failed!", true );
                return 1;
            }

            if ( 0 == DisconnectNamedPipe( hNotifyPipe ) )
            {
                DS( "CShaderDebugger::ShaderDebuggerThread() -- DisconnectNamedPipe( NOTIFY_PIPE_WRONG_PID ) failed!", true );
                return 1;
            }
        }
    }
    while ( Notify.dwPID != GetCurrentProcessId() );

    TCHAR szPipeGUID[256];
    TCHAR szPipeName[256];
    
    if(_sntprintf( szPipeGUID, 255, TEXT("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
                Notify.guid.Data1, Notify.guid.Data2, Notify.guid.Data3,
                Notify.guid.Data4[0], Notify.guid.Data4[1],
                Notify.guid.Data4[2], Notify.guid.Data4[3],
                Notify.guid.Data4[4], Notify.guid.Data4[5],
                Notify.guid.Data4[6], Notify.guid.Data4[7] ) < 0)
				szPipeGUID[255] = TEXT('\0');

    _stprintf( szPipeName, TEXT("%s_%s"), D3DDM_PIPE_NAME, szPipeGUID );

    HANDLE hDebugPipe = CreateFile( szPipeName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                    SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION | SECURITY_EFFECTIVE_ONLY,
                                    NULL );
    if ( hDebugPipe == INVALID_HANDLE_VALUE )
    {
        DS( "CShaderDebugger::ShaderDebuggerThread() -- CreatePipe( DEBUG_PIPE ) failed!", true );
        return 1;
    }

    if ( 0 == WriteFile( hNotifyPipe, &Notify, 0, &dwBytesWritten, NULL ) )
    {
        DS( "CShaderDebugger::ShaderDebuggerThread() -- WriteFile( NOTIFY_PIPE : ACK ) failed!", true );
        return 1;
    }

    if ( 0 == FlushFileBuffers( hNotifyPipe ) )
    {
        DS( "CShaderDebugger::ShaderDebuggerThread() -- FlushFileBuffers( NOTIFY_PIPE ) failed!", true );
        return 1;
    }

    BYTE *byEventData = NULL;
    D3DDM_Event *pEvent;
    D3DDM_CreateShader  *pVS   = NULL;
    D3DDM_CreateShader  *pPS   = NULL;
    D3DDM_Breakpoint9   *pBP   = NULL;
    D3DDM_VShadeInstBP9 *pVSBP = NULL;
    D3DDM_PShadeInstBP9 *pPSBP = NULL;
    DWORD dwVSCount = 0;
    DWORD dwPSCount = 0;
    INT   nVSID = -1, nPSID = -1;

    do
    {
        DWORD dwTotalBytesRead = 0;

        while ( dwTotalBytesRead < sizeof( D3DDM_Event ) )
        {
            if ( FALSE == ReadFile( hDebugPipe, ((BYTE *)m_pdwEventData) + dwTotalBytesRead, sizeof( D3DDM_Event ) - dwTotalBytesRead, &dwBytesRead, NULL ) )
            {
                DS( "CShaderDebugger::ShaderDebuggerThread() -- ReadFile( DEBUG_PIPE : event header ) failed!", true );
                return 1;
            }

            dwTotalBytesRead += dwBytesRead;
        }

        pEvent = reinterpret_cast< D3DDM_Event* >( m_pdwEventData );

        while ( dwTotalBytesRead < pEvent->dwByteSize )
        {
            if ( FALSE == ReadFile( hDebugPipe, ((BYTE *)m_pdwEventData) + dwTotalBytesRead, pEvent->dwByteSize - dwTotalBytesRead, &dwBytesRead, NULL ) )
            {
                DS( "CShaderDebugger::ShaderDebuggerThread() -- ReadFile( DEBUG_PIPE : event data ) failed!", true );
                return 1;
            }

            dwTotalBytesRead += dwBytesRead;
        }

        D3DDM_SetShaderInstBP cmdVSBP;
        D3DDM_SetShaderInstBP cmdPSBP;

        switch ( pEvent->eType )
        {
        case D3DDM_EVENT_CREATEVSHADER:
            {
                nVSID = -1;
                nPSID = -1;
                pVS = reinterpret_cast< D3DDM_CreateShader* >( m_pdwEventData );

                ZeroMemory( &cmdVSBP, sizeof( D3DDM_SetShaderInstBP ) );
                cmdVSBP.eCmd            = D3DDM_CMD_SETBP;
                cmdVSBP.eBPType         = D3DDM_BP_VSHADEINST;
                cmdVSBP.iByteOffset     = (pVS->nDefinitionSize - 1) * 4;
                cmdVSBP.iShader         = pVS->iShader;
                cmdVSBP.nSequenceNumber = pVS->nSequenceNumber;
                cmdVSBP.dwByteSize      = sizeof( cmdVSBP );
                if ( 0 == WriteFile( hDebugPipe, &cmdVSBP, sizeof( cmdVSBP ), &dwBytesWritten, NULL ) )
                {
                    DS( "CShaderDebugger::ShaderDebuggerThread() -- WriteFile( DEBUG_PIPE : VS_BP ) failed!", true );
                    return 1;
                }

                D3DDM_Go cmdGo;
                cmdGo.eCmd       = D3DDM_CMD_GO;
                cmdGo.dwByteSize = sizeof( cmdGo );
                if ( 0 == WriteFile( hDebugPipe, &cmdGo, sizeof( cmdGo ), &dwBytesWritten, NULL ) )
                {
                    DS( "CShaderDebugger::ShaderDebuggerThread() -- WriteFile( DEBUG_PIPE : VS_GO ) failed!", true );
                    return 1;
                }

                //DS( "CShaderDebugger -- EVENT : CreateVertexShader", false );
            }
            break;

        case D3DDM_EVENT_CREATEPSHADER:
            {
                nVSID = -1;
                nPSID = -1;
                pPS = reinterpret_cast< D3DDM_CreateShader* >( m_pdwEventData );

                ZeroMemory( &cmdPSBP, sizeof( D3DDM_SetShaderInstBP ) );
                cmdPSBP.eCmd            = D3DDM_CMD_SETBP;
                cmdPSBP.eBPType         = D3DDM_BP_PSHADEINST;
                cmdPSBP.iByteOffset     = (pPS->nDefinitionSize -1) * 4;
                cmdPSBP.iShader         = pPS->iShader;
                cmdPSBP.nSequenceNumber = pPS->nSequenceNumber;
                cmdPSBP.dwByteSize      = sizeof( cmdPSBP );
                if ( 0 == WriteFile( hDebugPipe, &cmdPSBP, sizeof( cmdPSBP ), &dwBytesWritten, NULL ) )
                {
                    DS( "CShaderDebugger::ShaderDebuggerThread() -- WriteFile( DEBUG_PIPE : PS_BP ) failed!", true );
                    return 1;
                }

                D3DDM_Go cmdGo;
                cmdGo.eCmd       = D3DDM_CMD_GO;
                cmdGo.dwByteSize = sizeof( cmdGo );
                if ( 0 == WriteFile( hDebugPipe, &cmdGo, sizeof( cmdGo ), &dwBytesWritten, NULL ) )
                {
                    DS( "CShaderDebugger::ShaderDebuggerThread() -- WriteFile( DEBUG_PIPE : PS_GO ) failed!", true );
                    return 1;
                }

                //DS( "CShaderDebugger -- EVENT : CreatePixelShader", false );
            }
            break;

        case D3DDM_EVENT_BREAKPOINT:
            {
                pBP = reinterpret_cast< D3DDM_Breakpoint9* >( m_pdwEventData );

                switch ( pBP->eBPType )
                {
                case D3DDM_BP_VSHADEINST:
                    {
                        pVSBP = reinterpret_cast< D3DDM_VShadeInstBP9* >( m_pdwEventData );

                        if ( nVSID != pVSBP->iShader )
                        {
                            m_dwSampleIndex_VS = 0;
                            dwVSCount = 0;
                            nVSID = pVSBP->iShader;
                        }

                        ++dwVSCount;
                        if ( m_dwSampleIndex_VS == 0 ||
                            ( dwVSCount - 1 == m_dwSampleInterval_VS && m_dwSampleIndex_VS < MAX_VS_SAMPLES ) )
                        {
                            CopyMemory( m_pVS_State[ m_dwSampleIndex_VS ].Reg_INPUT,     pVSBP->vstate      .InputRegs,     sizeof( pVSBP->vstate      .InputRegs ) );
                            CopyMemory( m_pVS_State[ m_dwSampleIndex_VS ].Reg_TEMP,      pVSBP->vshadestate .TempRegs,      sizeof( pVSBP->vshadestate .TempRegs ) );
                            CopyMemory( m_pVS_State[ m_dwSampleIndex_VS ].Reg_ADDR,      pVSBP->vshadestate .AddressReg,    sizeof( pVSBP->vshadestate .AddressReg ) );
                            CopyMemory( m_pVS_State[ m_dwSampleIndex_VS ].Reg_LOOP,      pVSBP->vshadestate .LoopReg,       sizeof( pVSBP->vshadestate .LoopReg ) );
                            CopyMemory( m_pVS_State[ m_dwSampleIndex_VS ].Reg_PREDICATE, pVSBP->vshadestate .PredicateRegs, sizeof( pVSBP->vshadestate .PredicateRegs ) );
                            CopyMemory( m_pVS_State[ m_dwSampleIndex_VS ].Reg_RASTOUT,   pVSBP->vshadestate .RastOutRegs,   sizeof( pVSBP->vshadestate .RastOutRegs ) );
                            CopyMemory( m_pVS_State[ m_dwSampleIndex_VS ].Reg_ATTROUT,   pVSBP->vshadestate .AttrOutRegs,   sizeof( pVSBP->vshadestate .AttrOutRegs ) );
                            CopyMemory( m_pVS_State[ m_dwSampleIndex_VS ].Reg_TEXCRDOUT, pVSBP->vshadestate .TexCrdOutRegs, sizeof( pVSBP->vshadestate .TexCrdOutRegs ) );
                            CopyMemory( m_pVS_State[ m_dwSampleIndex_VS ].Reg_CONST,     pVSBP->vshadeconsts.ConstRegs,     sizeof( pVSBP->vshadeconsts.ConstRegs ) );
                            CopyMemory( m_pVS_State[ m_dwSampleIndex_VS ].Reg_CONSTINT,  pVSBP->vshadeconsts.IntConstRegs,  sizeof( pVSBP->vshadeconsts.IntConstRegs ) );
                            CopyMemory( m_pVS_State[ m_dwSampleIndex_VS ].Reg_CONSTBOOL, pVSBP->vshadeconsts.BoolConstReg,  sizeof( pVSBP->vshadeconsts.BoolConstReg ) );
                            ++m_dwSampleIndex_VS;
                            dwVSCount = 0;
                            //DS( "CShaderDebugger -- EVENT : VertexShader breakpoint", false );
                        }

                        D3DDM_Go cmdGo;
                        ZeroMemory( &cmdGo, sizeof( D3DDM_Go ) );
                        cmdGo.eCmd       = D3DDM_CMD_GO;
                        cmdGo.dwByteSize = sizeof( cmdGo );
                        if ( 0 == WriteFile( hDebugPipe, &cmdGo, sizeof( cmdGo ), &dwBytesWritten, NULL ) )
                        {
                            DS( "CShaderDebugger -- WriteFile( D3DDM_CMD_GO ) failed : VertexShader breakpoint!", true );
                            return 1;
                        }

                        if ( m_bVSBPOnce )
                        {
                            D3DDM_ClearShaderInstBP CVSBP;
                            ZeroMemory( &CVSBP, sizeof( CVSBP ) );
                            CVSBP.eCmd            = D3DDM_CMD_CLEARBP;
                            CVSBP.nSequenceNumber = pVS->nSequenceNumber;
                            CVSBP.eBPType         = D3DDM_BP_VSHADEINST;
                            CVSBP.iShader         = cmdVSBP.iShader;
                            CVSBP.iByteOffset     = cmdVSBP.iByteOffset;
                            CVSBP.dwByteSize      = sizeof( CVSBP );
                            if ( 0 == WriteFile( hDebugPipe, &CVSBP, sizeof( CVSBP ), &dwBytesWritten, NULL ) )
                            {
                                DS( "CShaderDebugger -- WriteFile( D3DDM_CMD_CLEARBP ) failed : VertexShader breakpoint!", true );
                                return 1;
                            }
                        }
                    }
                    break;

                case D3DDM_BP_PSHADEINST:
                    {
                        pPSBP = reinterpret_cast< D3DDM_PShadeInstBP9* >( m_pdwEventData );

                        if ( nPSID != pPSBP->iShader )
                        {
                            m_dwSampleIndex_PS = 0;
                            dwPSCount = 0;
                            nPSID = pPSBP->iShader;
                        }

                        ++dwPSCount;
                        if ( m_dwSampleIndex_PS == 0 ||
                            ( dwPSCount - 1 == m_dwSampleInterval_PS && m_dwSampleIndex_PS < MAX_PS_SAMPLES ) )
                        {
                            CopyMemory(   m_pPS_State[ m_dwSampleIndex_PS ].Reg_INPUT,       pPSBP->pstate      .InputRegs,      sizeof( pPSBP->pstate      .InputRegs ) );
                            CopyMemory( &(m_pPS_State[ m_dwSampleIndex_PS ].Reg_DEPTHOUT), &(pPSBP->pstate      .Depth),         sizeof( pPSBP->pstate      .Depth ) );
                            CopyMemory( &(m_pPS_State[ m_dwSampleIndex_PS ].Reg_FACE),     &(pPSBP->pstate      .Face),          sizeof( pPSBP->pstate      .Face ) );
                            CopyMemory(   m_pPS_State[ m_dwSampleIndex_PS ].Reg_POSITION,    pPSBP->pstate      .Position,       sizeof( pPSBP->pstate      .Position ) );
                            CopyMemory(   m_pPS_State[ m_dwSampleIndex_PS ].Reg_TEMP,        pPSBP->pshadestate .TempRegs,       sizeof( pPSBP->pshadestate .TempRegs ) );
                            CopyMemory(   m_pPS_State[ m_dwSampleIndex_PS ].Reg_TEXTURE,     pPSBP->pshadestate .TextRegs,       sizeof( pPSBP->pshadestate .TextRegs ) );
                            CopyMemory(   m_pPS_State[ m_dwSampleIndex_PS ].Reg_COLOROUT,    pPSBP->pshadestate .ColorOutRegs,   sizeof( pPSBP->pshadestate .ColorOutRegs ) );
                            CopyMemory(   m_pPS_State[ m_dwSampleIndex_PS ].Reg_LOOP,        pPSBP->pshadestate .LoopReg,        sizeof( pPSBP->pshadestate .LoopReg ) );
                            CopyMemory(   m_pPS_State[ m_dwSampleIndex_PS ].Reg_PREDICATE,   pPSBP->pshadestate .PredicateRegs,  sizeof( pPSBP->pshadestate .PredicateRegs ) );
                            CopyMemory( &(m_pPS_State[ m_dwSampleIndex_PS ].Reg_DISCARD),  &(pPSBP->pshadestate .Discard),       sizeof( pPSBP->pshadestate .Discard ) );
                            CopyMemory(   m_pPS_State[ m_dwSampleIndex_PS ].Reg_CONST,       pPSBP->pshadeconsts.ConstRegs,      sizeof( pPSBP->pshadeconsts.ConstRegs ) );
                            CopyMemory(   m_pPS_State[ m_dwSampleIndex_PS ].Reg_CONSTINT,    pPSBP->pshadeconsts.IntConstRegs,   sizeof( pPSBP->pshadeconsts.IntConstRegs ) );
                            CopyMemory(   m_pPS_State[ m_dwSampleIndex_PS ].Reg_CONSTBOOL,   pPSBP->pshadeconsts.BoolConstRegs,  sizeof( pPSBP->pshadeconsts.BoolConstRegs ) );
                            ++m_dwSampleIndex_PS;
                            dwPSCount = 0;
                            //DS( "CShaderDebugger -- EVENT : PixelShader breakpoint", false );
                        }

                        D3DDM_Go cmdGo;
                        ZeroMemory( &cmdGo, sizeof( D3DDM_Go ) );
                        cmdGo.eCmd       = D3DDM_CMD_GO;
                        cmdGo.dwByteSize = sizeof( cmdGo );
                        if ( 0 == WriteFile( hDebugPipe, &cmdGo, sizeof( cmdGo ), &dwBytesWritten, NULL ) )
                        {
                            DS( "CShaderDebugger -- WriteFile( D3DDM_CMD_GO ) failed : PixelShader breakpoint!", true );
                            return 1;
                        }

                        if ( m_bPSBPOnce )
                        {
                            D3DDM_ClearShaderInstBP CPSBP;
                            ZeroMemory( &CPSBP, sizeof( CPSBP ) );
                            CPSBP.eCmd            = D3DDM_CMD_CLEARBP;
                            CPSBP.nSequenceNumber = pPS->nSequenceNumber;
                            CPSBP.eBPType         = D3DDM_BP_PSHADEINST;
                            CPSBP.iShader         = cmdPSBP.iShader;
                            CPSBP.iByteOffset     = cmdPSBP.iByteOffset;
                            CPSBP.dwByteSize      = sizeof( CPSBP );
                            if ( 0 == WriteFile( hDebugPipe, &CPSBP, sizeof( CPSBP ), &dwBytesWritten, NULL ) )
                            {
                                DS( "CShaderDebugger -- WriteFile( D3DDM_CMD_CLEARBP ) failed : PixelShader breakpoint!", true );
                                return 1;
                            }
                        }
                    }
                    break;
                }
            }
            break;

        case D3DDM_EVENT_CONNECT:
        case D3DDM_EVENT_DELETEVSHADER:
        case D3DDM_EVENT_DELETEPSHADER:
        case D3DDM_EVENT_VSTEP:
        case D3DDM_EVENT_PSTEP:
            {
                D3DDM_Go Cmd;
                Cmd.eCmd       = D3DDM_CMD_GO;
                Cmd.dwByteSize = sizeof( Cmd );
                if ( 0 == WriteFile( hDebugPipe, &Cmd, sizeof( Cmd ), &dwBytesWritten, NULL ) )
                {
                    DS( "CShaderDebugger::ShaderDebuggerThread() -- WriteFile( DEBUG_PIPE : GO ) failed!", true );
                    return 1;
                }
            }
            break;

        default:
            break;
        }
    }
    while ( pEvent->eType != D3DDM_EVENT_DISCONNECT );

    BOOL bSuccessClose = FALSE;

    for ( UINT i = 0; i < 5; ++i )
    {
        if ( !PeekNamedPipe( hDebugPipe, NULL, 0, NULL, &dwBytesRead, NULL ) )
        {
            bSuccessClose = TRUE;
            break;
        }
        Sleep( 1 );
    }

    if ( !bSuccessClose )
    {
        DS( "CShaderDebugger::ShaderDebuggerThread() -- DEBUG_PIPE fails to close after 5 tries!", true );
    }
    if( 0 == CloseHandle( hDebugPipe ) )
    {
        {
            DS( "CShaderDebugger::ShaderDebuggerThread() -- CloseHandle( DEBUG_PIPE ) failed!", true );
        }
    }

    bSuccessClose = FALSE;

    for ( UINT i = 0; i < 5; ++i )
    {
        if ( !PeekNamedPipe( hNotifyPipe, NULL, 0, NULL, &dwBytesRead, NULL ) )
        {
            bSuccessClose = TRUE;
            break;
        }
        Sleep( 1 );
    }

    if ( !bSuccessClose )
    {
        DS( "CShaderDebugger::ShaderDebuggerThread() -- NOTIFY_PIPE fails to close after 5 tries!", true );
    }
    if ( 0 == CloseHandle( hNotifyPipe ) )
    {
        DS( "CShaderDebugger::ShaderDebuggerThread() -- CloseHandle( NOTIFY_PIPE ) failed!", true );
    }

    DS( "CShaderDebugger::ShaderDebuggerThread() -- Ended!", false );
    return 0;
}




CShaderDebugger::CShaderDebugger()
{
}




CShaderDebugger::~CShaderDebugger()
{
}




bool CShaderDebugger::Start()
{
    HKEY hNotifyRegKey;
    DWORD dwKeyDisposition;

    if ( ERROR_SUCCESS != RegCreateKeyEx( HKEY_CURRENT_USER,        // key
                                          D3DDM_NOTIFY_REGISTRY,    // subkey
                                          0,                        // reserved, must be 0
                                          NULL,                     // class
                                          REG_OPTION_NON_VOLATILE,  // the key should preserve after reboot
                                          KEY_SET_VALUE,            // we want to set the value of subkey
                                          NULL,                     // default security descriptor
                                          &hNotifyRegKey,           // returned key handle
                                          &dwKeyDisposition ) )     // returned key disposition
    {
        DS( "CShaderDebugger::Start() -- RegCreateKeyEx( NOTIFY_KEY ) failed!", true );
        return false;
    }

    switch ( dwKeyDisposition )
    {
    case REG_CREATED_NEW_KEY:      DS( "CShaderDebugger::Start() -- New NOTIFY_KEY created!", false );  break;
    case REG_OPENED_EXISTING_KEY:  DS( "CShaderDebugger::Start() -- NOTIFY_KEY already exsits!", false );  break;
    default:                       DS( "CShaderDebugger::Start() -- Unrecognized disposition info returned for NOTIFY_KEY!", false );  break;
    }

    if ( ERROR_SUCCESS != RegSetValueEx( hNotifyRegKey,                     // the regkey we just created
                                         TEXT("ShaderDebuggerNotifyPipe"),  // value name
                                         0,                                 // reserved, must be 0
                                         REG_NONE,                          // no defined value type
                                         0, 0 ) )                           // no data need to be set for this value
    {
        DS( "CShaderDebugger::Start() -- RegSetValueEx( NOTIFY_KEY ) failed!", true );
        return false;
    }

    if ( ERROR_SUCCESS != RegCloseKey( hNotifyRegKey ) )
    {
        DS( "CShaderDebugger::Start() -- RegCloseKey( NOTIFY_KEY ) failed!", true );
        return false;
    }

    // m_hSyncEvent is a manual-reset event, with initial state = non-signaled
    if ( NULL == ( m_hSyncEvent = CreateEvent( NULL, TRUE, FALSE, NULL ) ) )
    {
        DS( "CShaderDebugger::Start() -- CreateEvent() failed!", true );
        return false;
    }

    // m_hSyncEvent is signaled by this thread
    if ( NULL == CreateThread( NULL,                  // default security descriptor
                               0,                     // default stack size as current process
                               ShaderDebuggerThread,  // shader debugger thread function
                               NULL,                  // no param passed to the thread function above
                               0,                     // run the thread immediately after it is created
                               NULL ) )               // we don't need the returned thread ID
    {
        DS( "CShaderDebugger::Start() -- CreateThread() : # 1 failed!", true );
        return false;
    }

    // wait for m_hSyncEvent to be signaled
    if ( WAIT_OBJECT_0 != WaitForSingleObject( m_hSyncEvent, INFINITE ) )
    {
        DS( "CShaderDebugger::Start() -- WAIT_OBJECT_0 != WaitForsingleObject( #1 )", true );
        return false;
    }

    // reset m_hSyncEvent to non-signaled
    if ( 0 == ResetEvent( m_hSyncEvent ) )
    {
        DS( "CShaderDebugger::Start() -- ResetEvent( #1 ) failed!", true );
        return false;
    }

    // m_hSyncEvent is signaled by this thread
    if ( NULL == CreateThread( NULL,                  // default security descriptor
                               0,                     // default stack size as current process
                               ShaderDebuggerThread,  // shader debugger thread function
                               NULL,                  // no param passed to the thread function above
                               0,                     // run the thread immediately after it is created
                               NULL ) )               // we don't need the returned thread ID
    {
        DS( "CShaderDebugger::Start() -- CreateThread() : # 2 failed!", true );
        return false;
    }

    // wait for m_hSyncEvent to be signaled
    if ( WAIT_OBJECT_0 != WaitForSingleObject( m_hSyncEvent, INFINITE ) )
    {
        DS( "CShaderDebugger::Start() -- WAIT_OBJECT_0 != WaitForsingleObject( #2 )", true );
        return false;
    }

    return true;
}


bool CShaderDebugger::End()
{
    //m_hNotifyPipe         = NULL;
    //m_hDebugPipe          = NULL;
    m_dwSampleInterval_VS = 0xffffffff;
    m_dwSampleInterval_PS = 0xffffffff;
    m_dwSampleIndex_VS    = 0;
    m_dwSampleIndex_PS    = 0;
    return true;
}


void CShaderDebugger::DebugSpew( TCHAR *szErrMsg, bool bShowErrorCode = true )
{
    if ( !m_bEnableDebugSpew )
    {
        return;
    }

    if ( bShowErrorCode )
    {
        TCHAR szTemp[128];
        LPVOID lpMsgBuf;
        if ( 0 == FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                 NULL, GetLastError(), MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                                 (LPTSTR)&lpMsgBuf, 0, NULL ) )
        {
            _stprintf( szTemp, _T("CShaderDebugger::DebugSpew() -- FormatMessage() failed for error code : %d !"), GetLastError() );
            OutputDebugString( szTemp );
            return;
        }

        if(_sntprintf( szTemp, 127, _T("\n######### GetLastError() returns %d : %s \n"), GetLastError(), (TCHAR*)lpMsgBuf ) < 0)
			szTemp[127] = _T('\0');

        LocalFree( lpMsgBuf );
        OutputDebugString( TEXT("######### ") );
        OutputDebugString( szErrMsg );
        OutputDebugString( szTemp );
    }
    else
    {
        OutputDebugString( TEXT("######### ") );
        OutputDebugString( szErrMsg );
        OutputDebugString( "\n" );
    }
}