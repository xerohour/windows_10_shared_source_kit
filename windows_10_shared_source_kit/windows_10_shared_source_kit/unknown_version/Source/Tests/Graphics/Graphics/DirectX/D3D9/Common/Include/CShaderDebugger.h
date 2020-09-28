#include <windows.h>
#include <d3d9.h>
#include <stdio.h>
#include <d3d9dm.hpp>


#define MAX_VS_SAMPLES  10
#define MAX_PS_SAMPLES  100

#define MAX_EVENT_DWORD_SIZE  81920

#define DS( szErrMsg, bShowErrorCode )  DebugSpew( TEXT(szErrMsg), bShowErrorCode );


struct VS_STATE
{
    FLOAT Reg_TEMP     [ D3DDM_VS_TEMPREG_MAX_9      ][ 4 ];
    FLOAT Reg_INPUT    [ D3DDM_VS_INPUTREG_MAX_9     ][ 4 ];
    FLOAT Reg_CONST    [ D3DDM_VS_CONSTREG_MAX_9     ][ 4 ];
    INT32 Reg_ADDR     [ 4 ];
    FLOAT Reg_RASTOUT  [ D3DDM_VS_RASTOUTREG_MAX_9   ][ 4 ];
    FLOAT Reg_ATTROUT  [ D3DDM_VS_ATTROUTREG_MAX_9   ][ 4 ];
    FLOAT Reg_TEXCRDOUT[ D3DDM_VS_TEXCRDOUTREG_MAX_9 ][ 4 ];
    INT32 Reg_CONSTINT [ D3DDM_VS_INTCONSTREG_MAX_9  ][ 4 ];
    BOOL  Reg_CONSTBOOL[ D3DDM_VS_BOOLCONSTREG_MAX_9 ];
    INT32 Reg_LOOP     [ 4 ];
    BOOL  Reg_PREDICATE[ D3DDM_VS_PREDICATEREG_MAX_9 ][ 4 ];
};


struct PS_STATE
{
    FLOAT Reg_TEMP     [ D3DDM_PS_TEMPREG_MAX_9      ][ 4 ];
    FLOAT Reg_INPUT    [ D3DDM_PS_INPUTREG_MAX_9     ][ 4 ];
    FLOAT Reg_CONST    [ D3DDM_PS_CONSTREG_MAX_9     ][ 4 ];
    FLOAT Reg_TEXTURE  [ D3DDM_PS_TEXTREG_MAX_9      ][ 4 ];
    FLOAT Reg_CONSTINT [ D3DDM_PS_CONSTINTREG_MAX_9  ][ 4 ];
    FLOAT Reg_CONSTBOOL[ D3DDM_PS_CONSTBOOLREG_MAX_9 ];
    FLOAT Reg_COLOROUT [ D3DDM_PS_COLOROUTREG_MAX_9  ][ 4 ];
    FLOAT Reg_DEPTHOUT;
    INT32 Reg_LOOP     [ 4 ];
    BOOL  Reg_PREDICATE[ D3DDM_VS_PREDICATEREG_MAX_9 ][ 4 ];
    FLOAT Reg_FACE;
    FLOAT Reg_POSITION [ 4 ];
    BOOL  Reg_DISCARD;
};


class CShaderDebugger
{
public:

    CShaderDebugger();
    ~CShaderDebugger();

    static bool Start();
    static bool End();
    static void SetSampleInterval_VS( DWORD dwInterval )  { m_dwSampleInterval_VS = dwInterval; }
    static void SetSampleInterval_PS( DWORD dwInterval )  { m_dwSampleInterval_PS = dwInterval; }
    static DWORD GetSampleCount_VS()  { return m_dwSampleIndex_VS; }
    static DWORD GetSampleCount_PS()  { return m_dwSampleIndex_PS; }
    static void  SetSampleCounter_VS( DWORD counter )  { m_dwSampleIndex_VS = counter; }
    static void  SetSampleCounter_PS( DWORD counter )  { m_dwSampleIndex_PS = counter; }
    static DWORD GetOpCount_VS()      { return m_dwOpCount_VS; }
    static DWORD GetOpCount_PS()      { return m_dwOpCount_PS; }

    static void  EnableDebugSpew()   { m_bEnableDebugSpew = true; }
    static void  DisableDebugSpew()  { m_bEnableDebugSpew = false; }

    static void  EnableVSBPOnce()    { m_bVSBPOnce = true; }
    static void  DisableVSBPOnce()   { m_bVSBPOnce = false; }
    static void  EnablePSBPOnce()    { m_bPSBPOnce = true; }
    static void  DisablePSBPOnce()   { m_bPSBPOnce = false; }

    static VS_STATE m_pVS_State[ MAX_VS_SAMPLES ];
    static PS_STATE m_pPS_State[ MAX_PS_SAMPLES ];


private:

    static DWORD WINAPI ShaderDebuggerThread( LPVOID );
    static void DebugSpew( TCHAR *szErrMsg, bool bShowErrorCode );

    //static HANDLE m_hNotifyPipe;
    //static HANDLE m_hDebugPipe;

    static DWORD m_pdwEventData[MAX_EVENT_DWORD_SIZE];

    static DWORD m_dwSampleInterval_VS;
    static DWORD m_dwSampleInterval_PS;
    static DWORD m_dwSampleIndex_VS;
    static DWORD m_dwSampleIndex_PS;
    static DWORD m_dwOpCount_VS;
    static DWORD m_dwOpCount_PS;

    static bool  m_bEnableDebugSpew;

    static bool  m_bVSBPOnce;
    static bool  m_bPSBPOnce;

    static HANDLE m_hSyncEvent;
};
