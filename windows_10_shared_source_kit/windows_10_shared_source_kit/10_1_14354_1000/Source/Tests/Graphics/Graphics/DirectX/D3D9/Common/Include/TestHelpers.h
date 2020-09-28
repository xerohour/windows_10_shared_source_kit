// general test helper bits.  Everyone should have a friend like ( * this )!


#ifndef TESTHELPERS_h
#define TESTHELPERS_h

#include <windows.h>
#include <string>
#include <fstream>
#include <vector>


#ifdef  SAFEDELETE
#undef  SAFEDELETE
#endif // #ifdef SAFEDEELTE
#define SAFEDELETE(pRes)	{ if( pRes ) delete ( pRes ); ( pRes ) = NULL; }

#ifdef  SAFEDELETEA
#undef  SAFEDELETEA
#endif // #ifdef SAFEDELETEA
#define SAFEDELETEA(pRes)	{ if( pRes ) delete [] ( pRes ); ( pRes ) = NULL; }

#ifdef  SAFERELEASE
#undef  SAFERELEASE
#endif // #ifdef SAFERELEASE
#define SAFERELEASE(pRes)	{ if( pRes ) ( pRes )->Release(); ( pRes ) = NULL; }

#ifdef  SAFERELEASEC
#undef  SAFERELEASEC
#endif // #ifdef SAFERELEASE
#define SAFERELEASEC(pRes)	{ if( pRes ) { if( 0 == ( pRes )->Release() ) { ( pRes ) = NULL; } } }

#define HRSTR( hr )		m_pD3D->HResultToString( hr )

#define GRF( res )		( ( res ) ? ( ( res )->AddRef(), ( res )->Release() ) : 0 )

class Rect : public RECT
{

public:

	Rect( LONG l, LONG t, LONG r, LONG b )
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
};

// ANSI/UNICODE/TCHAR string helpers

#define Strlen		_tcslen
#define Strncpy		_tcsncpy
#define Strcpy		_tcscpy
#define Sprintf		_stprintf
#define Atoi		_tstoi
#define Strtol		_tcstol
#define Strcmp		_tcscmp
#define Strncmp		_tcsncmp
#define Strcat		_tcscat

typedef std::basic_string < TCHAR > String;
typedef std::basic_ofstream < TCHAR, std::char_traits < TCHAR > > Ofstream;

static std :: string WtoA( std :: wstring & sSrc )
{
	std :: string sRet;
	char cRec[ 2 ];
	cRec[ 1 ] = '\0';
	for( UINT i = 0; i < sSrc.length(); ++ i )
	{
		WideCharToMultiByte( CP_ACP, 0, sSrc.c_str() + i, 1, cRec, 1, NULL, NULL );
		sRet += cRec;
	}

	return sRet;
}

static std :: wstring AtoW( std :: string & sSrc )
{
	std :: wstring sRet;
	wchar_t cRec[ 2 ];
	cRec[ 1 ] = L'\0';
	for( UINT i = 0; i < sSrc.length(); ++ i )
	{
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, sSrc.c_str() + i, 1, cRec, 1 );
		sRet += cRec;
	}

	return sRet;
}

/*
static String AtoT( std :: string & sSrc )
{
	if( sizeof( TCHAR ) == sizeof( char ) )
		return sSrc;
	else
		return AtoW( sSrc );
}

static String WtoT( std :: string & sSrc )
{
	if( sizeof( TCHAR ) == sizeof( wchar_t ) )
		return sSrc;
	else
		return WtoA( sSrc );
}

static std :: string TtoA( String & sSrc )
{
	if( sizeof( TCHAR ) == sizeof( char ) )
		return sSrc;
	else
		return WtoA( sSrc );
}

static std :: wstring TtoW( String & sSrc )
{
	if( sizeof( TCHAR ) == sizeof( wchar_t ) )
		return sSrc;
	else
		return AtoW( sSrc );
}
*/


// Tests float equality with an epsilon
inline bool feq( float f0, float f1, float fEpsilon )
{
	/*
	if( f1 != 0.0f )
		return f0 / f1 - 1.0f <= fEpsilon;
	else
		return fabsf( f0 ) <= fEpsilon;
	*/

	return fabsf( f1 - f0 ) <= fEpsilon;
}


// Tests double equality with an epsilon
inline bool deq( double f0, double f1, double fEpsilon )
{
	/*
	if( f1 != 0.0 )
		return f0 / f1 - 1.0 <= fEpsilon;
	else
		return fabs( f0 ) <= fEpsilon;
	*/

	return fabs( f1 - f0 ) <= fEpsilon;
}

// Gets a user memory pointer from a resource id
static HRESULT MemoryFromResource( HMODULE hModule, LPCTSTR sName, void ** pMemory, DWORD * pdwSize )
{
	HRESULT hr;

	* pMemory = NULL;
	* pdwSize = 0;

	// find the resource pointer
	HRSRC h = FindResource( hModule, sName, RT_RCDATA );
	if( h == NULL )
		return E_FAIL;

	DWORD dwSize = SizeofResource( NULL, h );

	HGLOBAL hg = LoadResource( NULL, h );
	if( hg == NULL )
		return E_FAIL;

	void * pData = LockResource( hg );
	if( pData == NULL )
		return E_FAIL;

	* pMemory = pData;
	* pdwSize = dwSize;

	return S_OK;
}


// build both versions of the parser
#define UNICODEPARSER
#include "Parser.h"
typedef _ParserW ParserW;
#undef UNICODEPARSER
#include "Parser.h"
typedef _ParserA ParserA;


struct STestCase
{
	std::string			m_sTestDesc;

	virtual ~STestCase()
	{
	}

	virtual void Cleanup()
	{
		// test data should clean itself up here
	}
};


class CTester
{

protected:

	ParserA			m_Parser;
	std::vector < STestCase * >
					m_v_pTestCases;

public:

	~CTester()
	{
		m_Parser.Cleanup();

		std::vector < STestCase * > ::iterator itCur, itEnd = m_v_pTestCases.end();
		for( itCur = m_v_pTestCases.begin(); itCur != itEnd; ++ itCur )
		{
			( * itCur )->Cleanup();
			delete * itCur;
		}
	}

	virtual HRESULT ReadTestCases( String & sScript, bool bFromFile ) = 0;

	virtual void AddTestCase( STestCase * pTestCase )
	{
		m_v_pTestCases.push_back( pTestCase );
	}

	virtual DWORD NumTestCases()
	{
		return m_v_pTestCases.size();
	}

	STestCase * GetTestCase( DWORD dwCase )
	{
		return m_v_pTestCases[ dwCase ];
	}

	ParserA * GetParser()
	{
		return & m_Parser;
	}

	// methods to add test cases to the case file

	virtual HRESULT AddTestCaseToFile( STestCase * pTestCase, std::string & sFile )
	{
		std::ofstream ofs;
		ofs.open( sFile.c_str(), std::ios::app | std::ios::out );
		if( ofs.fail() )
		{
			MessageBox( 0, "Could not open auto file", NULL, MB_OK );
			return E_FAIL;
		}

		return WriteCase( pTestCase, ofs );
	}

	virtual HRESULT WriteCase( STestCase * pTestCase, std::ofstream & ofs ) = 0;
};


#endif // #ifndef TESTHELPERS_h