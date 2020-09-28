/******************************Module*Header*******************************
 * Module Name: CDSReset.h
 *
 * Header for Reseting system configuration test component using ChangeDisplaySetting
 *
 * Author: Mai Dao [maithida]
 *
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 **************************************************************************/
#ifndef __TEST_CDS_RESET_H_
#define __TEST_CDS_RESET_H_

#include <windows.h>
#include "TestComponent.h"
#include "PDBCcdConfig.h"

// The possible results when attempting to set a display mode
enum SetDisplayModeResult
{
    DisplayModeNotSupported = 0,    //the specified mode is not supported by the adapter
    DisplayModeSet = 1,             //the mode change was succesful
    DisplayModeFailed = 2,          //the mode change failed
};


void SetupDeviceModeCommandLine(CTestRunner& Runner, const wchar_t* szPath);

UINT FindDisplayModeIndex(std::vector<DEVMODE> &Modes, UINT nWidth, UINT nHeight, UINT nFreq, UINT nRotation);

class CCDSReset : public CTestComponent
{
public:
    CCDSReset();
    ~CCDSReset();

protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);

private:
    CLogWriter* m_pLog;
    bool m_FailResult;
    // Saves the adapter names
    std::vector<std::wstring> m_AdapterName;
    // Saves display mode that support by the system, and specify by the command line
    std::vector<DEVMODE> m_Modes;
    bool m_ModeChange;

    // Set display modes
    SetDisplayModeResult SetDisplayMode(CLogWriter* pLog,
        const wchar_t* AdapterName, DEVMODE* Mode, UINT dwflags);
};

#endif
