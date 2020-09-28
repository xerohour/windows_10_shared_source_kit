/******************************Module*Header*******************************
 * Module Name: CDSDevMode.cpp
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
#include "CDSDevMode.h"

#include <exception>

using namespace std;

void SetupChangeDevModeCommandLine(CTestRunner& Runner, const wchar_t* szPath)
{
    Runner.DescribeCommandLineUsage(L"Change:Dimension|Rotation|Frequency",
                                    L"The device data that will be changed");
    UINT nChange = 0;

    if(Runner.IsCmdLineSet(L"Change"))
    {
        const std::vector< std::wstring >& Values = Runner.GetCmdLineValues(L"Change");

        if(Values.size() > 0)
        {
            std::wstring sLowerCase = ToLowerCase(Values[0]);

            if( sLowerCase == L"dimension" )
            {
                nChange = CHANGE_DIMENTION;
            }
            else if( sLowerCase == L"rotation" )
            {
                nChange = CHANGE_ROTATION;
            }
            else if( sLowerCase == L"frequency" )
            {
                nChange = CHANGE_REQUENCY;
            }
            else
            {
                sLowerCase += L": Unknown change option";
                wcout << sLowerCase.c_str() << endl;

                throw std::exception("Unknown change option");
            }

            Runner.SetParamTestDomainSingleValue<UINT>(szPath, L"Change", nChange);
        }
    }
    
    Runner.DescribeCommandLineUsage(L"NoUpdate",
                                    L"Not update database");

    if(Runner.IsCmdLineSet(L"NoUpdate"))
    {
        Runner.SetParamTestDomainSingleValue<UINT>(szPath, L"NoUpdate", true);
    }
}

CCDSDevMode::CCDSDevMode()
{
    SetDescription(L"This component change the configuration using ChangeDisplaySettingEx(CDS_RESET)");
    
    AddParam<UINT>(L"Change", L"Change");

    std::vector<UINT> vChange;
    vChange.push_back(CHANGE_DIMENTION);
    vChange.push_back(CHANGE_ROTATION);
    vChange.push_back(CHANGE_REQUENCY);
    SetParamTestDomain<UINT>(L"Change", vChange.begin(), vChange.end());
    
    AddParam<bool>(L"NoUpdate", L"NoUpdate");
    SetParamTestDomainSingleValue<bool>(L"NoUpdate", true);
}
CCDSDevMode::~CCDSDevMode()
{
}

void CCDSDevMode::PreWalk(CLogWriter* pLog)
{

    m_pLog = pLog;
    m_FailResult = false;
    // Clean up the Display modes list from previous tests
    m_AdapterName.clear();
    m_Modes.clear();
    
    const UINT nChange = GetParam<UINT>(L"Change");

    const bool bNoUpdate = GetParam<bool>(L"NoUpdate");
     
    // Exclude the DISPLAY_DEVICE_DISCONNECT already
    GetAttachAdapterNames(m_AdapterName);

    //First Monitor should be OK
    UINT nMonitor = 0;

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
    GetDisplayModes(AdapterName.c_str(), m_Modes );

    if(!m_Modes.size())
    {
        pLog->Skip(L"Test DEVMODEs is not supported, skip test");
        return;
    }

    // This call will return -1 if the display device do not support width/height/BPP/Rotation
    UINT nModeIndex = FindDisplayModeIndex(AdapterName.c_str(), m_Modes, nChange);

    if( nModeIndex >= m_Modes.size())
    {
        pLog->Skip(L"Test DEVMODEs is not supported, skip test");
        return;
    }
   
    DEVMODE &Mode = m_Modes[nModeIndex];
    // Initialise return code from SetDisplayMode (ChangeDisplaySettingsEx)
    Buff.resize(0);
    DWORD dwFlags = CDS_UPDATEREGISTRY;
    if(bNoUpdate)
    {
        Buff += L"dwflags=0";
        dwFlags = 0;
    }
    else
    {
        Buff += L"dwflags=CDS_UPDATEREGISTRY";
    }

    SetDisplayModeResult RetStatus = DisplayModeNotSupported;

    LogMessageF( pLog, L"ChangeDisplaySettingsEx (dwflags=CDS_UPDATEREGISTRY), DEVMODE: ",
        Buff.c_str());
    RetStatus = SetDisplayMode( pLog, AdapterName.c_str(), &Mode, dwFlags );

    if(DisplayModeSet != RetStatus)
    {
        pLog->Fail(L"!!! ChangeDisplaySettingsEx (%ls) failed", Buff.c_str());
        m_FailResult = true;
    }

    // Wait for the system to be stable after the display mode change
    if( DisplayModeSet == RetStatus)
    {
        Sleep(1000);
    }
}

void CCDSDevMode::PostWalk(CLogWriter* pLog)
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
SetDisplayModeResult CCDSDevMode::SetDisplayMode(CLogWriter* pLog, 
                                                      const wchar_t* AdapterName, DEVMODE* Mode, UINT dwflags)
{

    std::wstring AdapterString(L"ChangeDisplaySettingsEx: ");
    
    AdapterString += PdbFormatDisplayMode(AdapterName, Mode);

    LONG lRet = ChangeDisplaySettingsEx(AdapterName,
                                        Mode,
                                        NULL,
                                        dwflags,
                                        NULL);
    
    if(DISP_CHANGE_SUCCESSFUL == lRet)
    {
        // Log Display mode in test
        AdapterString += L" OK";
        pLog->Message( AdapterString.c_str() );
        return DisplayModeSet;
    }
    else if(DISP_CHANGE_BADDUALVIEW == lRet)
    {
        LogMessageF( pLog, L"%ls returned DISP_CHANGE_BADDUALVIEW", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
     else if(DISP_CHANGE_BADFLAGS == lRet)
    {
        LogMessageF( pLog, L"%ls returned DISP_CHANGE_BADFLAGS", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
    else if(DISP_CHANGE_BADMODE == lRet)
    {
        LogMessageF( pLog, L"%ls returned DISP_CHANGE_BADMODE", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
    else if(DISP_CHANGE_BADPARAM == lRet)
    {
        LogMessageF( pLog, L"%ls returned DISP_CHANGE_BADPARAM", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
    else if(DISP_CHANGE_FAILED == lRet)
    {
        LogMessageF( pLog, L"%ls returned DISP_CHANGE_FAILED", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
    else if(DISP_CHANGE_NOTUPDATED == lRet)
    {
        LogMessageF( pLog, L"%ls returned DISP_CHANGE_NOTUPDATED", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
    else if(DISP_CHANGE_RESTART == lRet)
    {
        LogMessageF( pLog, L"%ls returned DISP_CHANGE_RESTART", AdapterString.c_str() );
        return DisplayModeNotSupported;
    }
   else
    {
        LogMessageF( pLog, L"%ls returned %08Xh", AdapterString.c_str(), lRet );
        return DisplayModeFailed;
    } 
}


// This call will return -1 if the display device do not support width/height/BPP/Rotation
UINT CCDSDevMode::FindDisplayModeIndex(const wchar_t* AdapterName, std::vector<DEVMODE> &Modes, UINT Change)
{
    UINT ModeIndex = 0xFFFFFFFF;

    DEVMODE CurMode;
    memset(&CurMode, 0, sizeof(CurMode));
    CurMode.dmSize = sizeof(CurMode);
    if(EnumDisplaySettingsEx(AdapterName,
                        ENUM_CURRENT_SETTINGS,
                        &CurMode,
                        0))
    {
        std::wstring AdapterString(L"Current DEVMODE: ");
        AdapterString += PdbFormatDisplayMode(AdapterName, &CurMode);
        m_pLog->Message(AdapterString.c_str());

        const UINT nChange = GetParam<UINT>(L"Change");
        UINT StartBlock = 0;

        switch(nChange)
        {
        case CHANGE_DIMENTION:
            StartBlock = 2;
            break;
        case CHANGE_ROTATION:
            StartBlock = 1;
            break;
        case CHANGE_REQUENCY:
        default:
            StartBlock = 3;
            break;

        }


        // Start from the middle
        for( UINT Index = (StartBlock * Modes.size())/4; Index < Modes.size(); Index++ )
        {
            DEVMODE& Mode = Modes[Index];

            switch(Change)
            {
                case CHANGE_DIMENTION:
                    if( (CurMode.dmPelsWidth + CurMode.dmPelsHeight ) != (Mode.dmPelsWidth + Mode.dmPelsHeight ))
                    {
                        ModeIndex = Index;
                    }
                    break;
                case CHANGE_ROTATION:
                    if(CurMode.dmDisplayOrientation != Mode.dmDisplayOrientation)
                    {
                        ModeIndex = Index;
                    }
                    break;
                case CHANGE_REQUENCY:
                default:
                    if( CurMode.dmDisplayFrequency != Mode.dmDisplayFrequency) 
                    {
                        ModeIndex = Index;
                    }
                    break;
            }

            if(ModeIndex != 0xFFFFFFFF)
            {
                break;
            }
        }
    }
    else
    {
       m_pLog->Fail(L"!!! EnumDisplaySettingsEx(ENUM_CURRENT_SETTINGS, ... ) failed");

    }

    return ModeIndex;
}
void CCDSDevMode::GetDisplayModes(const wchar_t* AdapterName, std::vector<DEVMODE>& DisplayMode)
{
    DisplayMode.empty();
    
    DEVMODE DM;
    memset(&DM, 0, sizeof(DM));
    DM.dmSize = sizeof(DM);
        
    for(DWORD i = 0; EnumDisplaySettingsEx(AdapterName, i, &DM, EDS_ROTATEDMODE); i++)
    {
        // Skip the modes that are not 60 Hz or 75 Hz
        if((DM.dmDisplayFrequency != 60) && (DM.dmDisplayFrequency != 75)&&(DM.dmDisplayFrequency != 64) )
        {
            continue;
        }

        if( DM.dmBitsPerPel != 32 )
        {
            continue;
        }
        DisplayMode.push_back(DM);
    }
}
