//
// Project headers
//

#include "DXGIWaitVBl.h"


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

		virtual bool Setup();

		virtual void InitOptionVariables();


	//
	// Public methods
	//

	public:
		bool GetBreakOnFailure() const
		{
			return m_BreakOnFailure;
		};


	//
	// Member data
	//

	protected:	
		bool m_BreakOnFailure;
};


extern CMyTestApp g_Application;
