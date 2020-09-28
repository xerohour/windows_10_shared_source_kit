// PresentFlipEx.h: interface for the CPresentFlipEx class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Present.h"

class CPresentFlipEx : public CPresent
{
public:
    CPresentFlipEx();
    virtual ~CPresentFlipEx();

protected:
    // virtual methods
    virtual bool    CapsCheck();
    virtual char*   Name( char* szName );
    virtual void	InitPresentParameters();		  // build the vector of presentparameters to test
    virtual void    InitPresentSourceRect();          // build the vector of present source rects to test
    virtual void    InitPresentDestRect();            // build the vector of present dest rects to test
    virtual void    InitPresentWindows();             // build the vector of present windowsto test
    virtual void    AfterReleaseDeviceBeforeVerify(); // called by test, so it can force a dwm repaint
    virtual void    InitDeviceReleaseBeforeVerify();  // Used by FlipEx Tests to validate DWM frame reuse after device change.
    virtual void    InitWindowTrees();                // build vector of child window hierarchy
    virtual void    PrivateDataProcessing();
    virtual void    Present();
    virtual void    InitFlagsEx();
    virtual bool TestTerminate();
    virtual UINT TestInitialize();
     virtual bool CanUseFlipEx();

    HWND m_clearWindow;

};

