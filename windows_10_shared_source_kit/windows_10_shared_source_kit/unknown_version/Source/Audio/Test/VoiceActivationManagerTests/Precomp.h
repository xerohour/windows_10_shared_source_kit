//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft shared
// source or premium shared source license agreement under which you licensed
// this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the SOURCE.RTF on your install media or the root of your tools installation.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
#ifndef _PRECOMP_H_
#define _PRECOMP_H_

#pragma once

#include <nt.h>  
#include <ntrtl.h> 
#include <nturtl.h> 
#include <ntexapi.h>          
#include <wnfnamesp.h>      

#include <atlbase.h>
#include <atlcom.h>
#include <initguid.h>
#include <VoiceActivationManager.h>
#include <KeywordDetectorOemAdapter.h>
#include <wextestclass.h>
#include <mmdeviceapip.h>
#include <AudioClient.h>
#include <wrl/ftm.h>

using namespace Microsoft::WRL;
using namespace WEX::Logging;
using namespace WEX::Common;

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)                 if (p) { p->Release(); p = NULL; }
#endif
                
#endif // _PRECOMP_H_