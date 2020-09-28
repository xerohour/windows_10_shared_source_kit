/************************************************************
 *
 *   Copyright (c) Microsoft Corporation, 2001
 *
 *   Program:   Implementation for CTestPtr class
 *   Created:   12/14/2001
 *    Author:   Daniel Roman (danielro)
 *
 ************************************************************/

#include "CTestPtr.h"


UINT GetAvailMemory()
{
	// there is no way to determine memory for vb/ib, so pretend it's in system memory
	MEMORYSTATUS memstat;

	// check if really out of memory
	GlobalMemoryStatus( &memstat );
	
	return UINT( memstat.dwAvailVirtual );
}



//-----------------------------------------------------------------------------
// CTestPtr members
//-----------------------------------------------------------------------------

CTestPtr::CTestPtr(UINT nSize, UINT nBeginPad, UINT nEndPad)
{
	m_Ptr = new unsigned char[nSize + nBeginPad + nEndPad];
	if( 0L == m_Ptr )
		return;
	m_nBeginPad = nBeginPad;
	m_nEndPad   = nEndPad;
	m_nSize     = nSize;
	m_PtrData = (LPVOID)((char*)m_Ptr + nBeginPad);
	Clear();
}

//-----------------------------------------------------------------------------

CTestPtr::~CTestPtr()
{
	if( m_Ptr )
		delete[] m_Ptr;
}

//-----------------------------------------------------------------------------

void CTestPtr::Clear()
{
	memset(m_Ptr,PTR_PAD_SIG,m_nSize + m_nBeginPad + m_nEndPad);
}

//-----------------------------------------------------------------------------

bool CTestPtr::IsValid()
{
	if( 0L == m_Ptr )
		return false;
	BYTE* pBeginPad = (BYTE*)m_Ptr;
        unsigned int i;
        for( i=0; i<m_nBeginPad; i++ )
	{
		if( *pBeginPad != PTR_PAD_SIG )
			return false;
		pBeginPad++;
	}
	BYTE* pEndPad = (BYTE*)m_PtrData + m_nSize;
	for( i=0; i<m_nEndPad; i++ )
	{
		if( *pEndPad != PTR_PAD_SIG )
			return false;
		pEndPad++;
	}
	return true;
}

//-----------------------------------------------------------------------------

bool CTestPtr::IsZeroed()
{
	if( !IsValid() )
		return false;

	BYTE* p = (BYTE*)m_PtrData;

	for( unsigned int i=0; i<m_nSize; i++ )
	{
		if( *p != 0 )
			return false;
		p++;
	}
	return true;
}
