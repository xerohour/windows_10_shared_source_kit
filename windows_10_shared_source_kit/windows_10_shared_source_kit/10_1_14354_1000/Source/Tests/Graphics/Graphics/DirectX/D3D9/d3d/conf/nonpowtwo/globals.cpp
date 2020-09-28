#include "NonPowTwoConfTest.h"
#include "RenderTargetTest.h"
#include "NonPowTwoStressTest.h"

CD3DWindowFramework			g_App;
CNonPowTwoConfTest			g_ConfTest;
#ifndef FAST_TEST
CNonPowTwoRTTest            g_RenderTargetTest;
CNonPowTwoStressTest		g_StressTest;
#endif
