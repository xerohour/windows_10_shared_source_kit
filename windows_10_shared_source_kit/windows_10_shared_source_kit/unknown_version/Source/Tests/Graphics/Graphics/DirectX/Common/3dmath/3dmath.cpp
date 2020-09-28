#define D3D_OVERLOADS
#include "windows.h"
#include "d3d.h"
#include "3DMath.h"

// ============================================================================
//
//  Functions
//
// ============================================================================

// ----------------------------------------------------------------------------
//  Name:       ZeroMatrix
//  Purpose:	sets D3D matrix to all 0's
// ----------------------------------------------------------------------------

D3DMATRIX ZeroMatrix(void)
{
    D3DMATRIX ret;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            ret(i, j) = 0.0f;
		}
	}
    return ret;
} // end ZeroMatrix

// ----------------------------------------------------------------------------
//  Name:       IdentityMatrix
//  Purpose:	sets D3D matrix to Identiy (1's on diagonal, zero's elsewhere)
// ----------------------------------------------------------------------------

D3DMATRIX IdentityMatrix(void)
{
    D3DMATRIX ret;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++)	{
            ret(i, j) = 0.0f;
		}
		ret(i,i) = 1.0f;
	}
    return ret;
} // end IdentityMatrix
  
// ----------------------------------------------------------------------------
//  Name:       ProjectionMatrix
//  Purpose:	sets Projection matrix from fovy, aspect ratio, near and far planes
//  Notes:		
//		1. fovy		- field of view in Y direction in radians (for example pi/2)
//		2. aspect	- window_width / window_height
// ----------------------------------------------------------------------------

D3DMATRIX ProjectionMatrix(const float fovy, const float aspect, const float zNear, const float zFar) 
{
	float f = 1.f / (float)tan(fovy/2.f);

	D3DMATRIX ret = ZeroMatrix();
	ret(0,0) = f / aspect;
	ret(1,1) = f;
	ret(2,2) = (zFar)/(zFar-zNear);
	ret(2,3) = 1.f;
	ret(3,2) = zNear*zFar / (zNear - zFar);

	return ret;
} // end ProjectionMatrix


// ----------------------------------------------------------------------------
//  Name:       ProjectionMatrix
//  Purpose:	sets "shifted" projection matrix from left, right, bottom, top, near and far planes
// ----------------------------------------------------------------------------

D3DMATRIX ProjectionMatrix(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar) 
{

	D3DMATRIX ret = ZeroMatrix();
	ret(0,0) = 2.f * zNear / (right - left);
	ret(1,1) = 2.f * zNear / (top - bottom);
	ret(2,0) = (right + left) / (left - right);
	ret(2,1) = (top + bottom) / (bottom - top);
	ret(2,2) = (zFar)/(zFar-zNear);
	ret(2,3) = 1.f;
	ret(3,2) = zNear*zFar / (zNear - zFar);

	return ret;
} // end ProjectionMatrix


// ----------------------------------------------------------------------------
//  Name:       ProjectionMatrix
//  Purpose:	sets Projection matrix from fov, near and far planes
// ----------------------------------------------------------------------------

D3DMATRIX ProjectionMatrix(const float near_plane, 
				 const float far_plane, 
				 const float fov)
{
	float	c, s, Q;

	c = (float) cos(fov*0.5);
	s = (float) sin(fov*0.5);
	Q = s/(1.0f - near_plane/far_plane);

    D3DMATRIX ret = ZeroMatrix();
    ret(0, 0) = c;
    ret(1, 1) = c;
	ret(2, 2) = Q;
	ret(3, 2) = -Q*near_plane;
    ret(2, 3) = s;
    return ret;
}	// end ProjectionMatrix


// ----------------------------------------------------------------------------
//  Name:       OrthoMatrix
//  Purpose:	sets orthogonal projection matrix from width, height, near and far planes
// ----------------------------------------------------------------------------

D3DMATRIX OrthoMatrix(const float width, const float height, const float zNear, const float zFar)
{
	D3DMATRIX ret = IdentityMatrix();
	ret(0,0) = 2.f / width;
	ret(1,1) = 2.f / height;
	ret(2,2) = 1.f / (zFar - zNear);
	ret(3,2) = zNear / (zNear - zFar);
	return ret;
}

// ----------------------------------------------------------------------------
//  Name:       OrthoMatrix
//  Purpose:	sets "shifted" orthogonal projection matrix from left, right, bottom, top, near and far planes
// ----------------------------------------------------------------------------

D3DMATRIX OrthoMatrix(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
	D3DMATRIX ret = IdentityMatrix();
	ret(0,0) = 2.f / (right - left);
	ret(1,1) = 2.f / (top - bottom);
	ret(2,2) = 1.f / (zFar - zNear);
	ret(3,0) = (right + left) / (left - right);
	ret(3,1) = (top + bottom) / (bottom - top);
	ret(3,2) = zNear / (zNear - zFar);
	return ret;
}

// ----------------------------------------------------------------------------
//  Name:       ViewMatrix
//  Purpose:	Controls where the camara is.
//  Notes:		
//		1. Note the roll parameter is in radians and rools the viewpoint
//			around the viewing direction
// ----------------------------------------------------------------------------

D3DMATRIX ViewMatrix(const D3DVECTOR& from, 
		   const D3DVECTOR& at, 
		   const D3DVECTOR& world_up, 
		   const float roll)
{
    D3DMATRIX view = IdentityMatrix();
    D3DVECTOR up, right, view_dir;

    view_dir = Normalize(at - from);
	right = CrossProduct(world_up, view_dir);
	up = CrossProduct(view_dir, right);

	right = Normalize(right);
	up = Normalize(up);
	
    view(0, 0) = right.x;
    view(1, 0) = right.y;
    view(2, 0) = right.z;
    view(0, 1) = up.x;
    view(1, 1) = up.y;
    view(2, 1) = up.z;
    view(0, 2) = view_dir.x;
    view(1, 2) = view_dir.y;
    view(2, 2) = view_dir.z;
	
    view(3, 0) = -DotProduct(right, from);
    view(3, 1) = -DotProduct(up, from);
    view(3, 2) = -DotProduct(view_dir, from);

	// Set roll
	if (roll != 0.0f) {
		view = MatrixMult(RotateZMatrix(-roll), view);
	}

    return view;
} // end ViewMatrix

// ----------------------------------------------------------------------------
//  Name:       RotateXMatrix
//  Purpose:	Rotate matrix about X axis
// ----------------------------------------------------------------------------

D3DMATRIX RotateXMatrix(const float rads)
{
	float	cosine, sine;

	cosine = (float) cos(rads);
	sine = (float) sin(rads);
    D3DMATRIX ret = IdentityMatrix();
    ret(1,1) = cosine;
	ret(2,2) = cosine;
	ret(1,2) = -sine;
	ret(2,1) = sine;
    return ret;
} // end RotateXMatrix

// ----------------------------------------------------------------------------
//  Name:       RotateYMatrix
//  Purpose:	Rotate matrix about Y axis
// ----------------------------------------------------------------------------

D3DMATRIX RotateYMatrix(const float rads)
{
	float	cosine, sine;

	cosine = (float) cos(rads);
	sine = (float) sin(rads);
    D3DMATRIX ret = IdentityMatrix();
    ret(0,0) = cosine;
	ret(2,2) = cosine;
	ret(0,2) = sine;
	ret(2,0) = -sine;
    return ret;
} // end RotateY
  
// ----------------------------------------------------------------------------
//  Name:       RotateZMatrix
//  Purpose:	Rotate matrix about Z axis
// ----------------------------------------------------------------------------

D3DMATRIX RotateZMatrix(const float rads)
{
	float	cosine, sine;

	cosine = (float) cos(rads);
	sine = (float) sin(rads);
    D3DMATRIX ret = IdentityMatrix();
    ret(0,0) = cosine;
	ret(1,1) = cosine;
	ret(0,1) = -sine;
	ret(1,0) = sine;
    return ret;
} // end RotateZMatrix

// ----------------------------------------------------------------------------
//  Name:       TranslateMatrix
//  Purpose:    Returns matrix to translate by (dx, dy, dz)
// ----------------------------------------------------------------------------

D3DMATRIX TranslateMatrix(const float dx, const float dy, const float dz)
{
    D3DMATRIX ret = IdentityMatrix();
	ret(3, 0) = dx;
	ret(3, 1) = dy;
	ret(3, 2) = dz;
	return ret;
} // end TranslateMatrix

// ----------------------------------------------------------------------------
//  Name:       TranslateMatrix
//  Purpose:    Returns matrix to translate by v
// ----------------------------------------------------------------------------

D3DMATRIX TranslateMatrix(const D3DVECTOR& v)
{
    D3DMATRIX ret = IdentityMatrix();
	ret(3, 0) = v[0];
	ret(3, 1) = v[1];
	ret(3, 2) = v[2];
	return ret;
} // end TranslateMatrix

// ----------------------------------------------------------------------------
//  Name:       ScaleMatrix
//  Purpose:    scale matrix (uniform)
// ----------------------------------------------------------------------------

D3DMATRIX ScaleMatrix(const float size)
{
    D3DMATRIX ret = IdentityMatrix();
	ret(0, 0) = size;
	ret(1, 1) = size;
	ret(2, 2) = size;
	return ret;
} // end ScaleMatrix
  
// ----------------------------------------------------------------------------
//  Name:       ScaleMatrix
//  Purpose:	scale matrix
// ----------------------------------------------------------------------------

D3DMATRIX ScaleMatrix(const float a, const float b, const float c)
{
    D3DMATRIX ret = IdentityMatrix();
	ret(0, 0) = a;
	ret(1, 1) = b;
	ret(2, 2) = c;
	return ret;
} // end ScaleMatrix
  
// ----------------------------------------------------------------------------
//  Name:       ScaleMatrix
//  Purpose:	scale matrix
// ----------------------------------------------------------------------------

D3DMATRIX ScaleMatrix(const D3DVECTOR& v)
{
    D3DMATRIX ret = IdentityMatrix();
	ret(0, 0) = v.x;
	ret(1, 1) = v.y;
	ret(2, 2) = v.z;
	return ret;
} // end ScaleMatrix

// ----------------------------------------------------------------------------
//  Name:       MatrixMult
//  Purpose:	[C] = [A] * [B]
// ----------------------------------------------------------------------------

D3DMATRIX MatrixMult(const D3DMATRIX & a, const D3DMATRIX & b)
{
	D3DMATRIX ret = ZeroMatrix();

	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			for (int k=0; k<4; k++) {
				ret(i, j) += a(k, j) * b(i, k);
			}
		}
	}
	return ret;
} // end MatrixMult


// ----------------------------------------------------------------------------
//  Name:       MatrixMult
//  Purpose:	[C] = [A] * scalar
// ----------------------------------------------------------------------------

D3DMATRIX MatrixMult(const D3DMATRIX & a, const float scalar)
{
	D3DMATRIX ret = ZeroMatrix();

	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
				ret(i, j) += a(i, j) * scalar;
		}
	}
	return ret;
} // end MatrixMult


// ----------------------------------------------------------------------------
//  Name:       TransformVector
//  Purpose:	V' = V * [M]
// ----------------------------------------------------------------------------

D3DVECTOR TransformVector(const D3DVECTOR& v, const D3DMATRIX & m)
{
	float	hvec[4];

	for (int i=0; i<4; i++) {
		hvec[i] = 0.0f;
		for (int j=0; j<4; j++) {
			if (j==3) {
				hvec[i] += m(j, i);
			} else {
				hvec[i] += v[j] * m(j, i);
			}
		}
	}
	D3DVECTOR ret(hvec[0]/hvec[3], hvec[1]/hvec[3], hvec[2]/hvec[3]);

	return ret;
} // end TransformVector

// ----------------------------------------------------------------------------
//  Name:       TransformNormal
//  Purpose:	N' = N * [M]
// ----------------------------------------------------------------------------

D3DVECTOR TransformNormal(const D3DVECTOR& v, const D3DMATRIX & mat)
{
	D3DMATRIX	m;
	m = MatrixInverse(mat);
	m = MatrixTranspose(m);
	return Normalize(TransformVector(v, m));
	return TransformVector(v, m);
}  // end TransformNormal

// ----------------------------------------------------------------------------
//  Name:       MatrixInverse
//  Purpose:	Creates the inverse of a 4x4 matrix
// ----------------------------------------------------------------------------

static void	lubksb(D3DMATRIX & a, int *indx, float *b);
static void ludcmp(D3DMATRIX & a, int *indx, float *d);

D3DMATRIX MatrixInverse(const D3DMATRIX & m)
{
	D3DMATRIX	n, y;
	int			i, j, indx[4];
	float		d, col[4];

	n = m;
	ludcmp(n, indx, &d);

	for (j=0; j<4; j++) {
		for (i=0; i<4; i++) {
			col[i] = 0.0f;
		}
		col[j] = 1.0f;
		lubksb(n, indx, col);
		for (i=0; i<4; i++) {
			y(i, j) = col[i];
		}
	}
	return y;
} // end MatrixInverse

// ----------------------------------------------------------------------------
//  Name:       lubksb
//  Purpose:	backward subsitution
// ----------------------------------------------------------------------------

static void lubksb(D3DMATRIX & a, int *indx, float *b)
{
	int		i, j, ii=-1, ip;
	float	sum;

	for (i=0; i<4; i++) {
		ip = indx[i];
		sum = b[ip];
		b[ip] = b[i];
		if (ii>=0) {
			for (j=ii; j<=i-1; j++) {
				sum -= a(i, j) * b[j];
			}
		} else if (sum != 0.0) {
			ii = i;
		}
		b[i] = sum;
	}
	for (i=3; i>=0; i--) {
		sum = b[i];
		for (j=i+1; j<4; j++) {
			sum -= a(i, j) * b[j];
		}
		b[i] = sum/a(i, i);
	}
} // end lubksb

// ----------------------------------------------------------------------------
//  Name:       ludcmp
//  Purpose:	LU decomposition
// ----------------------------------------------------------------------------

static void ludcmp(D3DMATRIX & a, int *indx, float *d)
{
	float	vv[4]; // implicit scale for each row
	float	big, dum, sum, tmp;
	int		i, imax, j, k;

	*d = 1.0f;
	for (i=0; i<4; i++) {
		big = 0.0f;
		for (j=0; j<4; j++) {
			if ((tmp = (float) fabs(a(i, j))) > big) {
				big = tmp;
			}
		}
		vv[i] = 1.0f/big;
	}
	for (j=0; j<4; j++) {
		for (i=0; i<j; i++) {
			sum = a(i, j);
			for (k=0; k<i; k++) {
				sum -= a(i, k) * a(k, j);
			}
			a(i, j) = sum;
		}
		big = 0.0f;
		for (i=j; i<4; i++) {
			sum = a(i, j);
			for (k=0; k<j; k++) {
				sum -= a(i, k)*a(k, j);
			}
			a(i, j) = sum;
			if ((dum = vv[i] * (float)fabs(sum)) >= big) {
				big = dum;
				imax = i;
			}
		}
		if (j != imax) {
			for (k=0; k<4; k++) {
				dum = a(imax, k);
				a(imax, k) = a(j, k);
				a(j, k) = dum;
			}
			*d = -(*d);
			vv[imax] = vv[j];
		}
		indx[j] = imax;
		if (a(j, j) == 0.0f) {
			a(j, j) = 1.0e-20f; // can be 0.0 also...
		}
		if (j != 3) {
			dum = 1.0f/a(j, j);
			for (i=j+1; i<4; i++) {
				a(i, j) *= dum;
			}
		}
	}
} // end ludcmp
  
// ----------------------------------------------------------------------------
//  Name:       Matrix Transpose
//  Purpose:	[M] = [M]'
// ----------------------------------------------------------------------------

D3DMATRIX MatrixTranspose(const D3DMATRIX & m)
{
	D3DMATRIX	ret;
	int			i, j;

	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			ret(i, j) = m(j, i);
		}
	}

	return ret;
} // end MatrixTranspose

// ----------------------------------------------------------------------------
//  Name:       TextureMapSpherical
//  Purpose:    Spherically map a texture to an object
//              Requires untransformed data, assumes object is centered
//              at the origin, and that all vertices are surface vertices.
// ----------------------------------------------------------------------------

void TextureMapSpherical(D3DVERTEX * pVertices, int nCount)
{
	const float	pi = 3.1415926535f;
	float u, v;
	float maxx, minx, x_radius;
	float maxy, miny, y_radius;
	int i;

	maxx = minx = maxy = miny = 0;

	// get the x and y radius of the shape
	for(i = 0; i < nCount; i++)
	{
		if((pVertices[i].x) < minx)
			minx = pVertices[i].x;

		if((pVertices[i].x) > maxx)
			maxx = pVertices[i].x;

		if((pVertices[i].y) < miny)
			miny = pVertices[i].y;

		if((pVertices[i].y) > maxy)
			maxy = pVertices[i].y;
	}
	
	// we need the x_radius to be *slightly* off, so we don't get any results where
	// acos(x / x_radius) == 0 or pi.  This would mess up our later calculation of u+=pi.

	x_radius = (maxx - minx) / 1.99999f; // we need this to be slightly off
	y_radius = (maxy - miny) / 2.0f;

	for(i = 0; i < nCount; i++)
	{
		// calculate texture U coordinate based on model x-z coordinates.
		u = (float)acos(pVertices[i].x / x_radius);

		if(pVertices[i].z < 0.0f)
			u += pi;

		u /= (2.0f * pi); // convert to a value between 0 and 1.

		// calculate texture V coordinate based on model y coordinates.
		v = (float)acos(pVertices[i].y / y_radius);

		v /= pi;

		// now assign the calculated values.
		pVertices[i].tu = u;
		pVertices[i].tv = v;
	}
}
