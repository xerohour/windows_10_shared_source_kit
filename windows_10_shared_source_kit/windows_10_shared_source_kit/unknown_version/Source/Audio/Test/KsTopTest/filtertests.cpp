// Copyright(C) Microsoft.All rights reserved.

#include "common.h"


DWORD WINAPI CFilterReq::CheckPhysicalConnections(CDevice *pDevice)
{
    DWORD dwRet       = FNS_PASS;
    UINT  ii          = 0;
    UINT  jj          = 0; 
    UINT  iDestPin    = 0; 
    UINT  iDestFilter = 0;

    // Perform the analysis of the physical connections for all
    // the filters
    if(!GetPinPhysicalConnections(pDevice))
    {
        return FNS_FAIL;
    }
    
    // Loop through the physical connections and see if they
    // match, i.e. if: (Fil 1, Pin 1 -> Fil 2, Pin 2)
    // then it must be mirrored: (Fil 2, Pin 2 -> Fil 1, Pin 1)
    for(ii=0; ii < m_iFilterConnSize; ++ii)
    {
        for(jj=0; jj < m_pFilterConn[ii].iPhysicalConnSize; ++jj)
        {
            switch(m_pFilterConn[ii].pPhysicalConn[jj].iDestFilterId)
            {
                case CONN_NONE:
                case CONN_DIFF_DEVICE:
                    break;
                    //XLOG(XFAIL, eError, " FAIL: Filter[%d] connected to another filter"
                    //             " of a different device at Pin[%d]", ii, jj);
                    //dwRet = FNS_FAIL;
                default:
                    iDestFilter = m_pFilterConn[ii].pPhysicalConn[jj].iDestFilterId;
                    iDestPin =  m_pFilterConn[ii].pPhysicalConn[jj].iDestPinId;
                    if( (m_pFilterConn[iDestFilter].pPhysicalConn[iDestPin].iDestFilterId != ii)
                        && (m_pFilterConn[iDestFilter].pPhysicalConn[iDestPin].iDestPinId != jj))
                    {
                        XLOG(XFAIL, 
                             eError, 
                             " FAIL: Filter[%d], Pin [%d] connection to "
                             " Filter[%d], Pin[%d] not mirrored by Filter[%d]", 
                             ii, 
                             jj, 
                             iDestFilter, 
                             iDestPin, 
                             iDestFilter);
                        dwRet = FNS_FAIL;
                    }
                    break;
            }
        }
    }

    return dwRet;
}


DWORD WINAPI CFilterReq::CheckKsComponentId(CDevice *pDevice)
{
    DWORD                       dwRet = FNS_PASS;
    CFilter                    *pFilter = NULL;
    UINT                        cFilter = 0;
    KSCOMPONENTID               CompId = {0};
    CAtlArray<KSCOMPONENTID>    CompIdList;

    if (NULL == pDevice)
    {
        XLOG(XFAIL, 1, "ERROR: Unable to obtain test device.");
        return (FNS_FAIL);
    }
    SLOG(eInfo1, "Device: %s (%s)", pDevice->GetFriendlyName(), pDevice->GetInterfaceId());

    // Loop through all filters for each device
    cFilter = pDevice->GetFilterCount();
    for (UINT iFilter = 0; iFilter < cFilter; ++iFilter)
    {
        pFilter = pDevice->GetFilter(iFilter);
        if ( (NULL == pFilter) || 
             !(pFilter->IsValid()) )
        {
            XLOG(XFAIL, 1, " ERROR: Unable to obtain Filter[%d].", iFilter);
            dwRet = FNS_FAIL;
            //Check other filters
            continue;           
        }
        SLOG(eInfo1, " Filter: %s", pFilter->GetFriendlyName());

        DWORD dwResComp = GetKsComponentId(pFilter, &CompId);
        // This property is optiional. So do not fail if property
        // is not implemented or supported
        if(0 == dwResComp)
        {
            CompIdList.Add(CompId);
            LogKsComponentId(XMSG, 10, CompId);
        }
        else if( (ERROR_NOT_SUPPORTED != dwResComp) &&
                 (ERROR_CALL_NOT_IMPLEMENTED != dwResComp) &&
                 (ERROR_SET_NOT_FOUND != dwResComp) &&
                 (ERROR_NOT_FOUND != dwResComp) &&
                 (ERROR_INVALID_FUNCTION != dwResComp) )
        {
            XLOG(XFAIL, 1, "  FAIL: DeviceIoControl call for property " 
                           "KSPROPERTY_GENERAL_COMPONENTID failed with error %d.\n"
                           "        If property is not supported, valid "
                           "failures returned should be ERROR_SET_NOT_FOUND, "
                           "ERROR_NOT_SUPPORTED, ERROR_CALL_NOT_IMPLEMENTED, "
                           "ERROR_NOT_FOUND, or ERROR_INVALID_FUNCTION",
                           dwResComp);
            dwRet = FNS_FAIL;
        }
        else
        {
            XLOG(XMSG, 2, "  KSPROPERTY_GENERAL_COMPONENTID is not supported "
                          "for this filter.");
        }
    }// For Loop m_iFilter

    // Make sure that all KSCOMPONENTIDs are the same for filters that
    // support the property
    size_t cCompIdList = CompIdList.GetCount();
    for(size_t i = 1; i < cCompIdList; ++i)
    {
        KSCOMPONENTID prev, curr;
        prev = CompIdList.GetAt(i-1);
        curr = CompIdList.GetAt(i);
        if(0 != memcmp(&prev, &curr, sizeof(KSCOMPONENTID)))
        {
            XLOG(XFAIL, 1, "  FAIL: KsComponentId values are not the "
                           "same for all the filters of one device.");
            dwRet = FNS_FAIL;
        }
    }

    return dwRet;
}

bool CFilterReq::GetPinPhysicalConnections(CDevice *pDevice)
{
    bool        fRet         = TRUE;
    UINT        iFilter      = 0; 
    UINT        iPin         = 0; 
    UINT        cPin         = 0;
    CFilter     *pFilter     = NULL;
    CPinFactory *pPinFactory = NULL;

    // Get pointer to a CDevice object
    if (NULL == pDevice || 
        !(pDevice->IsValid()))
    {
        XLOG(XFAIL, 
             eError, 
             "ERROR: Unable to obtain test device.");
        fRet = false;
        goto Exit;
    }
    SLOG(eInfo1, 
         "Device: %s (%s)", 
         pDevice->GetFriendlyName(), 
         pDevice->GetInterfaceId());

    // Create required objects
    m_iFilterConnSize = pDevice->GetFilterCount();
    m_pFilterConn     = new FILTER_CONN[m_iFilterConnSize];
    if(NULL == m_pFilterConn)
    {
        XLOG(XFAIL, 
             eError, 
             "ERROR: Unable to allocate memory");
        fRet = false;
        goto Exit;
    }

    for(iFilter = 0; iFilter < m_iFilterConnSize; ++iFilter)
    {
        pFilter = pDevice->GetFilter(iFilter);
        if((NULL == pFilter) || 
           !(pFilter->IsValid()))
        {
            XLOG(XFAIL, 
                 eError, 
                 " ERROR: Unable to obtain Filter[%d].", 
                 iFilter);
            fRet = false;
            goto Exit;
        }

        cPin                                     = pFilter->GetCountPins();
        m_pFilterConn[iFilter].iPhysicalConnSize = cPin;
        m_pFilterConn[iFilter].pPhysicalConn     = new PHYS_CONN[cPin];
        if(NULL == m_pFilterConn[iFilter].pPhysicalConn)
        {
            XLOG(XFAIL, 
                 eError, 
                 " Unable to allocate memory");
            fRet = false;
            goto Exit;
        }

        for(iPin = 0; iPin < cPin; ++iPin)
        {
            pPinFactory = pFilter->GetPinFactory(iPin);
            if((NULL == pPinFactory) || 
               !(pPinFactory->IsValid()))
            {
                XLOG(XFAIL, 
                     eError, 
                     " ERROR: Unable to obtain Pin[%d].", 
                     iPin);
                fRet = false;
                goto Exit;
            }
            
            if(!GetFilterId(pDevice, 
                            pPinFactory->GetPhysicalConnectionLink(), 
                            m_pFilterConn[iFilter].pPhysicalConn[iPin].iDestFilterId))
            {

                fRet = false;
                goto Exit;
            }

            m_pFilterConn[iFilter].pPhysicalConn[iPin].iDestPinId = pPinFactory->GetPhysicalConnectionPin();
        }
    }

Exit:
    return fRet;
}

// This function is only called by other members of this class 
// and is assumed to have a valid value of pDevice passed to it.
bool CFilterReq::GetFilterId( _In_ CDevice *pDevice, _In_ LPCSTR szSymLinkName, _Out_ UINT &Id)
{
    bool        fRet = true;
    CFilter    *pFilter = NULL;
    UINT        iFilter = 0, 
                iFilterSize = pDevice->GetFilterCount();
    char        szDevicePath[MAX_BUFFER_SIZE]={0};


    if(*szSymLinkName == '\0')
    {
        Id = CONN_NONE;
        return true;
    }
    else
    {
        Id = CONN_DIFF_DEVICE;
    }

    for (iFilter = 0; iFilter < iFilterSize; ++iFilter)
    {
        // Get pointer to filter
        pFilter = pDevice->GetFilter(iFilter);
        if( (NULL == pFilter) || !(pFilter->IsValid()) )
        {
            XLOG(XFAIL, eError, " ERROR: Unable to obtain Filter[%d].", iFilter);
            fRet = false;
            break;
        }

        // Get DevicePath and fix it to be in a similar format
        // to the SymbolicLinkName
        if(pFilter->GetDevicePath() != NULL)
        {
            strncpy_s(szDevicePath, MAX_BUFFER_SIZE, pFilter->GetDevicePath(), MAX_BUFFER_SIZE-1);
            szDevicePath[MAX_BUFFER_SIZE-1] = 0;
            if(szDevicePath[1] == '\\')
            {
                szDevicePath[1] = '?';
            }
            else
            {
                XLOG(XFAIL, eError, " ERROR: Problem with Device Path value.");
                fRet = false;
                break;
            }
        }

        //Compare SymbolicLinkName to DevicePath
        if(_stricmp(szSymLinkName, szDevicePath) == 0)
        {
            Id = iFilter;
            break;
        }
    }

    return fRet;
}

bool CFilterReq::CheckInternalConnectionsHelper(void *pArg)
{
    bool            fRet = true;
    CFilterReq     *pThis = static_cast<CFilterReq*>(pArg);
    CFilterTopoGraph filterTopoGraph;

    if(!filterTopoGraph.BuildFromFilter(pThis->m_pFilter))
    {
        XLOG(XFAIL, eError, "  ERROR: Unable to build filter topology.");

        return false;
    }

    // loop through all the nodes contained within this filter
    for(ULONG nodeId = 0; nodeId < filterTopoGraph.GetNodeVertexCount(); nodeId++)
    {
        size_t nodePinCount = 0;

        // Validation #1: Ensure that nodes are numbered 0->[number of nodes-1]
        NodeVertex* pNodeVertex = filterTopoGraph.GetNodeVertex(nodeId);
        if(!pNodeVertex)
        {
            XLOG(XFAIL, eError, "  FAIL: This filter has %Iu nodes.  Node Ids must range from 0 to %Iu but Node Id %u does not exist.",
                filterTopoGraph.GetNodeVertexCount(), filterTopoGraph.GetNodeVertexCount() - 1, nodeId);
            fRet = false;

            continue;
        }

        nodePinCount = pNodeVertex->GetInputPinCount() + pNodeVertex->GetOutputPinCount();

        for(ULONG nodePinId = 0; nodePinId < nodePinCount; nodePinId++)
        {
            int inputConnectionCount = pNodeVertex->GetNodeInputPinConnectionCount(nodePinId);
            int outputConnectionCount = pNodeVertex->GetNodeOutputPinConnectionCount(nodePinId);

            // Validation #2: Ensure that node pins are numbered 0->[number of pins-1]
            if(0 == inputConnectionCount && 0 == outputConnectionCount)
            {
                // nodePinId does not exist, a pin is numbered incorrectly
                XLOG(XFAIL, eError, "  FAIL: Node %u has %Iu Node Pins.  Node Pin Ids must range from 0 to %Iu but Node Pin %u does not exist.",
                    pNodeVertex->GetNodeId(), nodePinCount, nodePinCount - 1, nodePinId);
                fRet = false;
            }

            // Validation #3: Ensure that a node pin is either for input or output, but not both
            else if(0 < inputConnectionCount && 0 < outputConnectionCount)
            {
                // this node pin has both input and output connections
                XLOG(XFAIL, eError, "  FAIL: Node Pin %u on Node %u has %u input connection(s) and %u output connection(s).  A node pin cannot have both input and output connections.",
                    nodePinId, pNodeVertex->GetNodeId(), inputConnectionCount, outputConnectionCount);
                fRet = false;
            }

            // Validation #4: Ensure that a node input pin's connection count is 1
            else if(1 < inputConnectionCount)
            {
                // connection count on a node input pin must be 1
                XLOG(XFAIL, eError, "  FAIL: Node Input Pin %u on Node %u has %d input connections.  Expected 1.", nodePinId,
                    pNodeVertex->GetNodeId(), inputConnectionCount);
                fRet = false;
            }

            // Validation #5: Ensure that a node output pin's connection count is 1 or more
            // if(0 == inputConnectionCount && 1 > outputConnectionCount)
            // this is caught by "Validation #2" step above

        } // nodePinId
    } // nodeId

    return fRet;
}

DWORD CFilterReq::GetKsComponentId( _In_ CFilter *pFilter, _Out_ KSCOMPONENTID *CompId)
{
    DWORD                       dwRet = 0;
    HANDLE                      hFilter = pFilter->GetHandle();
    ULONG                       cbReturned = 0;
    KSPROPERTY                  ksp = {0};

    if( NULL == CompId )
    {
        XLOG(XFAIL, eError, "CompId passed to GetKsComponentId is NULL");
        return false;
    }

    ZeroMemory(CompId, sizeof(KSCOMPONENTID));

    if(!hFilter)
    {
        XLOG(XFAIL, eError, "Invalid filter handle");
        return false;
    }

    // Initialize KSPROPERTY for support query
    ksp.Set = KSPROPSETID_General;
    ksp.Id = KSPROPERTY_GENERAL_COMPONENTID;
    ksp.Flags = KSPROPERTY_TYPE_GET;

    if(!DeviceIoControl(hFilter, IOCTL_KS_PROPERTY, &ksp, sizeof(ksp), 
                           CompId, sizeof(KSCOMPONENTID), &cbReturned, NULL))
    {
        dwRet = GetLastError();
    }
    
    return dwRet;
}

void CFilterReq::LogKsComponentId(XLOGTYPE XValue, UINT LogLevel, KSCOMPONENTID CompId)
{
    XLOG(XValue, LogLevel, 
         " KsComponentId found for this filter\n"
         "  KSCOMPONENTID.Manufacturer = "
         "{%.8lX-%.4hX-%.4hX-%.2X%.2X-%.2X%.2X%.2X%.2X%.2X%.2X}\n"
         "  KSCOMPONENTID.Product = "
         "{%.8lX-%.4hX-%.4hX-%.2X%.2X-%.2X%.2X%.2X%.2X%.2X%.2X}\n"
         "  KSCOMPONENTID.Component = "
         "{%.8lX-%.4hX-%.4hX-%.2X%.2X-%.2X%.2X%.2X%.2X%.2X%.2X}\n"
         "  KSCOMPONENTID.Name = "
         "{%.8lX-%.4hX-%.4hX-%.2X%.2X-%.2X%.2X%.2X%.2X%.2X%.2X}\n"
         "  KSCOMPONENTID.Version = %d\n"
         "  KSCOMPONENTID.Revision = %d\n",
         CompId.Manufacturer.Data1,    CompId.Manufacturer.Data2,
         CompId.Manufacturer.Data3,    CompId.Manufacturer.Data4[0],
         CompId.Manufacturer.Data4[1], CompId.Manufacturer.Data4[2],
         CompId.Manufacturer.Data4[3], CompId.Manufacturer.Data4[4],
         CompId.Manufacturer.Data4[5], CompId.Manufacturer.Data4[6],
         CompId.Manufacturer.Data4[7],
         CompId.Product.Data1,          CompId.Product.Data2,
         CompId.Product.Data3,          CompId.Product.Data4[0],
         CompId.Product.Data4[1],       CompId.Product.Data4[2],
         CompId.Product.Data4[3],       CompId.Product.Data4[4],
         CompId.Product.Data4[5],       CompId.Product.Data4[6],
         CompId.Product.Data4[7],
         CompId.Component.Data1,        CompId.Component.Data2,
         CompId.Component.Data3,        CompId.Component.Data4[0],
         CompId.Component.Data4[1],     CompId.Component.Data4[2],
         CompId.Component.Data4[3],     CompId.Component.Data4[4],
         CompId.Component.Data4[5],     CompId.Component.Data4[6],
         CompId.Component.Data4[7],
         CompId.Name.Data1,             CompId.Name.Data2,
         CompId.Name.Data3,             CompId.Name.Data4[0],
         CompId.Name.Data4[1],          CompId.Name.Data4[2],
         CompId.Name.Data4[3],          CompId.Name.Data4[4],
         CompId.Name.Data4[5],          CompId.Name.Data4[6],
         CompId.Name.Data4[7],
         CompId.Version, CompId.Revision);
}