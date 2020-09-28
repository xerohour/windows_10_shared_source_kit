// Str - ref counted simple string class

// uses TSRef:
#include "LegacyTRef.h"

#ifndef _TSTR_H_
#define _TSTR_H_


////////////////////////////////////////////////////////////////////////////////////////////
// C is the character class (Str is char by default):
template<class C>
class TStr
{
	TSRef<C, true> m_rChars;
	unsigned long m_iLength;

public:
	TStr(const C* pC = 0);
	TStr(const TStr& str);

	void Clear();

	TStr& operator=(const C* pC);
	TStr& operator=(const TStr& str);

	TStr& operator+=(const C* pC);
	TStr& operator+=(const TStr& str);

	//TStr operator+(const TStr& r)	{ TStr n = l; n += r; return n; }
	//TStr operator+(const C* r)		{ TStr n = l; n += r; return n; }

	const C* Get() const;
	operator const C*() const { return Get(); }
	const C* operator*() const { return Get(); }

	bool operator==(const C* pC) const;
	bool operator==(const TStr& str) const;

	unsigned long Len() const { return m_iLength; }
	bool Valid() const { return m_rChars.Valid(); }
};
////////////////////////////////////////////////////////////////////////////////////////////

// default Str will be TStr<TCHAR>...
// must define these beforehand:
inline unsigned long CLen(const TCHAR* c)			{ return _tcslen(c); }
inline void CCopy(TCHAR* newc, const TCHAR* c)		{ _tcscpy(newc, c); }
inline void CCat(TCHAR* oldc, const TCHAR* addc)	{ _tcscat(oldc, addc); }
inline int CCmp(const TCHAR* one, const TCHAR* two)	{ return _tcscmp(one, two); }
typedef TStr<TCHAR> Str;

////////////////////////////////////////////////////////////////////////////////////////////
// TStr Implementation:

template<class C>
TStr<C>::TStr(const C* pC) : m_iLength(0)
{
	if(pC)
	{
		m_iLength = CLen(pC);

		C* pNewC = new C[m_iLength+1];
		if(pNewC)
			CCopy(pNewC, pC);
		else
			m_iLength = 0;
		m_rChars.Own(pNewC);
	}
}
template<class C>
TStr<C>::TStr(const TStr<C>& str) : m_rChars(str.m_rChars), m_iLength(str.m_iLength) {}

template<class C>
void TStr<C>::Clear()
{
	m_rChars.Release();
	m_iLength = 0;
}

template<class C>
TStr<C>& TStr<C>::operator=(const C* pC)
{
	if(pC)
	{
		m_iLength = CLen(pC);

		C* pNewC = new C[m_iLength+1];
		if(pNewC)
			CCopy(pNewC, pC);
		else
			m_iLength = 0;
		m_rChars.Own(pNewC);
	}
	else
		Clear();
	return *this;
}

template<class C>
TStr<C>& TStr<C>::operator=(const TStr<C>& str)
{
	m_rChars = str.m_rChars;
	m_iLength = str.m_iLength;
	return *this;
}


template<class C>
TStr<C>& TStr<C>::operator+=(const C* pC)
{
	if(m_rChars.Valid() && pC)
	{
		m_iLength = m_iLength + CLen(pC);
		C* pNewC = new C[m_iLength+1];
		if(pNewC)
		{
			CCopy(pNewC, m_rChars);
			CCat(pNewC, pC);
		}
		else
			m_iLength = 0;
		m_rChars.Own(pNewC);
	}
	return *this;
}
template<class C>
TStr<C>& TStr<C>::operator+=(const TStr<C>& str)
{
	if(m_rChars.Valid() && str.Valid())
	{
		m_iLength = m_iLength + str.Len();
		C* pNewC = new C[m_iLength+1];
		if(pNewC)
		{
			CCopy(pNewC, m_rChars);
			CCat(pNewC, (const C*)str);
		}
		else
			m_iLength = 0;
		m_rChars.Own(pNewC);
	}
	return *this;
}

template<class C>
const C* TStr<C>::Get() const
{
	//assert(m_rChars.Valid());
	return m_rChars.Get();
}

template<class C>
bool TStr<C>::operator==(const C* pC) const
{
	if(m_rChars.Valid())
		return CCmp(m_rChars, pC) == 0;
	else
		return false;
}
template<class C>
bool TStr<C>::operator==(const TStr<C>& str) const
{
	if(m_iLength == str.m_iLength && m_rChars.Valid() && str.m_rChars.Valid())
	{
		if(m_rChars.Get() == str.m_rChars.Get())
			return true;
		return CCmp(m_rChars.Get(), str.m_rChars.Get()) == 0;
	}
	return false;
}




template<class C> 
TStr<C> operator+(const TStr<C>& l, const TStr<C>& r)
{
	TStr<C> n = l;
	n += r;
	return n;
}

template<class C> 
TStr<C> operator+(const C* l, const TStr<C>& r)
{
	TStr<C> n = l;
	n += r;
	return n;
}

template<class C> 
TStr<C> operator+(const TStr<C>& l, const C* r)
{
	TStr<C> n = l;
	n += r;
	return n;
}


////////////////////////////////////////////////////////////////////////////////////////////
#endif // _TSTR_H_