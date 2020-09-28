#ifndef _SET_DISPLAY_CONFIG_H
#define _SET_DISPLAY_CONFIG_H

#include "BaseVid.h"
#include "CcdCommon.h"

class CSetDisplayConfig: public CTestComponent
{
public:
	CSetDisplayConfig();

protected:
	/*
	Overrides from CGenericI3DComponent
	This is where we actually execute the test.
	*/
	virtual void PreWalk(CLogWriter * pLog);
	virtual void PostWalk(CLogWriter * pLog);   
};

#endif //_SET_DISPLAY_CONFIG_H
