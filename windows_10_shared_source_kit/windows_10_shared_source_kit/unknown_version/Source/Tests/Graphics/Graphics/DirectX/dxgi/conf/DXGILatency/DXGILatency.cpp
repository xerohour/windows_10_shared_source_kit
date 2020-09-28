//
// Project headers
//

#include "TestApp.h"


CDXGILatency::CDXGILatency()
{
}

CDXGILatency::~CDXGILatency()
{
}

bool CDXGILatency::InitializeTestGroup()
{
	DebugOut.Write("Entering InitializeTestGroup.\n");

	CleanupTestGroup();

	if (InitializeWindowClass())
	{
		DebugOut.Write("Exiting InitializeTestGroup (Succeeded).\n");

		return true;
	}

	DebugOut.Write("Exiting InitializeTestGroup (Failed).\n");

	return false;
}

void CDXGILatency::CleanupTestGroup()
{
	DebugOut.Write("Entering CleanupTestGroup.\n");

	//CleanupWindowClass();

	DebugOut.Write("Exiting CleanupTestGroup.\n");
}
