#ifndef __VertexStructDefs_h__
#define __VertexStructDefs_h__

#include "d3dx9.h"

struct FLOAT_VERTEX
{
	FLOAT x, y, z, w;
    FLOAT nx, ny, nz, nw;
	FLOAT tu, tv, tu1, tv1;

	FLOAT_VERTEX() { }
	FLOAT_VERTEX( FLOAT  _x, FLOAT  _y, FLOAT  _z, FLOAT  _w,
				  FLOAT _nx, FLOAT _ny, FLOAT _nz, FLOAT _nw,
				  FLOAT  _u, FLOAT  _v, FLOAT _u1, FLOAT _v1 )
	{ 
		  x =  _x;  y =  _y;  z =  _z;   w =  _w;
         nx = _nx; ny = _ny; nz = _nz;  nw = _nw;
		 tu = _u;  tv = _v; tu1 = _u1; tv1 = _v1;
    }
};

struct SHORT_VERTEX
{
	SHORT x, y, z, w;
    SHORT nx, ny, nz, nw;
	SHORT tu, tv, tu1, tv1;

	SHORT_VERTEX() { }
	SHORT_VERTEX( SHORT _x, SHORT _y, SHORT _z, SHORT _nx, SHORT _ny, SHORT _nz, SHORT _u, SHORT _v, SHORT _u1, SHORT _v1 )
	{ 
		  x =  _x;  y =  _y;  z =  _z;   w = 0x7fff;
         nx = _nx; ny = _ny; nz = _nz;  nw = 0;
		 tu = _u;  tv = _v; tu1 = _u1; tv1 = _v1;
    }
};

struct BYTE_VERTEX
{
	BYTE x, y, z, w;
    BYTE nx, ny, nz, nw;
	BYTE tu, tv, tu1, tv1;

	BYTE_VERTEX() { }
	BYTE_VERTEX( BYTE _x, BYTE _y, BYTE _z, BYTE _nx, BYTE _ny, BYTE _nz, BYTE _u, BYTE _v )
	{ 
		  x =  _x;  y =  _y;  z =  _z; w = 0xff; 
         nx = _nx; ny = _ny; nz = _nz; nw = 0;
		 tu =_u;  tv = _v;
    }
	BYTE_VERTEX( BYTE _x, BYTE _y, BYTE _z, BYTE _nx, BYTE _ny, BYTE _nz, BYTE _u, BYTE _v, BYTE _u1, BYTE _v1 )
	{ 
		  x =  _x;  y =  _y;  z =  _z;   w = 0xff;
         nx = _nx; ny = _ny; nz = _nz;  nw = 0;
		 tu = _u;  tv = _v; tu1 = _u1; tv1 = _v1;
    }
};

struct DWORD_VERTEX
{
	DWORD position;
    DWORD normal;
	DWORD texcoord;

	DWORD_VERTEX() {}
	DWORD_VERTEX( DWORD p, DWORD n, DWORD tc )
	{ 
		position = p;
        normal = n;
		texcoord = tc;
    }
};

/*struct COLOR_VERTEX
{
	D3DCOLOR position;
    D3DCOLOR normal;
	D3DCOLOR texcoord;

	COLOR_VERTEX() { }
	COLOR_VERTEX( D3DCOLOR v, D3DCOLOR n, D3DCOLOR tc )
	{
		position = v;
        normal = n;
		texcoord = tc;
    }
};*/

/*struct SHORTTEX_VERTEX
{
	D3DVALUE x, y, z;
    D3DVALUE nx, ny, nz;
	SHORT    tu, tv;

	SHORTTEX_VERTEX() { }
	SHORTTEX_VERTEX( const D3DXVECTOR3& v, const D3DXVECTOR3& n, SHORT _u, SHORT _v )
	{     x = v.x;  y = v.y;  z = v.z; 
         nx = n.x; ny = n.y; nz = n.z;
		 tu =_u;  tv = _v;
    }
};

struct SHORTPOS_VERTEX
{
	SHORT    x, y, z, w;
    D3DVALUE nx, ny, nz;
	D3DVALUE tu, tv;

	SHORTPOS_VERTEX() {}
	SHORTPOS_VERTEX( SHORT _x, SHORT _y, SHORT _z, const D3DXVECTOR3& n, float _u, float _v )
	{     x = _x;  y = _y;  z = _z; w = 0xff;
         nx = n.x; ny = n.y; nz = n.z;
		 tu =_u;  tv = _v;
    }
};

struct SHORTNORM_VERTEX
{
    D3DVALUE  x, y, z;
    SHORT    nx, ny, nz, nw;
	D3DVALUE tu, tv;

	SHORTNORM_VERTEX() {}
	SHORTNORM_VERTEX( const D3DXVECTOR3& v, SHORT _nx, SHORT _ny, SHORT _nz, float _u, float _v )
	{     x = v.x;  y = v.y;  z = v.z; 
         nx = _nx; ny = _ny; nz = _nz; nw = 0xff;
		 tu =_u;  tv = _v;
    }
};

struct UBYTEPOS_VERTEX
{
	BYTE    x, y, z, w;
    D3DVALUE nx, ny, nz;
	D3DVALUE tu, tv;

	UBYTEPOS_VERTEX() {}
	UBYTEPOS_VERTEX( BYTE _x, BYTE _y, BYTE _z, const D3DXVECTOR3& n, float _u, float _v )
	{     x = _x;  y = _y;  z = _z; w = 0xff;
         nx = n.x; ny = n.y; nz = n.z;
		 tu =_u;  tv = _v;
    }
};

struct COLORPOS_VERTEX
{
	D3DCOLOR position;
    D3DVALUE nx, ny, nz;
	D3DVALUE tu, tv;

	COLORPOS_VERTEX() {}
	COLORPOS_VERTEX( D3DCOLOR c, const D3DXVECTOR3& n, float _u, float _v )
	{ 
		position = c;
        nx = n.x; ny = n.y; nz = n.z;
		tu =_u;  tv = _v;
    }
};

struct DWORDPOS_VERTEX
{
	DWORD	 position;
    D3DVALUE nx, ny, nz;
	D3DVALUE tu, tv;

	DWORDPOS_VERTEX() {}
	DWORDPOS_VERTEX( DWORD p, const D3DXVECTOR3& n, float _u, float _v )
	{ 
		position = p;
        nx = n.x; ny = n.y; nz = n.z;
		tu =_u;  tv = _v;
    }
};*/

#endif // __VertexStructDefs_h__