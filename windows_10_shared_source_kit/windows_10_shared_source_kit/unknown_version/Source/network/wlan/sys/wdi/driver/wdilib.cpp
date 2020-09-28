#include "precomp.hpp"
#include "wdilib.tmh"

NTSTATUS
WdiInitialize(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath)
/*++

    WDI driver binary will be separated from miniport.
    When that happens, this function is the DriverEntry

--*/
{
    NTSTATUS   status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    WFCInfo("WDI Helper Library");    

    // Create the global object
    g_pWdiDriver = new CWdiDriver();
    if ( g_pWdiDriver == NULL) 
    {
        WFCError("Failed to allocate WDI Driver object");  
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    status = g_pWdiDriver->Initialize();
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize WDI Driver object. NTSTATUS = %08x", status);
        goto exit;
    }

    return STATUS_SUCCESS;

 exit:
 
    if(!NT_SUCCESS(status))
    {        
        if (g_pWdiDriver)
        {
            delete g_pWdiDriver;
            g_pWdiDriver = NULL;
        }        
    }

    return status;    
}


VOID
WdiCleanup()
{
    WFCInfo("WDI Helper Library - Unload");
    
    if (g_pWdiDriver)
    {
        delete g_pWdiDriver;
        g_pWdiDriver = NULL;
    }
}


