#ifndef ___CINCLUDE_H___
#define ___CINCLUDE_H___


#include <windows.h>
#include <CD3DTest.h>
#include <d3dx9.h>


#define MAX_INCLUDE_DATA_SIZE 1024000
#define MAX_DATA_SET_COUNT    32


class CIncludeFromResource : public ID3DXInclude
{
public:

    virtual HRESULT __stdcall Open( D3DXINCLUDE_TYPE, LPCSTR, LPCVOID, LPCVOID *, UINT * );
    virtual HRESULT __stdcall Close( LPCVOID )  { return S_OK; };
};


class CIncludeFromFile : public ID3DXInclude
{
public:

    CIncludeFromFile()
    {
        m_uDataSetIndex = 0;
        for ( UINT i = 0; i < MAX_DATA_SET_COUNT; ++i )
        {
            m_ppData[ i ] = NULL;
        }
    }

    ~CIncludeFromFile()
    {
        m_uDataSetIndex = 0;
        for ( UINT i = 0; i < MAX_DATA_SET_COUNT; ++i )
        {
            SAFEDELETEA( m_ppData[ i ] );
        }
    }

    virtual HRESULT __stdcall Open( D3DXINCLUDE_TYPE, LPCSTR, LPCVOID, LPCVOID *, UINT * );
    virtual HRESULT __stdcall Close( LPCVOID )  { return S_OK; };

private:

    UINT   m_uDataSetIndex;
    BYTE * m_ppData[ MAX_DATA_SET_COUNT ];
    CHAR   m_szFileName[ 256 ];
};


#endif