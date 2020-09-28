#include "InputAssembler.h"

bool CInputAssemblerApp::InitTestGroups()
{
	// Register test classes with the group manager.
	if( !ADD_TEST( _T( "InputLayouts" ), ID3D11InputLayoutsTest ) ) return false;
	if( !ADD_TEST( _T( "Buffers" ), CBufferTest ) ) return false;

	return true;
}