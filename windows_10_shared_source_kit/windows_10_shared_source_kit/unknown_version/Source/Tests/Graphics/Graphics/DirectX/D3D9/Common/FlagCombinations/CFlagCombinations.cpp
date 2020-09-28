// A class that allows you to construct a subset of all valid combinations of a user-defined set of flags.
// By: texr
// (c) Microsoft

#include "CFlagCombinations.h"

#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions:  TODO: these eventually should be somewhere common (unless there are already functions that do this)

// return index of lowest bit set in dwFlags (or -1 if no bit set)
int LowestBitSet(DWORD dwFlags)
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
#define TMP(b) if(!(dwFlags&((1<<(b))-1))) { iPos|=(b); dwFlags>>=(b); }

	// eliminate by halves:
	TMP(16)
	TMP(8)
	TMP(4)
	TMP(2)
	TMP(1)

#undef TMP
	
	return iPos;
}

// Return index of highest bit set in dwFlags (or -1 if no bit set)
int HighestBitSet(DWORD dwFlags)
{
	if(!dwFlags)
		return -1;

	// this will hold the constructed position of the highest set bit:
	int iPos = 31;

	// if it isn't in the upper half, it must be in the lower (because there must be at least one bit set)
#define TMP(b) if(!(dwFlags&(((1<<(b))-1) << (b)))) iPos&=~(b); else dwFlags>>=(b);

	// eliminate by halves:
	TMP(16)
	TMP(8)
	TMP(4)
	TMP(2)
	TMP(1)

#undef TMP
	
	return iPos;	
}

// One more weird one... returns true if exactly one bit is set:
bool ExactlyOneBitSet(DWORD dwFlags)
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

#define TMP(b) if(dwFlags & (((1 << (b)) - 1) << (b))) { if(dwFlags & ((1 << (b)) - 1)) return false; dwFlags>>=(b); }

	// eliminate by halves:
	TMP(16)
	TMP(8)
	TMP(4)
	TMP(2)
	TMP(1)

#undef TMP

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Local helper:

// recursive flag combination function, for each valid combination, we add dwFlags to listCombos:
// alternate version that counts or sets elements in an array:
bool CombineWith(
	DWORD dwFlags,						// set of flags to or with (usually init to 0)

	CFlagCombinations::FLAG* pFlags,	// array of unique flags to combine
	UINT iCount,						// number of unique flags to combine

	DWORD* pCombinations,		// if NULL, then we are just counting and setting iCombinationCount
	UINT& iCombinationCount,	// always init to 0.  After this call, it will be # of combos

	CFlagCombinations::VALIDATEFUNCTION pValidateFunction)
{
	if(iCount)
	{
		iCount--;

		// without current flag
		if(!CombineWith(dwFlags, pFlags, iCount, pCombinations, iCombinationCount, pValidateFunction))
			return false;
		// with current flag
		if(!CombineWith(dwFlags | pFlags[iCount].dwFlag, pFlags, iCount, pCombinations, iCombinationCount, pValidateFunction))
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
				pCombinations[iCombinationCount] = dwFlags;
			}

			// Count up:
			iCombinationCount++;
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CFlagCombinations

CFlagCombinations::CFlagCombinations() :
	m_pFlags(0),
	m_pFlagCombinations(0),
	m_iFlagCount(0),
	m_iCombinationCount(0),
	m_tcsDefaultDesc(0)
{
}

CFlagCombinations::~CFlagCombinations()
{
	Clear();
}


bool CFlagCombinations::Init(const FLAG* pFlags, UINT iCount)
{
	Clear();

	// at the same time, clear our reverse lookup table (allows us to lookup a flag in m_pFlags by the bit that's set: (0 to 31)
	::ZeroMemory(m_pFlagMap, 32 * sizeof(UINT));

	if(iCount <= 0 || iCount >= 32 || !pFlags)
	{
		D_Out(_T("CFlagCombinations::Init: parameter error\n"));
		return false;
	}

	UINT i;

	// Iterate through flags and check for a "default" flag (a flag that has no bits set, used for describing no flags set)
	UINT iDefaultCount = 0;
	for(i = 0; i < iCount; i++)
	{
		if(pFlags[i].dwFlag == 0)
			iDefaultCount++;
	}

	// Error if there is more than one "default" flag:
	if(iDefaultCount > 1)
	{
		D_Out(_T("CFlagCombinations::Init: more than one default flag (flag with value of 0) detected.\n"));
		return false;
	}

	// copy flag array for ourselves:
	m_iFlagCount = iCount - iDefaultCount;				// Subtract default flag if there is one
	if( !CHECKNEW( m_pFlags , new FLAG[m_iFlagCount] ) )
	{
		Clear();
		return false;
	}

	// Clear m_pFlags array (so if we encounter an error while filling, ptr to str will be NULL if unfilled:
	::ZeroMemory(m_pFlags, m_iFlagCount * sizeof(DWORD));

	// By default we fill the flag array with one flag each (matching array passed in)
	m_iCombinationCount = m_iFlagCount;
	if( !CHECKNEW( m_pFlagCombinations , new DWORD[m_iCombinationCount] ) )
	{
		Clear();
		return false;
	}

	int iBitNum;
	UINT j = 0;		// keep track of *our* flag index (without default flag entry)
	for(i = 0; i < iCount; i++)
	{
		if(pFlags[i].dwFlag == 0)
		{	// This is the "default" flag entry:
			if(!CHECKNEW(m_tcsDefaultDesc, new TCHAR[_tcslen(pFlags[i].tcsDesc)+1]))
			{
				Clear();
				return false;
			}

			_tcscpy(m_tcsDefaultDesc, pFlags[i].tcsDesc);

			// Skip the rest of the flag copying etc...
			continue;
		}

		if(!ExactlyOneBitSet(pFlags[i].dwFlag))
		{
			D_Out(_T("CFlagCombinations::Init: must have exactly one bit set for: pFlags[%d].  It equals: %X\n"), i, pFlags[i].dwFlag);
			Clear();
			return false;
		}

		// set our default flag combination array value:
		m_pFlagCombinations[j] = pFlags[i].dwFlag;

		// copy contents:
		m_pFlags[j].dwFlag = pFlags[i].dwFlag;
		// m_pFlags[i].tcsDesc = pFlags[i].tcsDesc;		// uncomment if we want to reference the original strings (then change a bunch more code)

		if(pFlags[i].tcsDesc)
		{
			if( !CHECKNEW( m_pFlags[j].tcsDesc , new TCHAR[_tcslen(pFlags[i].tcsDesc)+1] ) )
			{
				Clear();
				return false;
			}

			_tcscpy(m_pFlags[j].tcsDesc, pFlags[i].tcsDesc);
		}
		else
		{
			m_pFlags[j].tcsDesc = 0;
		}

		// Set the reverse lookup index:
		iBitNum = LowestBitSet(m_pFlags[j].dwFlag);
		if(iBitNum < 0)
		{
			Clear();
			D_Out(_T("CFlagCombinations::Init: no flag set in pFlags[%d]\n"), i);
			return false;
		}
		m_pFlagMap[iBitNum] = j + 1;

		j++;
	}

	return true;
}

void CFlagCombinations::Clear()
{
	// free flag array:
	if(m_pFlags)
	{
		for(UINT i = 0; i < m_iFlagCount; i++)
		{
			delete [] m_pFlags[i].tcsDesc;
		}
		delete [] m_pFlags;
	}
	m_pFlags = 0;
	m_iFlagCount = 0;

	delete [] m_tcsDefaultDesc;
	m_tcsDefaultDesc = 0;

	ResetCombinations();
}

void CFlagCombinations::ResetCombinations()
{
	// free combination array:
	if(m_pFlagCombinations)
		delete [] m_pFlagCombinations;
	m_pFlagCombinations = 0;
	m_iCombinationCount = 0;
}

bool CFlagCombinations::Expand(VALIDATEFUNCTION pValidateFunction)
{
	// Expand uses a two pass approach, first count valid combinations
	// then store the combinations

	// retrieve the number of valid flag combinations:
	UINT iCombinationCount = 0;
	CombineWith(0, m_pFlags, m_iFlagCount, NULL, iCombinationCount, pValidateFunction);

	if(0 == iCombinationCount)
		return false;

	DWORD* pCombos;

	// Create new combination array of correct size:
	if(!CHECKNEW(pCombos, new DWORD[iCombinationCount]))
		return false;

	UINT iCount = 0;

	// Fill in the array:
	CombineWith(0, m_pFlags, m_iFlagCount, pCombos, iCount, pValidateFunction);
	if(iCombinationCount != iCount)
	{
		delete [] pCombos;
		return false;
	}

	ResetCombinations();
	m_iCombinationCount = iCount;
	m_pFlagCombinations = pCombos;

	return true;
}

// false for fail (because of no combos or memory allocation failure)
bool CFlagCombinations::SetCombinations(DWORD* pFlagCombinations, UINT iCount)
{
	// fail if none passed in:
	if(iCount < 1 || NULL == pFlagCombinations) return false;

	// Now transfer to the array:
	DWORD* pCombos;
	if(!CHECKNEW(pCombos, new DWORD[iCount]))
		return false;

	ResetCombinations();
	m_iCombinationCount = iCount;
	m_pFlagCombinations = pCombos;

	for(UINT i = 0; i < m_iCombinationCount; i++)
		m_pFlagCombinations[i] = pFlagCombinations[i];

	return true;
}


// Constructs a string with all the flag descriptions seperated by "|" for given set of flags:
TCHAR* CFlagCombinations::Describe(DWORD dwFlags)
{
	if(dwFlags == 0 && m_tcsDefaultDesc)
		return m_tcsDefaultDesc;

	static TCHAR buff[1024];
	static TCHAR numberbuff[20];

	buff[0] = 0;

	bool bPrevBit = false;

	while(dwFlags)
	{
		// get the highest bit set:
		int iHighestBit = HighestBitSet(dwFlags);

		// can't happen:
		if(iHighestBit < 0) break;

		// place OR seperator if we are between flags:
		if(bPrevBit)
			_tcscat(buff, _T("|"));
		else
			bPrevBit = true;

		// which flag is it...
		int iFlag = m_pFlagMap[iHighestBit];

		// if we don't have an entry for this flag, write the value and continue:
		if(0 == iFlag)
		{
			_stprintf(numberbuff, _T("(0x%x)"), 1 << (UINT)iHighestBit);
			_tcscat(buff, numberbuff);
		}
		else
			_tcscat(buff, m_pFlags[iFlag-1].tcsDesc);

		// remove bit to find next:
		dwFlags &= ~(1<<iHighestBit);
	}

	return buff;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
