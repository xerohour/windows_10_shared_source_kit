// Copyright(C) Microsoft.All rights reserved.

class CFilterReq: private CKSTopoDevLoop
{
public:
    CFilterReq()        
        { 
            m_pFilterConn = NULL; 
            m_iFilterConnSize = 0; 
        }
    ~CFilterReq()       
        { 
            if(m_pFilterConn != NULL)
            {
                for(UINT i=0; i < m_iFilterConnSize; ++i)
                {
                    if(m_pFilterConn[i].pPhysicalConn != NULL)
                    {
                        delete[] m_pFilterConn[i].pPhysicalConn;
                    }
                }
                delete[] m_pFilterConn;
            }
        }
    DWORD WINAPI CheckPhysicalConnections(CDevice *);
    DWORD WINAPI CheckInternalConnections(CDevice *pDevice)
        {
            return (LoopFilters(pDevice, 
                &CFilterReq::CheckInternalConnectionsHelper, static_cast<void*>(this)));                 
        }
    DWORD WINAPI CheckKsComponentId(CDevice *);

    // When a connection is read, it contains a pair of 
    // ((FromNode, FromPin), (ToNode, ToPin))
    // When the connection starts/ends at a Pin and not a node,
    // the FromNode/ToNode is set to (UINT)(-1)
    static const UINT PIN_CONN = (UINT)(-1);

    static const UINT CONN_NONE = UINT_MAX;
    static const UINT CONN_DIFF_DEVICE = UINT_MAX-1;
    
    static bool GetFilterId(_In_ CDevice*, _In_ LPCSTR, _Out_ UINT &);

private:
    // Physical Connections between filters of one device
    FILTER_CONN    *m_pFilterConn;
    UINT            m_iFilterConnSize;
  
    bool GetPinPhysicalConnections(CDevice *);
    DWORD GetKsComponentId( _In_ CFilter *, _Out_ KSCOMPONENTID *);
    void  LogKsComponentId(XLOGTYPE XValue, UINT LogLevel, KSCOMPONENTID CompId);
    static bool CheckInternalConnectionsHelper(void *);
};