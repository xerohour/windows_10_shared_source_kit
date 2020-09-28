#include "stdafx.h"

//Timeout period for syncronization actions.
DWORD g_syncronizationTimeout = 120000;


HRESULT UtilReadPixel(CI3DSurface2Ptr pSrf, D3DRECT * pRect, SColor * pColor)
{
    HRESULT hr = S_OK;
    D3DLOCKED_RECT LockedRect;

    //convert new surface to a known format
    pToSColor pfnConvert;
    if(!GetConversionFunctions(pSrf->GetFormat(), NULL, &pfnConvert))
    {
        hr = E_FAIL;
        //this should never happen because we checkin CapsCheck() to make sure the format is supported
        PATH_TRAP();
        goto TESTFAIL;
    }

    //lock and analyze the destination surface
    hr=pSrf->Lock(0,pRect,&LockedRect);
    if(FAILED(hr))
    {
        PATH_TRAP();
        goto TESTFAIL;
    }

    pfnConvert((BYTE*)LockedRect.pBits, pColor);
    hr = pSrf->Unlock();
    if(FAILED(hr))
    {
        PATH_TRAP();
        goto TESTFAIL;
    }
TESTFAIL:
    return hr;
}


float UtilCalcDeviation(SColor * pColor, float ExpAlpha, float ExpRed, float ExpGreen, float ExpBlue)
{
    
    float adiff, rdiff, gdiff, bdiff;

    adiff = ExpAlpha - pColor->a ;
    rdiff = ExpRed   - pColor->r;
    gdiff = ExpGreen - pColor->g;
    bdiff = ExpBlue  - pColor->b;

    return sqrtf((adiff * adiff) + (rdiff * rdiff) + (gdiff * gdiff) + (bdiff * bdiff));
}


// Note that this routine must be called under the lock. But, because it's a 'global'
// routine, we can't validate that we're under the lock.
void UtilPrintMessage(Message * msg,CCTLLogger* _Log)
{
    TCHAR szMessage[512];
    StringCchPrintf(szMessage,512, 
TEXT("TestCase: %d HRESULT: %X Handle: %X Format: %X Height: %i Width: %i Levels: %X"),
                    msg->dwTestId,
                    msg->hrUser,                                    
                    msg->hTexHandle,
                    msg->d3dFormat,
                    msg->uiHeight,
                    msg->uiWidth,
                    msg->uiLevels );
    // if the caller gives us a CCTLLogger pointer, we'll also log this information 
    // to that destination.
    if( _Log )
    {
        _Log->Message(szMessage);
    }
    DBG_MSG(szMessage);
}


HRESULT UtilCreateUserProcess()
{
    HRESULT hr = S_OK;  
    BOOL fCreated = FALSE;
    PROCESS_INFORMATION ProcInfo;
    STARTUPINFOA StartInfo;
    ZeroMemory( &StartInfo, sizeof(STARTUPINFO) );
    ZeroMemory( &ProcInfo, sizeof(PROCESS_INFORMATION) );

    StartInfo.cb = sizeof(STARTUPINFO);

    char * szArgs = " user";
    fCreated = CreateProcessA("crossproc.exe",
                  szArgs,
                  NULL,
                  NULL,
                  FALSE,
                  0,
                  NULL,
                  NULL,
                  &StartInfo,
                  &ProcInfo);
    if(fCreated == FALSE)
    {
        DBG_MSG(TEXT("Failed to create user process."));
        return E_FAIL;
    }
    CloseHandle(ProcInfo.hProcess);
    CloseHandle(ProcInfo.hThread);    
    

    return hr;
}

