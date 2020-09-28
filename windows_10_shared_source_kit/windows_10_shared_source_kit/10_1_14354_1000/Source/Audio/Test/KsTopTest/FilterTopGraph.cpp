// Copyright(C) Microsoft.All rights reserved.

#include "common.h"

CFilterTopoGraph::~CFilterTopoGraph()
{
    while( !vectorNodeVertex.empty() )
    {
        NodeVertex* pNodeVertex = vectorNodeVertex.back();
        if( nullptr != pNodeVertex )
        {
            delete pNodeVertex;
            pNodeVertex = nullptr;
        }

        vectorNodeVertex.pop_back();
    }

    while( !vectorFilterPinVertex.empty() )
    {
        FilterPinVertex* pFilterPinVertex = vectorFilterPinVertex.back();
        if( nullptr != pFilterPinVertex )
        {
            delete pFilterPinVertex;
            pFilterPinVertex = nullptr;
        }

        vectorFilterPinVertex.pop_back();
    }
}

bool CFilterTopoGraph::BuildFromFilter( CFilter* pFilter )
{
    bool fRet = true;

    // populate the filter pin and node vectors
    UINT connectionCount = pFilter->GetCountConnections();
    for( UINT i = 0; i < connectionCount; i++ )
    {
        CConnection* pConnection = pFilter->GetConnection(i);
        if( nullptr == pConnection )
        {
            XLOG( XFAIL, eError, "  ERROR: Unable to obtain reference to Connection[%d]", i );
            fRet = false;
            goto Exit;
        }

        // First, do the FROM side of the topology connection

        if( pConnection->FromNode() == KSFILTER_NODE )
        {
            // this side of the connection is a filter pin.
            // we will create a new FilterPinVertex if we have not created one already.

            FilterPinVertex* pFilterPinVertex = nullptr;

            for( auto iter = vectorFilterPinVertex.begin(); iter != vectorFilterPinVertex.end(); iter++ )
            {
                if( (*iter)->GetPinId() == pConnection->FromNodePin() )
                {
                    pFilterPinVertex = *iter;
                    break;
                }
            }

            if( !pFilterPinVertex )
            {
                pFilterPinVertex = new FilterPinVertex( pConnection->FromNodePin() );
                if( !pFilterPinVertex )
                {
                    XLOG( XFAIL, eError, "  ERROR: Unable to create filter pin vertex for filter pin %u", pConnection->FromNodePin() );
                    fRet = false;
                    goto Exit;
                }

                vectorFilterPinVertex.push_back( pFilterPinVertex );
            }
        }
        else
        {
            // this side of the connection is a node pin.
            // we will create a new NodeVertex if we have not created one already.
            // Then increment the connection count for the node's output pin

            NodeVertex* pNodeVertex = nullptr;

            for( auto iter = vectorNodeVertex.begin(); iter != vectorNodeVertex.end(); iter++ )
            {
                if( (*iter)->GetNodeId() == pConnection->FromNode() )
                {
                    pNodeVertex = *iter;
                    break;
                }
            }

            if( !pNodeVertex )
            {
                pNodeVertex = new NodeVertex( pConnection->FromNode() );
                if( !pNodeVertex )
                {
                    XLOG( XFAIL, eError, "  ERROR: Unable to create node vertex for node %u", pConnection->FromNode() );
                    fRet = false;
                    goto Exit;
                }

                vectorNodeVertex.push_back( pNodeVertex );
            }

            auto outputPinIter = pNodeVertex->mapOutputNodePin.find( pConnection->FromNodePin() );
            if( pNodeVertex->mapOutputNodePin.end() == outputPinIter )
            {
                // this is the first time we've seen this node pin
                pNodeVertex->mapOutputNodePin.insert( std::pair<ULONG,int>( pConnection->FromNodePin(), 1 ) );
            }
            else
            {
                // otherwise, just increment the connection count
                outputPinIter->second++;
            }
        }

        // now, do the same thing for the TO side of the topology connection

        if( pConnection->ToNode() == KSFILTER_NODE )
        {
            // this side of the connection is a filter pin.
            // we will create a new FilterPinVertex if we have not created one already.

            FilterPinVertex* pFilterPinVertex = nullptr;

            for( auto iter = vectorFilterPinVertex.begin(); iter != vectorFilterPinVertex.end(); iter++ )
            {
                if( (*iter)->GetPinId() == pConnection->ToNodePin() )
                {
                    pFilterPinVertex = *iter;
                    break;
                }
            }

            if( !pFilterPinVertex )
            {
                pFilterPinVertex = new FilterPinVertex( pConnection->ToNodePin() );
                if( !pFilterPinVertex )
                {
                    XLOG(XFAIL, eError, "  ERROR: Unable to create filter pin vertex for filter pin %u", pConnection->ToNodePin() );
                    fRet = false;
                    goto Exit;
                }

                vectorFilterPinVertex.push_back( pFilterPinVertex );
            }
        }
        else
        {
            // this side of the connection is a node pin.
            // we will create a new NodeVertex if we have not created one already.
            // Then increment the connection count for the node's input pin

            NodeVertex* pNodeVertex = nullptr;

            for( auto iter = vectorNodeVertex.begin(); iter != vectorNodeVertex.end(); iter++ )
            {
                if( (*iter)->GetNodeId() == pConnection->ToNode() )
                {
                    pNodeVertex = *iter;
                    break;
                }
            }

            if( !pNodeVertex )
            {
                pNodeVertex = new NodeVertex( pConnection->ToNode() );
                if( !pNodeVertex )
                {
                    XLOG( XFAIL, eError, "  ERROR: Unable to create node vertex for node %u", pConnection->ToNode() );
                    fRet = false;
                    goto Exit;
                }

                vectorNodeVertex.push_back( pNodeVertex );
            }

            auto inputPinIter = pNodeVertex->mapInputNodePin.find( pConnection->ToNodePin() );
            if( pNodeVertex->mapInputNodePin.end() == inputPinIter )
            {
                // this is the first time we've seen this node pin
                pNodeVertex->mapInputNodePin.insert( std::pair<ULONG,int>( pConnection->ToNodePin(), 1 ) );
            }
            else
            {
                // otherwise, just increment the connection count
                (inputPinIter->second)++;
            }
        }
    }

Exit:

    return fRet;
}