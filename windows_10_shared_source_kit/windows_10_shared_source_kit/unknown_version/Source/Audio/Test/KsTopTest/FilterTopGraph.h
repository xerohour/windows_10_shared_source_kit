#pragma once

interface Vertex
{
    virtual ~Vertex() {}
};

class FilterPinVertex : public Vertex
{

public:

    FilterPinVertex( ULONG pinId ) : Vertex(), m_pinId( pinId ) {}
    ULONG GetPinId() { return m_pinId; }

private:

    ULONG m_pinId;

};

class NodeVertex : public Vertex
{

public:

    NodeVertex( int nodeId ) : Vertex(), m_nodeId( nodeId ) {}

    ULONG GetNodeId() { return m_nodeId; }

    int GetNodeInputPinConnectionCount( ULONG nodePinId )
    {
        auto iter = mapInputNodePin.find( nodePinId );

        if( iter == mapInputNodePin.end() )
        {
            return 0;
        }
        else
        {
            return iter->second;
        }
    }

    int GetNodeOutputPinConnectionCount( ULONG nodePinId )
    {
        auto iter = mapOutputNodePin.find( nodePinId );

        if( iter == mapOutputNodePin.end() )
        {
            return 0;
        }
        else
        {
            return iter->second;
        }
    }

    size_t GetInputPinCount() { return mapInputNodePin.size(); }

    size_t GetOutputPinCount() { return mapOutputNodePin.size(); }

    // key = pin id
    // value = number of connections
    std::map<ULONG,int> mapInputNodePin;
    std::map<ULONG,int> mapOutputNodePin;

private:

    ULONG m_nodeId;

};

class CFilterTopoGraph
{

public:

    ~CFilterTopoGraph();

    bool BuildFromFilter( CFilter* pFilter );

    NodeVertex* GetNodeVertex( ULONG nodeId )
    {
        NodeVertex* pNodeVertex = nullptr;

        for( auto iter = vectorNodeVertex.begin(); iter != vectorNodeVertex.end(); iter++ )
        {
            if( (*iter)->GetNodeId() == nodeId )
            {
                pNodeVertex = *iter;
                break;
            }
        }

        return pNodeVertex;
    }

    FilterPinVertex* GetFilterPinVertex( ULONG filterPinId )
    {
        FilterPinVertex* pFilterPinVertex = nullptr;

        for( auto iter = vectorFilterPinVertex.begin(); iter != vectorFilterPinVertex.end(); iter++ )
        {
            if( (*iter)->GetPinId() == filterPinId )
            {
                pFilterPinVertex = *iter;
                break;
            }
        }

        return pFilterPinVertex;
    }

    size_t GetNodeVertexCount() { return vectorNodeVertex.size(); }

    size_t GetFilterPinVertexCount() { return vectorFilterPinVertex.size(); }

private:

    std::vector<NodeVertex *> vectorNodeVertex;
    std::vector<FilterPinVertex *> vectorFilterPinVertex;

};