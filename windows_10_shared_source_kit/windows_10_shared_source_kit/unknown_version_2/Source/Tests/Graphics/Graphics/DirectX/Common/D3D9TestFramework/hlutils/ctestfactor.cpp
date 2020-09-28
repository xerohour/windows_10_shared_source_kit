// Test Factor implementation

#include "CTestFactor.h"
//#include "CD3DWindowFramework.h"		// seems I have to include this just for DPF

#include <stdio.h>
#include <stdlib.h>

//const Str CTestFactor::s_strDefaultName("UnNamed");

const Str k_strMultName(_T("Mult"));
const Str k_strAddName(_T("Add"));
const Str k_strModName(_T("Mod"));

#ifndef SAFEDELETE
#define SAFEDELETE(p) { if(p) delete (p); p = NULL; }
#endif

#ifndef SAFEDELETEA
#define SAFEDELETEA(p) { if(p) delete [] (p); p = NULL; }
#endif

static bool D_CHECKError(LPCSTR exp, LPCSTR file, UINT line)
{
	D_Out(_T("%s(%d) CHECK Error: %s\n"), file, line, exp);
	return false;
}

#define CHECK(exp) ((exp) ? true : D_CHECKError(#exp, __FILE__, __LINE__))

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction utility:
RFactor NewFactor(UINT uCount, Str strName)
{
	CTestFactor* pFactor = new CTestFactor(uCount, strName);
	RFactor rFactor;
	if(pFactor)
	{
		if(!rFactor.Own(pFactor).Valid())
			D_Out(_T("NewFactor: Out of Memory error\n"));
	}
	return rFactor;
}


UINT CTestFactor::Describe(Str& strOut, bool bState)
{
	static TCHAR tcsOut[256];
	if(bState)
	{
		if(Active())
			_stprintf(tcsOut, _T("%s(%d)"), *m_strName, Value());
		else
			_stprintf(tcsOut, _T("%s(%s)"), *m_strName, _T("N/A"));
	}
	else
		_stprintf(tcsOut, _T("%s(%d)"), *m_strName, Count());
	strOut = tcsOut;
	return 0;
}

CTestFactor::~CTestFactor()
{
	ClearSkips();
}

bool CTestFactor::Skip(UINT uStart, UINT uStop)
{
	if(uStop == 0) uStop = uStart + 1;
	assert(uStop > uStart);
	assert(uStop <= m_uCount);

	// Search forward from current or head:
	SSkipRange* pSkip = (m_pCurSkip && m_pCurSkip->uStart <= uStart) ? m_pCurSkip : m_pSkipHead;
	SSkipRange* pNext = pSkip ? pSkip->pNext : 0;
	SSkipRange* pTemp = 0;

	while(pNext && pNext->uStop < uStart)
	{
		pSkip = pNext;
		pNext = pSkip->pNext;
	}

	// if pSkip is 0, we had no nodes:
	if(pSkip == 0)
	{
		if((pTemp = new SSkipRange()) == 0)
			return false;						// memory allocation failure
		pTemp->pNext = 0;
		pTemp->uStart = uStart;
		pTemp->uStop = uStop;
		m_pCurSkip = m_pSkipHead = pTemp;		// set head and cur pos here
	}
	else
	{
		pNext = pSkip->pNext;
		bool bCombine = false;

		// is range completely before pSkip?
		if(pSkip->uStart > uStop)			// insert before pSkip (at head)
		{
			if((pTemp = new SSkipRange()) == 0)
				return false;				// memory allocation failure
			pTemp->pNext = pSkip;
			pTemp->uStart = uStart;
			pTemp->uStop = uStop;
			m_pCurSkip = m_pSkipHead = pTemp;		// set head and cur pos here
		}
		// is range completely after pSkip?
		else if(pSkip->uStop < uStart)
		{
			// if there is a next, combine with it if necessary, otherwise, insert new node:
			if(pNext && pNext->uStart <= uStop)
			{	// touches next, set up to combine with it...
				pSkip = pNext;
				pNext = pSkip->pNext;
				bCombine = true;
			}
			else
			{	// insert between nodes (or at tail)
				if((pTemp = new SSkipRange()) == 0)
					return false;				// memory allocation failure
				pTemp->pNext = pNext;
				pTemp->uStart = uStart;
				pTemp->uStop = uStop;
				pSkip->pNext = pTemp;
				m_pCurSkip = pTemp;		// Set current pos here
			}
		}
		else		// it touches pSkip, set to combine
		{
			bCombine = true;
		}

		if(bCombine)			// must combine with pSkip, may bridge multiple nodes
		{
			pSkip->uStart = (uStart < pSkip->uStart) ? uStart : pSkip->uStart;	// set pSkip->uStart to least of these two
			uStop = (pSkip->uStop < uStop) ? uStop : pSkip->uStop;				// set uStop to the greater of these two
			while(pNext && pNext->uStart <= uStop)					// while it combines with next node, eat it:
			{
				if(pNext->uStop > uStop)		// if next stop is after ours, make it ours.. loop should end after this
					uStop = pNext->uStop;

				pTemp = pNext;
				pNext = pNext->pNext;
				delete pTemp;
			}
			pSkip->uStop = uStop;
			pSkip->pNext = pNext;
			m_pCurSkip = pSkip;		// set current pos here
		}
	}

	return true;
}


bool CTestFactor::DontSkip(UINT uStart, UINT uStop)
{
	if(uStop == 0) uStop = uStart + 1;
	assert(uStop > uStart);
	assert(uStop <= m_uCount);

	// Search forward from current or head:
	SSkipRange* pSkip = (m_pCurSkip && m_pCurSkip->uStart <= uStart) ? m_pCurSkip : m_pSkipHead;
	SSkipRange* pNext = pSkip ? pSkip->pNext : 0;
	SSkipRange* pTemp = 0;

	if(pSkip == 0)
		return true;		// no skips to remove

	// Skip nodes this range has no effect upon: (Note: we may have an effect upon a node previous to one we remove or add)
	while(pNext && pNext->uStart < uStart)
	{
		pSkip = pNext;
		pNext = pSkip->pNext;
	}

	bool bRemoval = false;

	if(pSkip->uStart >= uStop)		// >= 'cause we don't care if it just touches
		return true;		// range lies entirely before skip (and entirely after anything before)

	if(pSkip->uStop <= uStart)		// again: don't care if it just touches
	{						// range lies after pSkip, and starts before (or at) BEGINNING of next (if there is a next)
		if(pNext && pNext->uStart < uStop)		// there is a next and we affect it, so start removal
			bRemoval = true;
		else	// no next or don't touch next
			return true;		// lies after pSkip and before pNext (if there is one)
	}
	else
	{	// range affects pSkip directly (start of range lies before the end of pSkip)
		if(pSkip->uStart < uStart)	// starts after beginning of pSkip
		{
			if(pSkip->uStop > uStop)	// Special case, must break pSkip in two
			{
				if((pTemp = new SSkipRange()) == 0)
					return false;		// memory allocation failure
				pTemp->pNext = pSkip->pNext;
				pTemp->uStart = uStop;
				pTemp->uStop = pSkip->uStop;
				pSkip->uStop = uStart;
				pSkip->pNext = pTemp;
				m_pCurSkip = pSkip;		// set cur pos back at skip since re-entry is most likely pSkip or later
			}
			else					// range extends beyond end of pSkip and possibly over other ranges
			{
				pSkip->uStop = uStart;		// adjust pSkip's end before moving on to removal
				bRemoval = true;
			}
		}
		else		// Starts at/before beginning of pSkip (I think this can only happen at the head)
		{
			assert(pSkip == m_pSkipHead);
			pNext = pSkip;		// start scanning from pSkip (instead of next)
			pSkip = 0;
			bRemoval = true;
		}
	}

	if(bRemoval)
	{
		// remove nodes between pSkip and where pNext ends up after this loop (after removal range)
		while(pNext && pNext->uStop < uStop)
		{
			pTemp = pNext->pNext;
			delete pNext;
			pNext = pTemp;
		}
		// values for pNext here: 0 (no next), uStop lies before pNext, and uStop lies within pNext
		// simple to deal with, however:
		if(pSkip)
			pSkip->pNext = pNext;
		else
			m_pSkipHead = pNext;		// pSkip was head, reset head.

		if(pNext && uStop > pNext->uStart)
			pNext->uStart = uStop;
	}

	return true;
}


void CTestFactor::ClearSkips()
{
	while(m_pSkipHead)
	{
		SSkipRange* pNext = m_pSkipHead->pNext;
		delete m_pSkipHead;
		m_pSkipHead = pNext;
	}
	m_pCurSkip = 0;
}

UINT CTestFactor::Set(UINT value, BOOL active)
{
	m_bActive = active;

	if(active)
	{
		m_uValue = value;
	
		// Check for skips:

		// reset to head if NULL or after value:
		if(!m_pCurSkip || value < m_pCurSkip->uStart)
			m_pCurSkip = m_pSkipHead;

		if(m_pCurSkip)
		{
			// move current to next, only if next is <= value (that's where we'll start next time)
			while(m_pCurSkip->pNext && m_pCurSkip->pNext->uStart <= value )
				m_pCurSkip = m_pCurSkip->pNext;

			if(m_pCurSkip && m_pCurSkip->uStart <= value && m_pCurSkip->uStop > value)
			{
				m_uValue = 0;
				m_bActive = false;
				return m_pCurSkip->uStop;
			}
		}
	}
	else
	{
		m_uValue = 0;
	}

	return 0;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Multiplication Branch:
class CMultTestFactor : public CTestFactor
{
public:
	virtual ~CMultTestFactor() {}
	CMultTestFactor(RFactor& left, RFactor& right) : CTestFactor(0, k_strMultName), m_rLeft(left), m_rRight(right)
	{
		m_bValid = (m_rLeft.Valid() && m_rRight.Valid()) &&
			// Both children must be valid for this node to be valid:
			(m_rLeft->Valid() && m_rRight->Valid());
		if(Valid())
			m_uCount = left->Count() * right->Count();
	}

	virtual UINT Set(UINT value, BOOL active = true)
	{
		UINT uSkip = CTestFactor::Set(value, active);
		if(Valid())
		{
			if(m_bActive)		// won't be active if uSkip > 0
			{
				UINT uRightCount = m_rRight->Count();
				if(value < (m_rLeft->Count() * uRightCount))
				{
					uSkip = m_rLeft->Set(value / uRightCount);
					if(uSkip)
					{
						uSkip *= uRightCount;
						// Then fall through to inactive case
					}
					else
					{
						uSkip = m_rRight->Set(value % uRightCount);
						if(uSkip)
							uSkip += (value / uRightCount) * uRightCount;	// fall through to inactive
						else
							return 0;		// no skip required
					}
				}
				// else fall through: value is too high, or we're skipping...
			}

			m_bActive = false;
			m_rLeft->Set(0,false);
			m_rRight->Set(0,false);
		}

		return uSkip;
	}

	virtual UINT Describe(Str& strOut, bool bState = false)
	{
		Str strLeft, strRight;
		UINT iLeftPrec = m_rLeft->Describe(strLeft, bState);
		UINT iRightPrec = m_rRight->Describe(strRight, bState);
		if(iLeftPrec > 1)
			strLeft = _T("(") + strLeft + _T(")");
		if(iRightPrec > 1)
			strRight = _T("(") + strRight + _T(")");
		strOut = strLeft + _T(" * ") + strRight;
		return 1;
	}

protected:
	RFactor m_rLeft, m_rRight;
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Addition Branch:
class CAddTestFactor : public CTestFactor
{
public:
	virtual ~CAddTestFactor() {}
	CAddTestFactor(RFactor& left, RFactor& right) : CTestFactor(0, k_strAddName), m_rLeft(left), m_rRight(right) 
	{
		m_bValid = (m_rLeft.Valid() && m_rRight.Valid()) &&
			// only one child must be valid for this node to be valid:
			(m_rLeft->Valid() || m_rRight->Valid());
		if(Valid())
			m_uCount = left->Count() + right->Count();
	}

	virtual UINT Set(UINT value, BOOL active = true)
	{
		UINT uSkip = CTestFactor::Set(value, active);
		if(Valid())
		{
			if(m_bActive)
			{
				UINT uLeftCount = m_rLeft->Count();
				if(value < uLeftCount)
				{
					m_rRight->Set(0,false);		// Always turn off a side before turning on the other 
												// (this allows multiple instances of the same factor under different sides)
					uSkip = m_rLeft->Set(value);
					if(uSkip == 0)
						return 0;		// no skip
				}
				else if(value < (uLeftCount + m_rRight->Count()))
				{
					m_rLeft->Set(0,false);
					uSkip = m_rRight->Set(value - uLeftCount);
					if(uSkip)
						uSkip += uLeftCount;
					else
						return 0;		// no skip
				}
				// else fall through: value is too high, or we're skipping...
			}
			
			m_bActive = false;
			m_rLeft->Set(0,false);
			m_rRight->Set(0,false);
		}

		return uSkip;
	}

	virtual UINT Describe(Str& strOut, bool bState = false)
	{
		Str strLeft, strRight;
		UINT iLeftPrec = m_rLeft->Describe(strLeft, bState);
		UINT iRightPrec = m_rRight->Describe(strRight, bState);
		if(iLeftPrec > 3)
			strLeft = _T("(") + strLeft + _T(")");
		if(iRightPrec > 3)
			strRight = _T("(") + strRight + _T(")");
		strOut = strLeft + _T(" + ") + strRight;
		return 3;
	}

protected:
	RFactor m_rLeft, m_rRight;
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mod Branch:
class CModTestFactor : public CTestFactor
{
public:
	virtual ~CModTestFactor() {}
	CModTestFactor(RFactor& left, RFactor& right) : CTestFactor(0, k_strModName), m_rLeft(left), m_rRight(right) 
	{
		m_bValid = (m_rLeft.Valid() && m_rRight.Valid()) &&
			// Only left node must be valid for this node to be valid:
			m_rLeft->Valid();
		if(Valid())
			m_uCount = left->Count();
	}

	virtual UINT Set(UINT value, BOOL active = true)
	{
		UINT uSkip = CTestFactor::Set(value, active);
		if(Valid())
		{
			if(m_bActive)
			{
				UINT uLeftCount = m_rLeft->Count();
				if(value < uLeftCount)
				{
					uSkip = m_rLeft->Set(value);
					if(uSkip == 0)
					{
						if(m_rRight->Valid())
						{
							uSkip = m_rRight->Set(value % m_rRight->Count());
							if(uSkip)
								uSkip += (value / m_rRight->Count()) * m_rRight->Count();
							else
								return 0;		// no skip
						}
					}
				}
				// else fall through: value is too high, or we're skipping...
			}
			
			m_bActive = false;
			m_rLeft->Set(0,false);
			m_rRight->Set(0,false);
		}

		return uSkip;
	}

	virtual UINT Describe(Str& strOut, bool bState = false)
	{
		Str strLeft, strRight;
		UINT iLeftPrec = m_rLeft->Describe(strLeft, bState);
		UINT iRightPrec = m_rRight->Describe(strRight, bState);
		if(iLeftPrec > 2)
			strLeft = _T("(") + strLeft + _T(")");
		if(iRightPrec > 2)
			strRight = _T("(") + strRight + _T(")");
		strOut = strLeft + _T(" MOD ") + strRight;
		return 2;
	}

protected:
	RFactor m_rLeft, m_rRight;
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Operators for constructing the test factor structure:

RFactor operator*(RFactor& left, RFactor& right)
{
	RFactor out;
	if(left.Valid() && right.Valid())
	{
		CTestFactor* pFactor = new CMultTestFactor(left, right);
		if(pFactor)
			out.Own(pFactor);
		left->m_rParent = right->m_rParent = out.Weak();
	}
	return out;
}

RFactor operator+(RFactor& left, RFactor& right)
{
	RFactor out;
	if(left.Valid() && right.Valid())
	{
		CTestFactor* pFactor = new CAddTestFactor(left, right);
		if(pFactor)
			out.Own(pFactor);
		left->m_rParent = right->m_rParent = out.Weak();
	}
	return out;
}

RFactor operator%(RFactor& left, RFactor& right)
{
	RFactor out;
	if(left.Valid() && right.Valid())
	{
		CTestFactor* pFactor = new CModTestFactor(left, right);
		if(pFactor)
			out.Own(pFactor);
		left->m_rParent = right->m_rParent = out.Weak();
	}
	return out;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pairwise Combinatorial Testing:


// I need a list class, argh!  Since people are so against STL lists, I'll roll my own!
// just a simple bi-directional linked list that does what I need:
template<typename T>
class TList
{
	struct Node
	{
		Node(const T& item = T()) : t(item), prev(0), next(0) {}
		T t;
		Node *prev, *next;
	} *m_tail;
	UINT m_count;

	void insert(Node* node, const T& item)
	{
		Node* newnode = new Node(item);
		newnode->prev = node->prev;
		newnode->next = node;
		node->prev->next = newnode;
		node->prev = newnode;
		m_count++;
	}
	void remove(Node* node)
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
		delete node;
	}
public:
	TList() : m_tail(new Node()), m_count(0) { if(m_tail) m_tail->prev = m_tail->next = m_tail; }
	~TList() { Clear(); delete m_tail; }

	class Iterator
	{
	public:	// must all be public for vc6 (friend class TList doesn't work)
		Iterator(Node* n) : node(n) {}
		Node* node;
		Iterator() : node(NULL) {}
		T& operator*() { return node->t; } const
		T& operator->() { return node->t; } const
		Iterator& operator++() { node = node->next; return *this; }
		Iterator& operator--() { node = node->prev; return *this; }
		bool operator==(Iterator& other) { return node == other.node; }
		bool operator!=(Iterator& other) { return node != other.node; }
	};

	void Clear()
	{
		Node* head = m_tail->next;
		while(head != m_tail)
		{
			head = head->next;
			delete head->prev;
		}
		m_count = 0;
	}
	void push_front(const T& item) { insert(m_tail->next, item); }
	void push_back(const T& item) { insert(m_tail, item); }
	void insert(const Iterator& it, const T& item) { insert(it.node, item); }
	void remove(Iterator& it) { remove(it.node); }

	Iterator begin() const { return Iterator(m_tail->next); }
	Iterator end() const { return Iterator(m_tail); }

	UINT count() const { return m_count; }
};

class CXFactor;
struct CXInteraction;
class CCrossTestFactor;

typedef TRef<CXFactor> RXFactor;
typedef TRef<RXFactor, true> RXFactorArray;

typedef TRef<CXInteraction> RXInteraction;
typedef TRef<RXInteraction, true> RXInteractionArray;
typedef TList<RXInteraction> RXInteractionList;

class CXFactor
{
public:
	CXFactor(RFactor rFactor, INT iGroup) : 
		m_rFactor(rFactor), m_iGroup(iGroup),
		m_pNotGroups(NULL), m_cNotGroups(0),
		m_bSet(false),
		m_pUnskipped(NULL), m_cUnskipped(0)
	{
		UINT i;
		if(!CHECK(m_rFactor.Valid()))
			return;

		// count, allocate, then set unskipped value array:
		while(1)
		{
			for(i=0; i<m_rFactor->Count(); i++)
			{
				UINT uSkip = m_rFactor->Set(i);
				if(uSkip)
				{
					i = uSkip-1;
					continue;
				}
				else
				{
					if(m_pUnskipped)
						m_pUnskipped[m_cUnskipped] = i;
					m_cUnskipped++;
				}
			}
			if(m_pUnskipped)
				break;
			if(!m_cUnskipped)
				break;
			m_pUnskipped = new UINT[m_cUnskipped];
			m_cUnskipped = 0;
			if(!m_pUnskipped)
				break;
		}
	}
	~CXFactor()
	{
		SAFEDELETEA(m_pUnskipped);
		SAFEDELETEA(m_pNotGroups);
	}

	UINT Set(UINT uValue)
	{
		m_bSet = true;
		m_uUnskippedValue = uValue;
		return m_rFactor->Set(m_pUnskipped[uValue]);
	}
	void Clear() { m_bSet = false; m_rFactor->Set(0, false); }
	UINT Get() { return m_uUnskippedValue; }
	bool IsSet() { return (m_bSet)? true : false; }
	UINT Count() { return m_cUnskipped; }
	bool Valid() { return m_rFactor.Valid() && m_pUnskipped; }

	inline bool operator==(const CXFactor& other) { return this == &other; }

	RFactor m_rFactor;
	INT m_iGroup;
	INT* m_pNotGroups;
	UINT m_cNotGroups;
	BOOL m_bSet;
	UINT* m_pUnskipped;
	UINT m_cUnskipped;
	UINT m_uUnskippedValue;
};

struct CXInteraction
{
	CXInteraction(RXFactorArray rFactors, UINT cFactors) : m_rFactors(rFactors), m_cFactors(cFactors)
	{
		UINT i;
		m_count = 1;
		for(i=0; i<cFactors; i++)
			m_count *= rFactors[i]->Count();
		m_Flags = new char[m_count];

		m_Name = _T("(");
		for(i=0; i<m_cFactors; i++)
		{
			if(i) m_Name += _T("-");
			m_Name += m_rFactors[i]->m_rFactor->Name();
		}
		m_Name += _T(")");

		// Allocate these temporary arrays once at max size they will ever be:
		m_pUnset = new UINT[m_cFactors];
		m_pUnsetMultiples = new UINT[m_cFactors];
		m_pUnsetValues = new UINT[m_cFactors];
		if(!m_pUnset || !m_pUnsetMultiples || !m_pUnsetValues)
		{
			Free();
			return;
		}

		Clear();
	}
	~CXInteraction() { Free(); }

	inline bool operator==(const CXInteraction& other) { return this == &other; }

	// Clean up references:
	void Free() {
		m_count = 0;
		m_cFactors = 0;
		m_cRemaining = 0;
		m_rFactors.Release();
		SAFEDELETEA(m_Flags);
		SAFEDELETEA(m_pUnset);
		SAFEDELETEA(m_pUnsetMultiples);
		SAFEDELETEA(m_pUnsetValues);
	}

	void Clear()	// Clear history
	{
		UINT i;
		for(i=0; i<m_count; i++)
			m_Flags[i] = 0;
		m_cRemaining = m_count;
	}

	UINT TestCombos( bool bRate, 
		const UINT* pRelated = NULL, UINT cRelated = 0,
		const RXInteractionArray& rInteractions = RXInteractionArray())
	{
		UINT i,j;					// temp. counters
		UINT cUnset = 0;
		UINT cComboCount = 1;		// number of combinations of unset factors
		UINT uResult = 0;
		UINT uHardIndex = 0;		// flag index minus unset factors

		// to keep track of the best rated combination
		UINT uBestRating = 0, uBestIndex = 0;

		// set unset indices and multiples, and initialize the combo count, while we're at it
		UINT multiple = m_count;
		for(i=0; i<m_cFactors; i++)
		{
			multiple /= m_rFactors[i]->Count();
			if(!m_rFactors[i]->IsSet())
			{
				m_pUnset[cUnset] = i;
				m_pUnsetMultiples[cUnset] = multiple;
				cComboCount *= m_rFactors[i]->Count();
				++cUnset;
			}
			else
			{
				uHardIndex += m_rFactors[i]->Get() * multiple;
			}
		}

		if(!bRate && cUnset == 0)
			return 0;

		// Test every combination of unset factors:
		for(i=0; i<cComboCount; i++)
		{
			UINT uFlagIndex = uHardIndex;
			UINT uRating = 0;

			// Init current value array for unset factors,
			// and set flag index while we're at it
			{
				UINT cSubCount = cComboCount;
				UINT uSubIndex = i;
				for(j=0; j<cUnset; j++)
				{
					cSubCount /= m_rFactors[m_pUnset[j]]->Count();
					m_pUnsetValues[j] = uSubIndex / cSubCount;
					uSubIndex %= cSubCount;
					uFlagIndex += m_pUnsetValues[j] * m_pUnsetMultiples[j];
				}
			}

			// is this combination untested?
			if(!m_Flags[uFlagIndex])
			{
				uRating += m_cFactors;
				if(!bRate)
				{
					for(j=0; j<cUnset; j++)
						m_rFactors[m_pUnset[j]]->Set(m_pUnsetValues[j]);
					for(j=0; j<cRelated; j++)
						uRating += rInteractions[pRelated[j]]->Rate();
				}
			}

			if(bRate)
			{
				uBestRating += uRating;
			}
			else if(uRating > uBestRating)
			{
				uBestRating = uRating;
				uBestIndex = i;
			}
		}

		if(bRate)
		{
			uResult = uBestRating;
			goto done;
		}

		// were we able to find something that was untested?
		if(uBestRating)
		{
			// set best values using best index:
			UINT cSubCount = cComboCount;
			UINT uSubIndex = uBestIndex;
			for(j=0; j<cUnset; j++)
			{
				cSubCount /= m_rFactors[m_pUnset[j]]->Count();
				m_rFactors[m_pUnset[j]]->Set(uSubIndex / cSubCount);
				uSubIndex %= cSubCount;
			}
			uResult = 1;
//			DPF(1, _T("bestRating: %d"), uBestRating);
		}
		else
		{
			// Clear unset factors:
			for(j=0; j<cUnset; j++)
				m_rFactors[m_pUnset[j]]->Clear();
			uResult = 0;
		}

	done:
		return uResult;
	}

	bool PickNew(const UINT* pRelated, UINT cRelated, const RXInteractionArray& rInteractions)
	{
		if(!m_cRemaining)
			return false;

		return (TestCombos(false, pRelated, cRelated, rInteractions)) ? true : false;
	}

	// Rates currently set combination
	UINT Rate()
	{
		if(!m_cRemaining)
			return 0;

		return TestCombos(true);
	}

	void UpdateFlags()
	{
		UINT i;
		UINT uFlagIndex = 0;
		UINT uSubCount = m_count;
		for(i=0; i<m_cFactors; i++)
		{
			uSubCount /= m_rFactors[i]->Count();
			uFlagIndex += m_rFactors[i]->Get() * uSubCount;
		}
		if(!m_Flags[uFlagIndex])
		{
			m_Flags[uFlagIndex] = 1;
			m_cRemaining--;
		}
	}

	bool Valid()
	{
		return (m_cFactors && m_count && m_pUnset && m_pUnsetValues && m_pUnsetMultiples);
	}

	Str m_Name;
	RXFactorArray m_rFactors;	// array of factors for this interaction
	UINT m_cFactors;	// number of factors in above array
	char* m_Flags;		// which combinations have been tested
	UINT m_count;		// total combinations (also count of flags)
	UINT m_cRemaining;	// remaining untested combinations
	// Temporary counters that we only want to allocate once:
	UINT *m_pUnset, *m_pUnsetValues, *m_pUnsetMultiples;
};

typedef TRef<UINT, true> RUINTArray;

// Sort comparison functions:
int cmpInt(int a, int b) { return (a<b) ? -1 : (a>b) ? 1 : 0; }
int cmpUInt(UINT a, UINT b) { return (a<b) ? -1 : (a>b) ? 1 : 0; }
int __cdecl cmpXFactorGroups(const void* arg1, const void* arg2)
{ return cmpInt((*(RXFactor*)arg1)->m_iGroup, (*(RXFactor*)arg2)->m_iGroup); }
int __cdecl cmpXInteractions(const void* arg1, const void* arg2)
{ return cmpUInt((*(RXInteraction*)arg2)->m_cRemaining, (*(RXInteraction*)arg1)->m_cRemaining); }
UINT g_cFactorsForCmpStates = 0;	// used by cmpStates to know the number of factors
int __cdecl cmpStates(const void* arg1, const void* arg2)
{
	UINT* pArg1 = (*(RUINTArray*)arg1).Get();
	UINT* pArg2 = (*(RUINTArray*)arg2).Get();
//	UINT iElements = sizeof(pArg1) / sizeof(UINT);
	for(UINT i=0; i<g_cFactorsForCmpStates; i++)
	{
		if(pArg1[i] == pArg2[i])
			continue;
		return (pArg1[i]<pArg2[i]) ? -1 : 1;
	}
	return 0;
}

class CCrossTestFactor : public CTestFactor
{
public:
	virtual ~CCrossTestFactor() {}
	CCrossTestFactor(Str strName) : 
		CTestFactor(0, strName), 
		m_cFactors(0),
		m_cInteractions(0),
		m_cStates(0)
		{}

	// This does the actual setup work:
	bool Compute(
		const XFACTOR* pFactors, 
		UINT cFactorCount, 
		const UINT* pGroupOrders = NULL, 
		UINT cGroupOrders = 0)
	{
		UINT i,j;
		Str strOrders = _T("");
		static TCHAR tsBuff[256];
		{	// Create the factor array:
			RXFactor* pRXFactors = new RXFactor[cFactorCount];
			if(!pRXFactors || !m_rFactors.Own(pRXFactors).Valid())
				goto failed;
			m_cFactors = cFactorCount;
		}
		for(i=0; i<cFactorCount; i++)
		{
			{
				CXFactor* pXFactor = new CXFactor(pFactors[i].rFactor, pFactors[i].iGroup);
				if(!pXFactor || !m_rFactors[i].Own(pXFactor).Valid() || !m_rFactors[i]->Valid())
					goto failed;
			}
			m_rFactors[i]->m_rFactor = pFactors[i].rFactor;
			m_rFactors[i]->m_iGroup = pFactors[i].iGroup;
			m_rFactors[i]->m_cNotGroups = pFactors[i].cNotGroups;
			if(pFactors[i].cNotGroups)
			{
				m_rFactors[i]->m_pNotGroups = new INT[pFactors[i].cNotGroups];
				if(!m_rFactors[i]->m_pNotGroups)
					goto failed;
				for(j=0; j<pFactors[i].cNotGroups; j++)
					m_rFactors[i]->m_pNotGroups[j] = pFactors[i].pNotGroups[j];
			}
		}

		// Now calculate all of the interactions:
		if(!CreateInteractions(pGroupOrders, cGroupOrders))
			goto failed;

		// Print the interactions:
/*		DPF(1, _T("Interactions:\n"));
		for(i=0; i<m_cInteractions; i++)
		{
			DPF(1, _T(" ("));
			for(j=0; j<m_rInteractions[i]->m_cFactors; j++)
			{
				if(j) { DPF(1, _T("-")); }	// separator
				DPF(1, _T("%s"), m_rInteractions[i]->m_rFactors[j]->m_rFactor->Name().Get());
			}
			DPF(1, _T(")"));
		}
		DPF(1, _T("\n"));
*/
		// Now calculate our combinations:
		if(!CalculateStates())
			goto failed;

		// sort combinations by factor order
		g_cFactorsForCmpStates = m_cFactors;
		qsort((void*)(m_States.Get()), (size_t)m_cStates, sizeof(RUINTArray), cmpStates);

		// if m_cStates < 1 this factor is invalid
		if(m_cStates < 1)
			goto failed;

		m_bValid = true;
		m_uCount = m_cStates;

		// Fill in info:
		if(cGroupOrders && pGroupOrders)
		{
			strOrders = _T(",");
			for(i=0; i<cGroupOrders; i++)
			{
				_sntprintf(tsBuff, 255, _T(" %d:%d"), pGroupOrders[i] >> 16, pGroupOrders[i] & 0xFFFF);
				strOrders += tsBuff;
			}
		}
		_sntprintf(tsBuff, 255, _T("%d"), m_uCount);
		m_strInfo = tsBuff;
		m_strInfo += strOrders;

		// We don't need these anymore:
		m_rInteractions.Release();
		return true;
	failed:
		m_bValid = false;
		m_uCount = 0;
		m_rFactors.Release();
		m_rInteractions.Release();
		return false;
	}

	virtual UINT Set(UINT value, BOOL active = true)
	{
		UINT i;
		UINT uSkip = CTestFactor::Set(value, active);
		if(Valid())
		{
			if(m_bActive)
			{
				for(i=0; i<m_cFactors; i++)
				{
					// Any skips below this point are ignored.
					m_rFactors[i]->Set(m_States[value][i]);
				}
				return 0;
			}
			
			m_bActive = false;
			for(i=0; i<m_cFactors; i++)
			{
				m_rFactors[i]->Clear();
			}
		}

		return uSkip;
	}

	virtual UINT Describe(Str& strOut, bool bState = false)
	{
		UINT i, j;
		static TCHAR tcsOut[256];
		Str strTmp;
		//strOut = _T("");
		if(bState)
		{
			if(Active())
			{
				if(_sntprintf(tcsOut, 255, _T("%s[%d]"), *m_strName, Value()) < 0)
					tcsOut[255] = _T('\0');
				strOut = tcsOut;
			}
			else
			{
				if(_sntprintf(tcsOut, 255, _T("%s(%s)"), *m_strName, _T("N/A")) < 0)
					tcsOut[255] = _T('\0');
				strOut = tcsOut;
				return 0;
			}
		}
		else
		{
			if(_sntprintf(tcsOut, 255, _T("%s(%s)"), *m_strName, *m_strInfo) < 0)
				tcsOut[255] = _T('\0');
			strOut = tcsOut;
		}
		strOut += _T("<");
		if(Valid())
		{
			for(i=0; i<m_cFactors; i++)
			{
				if(strTmp.Len()) strOut += _T(", ");
				m_rFactors[i]->m_rFactor->Describe(strTmp, bState);
				strOut += strTmp;
				if(!bState)
				{
					if(m_rFactors[i]->m_iGroup == -1)
					{
						strOut += _T("-1");
					}
					else
					{
						if(m_rFactors[i]->m_iGroup)
						{
							if(_sntprintf(tcsOut, 255, _T(":%d"), m_rFactors[i]->m_iGroup) < 0)
								tcsOut[255] = _T('\0');
							strOut += tcsOut;
						}
						if(m_rFactors[i]->m_cNotGroups && m_rFactors[i]->m_pNotGroups)
						{
							for(j=0; j<m_rFactors[i]->m_cNotGroups; j++)
							{
								if(_sntprintf(tcsOut, 255, _T("!%d"), m_rFactors[i]->m_pNotGroups[j]) < 0)
									tcsOut[255] = _T('\0');
								strOut += tcsOut;
							}
						}
					}
				}
			}
		}
		else
		{
			strOut += _T("INVALID");
		}
		strOut += _T(">");
		return 0;
	}

protected:
	RXFactorArray m_rFactors;
	RXInteractionArray m_rInteractions;
	UINT m_cInteractions;
	UINT m_cFactors;
	TRef<RUINTArray, true> m_States;
	UINT m_cStates;
	Str m_strInfo;

	bool CalculateStates()
	{
		bool bResult = false;
		// our rating sytem uses an array of indices to interactions that are related through one or
		// more factors effected by the current interaction searching for a good value.
		UINT *pRelated = new UINT[m_cInteractions];
		UINT cRelated = 0;
		UINT i,j,k,l;
		TList<RUINTArray> listStates;
		TList<RUINTArray>::Iterator it;

		bool bNewCombinations = true;

		if(!CHECK(pRelated))
			goto failed;

		// seed rand with something
		i = (m_cFactors * 37) + m_cInteractions;
		srand((int)i);
		
//		DPF(1, _T("Calculating states... random seed = %d\n"), i);

		while(bNewCombinations)
		{
			bNewCombinations = false;
			// sort interactions by remaining combinations:
			qsort((void*)(m_rInteractions.Get()), (size_t)m_cInteractions, sizeof(RXInteraction), cmpXInteractions);

			// Clear all factors:
			for(i=0; i<m_cFactors; i++)
				m_rFactors[i]->Clear();

			// iterate through interactions, trying to set new states:
			for(i=0; i<m_cInteractions; i++)
			{
				// set related interaction array
				
				// clear first:
				for(j=0; j<m_cInteractions; j++)
					pRelated[j] = 0;

				// iterate each factor in current interaction, and flag related interactions
				for(j=i+1; j<m_rInteractions[i]->m_cFactors; j++)
				{
					// iterate all interactions and flag interactions that reference this factor:
					for(k=0; k<m_cInteractions; k++)
					{
						// skip same interaction:
						if(k == i)
							continue;

						for(l=0; l<m_rInteractions[k]->m_cFactors; l++)
						{
							if(m_rInteractions[i]->m_rFactors[j] == m_rInteractions[k]->m_rFactors[l])
							{
								pRelated[k] = 1;
								break;
							}
						}
					}
				}
				
				// translate related array to indices:
				cRelated = 0;
				for(j=0; j<m_cInteractions; j++)
					if(pRelated[j])
						pRelated[cRelated++] = j;

				// Attempt to pick a new combination:
				if(m_rInteractions[i]->PickNew(pRelated, cRelated, m_rInteractions))
				{
/*					DPF(1, _T("  picked: ["));
					for(j=0; j<m_cFactors; j++)
					{
						if(j) { DPF(1, _T(", ")); }
						if(m_rFactors[j]->IsSet())
						{
							DPF(1, _T("%d"), m_rFactors[j]->Get());
						}
						else
						{
							DPF(1, _T("-"));
						}
					}
					DPF(1, _T("]\n"));
*/					bNewCombinations = true;
				}
			}

			if(!bNewCombinations)
				break;

			// randomly set unset factors (or find a better way of prioritizing them)
			for(i=0; i<m_cFactors; i++)
			{
				if(!m_rFactors[i]->IsSet())
				{
					m_rFactors[i]->Set(rand() % m_rFactors[i]->Count());
				}
			}

			// update flags
			for(i=0; i<m_cInteractions; i++)
			{
				m_rInteractions[i]->UpdateFlags();
			}

			// add state to list:
			{
				RUINTArray rState = ArrayRef(new UINT[m_cFactors]);
				if(!CHECK(rState.Valid()))
					goto failed;
				listStates.push_back(rState);
				
//				DPF(1, _T("%d\t"), listStates.count());
				for(i=0; i<m_cFactors; i++)
				{
					rState[i] = m_rFactors[i]->Get();
//					m_rFactors[i]->Set(rState[i]);
//					DPF(1, _T("%s(%d) "), m_rFactors[i]->m_rFactor->Name().Get(), rState[i]);
				}
//				DPF(1, _T("\n"));
			}
		}

//		DPF(1, _T("Done. Transferring %d states to array\n"), listStates.count());

		// Transfer state list to member array:
		m_States = ArrayRef(new RUINTArray[listStates.count()]);
		it = listStates.begin();
		i = 0;
		while(it != listStates.end())
		{
			m_States[i++] = *it;
			++it;
		}
		m_cStates = i;
		
		bResult = true;
	failed:
		if(pRelated)
			delete [] pRelated;
		return bResult;
	}

	// calculate # of <uOrder>-way combinations in <uCount> factors:
	UINT CalcCombos(UINT uOrder, UINT uCount, UINT uStart = 0)
	{
		if(uOrder == 0) return 1;
		uOrder--;
		UINT uTotal = 0;
		for(UINT i=uStart; i < uCount-uOrder; i++)
			uTotal += CalcCombos(uOrder, uCount, i+1);
		return uTotal;
	}

	bool CreateInteractions(const UINT* pUserGroupOrders, UINT cUserGroupOrders)
	{
		UINT i,j;
		UINT cGroupOrders = 0;
		UINT *pGroupOrders = NULL;

		// sort factors by group (-1 first, then 0, then sorted custom group factors)
		RXFactorArray rSortedFactors = ArrayRef(new RXFactor[m_cFactors]);
		if(!CHECK(rSortedFactors.Valid()))
			goto failed;
		for(i=0; i<m_cFactors; i++)
			rSortedFactors[i] = m_rFactors[i];
		qsort((void*)(rSortedFactors.Get()), (size_t)m_cFactors, sizeof(RXFactor), cmpXFactorGroups);
		
		// count custom groups (possibly none)
		{
			INT iLastGroup = 0;
			for(i=0; i<m_cFactors; i++)
			{
				if(rSortedFactors[i]->m_iGroup > iLastGroup)
				{
					iLastGroup = rSortedFactors[i]->m_iGroup;
					cGroupOrders++;
				}
			}
		}

		if(cGroupOrders)
		{
			INT iLastGroup = 0;
			UINT iLastIndex = 0;		// index into pGroupOrders array
			UINT iFirst_i = 0;

			// create group order array (for all custom groups)
			pGroupOrders = new UINT[cGroupOrders];
			if(!CHECK(pGroupOrders))
				goto failed;
			
			// set default group orders to group sizes
			for(i=0; i<m_cFactors; i++)
			{
				if(rSortedFactors[i]->m_iGroup > iLastGroup)
				{
					if(CHECK(iLastIndex >= cGroupOrders))
						goto failed;

					// set order of previous group based on number in group:
					if(iLastGroup)
						pGroupOrders[iLastIndex-1] += i - iFirst_i;

					iLastGroup = rSortedFactors[i]->m_iGroup;
					pGroupOrders[iLastIndex++] = iLastGroup << 16;
					iFirst_i = i;
				}
			}
			if(iLastGroup)
				pGroupOrders[iLastIndex-1] += m_cFactors - iFirst_i;

			// override default orders with user provided orders:
			for(i=0; i<cUserGroupOrders; i++)
			{
				// iterate through 
				for(j=0; j<cGroupOrders; j++)
				{
					if((pGroupOrders[j] >> 16) == (pUserGroupOrders[i] >> 16))	// group number same
						pGroupOrders[j] = pUserGroupOrders[i];	// override with user order
				}
			}
		}

		// create all interactions:
		ComputeInteractions(rSortedFactors, pGroupOrders, cGroupOrders);

		if(pGroupOrders)
			delete [] pGroupOrders;
		return true;
	failed:
		if(pGroupOrders)
			delete [] pGroupOrders;
		m_rFactors.Release();
		m_rInteractions.Release();
		return false;
	}

	bool ComputeInteractions(
		RXFactorArray& rSortedFactors,	// factors sorted by group #
		UINT* pGroupOrders,				// custom group orders
		UINT cGroupOrders)				// # of custom groups
	{
		bool bResult = false;
		TList<RXInteraction> listInteractions;
		TList<RXInteraction>::Iterator it;
		UINT i,j,k;
		UINT *pCounters = NULL;

		// Initialize counter array to maximum order of custom groups:
		if(cGroupOrders && pGroupOrders)
		{
			UINT uMaxOrder = 0;
			for(i=0; i<cGroupOrders; i++)
				if((pGroupOrders[i] & 0xFFFF) > uMaxOrder)
					uMaxOrder = pGroupOrders[i] & 0xFFFF;
			if(uMaxOrder)
				pCounters = new UINT[uMaxOrder];
		}

		// create singleton interactions for group -1 factors
		for(i=0; i<m_cFactors; i++)
		{
			if(rSortedFactors[i]->m_iGroup < 0)
			{
				RXFactorArray rFactors;
				RXInteraction rInteraction;

				rFactors = ArrayRef(new RXFactor[1]);
				if(!CHECK(rFactors.Valid()))
					goto failed;
				rFactors[0] = rSortedFactors[i];
				rInteraction = PointerRef(new CXInteraction(rFactors, 1));
				if(!CHECK(rInteraction.Valid() && rInteraction->Valid()))
					goto failed;
				listInteractions.push_back(rInteraction);
			}
			else
			{
				// since it's sorted, we're done:
				break;
			}
		}

		// create all default pairwise interactions for other factors
		// start at leftover value of i from last loop:
		for(; i<m_cFactors && rSortedFactors[i]->m_iGroup == 0; i++)
		{
			for(j=i+1; j<m_cFactors; j++)
			{
				RXFactorArray rFactors;
				RXInteraction rInteraction;
				INT igroup = rSortedFactors[i]->m_iGroup;
				INT jgroup = rSortedFactors[j]->m_iGroup;
				if(igroup==0 || igroup!=jgroup)
				{
					bool bNotGroup = false;
					// make sure i is not in j's notgroup and visa-versa
					for(k=0; k<rSortedFactors[i]->m_cNotGroups; k++)
					{
						if(jgroup == rSortedFactors[i]->m_pNotGroups[k])
						{
							bNotGroup = true;
							break;
						}
					}
					if(bNotGroup)
						continue;
					for(k=0; k<rSortedFactors[j]->m_cNotGroups; k++)
					{
						if(igroup == rSortedFactors[j]->m_pNotGroups[k])
						{
							bNotGroup = true;
							break;
						}
					}
					if(bNotGroup)
						continue;

					// create factor array
					rFactors = ArrayRef(new RXFactor[2]);
					if(!CHECK(rFactors.Valid()))
						goto failed;
					rFactors[0] = rSortedFactors[i];
					rFactors[1] = rSortedFactors[j];
					rInteraction = PointerRef(new CXInteraction(rFactors, 2));
					if(!CHECK(rInteraction.Valid() && rInteraction->Valid()))
						goto failed;
					listInteractions.push_back(rInteraction);
				}

			}
		}

		// create all intra-group interactions
		if(pCounters)
		{
			INT iGroup = 0;
			UINT iStart = 0;
			UINT iStop = 0;
			for(; iStop<m_cFactors && rSortedFactors[iStop]->m_iGroup <= 0; iStop++) ;
			while(1)
			{
				bool bSomethingLeft = true;

				// set start past last group:
				iStart = iStop;

				if(iStart >= m_cFactors)
					break;		// we're done

				iGroup = rSortedFactors[iStart]->m_iGroup;

				// increment stop past group end
				for(; iStop<m_cFactors && rSortedFactors[iStop]->m_iGroup <= iGroup; iStop++) ;

				// pick up order (only if pCounters and therefore if pGroupOrders && cGroupOrders)
				UINT uOrder = 0;
				for(i=0; i<cGroupOrders; i++)
					if((pGroupOrders[i]>>16) == (UINT)iGroup)
						uOrder = pGroupOrders[i] & 0xFFFF;
				
				// Order cannot be larger than group:
				if(uOrder>iStop-iStart)
					uOrder = iStop-iStart;

				// Initialize counters:
				for(i=0; i<uOrder; i++)
					pCounters[i] = iStart+i;

				while(bSomethingLeft)
				{
					RXFactorArray rFactors;
					RXInteraction rInteraction;

					rFactors = ArrayRef(new RXFactor[uOrder]);
					if(!CHECK(rFactors.Valid()))
						goto failed;
					for(i=0; i<uOrder; i++)
						rFactors[i] = rSortedFactors[pCounters[i]];
					rInteraction = PointerRef(new CXInteraction(rFactors, uOrder));
					if(!CHECK(rInteraction.Valid() && rInteraction->Valid()))
						goto failed;
					listInteractions.push_back(rInteraction);

					bSomethingLeft = false;
					for(i=1; i<(uOrder+1); i++)
					{
						if(pCounters[uOrder-i] < iStop-i)
						{
							UINT uIndex = ++ pCounters[uOrder-i];
							for(--i; i>=1; i--)
								pCounters[uOrder-i] = ++uIndex;
							bSomethingLeft = true;
							break;
						}
					}
				}
			}
		}

		m_cInteractions = 0;
		m_rInteractions = ArrayRef(new RXInteraction[listInteractions.count()]);
		if(!CHECK(m_rInteractions.Valid()))
			goto failed;
		it = listInteractions.begin();
		while(it != listInteractions.end())
		{
			m_rInteractions[m_cInteractions++] = *it;
			++it;
		}

		bResult = true;
	failed:
		if(pCounters)
			delete [] pCounters;
		return bResult;
	}


};

RFactor NewCrossFactor(
	const XFACTOR* pFactors, 
	UINT cFactorCount, 
	Str strName, 
	const UINT* pGroupOrders, 
	UINT cGroupOrders)
{
	CTestFactor* pFactor = NULL;
	CCrossTestFactor* pCrossFactor = new CCrossTestFactor(strName);
	if(pCrossFactor)
	{
		if(pCrossFactor->Compute(pFactors, cFactorCount, pGroupOrders, cGroupOrders))
			pFactor = pCrossFactor;
	}
	RFactor rFactor;
	if(pFactor)
	{
		if(!rFactor.Own(pFactor).Valid())
			D_Out(_T("NewFactor: Out of Memory error\n"));
	}
	return rFactor;
}
