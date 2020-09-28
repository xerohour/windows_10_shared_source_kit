/******************************Module*Header*******************************
 * Module Name: SDCReset.cpp
 *
 * Implementations of reseting system configuration test component using 
 * SetDisplayConfig
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
#include "CDSReset.h"
#include "SaveDispDiag.h"
#include "SDCReset.h"

#include <exception>

using namespace std;

CSDCReset::CSDCReset()
{
SetDescription(L"This component tests the persistent data base due to SetDisplayConfig (SDC_APPLY|SDC_USE_DATABASE_CURRENT). \
First, the system is reset to the configuration saved in the database. Second, the test calls SDC \
(SDC_USE_SUPPLIED_DISPLAY_CONFIG|SDC_APPLY|SDC_ALLOW_CHANGES) to to change a display mode without saving configuration \
data. Third, the test call SDC again to retrieve the system configuration. Configuration data after step 1, and 3 should compared");
    
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
}

CSDCReset::~CSDCReset()
{
}

void CSDCReset::PreWalk(CLogWriter* pLog)
{

    m_pLog = pLog;
    m_FailResult = false;

    m_ModeChange = false;
    m_Modes.clear();
    m_AdapterName.clear();
    // Clean up the Display modes list from previous tests

    // Change the system to what are in Persistent database
    LONG lRet = CcdSetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY| SDC_USE_DATABASE_CURRENT);
    if( lRet != S_OK)
    {
        // Log Display mode in test
        LogFailF( pLog, L"CcdSetDisplayConfig(SDC_APPLY| SDC_USE_DATABASE_CURRENT) failed, return code = %08Xh", lRet );
        m_FailResult = true;
        return;
    }
    else
    {
        LogMessageF( pLog, L"Call CcdSetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY| SDC_USE_DATABASE_CURRENT) to change the system to what are in Persistent database was OK" );
    }

    const UINT nWidth = GetParam<UINT>(L"Width");
    
    const UINT nHeight = GetParam<UINT>(L"Height");

    const UINT nFreq = GetParam<UINT>(L"Freq");

    const UINT nRotation = GetParam<UINT>(L"Rotation");
    
    //A random Monitor should be enough

    m_AdapterName.clear();
    m_Modes.clear();

    GetAttachAdapterNames(m_AdapterName);

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

    LogMessageF( pLog, L"Use SDC to set display mode: %ls", tszBuff.c_str() );
    
    
    if( SetDisplayModes(pLog, AdapterName.c_str(), &Mode) )
    {
        m_ModeChange = true;
        Sleep(1000);
    }
}

void CSDCReset::PostWalk(CLogWriter* pLog)
{
    m_AdapterName.clear();
    m_Modes.clear();
  
    if(m_ModeChange)
    {
        // Reset the system to what are in Persistent database
        // Change the system to what are in Persistent database
        LogMessageF( pLog, L"Call CcdSetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY| SDC_USE_DATABASE_CURRENT) to change the system to what are in Persistent database was OK" );
        LONG lRet = CcdSetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY| SDC_USE_DATABASE_CURRENT);
        if( lRet != S_OK)
        {
            // Log Display mode in test
            LogFailF( pLog, L"!!! CcdSetDisplayConfig(SDC_APPLY| SDC_USE_DATABASE_CURRENT) failed, return code = %08Xh", lRet );
            m_FailResult = true;
        }
    }

    if(m_FailResult)
    {
        CSaveDispDiagData SaveDispDiag(pLog);
        SaveDispDiag.SaveDispDiagData();
    }
}

bool CSDCReset::SetDisplayModes(CLogWriter* pLog, std::wstring AdapterName, DEVMODE *Mode)
{
    bool Status = true;
    LONG lResult = E_FAIL;
    UINT32 numPaths;
    UINT32 numModes;

    std::wstring AdapterString = PdbFormatDisplayMode(AdapterName.c_str(), Mode);
    LogMessageF( pLog, L"Change to a temporary DEVMODE using SetDisplayConfig, DEVMODE: %ls", AdapterString.c_str() );
    
    lResult = CcdGetDisplayConfigBufferSizes(QDC_ALL_PATHS, &numPaths, &numModes);
    
    if(lResult == ERROR_SUCCESS)
    {
        UINT32 numPathArrayElements = numPaths;
        UINT32 numModeInfoArrayElements = numModes;

        if( !numPaths || !numModes )
        {
            pLog->Fail(L"!!! CcdGetDisplayConfigBufferSizes found zero paths or zero modes");
            m_FailResult = true;
            return false;
        }
        DISPLAYCONFIG_PATH_INFO* pathArray = new DISPLAYCONFIG_PATH_INFO[numPaths];
        DISPLAYCONFIG_MODE_INFO* modeInfoArray = new DISPLAYCONFIG_MODE_INFO[numModes];
        DISPLAYCONFIG_PATH_INFO *SDCPathArray = new DISPLAYCONFIG_PATH_INFO[numPaths];
        UINT32 numSDCPaths = 0;

        if(!pathArray || !modeInfoArray || !SDCPathArray)
        {
            pLog->Fail(L"!!! Out of Memory");
            m_FailResult = true;
            return false;
        }

        UINT32 Flags = QDC_ONLY_ACTIVE_PATHS;
        lResult = CcdQueryDisplayConfig(Flags, &numPathArrayElements, pathArray, 
            &numModeInfoArrayElements, modeInfoArray, NULL);	
        
        bool ChangeMode = false;
        if(lResult == ERROR_SUCCESS)
        {
            std::wstring QDCBuff = SDCDataToString(numPathArrayElements,
                pathArray,
                numModeInfoArrayElements,
                modeInfoArray,
                Flags);

            LogMessageF(pLog, L"CcdQueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS) data: %ls", QDCBuff.c_str());

            for( UINT pIndex =0; pIndex < numPathArrayElements; pIndex++ )
            {
                if(ChangeMode)
                {
                    break;
                }
                // Change dev mode
                //Get the name of the source corresponding to this index.                
                DISPLAYCONFIG_PATH_SOURCE_INFO *sInfo = &pathArray[pIndex].sourceInfo;
                DISPLAYCONFIG_PATH_TARGET_INFO *tInfo = &pathArray[pIndex].targetInfo;

                DISPLAYCONFIG_MODE_INFO *sMode = &modeInfoArray[sInfo->modeInfoIdx]; 
                DISPLAYCONFIG_SOURCE_MODE   *pSourceMode = &(sMode->sourceMode);

                DISPLAYCONFIG_SOURCE_DEVICE_NAME    SourceDeviceName;
                SourceDeviceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
                SourceDeviceName.header.size = sizeof(SourceDeviceName);
                SourceDeviceName.header.adapterId = pathArray[pIndex].sourceInfo.adapterId;
                SourceDeviceName.header.id = pathArray[pIndex].sourceInfo.id;

                lResult = DisplayConfigGetDeviceInfo(&SourceDeviceName.header);

                if(lResult != ERROR_SUCCESS)
                {

                    wcout << L"!!! DisplayConfigGetDeviceInfo failed";
                    LOG_STATUS(lResult, L"DisplayConfigGetDeviceInfo");
                    m_FailResult = true;
                }
                else
                {
                    if( !wcscmp(SourceDeviceName.viewGdiDeviceName, AdapterName.c_str()) )
                    {
                        // wcout << L"Found viewGdiDeviceName for " << AdapterName.c_str() << endl;

                        DEVMODE devMode;
                        ZeroMemory(&devMode, sizeof(devMode));
                        devMode.dmSize  = sizeof(devMode);
                        
                        // Find the matching mode
                        for(UINT  mIndex=0; EnumDisplaySettingsEx( SourceDeviceName.viewGdiDeviceName, 
                            mIndex, &devMode, EDS_ROTATEDMODE ); mIndex++)
                        {
                            if(ChangeMode)
                            {
                                break;
                            }
                            if(devMode.dmPelsWidth != Mode->dmPelsWidth)
                            {
                                continue;
                            }
                            if(devMode.dmPelsHeight != Mode->dmPelsHeight)
                            {
                                continue;
                            }

                            if(devMode.dmDisplayOrientation != Mode->dmDisplayOrientation)
                            {
                                continue;
                            }

                            if(devMode.dmDisplayFrequency != Mode->dmDisplayFrequency)
                            {
                                continue;
                            }
                            
                            if(devMode.dmBitsPerPel != Mode->dmBitsPerPel)
                            {
                                continue;
                            }


                            ChangeMode = true;

                            pSourceMode->position.x = devMode.dmPosition.x;
                            pSourceMode->position.y = devMode.dmPosition.y;
                            pSourceMode->pixelFormat = (DISPLAYCONFIG_PIXELFORMAT)(devMode.dmBitsPerPel/8);
                            pSourceMode->width = devMode.dmPelsWidth;
                            pSourceMode->height = devMode.dmPelsHeight;
                            pathArray[pIndex].targetInfo.rotation = ConvertGdiRotationToRotation(devMode.dmDisplayOrientation);
                            pathArray[pIndex].targetInfo.scaling = ConvertGdiScalingToScaling(devMode.dmDisplayFixedOutput);

                            // Find and change info of all path to this Source 
                            for( UINT Index =0; Index < numPathArrayElements; Index++ )
                            {
                                if( (pathArray[Index].sourceInfo.adapterId == pathArray[pIndex].sourceInfo.adapterId) &&
                                    (pathArray[Index].sourceInfo.id == pathArray[pIndex].sourceInfo.id) )
                                {
                                    UINT SourceModeIndex = pathArray[Index].sourceInfo.modeInfoIdx;
                                    DISPLAYCONFIG_MODE_INFO *CloneSrcMode = &modeInfoArray[SourceModeIndex]; 
                                    DISPLAYCONFIG_SOURCE_MODE   *CloneSourceMode = &(CloneSrcMode->sourceMode);

                                    CloneSourceMode->position.x = devMode.dmPosition.x;
                                    CloneSourceMode->position.y = devMode.dmPosition.y;
                                    CloneSourceMode->pixelFormat = (DISPLAYCONFIG_PIXELFORMAT)(devMode.dmBitsPerPel/8);
                                    CloneSourceMode->width = devMode.dmPelsWidth;
                                    CloneSourceMode->height = devMode.dmPelsHeight;
                                    pathArray[Index].targetInfo.rotation = ConvertGdiRotationToRotation(devMode.dmDisplayOrientation);
                                    pathArray[Index].targetInfo.scaling = ConvertGdiScalingToScaling(devMode.dmDisplayFixedOutput);
                                }
                            }

                            // Let BTL figure out all other target mode 
                            for( UINT Index =0; Index < numPathArrayElements; Index++ )
                            {
                                DISPLAYCONFIG_SOURCE_DEVICE_NAME    SrcDevName;
                                SrcDevName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
                                SrcDevName.header.size = sizeof(SourceDeviceName);
                                SrcDevName.header.adapterId = pathArray[pIndex].sourceInfo.adapterId;
                                SrcDevName.header.id = pathArray[pIndex].sourceInfo.id;

                                lResult = DisplayConfigGetDeviceInfo(&SrcDevName.header);

                                if(lResult != ERROR_SUCCESS)
                                {

                                    wcout << L"!!! DisplayConfigGetDeviceInfo failed";
                                    LOG_STATUS(lResult, L"DisplayConfigGetDeviceInfo");
                                    m_FailResult = true;
                                }
                                else
                                {
                                    if( wcscmp(SrcDevName.viewGdiDeviceName, AdapterName.c_str()) )
                                    {                             
                                        pathArray[Index].sourceInfo.modeInfoIdx = DISPLAYCONFIG_PATH_MODE_IDX_INVALID;
                                    }
                                }

                                pathArray[Index].targetInfo.modeInfoIdx = DISPLAYCONFIG_PATH_MODE_IDX_INVALID;
                            }

                            Flags = SDC_USE_SUPPLIED_DISPLAY_CONFIG | SDC_VALIDATE| SDC_ALLOW_CHANGES;
                            lResult = CcdSetDisplayConfig(numPathArrayElements,
                                pathArray,
                                numModeInfoArrayElements,
                                modeInfoArray,
                                Flags);

                            if(lResult == ERROR_SUCCESS)
                            {
                                Flags = SDC_USE_SUPPLIED_DISPLAY_CONFIG | SDC_APPLY | SDC_ALLOW_CHANGES;
                                std::wstring Buff = SDCDataToString(numPathArrayElements,
                                    pathArray,
                                    numModeInfoArrayElements,
                                    modeInfoArray,
                                    Flags);

                                LogMessageF(pLog, L"SetDisplayConfig data: %ls", Buff.c_str());

                                lResult = CcdSetDisplayConfig(numPathArrayElements,
                                    pathArray,
                                    numModeInfoArrayElements,
                                    modeInfoArray,
                                    Flags);

                                if(lResult != ERROR_SUCCESS)
                                {

                                    wcout << L"!!! CcdSetDisplayConfig(" << SDCFlagsToString(Flags) << L") failed" << endl;
                                    LOG_STATUS(lResult, L"CcdSetDisplayConfig");
                                    m_FailResult = true;
                                }
                                else
                                {
                                    Sleep(1000);
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            wcout << L"!!! CcdQueryDisplayConfig failed";
            LOG_STATUS(lResult, L"CcdQueryDisplayConfig");
            m_FailResult = true;
            Status = false;
        }

        delete [] pathArray;

        delete [] modeInfoArray;

        delete [] SDCPathArray;

    }
    else
    {
        wcout << L"!!! CcdGetDisplayConfigBufferSizes failed";
        LOG_STATUS(lResult,L"CcdGetDisplayConfigBufferSizes");
        m_FailResult = true;
        Status = false;
    }

    return true;

}

CAdapterInfo::CAdapterInfo()
{
   SetDescription(L"This component logs number of sources and targets of all the display adapters");

}

void CAdapterInfo::PreWalk(CLogWriter* pLog)
{
    bool bStatus = false;

    std::vector<QdcAdapterSets> QdcAdapterSets;

    // Initialize m_QdcAdapterSets data
    CleanupQdcAdapterSets(QdcAdapterSets);

    // Format the CCD info
    std::wstring Wss = AdapterSetsFromQDC( QdcAdapterSets, QDC_ALL_PATHS, &bStatus);
    if(!bStatus)
    {
        LogMessageF( pLog, Wss.c_str());
        wcout <<L"AdapterSetsFromQDC failed\n" << endl;
        wcout << Wss.c_str() << endl;
    }
    else
    {
        Wss.resize(0);
        for( UINT aIndex = 0; aIndex < QdcAdapterSets.size(); aIndex++ )
        {
            LUID adapterLuid = QdcAdapterSets[aIndex].adapterLuid;
            Wss +=  L"\n  Display Adapter.AdapterLuid ";
            Wss += LUIDToHex(adapterLuid);
            Wss +=  L"\n    Number of sources: ";
            Wss += ToString(QdcAdapterSets[aIndex].numSource);
            Wss +=  L"\n    Number of targets: ";
            Wss += ToString(QdcAdapterSets[aIndex].numTarget);
            for( UINT sIndex = 0; sIndex < QdcAdapterSets[aIndex].sourceIds.size(); sIndex++ )
            {
                ULONG  sourceId = QdcAdapterSets[aIndex].sourceIds[sIndex];
                bool GetGdiStatus = true;
                std::wstring GdiDeviceName = GetGdiDeviceName(pLog, adapterLuid, sourceId, &GetGdiStatus);
                if( GetGdiStatus )
                {
                    Wss +=  L"\n    SourceID: ";
                    Wss += ToHex( sourceId );
                    Wss +=  L"\n    GdiDeviceName: ";
                    Wss += GdiDeviceName.c_str();
                }    
            }
        }
    
        LogMessageF( pLog, Wss.c_str());
        CleanupQdcAdapterSets(QdcAdapterSets);
    }

}

void CAdapterInfo::PostWalk(CLogWriter* pLog)
{

}

std::wstring CAdapterInfo::GetGdiDeviceName(CLogWriter* pLog,
    LUID        AdapterLuid,
    UINT		SourceId,
    bool		*Status
)
{

    *Status = false;
    std::wstring Wss(L"");

    DISPLAYCONFIG_SOURCE_DEVICE_NAME    SourceDeviceName;
    SourceDeviceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
    SourceDeviceName.header.size = sizeof(SourceDeviceName);
    SourceDeviceName.header.adapterId = AdapterLuid;
    SourceDeviceName.header.id = SourceId;

    LONG lResult = DisplayConfigGetDeviceInfo(&SourceDeviceName.header);

    if(lResult == ERROR_SUCCESS)
    {
        Wss = SourceDeviceName.viewGdiDeviceName;
        *Status = true;
    }
    else
    {
        // Need m_pLog defined for LOG_STATUS
        CLogWriter* m_pLog = pLog;
        LOG_STATUS(lResult, L"DisplayConfigGetDeviceInfo");
    }

    return Wss.c_str();

}