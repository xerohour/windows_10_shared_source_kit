/******************************Module*Header*******************************
* Module Name: fpo_Check.h
*
* Header for code that validates stackwalk events (via ETW)
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#ifndef __STACKWALK_LISTENER_H_
#define __STACKWALK_LISTENER_H_

HRESULT PerfinfoGuidEnabled(HANDLE _Event,
                            CLogWriter* pLog);


#endif
