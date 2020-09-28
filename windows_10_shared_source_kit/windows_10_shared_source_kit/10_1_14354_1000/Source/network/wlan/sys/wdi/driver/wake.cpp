/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    wake.cpp

Abstract:

    Implementation for generic request handler

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "wake.tmh"


//++ CSetPmParametersJob
NDIS_STATUS CSetPmParametersJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _In_ PUINT  pBytesRead,
    _In_ PUINT  pBytesNeeded )
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;
    PNDIS_PM_PARAMETERS pPmParameters;
    NDIS_PM_PARAMETERS noWakes = { 0 };

    TraceActEntry();

    UNREFERENCED_PARAMETER(pBytesRead);
    UNREFERENCED_PARAMETER(pBytesNeeded);

    if ( m_NdisPortNumber != NDIS_DEFAULT_PORT_NUMBER )
    {
        ndisStatus = STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    CPort * pPort = m_pAdapter->GetPortFromNdisPortNumber( m_NdisPortNumber );

    if ( pPort == NULL || 
        InformationBufferLength < sizeof(NDIS_PM_PARAMETERS))
    {
        ndisStatus = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    //
    // cache a copy of PM_PARAMETER in the port
    //
    pPmParameters = pPort->GetNdisPmParameters();
    RtlCopyMemory( pPmParameters, pInformationBuffer, sizeof(NDIS_PM_PARAMETERS));

    //
    // Parameter contemt starts after object header, the length is 4 dwords
    //
    if ( RtlCompareMemory( &noWakes.EnabledWoLPacketPatterns,
                            &pPmParameters->EnabledWoLPacketPatterns,
                            sizeof(DWORD)*4 )
            == sizeof(DWORD)*4) 
    {
        //
        // On non-AOAC platform, at S5, NDIS sends us with all 0 PM_PARAMETERS.
        // Treat it as not-arm-to-wake. Hence set it FALSE which should have 
        // been named bArmedToWake instead of bValidPmParametes
        //
        WFCInfo( "Recevied PM_PARAMETER OID with nothing enabled" );
        (void) pPort->SetbValidPmParameters( FALSE );
    }
    else
    {
        (void) pPort->SetbValidPmParameters( TRUE );
    }

    Exit:

    TraceActExit( ndisStatus );
    return ndisStatus;
}


//++ CGetPmParametersJob
NDIS_STATUS CGetPmParametersJob::QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesWritten,
        _In_ PUINT  pBytesNeeded )
{
    NDIS_STATUS  ndisStatus;
    CPort *pPort = NULL;
    PNDIS_PM_PARAMETERS pPmParameters;

    TraceActEntry();

    if ( m_NdisPortNumber != NDIS_DEFAULT_PORT_NUMBER )
    {
        ndisStatus = STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    pPort = m_pAdapter->GetPortFromNdisPortNumber( m_NdisPortNumber );
    NT_ASSERT( pPort );
    
    *pBytesNeeded = sizeof( NDIS_PM_PARAMETERS );

    if ( InformationBufferLength < sizeof( NDIS_PM_PARAMETERS ) )
    {
        ndisStatus = STATUS_BUFFER_TOO_SMALL;
        goto Exit;
    }

    pPmParameters = pPort->GetNdisPmParameters();
    RtlCopyMemory( pInformationBuffer, pPmParameters, sizeof( NDIS_PM_PARAMETERS));

    *pBytesWritten = sizeof( NDIS_PM_PARAMETERS );
    ndisStatus = NDIS_STATUS_SUCCESS;

Exit:

    TraceActExit( ndisStatus );
    return ndisStatus;
};

