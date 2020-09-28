#ifndef __TEST_EVICT_H
#define __TEST_EVICT_H

// This component tests eviction.

class CTestSlowEviction : public CGenericI3DComponent
{

public:
    //  In the constructor, we describe this component, and each variable it contains
    CTestSlowEviction();    

protected:
    //  Overrides from CGenericI3DComponent
    //  This is where we actually execute the test.
    virtual void RunTest();
    virtual void EndTest();

private:

    HRESULT EvictVB();
    HRESULT EvictTex2();
    HRESULT EvictVolTex();

    // utility function to fill with or check allocation pattern
    HRESULT ResourcePattern(UINT size, BYTE* Start, DWORD dwFillPattern, DWORD Mask, BOOL bFill, BOOL bCheck);

    LONGLONG                m_size;    // Size of total video memory in bytes
    UINT                    m_fault;   // fault that will be set    
    bool                    m_bBVT;	   // run in BVT mode
};

void SetFault(UINT mask);
DWORD WINAPI ControlFault(LPVOID mask); 

#endif

