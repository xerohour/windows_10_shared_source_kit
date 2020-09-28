//////////////////////////////////////////////////////////////////////////
//  TestClassFactory.h
//  created:	2004/09/21
//
//  purpose: Implements the CTestClassFactory class
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "Test.h"
#include "TestClassFactory.h"

void CTestClassFactory::RegisterClassByName( const tstring &strClass, TestAllocatorProc pAllocator )
{
	TestAllocatorMap::iterator it = m_TestAllocatorMap.find( strClass );
	
	if( it == m_TestAllocatorMap.end() )
	{
		m_TestAllocatorMap.insert( TestAllocatorMap::value_type( strClass, pAllocator ) );
	}
}

CTest *CTestClassFactory::AllocateTest( const tstring &strClass )
{
	CTest *pTest = NULL;
	TestAllocatorMap::iterator it = m_TestAllocatorMap.find( strClass );
	
	if( it != m_TestAllocatorMap.end() )
	{
		TestAllocatorProc AllocTest = it->second;
		pTest = AllocTest();
	}
	
	return pTest;
}

CTestClassFactory::~CTestClassFactory()
{
};