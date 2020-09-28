#include "Present.h"
#include "ColorConverting.h"
#include "ColorConvertingFlipEx.h"
#include "GammaPresent.h"
#include "PresentFlipEx.h"

//
// Declare the framework
//
CD3DWindowFramework	App;


//
// Declare the tests
//
CPresent testPresent;
#ifndef FAST_TEST
CPresentFlipEx testPresentFlipEx;
CColorConvertingFlipEx testColorConvertingFlipEx;
#else
// Split up testing of different present rects and presenting to different types of windows when doing FAST_TEST variations
CWindowPresent testWindowPresent;
#endif
CColorConverting testColorConverting;
CGammaPresent testGammaPresent;