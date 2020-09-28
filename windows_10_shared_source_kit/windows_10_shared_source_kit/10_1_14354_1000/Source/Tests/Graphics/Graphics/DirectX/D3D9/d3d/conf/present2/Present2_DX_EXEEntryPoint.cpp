#include "DialogBoxMode.h"
#include "DirtyRegion.h"
#include "clippingwindows.h"
#include "DirtyRegionFlipEx.h"
#include "clippingwindowsFlipEx.h"
//
// Declare the framework
//
CD3DWindowFramework	App;


//
// Declare the tests
//
CDirtyRegion testDirtyRegion;
CClippingWindows testClippingWindows;
#ifndef FAST_TEST
CDialogBoxMode testDialogBoxMode;
CDirtyRegionFlipEx testDirtyRegionFlipEx;
CClippingWindowsFlipEx testClippingWindowsFlipEx;
#endif