#include "DXGIStereoPresent.h"

class TestApp : public CD3D11TestFramework
{
public:
	TESTTYPE	GetTestType() const { return TESTTYPE_CONF; }

	void		InitOptionVariables( );
	bool		InitTestGroups( );
};

extern TestApp g_TestApp;
