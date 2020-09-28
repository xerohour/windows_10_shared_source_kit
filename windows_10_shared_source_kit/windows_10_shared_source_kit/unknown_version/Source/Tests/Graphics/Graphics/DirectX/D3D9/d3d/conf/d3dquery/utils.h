/*
 *  Utils.h
 *
 *  Test Utilities
 *
 *  Author: Bruce Nelson    (brucen)    06/18/2002
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <strstream>

/*
 *  List of pointers that are deleted when List is deleted
 */

template< class T >
class CPtrList : public std::vector< T* >
{
public:
    ~CPtrList()
    {
        std::for_each( begin(), end(), deletePtr );
    }
private:
    static void deletePtr( T *pT )
    {
        delete pT;
    }
};


/*
 *  Throws an exception (itself) if assigned a false value or bad HRESULT result - maintains assignment count
 */

class CBoolException
{
public:
    CBoolException() :
        m_count(0),
		m_hr( S_OK )
    {
    }
    CBoolException &operator=( bool bValue )
    {
        m_count++;
        if ( !bValue )
        {
            throw *this;
        }
        return *this;
    }
    CBoolException &operator=( HRESULT hr )
    {
        return *this = SUCCEEDED( m_hr = hr );
    }
    unsigned getCount() const
    {
        return m_count;
    }
	operator HRESULT() const
	{
		return m_hr;
	}
private:
    unsigned m_count;
	HRESULT m_hr;
};


/*
 *  Base class for smart pointer functionality - must override destructor appropriately
 */

template< class T >
class CAuto
{
public:
    virtual ~CAuto()
    {
    }
    operator T*()
    {
        return m_t;
    }
    operator const T*() const
    {
        return m_t;
    }
    T *operator->()
    {
        return m_t;
    }
    const T *operator->() const
    {
        return m_t;
    }
    T **operator&()
    {
        return &m_t;
    }
    const T **operator&() const
    {
        return &m_t;
    }
    T &operator*()
    {
        return *m_t;
    }
    const T &operator*() const
    {
        return *m_t;
    }
    CAuto &operator=( const T *t )
    {
        if ( !m_t )
        {
            m_t = t;
        }
        return *this;
    }
    bool operator==( const CAuto<T> *pT ) const
    {
        return pT ? pT->m_t == m_t : !m_t;
    }
    bool operator!=( const CAuto<T> *pT ) const
    {
        return !operator==( pT );
    }
protected:
    CAuto( T *t = 0) :
        m_t( t )
    {
    }

    mutable T *m_t;
};


/*
 *  Smart Pointer for releasing objects
 */

template< class T >
class CAutoRelease : public CAuto< T >
{
public:
    CAutoRelease( T *t = 0 ) :
        CAuto< T >( t )
    {
    }
    virtual ~CAutoRelease()
    {
		release();
    }
    CAutoRelease &operator=( T *t )
    {
        return CAuto< T >::operator=( t ), *this;
    }
	void release()
    {
        if ( m_t )
        {
            unsigned uRef = m_t->Release();
            if ( 0 == uRef )
            {
			    m_t = 0;
            }
        }
    }
};


/*
 *  Smart Pointer for heap objects
 */

template< class T >
class CAutoDelete : public CAuto< T >
{
public:
    CAutoDelete( T *t = 0 ) :
        CAuto< T >( t )
    {
    }
    virtual ~CAutoDelete()
    {
        delete m_t;
    }
    CAutoDelete &operator=( T *t )
    {
        return CAuto< T >::operator=( t ), *this;
    }
    T &operator[]( unsigned i )
    {
        return m_t[i];
    }
    const T &operator[]( unsigned i ) const
    {
        return m_t[i];
    }
    T *get()
    {
        return m_t;
    }
};

/*
 *  Timer classes
 */

class CPerformanceCounter
{
public:
    CPerformanceCounter()
    {
        memset( this, 0, sizeof( *this ) );
        QueryPerformanceFrequency( &m_Frequency );
    }
    void start()
    {
        QueryPerformanceCounter( &m_PerformanceCount );
    }
    double finish() const
    {
        LARGE_INTEGER PerformanceCount2;
        QueryPerformanceCounter( &PerformanceCount2 );
        return double(PerformanceCount2.QuadPart - m_PerformanceCount.QuadPart) / double(m_Frequency.QuadPart);
    }
private:
    LARGE_INTEGER m_Frequency;
    LARGE_INTEGER m_PerformanceCount;
};


class CTimer
{
public:
    CTimer()
    {
        memset( this, 0, sizeof( *this ) );
    }
    void start( float length )
    {
        m_elapsed = length;
        m_time = clock();
    }
    bool elapsed() const
    {
        return float( clock() - m_time ) / CLOCKS_PER_SEC > m_elapsed;
    }
private:
    clock_t m_time;
    float m_elapsed;
};

class CThread
{
public:
	CThread()
	{
        DWORD threadId;

		_hThread = CreateThread(NULL, 0, _ThreadProc, this, CREATE_SUSPENDED, &threadId);
	}
	virtual ~CThread()
	{
        CloseHandle(_hThread);
	}
	virtual bool start(int priority = THREAD_PRIORITY_NORMAL)
	{
		if (GetThreadPriority(_hThread) != priority)
        {
			SetThreadPriority(_hThread, priority);
        }

        return ResumeThread(_hThread) == -1 ? false : true;
	}
	operator HANDLE ()
	{
		return _hThread;
	}
protected:
	virtual UINT run()
	{
		return 0;
	}
private:
	HANDLE _hThread;
    CThread(const CThread &);
    CThread &operator=(const CThread &);
	static DWORD WINAPI _ThreadProc(LPVOID param)
	{
		return static_cast<CThread*>(param)->run();
	}
};

template< class T >
class CStats
{
	friend std::ostream &operator<<( std::ostream &stream, const CStats< T > &stats );
public:
	typedef std::vector< T > DataType;
	CStats( const DataType &timings, unsigned nBuckets );
	static T stdDev( const T *pFirst, const T *pLast, T fMean );
	T m_Mean;
	T m_Min;
	T m_Max;
	T m_StdDev;
	T m_ExpVal;
};

template< class T >
CStats< T >::CStats( const DataType &timings, unsigned nBuckets )
{
	const T zero = static_cast< T >( 0 );

	m_Min = *min_element( timings.begin(), timings.end() );
	m_Max = *max_element( timings.begin(), timings.end() );
	m_Mean = accumulate( timings.begin(), timings.end(), zero ) / timings.size();
	m_StdDev = stdDev( &*timings.begin(), &*timings.end(), m_Mean );

	vector< T > buckets( nBuckets, zero );
	T bucketSize = ( m_Max - m_Min ) / nBuckets;

	// fill the buckets with frequency counts
	for( DataType::const_iterator it = timings.begin(); it != timings.end(); ++it )
	{
		unsigned index = min( unsigned(( *it - m_Min ) / bucketSize), nBuckets - 1 );
		buckets[index] += 1.0;
	}

	// compute probabilities of each bucket
	T sum = zero;
	for( int i=0; i < nBuckets; i++ )
	{
		double n = buckets[i];
		buckets[i] /= timings.size();
		sum += n * buckets[i];
	}

	// compute expected value
	m_ExpVal = zero;
	if ( sum > zero )
	{
		for( DataType::const_iterator it = timings.begin(); it != timings.end(); ++it )
		{
			unsigned index = min( unsigned(( *it - m_Min ) / bucketSize), nBuckets - 1 );
			m_ExpVal += *it * buckets[index];
		}
		m_ExpVal /= sum;
	}
}

template< class T >
T CStats< T >::stdDev( const T *pFirst, const T *pLast, T fMean )
{  
	T stdDev = 0;
	int n = int( pLast - pFirst );

	if ( n > 1 )
	{
		for( const T *pValue = pFirst; pValue != pLast; pValue++ )
		{
			T fDiff = *pValue - fMean;
			stdDev += fDiff * fDiff;
		}
		stdDev = static_cast< T >( sqrt( stdDev / ( n - 1 ) ) );
	}

	return stdDev;
}

std::ostream &operator<<( std::ostream &stream, const CStats< double > &stats )
{
	stream << stats.m_Mean << "\n";
	stream << stats.m_Min << "\n";
	stream << stats.m_Max << "\n";
	stream << stats.m_StdDev << "\n";
	return stream << stats.m_ExpVal;
}


class CClipBoard
{
public:
	CClipBoard( HWND hWnd ) :
		m_hWnd( hWnd )
	{
	}
	CClipBoard &operator<<( const char *s )
	{
		m_String += s;
		return *this;
	}
	template< class T >
	CClipBoard &operator<<( const T &t )
	{
		std::ostrstream stream;
		stream << t << std::ends;
		std::auto_ptr< const char > ptr( stream.str() );
		return *this << ptr.get();
	}
	void Copy()
	{
		int len = m_String.length() + 1;
		HGLOBAL hGlobal = GlobalAlloc( GHND, len);
		void *pMem = GlobalLock( hGlobal );
		memcpy( pMem, m_String.c_str(), len );
		GlobalUnlock( hGlobal );
		if ( OpenClipboard( m_hWnd ) )
		{
			EmptyClipboard();
			HANDLE handle = SetClipboardData( CF_TEXT, hGlobal );
			CloseClipboard();
		}
		m_String = "";
	}
private:
	HWND m_hWnd;
	std::string m_String;
};



/*
 *  Utility Functions
 */

template< typename T1, typename T2 >
T2 mapf( const T1 &t, const T1 &l1, const T1 &h1, const T2 &l2, const T2 &h2 )
{
    return static_cast<T2>( t - l1 ) * ( h2 - l2 ) / static_cast<T2>( h1 - l1 ) + l2;
}

template< typename T >
UINT getRefCount( T *t )
{
    return t ? t->AddRef(), t->Release() : 0;
}

template< typename T >
T *zeroMemory( T *t, unsigned count = 1 )
{
    return static_cast<T*>( memset( t, 0, sizeof( T ) * count ) );
}

template< typename T >
bool isZeroMemory( const T *t, unsigned count = 1 )
{
    T z;

    return !memcmp( t, zeroMemory( &z ), sizeof( T ) * count );
}

void *zeroMem( void *p, unsigned s )
{
    return memset( p, 0, s );
}

bool isZeroMem( const void *p, unsigned s )
{
    CAutoDelete<char> pT = new char[s];
    return !memcmp( p, zeroMem( (char*)pT, s ), s );
}

#endif //__UTILS_H__