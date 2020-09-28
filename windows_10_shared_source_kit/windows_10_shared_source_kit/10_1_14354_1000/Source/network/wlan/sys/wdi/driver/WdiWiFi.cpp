#include "precomp.hpp"
#include "wdiwifi.tmh"

ULONG       g_DebugFailDriverEntry = 0;

ULONG       g_DebugDllRefCount = 0;

#ifndef WDI_WIN81_COMPLIANT
// The global TraceLogging registration status
BOOLEAN	    g_RegisteredWithTraceLogging = FALSE;

// {1fb5ff90-a110-5110-3fa7-5e54b1e386fe}
TRACELOGGING_DEFINE_PROVIDER(g_hProvider, "Microsoft.Windows.WdiWiFi", 
    ( 0x1fb5ff90, 0xa110, 0x5110, 0x3f, 0xa7, 0x5e, 0x54, 0xb1, 0xe3, 0x86, 0xfe ),
    TraceLoggingOptionMicrosoftTelemetry());
#endif

extern "C"
{

    NTSTATUS
        DllInitialize(
        IN PUNICODE_STRING RegistryPath
        )
    {
        UNREFERENCED_PARAMETER( RegistryPath );
        if (g_pWdiDriver == NULL)
        {
            DbgPrintEx( DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "WDIWiFi: DriverEntry NOT called\n" );
            return STATUS_BAD_SERVICE_ENTRYPOINT;
        }

        return STATUS_SUCCESS;
    }

    NTSTATUS
        DllUnload()
    {
        return NDIS_STATUS_SUCCESS;
    }

    // Declaration to make OACR happy
    EVT_WDF_DRIVER_UNLOAD EvtDriverUnload;

    VOID
        EvtDriverUnload(
        _In_  WDFDRIVER Driver
        )
    {
        if (g_DebugDllRefCount != 0)
        {
            DbgPrintEx( DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "WDIWiFi: DriverEntry being unloaded earlier than expected\n" );
            // This should be done correctly
#pragma warning(suppress: 28159) // We want to use a BugCheck
            KeBugCheckEx(
                0xDEADDEAD,     // User initiated this by attempting to stop our service
                0,
                0,
                0,
                0
                );
        }
        WdiCleanup();

        WPP_CLEANUP( WdfDriverWdmGetDriverObject( Driver ) );

#ifndef WDI_WIN81_COMPLIANT
        TraceLoggingUnregister( g_hProvider );
#endif
    }

    // Declaration to make OACR happy
    DRIVER_INITIALIZE DriverEntry;

    NTSTATUS
        DriverEntry(
        IN OUT PDRIVER_OBJECT   DriverObject,
        IN PUNICODE_STRING      RegistryPath
        )
        /*++

        Routine Description:
        This routine is called by the Operating System to initialize the driver.

        It creates the device object, fills in the dispatch entry points and
        completes the initialization.

        Arguments:
        DriverObject - a pointer to the object that represents this device
        driver.

        RegistryPath - a pointer to our Services key in the registry.

        Return Value:
        STATUS_SUCCESS if initialized; an error otherwise.

        --*/
    {
        NTSTATUS                       ntStatus = STATUS_SUCCESS;
        WDF_DRIVER_CONFIG              config;
        WDFDRIVER                      hDriver;
        WDF_OBJECT_ATTRIBUTES          attributes;
        bool                           bLibInitialized = FALSE;

        // Allows us to recover from bad state
        if (g_DebugFailDriverEntry)
        {
            DbgPrintEx( DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "WDIWiFi: FAILING DRIVER ENTRY\n" );
            return STATUS_UNSUCCESSFUL;
        }

        WPP_INIT_TRACING( DriverObject, RegistryPath );

        WFCInfo( "WDIWIFI Version %d Build " __DATE__ " at " __TIME, WDI_VERSION_LATEST );

        CSystem::Init();

#ifndef WDI_WIN81_COMPLIANT
        ntStatus = TraceLoggingRegister( g_hProvider );
        if (!NT_SUCCESS( ntStatus ))
        {
            g_RegisteredWithTraceLogging = FALSE;
            WFCError( "TraceLoggingRegister failed. Error = %08x", ntStatus );
        }
        else
        {
            g_RegisteredWithTraceLogging = TRUE;
        }
#endif

        WDF_DRIVER_CONFIG_INIT(
            &config,
            WDF_NO_EVENT_CALLBACK // This is a non-pnp driver. 
            );

        // 
        // Tell the framework that this is non-pnp driver so that it doesn't 
        // set the default AddDevice routine. 
        // 
        config.DriverInitFlags |= WdfDriverInitNonPnpDriver;

        // 
        // NonPnp driver must explicitly register an unload routine for 
        // the driver to be unloaded. 
        // 
        config.EvtDriverUnload = EvtDriverUnload;

        WDF_OBJECT_ATTRIBUTES_INIT( &attributes );

        // 
        // Create a framework driver object to represent our driver. 
        // 
        ntStatus = WdfDriverCreate( DriverObject,
            RegistryPath,
            &attributes,
            &config,
            &hDriver );
        if (!NT_SUCCESS( ntStatus ))
        {
            WFCError( "WdfDriverCreate failed. Error = %08x", ntStatus );
            goto exit;
        }

        ntStatus = WdiInitialize( DriverObject, RegistryPath );
        if (ntStatus != STATUS_SUCCESS)
        {
            WFCError( "Failed to initialize WDI Library. Error = %08x", ntStatus );
            goto exit;
        }

        bLibInitialized = true;

exit:
        if (ntStatus != STATUS_SUCCESS)
        {
            if (bLibInitialized)
            {
                WdiCleanup();
            }

            WPP_CLEANUP( DriverObject );
#ifndef WDI_WIN81_COMPLIANT
            if (g_RegisteredWithTraceLogging)
            {
                TraceLoggingUnregister( g_hProvider );
            }
#endif
        }

        return ntStatus;
    }

}

