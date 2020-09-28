//Need to include d3d10_1.h before anything that includes d3d10.h until the header is fixed
#include "D3D10_1.h"

#include "new_off.h"
#include "d3dx10.h"
#include "new_on.h"

#include "StringConverters.hpp"
#include "StringUtils.h"

extern tstring __stdcall ToString( const D3DXVECTOR4& Vector );
extern bool __stdcall FromString( D3DXVECTOR4 *pVector, const tstring &str );

