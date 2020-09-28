//////////////////////////////////////////////////////////////////////////
//  FillMode.cpp
//  created:	2006/07/14
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////


#include <WGFTestCore.h>
#include <stdlib.h>

#include "rasterizer.h"
#include "fillmode.h"

CFillMode::CFillMode()  
{

}

void CFillMode::InitTestParameters()
{
	InitRasterizerParameters();
	SetRootTestFactor( m_rfCullModeNone * m_rfFCCTrue * m_rfFillMode * ( m_rfIBSet ) );
}

TEST_RESULT CFillMode::SetupTestCase()
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

TEST_RESULT CFillMode::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		// validatation
		UINT64 occ = 0;		// number of pixels drawn
		UINT diff = 1;		// pixel different between TexA and TexB
	
		// clear
		FLOAT ClearColor[4] = { 0, 0, 0, 0 };
		m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, NULL );
		m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
		
		// draw primitives to TexA
		BeginQuery();
		m_pDeviceContext->DrawIndexed(m_pBuffer->size, 0, 0);
		occ = EndQuery();
		m_pDeviceContext->CopySubresourceRegion( m_pTexA, 0, 0, 0, 0, m_pRTTexture2D, 0, &m_TexBox );

		// sanity check
		if (!occ) 
			throw TRException("Primitive did not render.", RESULT_FAIL);

		// validate
		if ( m_FillMode == D3D11_FILL_WIREFRAME && 
			 ( m_pBuffer->top == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ||
			   m_pBuffer->top == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP ) )
		{
			UINT uIndexCount = ConvertToLines();
			m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
			m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
			m_pDeviceContext->DrawIndexed(uIndexCount, 0, 0);
			m_pDeviceContext->CopySubresourceRegion( m_pTexB, 0, 0, 0, 0, m_pRTTexture2D, 0, &m_TexBox );

			// compare
			UINT diff = CompareBuffers();
			if (diff) 
				throw TRException("Wireframe triangles do not match lines.", RESULT_FAIL);

		}
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CFillMode::ExecuteTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

UINT CFillMode::ConvertToLines()
{
	HRESULT hr;
	assert(m_pBuffer && m_pBuffer->size > 0);

	UINT *pData = m_pBuffer->data;
	UINT uSize = m_pBuffer->size;

	D3D11_MAPPED_SUBRESOURCE pBufData;
	if( FAILED( hr = GetEffectiveContext()->Map( m_pIB, 0, D3D11_MAP_WRITE_DISCARD, NULL, &pBufData ) ) )
		throw TRException("Map on m_pIB Failed", hr, RESULT_FAIL);
	
	UINT uCount = 0;
	if (m_pBuffer->top == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
		//
		// TriList to lines
		//
		for (UINT i = 0; i < uSize/3; i++)
		{
			UINT j = i*3;
			UINT tri[3] = { pData[j], pData[j+1], pData[j+2] };

			if ((tri[0] == tri[1]) ||
				(tri[1] == tri[2]) ||
				(tri[2] == tri[0]))
				continue;

			// triangle lines
			((UINT*)pBufData.pData)[uCount++] = tri[0];
			((UINT*)pBufData.pData)[uCount++] = tri[1];

			((UINT*)pBufData.pData)[uCount++] = tri[1];
			((UINT*)pBufData.pData)[uCount++] = tri[2];

			((UINT*)pBufData.pData)[uCount++] = tri[2];
			((UINT*)pBufData.pData)[uCount++] = tri[0];
		}
	}
	else
	{
		//
		// TriStrip to Line
		//
		for (UINT i = 0; i < (int)(uSize-2); i++)
		{
			// triangle
			UINT tri[3] = { pData[i], pData[i+1], pData[i+2] };
	
			// degenerate
			if ((tri[0] == tri[1]) ||
				(tri[1] == tri[2]) ||
				(tri[2] == tri[0]))
				continue;

			// keep winding order of 1st triangle
			if (i%2) std::swap(tri[1], tri[2]);

			// triangle lines
			((UINT*)pBufData.pData)[uCount++] = tri[0];
			((UINT*)pBufData.pData)[uCount++] = tri[1];

			((UINT*)pBufData.pData)[uCount++] = tri[1];
			((UINT*)pBufData.pData)[uCount++] = tri[2];

			((UINT*)pBufData.pData)[uCount++] = tri[2];
			((UINT*)pBufData.pData)[uCount++] = tri[0];
		}
	}

	GetEffectiveContext()->Unmap(m_pIB,0);

	return uCount;
}

void CFillMode::CleanupTestCase()
{
	CleanupRasterizerTestCase();
}


