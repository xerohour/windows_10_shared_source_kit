/******************************Module*Header*******************************
 * Module Name: CDSReset.cpp
 *
 * Implementations of reseting system configuration test component using 
 * ChangeDisplaySetting
 *
 * Author: Mai Dao [maithida]
 *
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 **************************************************************************/

#include "BaseVid.h"
#include "windows.h"
#include "I3D.h"
#include "wingdi.h"
#include "winuserp.h"
#include "ccdfunctions.h"
#include "PDBRegistry.h"
#include "PDBDisplay.h"
#include "PDBDiagInfo.h"
#include "SaveDispDiag.h"
#include "CDSReset.h"

#include <exception>

using namespace std;

void SetupDeviceModeCommandLine(CTestRunner& Runner, const wchar_t* szPath)
{

    Runner.DescribeCommandLineUsage(L"height:XXXX",
                                    L"The height of the primary surface");
    UINT nHeight = 0;
    if(Runner.IsCmdLineSet(L"height"))
    {
        const std::vector< std::wstring >& Values = Runner.GetCmdLineValues(L"height");

        if(Values.size() > 0)
        {
            nHeight = FromString<UINT>(Values[0].c_str());

            if(nHeight == 0)
            {
                throw std::exception("0 is an invalid height");
            }
        }
    }

    UINT nWidth = 0;
    Runner.DescribeCommandLineUsage(L"width:XXXX",
                                L"The width of the primary surface");
    if(Runner.IsCmdLineSet(L"width"))
    {
        const std::vector< std::wstring >& Values = Runner.GetCmdLineValues(L"width");

        if(Values.size() > 0)
        {
            nWidth = FromString<UINT>(Values[0].c_str());

            if(nWidth == 0)
            {
                throw std::exception("0 is an invalid width");
            }
        }
    }

    if( !nWidth && !nHeight )
    {
        std::vector<UINT> vHeight;
        vHeight.push_back(600);
        vHeight.push_back(720); // for HDTV monitor
        vHeight.push_back(768);
        vHeight.push_back(1024);
        vHeight.push_back(1080); // for HDTV monitor
        vHeight.push_back(1280);

        Runner.SetParamTestDomainSingleValue<UINT>(szPath, L"Width", nWidth);
        Runner.SetParamTestDomain<UINT>(szPath, L"Height", vHeight.begin(), vHeight.end());
    }
    else if( nWidth && !nHeight )
    {
        std::vector<UINT> vHeight;
        vHeight.push_back(600);
        vHeight.push_back(720); // for HDTV monitor
        vHeight.push_back(768);
        vHeight.push_back(1024);
        vHeight.push_back(1080); // for HDTV monitor
        vHeight.push_back(1280);

        Runner.SetParamTestDomainSingleValue<UINT>(szPath, L"Width", nWidth);
        Runner.SetParamTestDomain<UINT>(szPath, L"Height", vHeight.begin(), vHeight.end());
    }
    else if( !nWidth && nHeight )
    {
        std::vector<UINT> vWidth;
        vWidth.push_back(800);
        vWidth.push_back(1024);
        vWidth.push_back(1280);
        vWidth.push_back(1600);
        vWidth.push_back(1920); // for HDTV monitor

        Runner.SetParamTestDomain<UINT>(szPath, L"Width", vWidth.begin(), vWidth.end());
        Runner.SetParamTestDomainSingleValue<UINT>(szPath, L"Height", nHeight);
    }
    else
    {
        Runner.SetParamTestDomainSingleValue<UINT>(szPath, L"Width", nWidth);
        Runner.SetParamTestDomainSingleValue<UINT>(szPath, L"Height", nHeight);
    }

    bool bParamSet = true;
    Runner.DescribeCommandLineUsage(L"Rotation:XXX",
                                    L"The rotation of the display");
    if(Runner.IsCmdLineSet(L"Rotation"))
    {
        const std::vector< std::wstring >& Values = Runner.GetCmdLineValues(L"Rotation");
        if(Values.size() > 0)
        {
            UINT nRotation = FromString<UINT>(Values[0].c_str());

            if((nRotation != 0) && (nRotation != 90) && (nRotation != 180) && (nRotation != 270))
            {
                throw std::exception("%d is not a valid Rotation", nRotation);
            }
            Runner.SetParamTestDomainSingleValue<UINT>(szPath, L"Rotation", nRotation);
            bParamSet = false; // Don't have to set Rotation parameter 
        }
    }

    Runner.DescribeCommandLineUsage(L"Freq:XX",
                                    L"Display Frequency");
    UINT nFreq = 0;
    if(Runner.IsCmdLineSet(L"Freq"))
    {
        const std::vector< std::wstring >& Values = Runner.GetCmdLineValues(L"Freq");
        if(Values.size() > 0)
        {
            nFreq = FromString<UINT>(Values[0].c_str());

            if( (nFreq < 60) || (nFreq > 200))
            {
                throw std::exception("%d is not a valid Freq", nFreq);
            }
           Runner.SetParamTestDomainSingleValue<UINT>(szPath, L"Freq", nFreq);
        }
    }
}


CCDSReset::CCDSReset()
{
    SetDescription(L"This component change the configuration using ChangeDisplaySettingEx(CDS_RESET)");
    
    AddParam<UINT>(L"Width", L"Width");
    AddParam<UINT>(L"Height", L"Height");
    AddParam<UINT>(L"Freq", L"Freq");
    AddParam<UINT>(L"Rotation", L"Rotation");

    std::vector<UINT> vFreq;
    vFreq.push_back(60);
    vFreq.push_back(75);
    SetParamTestDomain<UINT>(L"Freq", vFreq.begin(), vFreq.end());

    std::vector<UINT> vRotation;
    vRotation.push_back(90);
    vRotation.push_back(0); // Should get back to default mode
    SetParamTestDomain<UINT>(L"Rotation", vRotation.begin(), vRotation.end());
    srand((unsigned)time(NULL));
}

CCDSReset::~CCDSReset()
{
}

void CCDSReset::PreWalk(CLogWriter* pLog)
{

    m_pLog = pLog;
    m_ModeChange = false;
    m_FailResult = false;
    // Clean up the Display modes list from previous tests
    m_AdapterName.clear();
    m_Modes.clear();
    
    const UINT nWidth = GetParam<UINT>(L"Width");
    
    const UINT nHeight = GetParam<UINT>(L"Height");

    const UINT nFreq = GetParam<UINT>(L"Freq");

    const UINT nRotation = GetParam<UINT>(L"Rotation");
    
    // Exclude the DISPLAY_DEVICE_DISCONNECT already
    GetAttachAdapterNames(m_AdapterName);

    //A random Monitor should be OK
    UINT nMonitor = rand() % m_AdapterName.size();

    std::wstring &AdapterName = m_AdapterName[nMonitor];
    DISPLAY_DEVICE AdapterInfo = {0};
    AdapterInfo.cb = sizeof(AdapterInfo);
    std::wstring Buff = L"EnumDisplayDevices on Display device ";  
    Buff += AdapterName.c_str();
    UINT NumAttachMonitors = 0;
    for( UINT nMonitor = 0; EnumDisplayDevices(AdapterName.c_str(), nMonitor, &AdapterInfo, 0); nMonitor++)
    {
        if(!(AdapterInfo.StateFlags & DISPLAY_DEVICE_ACTIVE))
        {
            continue;
        }

        ++NumAttachMonitors;
    }

    if( !NumAttachMonitors )
    {
        Buff +=  L" found no active monitor";
        Buff += L", DISPLAY_DEVICE_ACTIVE bit was not set. Skip test";
        pLog->Skip(Buff.c_str());
        return;
    }


    // Get all the display modes that has dmDisplayFrequency equal 60Hz or 75Hz
    GetDisplayModes(AdapterName.c_str(), nFreq, m_Modes );

    std::wstring tszBuff( AdapterName.c_str());
    tszBuff += L" Display mode:";

    if(nWidth)
        tszBuff += ToString<UINT>(nWidth);
    else
        tszBuff += L"XXXX";

    tszBuff += L"x";

    if(nHeight)
    {
        tszBuff += ToString<UINT>(nHeight);
    }
    else
    {
        tszBuff += L"XXXX";
    }

    tszBuff += L" ";
    tszBuff += ToString<UINT>(nFreq);
    tszBuff += L"hz";

    tszBuff += L" Rotation = ";
    tszBuff += ToString<UINT>(nRotation);

    if(!m_Modes.size())
    {
        tszBuff += L"Test DEVMODE is not supported, skip test";
        pLog->Skip(tszBuff.c_str());
        return;
    }

    // This call will return -1 if the display device do not support width/height/BPP/Rotation
    UINT nModeIndex = FindDisplayModeIndex(m_Modes, nWidth, nHeight, nFreq, nRotation);

    if( nModeIndex >= m_Modes.size())
    {

        tszBuff += L" is not supported, skip test";
        pLog->Skip(tszBuff.c_str());
        return;
    }
   
    DEVMODE &Mode = m_Modes[nModeIndex];
    // Initialise return code from SetDisplayMode (ChangeDisplaySettingsEx)
    DWORD dwFlags = CDS_UPDATEREGISTRY;

    SetDisplayModeResult RetStatus = DisplayModeNotSupported;

    LogMessageF( pLog, L"ChangeDisplaySettingsEx (dwflags=CDS_UPDATEREGISTRY), DEVMODE: ",
        tszBuff.c_str());

    RetStatus = SetDisplayMode( pLog, AdapterName.c_str(), &Mode, dwFlags );

    if(DisplayModeSet != RetStatus)
    {
        pLog->Fail(L"!!! ChangeDisplaySettingsEx (dwflags=CDS_UPDATEREGISTRY) failed");
        m_FailResult = true;
    }

    // Wait for the system to be stable after the display mode change
    if( DisplayModeSet == RetStatus)
    {
        m_ModeChange = true;
        Sleep(500);
    }
}

void CCDSReset::PostWalk(CLogWriter* pLog)
{
    m_AdapterName.clear();
    m_Modes.clear();

    if(m_FailResult)
    {
        CSaveDispDiagData SaveDispDiag(pLog);
        SaveDispDiag.SaveDispDiagData();
    }  
}



// Set the testing display mode
SetDisplayModeResult CCDSReset::SetDisplayMode(CLogWriter* pLog, 
                                                      const wchar_t* AdapterName, DEVMODE* Mode, UINT dwflags)
{

    std::wstring AdapterString = PdbFormatDisplayMode(AdapterName, Mode);

    LONG lRet = ChangeDisplaySettingsEx(AdapterName,
                                        Mode,
                                        NULL,
                                        dwflags,
                                        NULL);
    
    if(DISP_CHANGE_SUCCESSFUL == lRet)
    {
        // Log Display mode in test
        LogMessageF( pLog, AdapterString.c_str() );
        return DisplayModeSet;
    }
    else if(DISP_CHANGE_BADDUALVIEW == lRet)
    {
        LogMessageF( pLog, L"!!! %ls ChangeDisplaySettingsEx returned DISP_CHANGE_BADDUALVIEW", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
     else if(DISP_CHANGE_BADFLAGS == lRet)
    {
        LogMessageF( pLog, L"!!! %ls ChangeDisplaySettingsEx returned DISP_CHANGE_BADFLAGS", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
    else if(DISP_CHANGE_BADMODE == lRet)
    {
        LogMessageF( pLog, L"!!! %ls ChangeDisplaySettingsEx returned DISP_CHANGE_BADMODE", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
    else if(DISP_CHANGE_BADPARAM == lRet)
    {
        LogMessageF( pLog, L"!!! %ls ChangeDisplaySettingsEx returned DISP_CHANGE_BADPARAM", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
    else if(DISP_CHANGE_FAILED == lRet)
    {
        LogMessageF( pLog, L"!!! %ls ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
    else if(DISP_CHANGE_NOTUPDATED == lRet)
    {
        LogMessageF( pLog, L"!!! %ls ChangeDisplaySettingsEx returned DISP_CHANGE_NOTUPDATED", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
    else if(DISP_CHANGE_RESTART == lRet)
    {
        LogMessageF( pLog, L"!!! %ls ChangeDisplaySettingsEx returned DISP_CHANGE_RESTART", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
   else
    {
        LogMessageF( pLog, L"!!! %ls ChangeDisplaySettingsEx returned %08Xh", AdapterString.c_str(), lRet );
        return DisplayModeFailed;
    } 
}


// This call will return -1 if the display device do not support width/height/BPP/Rotation
UINT FindDisplayModeIndex(std::vector<DEVMODE> &Modes, UINT nWidth, UINT nHeight, UINT nFreq, UINT nRotation)
{
    UINT ModeIndex = 0xFFFFFFFF;
    UINT Rotation = DMDO_DEFAULT;
    UINT uWidth = nWidth;
    UINT uHeight = nHeight;

    // for 90 and 270 rotation mode. The primary width is the GDI mode height and vice versa
    if( nRotation == 0 )
    {
        Rotation = DMDO_DEFAULT;
    }
    else if(nRotation == 90)
    {
        Rotation = DMDO_90;
        uWidth = nHeight;
        uHeight = nWidth;
    }
    else if(nRotation == 180)
    {
        Rotation = DMDO_180;
    }
    else 
    {
        Rotation = DMDO_270;
        uWidth = nHeight;
        uHeight = nWidth;
    }

    for( UINT Index = 0; Index < Modes.size(); Index++ )
    {
        DEVMODE& Mode = Modes[Index];

        if(Mode.dmBitsPerPel != 32 )
        {
            continue;
        }

        // only look for these values
        if(nWidth && nHeight)
        {

            if( (uWidth == Mode.dmPelsWidth ) && 
                (uHeight == Mode.dmPelsHeight ) && 
                ( nFreq == Mode.dmDisplayFrequency) &&
                (Rotation == Mode.dmDisplayOrientation))
            {
                ModeIndex = Index;
                break;
            }
        }
        // If the height is not specified, use the width, Freq and rotation to find matching height
        else if( uWidth && !uHeight)
        {
            
            if( (uWidth == Mode.dmPelsWidth ) && 
            ( nFreq == Mode.dmDisplayFrequency) &&
            (Rotation == Mode.dmDisplayOrientation))
            {
                ModeIndex = Index;
                break;
            }
        }

        // There is no !uWidth && !uHeight condition
        // If the width is not specified, use the height, Freq and rotation to find matching width
        else    //( !uWidth && uHeight)
        {
            if( (uHeight == Mode.dmPelsHeight ) && 
            ( nFreq == Mode.dmDisplayFrequency) &&
            (Rotation == Mode.dmDisplayOrientation))
            {
                ModeIndex = Index;
                break;
            }
        }
    }

    return ModeIndex;
}
