#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#pragma warning( disable : 4786 )
#include <map>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <typeinfo>

#include <strstream>
#include <iostream>

using namespace std;


#include <windows.h>


/*
 *  Generic Type Management Class
 */

class CTyped
{
public:
    virtual ~CTyped()
    {
    }
    virtual const type_info &getType() const = 0;
    virtual size_t getSize() const = 0;
};


/*
 *  Generic Value Class
 */

class CValue : public CTyped
{
public:
    template< class T >
    bool set( const T &t )
    {
        T *pT;

        if ( getp( &pT ) )
        {
            *pT = t;
            return true;
        }
        return false;
    }
    template< class T >
    bool get( T &t ) const
    {
        T *pT;

        if ( getp( &pT ) )
        {
            t = *pT;
            return true;
        }
        return false;
    }
    template< class T >
    bool getp( T **ppT ) const
    {
        if ( ppT && ( typeid( T ) == getType() ) )
        {
            *ppT = static_cast< T* >( getv() );
            return true;
        }
        return false;
    }
    virtual void setva( va_list &args ) = 0;
    virtual bool equal( const CValue *pValue ) const = 0;
    virtual CValue *clone() const = 0;
protected:
    virtual void *getv() const = 0;
};


/*
 *  Typed Storage Class
 */

template< class T >
class CTypedValue : public CValue
{
public:
    CTypedValue()
    {
    }
    CTypedValue( const T &t) :
        m_t( t )
    {
    }
    virtual const type_info &getType() const
    {
        return typeid( T );
    }
    virtual size_t getSize() const
    {
        return sizeof( T );
    }
    virtual void setva( va_list &args )
    {
        m_t = va_arg( args, T );
    }
    virtual bool equal( const CValue *pValue ) const
    {
        T *pT;

        return pValue && pValue->getp( &pT ) && *pT == m_t;
    }
    virtual CValue *clone() const
    {
        return new CTypedValue< T >( m_t );
    }
protected:
    virtual void *getv() const
    {
        return &m_t;
    }
private:
    mutable T m_t;
};


/*
 *	Generic Set-Access Base Class
 */

class CValueSet : public CTyped
{
public:
    virtual CValue *makev() const = 0;
	virtual size_t getCount() const = 0;
    virtual bool enumSet( int i, CValue *pValue ) const = 0;
    virtual bool existElement( const CValue *pValue ) const = 0;
};


/*
 *  Generic Type-Domain Base Class
 */

class BaseSet : public CValueSet
{
public:
    BaseSet( bool bEditable, bool bOrable ) :
        m_bEditable( bEditable ),
        m_bOrable( bOrable )
    {
    }
    bool getEditable() const
    {
        return m_bEditable;
    }
    bool getOrable() const
    {
        return m_bOrable;
    }
    virtual bool add( const char *s, const CValue &value ) = 0;
    virtual bool lookup( const CValue *pValue, std::string &s ) const = 0;
    virtual bool lookup( const char *s, CValue *pValue ) const = 0;
    virtual bool enumSetStrings( int i, std::string &s ) const = 0;
private:
    bool m_bEditable;
    bool m_bOrable;
};


/*
 *  Typed Container Class ( static constants and enumerations values )
 */

template< class T >
class ValueSet : public std::list< std::pair< std::string, T > >
{
public:
    typedef std::pair< std::string, T > SetElement;
    typedef std::list< SetElement > SetType;

    bool add( const char *s, const CValue &v )
    {
        T t;
        return v.get( t ) && insert( end(), value_type( s, t ) ) != end();
    }
    bool lookup( const CValue *pValue, std::string &s ) const
    {
        T t;

        if ( pValue && pValue->get( t ) )
        {
            for( const_iterator it = begin(); it != end(); ++it )
            {
                if ( it->second == t )
                {
                    s = it->first;
                    return true;
                }
            }
        }
        return false;
    }
    bool lookup( const char *s, CValue *pValue ) const
    {
        for( const_iterator it = begin(); it != end(); ++it )
        {
            if ( it->first == s )
            {
                return pValue && pValue->set( it->second );
            }
        }
        return false;
    }
    bool enumSetStrings( int i, std::string &s ) const
    {
        CTypedValue< T > value;
        return enumSet( i, &value ) && lookup( &value, s );
    }
	size_t getCount() const
	{
		return size();
	}
    bool existElement( const CValue *pValue ) const
    {
        T t;

        if ( pValue && pValue->get( t ) )
        {
            for( const_iterator it = begin(); it != end(); ++it )
            {
                if ( it->second == t )
                {
                    return true;
                }
            }
        }
        return false;
    }
    bool enumSet( int i, CValue *pValue ) const
    {
        const_iterator it = begin();
		std::advance( it, i );
        return it != end() && pValue && pValue->set( it->second );
    }
};


/*
 *  Variable Domain Class supporting dynamic I/O
 */

template< class T >
class StreamSet : public BaseSet
{
public:
    StreamSet() :
        BaseSet( true, false )
    {
    }
    virtual const type_info &getType() const
    {
        return typeid( T );
    }
    virtual size_t getSize() const
    {
        return sizeof( T );
    }
    virtual CValue *makev() const
    {
        return new CTypedValue< T >;
    }
    virtual bool add( const char *s, const CValue &v )
    {
        return false;
    }
	virtual size_t getCount() const
	{
		return 0;
	}
    virtual bool existElement( const CValue *pValue ) const
    {
        return false;
    }
    virtual bool enumSet( int i, CValue *pValue ) const
    {
        return false;
    }
    virtual bool lookup( const CValue *pValue, std::string &s ) const
    {
        T t;

        if ( pValue && pValue->get( t ) )
        {
			ostrstream stream;
            stream << t << ends;
            std::auto_ptr< char > ptr( stream.str() );
            s = ptr.get();
            return true;
        }
        return false;
    }
    virtual bool lookup( const char *s, CValue *pValue ) const
    {
        T t;
		istrstream stream( (char *) s );
        stream >> t;
        return pValue && pValue->set( t );
    }
    virtual bool enumSetStrings( int i, std::string &s ) const
    {
        return false;
    }
};


/*
 *  Variable Domain Class supporting dynamic I/O and containing Fixed Values
 */

template< class T >
class ConstStreamSet : public StreamSet< T >, public ValueSet< T >
{
public:
    virtual bool add( const char *s, const CValue &v )
    {
        return ValueSet< T >::add( s, v );
    }
	virtual size_t getCount() const
	{
		return ValueSet< T >::getCount();
	}
    virtual bool existElement( const CValue *pValue ) const
    {
        return ValueSet< T >::existElement( pValue );
    }
    virtual bool enumSet( int i, CValue *pValue ) const
    {
        return ValueSet< T >::enumSet( i, pValue );
    }
    virtual bool lookup( const CValue *pValue, std::string &s ) const
    {
        return ValueSet< T >::lookup( pValue, s ) || StreamSet< T >::lookup( pValue, s );
    }
    virtual bool lookup( const char *s, CValue *pValue ) const
    {
        return ValueSet< T >::lookup( s, pValue ) || StreamSet< T >::lookup( s, pValue );
    }
    virtual bool enumSetStrings( int i, std::string &s ) const
    {
        return ValueSet< T >::enumSetStrings( i, s );
    }
};


/*
 *  Fixed Domain Class containing static constants and enumerations
 */

template< class T >
class TypeSet : public BaseSet, public ValueSet< T >
{
public:
    TypeSet() :
        BaseSet( false, false )
    {
    }
    virtual const type_info &getType() const
    {
        return typeid( T );
    }
    virtual size_t getSize() const
    {
        return sizeof( T );
    }
    virtual CValue *makev() const
    {
        return new CTypedValue< T >;
    }
    virtual bool add( const char *s, const CValue &v )
    {
        return ValueSet< T >::add( s, v );
    }
	virtual size_t getCount() const
	{
		return ValueSet< T >::getCount();
	}
    virtual bool existElement( const CValue *pValue ) const
    {
        return ValueSet< T >::existElement( pValue );
    }
    virtual bool enumSet( int i, CValue *pValue ) const
    {
        return ValueSet< T >::enumSet( i, pValue );
    }
    virtual bool lookup( const CValue *pValue, std::string &s ) const
    {
        return ValueSet< T >::lookup( pValue, s );
    }
    virtual bool lookup( const char *s, CValue *pValue ) const
    {
        return ValueSet< T >::lookup( s, pValue );
    }
    virtual bool enumSetStrings( int i, std::string &s ) const
    {
        return ValueSet< T >::enumSetStrings( i, s );
    }

protected:
    TypeSet( bool bEditable, bool bOrable ) :
        BaseSet( bEditable, bOrable )
    {
    }
};


/*
 *  Fixed Domain Class containing static constants that are ORable
 */

template< class T >
class BitSet : public TypeSet< T >
{
public:
    BitSet() :
        TypeSet<T>( false, true )
    {
    }
    virtual bool lookup( const CValue *pValue, std::string &s ) const
    {
        T t;

        s = "";
        if ( pValue && pValue->get( t ) )
        {
			if ( t == 0 )
			{
				s = "0";
			}
			else
			{
				for( const_iterator it = begin(); it != end(); ++it )
				{
					if ( ( it->second & t ) == it->second )
					{
						if ( !s.empty() )
						{
							s += " | ";
						}
						s += it->first;
					}
				}
			}
            return true;
        }
        return ValueSet< T >::lookup( pValue, s );
    }
    virtual bool lookup( const char *s, CValue *pValue ) const
    {
        T t = 0;

        std::auto_ptr< char > str( _strdup( s ) );

        for( char *pStr = strtok( str.get(), " |" ); pStr; pStr = strtok( NULL, " |" ) )
        {
            for( const_iterator it = begin(); it != end(); ++it )
            {
                if ( it->first == pStr )
                {
                    t |= it->second;
                    break;
                }
            }
        }
        return pValue->set( t );
    }
};


/*
 *  Convenient Type Domain Definition Macros
 */

#define ENUMARG( t ) #t, t
#define DWORDARG( t ) #t, (DWORD) t
#define INTARG( t ) #t, (INT) t
#define UINTARG( t ) #t, (UINT) t
#define STREAMARG( T ) StreamSet< T >()


/*
 *  Type-Domain Management Class
 */

class TypeMap : public std::map< std::string, BaseSet* >
{
public:
    typedef std::map< std::string, BaseSet* > MapType;

    virtual ~TypeMap()
    {
        for( iterator it = begin(); it != end(); ++it )
        {
            delete it->second;
        }
    }
    template< class T >
    bool addType( const StreamSet< T > & )
    {
        return findSet( typeid( T ).name(), Allocator< StreamSet< T > >() ) != end();
    }
    template< class T >
    bool addExtendType( const char *setName, const char *s, const T &t )
    {
        iterator it = findSet( setName, Allocator< ConstStreamSet< T > >() );
        return it != end() ? it->second->add( s, CTypedValue< T >( t ) ) : false;
    }
    template< class T >
    bool addType( const char *setName, const char *s, const T &t )
    {
        iterator it = findSet( setName, Allocator< TypeSet< T > >() );
        return it != end() ? it->second->add( s, CTypedValue< T >( t ) ) : false;
    }
    template< class T >
    bool addBitType( const char *setName, const char *s, const T &t )
    {
        iterator it = findSet( setName, Allocator< BitSet< T > >() );
        return it != end() ? it->second->add( s, CTypedValue< T >( t ) ) : false;
    }
    template< class T >
    bool addType( const char *s, const T &t )
    {
        return addType( typeid( T ).name(), s, t );
    }
    virtual BaseSet *getSet( const char *setName ) const
    {
        MapType::const_iterator it = find( setName );
        return it != end() ? it->second : 0;
    }
protected:
    template< class T >
    struct Allocator
    {
        operator T*() const
        {
            return new T;
        }
    };
    template< class T >
    iterator findSet( const char *setName, const Allocator< T > &allocator )
    {
        iterator it = find( setName );
        if ( it == end() )
        {
            typedef std::pair< iterator, bool > ResultType;

            ResultType result = insert( value_type( setName, allocator ) );
            it = result.first;
        }
        return it;
    }
};

class CTypeMapManager : public TypeMap
{
public:
    CTypeMapManager();
    virtual ~CTypeMapManager();
    bool addTypeMap( TypeMap *pTypeMap );
    bool gets( const char *setName, const CValue *pValue, std::string &s );
    bool sets( const char *setName, const char *s, CValue *pValue );
    CValue *makev( const char *setName, const char *valueStr );
    virtual BaseSet *getSet( const char *setName ) const;
private:
    typedef std::list< TypeMap* >::const_iterator Iterator;
    std::list< TypeMap* > m_List;
};

extern CTypeMapManager D3DTypeMap;


/*
 *	Generic Domain Collection Class
 */

class CCollection : public CValueSet
{
public:
    virtual CCollection *clone() const = 0;
    virtual void reset() = 0;
    virtual size_t getCount() const = 0;
    virtual bool addElement( const CValue *pValue ) = 0;
    virtual bool removeElement( const CValue *pValue ) = 0;

    typedef CCollection *( CCollection::*SetOp )( const CValueSet * );
    CCollection *unionSet( const CValueSet *pValueSet );
    CCollection *intersectSet( const CValueSet *pValueSet );
    CCollection *subtractSet( const CValueSet *pValueSet );

    CCollection *updateSet( const CValueSet *pValueSet, unsigned mOfn, unsigned mMin = 0 );
    template< class It >
    CCollection *updateSet( It begin, It end, SetOp pOp )
    {
        std::auto_ptr< CCollection > ptr( clone() );
        return ( this->*pOp )( ptr->updateSet( begin, end ) );
    }
    template< class It >
    CCollection *updateSet( It begin, It end )
    {
        reset();
        std::auto_ptr< CValue > ptr( makev() );
        if ( ptr.get() )
        {
            while ( begin != end )
            {
                if ( ptr->set( *begin++ ) )
                {
                    addElement( ptr.get() );
                }
            }
        }
        return this;
    }
    template< class Pred >
    CCollection *updateSet( const CValueSet *pValueSet, Pred pred )
    {
        if ( pValueSet )
	    {
		    reset();
		    std::auto_ptr< CValue > ptr( makev() );
		    for( int i=0; pValueSet->enumSet( i, ptr.get() ); i++ )
		    {
			    if ( pred( ptr.get() ) )
			    {
				    addElement( ptr.get() );
			    }
		    }
	    }
	    return this;
    }
private:
    void rcombo( const CValueSet *pValueSet, unsigned n, unsigned k, unsigned long t[], unsigned l = 0, unsigned i = 0 );
    void removeElements( std::vector< int > &v );
};


/*
 *	Typed Domain Collection
 */

template< class T >
class CDomainList : public CCollection, protected std::list< T >
{
public:
    CDomainList()
    {
    }
    CDomainList( const T &t )
    {
        push_back( t );
    }
    template< class It >
    CDomainList( It begin, It end )
    {
        updateSet( begin, end );
    }
    virtual const type_info &getType() const
    {
        return typeid( T );
    }
    virtual size_t getSize() const
    {
        return sizeof( T );
    }
    virtual CValue *makev() const
    {
        return new CTypedValue< T >;
    }
    virtual bool existElement( const CValue *pValue ) const
    {
        T t;

        if ( pValue && pValue->get( t ) )
        {
            const_iterator it = std::find( begin(), end(), t );
            return it != end();
        }
        return false;
    }
    virtual bool enumSet( int i, CValue *pValue ) const
    {
        const_iterator it = begin();
		std::advance( it, i );
        return it != end() && pValue && pValue->set( *it );
    }
    virtual CCollection *clone() const
    {
        return new CDomainList< T >( *this );
    }
    virtual void reset()
    {
        clear();
    }
    virtual size_t getCount() const
    {
        return size();
    }
    virtual bool addElement( const CValue *pValue )
    {
        T t;

        if ( pValue && pValue->get( t ) && !existElement( pValue ) )
        {
            push_back( t );
            return true;
        }
        return false;
    }
    virtual bool removeElement( const CValue *pValue )
    {
        T t;

        if ( pValue && pValue->get( t ) )
        {
            iterator it = std::find( begin(), end(), t );
            if ( it != end() )
            {
                erase( it );
                return true;
            }
        }
        return false;
    }
};


/*
 *	Parameter Attribute Flags
 */

const unsigned uInputParam      = 1L << 1;
const unsigned uOutputParam     = 1L << 2;
const unsigned uResultParam     = 1L << 3;
const unsigned uEditableParam   = 1L << 4;
const unsigned uCycleParam      = 1L << 5;

const unsigned uAllSubsets      = 0L;


/*
 *	CParamInfo contains information about a parameter
 */

class CParamInfo
{
public:
    template< class T >
    CParamInfo( const T &t, unsigned flags ) :
        m_pValue( new CTypedValue< T >( t ) ),
        m_pDomainSet( 0 ),
        m_domain( typeid( T ).name() ),
        m_flags( flags )
    {
    }
    template< class T >
    CParamInfo( const T &t, unsigned flags, const char *domain ) :
        m_pValue( new CTypedValue< T >( t ) ),
        m_pDomainSet( new CDomainList< T >( t ) ),
        m_domain( domain ),
        m_flags( flags )
    {
    }
    CParamInfo( const CParamInfo &paramInfo ) :
        m_pValue( 0 ),
        m_pDomainSet( 0 ),
        m_flags( 0 )
    {
        *this = paramInfo;
    }
    ~CParamInfo();
    CParamInfo &operator=( const CParamInfo &paramInfo );
    bool operator==( const CParamInfo &paramInfo ) const
    {
        return getValue()->equal( paramInfo.getValue() );
    }
    CValue *getValue() const
    {
        return m_pValue;
    }
    CCollection *getDomainSet() const
    {
        return m_pDomainSet;
    }
    unsigned getFlags() const
    {
        return m_flags;
    }
    unsigned setFlags( unsigned flags )
    {
        return m_flags = flags;
    }
    const char *getDomain() const
    {
        return m_domain.c_str();
    }
    BaseSet *getBaseSet() const;
    bool gets( const CValue *pValue, std::string &s ) const;
    bool sets( const char *s, CValue *pValue );

private:
    CValue *m_pValue;               // CTypedValue<T>*
    CCollection *m_pDomainSet;      // CDomainList<T>*
    std::string m_domain;
    unsigned m_flags;
};

/*
 *  Some Useful Domain Filtering Predicates
 */

struct All
{
    bool operator()( CValue *pValue ) const
    {
        return true;
    }
};

template< class T >
struct Eq
{
    Eq( const T &t ) :
        m_t( t )
    {
    }
    bool operator()( CValue *pValue ) const
    {
        T t;
        return pValue && pValue->get( t ) && t == m_t;
    }
    T m_t;
};


/*
 *	CParameters contains Parameter values
 */

class CParameters : public std::map< std::string, CParamInfo >
{
public:
    typedef std::map< std::string, CParamInfo > ParamMapType;

    // parameter addition functions
	bool addParamNode( const char *name, const CParameters &p, unsigned flags = 0 )
	{
        return addParameterInfo( name, CParamInfo( p, flags ) );
	}
    bool addParameter( const char *name, const char *s, unsigned flags, const char *domain )
    {
        return addParameter( name, std::string( s ), flags, domain );
    }
    bool addParameter( const char *name, const char *s, unsigned flags )
    {
        return addParameter( name, std::string( s ), flags  );
    }
    template<class T>
    bool addParameter( const char *name, const T &t, unsigned flags )
    {
        return addParameter( name, t, flags , typeid( T ).name() );
    }
    template<class T>
    bool addParameter( const char *name, const T &t, unsigned flags, const char *domain )
    {
        return addParameterInfo( name, CParamInfo( t, flags, domain ) );
    }
    bool existParameter( const char *name ) const
    {
        return findParameter( name ) != 0;
    }

    // paramter access functions
    template<class T>
    bool getParameter( const char *name, T &t ) const
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo && pParamInfo->getValue() ? pParamInfo->getValue()->get( t ) : false;
    }
    template<class T>
    bool getParameter( const char *name, T **ppT ) const
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo && pParamInfo->getValue() ? pParamInfo->getValue()->getp( ppT ) : false;
    }
    bool setParameter( const char *name, const char *s ) const
    {
        return setParameter( name, std::string( s ) );
    }
    template<class T>
    bool setParameter( const char *name, const T &t ) const
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo && pParamInfo->getValue() ? pParamInfo->getValue()->set( t ) : false;
    }
    bool getString( const char *name, std::string &s ) const
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo ? pParamInfo->gets( pParamInfo->getValue(), s ) : false;
    }
    bool setString( const char *name, const char *s )
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo ? pParamInfo->sets( s, pParamInfo->getValue() ) : false;
    }

    // parameter type domain functions
    const char *getTypeDomain( const char *name ) const
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo ? pParamInfo->getDomain() : 0;
    }
    const char *getTypeName( const char *name ) const
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo && pParamInfo->getValue() ? pParamInfo->getValue()->getType().name() : 0;
    }

	// domain set manipulation
    template< class It >
    bool unionDomain( const char *name, It begin, It end ) const
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo && pParamInfo->getDomainSet() ? pParamInfo->getDomainSet()->updateSet( begin, end, &CCollection::unionSet ), true : false;
    }
    template< class It >
    bool intersectDomain( const char *name, It begin, It end ) const
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo && pParamInfo->getDomainSet() ? pParamInfo->getDomainSet()->updateSet( begin, end, &CCollection::intersectSet ), true : false;
    }
    template< class It >
    bool subtractDomain( const char *name, It begin, It end ) const
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo && pParamInfo->getDomainSet() ? pParamInfo->getDomainSet()->updateSet( begin, end, &CCollection::subtractSet ), true : false;
    }
    template< class It >
    bool setDomain( const char *name, It begin, It end ) const
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo && pParamInfo->getDomainSet() ? pParamInfo->getDomainSet()->updateSet( begin, end ), true : false;
    }
    template< class Pred >
    bool setDomain( const char *name, Pred pred ) const
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo && pParamInfo->getDomainSet() ? pParamInfo->getDomainSet()->updateSet( pParamInfo->getBaseSet(), pred ), true : false;
    }
    bool setDomain( const char *name, unsigned mOfn, unsigned mMin = 0 ) const
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo && pParamInfo->getDomainSet() ? pParamInfo->getDomainSet()->updateSet( pParamInfo->getBaseSet(), mOfn, mMin ), true : false;
    }

    // parameter flag function
    bool setParameterFlags( const char *name, unsigned flags )
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo ? pParamInfo->setFlags( flags ), true : false;
    }
    bool getParameterFlags( const char *name, unsigned &flags )
    {
        CParamInfo *pParamInfo = findParameter( name );
        return pParamInfo ? flags = pParamInfo->getFlags(), true : false;
    }

    // parameter cycling and cycle-state functions
    typedef void (*CycleCallback)( void * );
    void cycle( CycleCallback callback, void *client = 0 );
    unsigned cycleCount();
    void setState( unsigned iState );
	unsigned getState() const;

    // parameter info lookup function
    virtual CParamInfo *findParameter( const char *name ) const;

protected:
    bool addParameterInfo( const char *name, const CParamInfo &paramInfo )
    {
        typedef std::pair< ParamMapType::iterator, bool > ResultType;
        ResultType result = insert( ParamMapType::value_type( name, paramInfo ) );
        return result.second;
    }

    typedef std::vector< CParamInfo* > ParamInfoVecType;
    void fetch( ParamMapType::const_iterator it, ParamInfoVecType &v ) const;

    void rcycle( ParamInfoVecType::iterator begin, 
                 ParamInfoVecType::iterator end,
                 CycleCallback callback, void *client );
    unsigned count( ParamInfoVecType::iterator begin, 
                    ParamInfoVecType::iterator end );
};

extern ostream &operator<<( ostream &stream, const class CParameters &params );
extern istream &operator>>( istream &stream, class CParameters &params );


#endif
