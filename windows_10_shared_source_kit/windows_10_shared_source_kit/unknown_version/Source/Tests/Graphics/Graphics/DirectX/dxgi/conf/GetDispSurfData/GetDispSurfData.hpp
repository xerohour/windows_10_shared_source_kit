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

#include <MinMaxRange.h>


union TEST_VEC4F
{
	struct{
		float a,b,c,d;
	};
	struct{
		float x,y,z,w;
	};
	float f[4];
};
using MinMaxRange::TEST_COLOR;

//
// Project headers
//

#include "Common.hpp"

#include "DXGIBBFMT.H"
#include "DXGIValueSets.h"
#include "DXGISwapEffects.h"
#include "DXGIFMT.h"
#include "D3D10Convert.h"

#include "DXGIWndClass.h"
#include "DrawD3D10Scene.hpp"


//
// StrSafe.h has to come last, because it deprecates functions
// that it doesn't like.
//

#include <StrSafe.h>


//////////////////////////////////////////////////////////////////////////
// CGetDispSurfDataTestApp - An instance of a single object derived from
// CWGFTestFramework is required by the framework.
//////////////////////////////////////////////////////////////////////////
class CGetDispSurfDataTestApp
:
	public CDXGITestFramework
{
	public:
		CGetDispSurfDataTestApp();
		~CGetDispSurfDataTestApp();

	public:   
		virtual TESTTYPE  GetTestType() const { return TESTTYPE_CONF; }
	    
		virtual bool  InitTestGroups();

		virtual bool  Setup();
};


extern CGetDispSurfDataTestApp  g_Application;
