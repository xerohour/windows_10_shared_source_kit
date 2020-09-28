#ifdef BUILD_DX8

#include <d3d8.h>
#include <d3dx8.h>

typedef LPDIRECT3D8				LPDIRECT3D;
typedef LPDIRECT3DDEVICE8		LPDIRECT3DDEVICE;
typedef LPDIRECT3DVERTEXBUFFER8 LPDIRECT3DVERTEXBUFFER;
typedef D3DMATERIAL8			D3DMATERIAL;
typedef D3DADAPTER_IDENTIFIER8  D3DADAPTER_IDENTIFIER;
typedef LPDIRECT3DTEXTURE8		LPDIRECT3DTEXTURE;
typedef D3DCAPS8				D3DCAPS;
typedef LPDIRECT3DSURFACE8		LPDIRECT3DSURFACE;
typedef D3DVIEWPORT8			D3DVIEWPORT;

#define Direct3DCreate(a) Direct3DCreate8(a)
#define SetGammaRamp(a ,b) SetGammaRamp(a, b)

#else

#include <d3d9.h>
#include <d3dx9.h>

typedef LPDIRECT3D9				LPDIRECT3D;
typedef LPDIRECT3DDEVICE9		LPDIRECT3DDEVICE;
typedef LPDIRECT3DVERTEXBUFFER9 LPDIRECT3DVERTEXBUFFER;
typedef D3DMATERIAL9			D3DMATERIAL;
typedef D3DADAPTER_IDENTIFIER9  D3DADAPTER_IDENTIFIER;
typedef LPDIRECT3DTEXTURE9		LPDIRECT3DTEXTURE;
typedef D3DCAPS9				D3DCAPS;
typedef LPDIRECT3DSURFACE9		LPDIRECT3DSURFACE;
typedef D3DVIEWPORT9			D3DVIEWPORT;

#define SetGammaRamp(a ,b) SetGammaRamp(0, a, b)

#define Direct3DCreate(a) Direct3DCreate9(a)
#define CreateTexture(a,b,c,d,e,f,g) CreateTexture(a,b,c,d,e,f,g,NULL)
#define CreateVertexBuffer(a,b,c,d,e) CreateVertexBuffer(a,b,c,d,e, NULL)
#endif


