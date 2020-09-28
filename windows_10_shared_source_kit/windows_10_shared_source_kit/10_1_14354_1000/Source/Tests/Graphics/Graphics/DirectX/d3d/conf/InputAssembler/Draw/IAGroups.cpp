#include "InputAssembler.h"

bool CInputAssemblerApp::InitTestGroups()
{
	// Register test classes with the group manager.
	if( !ADD_TEST( _T( "Draw" ), CDrawTest ) ) return false;
	if( !ADD_TEST( _T( "DrawIndirect" ), CDrawIndirectTest ) ) return false;

	return true;
}