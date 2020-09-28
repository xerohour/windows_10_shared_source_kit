// PresentIntervalWindowedFlipEx.h: interface for the CPresentInterval class.
//
//////////////////////////////////////////////////////////////////////


#pragma once

#include <d3d9.h>
#include "PresentInterval.h"
#include <vector>

using namespace std;
#pragma warning( disable : 4786 4503 )
typedef vector<int> VECINT;


class CPresentIntervalWindowedFlipEx : public CPresentInterval
{
public:
    CPresentIntervalWindowedFlipEx();
    virtual ~CPresentIntervalWindowedFlipEx(){};
    virtual void    SetTestParameters();
    virtual bool	CapsCheck();
    virtual void	CommandLineHelp();
    virtual UINT	TestInitialize();



};

