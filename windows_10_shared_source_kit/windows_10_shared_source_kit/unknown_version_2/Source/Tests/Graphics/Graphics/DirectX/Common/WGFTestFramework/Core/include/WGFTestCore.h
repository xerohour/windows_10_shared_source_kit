//////////////////////////////////////////////////////////////////////////
//  d3dtestfw.h
//  created:	2004/09/18
//
//  purpose: Generic header file that includes all necessary framework headers.
//////////////////////////////////////////////////////////////////////////

#include "memmgr.h"

#include "tchar.h"

// STL
#include <string>
#include <map>
#include <list>
#include <vector>
#include <set>

// Framework headers
#include "new_off.h"
#include "regexpr2.h"
#include "new_on.h"
#include "stringutils.h"
#include "fwtypes.h"
#include "ctestfactor.h"
#include "CommandLine.h"
#include "VariationCore.h"
#include "DPF.h"
#include "Error.h"
#include "Handle.h"
#include "Logger.h"
#include "Test.h"
#include "TestClassFactory.h"
#include "TestGroupManager.h"
#include "FloatUtils.h"
#include "Framework.h"
#include "Shell.h"

// Defines
#if !defined(PASTER2)
#define PASTER2(x,y) x ## y
#endif

#if !defined(PASTER)
#define PASTER(x,y) PASTER2(x,y)
#endif

#if !defined(UNICODIFY)
#define UNICODIFY( x ) PASTER(L,x)
#endif

#if !defined(__FILEW__)
#define __FILEW__ UNICODIFY(__FILE__)
#endif
