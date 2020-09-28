//
// Project headers
//

#include "DXGILatency.h"


//////////////////////////////////////////////////////////////////////////
// CMyTestApp - An instance of a single object derived from CWGFTestFramework
// is required by the framework.
//////////////////////////////////////////////////////////////////////////
class CMyTestApp
    :
    public CDXGITestFramework
{
    //
    // Construction/deestruction
    //

public:
    CMyTestApp();

    virtual ~CMyTestApp();

    bool m_bLoosenTolerances;
    bool m_bDFlip;

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

    virtual bool Setup();

};

extern CMyTestApp g_Application;
