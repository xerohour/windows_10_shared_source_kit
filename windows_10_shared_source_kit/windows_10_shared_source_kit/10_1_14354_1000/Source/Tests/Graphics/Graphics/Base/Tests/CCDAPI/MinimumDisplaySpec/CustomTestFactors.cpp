#include "CustomTestFactors.h"

UINT32 NChooseK( UINT32 n, UINT32 k )
{
    if( k > n )
    {
        return 0;
    }

    if( k == n )
    {
        return 1;
    }

    UINT32 N = 1;
    UINT32 K = 1;
    for( UINT32 i = 0; i < k; ++i )
    {
        N *= n - i;
        K *= i + 1;
    }

    return N/K;
}

CTestNChooseKFactor::~CTestNChooseKFactor()
{
}

size_t CTestNChooseKFactor::Set( size_t value, bool active )
{
    // convert the given index to the specific combinations requested.
    size_t uSkip = CTestFactor::Set(value,active);
    if(Valid())
    {
        if( m_bActive )
        {
            UINT32 index = value;
            UINT32 uNCount = m_uNCount;
            UINT32 uKCount = m_uKCount;

            // Should be O(K*((N*N+N)/2)) -> O(K*N^2)
            // if this is too slow, then we could binary search each range
            // instead.  That would leave O(K * log( (N*N+N)/2 ) )
            for( UINT32 i = 0; i < m_KParams.size(); ++i )
            {
                // figure out where in the range we are
                UINT32 lowerBound = 0;
                UINT32 upperBound = 0;
                while( true )
                {
                    assert( uNCount >= uKCount );
                    UINT32 step = uNCount - (uKCount - 1);
                    upperBound  = ((step * step + step) / 2) + lowerBound;
                    if( value > upperBound )
                    {
                        // still not in the right range, keep going.
                        lowerBound = upperBound;
                        --uNCount;
                        continue;
                    }
                    break;
                }
                
                // which index are we at now?  should be nNCount - 1?
                m_KParams[i]->Set( --uNCount, active );
                --uKCount;
                index -= lowerBound;
            }

            return 0;
        }

        m_bActive = false;
        std::for_each( m_KParams.begin(), m_KParams.end(), []( testfactor::RFactor f )
        {
            f->Set(0,false);
        } );
    }

    return uSkip;
}

size_t CTestNChooseKFactor::Describe( tstring &strOut, bool bState )
{
    tstringstream s;
    s << _T("nCk(");
    {
        bool empty = true;
        std::for_each( m_KParams.begin(), m_KParams.end(), [&]( testfactor::RFactor f )
        { 
            tstring str;
            f->Describe( str, bState );
            if( !empty )
            {
                s << _T(",");
            }
            else
            {
                empty = false;
            }
            s << str;
        } );
    }

    s << _T(")");
    strOut = s.str();
    return 1;
}

CTestSimultaneousIndependentSetsFactor::~CTestSimultaneousIndependentSetsFactor()
{
}

size_t CTestSimultaneousIndependentSetsFactor::Set( size_t value, bool active )
{
    // convert the given index to the specific combinations requested.
    size_t uSkip = CTestFactor::Set(value,active);
    if(Valid())
    {
        if( m_bActive )
        {
            std::for_each( m_IndependentSets.begin(), m_IndependentSets.end(), [&]( testfactor::RFactor f ) 
            {
                size_t i = value % f->Count();
                f->Set( i, active );
            } );

            return 0;
        }

        m_bActive = false;
        std::for_each( m_IndependentSets.begin(), m_IndependentSets.end(), []( testfactor::RFactor f )
        {
            f->Set(0,false);
        } );
    }

    return uSkip;
}

size_t CTestSimultaneousIndependentSetsFactor::Describe( tstring &strOut, bool bState )
{
    tstringstream s;
    s << _T("SimulSet(");
    {
        bool empty = true;
        std::for_each( m_IndependentSets.begin(), m_IndependentSets.end(), [&]( testfactor::RFactor f )
        { 
            tstring str;
            f->Describe( str, bState );
            if( !empty )
            {
                s << _T(",");
            }
            else
            {
                empty = false;
            }
            s << str;
        } );
    }

    s << _T(")");
    strOut = s.str();
    return 1;
}

