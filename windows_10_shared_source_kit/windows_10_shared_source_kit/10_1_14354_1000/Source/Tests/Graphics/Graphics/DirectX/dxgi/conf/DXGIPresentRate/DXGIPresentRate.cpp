//
// Project headers
//

#include "DXGIPresentRate.h"


CDXGIPresentRate::CDXGIPresentRate()
{
}

CDXGIPresentRate::~CDXGIPresentRate()
{
}

bool CDXGIPresentRate::InitializeTestGroup()
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

void CDXGIPresentRate::CleanupTestGroup()
{
	DebugOut.Write("Entering CleanupTestGroup.\n");

	//CleanupWindowClass();

	DebugOut.Write("Exiting CleanupTestGroup.\n");
}
