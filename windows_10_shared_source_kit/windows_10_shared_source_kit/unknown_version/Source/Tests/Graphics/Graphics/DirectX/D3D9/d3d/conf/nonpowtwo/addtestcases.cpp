#include "NonPowTwoTest.h"
#include "TestCaseStruct.h"

#define ADD_CASE(Type, Permutation) { pAdd = new CTestVariableImpl<Type>; if(NULL == pAdd) {return false;} 	if(!pTestMatrix->AddTestVariable(pAdd, Permutation)) {delete pAdd; return false;}}

bool CNonPowTwoTest::AddTestCases(CTestMatrix* pTestMatrix)
{
	if(NULL == pTestMatrix)
	{
		return false;
	}

	CTestVariable* pAdd = NULL;

    bool bPermute = false;
	
	ADD_CASE(STextureCase, true);

	if(!KeySet(_T("NoWrap")))
    {
	    ADD_CASE(SWrapCase, bPermute);
    }
	
    if(!KeySet(_T("NoAddress")))
    {
	    ADD_CASE(SAddressModeCase, bPermute);
    }

    if(!KeySet(_T("NoAlphaBlend")))
    {
	    ADD_CASE(SAlphaBlendCase,bPermute);
    }
    
	if(!KeySet(_T("NoTexOp")))
    {
        ADD_CASE(STextureOpCase, bPermute);
    }

	if(!KeySet(_T("NoLODBias")))
    {
        ADD_CASE(SLODBiasCase, bPermute);
    }

	if(!KeySet(_T("NoFilter")))
    {
	    ADD_CASE(SFilterCase, bPermute);
    }

	if(!KeySet(_T("NoVShader")))
    {
	    ADD_CASE(SVShaderCase, bPermute);
    }

	return true;
}

