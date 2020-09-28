/*	
	File:			zbuffer.cpp
	Description:    z bias conformance tests (z buffer)
	Author:			Todd M. Frost
	Modifications:	Matthew E. Haigh - 07/2003 thru 08/2003
						-Improved test for better feature coverage.
*/

#include "zbias.h"

/*
	Name:		CZBiasZBuffer()
	Desc:		z bias / z buffer class constructor
*/
CZBiasZBuffer::CZBiasZBuffer()
{
    m_szTestName = "ZBias - ZBuffer";
    m_szCommandKey = "ZBuffer";
}


/*
	Name:		~CZBiasZBuffer()
	Desc:		z bias / z buffer class destructor
*/
CZBiasZBuffer::~CZBiasZBuffer()
{
}


/*
	Name:		CapsCheck()
	Desc:		checks device capabilities
	Returns:	true if successful, false if not
*/
bool CZBiasZBuffer::CapsCheck()
{
	CZBias::m_Flags.Clear (CF_WENABLED);

	return CZBias::CapsCheck();
}


/*
	Name:		ExecuteTest()
	Desc:		executes zbuffer tests
	Arguments:	uTest - test number
	Returns:	true if test was successful, false if it failed
*/
bool CZBiasZBuffer::ExecuteTest (UINT uTest)
{
    SetRenderState (D3DRS_ZENABLE, D3DZB_TRUE);
    return CZBias::ExecuteTest (uTest);
}
