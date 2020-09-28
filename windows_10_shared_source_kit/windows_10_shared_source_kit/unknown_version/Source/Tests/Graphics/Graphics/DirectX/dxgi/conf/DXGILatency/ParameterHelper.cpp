//-----------------------------------------------------------------------------
// 
//  ParameterHelper
//
//
//-----------------------------------------------------------------------------

#include "parameterhelper.h"
#include <strsafe.h>

///////////////////////////////////////////////////////
// Macro:  STRING_TO_ENUM
//
// Purpose:
// Maps a test a stringized enum name to an actual enum
// value.
///////////////////////////////////////////////////////
#define STRING_TO_ENUM(x, y, z) \
	if ( (x) == _T( #y ) ) \
	{ \
		*z = y; \
		return true; \
	}


///////////////////////////////////////////////////////
// Macro:  ENUM_TO_STRING
//
// Purpose:
// Maps an enum value to a string (the stringized
// version of the enum value's name).
///////////////////////////////////////////////////////
#define ENUM_TO_STRING(x, y) \
	if ( x == y ) \
	{ \
		return tstring( _T( #y ) ); \
	}



//-----------------------------------------------------------------------------
// Function: ToString
//-----------------------------------------------------------------------------
tstring __stdcall ToString(TestType type)
{
    
    ENUM_TO_STRING( type, DEFAULT_TESTS );
    ENUM_TO_STRING( type, SET_TESTS );
    ENUM_TO_STRING( type, SECOND_DEVICE_CREATED_BEFORE_SET_TESTS );
    ENUM_TO_STRING( type, SECOND_DEVICE_CREATED_AFTER_SET_TESTS );
    
    return tstring( _T( "" ) );
};

//-----------------------------------------------------------------------------
// Function: FromString
//-----------------------------------------------------------------------------
bool __stdcall FromString(TestType *pType, const tstring &str)
{
    STRING_TO_ENUM( str, DEFAULT_TESTS, pType );
	STRING_TO_ENUM( str, SET_TESTS, pType );
	STRING_TO_ENUM( str, SECOND_DEVICE_CREATED_BEFORE_SET_TESTS, pType );
    STRING_TO_ENUM( str, SECOND_DEVICE_CREATED_AFTER_SET_TESTS, pType );

	return FromString
	(
		(UINT *) pType,
		str
	);
};

//-----------------------------------------------------------------------------
// Function: ToString
//-----------------------------------------------------------------------------
tstring __stdcall ToString(DXGI_SWAP_EFFECT type)
{
    ENUM_TO_STRING( type, DXGI_SWAP_EFFECT_DISCARD );
    ENUM_TO_STRING( type, DXGI_SWAP_EFFECT_SEQUENTIAL );
    ENUM_TO_STRING( type, (DXGI_SWAP_EFFECT)DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL );
    
    return tstring( _T( "" ) );
};

//-----------------------------------------------------------------------------
// Function: FromString
//-----------------------------------------------------------------------------
bool __stdcall FromString(DXGI_SWAP_EFFECT *pType, const tstring &str)
{
    STRING_TO_ENUM( str, DXGI_SWAP_EFFECT_DISCARD, pType );
    STRING_TO_ENUM( str, DXGI_SWAP_EFFECT_SEQUENTIAL, pType );
    STRING_TO_ENUM( str, (DXGI_SWAP_EFFECT)DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL, pType );

	return FromString
	(
		(UINT *) pType,
		str
	);
};
