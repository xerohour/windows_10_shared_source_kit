#ifndef _NODES_H_
#define _NODES_H_

#include <CD3DTest.h>
#include "parameters.h"


/*
 *  Definition of Framework TypeMap Class
 */

class CFrameworkTypeMap : public TypeMap
{
public:
    CFrameworkTypeMap();
};


/*
 *  General Purpose Dimension Class
 */

struct Dimension
{
    Dimension( UINT Width = 0, UINT Height = 0 ) :
        Width( Width ),
        Height( Height )
    {
    }
    bool operator==( const Dimension &dimension ) const
    {
        return Width == dimension.Width && Height == dimension.Height;
    }
    UINT Width;
    UINT Height;
};

inline istream &operator>>( istream &stream, Dimension &dimension )
{
    return stream >> dimension.Width >> dimension.Height;
}

inline ostream &operator<<( ostream &stream, const Dimension &dimension )
{
    return stream << "( " << dimension.Width << ", " << dimension.Height << " )";
}


/*
 *  Reusable Parameter Node declarations
 */

class CRenderTargetParams : public CParameters
{
public:
    CRenderTargetParams()
    {
        addParameter( "Dimension", Dimension( 1, 1 ), uInputParam | uCycleParam );
        addParameter( "Format", FMT_X8R8G8B8, uInputParam | uCycleParam );
        addParameter( "MultiSample", MULTISAMPLE_NONE, uInputParam | uCycleParam );
        addParameter( "MultisampleQuality", (DWORD) 0, uInputParam | uCycleParam );
        addParameter( "Lockable", (BOOL) TRUE, uInputParam | uCycleParam, "BOOL" );
    }
};

class COffscreenPlainSurfaceParams : public CParameters
{
public:
    COffscreenPlainSurfaceParams()
    {
        addParameter( "Dimension", Dimension(1, 1 ), uInputParam | uCycleParam );
        addParameter( "Format", FMT_X8R8G8B8, uInputParam | uCycleParam );
        addParameter( "Pool", (DWORD) POOL_DEFAULT, uInputParam | uCycleParam, "POOL" );
    }
};

class CTextureParams : public CParameters
{
public:
    CTextureParams()
    {
        addParameter( "Dimension", Dimension( 1, 1 ), uInputParam | uCycleParam );
        addParameter( "Levels", (UINT) 1, uInputParam | uCycleParam );
        addParameter( "Usage", (DWORD) 0, uInputParam | uCycleParam, "USAGE" );
        addParameter( "Format", FMT_X8R8G8B8, uInputParam | uCycleParam );
        addParameter( "Pool", (DWORD) POOL_DEFAULT, uInputParam | uCycleParam, "POOL" );
    }
};

class CCubeTextureParams : public CParameters
{
public:
    CCubeTextureParams()
    {
	    addParameter( "EdgeLength", (UINT) 1, uInputParam | uCycleParam );
	    addParameter( "Levels", (UINT) 1, uInputParam | uCycleParam );
	    addParameter( "Usage", (DWORD) 0, uInputParam | uCycleParam, "USAGE" );
        addParameter( "Format", FMT_X8R8G8B8, uInputParam | uCycleParam );
	    addParameter( "Pool", (DWORD) POOL_DEFAULT, uInputParam | uCycleParam, "POOL" );
	    addParameter( "Face", CUBEMAP_FACE_POSITIVE_X, uInputParam | uCycleParam );
    }
};

/*
 *  Useful Format Filtering Predicates
 */

struct TextureFmts
{
    bool operator()( CValue *pValue ) const;
};

struct ZBufferFmts
{
    bool operator()( CValue *pValue ) const;
};

struct DisplayModeFmts
{
    bool operator()( CValue *pValue ) const;
};

struct BackBufferFmts
{
    bool operator()( CValue *pValue ) const;
};

#endif
