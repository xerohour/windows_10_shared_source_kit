
#pragma once


class CMatrix
{
public:
    CMatrix()                           { m_M = NULL; m_iRowNum = m_iColNum = 0; }
    CMatrix(UINT a, UINT b)             { this->Create(a, b); }
    CMatrix(const CMatrix &A)           { m_M = NULL; m_iRowNum = m_iColNum = 0; *this = A; }
    ~CMatrix();
    
    bool Create(UINT, UINT);
    CMatrix& operator=(const CMatrix&);

    UINT GetRowNum()                    { return m_iRowNum; }
    UINT GetColNum()                    { return m_iColNum; }
    UINT& operator()(UINT a, UINT b)    { 
                                            return m_M[a][b]; 
                                        }
    const CMatrix operator*(const CMatrix&);
    const CMatrix& operator|=(const CMatrix&);

    const CMatrix SolveAdjacency();
    void Print(LPCSTR);

private:
    UINT  **m_M;
    UINT    m_iRowNum, m_iColNum;
};



enum KsGraphVertexType
{
    VERTEX_TYPE_NODE = 0,
    VERTEX_TYPE_AENODE,
    VERTEX_TYPE_JACK_PIN,
    VERTEX_TYPE_BRIDGE_PIN,
    VERTEX_TYPE_COMM_PIN,
    VERTEX_TYPE_COMM_AEHOST,
    VERTEX_TYPE_COMM_AEOFFLOAD,
    VERTEX_TYPE_COMM_AELOOPBACK,
};


struct KsGraphVertex
{
    KsGraphVertexType   iType;
    UINT                iDeviceId;
    UINT                iFilterId;

    // Node pVertex
    struct
    {
        UINT    iNodeId;
        GUID    guidNodeType;
    } Node;

    // There are three ways to differentiate pins
    // 1. JACK PINS have KSPIN_COMMUNICATION_BRIDGE or 
    //    KSPIN_COMMUNICATION_NONE with no physical connections
    // 2. COMMUNICATION PIN have KSPIN_COMMUNICATION_SINK,
    //    KSPIN_COMMUNICATION_SOURCE or KSPIN_COMMUNICATION_BOTH
    // 3. BRIDGE PIN have KSPIN_COMMUNICATION_BRIDGE or 
    //    KSPIN_COMMUNICATION_NONE with physical connections
    struct
    {
        UINT    iPinId;
        UINT    iCommType;
        UINT    iDataFlow;
        GUID    guidCategory;
    } Pin;

};


struct KsDeviceInfo
{
    KsDeviceInfo();
    ~KsDeviceInfo();
    bool Create(CDevice *, UINT, UINT&, UINT&, UINT&);

    CDevice    *pDevice;
    CFilter   **ppFilter;
    UINT        iFilterCount,
               *piPinCount,
               *piNodeCount;
};

class CKsGraph
{
public:
    CKsGraph();
    ~CKsGraph();
    bool BuildFromDevices(CEnumDevice *, CKsDevice *);
    DWORD WINAPI CheckBridgeCommPath();

private:
    KsGraphVertex  *m_pVertex;
    CMatrix         m_Edge;
    UINT            m_iVertexCount;

    KsDeviceInfo   *m_pDevInfo;
    UINT            m_iDevInfoCount;
    UINT            m_iPinCountTotal;
    UINT            m_iNodeCountTotal;

    bool AllocateGraphMemory(CEnumDevice *, CKsDevice *, UINT&);
    UINT PinToVertexId(UINT, UINT, UINT);
    UINT NodeToVertexId(UINT, UINT, UINT);
    UINT GetPinDestVertexIndex(LPCSTR, UINT);
    bool PopulateVertexPins(UINT, UINT);
    bool PopulateVertexNodes(UINT, UINT);
    bool PopulateEdges(UINT, UINT);
};