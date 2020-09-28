//////////////////////////////////////////////////////////////////////////
//  Mask.cpp
//  created:	2005/07/18
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "mask.h"

void CMultisampleTest_Mask::InitTestParameters()
{
	InitMultisampleParameters();
	SetRootTestFactor( m_rfSampleDesc * ( m_rfRTFormatTypedSubset % m_rfPixPos ) );
}


TEST_RESULT CMultisampleTest_Mask::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	tRes = SetFormatAndQualityLevel( false );
	if ( tRes != RESULT_PASS )
	{
		return tRes;
	}

	try 
	{
		SetupD3D();
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Mask::SetupTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}
	
	return tRes;
}

void CMultisampleTest_Mask::CleanupTestCase()
{
	CMultisampleTest::CleanupD3D();
}

TEST_RESULT CMultisampleTest_Mask::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	D3D11_MAPPED_SUBRESOURCE texmap;

	try
	{
		if( FAILED( m_Finder.FindSamples() ) )
			throw TRException("FindSamples failed.");
		if( FAILED( m_Finder.OrderSamples() ) )
			throw TRException("FindSamples failed.");
		ResetStates();
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Mask::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}
	
	try
	{
		Present();
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CMultisampleTest_Mask::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	return tRes;	
}

