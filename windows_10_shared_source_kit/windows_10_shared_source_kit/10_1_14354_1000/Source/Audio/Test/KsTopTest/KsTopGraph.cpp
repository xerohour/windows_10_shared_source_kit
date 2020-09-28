// Copyright(C) Microsoft.All rights reserved.

#include "common.h"

CMatrix::~CMatrix()
{
    if(m_M != NULL)
    {
        for(UINT i=0; i<m_iRowNum; ++i)
        {
            if(m_M[i] != NULL)
            {
                delete[] m_M[i];
            }
        }
        delete[] m_M;
        m_M = NULL;
    }
    m_iRowNum = m_iColNum = 0;
}


bool CMatrix::Create(UINT a, UINT b)
{
    m_iRowNum = a;
    m_iColNum = b;
    m_M = NULL;
    
    if((m_iRowNum > 0) && (m_iColNum > 0))
    {
        m_M = new UINT* [m_iRowNum];
        if(m_M == NULL)
        {
            return false;
        }
        for(UINT i=0; i < m_iRowNum; ++i)
        {
            m_M[i] = new UINT[m_iColNum];
            if(m_M[i] == NULL)
            {
                return false;
            }
            SecureZeroMemory(m_M[i], sizeof(UINT)*m_iColNum);
        }
    }
    return true;
}

CMatrix& CMatrix::operator=(const CMatrix &A)
{
    this->~CMatrix();
    this->Create(A.m_iRowNum, A.m_iColNum);

    for(UINT i=0; i < m_iRowNum; ++i)
    {
        memcpy((void*)m_M[i], (void*)A.m_M[i], 
                            sizeof(UINT)*m_iColNum);
    }

    return (*this);
}

const CMatrix CMatrix::operator*(const CMatrix &A)
{
    UINT ii=0, jj=0, kk=0;
    CMatrix C;
    
    if(m_iColNum == A.m_iRowNum)
    {
        C.Create(m_iRowNum, A.m_iColNum);
        for(ii=0; ii < m_iRowNum; ++ii)
        {
            for(jj=0; jj < A.m_iColNum; ++jj)
            {
                for(kk=0; kk < m_iColNum; ++kk)
                {
                    C.m_M[ii][jj] += m_M[ii][kk] * A.m_M[kk][jj];
                }
            }
        }
    }

    return C;
}

const CMatrix& CMatrix::operator|=(const CMatrix &A)
{
    UINT ii=0, jj=0;
    
    if((m_iRowNum == A.m_iRowNum) && (m_iColNum == A.m_iColNum))
    {
        for(ii=0; ii < m_iRowNum; ++ii)
        {
            for(jj=0; jj < m_iColNum; ++jj)
            {
                m_M[ii][jj] = m_M[ii][jj] | A.m_M[ii][jj];
            }
        }
    }

    return (*this);
}

const CMatrix CMatrix::SolveAdjacency()
{
    CMatrix Result;

    if(m_iRowNum == m_iColNum)
    {
        Result = *this;
        for(UINT i=0; i < m_iRowNum; ++i)
        {
            Result |= Result*(*this);
        }
    }
    return Result;
}

void CMatrix::Print(LPCSTR message)
{
    UINT i=0, j=0;
    const UINT MAX_BUFFER = 1000;
    char temp[10]={0}, buffer[MAX_BUFFER]={0};
    
    XLOG(XFAIL, eError, "%s", message);
    for(i=0; i < m_iRowNum; ++i)
    {
        buffer[0] = '\0';
        for(j=0; j < m_iColNum; ++j)
        {
            temp[0] = '\0';
            _snprintf_s(temp, 
                        sizeof(temp)/sizeof(temp[0]), 
                        ((sizeof(temp)/sizeof(temp[0])) - 1),
                        "%d ", m_M[i][j]);
            temp[(sizeof(temp)/sizeof(temp[0])) - 1] = 0;
            strcat_s(buffer, MAX_BUFFER, temp);
        }
        XLOG(XFAIL, eError, "%s", buffer);
    }
}




KsDeviceInfo::KsDeviceInfo()
{
    pDevice = NULL;
    ppFilter = NULL;
    piPinCount = piNodeCount = NULL;
    iFilterCount = 0;
}

KsDeviceInfo::~KsDeviceInfo()
{
    if(ppFilter != NULL)
    {
        delete[] ppFilter;
        ppFilter = NULL;
    }
    if(piPinCount != NULL)
    {
        delete[] piPinCount;
        piPinCount = NULL;
    }
    if(piNodeCount != NULL)
    {
        delete[] piNodeCount;
        piNodeCount = 0;
    }
}

bool KsDeviceInfo::Create(CDevice *pDev, UINT iDevice, 
                          UINT &iPinCountTotal, UINT &iNodeCountTotal, UINT &iVertexCount)
{
    bool    fRet = true;
    UINT    iFilter = 0;

    if(pDev == NULL)        return false;

    pDevice = pDev;
    iFilterCount = pDevice->GetFilterCount();
    ppFilter = new CFilter* [iFilterCount];
    piNodeCount = new UINT [iFilterCount];
    piPinCount = new UINT [iFilterCount];
    if(!ppFilter || !piNodeCount || !piPinCount)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to allocate memory");
        fRet = false;
        goto Exit;
    }

    for(iFilter = 0; iFilter < iFilterCount; ++iFilter)
    {
        ppFilter[iFilter] = pDevice->GetFilter(iFilter);
        if( (NULL == ppFilter[iFilter]) || !(ppFilter[iFilter]->IsValid()) )
        {
            XLOG(XFAIL, eError, "ERROR: Unable to obtain information "
                            "on Device[%d] - Filter[%d]", iDevice, iFilter);
            fRet = false;
            goto Exit;
        }

        piPinCount[iFilter] = ppFilter[iFilter]->GetCountPins();
        piNodeCount[iFilter] = ppFilter[iFilter]->GetCountNodes();

        iPinCountTotal += piPinCount[iFilter];
        iNodeCountTotal += piNodeCount[iFilter];
        iVertexCount += piPinCount[iFilter] + piNodeCount[iFilter];
    }  

Exit:
    return fRet;
}




CKsGraph::CKsGraph()
{
    m_pVertex = NULL;
    m_pDevInfo = NULL;
    m_iVertexCount = m_iDevInfoCount = m_iPinCountTotal = m_iNodeCountTotal = 0;
}

CKsGraph::~CKsGraph()
{
    UINT i=0;

    if(m_pVertex != NULL)
    {
        delete[] m_pVertex;
        m_pVertex = NULL;
    }
    if(m_pDevInfo != NULL)
    {
        for(i=0; i < m_iDevInfoCount; ++i)
        {
            m_pDevInfo[i].~KsDeviceInfo();
        }
        delete[] m_pDevInfo;
        m_pDevInfo = NULL;
    }
}

bool CKsGraph::BuildFromDevices(CEnumDevice *pEnumDevice, CKsDevice *pDevice)
{
    bool        fRet = true;
    UINT        iDevice = 0, iFilter = 0;

    if(!pEnumDevice)
    {
        return false;
    }

    SLOG(eBlab2, "Begin initializing graph...");

    if(!AllocateGraphMemory(pEnumDevice, pDevice, iDevice))
    {
        fRet = false;
        goto Exit;
    }

    // Populate the graph with the pins and nodes for each graph from
    // its corresponding device
    SLOG(eBlab2, "...Parsing information in Device[%d] (%s)",
                 iDevice, m_pDevInfo[iDevice].pDevice->GetFriendlyName());
  
    for(iFilter = 0; iFilter < m_pDevInfo[iDevice].iFilterCount; ++iFilter)
    {
        SLOG(eBlab2, " ...Parsing information in Filter[%d]", iFilter);
  
        if(   !PopulateVertexPins(iDevice, iFilter)
           || !PopulateVertexNodes(iDevice, iFilter)
           || !PopulateEdges(iDevice, iFilter)
          )
        {
            fRet = false;
            goto Exit;
        }
    }

Exit:
    if(true == fRet)
    {
        SLOG(eBlab2, "...Successfully completed initializing graph.");
    }
    else
    {
        SLOG(eBlab2, "...Initializing graph UNSUCCESSFUL!!");
    }
    return fRet;
}

DWORD WINAPI CKsGraph::CheckBridgeCommPath()
{
    DWORD       dwRet = FNS_PASS;
    CMatrix     Paths;
    UINT        ii=0, jj=0;
    bool        fIsConnected = false;

    Paths = m_Edge.SolveAdjacency();

    for(ii=0; ii < m_iVertexCount; ++ii)
    {
        switch(m_pVertex[ii].iType)
        {
            case VERTEX_TYPE_COMM_PIN:
                if(KSNODETYPE_AUDIO_LOOPBACK == m_pVertex[ii].Pin.guidCategory)
                {
                    if(KSPIN_DATAFLOW_OUT != m_pVertex[ii].Pin.iDataFlow)
                    {
                        dwRet = FNS_FAIL;
                        XLOG(XFAIL, eError, "  Device[%d] (%s) - Filter[%d] (%s): Pin[%d]"
                                    " is a loopback communications pin with data flow %u "
                                    " instead of KSPIN_DATAFLOW_OUT.", 
                                    m_pVertex[ii].iDeviceId, 
                                    m_pDevInfo[m_pVertex[ii].iDeviceId].pDevice->GetFriendlyName(),
                                    m_pVertex[ii].iFilterId, 
                                    m_pDevInfo[m_pVertex[ii].iDeviceId].ppFilter[m_pVertex[ii].iFilterId]->GetFriendlyName(),
                                    m_pVertex[ii].Pin.iPinId,
                                    m_pVertex[ii].Pin.iDataFlow);
                    }
                    
                    fIsConnected = false;
                    for(jj=0; jj < m_iVertexCount; ++jj)
                    {
                        if(ii == jj)
                        {
                            continue;
                        }
                        
                        // for loopback, we want a path
                        // FROM an "in" streaming pin
                        // TO this "out" streaming pin
                        if( (Paths(jj, ii) == 1)
                             &&(m_pVertex[jj].iType == VERTEX_TYPE_COMM_PIN)
                             &&(m_pVertex[jj].Pin.iDataFlow == KSPIN_DATAFLOW_IN))
                        {
                            fIsConnected = true;
                            break;
                        }
                    }
                    if(! fIsConnected)
                    {
                        dwRet = FNS_FAIL;
                        XLOG(XFAIL, eError, "  Device[%d] (%s) - Filter[%d] (%s): Pin[%d]"
                                     " is a loopback communications pin that does not have"
                                     " a path from any KSPIN_DATAFLOW_IN communications pin.", 
                                     m_pVertex[ii].iDeviceId, 
                                     m_pDevInfo[m_pVertex[ii].iDeviceId].pDevice->GetFriendlyName(),
                                     m_pVertex[ii].iFilterId, 
                                     m_pDevInfo[m_pVertex[ii].iDeviceId].ppFilter[m_pVertex[ii].iFilterId]->GetFriendlyName(),
                                     m_pVertex[ii].Pin.iPinId);
                    }
                }
                else
                {
                    fIsConnected = false;
                    for(jj=0; jj < m_iVertexCount; ++jj)
                    {
                        if(ii == jj)
                        {
                            continue;
                        }
                        if( ((Paths(ii, jj) == 1) || (Paths(jj, ii) == 1))
                             &&(m_pVertex[jj].iType == VERTEX_TYPE_JACK_PIN))
                        {
                            fIsConnected = true;
                            break;
                        }
                    }
                    if(! fIsConnected)
                    {
                        dwRet = FNS_FAIL;
                        XLOG(XFAIL, eError, "  Device[%d] (%s) - Filter[%d] (%s): Pin[%d]"
                                     " is a Communication Pin that does not have"
                                     " a path to/from any Jack Pin.", 
                                     m_pVertex[ii].iDeviceId, 
                                     m_pDevInfo[m_pVertex[ii].iDeviceId].pDevice->GetFriendlyName(),
                                     m_pVertex[ii].iFilterId, 
                                     m_pDevInfo[m_pVertex[ii].iDeviceId].ppFilter[m_pVertex[ii].iFilterId]->GetFriendlyName(),
                                     m_pVertex[ii].Pin.iPinId);
                    }
                }
                break;

            case VERTEX_TYPE_JACK_PIN:
                fIsConnected = false;
                for(jj=0; jj < m_iVertexCount; ++jj)
                {
                    if(ii == jj)
                    {
                        continue;
                    }
                    if( ((Paths(ii, jj) == 1) || (Paths(jj, ii) == 1))
                        &&(m_pVertex[jj].iType == VERTEX_TYPE_COMM_PIN ||
                           m_pVertex[jj].iType == VERTEX_TYPE_COMM_AEHOST ||
                           m_pVertex[jj].iType == VERTEX_TYPE_COMM_AEOFFLOAD ||
                           m_pVertex[jj].iType == VERTEX_TYPE_COMM_AELOOPBACK))
                    {
                        fIsConnected = true;
                        break;
                    }
                }
                if(! fIsConnected)
                {
                    dwRet = FNS_FAIL;
                    XLOG(XFAIL, eError, "  Device[%d] (%s) - Filter[%d] (%s): Pin[%d]"
                                 " is a Jack Pin that does not have"
                                 " a path to/from any Communication Pin.", 
                                 m_pVertex[ii].iDeviceId, 
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].pDevice->GetFriendlyName(),
                                 m_pVertex[ii].iFilterId, 
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].ppFilter[m_pVertex[ii].iFilterId]->GetFriendlyName(),
                                 m_pVertex[ii].Pin.iPinId);
                }
                break;
            case VERTEX_TYPE_COMM_AEHOST:
                fIsConnected = false;
                for(jj=0; jj < m_iVertexCount; ++jj)
                {
                    if(ii == jj)
                    {
                        continue;
                    }
                    // path has to be from pin to node
                    if( (Paths(ii, jj) == 1)
                        &&(m_pVertex[jj].iType == VERTEX_TYPE_AENODE))
                    {
                        fIsConnected = true;
                        break;
                    }
                    if( (Paths(jj, ii) == 1)
                        &&(m_pVertex[jj].iType == VERTEX_TYPE_AENODE))
                    {
                        dwRet = FNS_FAIL;
                        XLOG(XFAIL, eError, "  Device[%d] (%s) - Filter[%d] (%s): Pin[%d]"
                                 " is a Hardware Host Pin that has a path FROM an Audio Engine Node", 
                                 m_pVertex[ii].iDeviceId, 
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].pDevice->GetFriendlyName(),
                                 m_pVertex[ii].iFilterId, 
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].ppFilter[m_pVertex[ii].iFilterId]->GetFriendlyName(),
                                 m_pVertex[ii].Pin.iPinId);
                    }
                }
                if(! fIsConnected)
                {
                    dwRet = FNS_FAIL;
                    XLOG(XFAIL, eError, "  Device[%d] (%s) - Filter[%d] (%s): Pin[%d]"
                                 " is a Hardware Host Pin that does not have"
                                 " a direct path from a Audio Engine Node", 
                                 m_pVertex[ii].iDeviceId, 
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].pDevice->GetFriendlyName(),
                                 m_pVertex[ii].iFilterId, 
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].ppFilter[m_pVertex[ii].iFilterId]->GetFriendlyName(),
                                 m_pVertex[ii].Pin.iPinId);
                }

                fIsConnected = false;
                for(jj=0; jj < m_iVertexCount; ++jj)
                {
                    if(ii == jj)
                    {
                        continue;
                    }
                    if( ((Paths(ii, jj) == 1) || (Paths(jj, ii) == 1))
                         &&(m_pVertex[jj].iType == VERTEX_TYPE_JACK_PIN))
                    {
                        fIsConnected = true;
                        break;
                    }
                }
                if(! fIsConnected)
                {
                    dwRet = FNS_FAIL;
                    XLOG(XFAIL, eError, "  Device[%d] (%s) - Filter[%d] (%s): Pin[%d]"
                                 " is a Hardware Host Pin that does not have"
                                 " a path to/from any Jack Pin.", 
                                 m_pVertex[ii].iDeviceId, 
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].pDevice->GetFriendlyName(),
                                 m_pVertex[ii].iFilterId, 
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].ppFilter[m_pVertex[ii].iFilterId]->GetFriendlyName(),
                                 m_pVertex[ii].Pin.iPinId);
                }
                break;

            case VERTEX_TYPE_COMM_AEOFFLOAD:
                fIsConnected = false;
                for(jj=0; jj < m_iVertexCount; ++jj)
                {
                    if(ii == jj)
                    {
                        continue;
                    }
                    // path has to be from pin to node
                    if( (Paths(ii, jj) == 1)
                        &&(m_pVertex[jj].iType == VERTEX_TYPE_AENODE))
                    {
                        fIsConnected = true;
                        break;
                    }
                    if( (Paths(jj, ii) == 1)
                        &&(m_pVertex[jj].iType == VERTEX_TYPE_AENODE))
                    {
                        dwRet = FNS_FAIL;
                        XLOG(XFAIL, eError, "  Device[%d] (%s) - Filter[%d] (%s): Pin[%d]"
                                 " is a Hardware Offload Pin that has a path FROM an Audio Engine Node", 
                                 m_pVertex[ii].iDeviceId, 
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].pDevice->GetFriendlyName(),
                                 m_pVertex[ii].iFilterId, 
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].ppFilter[m_pVertex[ii].iFilterId]->GetFriendlyName(),
                                 m_pVertex[ii].Pin.iPinId);
                    }
                }
                if(! fIsConnected)
                {
                    dwRet = FNS_FAIL;
                    XLOG(XFAIL, eError, "  Device[%d] (%s) - Filter[%d] (%s): Pin[%d]"
                                 " is a Hardware Offload Pin that does not have"
                                 " a direct path from a Audio Engine Node",
                                 m_pVertex[ii].iDeviceId,
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].pDevice->GetFriendlyName(),
                                 m_pVertex[ii].iFilterId,
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].ppFilter[m_pVertex[ii].iFilterId]->GetFriendlyName(),
                                 m_pVertex[ii].Pin.iPinId);
                }

                fIsConnected = false;
                for(jj=0; jj < m_iVertexCount; ++jj)
                {
                    if(ii == jj)
                    {
                        continue;
                    }
                    if( ((Paths(ii, jj) == 1) || (Paths(jj, ii) == 1))
                         &&(m_pVertex[jj].iType == VERTEX_TYPE_JACK_PIN))
                    {
                        fIsConnected = true;
                        break;
                    }
                }
                if(! fIsConnected)
                {
                    dwRet = FNS_FAIL;
                    XLOG(XFAIL, eError, "  Device[%d] (%s) - Filter[%d] (%s): Pin[%d]"
                                 " is a Hardware Offload Pin that does not have"
                                 " a path to/from any Jack Pin.",
                                 m_pVertex[ii].iDeviceId,
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].pDevice->GetFriendlyName(),
                                 m_pVertex[ii].iFilterId,
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].ppFilter[m_pVertex[ii].iFilterId]->GetFriendlyName(),
                                 m_pVertex[ii].Pin.iPinId);
                }
                break;

            case VERTEX_TYPE_COMM_AELOOPBACK:
                fIsConnected = false;
                for(jj=0; jj < m_iVertexCount; ++jj)
                {
                    if(ii == jj)
                    {
                        continue;
                    }
                    // path has to be from node to pin
                    if( (Paths(jj, ii) == 1)
                        &&(m_pVertex[jj].iType == VERTEX_TYPE_AENODE))
                    {
                        fIsConnected = true;
                        break;
                    }
                    if( (Paths(ii, jj) == 1)
                        &&(m_pVertex[jj].iType == VERTEX_TYPE_AENODE))
                    {
                        dwRet = FNS_FAIL;
                        XLOG(XFAIL, eError, "  Device[%d] (%s) - Filter[%d] (%s): Pin[%d]"
                                 " is a Hardware Loopback Pin that has a path TO an Audio Engine Node", 
                                 m_pVertex[ii].iDeviceId, 
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].pDevice->GetFriendlyName(),
                                 m_pVertex[ii].iFilterId, 
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].ppFilter[m_pVertex[ii].iFilterId]->GetFriendlyName(),
                                 m_pVertex[ii].Pin.iPinId);
                    }
                }
                if(! fIsConnected)
                {
                    dwRet = FNS_FAIL;
                    XLOG(XFAIL, eError, "  Device[%d] (%s) - Filter[%d] (%s): Pin[%d]"
                                 " is a Hardware Loopback Pin that does not have"
                                 " a direct path from a Audio Engine Node",
                                 m_pVertex[ii].iDeviceId,
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].pDevice->GetFriendlyName(),
                                 m_pVertex[ii].iFilterId,
                                 m_pDevInfo[m_pVertex[ii].iDeviceId].ppFilter[m_pVertex[ii].iFilterId]->GetFriendlyName(),
                                 m_pVertex[ii].Pin.iPinId);
                }
                break;

            default:
                break;
        }
    }

    return dwRet;
}

bool CKsGraph::AllocateGraphMemory(CEnumDevice *pEnumDevice, CKsDevice *pDevice, UINT &iDeviceIndex)
{
    bool        fRet = true;
    CDevice    *pDev = 0;
    UINT        iDevice = 0;

    if(NULL == pDevice)
    {
       XLOG(XFAIL, eError, "ERROR: Invalid Parameter.");
       fRet = false;
       goto Exit;
    }
 
    // Allocate memory to keep state of Device, Filters and 
    // Node and Pin counts for each filter in each device
    m_iDevInfoCount = pEnumDevice->GetCountDevices();
    m_pDevInfo = new KsDeviceInfo [m_iDevInfoCount];
    if(!m_pDevInfo)
    {
        XLOG(XFAIL, eError, "ERROR: Unable to allocate memory");
        fRet = false;
        goto Exit;
    }
    
    // Loop through the devices and filters and allocate memory
    // and fill up the state variables
    for(iDevice = 0; iDevice < m_iDevInfoCount; ++iDevice)
    {
        CDevice* pDeviceTemp = pDevice->GetTopoDevice();
        pDev = pEnumDevice->GetDevice(iDevice);
        if((NULL == pDev) || !(pDev->IsValid()))
        {
            XLOG(XFAIL, eError, "ERROR: Unable to obtain information"
                         " on Device[%d]", iDevice);
            fRet = false;
            goto Exit;
        }

        if(_stricmp(pDev->GetInterfaceId(), pDeviceTemp->GetInterfaceId()) == 0)
        {        
           iDeviceIndex = iDevice;
        }

        if(!m_pDevInfo[iDevice].Create(pDev, iDevice, m_iPinCountTotal, m_iNodeCountTotal, m_iVertexCount))
        {
            fRet = false;
            goto Exit;
        }
    }

    // Allocate space for the Vertex table and the edge matrix
    m_pVertex = new KsGraphVertex [m_iVertexCount];
    if(!m_pVertex || !m_Edge.Create(m_iVertexCount, m_iVertexCount))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to allocate memory");
        fRet = false;
        goto Exit;
    }
    SecureZeroMemory(m_pVertex, sizeof(KsGraphVertex)*m_iVertexCount);

Exit:
    return fRet;
}


UINT CKsGraph::PinToVertexId(UINT iDevice, UINT iFilter, UINT iPin)
{
    UINT iRet = iPin, i = 0, j=0;

    for(i=0; i<iDevice; ++i)
    {
        for(j=0; j < m_pDevInfo[i].iFilterCount; ++j)
        {
            iRet += m_pDevInfo[i].piPinCount[j];
        }
    }
    for(i=0; i < iFilter; ++i)
    {
        iRet += m_pDevInfo[iDevice].piPinCount[i];
    }

    return iRet;
}

UINT CKsGraph::NodeToVertexId(UINT iDevice, UINT iFilter, UINT iNode)
{
    UINT iRet = iNode + m_iPinCountTotal,
         i = 0, j=0;

    for(i=0; i<iDevice; ++i)
    {
        for(j=0; j < m_pDevInfo[i].iFilterCount; ++j)
        {
            iRet += m_pDevInfo[i].piNodeCount[j];
        }
    }
    for(i=0; i < iFilter; ++i)
    {
        iRet += m_pDevInfo[iDevice].piNodeCount[i];
    }

    return iRet;
}

UINT CKsGraph::GetPinDestVertexIndex(LPCSTR szSymLinkName, UINT iPin)
{
    UINT        iRet = (ULONG)(-1);
    UINT        iDevice, iFilter = 0;
    char        szDevicePath[MAX_BUFFER_SIZE]={0};

    if(*szSymLinkName == '\0')
    {
        return iRet;
    }

    for(iDevice = 0; iDevice < m_iDevInfoCount; ++iDevice)
    {
        for (iFilter = 0; iFilter < m_pDevInfo[iDevice].iFilterCount; ++iFilter)
        {
            // Get DevicePath and fix it to be in a similar format
            // to the SymbolicLinkName
            if(m_pDevInfo[iDevice].ppFilter[iFilter]->GetDevicePath() != NULL)
            {
                strncpy_s(
                        szDevicePath, 
                        MAX_BUFFER_SIZE,
                        m_pDevInfo[iDevice].ppFilter[iFilter]->GetDevicePath(), 
                        MAX_BUFFER_SIZE-1);
                szDevicePath[MAX_BUFFER_SIZE-1]=0;
                if(szDevicePath[1] == '\\')
                {
                    szDevicePath[1] = '?';
                }
            }

            //Compare SymbolicLinkName to DevicePath
            if(_stricmp(szSymLinkName, szDevicePath) == 0)
            {
                iRet = PinToVertexId(iDevice, iFilter, iPin);
                return iRet;
            }
        }
    }
    return iRet;
}

bool CKsGraph::PopulateVertexPins(UINT iDevice, UINT iFilter)
{
    bool        fRet = true;
    CPinFactory *pPinFactory = NULL;
    UINT        iPin = 0, iConvPin = 0;

    for(iPin = 0; iPin < m_pDevInfo[iDevice].piPinCount[iFilter]; ++iPin)
    {
        pPinFactory = m_pDevInfo[iDevice].ppFilter[iFilter]->GetPinFactory(iPin);
        if( (NULL == pPinFactory) || !pPinFactory->IsValid())
        {
            XLOG(XFAIL, eError, "  ERROR: Unable to obtain Pin[%d]", iPin);
            fRet = false;
            break;
        }
        iConvPin = PinToVertexId(iDevice, iFilter, iPin);
        m_pVertex[iConvPin].iDeviceId = iDevice;
        m_pVertex[iConvPin].iFilterId = iFilter;
        m_pVertex[iConvPin].Pin.iPinId = iPin;
        m_pVertex[iConvPin].Pin.iCommType = pPinFactory->GetCommunication();
        m_pVertex[iConvPin].Pin.iDataFlow = pPinFactory->GetDataFlow();
        m_pVertex[iConvPin].Pin.guidCategory = *(pPinFactory->GetCategory());
        
        switch(m_pVertex[iConvPin].Pin.iCommType)
        {
            case KSPIN_COMMUNICATION_SINK:
            case KSPIN_COMMUNICATION_SOURCE:
            case KSPIN_COMMUNICATION_BOTH:
                // Pin is a Communication Pin
                m_pVertex[iConvPin].iType = VERTEX_TYPE_COMM_PIN;
                break;
            case KSPIN_COMMUNICATION_BRIDGE:
            case KSPIN_COMMUNICATION_NONE:
                m_pVertex[iConvPin].iType = ( (pPinFactory->GetPhysicalConnectionLink())[0] ? 
                                        VERTEX_TYPE_BRIDGE_PIN : VERTEX_TYPE_JACK_PIN);
                break;
            default:
                XLOG(XFAIL, eError, "  ERROR: Invalid Communication Type in Pin[%d]", iPin);
                fRet = false;
                break;
        }
        if(false == fRet)
        {
            break;
        }
    }

    return fRet;
}


bool CKsGraph::PopulateVertexNodes(UINT iDevice, UINT iFilter)
{
    bool        fRet = true;
    CNode       *pNode = NULL;
    UINT        iNode = 0, iConvNode = 0;

    // Place the verteces for the nodes after the pins
    for(iNode = 0; iNode < m_pDevInfo[iDevice].piNodeCount[iFilter]; ++iNode)
    {
        pNode = m_pDevInfo[iDevice].ppFilter[iFilter]->GetNode(iNode);
        if( (NULL == pNode) || !pNode->IsValid())
        {
            XLOG(XFAIL, eError, "  ERROR: Unable to obtain Node[%d]", iNode);
            fRet = false;
            break;
        }
        iConvNode = NodeToVertexId(iDevice, iFilter, iNode);
        m_pVertex[iConvNode].iDeviceId = iDevice;
        m_pVertex[iConvNode].iFilterId = iFilter;
        m_pVertex[iConvNode].iType = VERTEX_TYPE_NODE;
        m_pVertex[iConvNode].Node.iNodeId = iNode;
        m_pVertex[iConvNode].Node.guidNodeType = *(pNode->GetType());

        if( IsEqualGUID( KSNODETYPE_AUDIO_ENGINE, *(pNode->GetType() ) ) )
        {
            ULONG hostPinFactoryId = 0;
            ULONG offloadPinFactoryId = 0;
            ULONG loopbackPinFactoryId = 0;
            CAudioEngineNode aeNode( pNode );

            aeNode.GetHostPinFactoryId( &hostPinFactoryId );
            aeNode.GetOffloadPinFactoryId( &offloadPinFactoryId );
            aeNode.GetLoopbackPinFactoryId( &loopbackPinFactoryId );

            m_pVertex[PinToVertexId(iDevice, iFilter, hostPinFactoryId)].iType = VERTEX_TYPE_COMM_AEHOST;
            m_pVertex[PinToVertexId(iDevice, iFilter, offloadPinFactoryId)].iType = VERTEX_TYPE_COMM_AEOFFLOAD;
            m_pVertex[PinToVertexId(iDevice, iFilter, loopbackPinFactoryId)].iType = VERTEX_TYPE_COMM_AELOOPBACK;
            m_pVertex[iConvNode].iType = VERTEX_TYPE_AENODE;
        }
    }

    return fRet;
}

bool CKsGraph::PopulateEdges(UINT iDevice, UINT iFilter)
{
    bool            fRet = true;
    CConnection    *pConn = NULL;
    CPinFactory    *pPinFactory = NULL;
    UINT            iConn=0, iConnCount =0,
                    iPin = 0;
    UINT            iRow = 0, iCol = 0;

    for(iPin = 0; iPin < m_pDevInfo[iDevice].piPinCount[iFilter]; ++iPin)
    {
        pPinFactory = m_pDevInfo[iDevice].ppFilter[iFilter]->GetPinFactory(iPin);
        if( (NULL == pPinFactory) || !pPinFactory->IsValid())
        {
            XLOG(XFAIL, eError, "  ERROR: Unable to obtain Pin[%d]", iPin);
            fRet = false;
            goto Exit;
        }

        if(pPinFactory->GetDataFlow() == KSPIN_DATAFLOW_OUT)
        {
            iCol = GetPinDestVertexIndex(pPinFactory->GetPhysicalConnectionLink(),
                                             pPinFactory->GetPhysicalConnectionPin());
            if(iCol != (ULONG)(-1))
            {
                iRow = PinToVertexId(iDevice, iFilter, iPin);
                m_Edge(iRow, iCol) = 1;
            }
        }
    }

    iConnCount = m_pDevInfo[iDevice].ppFilter[iFilter]->GetCountConnections();
    for(iConn = 0; iConn < iConnCount; ++iConn)
    {
        pConn = m_pDevInfo[iDevice].ppFilter[iFilter]->GetConnection(iConn);
        if((NULL == pConn) || !pConn->IsValid())
        {
            XLOG(XFAIL, eError, "  ERROR: Unable to obtain Connection[%d]", iConn);
            fRet = false;
            goto Exit;
        }

        if(pConn->FromNode() != -1)
        {
            iRow = NodeToVertexId(iDevice, iFilter, pConn->FromNode());
        }
        else
        {
            iRow = PinToVertexId(iDevice, iFilter, pConn->FromNodePin());
        }
        if(pConn->ToNode() != -1)
        {
            iCol = NodeToVertexId(iDevice, iFilter, pConn->ToNode());
        }
        else
        {
            iCol = PinToVertexId(iDevice, iFilter, pConn->ToNodePin());
        }

        m_Edge(iRow, iCol) = 1;
    }

Exit:
    return fRet;
}
