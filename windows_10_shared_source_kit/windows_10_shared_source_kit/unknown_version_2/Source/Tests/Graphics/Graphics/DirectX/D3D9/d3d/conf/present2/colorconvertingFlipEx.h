// ColorConvertingFlipEx.h: interface for the CColorConvertingFlipEx class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ColorConverting.h"

class CColorConvertingFlipEx : public CColorConverting
{
public:
    CColorConvertingFlipEx();
    virtual ~CColorConvertingFlipEx();

protected:
    // virtual methods
    virtual bool    CapsCheck();
    virtual void    InitPresentParameters();
    virtual void    InitPresentSourceRect();            // build the vector of present source rects to test
    virtual void    InitPresentDestRect();                // build the vector of present dest rects to test
    virtual void    InitPresentWindows();               // build the vector of present windowsto test
    virtual void    ProcessREFBuffer( UINT uSwapChain );
};

