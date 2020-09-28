// Test Factors:
// Important Note: all test factors are 0 based, since everything is MUCH cleaner that way!
// Example of use:
#if(0)
class MyTest ...
{
...

	RFactor m_rRootFactor;		// keep this around as long as you want to keep the tree around
	
	// Individual factors:
	RFactor m_rMeshFactor;
	RFactor m_rOptFactor;			// iterate through normal optimization flags
	RFactor m_rInPlaceOptFactor;	// iterate through inplace optimization flags
	RFactor m_rNoOptFactor;			// count=1 factor for performing no optimizations
	RFactor m_rExtraFramesFactor;	// just extra frames to render at each setting
	RFactor m_rEvenFactor;			// count=2: used to check if overall index is even or odd
}

// now to initialize the factor tree:
// Note: This includes error checking...
BOOL MyTest::InitFactors(...)
{
	// Create factors (if something goes wrong, CHECKREF will catch it):
	if(	!CHECKREF(m_rMeshFactor = NewFactor(m_uMeshCount)) ||
		!CHECKREF(m_rOptFactor = NewFactor(m_OptFlags.Count())) ||
		!CHECKREF(m_rInPlaceOptFactor = NewFactor(m_InPlaceFlags.Count())) ||
		!CHECKREF(m_rNoOptFactor = NewFactor(1)) ||
		!CHECKREF(m_rEvenFactor = NewFactor(2)) ||
		!CHECKREF(m_rExtraFramesFactor = NewFactor(10)) )
		return false;

	// Now for the magic:
	m_rRootFactor = (m_rMeshFactor * (m_rOptFactor + m_rInPlaceFactor + m_rNoOptFactor) * m_rExtraFramesFactor) % m_rEvenFactor;

	// this means:
	//	iterate Mesh
	//		iterate Opt then InPlace then NoOpt
	//			iterate ExtraFrames
	//				Even = overall index % 2		(2 being the factor count of m_rEvenFactor)

	// This constructs a tree that looks like this: (where m_rRootFactor points to the CModFactor at the top)
	//									 CModFactor
	//									/		   \
	//						 CMultFactor			EvenFactor
	//						/		    \
	//				 CMultFactor		 ExtraFramesFactor
	//				/			\
	//		MeshFactor		 CAddFactor
	//						/		   \
	//				CAddFactor		    NoOptFactor
	//				/		\
	//		OptFactor		InPlaceFactor
	//
	// The factors that start with C are intermediate factors of specified type that you don't 
	//	have a reference to, but exist to make the whole thing work.

	// Or expressed a little differently:
	//	CModFactor
	//	+ CMultiFactor
	//	| + CMultiFactor
	//	| | + MeshFactor
	//	| | + CAddFactor
	//	| |   +	CAddFactor
	//	| |   | + OptFactor
	//	| |   | + InPlaceFactor
	//	| |   + NoOptFactor
	//	| + ExtraFramesFactor
	//	+ EvenFactor


	// Finally, make sure all is ok:
	//	if the reference is not valid, something went wrong in constructing the tree, like memory alloc failure
	//	if the factor is not valid, it's likely you have given an invalid count on construction of a factor (ie. 0)
	if(!(m_rRootFactor.Valid() && m_rRootFactor->Valid()))
		return false;

	return true;		// Success!
}

...
	// Later you can just set the test range:
	SetTestRange(1, m_rRootFactor.Count())		// since test numbers are 1 based (and inclusive)! :P
...

// now to use these factors:
MyTest::ExecuteTest(UINT uTestNumber)
{
...
	m_rRootFactor->Set(uTestNumber - 1);		// again, (-1) because uTestNumber is 1 based and inclusive

	if(m_rEvenFactor->Value() < 1)
		// do something for all even (adjusted) test numbers
	else
		// do something for all odd (adjusted) test numbers

	UINT iMesh = m_rMeshFactor->Value();

	if(!m_rNoOptFactor->Active())	// if NoOpt is not active, then we want to optimize one way or the other...
	{
		if(m_rOptFactor->Active())
			...						// this is active, do normal opt
		else
			...						// we are doing inplace opt
	}
	else
		...							// we are not optimizing

	// and get the others the same way... it's as simple as that!
}

// So you want to clean up, do you?
// the factors will clean themselves up when the references are destroyed, 
//	or you can force your references to release by calling Release():
MyTest::...()
{
	m_rRootFactor.Release();		// Releases tree and intermediate nodes.
	// Then if you want you can release your individual test factors:
	m_rMeshFactor.Release();		// etc...
}


// Here are some less trivial uses of the factors:
//	Given test factors a, b, c, and d, we construct root factor r.
//	We want to model a system that goes:
	iterate a
		one:
			iterate b
				iterate c
		two:
			iterate b
				iterate d

// You could construct and keep track of intermediate factors one and two and construct this like so:
	one = b * c;
	two = b * d;
	r = a * (one + two);
// Later, you can check one->Active() or two->Active() to see which side of the branch you're on.  Alternatavely,
//  for this example, at least, you could just check c->Active() and d->Active(), but other cases may not allow this.

// Note that b appears on both sides of the + branch.  This is perfectly legal, but may cause confusion
// if you don't fully understand how it works.  Here is an even simpler example that won't work for checking one/two ->Active():
	one = b;
	two = b;
	r = one + two;
// In the first example, the act of multiplying b & c or b & d creates an intermediate node that is stored
// in the reference one or two, respectively.  In the second example, however, one and two will point to the
// same node (b), so calling one->Active() is the same as calling two->Active().  It's the same as saying:
	one = two = b;
	r = b + b;
// You can get around this problem by adding a dummy factor (count = 1) and Moding or Multiplying it in, like so:
	dummy = NewFactor(1);
	one = b % dummy;		// operator must either be % or *
	two = b % dummy;
	r = one + two;
// The dummy will have no effect on the other factors, it simply forces an intermediate node to be created (a Mod node in this case).

// While it is legal to have the same factor on two sides of an addition branch, it makes no sense to have a factor repeated on each
// side of a Mod or Multiply branch.  If you do so, it probably won't crash or anything, but the results will be undefined.

// You may look at the r = b + b example and think: why can't you just go r = 2 * b.  It turns out that you can essentially do this,
// and if you really don't care which iteration you're in, you can just do this (where n is the number of times to loop):
	r = NewFactor(n) * b;
// This way, you're not keeping a reference specifically to the extra factor, because you don't care what it is.

// NOTE:	Combining two factors like (a * b) is NOT equivalent to (b * a).  Same goes for the + operator.  Order matters, 
//			because we're talking about iteration, not just a single calculation of a total.  With *, the left side is the 
//			outer loop.  With +, the left side is iterated first, then the right.



#endif





#ifndef _CTESTFACTOR_H_
#define _CTESTFACTOR_H_

#include "D_Util.h"
#include "LegacyTRef.h"
#include "Str.h"

struct XFACTOR;
class CXFactor;
class CTestFactor;
typedef TRef<CTestFactor> RFactor;		// reference to a factor

////////////////////////////////////////////////////////////////////
// Operators for constructing the test factor tree:

// Multiply operator means - for each of left do each right:
RFactor operator*(RFactor& left, RFactor& right);

// Addition operator means - do each left, then do each right:
RFactor operator+(RFactor& left, RFactor& right);

// Mod operator means - left value = parent value, right value = parent value % right count
RFactor operator%(RFactor& left, RFactor& right);


////////////////////////////////////////////////////////////////////
// Utility function for creating a factor:
RFactor NewFactor(UINT uCount, Str strName = _T("UnNamed"));

RFactor NewCrossFactor(
	const XFACTOR* pFactors, 
	UINT cFactorCount, 
	Str strName = _T("UnNamedCrossFactor"),
	const UINT* pGroupOrders = NULL, 
	UINT cGroupOrders = 0);


////////////////////////////////////////////////////////////////////

// Leaf test factor (also the base for branch factor types)
class CTestFactor
{
public:
	CTestFactor(UINT uCount, Str strName = _T("UnNamed")) : 
		m_bActive(false), m_uValue(0), m_uCount(uCount), m_strName(strName), m_pSkipHead(0), m_pCurSkip(0)
	{ m_bValid = uCount > 0; }

	virtual ~CTestFactor();

	////////////////////////////////////////////////////////////////////
	// Set current value (and propegate to leaves if this is a branch node):
	// Returns end of skip range, if value is in a skip section, or 0 for normal success.  Returns
	// Count if the rest of the range is entirely skipped.
	virtual UINT Set(UINT value, BOOL active = true);

	////////////////////////////////////////////////////////////////////
	// Things that shouldn't change over the life of a factor:
	inline UINT Count() { return m_uCount; }	// Get this factor's count
	inline BOOL Valid() { return m_bValid; }	// we are valid if our count is greater than 0

	////////////////////////////////////////////////////////////////////
	// Things that do change over the life of a factor:
	// Is this factor active?
	inline BOOL Active() { return m_bActive; }
	inline UINT Value() { return m_uValue; }		// Get this factor's current value

	inline Str Name() { return m_strName; }

	// Fills strOut with description of subtree, returns precedence level of this node:
	// the lower the precedence # the "higher" the precedence:
	// with bState = true, instead of names and counts, you get names and values (or N/A for inactive)
	virtual UINT Describe(Str& strOut, bool bState = false);

	// You can use these functions to skip ranges or clear skips:
	//	if uStop is unspecified, it will just skip a single number (uStart):
	//	note: both functions only return false for memory allocation failure
	bool Skip(UINT uStart, UINT uStop = 0);			// Skip from uStart to (uStop - 1)
	bool DontSkip(UINT uStart, UINT uStop = 0);		// Remove any skip in this range
	void ClearSkips();								// Remove all skip ranges (don't skip anything)

	inline bool operator==(const CTestFactor& other) { return this == &other; }

protected:
	friend RFactor operator*(RFactor& left, RFactor& right);
	friend RFactor operator+(RFactor& left, RFactor& right);
	friend RFactor operator%(RFactor& left, RFactor& right);
	friend class CXFactor;

	BOOL m_bActive;			// weather or not this node is active
	UINT m_uValue;			// keep track of value last set
	UINT m_uCount;			// remains constant throughout lifetime of tree
	BOOL m_bValid;			// is this factor and its subtree valid?

	RFactor m_rParent;		// Parent factor (weak reference)
	Str m_strName;			// name for this factor

	struct SSkipRange
	{
		UINT uStart, uStop;		// from uStart to (uStop - 1)
		SSkipRange *pNext;
	} *m_pSkipHead, *m_pCurSkip;
};

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// Cross Factor
// Use cross factors to combine a number of factors in a way that exercises
// pairwise (and n-wise) interactions, but does not have to exhaustivly test
// every combination of every factor.

// Use an array of these to create a Cross-Factor
struct XFACTOR
{
	RFactor rFactor;
	INT iGroup;
	const INT* pNotGroups;
	UINT cNotGroups;
};

// NOTE: Set all your skip ranges BEFORE creating your CrossFactor
// NOTE: Skipped ranges will not contribute to the final combinations AT ALL, this means that 
// changing skipped ranges changes (unpredictably) the output combinations from CrossFactor.
// The best thing to do when dealing with CrossFactors is to output a state string in order
// to debug testcases.  This will give you the state of all your factors.

// Use this macro to define your group-order array for the CrossFactor
// limit of 64k custom groups and max order is 64k
// (memory and computing limitations would make it impossible to go
// that high anyways)
// unspecified orders default to number of factors in group
#define XGROUPORDER(group, order) (((group)<<16) + (order))

// Note: the branch node types (Multiply and Add, etc...) are hidden, 
//	since you will never need to access these types directly.

#endif //_CTESTFACTOR_H_
