// presentintervalWindowedFlipExFlags.h: interface for the CPresentInterval class.
//
//////////////////////////////////////////////////////////////////////


#pragma once

#include <d3d9.h>
#include "PresentInterval.h"
#include <vector>

using namespace std;
typedef vector<int> VECINT;


class CPresentintervalWindowedFlipExFlags : public CPresentInterval
{
public:
    CPresentintervalWindowedFlipExFlags();
    virtual ~CPresentintervalWindowedFlipExFlags(){};
    virtual void    SetTestParameters();
    virtual bool	CapsCheck();
    virtual void	CommandLineHelp();
    virtual UINT	TestInitialize();


};

