#ifndef __TEST_EVICTION_H
#define __TEST_EVICTION_H

#include "BaseVid.h"

/*
  This component tests eviction.

  It is derived from CGenericI3DComponent because it uses a 3D device
  and it throws exceptions on error (which CGenericI3DComponent turns into failures in the log)
*/

class CTestEviction : public CGenericI3DComponent
{

    public:
    /*
      In the constructor, we describe this component, and each variable it contains
    */
    CTestEviction();        

protected:
    /*
      Overrides from CGenericI3DComponent
      This is where we actually execute the test.
    */
    virtual void RunTest();
    virtual void EndTest();
private:

};

#endif