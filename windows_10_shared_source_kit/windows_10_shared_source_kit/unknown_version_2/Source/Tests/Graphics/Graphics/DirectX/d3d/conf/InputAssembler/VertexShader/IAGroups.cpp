#include "InputAssembler.h"

bool CInputAssemblerApp::InitTestGroups()
{
	// Register test classes with the group manager.
	if( !ADD_TEST( _T( "GeneratedIDs" ), CGeneratedValuesTest ) ) return false;
	if( !ADD_TEST( _T( "ReadFromUnboundBuffers" ), CUnboundBuffersTest ) ) return false;

	// Only do limits testing if the priority level > 1, or no priority level is set
	if( GetExecutionPriorityLevel() > 1 || GetExecutionPriorityLevel() == -1 )
	{
		if( !ADD_TEST( _T( "Limits\\Addressing" ), CLimitsTest ) ) return false;
	}

	return true;
}