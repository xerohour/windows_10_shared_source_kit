//-----------------------------------------------------------------------------
// File: sched_bvt.h
// Desc: Header file sched_bvt sample app
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#ifndef D3DX_TEST_ONLY
	#define D3DX_TEST_ONLY
#endif

#include "ctllogger.h"

#include <vector>
using std::vector;
#include <string>
using std::basic_string;

//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
#define RESOURCEFROMRC
#define MAX_SIMULTANEOUS_APPS_SCH_BVT 20

typedef basic_string<TCHAR> tstring;

enum TEST_MODE
{
    INITIALIZING = 0,
    WAITING = 1,
    WARMUP = 2,
    TESTING = 3,
    COOLDOWN = 4,
    PROCESSWAIT = 5,
    PROCESSING = 6
};

#ifdef _X86_
#define RDTSC_OPTION
#endif

inline void TimeStamp( __int64* time )
{
#ifdef RDTSC_OPTION
    _asm
    {
        mov  ebx, time
        rdtsc
        mov  [ebx], eax
        mov  [ebx+4], edx
    }
#else
    QueryPerformanceCounter( (LARGE_INTEGER*)time );
#endif
}


template <typename T, typename container >
void getMean( const container& values, T& mean )
{
    T sum( 0 );

    container::const_iterator it;
    for( it = values.begin();
         it != values.end();
         it ++ )
        sum += (T)*it;

    mean = sum / (T) values.size();
}

template <typename T, typename container >
void getMeanAndDeviation( const container& values, T& mean, T& std_deviation )
{
    getMean( values, mean );

    T var( 0 );

    container::const_iterator it;
    for( it = values.begin();
         it != values.end();
         it ++ )
        var += ((T)*it - mean) * ((T)*it - mean);

    var = var / (T)(values.size()-1);
    
    std_deviation = (T) sqrt( var );
}

class CLog
{
protected:
    HANDLE  m_hLogFile;
    bool    m_bCacheDirty;
    tstring m_cache;
public:
    const UINT READ;
    const UINT WRITE;
    CLog()
        : READ(1), WRITE(2)
    {
        m_hLogFile = INVALID_HANDLE_VALUE;
    }
    CLog( const CLog& rhs )
        : m_hLogFile( rhs.m_hLogFile ), READ( rhs.READ ), WRITE( rhs.WRITE )
    {
    }
    CLog& operator=( const CLog& rhs )
    {
        m_hLogFile = rhs.m_hLogFile;
    }
    bool open( LPCTSTR szLogName, UINT options )
    {
        if( INVALID_HANDLE_VALUE != m_hLogFile )
            close();
        
        m_bCacheDirty = true;

        DWORD dwDesiredAccess = 0, dwDisposition = CREATE_ALWAYS;
        if( options & WRITE )
            dwDesiredAccess |= GENERIC_WRITE;
        if( options & READ )
        {
            dwDesiredAccess |= GENERIC_READ;
            dwDisposition = OPEN_ALWAYS;
        }

        m_hLogFile = CreateFile( szLogName,
                                 dwDesiredAccess,
                                 0,
                                 NULL,
                                 dwDisposition,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL );

        return INVALID_HANDLE_VALUE != m_hLogFile;
    }
    bool write( LPCTSTR szData )
    {
        if( INVALID_HANDLE_VALUE == m_hLogFile )
            return false;
        
        if( INVALID_SET_FILE_POINTER == SetFilePointer( m_hLogFile, 0, NULL, FILE_END ) )
            return false;

        m_bCacheDirty = true;

        DWORD dwBytes = _tcslen( szData ) * sizeof( TCHAR );
        DWORD dwBytesWritten = 0;

        BOOL bResult = WriteFile( m_hLogFile, szData, dwBytes, &dwBytesWritten, NULL );
        
        return bResult && (dwBytes == dwBytesWritten);
    }
    template <typename T>
    bool readValue( LPCTSTR szKey, LPCTSTR szFmtString, T& value )
    {
        if( INVALID_HANDLE_VALUE == m_hLogFile )
            return false;

        if( m_bCacheDirty )
        {
            DWORD dwSize = GetFileSize( m_hLogFile, NULL );
            DWORD dwBytesRead = 0;
            m_cache.resize( (tstring::size_type)dwSize/sizeof(tstring::value_type) );
            BOOL bResult = ReadFile( m_hLogFile, &*m_cache.begin(), dwSize, &dwBytesRead, NULL );
            if( !bResult || dwSize != dwBytesRead )
                return false;
            m_bCacheDirty = false;
        }
        
        tstring key( szKey );
        tstring::size_type key_pos = m_cache.rfind( key );
        if( tstring::npos == key_pos )
            return false;

        T val;
        tstring val_str( m_cache.begin() + key_pos + key.size(), m_cache.end() );
        if( 1 != _stscanf( val_str.c_str(), szFmtString, &val ) )
            return false;

        value = val;
        return true;
    }
    void close()
    {
        if( INVALID_HANDLE_VALUE == m_hLogFile )
            return;

        CloseHandle( m_hLogFile );
        m_hLogFile = INVALID_HANDLE_VALUE;
    }
};

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    CCTLLogger*             m_pWTTLog;
    HANDLE                  m_hWTTTestCase;
    TEST_MODE               m_eMode;
    vector<__int64>         m_vecTimes;
    vector<HANDLE>          m_vecChildProcesses;
    FLOAT                   m_fKernelTime;
    HANDLE                  m_hInstanceMutex;
    HANDLE                  m_hReadyEvent;
    HANDLE                  m_hTestEvent;
    HANDLE                  m_hReadyToProcessEvent;
    HANDLE                  m_hProcessEvent;
    UINT                    m_nWarmUpFrames;
    UINT                    m_nTestFrames;
    UINT                    m_nWarmUpFrames_count;
    UINT                    m_nTestFrames_count;
    UINT                    m_nAppInstances;
    UINT                    m_nAppInstance_count;
    UINT                    m_nAppMonitor;
    UINT                    m_nAppInstance;
    bool                    m_bPrimaryApp;
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
    CD3DFont*               m_pFont;                // Font for drawing text
    
    ID3DXMesh*              m_pD3DXMesh;            // D3DX mesh to store knot
    ID3DXMesh*              m_pSkyboxMesh;          // D3DX mesh to store skybox
    LPD3DXEFFECT            m_pEffect;
    D3DXHANDLE              m_hProj;
    D3DXHANDLE              m_hWorld;
    D3DXHANDLE              m_hView;
    D3DXHANDLE              m_hTechnique;
    D3DXHANDLE              m_hTechBlur;
    D3DXHANDLE              m_hTechSkybox;
    D3DXHANDLE              m_hRTTexture;
    D3DXHANDLE              m_hTapOffset0;
    D3DXHANDLE              m_hTapOffset1;
    D3DXHANDLE              m_hTapOffset2;
    D3DXHANDLE              m_hTapOffset3;
    LPDIRECT3DCUBETEXTURE9  m_pSkyTexture;
    LPDIRECT3DTEXTURE9      m_pRTTexture;
    LPDIRECT3DTEXTURE9      m_pBlurTexture;
    LPDIRECT3DTEXTURE9      m_pMeshDiffTexture;
    LPDIRECT3DTEXTURE9      m_pMeshAlphaTexture;
    LPDIRECT3DSURFACE9      m_pBlurSurface;
    LPDIRECT3DSURFACE9      m_pRTSurface;
    LPDIRECT3DSURFACE9      m_pBackbuffer;

    FLOAT                   m_fWorldRotX;           // World rotation state X-axis
    FLOAT                   m_fWorldRotY;           // World rotation state Y-axis
    TCHAR                   m_strKernelTime[MAX_PATH];
    
protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );
    
    HRESULT CreateSkyBox( LPDIRECT3DCUBETEXTURE9* ppTexture );
    HRESULT RenderText();
    void    ResetFrame();
    void    ParseCommandLine();
    HRESULT OnFrameNotify();
    void    CmdInitialize();
    bool    StatusWarmup();
    bool    StatusTesting();
    bool    StatusCooldown();
    bool    StatusProcessWait();
    bool    StatusProcessing();
    void    ProcessStats();
    void    LogHeader( tstring& logHeader );
    bool    RollupResults();
    void    CreateWTTLog();
    void    DestroyWTTLog();


public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

