/*	
	File:			wbuffer.cpp
	Description:    z bias conformance tests (w buffer)
	Author:			Todd M. Frost
	Modifications:	Matthew E. Haigh - 07/2003 thru 08/2003
						-Improved test for better feature coverage.
*/

#include "zbias.h"


/*
	Name:		CZBiasWBuffer()
	Desc:		z bias / w buffer class constructor
*/
CZBiasWBuffer::CZBiasWBuffer ()
{
    m_szTestName = "ZBias - WBuffer";
    m_szCommandKey = "WBuffer";
}


/*
	Name:		~CZBiasWBuffer()
	Desc:		z bias / w buffer class destructor
*/
CZBiasWBuffer::~CZBiasWBuffer ()
{
}


/*
	Name:		CapsCheck()
	Desc:		checks device capabilities
	Returns:	true if successful, false if not
*/
bool CZBiasWBuffer::CapsCheck ()
{
	//check base zbias caps
    if (!CZBias::CapsCheck())
        return false;
    else if (m_pSrcDevice != NULL)
	{	
		//check for wbuffer support
		if (!(m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_WBUFFER))
		{
			WriteToLog("Device does not support W buffers. ");

			if (!KeySet ("!caps"))
			{
				WriteToLog ("Aborting group.\n");
				return false;
			}
			else
				WriteToLog ("Ignoring.\n");
		}
	}

	//set flag indicating that the w buffer has been enabled
	CZBias::m_Flags.Set (CF_WENABLED);

    return true;
}


/*
	Name:		ExecuteTest()
	Desc:		executes wbuffer tests
	Arguments:	uTest - test number
	Returns:	true if test was successful, false if it failed
*/
bool CZBiasWBuffer::ExecuteTest (UINT uTest)
{
    SetRenderState (D3DRS_ZENABLE, D3DZB_USEW);
    return CZBias::ExecuteTest (uTest);
}