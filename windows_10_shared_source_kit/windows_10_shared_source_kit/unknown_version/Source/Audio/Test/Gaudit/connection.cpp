
//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       connection.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"



/* ____________________________________________________________________

    @TEST KSPROPERTY_CONNECTION_DATAFORMAT
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_DATAFORMAT_1(PGAUDITPARAMS pParams)
//for setting dataformat sending zero outputbuffer length - it must fail
{

    if (!(pParams->Prop.Flags & KSPROPERTY_TYPE_SET))
		return FNS_PASS;


	// Zero out parameter size
	{
		CGenericIoControl gnt(pParams->hFile,&pParams->Prop, 
	    	                                  pParams->cbSize, pParams->cbSize, 
						      0, 0);


		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
	}

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_DATAFORMAT_2(PGAUDITPARAMS pParams)
//setting invalid dataformat (bit depth) - must fail
{
    if (!(pParams->Prop.Flags & KSPROPERTY_TYPE_SET))
		return FNS_PASS;

   // we need a valid pin from the framework
  if (!PIN() || PIN()->m_handle == INVALID_HANDLE)
    return FNS_PASS;

   KSDATAFORMAT_WAVEFORMATEX dfwfx = { 0 };
   KSDATAFORMAT_DSOUND dfds = { 0 };
   BOOL fIsWfx = FALSE;

    // acquire pin format
    if (FNS_PASS != GetPinFormat(pParams, &dfwfx, &dfds, fIsWfx))
        return FNS_PASS;

   PWAVEFORMATEX pWfx  = (fIsWfx) ? &dfwfx.WaveFormatEx : &dfds.BufferDesc.WaveFormatEx;
   PKSDATAFORMAT pFmt  = (fIsWfx) ? &dfwfx.DataFormat : &dfds.DataFormat;
   ULONG cbOutSize = (fIsWfx) ? sizeof(KSDATAFORMAT_WAVEFORMATEX) : sizeof(KSDATAFORMAT_DSOUND);

   // change bit depth
   pWfx->wBitsPerSample   = 0x800; // >= 8
   pWfx->nBlockAlign      = 0;
   pWfx->nAvgBytesPerSec  = 0;
	
   {
        	CGenericIoControl gnt(pParams->hFile,&pParams->Prop, 
		                                  pParams->cbSize, pParams->cbSize, 
						      cbOutSize, cbOutSize);


		memcpy(gnt.GetData(), pFmt, cbOutSize);
		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
	}

	return FNS_PASS;
}


DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_DATAFORMAT_3(PGAUDITPARAMS pParams)
//setting invalid dataformat (no of channels) - must fail
{

    if (!(pParams->Prop.Flags & KSPROPERTY_TYPE_SET))
		return FNS_PASS;

   // we need a valid pin from the framework
  if (!PIN() || PIN()->m_handle == INVALID_HANDLE)
    return FNS_PASS;

   KSDATAFORMAT_WAVEFORMATEX dfwfx = { 0 };
   KSDATAFORMAT_DSOUND dfds = { 0 };
   BOOL fIsWfx = FALSE;

    // acquire pin format
    if (FNS_PASS != GetPinFormat(pParams, &dfwfx, &dfds, fIsWfx))
        return FNS_PASS;

   PWAVEFORMATEX pWfx  = (fIsWfx) ? &dfwfx.WaveFormatEx : &dfds.BufferDesc.WaveFormatEx;
   PKSDATAFORMAT pFmt  = (fIsWfx) ? &dfwfx.DataFormat : &dfds.DataFormat;
   ULONG cbOutSize = (fIsWfx) ? sizeof(KSDATAFORMAT_WAVEFORMATEX) : sizeof(KSDATAFORMAT_DSOUND);

   // change no of channels
   pWfx->nChannels   = 0; // > 0
   pWfx->nBlockAlign      = 0;
   pWfx->nAvgBytesPerSec  = 0;
 
   {
        	CGenericIoControl gnt(pParams->hFile,&pParams->Prop, 
		                                  pParams->cbSize, pParams->cbSize, 
						      cbOutSize, cbOutSize);


		memcpy(gnt.GetData(), pFmt, cbOutSize);
		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
	}

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_DATAFORMAT_4(PGAUDITPARAMS pParams)
//send incomplete dataformat only 4 bytes
{
    if (!(pParams->Prop.Flags & KSPROPERTY_TYPE_SET))
		return FNS_PASS;

   // we need a valid pin from the framework
  if (!PIN() || PIN()->m_handle == INVALID_HANDLE)
    return FNS_PASS;

   KSDATAFORMAT_WAVEFORMATEX dfwfx = { 0 };
   KSDATAFORMAT_DSOUND dfds = { 0 };
   BOOL fIsWfx = FALSE;

    // acquire pin format
    if (FNS_PASS != GetPinFormat(pParams, &dfwfx, &dfds, fIsWfx))
        return FNS_PASS;

   PWAVEFORMATEX pWfx   = (fIsWfx) ? &dfwfx.WaveFormatEx : &dfds.BufferDesc.WaveFormatEx;
   PKSDATAFORMAT pFmt   = (fIsWfx) ? &dfwfx.DataFormat : &dfds.DataFormat;

	// 4 bytes out parameter size
	{
		CGenericIoControl gnt(pParams->hFile,&pParams->Prop, 
		                                  pParams->cbSize, pParams->cbSize, 
						      4, 4);


		memcpy(gnt.GetData(), pFmt, 4);
		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
	}

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_DATAFORMAT_5(PGAUDITPARAMS pParams)
//missing specifier
{
    if (!(pParams->Prop.Flags & KSPROPERTY_TYPE_SET))
		return FNS_PASS;

   // we need a valid pin from the framework
   _ASSERT(PIN());
   _ASSERT(PIN()->m_handle != INVALID_HANDLE);

   if (!PIN() || PIN()->m_handle == INVALID_HANDLE)
     return FNS_PASS;

   KSDATAFORMAT_WAVEFORMATEX dfwfx = { 0 };
   KSDATAFORMAT_DSOUND dfds = { 0 };
   BOOL fIsWfx = FALSE;

    // acquire pin format
    if (FNS_PASS != GetPinFormat(pParams, &dfwfx, &dfds, fIsWfx))
        return FNS_PASS;

   PWAVEFORMATEX pWfx  = (fIsWfx) ? &dfwfx.WaveFormatEx : &dfds.BufferDesc.WaveFormatEx;
   PKSDATAFORMAT pFmt  = (fIsWfx) ? &dfwfx.DataFormat : &dfds.DataFormat;
   ULONG cbOutSize = (fIsWfx) ? sizeof(KSDATAFORMAT_WAVEFORMATEX) : sizeof(KSDATAFORMAT_DSOUND);

   
   // Out parameter missing specifier
   {
		CGenericIoControl gnt(pParams->hFile,&pParams->Prop, 
		                                  pParams->cbSize, pParams->cbSize, 
						      sizeof(*pFmt), sizeof(*pFmt));


		memcpy(gnt.GetData(), pFmt, sizeof(*pFmt));
		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
	}

	return FNS_PASS;
}
DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_DATAFORMAT_6(PGAUDITPARAMS pParams)
//send large buffer with invalid data
{
    if (!(pParams->Prop.Flags & KSPROPERTY_TYPE_SET))
		return FNS_PASS;

   // we need a valid pin from the framework
   _ASSERT(PIN());
   _ASSERT(PIN()->m_handle != INVALID_HANDLE);

   if (!PIN() || PIN()->m_handle == INVALID_HANDLE)
     return FNS_PASS;

   KSDATAFORMAT_WAVEFORMATEX dfwfx = { 0 };
   KSDATAFORMAT_DSOUND dfds = { 0 };
   BOOL fIsWfx = FALSE;

    // acquire pin format
    if (FNS_PASS != GetPinFormat(pParams, &dfwfx, &dfds, fIsWfx))
        return FNS_PASS;

   PWAVEFORMATEX pWfx  = (fIsWfx) ? &dfwfx.WaveFormatEx : &dfds.BufferDesc.WaveFormatEx;
   PKSDATAFORMAT pFmt  = (fIsWfx) ? &dfwfx.DataFormat : &dfds.DataFormat;
   ULONG cbOutSize = (fIsWfx) ? sizeof(KSDATAFORMAT_WAVEFORMATEX) : sizeof(KSDATAFORMAT_DSOUND);
   pFmt->FormatSize = 0x400; 
   ULONG FormatSize = 0x40;
   // Out parameter missing specifier
   {
		CGenericIoControl gnt(pParams->hFile,&pParams->Prop, 
		                                  pParams->cbSize, pParams->cbSize, 
						      FormatSize, FormatSize);
              if (fIsWfx)  
                    memcpy(gnt.GetData(), &dfwfx, FormatSize);
              else
                    memcpy(gnt.GetData(), &dfds, FormatSize);
              
		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
	}

	return FNS_PASS;
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_CONNECTION_STATE
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_STATE_1(PGAUDITPARAMS pParams)
//setting pin to invalid state
{
    if (!(pParams->Prop.Flags & KSPROPERTY_TYPE_SET))
		return FNS_PASS;
	
    // Invalid pin state value
    {

		CGenericIoControl gnt(pParams->hFile,&pParams->Prop, pParams->cbSize, pParams->cbSize, 
		  				      sizeof(ULONG), sizeof(ULONG));

        *(ULONG*)gnt.GetData() = 0xcccccccc;
	    gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
    }

	return FNS_PASS;
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_CONNECTION_PRIORITY
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_PRIORITY(PGAUDITPARAMS pParams, eTestCaseType TestCase)
{
    ULONG BufferSize = sizeof(KSPRIORITY);
    BOOL bExpectedTestResult = TRUE;
    ULONG cbExpectedWritten = 0;
    KSPRIORITY PinConnectionPriority;
    PinConnectionPriority.PriorityClass = KSPRIORITY_NORMAL;
    PinConnectionPriority.PrioritySubClass = 0xFFFFFFFF;
    // now do the test
    switch (TestCase)   {
        case NULL_Buffer:
            BufferSize = 0;  //setting channel as invalid.   
            break;    
        case Small_Buffer:
            BufferSize = BufferSize -1;  //setting channel as invalid.   
            break;    
        case Large_Buffer:
            BufferSize = BufferSize + 1;  //sending small buffer then required one
            bExpectedTestResult = FALSE;        // in case of Large buffer - Get/set request will Pass. In case of set request check for overwrite.
            cbExpectedWritten = sizeof (KSPRIORITY);
            break;
        case Valid_Test:
            bExpectedTestResult = FALSE;        // in case of Large buffer - Get/set request will Pass. In case of set request check for overwrite. result will be success but needs to varify value of lpBytesReturned of DeviceIoControl
            cbExpectedWritten = sizeof (KSPRIORITY);
            break;
        case Invalid_Data:
            PinConnectionPriority.PriorityClass = ~(KSPRIORITY_LOW | KSPRIORITY_NORMAL | KSPRIORITY_HIGH);        //setiing invalid priority. it shoud be rejectd
            break;    
        default:
            _ASSERT(FALSE);
    }
    CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, BufferSize, BufferSize);
    if (pParams->NodeProp.Property.Flags & KSPROPERTY_TYPE_SET)     {
        // since for set request we are not expecting any data to be writtne back. 
        //Ideally we should expect zero bytes as written, however there is a bug in some of drivers that they are returning number during set request. However this was take care by KS so that IO manager will not copy data back to user mode for Property Set request
        cbExpectedWritten = (ULONG) -1;
        // now set value of the buffer
        if (BufferSize)
            memcpy(gnt.GetData(), &PinConnectionPriority, BufferSize);            
    }
    DWORD  dwResult = gnt.Test(bExpectedTestResult, cbExpectedWritten);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
	return FNS_FAIL;
    else 
       return FNS_PASS; 
}

DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_PRIORITY_1(PGAUDITPARAMS pParams)
{
    return Generic_Property_KSPROPERTY_CONNECTION_PRIORITY(pParams, NULL_Buffer);
}
DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_PRIORITY_2(PGAUDITPARAMS pParams)
{
    return Generic_Property_KSPROPERTY_CONNECTION_PRIORITY(pParams, Small_Buffer);
}
DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_PRIORITY_3(PGAUDITPARAMS pParams)
{
    return Generic_Property_KSPROPERTY_CONNECTION_PRIORITY(pParams, Large_Buffer);
}
DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_PRIORITY_4(PGAUDITPARAMS pParams)
{
    return Generic_Property_KSPROPERTY_CONNECTION_PRIORITY(pParams, Valid_Test);
}
DWORD WINAPI Generic_Property_KSPROPERTY_CONNECTION_PRIORITY_5(PGAUDITPARAMS pParams)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
                    return FNS_PASS;
    // this test case is for setting invalid property so no need to run for get request
    return Generic_Property_KSPROPERTY_CONNECTION_PRIORITY(pParams, Invalid_Data);
}


