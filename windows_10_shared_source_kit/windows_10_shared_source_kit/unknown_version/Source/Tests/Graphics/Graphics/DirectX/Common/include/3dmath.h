// 3DMath.h
//
// 3D math functions for use with direct3d.
//

#ifndef _3DMATH_H_
#define _3DMATH_H_

typedef struct _D3DMATRIX               D3DMATRIX, *LPD3DMATRIX;
typedef struct _D3DVECTOR               D3DVECTOR, *LPD3DVECTOR;
typedef struct _D3DVERTEX               D3DVERTEX, *LPD3DVERTEX; 


D3DMATRIX ZeroMatrix(void);
D3DMATRIX IdentityMatrix(void);
D3DMATRIX ProjectionMatrix(const float fovy, const float aspect, const float zNear, const float zFar);
D3DMATRIX ProjectionMatrix(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
D3DMATRIX ProjectionMatrix(const float near_plane, const float far_plane, const float fov);
D3DMATRIX OrthoMatrix(const float width, const float height, const float zNear, const float zFar);
D3DMATRIX OrthoMatrix(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
D3DMATRIX ViewMatrix(const D3DVECTOR& from, const D3DVECTOR& at, const D3DVECTOR& world_up, const float roll=0.0f);
D3DMATRIX RotateXMatrix(const float rads);
D3DMATRIX RotateYMatrix(const float rads);
D3DMATRIX RotateZMatrix(const float rads);
D3DMATRIX TranslateMatrix(const float dx, const float dy, const float dz);
D3DMATRIX TranslateMatrix(const D3DVECTOR& v);
D3DMATRIX ScaleMatrix(const float size);
D3DMATRIX ScaleMatrix(const float a, const float b, const float c);
D3DMATRIX ScaleMatrix(const D3DVECTOR& v);
D3DMATRIX MatrixMult(const D3DMATRIX & a, const D3DMATRIX & b);
D3DMATRIX MatrixMult(const D3DMATRIX & a, const float scalar);
D3DVECTOR TransformVector(const D3DVECTOR& v, const D3DMATRIX & m);
D3DVECTOR TransformNormal(const D3DVECTOR& v, const D3DMATRIX & mat);
D3DMATRIX MatrixInverse(const D3DMATRIX & m);
D3DMATRIX MatrixTranspose(const D3DMATRIX & m);
void TextureMapSpherical(D3DVERTEX * pVertices, int nCount);

#endif
