/********************************************************************
	filename: 	NResult.cpp
	created:	2005/12/08
	author:		billkris
	
	purpose:	
*********************************************************************/

#include <pch.h>
#include "NResult.h"

NRESULT::NRESULT( HRESULT hr, size_t size  )
{
	for( ; size > 0; --size )
		m_results.push_back( hr );
}

NRESULT::NRESULT( const NRESULT &nhr )
{
	m_results = nhr.m_results;
}

NRESULT::operator HRESULT() const
{
	HRESULT hr = 0;
	for( ResultsType::const_reverse_iterator it = m_results.rbegin(); it != m_results.rend(); ++it )
	{
		hr = *it;
		if( FAILED( hr ) )
			break;
	}
	return hr;
};

NRESULT &NRESULT::operator=( const HRESULT &hr )
{
	for( ResultsType::iterator it = m_results.begin(); it != m_results.end(); ++it )
	{
		*it = hr;
	}

	return *this;
}

bool NRESULT::operator==( const HRESULT &hr )
{
	if( GetSize() == 0 )
		return false;

	if( FAILED( hr ) )
	{
		// return 'true' if any child result matches hr
		for( ResultsType::const_iterator it = m_results.begin(); it != m_results.end(); ++it )
		{
			if( hr == *it )
				return true;
		}
		return false;
	}
	else
	{
		for( ResultsType::const_iterator it = m_results.begin(); it != m_results.end(); ++it )
		{
			if( hr != *it )
				return false;
		}

		return true;
	}
}

bool NRESULT::operator==( const NRESULT &nr )
{
	return m_results == nr.m_results;
}
