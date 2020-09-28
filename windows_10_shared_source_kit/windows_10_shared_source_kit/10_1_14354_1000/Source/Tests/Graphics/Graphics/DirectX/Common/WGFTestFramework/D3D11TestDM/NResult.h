/********************************************************************
	filename: 	NResult.h
	created:	2005/12/08
	author:		billkris
	
	purpose:	
*********************************************************************/

#pragma once

//--------------------------------------------------------------------
// NRESULT
// Container for multiple HRESULT values.  
// Used by the D3DWrapper classes.
//--------------------------------------------------------------------

class NRESULT
{
public:
	NRESULT( HRESULT hr = S_OK, size_t size = 1 );	
	NRESULT( const NRESULT &nhr );
	
    HRESULT operator[]( size_t nIndex ) const { return m_results[nIndex]; }
    HRESULT &operator[]( size_t nIndex ) { return m_results[nIndex]; }
    
    void SetSize( size_t size ) { m_results.resize( size ); }
    size_t GetSize() const { return m_results.size(); }
	
	// Returns the first failing HRESULT.  If no failing
	// results exist then returns a passing HRESULT value.
	operator HRESULT() const;
	NRESULT &operator=( const HRESULT &hr );	
	bool operator==( const HRESULT &hr );	
	bool operator==( const NRESULT &nr );
	
private:
	typedef std::vector< HRESULT > ResultsType;
	ResultsType m_results;
};
