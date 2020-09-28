//////////////////////////////////////////////////////////////////////////
//  FloatUtils.cpp
//  created:	2005/04/26
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "math.h"
#include "FloatUtils.h"

bool IsNAN(const float& f)
{
    return f != f;
}

bool IsNAN(const double& d)
{
    return d != d;
}

// Standard epsilon compare.
bool WGFTESTAPI CompareFloatEpsilon( const float &fsrc, const float &fref, float epsilon )
{
    if(IsNAN(fsrc))
        return IsNAN(fref);
    
	return fsrc == fref || fabsf( fsrc - fref ) < epsilon;
}

// Compare single-precision floats using ULP (unit last place) tolerance
bool WGFTESTAPI CompareFloatULP( const float &fsrc, const float &fref, int nULPTolerance )
{
    if(IsNAN(fsrc))
        return IsNAN(fref);

	if( fsrc == fref )
		return true;
		
	int nDiff = *( (DWORD *) &fsrc ) - *( (DWORD *) &fref );
	UINT uDiff = nDiff < 0 ? -nDiff : nDiff;
	return uDiff <= (UINT)nULPTolerance; 
}

// Compare double-precision floats using ULP (unit last place) tolerance
bool WGFTESTAPI CompareDoubleULP( const double &dsrc, const double &dref, UINT nULPTolerance )
{
    if (IsNAN(dsrc))
        return IsNAN(dref);

	if ( dsrc == dref )
		return true;
		
	__int64 nDiff = *( (__int64*) &dsrc ) - *( (__int64*) &dref );
	UINT uDiff = (UINT)(nDiff < 0 ? -nDiff : nDiff);
	return uDiff <= nULPTolerance; 
}

// Compare using relative epsilon error (relative diff < 2^nRelativeExp ).
bool WGFTESTAPI CompareFloatRelativeEpsilon( const float &fsrc, const float &fref, int nRelativeExp )
{
    if(IsNAN(fsrc))
        return IsNAN(fref);

	if( fsrc == fref )
		return true;
		
	return CompareFloatULP( fsrc, fref, 23 - nRelativeExp );
}
