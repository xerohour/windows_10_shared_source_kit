//
// Win32 headers
//

#include <windows.h>
#include <basetsd.h>


//
// Direct3D headers
//

#include <d3d10_1.h>
#include <dxgi.h>
#include <d3dx10.h>


//
// Framework headers
//

#include <WGFTestCore.h>
#include <WGFTestUI.h>
#include <DXGITest.h>


//
// Project headers
//

#include "DXGIModeEnum.h"


//////////////////////////////////////////////////////////////////////////
// CMyTestApp - An instance of a single object derived from CWGFTestFramework
// is required by the framework.
//////////////////////////////////////////////////////////////////////////
class CMyTestApp
:
	public CDXGITestFramework
{
	//
	// Construction/destruction
	//

	public:
		CMyTestApp();

		virtual ~CMyTestApp();


	//
	// Framework override methods
	//

	public:	
		virtual TESTTYPE GetTestType() const
		{
			return TESTTYPE_CONF;
		};

		virtual bool InitTestGroups();

		virtual void InitOptionVariables();
};


extern CMyTestApp g_Application;
