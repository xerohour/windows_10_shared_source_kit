#include "PresentDoNotWait.h"
#include "Lock_DoNotWait.h"
#include "PresentIntervalWindowed.h"
#include "PresentIntervalFullScreen.h"
#include "PresentIntervalWindowedFlipEx.h"
#include "PresentintervalWindowedFlipExFlags.h"
#include "FullScreenPresent.h"
#include "MultiheadPresent.h"

//
// Declare the framework
//
CD3DWindowFramework	App;


//
// Declare the tests
//
CPresentDoNotWaitWindowed testPresentDoNotWaitWindowed;
CPresentIntervalWindowed testPresentIntervalWindowed;
CPresentIntervalWindowedFlipEx testPresentIntervalWindowedFlipEx;
CPresentintervalWindowedFlipExFlags testPresentintervalWindowedFlipExFlags;

LockDoNotWait testLockDoNotWait;
CPresentDoNotWaitFullScreen testPresentDoNotWaitFullScreen;
CPresentIntervalFullScreen testPresentIntervalFullScreen;
CFullScreenPresent testFullScreenPresent;
CMultiheadPresent testMultiheadPresent;