#pragma once

// Defines

// This will force the test to check itself by detouring a function 
// and checking to see if it was detoured or not. This should cause the test to fail.
// You should only set this for debugging.
//#define CDETOURSTEST_ENABLEDETOUREDTEST 1

// Includes
#include "D3DWDDM.h"
#include "Adapter.h"
#include "d3d9.h"

#ifdef CDETOURSTEST_ENABLEDETOUREDTEST
#include "detours.h"
#endif

////////////////////////

class CDetoursTest : public CD3D11Test
{
public:
	CDetoursTest( void );
	void InitTestParameters();
	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	void Cleanup();

	// Helpers
	bool CheckForDisallowedLoadedModules();											// Returns true if a disallowed module is present
	bool CheckForDetouredSystemFunctions( const bool &bDetoursExpected = false );	// Returns true if bDetoursExpected is true AND all system functions have been detoured
	bool CheckForDetouredD3DFunctions( const bool &bDetoursExpected = false );		// Returns true if bDetoursExpected is true AND all D3D functions have been detoured
	bool CheckFunctionDetours( const bool &bFunctionIsDetoured, const bool &bDetouredFunctionExpected );	
	bool CheckVTableModuleSpaces();													// Returns true if vtables are present in D3D module spaces
	bool IsFunctionDetoured( void* pfnToCheck, const tstring fnName );
	bool IsAddressInModuleSpace( void* pToCheck, const tstring moduleName );
	std::vector<tstring> GetModuleNamesForProcess( const DWORD &processID );

#ifdef CDETOURSTEST_ENABLEDETOUREDTEST
	bool CheckTestDetectsDetours();			// Returns true if the test can successfully check for detoured functions
	bool AttachDetoursToSystemAndD3DFunctions();
	bool DetachDetoursFromSystemAndD3DFunctions();
	static bool TestFunctionToDetour();
	static bool TestFunctionToDetourTo();
#endif

protected:
	std::vector<CAdapter*> m_adapters;
	std::vector<bool> m_bTestAdapterDXGI;
	std::vector<bool> m_bTestAdapterD3D9;
	ATL::CComPtr<ID3D11Device> m_pDevice;
	ATL::CComPtr<ID3D11DeviceContext> m_pDeviceContext;
	ATL::CComPtr<IDirect3D9> m_pD3D9;
	ATL::CComPtr<IDirect3DDevice9> m_pD3D9Device;
	D3D_FEATURE_LEVEL m_featureLevel;
	D3DKMT_DRIVERVERSION m_kmtDriverVersion;
	HMODULE m_hD3D10;
	HMODULE m_hD3D10_1;

private:
	static const tstring s_disallowedModuleNames[2];
	bool m_bDetoursAttached;	// This flag is used for checking the test when it detours system functions.
};