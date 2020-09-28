#ifndef STRETCHRECTTEST_H_INCLUDED
#define STRETCHRECTTEST_H_INCLUDED

#include "cd3dwindowframework.h"

#define MAX_STRETCHRECT_LOG_FAILURES 500

class CD3DWindowFramework_Limit_Failures: public CD3DWindowFramework
{
public:
	CD3DWindowFramework_Limit_Failures() {m_LogInfo.uMaxLogFileFailures = MAX_STRETCHRECT_LOG_FAILURES;}
	~CD3DWindowFramework_Limit_Failures() {}
};

extern CD3DWindowFramework_Limit_Failures TestApp;

#endif
