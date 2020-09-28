#ifndef __RANDOM_MODE_CHANGE_H_
#define __RANDOM_MODE_CHANGE_H_

/*
 * Random Mode Change Test Component
 * This module selects a random display adapter head, and then selects a random mode
 * from the supported list for that display adapter, and attepts to change to that mode.
 * It stores the results (pass/fail) for the change along with the mode for every adapter
 * in the system for future comparison.
 *
 * -- Failures are logged if a mode switch works at one time, and fails later, or vice-versa.
 * -- For ease of interoperation with the framework, this component will take an integer "counter".
 *    This value is not used internally.
 * -- The random generator is seeded with the time unless seeding with a static variable is requested.
 * -- This component does not support hot plugging displays and behavior is undefined in that scenario.
 */

#include "DisplayModeStructures.h"

class CRandomModeChange : public CTestComponent
{
public:
    CRandomModeChange();
    virtual void PreWalk(CLogWriter* pLog);

private:
    bool    m_Initialized;
    DWORD   m_NumOfDisplays;

    std::set<FullDisplayMode>   m_ResultMap;
    std::vector< std::vector<DisplayMode> >     m_DisplayModes;

    void InitializeDataStructures();
    bool ChangeDisplayModeByIndex(DWORD ModeNumber, DWORD DisplayNumber);

    DWORD GetNumOfDisplays();
    DWORD SelectRandomDisplay();
    DWORD FindModeNumber(DWORD DisplayNumber, DWORD Width, DWORD Height, DWORD ColorDepth, DWORD Frequency);
};

#endif