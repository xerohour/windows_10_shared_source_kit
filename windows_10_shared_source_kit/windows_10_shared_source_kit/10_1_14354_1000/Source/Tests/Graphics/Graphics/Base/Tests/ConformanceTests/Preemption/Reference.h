#ifndef __REFERENCE_H
#define __REFERENCE_H

#include "BaseVid.h"
#include "BackBufferCompare.h"

/*
  This component tests eviction.

  It is derived from CGenericI3DComponent because it uses a 3D device
  and it throws exceptions on error (which CGenericI3DComponent turns into failures in the log)
*/

class CReference : public CGenericI3DComponent
{

    public:
    /*
      In the constructor, we describe this component, and each variable it contains
    */
    CReference();
    

protected:
    /*
      Overrides from CGenericI3DComponent
      This is where we actually execute the test.
    */
    virtual void RunTest();
    virtual void EndTest();
private:

    /*
        Will have the contents of the reference back buffer and will be used for comparison.
    */
    CBackBufferCompare* m_pBackBufferCompare;
    
};

#endif