#pragma once
#include <windows.h>
#include <set>
#include "VariationCore.h"

template<typename _Type>
testfactor::RFactor NChooseK( 
        CVariationGenerator* pGenerator, 
        CValueSet<_Type>* pValueSet,
        UINT32 nKParamsCount,
        CTestCaseParameter<_Type>** pParams )
{
    assert( pValueSet->GetSize() >= nKParamsCount );
    
    std::vector<testfactor::RFactor> factors;
    std::for_each( pParams, pParams + nKParamsCount, [&]( CTestCaseParameter<_Type>* param )
    {
        testfactor::RFactor f = pGenerator->AddParameterValueSet( param, pValueSet );
        factors.push_back( f );
    } );

    testfactor::RFactor out;
    testfactor::CTestFactor* pFactor = new CTestNChooseKFactor( pValueSet->GetSize(), nKParamsCount, factors.begin(), factors.end() );
    out.Own( pFactor );
    return out;
}

UINT32 NChooseK( UINT32 n, UINT32 k );

//**********************************************
//
// A test factor for doing N choose K or
//        N! / (K!(N-K)!) combinations (not permutations)
//
class CTestNChooseKFactor : public testfactor::CTestFactor
{
        UINT32 m_uNCount;
        UINT32 m_uKCount;
        std::vector<testfactor::RFactor> m_KParams;

        //std::vector<UINT32> m_States;

    public:

        template<typename _Iter>
        CTestNChooseKFactor( size_t uNCount, size_t uKCount, _Iter first, _Iter last )
            : CTestFactor( NChooseK( uNCount, uKCount ) )
            , m_uNCount( uNCount )
            , m_uKCount( uKCount )
            , m_KParams( first, last )
            //, m_States( m_uCount * uKCount, 0 )
        {
            assert( uNCount >= uKCount );
            
            //UINT32 index = 0;
            //auto f = [&]( UINT32 n, UINT32 k, UINT32 index ) -> UINT32
            //{
            //    if( k == 1 )
            //    {
            //        for( UINT32 i = 0; i < n; ++i )
            //        {
            //            m_States[index * uKCount + uKCount - 1] = n - i - 1;
            //        }
            //    }
            //    else
            //    {
            //        // current item is k - 1
            //        // there are n to choose from
            //        for( UINT32 i = 0;  k <= n - i; ++i )
            //        {

            //        }
            //    }
            //};
            
        }

        virtual ~CTestNChooseKFactor();

        virtual size_t Set( size_t value, bool active = true);

        virtual size_t Describe( tstring &strOut, bool bState = false);
};

template<typename _Iter>
testfactor::RFactor NewSimultaneousIndependentSetFactor( _Iter first, _Iter last )
{
    size_t maxSetSize = 0;
    std::for_each( first, last, [&]( testfactor::RFactor f )
    {
        size_t count = f->Count();
        if( count > maxSetSize )
        {
            maxSetSize = count;
        }
    } );

    testfactor::RFactor out;
    testfactor::CTestFactor* pFactor = new CTestSimultaneousIndependentSetsFactor( maxSetSize, first, last );
    out.Own( pFactor );
    return out;
}

class CTestSimultaneousIndependentSetsFactor : public testfactor::CTestFactor
{
        std::vector<testfactor::RFactor> m_IndependentSets;

        //std::vector<UINT32> m_States;

    public:

        template<typename _Iter>
        CTestSimultaneousIndependentSetsFactor( size_t size, _Iter first, _Iter last )
            : CTestFactor( size )
            , m_IndependentSets( first, last )
        {
        }

        virtual ~CTestSimultaneousIndependentSetsFactor();

        virtual size_t Set( size_t value, bool active = true);

        virtual size_t Describe( tstring &strOut, bool bState = false);
};

