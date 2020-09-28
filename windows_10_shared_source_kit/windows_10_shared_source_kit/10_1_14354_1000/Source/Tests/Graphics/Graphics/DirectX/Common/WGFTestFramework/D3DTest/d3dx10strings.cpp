#include "d3dx10strings.h"

tstring __stdcall ToString( const D3DXVECTOR4 &Vector )
{
	return ToString( Vector.x ) + tstring( ", " ) + ToString( Vector.y ) + tstring( ", " ) + ToString( Vector.z ) + tstring( ", " ) + ToString( Vector.w );
}

bool __stdcall FromString( D3DXVECTOR4 *pVector, const tstring &str )
{
   	tstring::size_type lastPos = 0;

	for (int i=0; i<4; i++)
    {
	   	tstring::size_type pos = str.find_first_of(",", lastPos);
		if (pos == tstring::npos)
			return false;
        
		// found a token, add it to the vector.
        if (! FromString( &pVector[i], str.substr(lastPos, pos - lastPos) ) )
			return false;

        // skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(",", pos);
//		if (lastPos == string::npos)
//			return false;
    }
	return true;
}
