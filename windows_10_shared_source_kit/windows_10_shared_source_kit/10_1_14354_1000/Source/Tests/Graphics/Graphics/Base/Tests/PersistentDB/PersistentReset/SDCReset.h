/******************************Module*Header*******************************
 * Module Name: SDCReset.h
 *
 * Header for Reseting system configuration test component using SetDisplayConfig
 *
 * Author: Mai Dao [maithida]
 *
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 **************************************************************************/
#ifndef __TEST_SDC_RESET_H_
#define __TEST_SDC_RESET_H_

#include <windows.h>
#include "TestComponent.h"
#include "PDBCcdConfig.h"
#include "PDBDisplay.h"
#include "PDBLogStatus.h"

class CSDCReset : public CTestComponent
{
public:
    CSDCReset();
    ~CSDCReset();

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

    bool SetDisplayModes(CLogWriter* pLog, std::wstring AdapterName, DEVMODE *devMode);
};

class CAdapterInfo : public CTestComponent
{
public:
    CAdapterInfo();

protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);

private:
    std::wstring CAdapterInfo::GetGdiDeviceName(CLogWriter* pLog,
        LUID        AdapterLuid,
        UINT		SourceId,
        bool		*Status
    );
};

#endif
