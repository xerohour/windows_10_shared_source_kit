//////////////////////////////////////////////////////////////////////
// File:  DXGICustomTestApp.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a DXGI custom test app object.
//
// History:
// 10 Mar 2006  SEdmison  Factored from other tests into a shared lib.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "DXGICustomTestApp.hpp"


BEGIN_NAMED_VALUES( D3D10_DRIVER_TYPE )
    NAMED_VALUE( _T( "HW" ), D3D10_DRIVER_TYPE_HARDWARE )
    NAMED_VALUE( _T( "REF" ), D3D10_DRIVER_TYPE_REFERENCE )
    NAMED_VALUE( _T( "NULL" ), D3D10_DRIVER_TYPE_NULL )
    NAMED_VALUE( _T( "SW" ), D3D10_DRIVER_TYPE_SOFTWARE )
END_NAMED_VALUES( D3D10_DRIVER_TYPE )
