#pragma once

#include "ResourceAccess.h"

#include <vector>
using namespace std;

// This test verifies no obvious leaks and that Flush clears out any deferred destruction queues available.
// In the current implementation, it uses Resources only and makes inferences.
// A more ideal implementation could possibly leverage ETW events in the future without much inference.
class CResourceFlushTest : public CD3D11Test
{
public:
	CResourceFlushTest( )
	{ 
	}

	virtual TEST_RESULT		SetupTestCase( );
	virtual TEST_RESULT		ExecuteTestCase( );
	virtual void			CleanupTestCase( );

protected:
	vector< ID3D11Buffer* > m_vBufs;
};

