#include "memmgr.h"

#include <windows.h>
#include <shlwapi.h>
#include <assert.h>
#include <tchar.h>
#include <DXGLogger.h>

// STL includes
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

#include "helperfunctions.h"
#include "stringutils.h"
#include "new_off.h"
#include "regexpr2.h"
#include "new_on.h"

// Local includes
#include "new_on.h"
#include "DPF.h"
#include "fwtypes.h"
#include "Error.h"
