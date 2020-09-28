/******************************Module*Header*******************************
 * Module Name: CDSDevMode.h
 *
 * Header for test component using ChangeDisplaySetting to change current DEVMODE
 *
 * Author: Mai Dao [maithida]
 *
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 **************************************************************************/
#ifndef __TEST_CDS_DEVMODE_H_
#define __TEST_CDS_DEVMODE_H_

#include <windows.h>
#include "TestComponent.h"
#include "PDBCcdConfig.h"
#include "CDSReset.h"

#define CHANGE_DIMENTION 1
#define CHANGE_ROTATION 2
#define CHANGE_REQUENCY 4

void SetupChangeDevModeCommandLine(CTestRunner& Runner, const wchar_t* szPath);

class CCDSDevMode : public CTestComponent
{
public:
    CCDSDevMode();
    ~CCDSDevMode();

protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);

private:
    void GetDisplayModes(const wchar_t* AdapterName, std::vector<DEVMODE>& DisplayMode);
    UINT FindDisplayModeIndex(const wchar_t* AdapterName, std::vector<DEVMODE> &Modes, UINT Change);
    SetDisplayModeResult SetDisplayMode(CLogWriter* pLog, 
                                                      const wchar_t* AdapterName, DEVMODE* Mode, UINT dwflags);
    CLogWriter* m_pLog;
    bool m_FailResult;
    // Saves the adapter names
    std::vector<std::wstring> m_AdapterName;
    // Saves display mode that support by the system, and specify by the command line
    std::vector<DEVMODE> m_Modes;

};

#endif
