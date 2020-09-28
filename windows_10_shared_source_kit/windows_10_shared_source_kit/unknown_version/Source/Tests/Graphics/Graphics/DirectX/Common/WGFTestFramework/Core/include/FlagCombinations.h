#ifndef _CFLAGCOMBINATIONS_H_
#define _CFLAGCOMBINATIONS_H_

//************************************************************************************************************
// With CFlagCombinations, you can define a set of flags, automatically expand those flags into combinations,
// while paying attention to rules about flags that may not be combined, keep track of that array of
// combinations, and convert any combination of the defined flags into a text description.

// NOTE: Important limitation: flags passed to Init() must be single bit flags only, no combined flags
//		(ie. 3 is made up of 2 & 1, 3 is not a flag, but a combination of the flags 2 and 1)
//		This class absolutely relies on a single flag having a single bit set.
//		This allows for reverse lookup, etc...
//	*	 There is one exception to this rule: one entry may have a value of 0.  This entry is used to set
//		the default description used when no flags are set.
//************************************************************************************************************

#include "fwtypes.h"

namespace FlagCombinations 
{

typedef bool (*VALIDATEFUNCTION)(DWORD dwFlags);

class CFlagCombinations
{
public:
	typedef std::map< UINT, tstring > Flags;

	CFlagCombinations();
	~CFlagCombinations();

	// setup the flag combo object:
	// you pass an array of individual flags to combine, along with a description for each flag...
	// In other words: this array is used to keep track of individual flags and their descriptions.
	// by default the flag combinations will be set to just the flags you pass in (ie, no combining)
	// Note: this will copy your array (as well as strings), not keep a reference.
	bool Init(const Flags flags, size_t nCount);

	// Clears all allocated memory and all flags:
	void Clear();

	// expands flags to all possible combinations, using a validate function prevent invalid combinations.
	// use Expand if most combinations are valid, with some exceptions.  If, however you just have a small,
	// limited set of valid flags, you may want to just explicitly set the combinations with the SetCombinations
	// function instead.
	// Expand returns true if >= 1 valid combinations were produced, false if 0 combinations produced or error
	// (memory allocation) occured.
	bool Expand(VALIDATEFUNCTION pValidateFunction = 0);

	// Create your own flag combination array and just pass it in when you just have a few:
	// this array will be copied, you are responsible for freeing of the array that you passed if necessary.
	bool SetCombinations(UINT* pFlagCombinations, size_t nCount);

	// Returns the number of valid flag combinations:
	inline size_t Count() const { return m_nCombinationCount; }

	// Returns the corresponding flag combination:
	// or 0 for invalid index
	UINT operator[](size_t iComboIndex) const { return (iComboIndex < m_nCombinationCount)? m_pFlagCombinations[iComboIndex] : (UINT)0; }

	// Use this to construct a text description of a particular combination of flags:
	// NOTE: don't keep the description around, it will point to a static char array:
	tstring Describe(UINT flags);

protected:
	void ResetCombinations();

	Flags m_Flags;					// owned array of individual flags (only 1 single bit may be set for each entry)
	size_t m_nFlagCount;				// size of flag array
	
	UINT* m_pFlagCombinations;		// array of combinations
	size_t m_nCombinationCount;		// number of combinations

	tstring m_strDefaultDesc;		// Description used if no flags set (this is pulled from the original FLAG array passed to Init() from a single
									// flag that has a value of 0, and a string description)
};

} // namespace

#endif // _CFLAGCOMBINATIONS_H_
