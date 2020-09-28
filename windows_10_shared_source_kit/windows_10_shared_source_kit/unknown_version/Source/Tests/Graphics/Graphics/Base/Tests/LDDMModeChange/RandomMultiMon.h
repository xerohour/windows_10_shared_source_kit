#ifndef __RANDOM_MULTI_MON_H_
#define __RANDOM_MULTI_MON_H_

/*
 * Random Multi-Monitor Component - Spec
 * This module selects to either attach, or detach a display head from the desktop.
 * It then selects a random head of the appropriate type to act upon, and attempts
 * to attach or detach the display head.
 *
 * When disconnecting a monitor, we store the mode it is currently in. We use these stored
 * modes for the attach calls. When the module initializes, we select a basic 
 * mode (800x600x16x60) for every monitor.
 *
 * -- Short of a bugcheck, this module currently has no "failures"
 * -- If we select a desktop "Position" that is incorrect, the API picks a good location for us, so we don't even try.
  * -- For ease of interoperation with the framework, this component will take an integer "counter".
 *    This value is not used internally.
 * -- The random generator is seeded with the time unless seeding with a static variable is requested.
 */

#include "DisplayModeStructures.h"

class CRandomMultiMon : public CTestComponent
{
public:
    CRandomMultiMon();
    virtual void PreWalk(CLogWriter* pLog);

private:
    bool    m_Initialized;
    DWORD   m_NumOfDisplays;
    DWORD   m_DisplaysAvailableToAttach;
    std::vector<DisplayMode>    m_StoredModes;

    void InitializeDataStructures();
    void AttachRandomDisplay(CLogWriter* pLog);
    void DetachRandomDisplay(CLogWriter* pLog);
    DWORD SelectDisplayForAttach();
    DWORD SelectDisplayForDetach();
    DWORD GetNumOfDisplays();
    DWORD GetDisplaysAvailableToAttach();
    DWORD GetNumOfDisplaysAttached();
};

#endif