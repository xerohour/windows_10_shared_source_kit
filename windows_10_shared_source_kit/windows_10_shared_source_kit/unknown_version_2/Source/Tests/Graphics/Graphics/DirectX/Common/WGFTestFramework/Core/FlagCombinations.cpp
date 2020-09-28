// A class that allows you to construct a subset of all valid combinations of a user-defined set of flags.
// By: texr
// (c) Microsoft
#include "pch.h"

#include "FlagCombinations.h"

namespace FlagCombinations 
{

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions:  TODO: these eventually should be somewhere common (unless there are already functions that do this)

#define CHECKNEW(lhs, rhs) (((lhs) = (rhs)) ? true : false)

// return index of lowest bit set in dwFlags (or -1 if no bit set)
inline int LowestBitSet(DWORD dwFlags)
{	// quick approach? ...

	// bail if no flags set:
	if(!dwFlags)
		return -1;

	// this will hold the constructed position of the first set bit:
	int iPos = 0;

#if(0)
	// expressed as a loop:
	int b = 16;
	while(b)
	{
		if(!(dwFlags & ((1 << b) - 1)))
		{
			iPos|=(b);
			dwFlags >>= b;
		}
		b >>= 1;
	}
#endif

	// Done this way for optimization reasons:

	// if it isn't in the lower half, it must be in the upper (because there must be at least one bit set)
#define TMP1(b) if(!(dwFlags&((1<<(b))-1))) { iPos|=(b); dwFlags>>=(b); }

	// eliminate by halves:
	TMP1(16)
	TMP1(8)
	TMP1(4)
	TMP1(2)
	TMP1(1)

#undef TMP1
	
	return iPos;
}

// Return index of highest bit set in dwFlags (or -1 if no bit set)
inline int HighestBitSet(DWORD dwFlags)
{
	if(!dwFlags)
		return -1;

	// this will hold the constructed position of the highest set bit:
	int iPos = 31;

	// if it isn't in the upper half, it must be in the lower (because there must be at least one bit set)
#define TMP2(b) if(!(dwFlags&(((1<<(b))-1) << (b)))) iPos&=~(b); else dwFlags>>=(b);

	// eliminate by halves:
	TMP2(16)
	TMP2(8)
	TMP2(4)
	TMP2(2)
	TMP2(1)

#undef TMP2
	
	return iPos;	
}

// One more weird one... returns true if exactly one bit is set:
inline bool ExactlyOneBitSet(DWORD dwFlags)
{
	if(!dwFlags)		// must be at least one bit set to enter
		return false;

#if(0)		// Expressed as loop:
	int b = 16;
	while(b)
	{
		if(dwFlags & (((1 << b) - 1) << b))		// if bit set in upper-half
		{
			if(dwFlags & ((1 << b)-1))			// then bit shouldn't be set in the lower
				return false;
			dwFlags >>= b;
		}
		b >>= 1;
	}
#endif

#define TMP3(b) if(dwFlags & (((1 << (b)) - 1) << (b))) { if(dwFlags & ((1 << (b)) - 1)) return false; dwFlags>>=(b); }

	// eliminate by halves:
	TMP3(16)
	TMP3(8)
	TMP3(4)
	TMP3(2)
	TMP3(1)

#undef TMP3

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Local helper:

// recursive flag combination function, for each valid combination, we add dwFlags to listCombos:
// alternate version that counts or sets elements in an array:
bool CombineWith(
	UINT dwFlags,						// set of flags to or with (usually init to 0)

	std::map< UINT, tstring >::const_iterator flagsIter,	// current flag
	std::map< UINT, tstring >::iterator flagsEndIter,	// last flag

	UINT* pCombinations,		// if NULL, then we are just counting and setting iCombinationCount
	size_t& nCombinationCount,	// always init to 0.  After this call, it will be # of combos

	VALIDATEFUNCTION pValidateFunction)
{
	if(flagsIter != flagsEndIter)
	{
		UINT dwCurrentFlag = flagsIter->first;
		flagsIter++;

		// without current flag
		if(!CombineWith(dwFlags, flagsIter, flagsEndIter, pCombinations, nCombinationCount, pValidateFunction))
			return false;
		// with current flag
		if(!CombineWith((UINT)(dwFlags | dwCurrentFlag), flagsIter, flagsEndIter, pCombinations, nCombinationCount, pValidateFunction))
			return false;
	}
	else
	{
		// if the current set of flags are valid, write them::
		if(!pValidateFunction || (*pValidateFunction)(dwFlags))
		{
			if(pCombinations)
			{
				// We are writing:
				pCombinations[nCombinationCount] = dwFlags;
			}

			// Count up:
			nCombinationCount++;
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CFlagCombinations

CFlagCombinations::CFlagCombinations() :
	m_pFlagCombinations(0),
	m_nFlagCount(0),
	m_nCombinationCount(0)
{
}

CFlagCombinations::~CFlagCombinations()
{
	Clear();
}


bool CFlagCombinations::Init(const Flags flags, size_t nCount)
{
	Clear();

	if(nCount <= 0 || nCount >= 32)
	{
		WriteError(_T("CFlagCombinations::Init: parameter error\n"));
		return false;
	}

	size_t i;

	// Iterate through flags and check for a "default" flag (a flag that has no bits set, used for describing no flags set)
	size_t nDefaultCount = 0;
	Flags::const_iterator iter;
	for(iter = flags.begin(); iter != flags.end(); iter++)
	{
		if(iter->first == 0)
			nDefaultCount++;
	}

	// Error if there is more than one "default" flag:
	if(nDefaultCount > 1)
	{
		WriteError(_T("CFlagCombinations::Init: more than one default flag (flag with value of 0) detected.\n"));
		return false;
	}

	// copy flag array for ourselves:
	m_nFlagCount = nCount - nDefaultCount;				// Subtract default flag if there is one

	// Clear m_pFlags array (so if we encounter an error while filling, ptr to str will be NULL if unfilled:
	m_Flags.clear();

	// By default we fill the flag array with one flag each (matching array passed in)
	m_nCombinationCount = m_nFlagCount;
	if( !CHECKNEW( m_pFlagCombinations , new UINT[m_nCombinationCount] ) )
	{
		Clear();
		return false;
	}

	int iBitNum;
	UINT j = 0;		// keep track of *our* flag index (without default flag entry)
	i = -1;
	for(iter = flags.begin(); iter != flags.end(); iter++)
	{
		UINT flagValue = iter->first;
		tstring flagString = iter->second;

		i++;

		if(flagValue == 0)
		{	// This is the "default" flag entry:
			m_strDefaultDesc = flagString;

			// Skip the rest of the flag copying etc...
			continue;
		}

		if(!ExactlyOneBitSet(flagValue))
		{
			WriteError(_T("CFlagCombinations::Init: must have exactly one bit set for: pFlags[%d].  It equals: %X\n"), i, flagValue);
			Clear();
			return false;
		}

		// set our default flag combination array value:
        // disable bogus warning of buffer overrun: see how nDefaultCount and m_nFlagCount = nCount - nDefaultCount
        // get we know it is a bogus warning.  
#pragma warning( push )
#pragma warning( disable : 26053 )
		m_pFlagCombinations[j] = flagValue;
#pragma warning( pop )

		// copy contents:
		m_Flags.insert( Flags::value_type(iter->first, iter->second) );

		j++;
	}

	return true;
}

void CFlagCombinations::Clear()
{
	// free flag array:
	m_Flags.clear();

	m_nFlagCount = 0;

	m_strDefaultDesc.erase();

	ResetCombinations();
}

void CFlagCombinations::ResetCombinations()
{
	// free combination array:
	if(m_pFlagCombinations)
		delete [] m_pFlagCombinations;
	m_pFlagCombinations = NULL;
	m_nCombinationCount = 0;
}

bool CFlagCombinations::Expand(VALIDATEFUNCTION pValidateFunction)
{
	// Expand uses a two pass approach, first count valid combinations
	// then store the combinations

	// retrieve the number of valid flag combinations:
	size_t nCombinationCount = 0;
	UINT zero = (UINT)0;
	UINT* pCombos = NULL;

	Flags::iterator iter = m_Flags.end();

	CombineWith(zero, m_Flags.begin(), iter, pCombos, nCombinationCount, pValidateFunction);

	if(0 == nCombinationCount)
		return false;

	// Create new combination array of correct size:
	if(!CHECKNEW(pCombos, new UINT[nCombinationCount]))
		return false;

	size_t nCount = 0;
	iter = m_Flags.end();

	// Fill in the array:
	CombineWith(zero, m_Flags.begin(), iter, pCombos, nCount, pValidateFunction);
	if(nCombinationCount != nCount)
	{
		delete [] pCombos;
		return false;
	}

	ResetCombinations();
	m_nCombinationCount = nCount;
	m_pFlagCombinations = pCombos;

	return true;
}

// false for fail (because of no combos or memory allocation failure)
bool CFlagCombinations::SetCombinations(UINT* pFlagCombinations, size_t nCount)
{
	// fail if none passed in:
	if(nCount < 1 || NULL == pFlagCombinations) return false;

	// Now transfer to the array:
	UINT* pCombos;
	if(!CHECKNEW(pCombos, new UINT[nCount]))
		return false;

	ResetCombinations();
	m_nCombinationCount = nCount;
	m_pFlagCombinations = pCombos;

	for(size_t i = 0; i < m_nCombinationCount; i++)
		m_pFlagCombinations[i] = pFlagCombinations[i];

	return true;
}


// Constructs a string with all the flag descriptions seperated by "|" for given set of flags:
tstring CFlagCombinations::Describe(UINT dwFlags)
{
	if(dwFlags == 0 && !m_strDefaultDesc.empty())
		return m_strDefaultDesc;

	//static TCHAR buff[1024];
	tstring buff;
	TCHAR numberbuff[20];

	bool bPrevBit = false;

	while(dwFlags)
	{
		// get the highest bit set:
		int iHighestBit = HighestBitSet(dwFlags);

		// if this is true, what the hey?!
		if(iHighestBit < 0) break;

		// place OR seperator if we are between flags:
		if(bPrevBit)
			buff += _T("|");
		else
			bPrevBit = true;

		// if we don't have an entry for this flag, write the value and continue:
		Flags::const_iterator iter = m_Flags.find( iHighestBit );
		if(iter == m_Flags.end())
		{
			_stprintf(numberbuff, _T("(0x%x)"), 1 << (UINT)iHighestBit);
			buff += numberbuff;
		}
		else
		{
			buff += iter->second;
		}

		// remove bit to find next:
		dwFlags &= ~(1<<iHighestBit);
	}

	return buff;
}

}