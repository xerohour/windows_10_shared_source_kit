//////////////////////////////////////////////////////////////////////////
//  CullMode.cpp
//  created:	2006/07/14
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////


#include <WGFTestCore.h>
#include <stdlib.h>

#include "rasterizer.h"
#include "cullmode.h"

CCullMode::CCullMode()  
{

}

void CCullMode::InitTestParameters()
{
	InitRasterizerParameters();
	SetRootTestFactor( m_rfFCC * m_rfCullMode * m_rfFillMode * (m_rfIBSet + m_rfDegenTriSet) );
}

TEST_RESULT CCullMode::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		SetupRasterizerTestCase();
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CFillMode::SetupTestCase()] ");
		if (tRes != RESULT_SKIP)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

TEST_RESULT CCullMode::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		// validation parameters
		bool occ = 0; 

		// clear
		FLOAT ClearColor[4] = { 0, 0, 0, 0 };
		m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, NULL );

		// draw primitives
		m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
		BeginQuery();
		m_pDeviceContext->DrawIndexed(m_pBuffer->size, 0, 0);
		occ = EndQuery() != 0;

		// validate
		bool bExpected = Expected();

		// except zero area triangles are considered back facing
		if ((m_pBuffer->size == 3) && 
		                        ((m_pBuffer->data[0] == m_pBuffer->data[1]) ||
							     (m_pBuffer->data[1] == m_pBuffer->data[2]) ||
							     (m_pBuffer->data[2] == m_pBuffer->data[0])	 ))
		{
			// 2 conditions
			// -solid zero area triangles should not be rendered
			// -wireframe zero area triangles should only render on cullmode front or none
			bExpected = (m_FillMode == D3D11_FILL_WIREFRAME) && !(m_CullMode == D3D11_CULL_BACK);
		}

		if (occ != bExpected)
			if (occ)
				throw TRException("Primitive wasn't culled.");
			else
				throw TRException("Primitive was culled.");
	}
	catch (TRException& exc)
	{
		WriteIBToLog();
		tRes = exc.GetResult();
		exc.Prepend("[CCullMode::ExecuteTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

bool CCullMode::Expected()
{
	// logic for culling or not
	// combination of:
	//		-bFCC
	//		-the winding order or the primitive
	//		-cull mode

	// First, assume cull mode is back
	bool expected = m_bFCC == m_pBuffer->bCC;
	
	// Next, correct for back face culling
	if (m_CullMode == D3D11_CULL_FRONT) 
		expected = !expected;

	// Finally, correct for no culling
	if (m_CullMode == D3D11_CULL_NONE)
		expected = true;

	return expected;
}

void CCullMode::CleanupTestCase()
{
	CleanupRasterizerTestCase();
}


