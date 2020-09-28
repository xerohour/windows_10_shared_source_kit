#include "parameters.h"

#include <cd3ddef.h>

CTypeMapManager D3DTypeMap;

class CSimpleTypeMap : public TypeMap
{
public:
    CSimpleTypeMap();
};

/*
 *  CCollection Implementation
 */

CCollection *CCollection::unionSet( const CValueSet *pValueSet )
{
    if ( pValueSet )
    {
        std::auto_ptr< CValue > ptr( makev() );
        for( int i=0; pValueSet->enumSet( i, ptr.get() ); i++ )
        {
            addElement( ptr.get() );
        }
    }
    return this;
}

CCollection *CCollection::intersectSet( const CValueSet *pValueSet )
{
    if ( pValueSet )
    {
		std::vector< int > v;
        std::auto_ptr< CValue > ptr( makev() );
        for( int i=0; enumSet( i, ptr.get() ); i++ )
        {
            if ( !pValueSet->existElement( ptr.get() ) )
            {
				v.push_back( i );
            }
        }
        removeElements( v );
    }
    return this;
}

CCollection *CCollection::subtractSet( const CValueSet *pValueSet )
{
    if ( pValueSet )
    {
		std::vector< int > v;
        std::auto_ptr< CValue > ptr( makev() );
        for( int i=0; enumSet( i, ptr.get() ); i++ )
        {
            if ( pValueSet->existElement( ptr.get() ) )
            {
				v.push_back( i );
            }
        }
        removeElements( v );
    }
    return this;
}

void CCollection::removeElements( std::vector< int > &v )
{
    std::auto_ptr< CValue > ptr( makev() );
	while ( !v.empty() )
	{	
		int i = v.back();
		v.pop_back();
		if ( enumSet( i, ptr.get() ) )
        {
			removeElement( ptr.get() );
        }
	}
}

CCollection *CCollection::updateSet( const CValueSet *pValueSet, unsigned mOfn, unsigned mMin )
{
    unsigned n = pValueSet ? pValueSet->getCount() : 0;
	
	if ( n )
	{
		reset();
		std::vector< unsigned long > t( n );

		if ( ( mOfn = min( mOfn, n ) ) == 0 )
		{
			for( unsigned k = mMin; k <= n; k++)
			{
				rcombo( pValueSet, n, k, &*t.begin() );
			}
		}
		else
		{
			rcombo( pValueSet, n, mOfn, &*t.begin() );
		}
	}
	return this;
}

void CCollection::rcombo( const CValueSet *pValueSet, unsigned n, unsigned k, unsigned long t[], unsigned l, unsigned i )
{
    if ( l < k )
    {
        for( unsigned j=i; j <= n-k+l; j++ )
        {
            t[l] = j;
            rcombo( pValueSet, n, k, t, l+1, j+1 );
        }
    }
    else
    {
        unsigned long uResult = 0;
        std::auto_ptr< CValue > ptr( makev() );
        CValue *pValue = ptr.get();

        for (unsigned ii=0; ii<k; ii++)
        {
            pValueSet->enumSet( t[ii], pValue );
        
            unsigned long uValue;
            pValue->get( uValue );

            uResult |= uValue;
        }

        if ( pValue->set( uResult ) )
        {
            addElement( pValue );
        }
    }
}


/*
 *	CParamInfo Implementation
 */

CParamInfo::~CParamInfo()
{
    delete m_pDomainSet;
    delete m_pValue;
}

CParamInfo &CParamInfo::operator=( const CParamInfo &paramInfo )
{
    if ( this != &paramInfo )
    {
        this->~CParamInfo();
        m_pValue = paramInfo.m_pValue ? paramInfo.m_pValue->clone() : 0;
        m_pDomainSet = paramInfo.m_pDomainSet ? paramInfo.m_pDomainSet->clone() : 0;
        m_domain = paramInfo.m_domain;
        m_flags = paramInfo.m_flags;
    }
    return *this;
}

BaseSet *CParamInfo::getBaseSet() const
{
    return D3DTypeMap.getSet( getDomain() );
}

bool CParamInfo::gets( const CValue *pValue, std::string &s ) const
{ 
    return D3DTypeMap.gets( getDomain(), pValue, s );
}

bool CParamInfo::sets( const char *s, CValue *pValue )
{
    return D3DTypeMap.sets( getDomain(), s, pValue );
}


/*
 *  CParameters Implementation
 */

CParamInfo *CParameters::findParameter( const char *pathname ) const
{
	char *pEnd = strchr( pathname, '/' );
	if ( pEnd )
	{
		CParameters *pParams;
		std::string name( pathname, pEnd );
		return getParameter( name.c_str(), &pParams ) ? pParams->findParameter( pEnd + 1 ) : 0;
	}

	ParamMapType::const_iterator it = find( pathname );
    return it != end() ? const_cast< CParamInfo* >( &it->second ) : 0;
}

void CParameters::setState( unsigned iState )
{
    ParamInfoVecType paramInfos;

    fetch( begin(), paramInfos );
    for( std::vector< CParamInfo* >::reverse_iterator it = paramInfos.rbegin(); it != paramInfos.rend(); ++it )
    {
        CParamInfo *pParamInfo = *it;

        unsigned nCount = ( pParamInfo->getFlags() & uCycleParam ) && pParamInfo->getDomainSet() ? pParamInfo->getDomainSet()->getCount() : 0;
        if ( nCount )
        {
            pParamInfo->getDomainSet()->enumSet( iState % nCount, pParamInfo->getValue() );
            iState /= nCount;
        }
    }
}

unsigned CParameters::getState() const
{
    unsigned iState = 0, iProduct = 1;
    ParamInfoVecType paramInfos;

    fetch( begin(), paramInfos );
    for( std::vector< CParamInfo* >::reverse_iterator it = paramInfos.rbegin(); it != paramInfos.rend(); ++it )
    {
        CParamInfo *pParamInfo = *it;

        if ( pParamInfo->getDomainSet() && pParamInfo->getDomainSet()->getCount() )
        {
            std::auto_ptr< CValue > ptr( pParamInfo->getValue()->clone() );
            for( unsigned idx=0; pParamInfo->getDomainSet()->enumSet( idx, ptr.get() ); idx++ )
            {
                if ( pParamInfo->getValue()->equal( ptr.get() ) )
                {
                    iState += iProduct * idx;
                    break;
                }
            }
            iProduct *= pParamInfo->getDomainSet()->getCount();
        }
    }
    return iState;
}

void CParameters::cycle( CycleCallback callback, void *client )
{
    ParamInfoVecType paramInfos;

    fetch( begin(), paramInfos );
    rcycle( paramInfos.begin(), paramInfos.end(), callback, client );
}

unsigned CParameters::cycleCount()
{
    ParamInfoVecType paramInfos;

    fetch( begin(), paramInfos );
    return count( paramInfos.begin(), paramInfos.end() );
}

void CParameters::fetch( ParamMapType::const_iterator it, ParamInfoVecType &v ) const
{
    if ( it != end() )
    {
        CParameters *pParams;

        if ( it->second.getValue()->getp( &pParams ) )
        {
            pParams->fetch( pParams->begin(), v );
        }
        else
        {
            v.push_back( const_cast<CParamInfo*>( &it->second ) );
        }

        fetch( ++it, v );
    }
}

void CParameters::rcycle( ParamInfoVecType::iterator begin, 
                          ParamInfoVecType::iterator end,
                          CycleCallback callback, void *client )
{
    if ( begin == end )
    {
        if ( callback )
        {
            callback( client );
        }
    }
    else
    {
        CParamInfo *pParamInfo = *begin;

        if ( ( pParamInfo->getFlags() & uCycleParam ) && pParamInfo->getDomainSet() && pParamInfo->getDomainSet()->getCount() )
        {
            for( int i=0; pParamInfo->getDomainSet()->enumSet( i, pParamInfo->getValue() ); ++i )
            {
                rcycle( begin + 1, end, callback, client );
            }
        }
        else
        {
            rcycle( begin + 1, end, callback, client );
        }
    }
}

unsigned  CParameters::count( ParamInfoVecType::iterator begin, 
                              ParamInfoVecType::iterator end )
{
    unsigned count = 1;

    while ( begin != end )
    {
        CParamInfo *pParamInfo = *begin;

        if ( ( pParamInfo->getFlags() & uCycleParam ) && pParamInfo->getDomainSet() && pParamInfo->getDomainSet()->getCount() )
        {
            count *= pParamInfo->getDomainSet()->getCount();
        }
        begin++;
    }

    return count;
}

ostream &operator<<( ostream &stream, const class CParameters &params )
{
    stream << "(";

    for( CParameters::ParamMapType::const_iterator it = params.begin(); it != params.end(); ++it )
    {
		std::string s;
		it->second.gets( it->second.getValue(), s );

        //stream << "\"" << it->first.c_str() << "\", " << s.c_str() << endl;
        if ( it != params.begin() )
        {
            stream << ",";
        }

        stream << s.c_str();
    }
    return stream << ")";
}

istream &operator>>( istream &stream, class CParameters &params )
{
    return stream;
}


/*
 *  CTypeMapManager Implementation
 */

CTypeMapManager::CTypeMapManager()
{
    addTypeMap( new CSimpleTypeMap );
}

CTypeMapManager::~CTypeMapManager()
{
    for( Iterator it = m_List.begin(); it != m_List.end(); ++it )
    {
        delete *it;
    }
}

bool CTypeMapManager::addTypeMap( TypeMap *pTypeMap )
{
    if ( pTypeMap )
    {
        m_List.push_back( pTypeMap );
    }
    return pTypeMap != 0;
}

BaseSet *CTypeMapManager::getSet( const char *setName ) const
{
    BaseSet *pSet = TypeMap::getSet( setName );

    for( Iterator it = m_List.begin(); !pSet && it != m_List.end(); ++it )
    {
        pSet = (*it)->getSet( setName );
    }
    return pSet;
}

bool CTypeMapManager::gets( const char *setName, const CValue *pValue, std::string &s )
{
    BaseSet *pSet = getSet( setName );
    return pSet && pSet->lookup( pValue, s );
}

bool CTypeMapManager::sets( const char *setName, const char *s, CValue *pValue )
{
    BaseSet *pSet = getSet( setName );
    return pSet && pSet->lookup( s, pValue );
}

CValue *CTypeMapManager::makev( const char *setName, const char *valueStr )
{
    CValue *pValue = 0;
    BaseSet *pSet = getSet( setName );
    if ( pSet )
    {
        pValue = pSet->makev();
        if ( !sets( setName, valueStr, pValue ) )
        {
            delete pValue;
        }
    }
    return pValue;
}

/*
 *	TypeMap Definitions
 */

CSimpleTypeMap::CSimpleTypeMap()
{
    //addType( STREAMARG( bool ) );
    addType( STREAMARG( char ) );
    addType( STREAMARG( unsigned char ) );
    addType( STREAMARG( short ) );
    addType( STREAMARG( unsigned short ) );
    addType( STREAMARG( int ) );
    addType( STREAMARG( unsigned int ) );
    addType( STREAMARG( long ) );
    addType( STREAMARG( unsigned long ) );
    addType( STREAMARG( float ) );
    addType( STREAMARG( double ) );
    addType( STREAMARG( long double ) );
    //addType( STREAMARG( void* ) );
    //addType( STREAMARG( std::string ) );

    addType( "BOOL", INTARG( FALSE ) );
    addType( "BOOL", INTARG( TRUE ) );

    addType( STREAMARG( CParameters ) );
}
