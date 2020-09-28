#include "PnPStop.h"
#include "GeneratePnPStop.h"

#define BUFSIZE 1024

/*
  This is where we define the component
*/
CPnPStopDeviceParams::CPnPStopDeviceParams()
{
    /*
      First, we describe the purpose of this component for the spec
    */
    SetDescription(L"Calls device control functions.")  ;

    AddParam<std::wstring>(L"DriverDesc", L"Set if the driver is to have this description.");
    SetParamTestDomainSingleValue<std::wstring>(L"DriverDesc", L"Default");
}


/*
  This method is called each time we are to run a test case
*/
void CPnPStopDeviceParams::PreWalk(CLogWriter * pLog)
{
    CPnPControlDevice controlAdapter(pLog);

    std::wstring sDriverDesc = GetParam<std::wstring>(L"DriverDesc");

    LPTSTR lpszDriverDesc;      
    TCHAR tchDriverDesc[BUFSIZE];
    lpszDriverDesc = tchDriverDesc;

    if(TRUE == controlAdapter.GetDriverDescription(lpszDriverDesc))
	{
        if(0 != _tcsicmp(lpszDriverDesc,sDriverDesc.c_str()))
        { 
            std::wstring sLog = L"Mismatch of driver descriptions - Expected description : ";
            sLog += sDriverDesc.c_str();
            sLog += L" Installed driver description : ";
            sLog += lpszDriverDesc;
            pLog->Fail(sLog.c_str());
            return;
        }
	}
    else
    {
        pLog->Fail(L"GetDriverDescription failed.");
    }

    return;
}

void CPnPStopDeviceParams::PostWalk(CLogWriter * pLog)
{
}

CPnPRestartLinkedDevice::CPnPRestartLinkedDevice()
{
    SetDescription(L"Restarts all links in LDA sequentially one after another.")  ;
}


/*
  This method is called each time we are to run a test case
*/
void CPnPRestartLinkedDevice::PreWalk(CLogWriter * pLog)
{
    CPnPControlDevice controlAdapter(pLog);
    int numDevices;

    if(FALSE == controlAdapter.GetNumberDeviceInstances(&numDevices))
    {
        pLog->Fail(L"GetNumberDeviceInstances failed.");
        return;
    }

    for(int i=0; i<numDevices; i++)
    {
        if(FALSE == controlAdapter.PnPStop(i))
        {
            pLog->Fail(L"PnPStop failed.");
            continue;
        }

        if(FALSE == controlAdapter.PnPStart(i))
        {
            pLog->Fail(L"PnPStart failed.");
        }		
    }

    return;
}

void CPnPRestartLinkedDevice::PostWalk(CLogWriter * pLog)
{
}

