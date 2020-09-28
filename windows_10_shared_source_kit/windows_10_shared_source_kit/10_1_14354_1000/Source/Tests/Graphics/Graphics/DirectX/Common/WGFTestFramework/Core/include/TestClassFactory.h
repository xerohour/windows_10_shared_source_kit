//////////////////////////////////////////////////////////////////////////
//  TestClassFactory.h
//  created:	2004/09/21
//
//  purpose: Factory class used to generate CTest-derived objects
//		from a string.
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "stringutils.h"

typedef class CTest *(WGFTESTAPI *TestAllocatorProc)();

class CTestClassFactory
{
public:
	~CTestClassFactory();
	
	void RegisterClassByName( const tstring &strClass, TestAllocatorProc pAllocator );
		// Registers a class so that it can be allocated by CTestClassFactory.
		
	CTest *AllocateTest( const tstring &strClass );
		// Allocates a registered test class.
	
private:
	typedef std::map< tcistring, TestAllocatorProc > TestAllocatorMap;
	TestAllocatorMap m_TestAllocatorMap;
};