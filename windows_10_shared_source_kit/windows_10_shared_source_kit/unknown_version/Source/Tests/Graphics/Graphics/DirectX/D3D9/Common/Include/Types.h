/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	types.h
 *
 ***************************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

#ifndef DX_SHARED_DEFINES

/*
 * This definition is shared with other DirectX components whose header files
 * might already have defined it. Therefore, we don't define this type if
 * someone else already has (as indicated by the definition of
 * DX_SHARED_DEFINES). We don't set DX_SHARED_DEFINES here as there are
 * other types in this header that are also shared. The last of these
 * shared defines in this file will set DX_SHARED_DEFINES.
 */
typedef float D3DVALUE, *LPD3DVALUE;
typedef float D3DVAL, *LPD3DVAL;

#endif /* DX_SHARED_DEFINES */

#ifndef RGB_MAKE
/*
 * Format of CI colors is
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    alpha      |         color index           |   fraction    |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#define CI_GETALPHA(ci)    ((ci) >> 24)
#define CI_GETINDEX(ci)    (((ci) >> 8) & 0xffff)
#define CI_GETFRACTION(ci) ((ci) & 0xff)
#define CI_ROUNDINDEX(ci)  CI_GETINDEX((ci) + 0x80)
#define CI_MASKALPHA(ci)   ((ci) & 0xffffff)
#define CI_MAKE(a, i, f)    (((a) << 24) | ((i) << 8) | (f))

/*
 * Format of RGBA colors is
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    alpha      |      red      |     green     |     blue      |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#define RGBA_GETALPHA(rgb)      ((rgb) >> 24)
#define RGBA_GETRED(rgb)        (((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      (((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       ((rgb) & 0xff)
#define RGBA_MAKE(r, g, b, a)   ((D3DCOLOR) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

/* D3DRGB and D3DRGBA may be used as initialisers for D3DCOLORs
 * The float values must be in the range 0..1
 */
#define D3DRGB(r, g, b) \
    (0xff000000L | ( ((long)((r) * 255)) << 16) | (((long)((g) * 255)) << 8) | (long)((b) * 255))
#define D3DRGBA(r, g, b, a) \
    (   (((long)((a) * 255)) << 24) | (((long)((r) * 255)) << 16) \
    |   (((long)((g) * 255)) << 8) | (long)((b) * 255) \
    )

/*
 * Format of RGB colors is
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    ignored    |      red      |     green     |     blue      |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#define RGB_GETRED(rgb)         (((rgb) >> 16) & 0xff)
#define RGB_GETGREEN(rgb)       (((rgb) >> 8) & 0xff)
#define RGB_GETBLUE(rgb)        ((rgb) & 0xff)
#define RGBA_SETALPHA(rgba, x) (((x) << 24) | ((rgba) & 0x00ffffff))
#define RGB_MAKE(r, g, b)       ((D3DCOLOR) (((r) << 16) | ((g) << 8) | (b)))
#define RGBA_TORGB(rgba)       ((D3DCOLOR) ((rgba) & 0xffffff))
#define RGB_TORGBA(rgb)        ((D3DCOLOR) ((rgb) | 0xff000000))

#endif

/*
 * Transformed/lit vertices
 */
typedef struct _D3DTLVERTEX {
    union {
    D3DVALUE    sx;             /* Screen coordinates */
    D3DVALUE    dvSX;
    };
    union {
    D3DVALUE    sy;
    D3DVALUE    dvSY;
    };
    union {
    D3DVALUE    sz;
    D3DVALUE    dvSZ;
    };
    union {
    D3DVALUE    rhw;        /* Reciprocal of homogeneous w */
    D3DVALUE    dvRHW;
    };
    union {
    D3DCOLOR    color;          /* Vertex color */
    D3DCOLOR    dcColor;
    };
    union {
    D3DCOLOR    specular;       /* Specular component of vertex */
    D3DCOLOR    dcSpecular;
    };
    union {
    D3DVALUE    tu;             /* Texture coordinates */
    D3DVALUE    dvTU;
    };
    union {
    D3DVALUE    tv;
    D3DVALUE    dvTV;
    };
#if (defined __cplusplus) && (defined D3D_OVERLOADS)
    _D3DTLVERTEX() { }
    _D3DTLVERTEX(const D3DVECTOR& v, float _rhw,
                 D3DCOLOR _color, D3DCOLOR _specular,
                 float _tu, float _tv)
        { sx = v.x; sy = v.y; sz = v.z; rhw = _rhw;
          color = _color; specular = _specular;
          tu = _tu; tv = _tv;
        }
#endif
} D3DTLVERTEX, *LPD3DTLVERTEX;

/*
 * Untransformed/lit vertices
 */
typedef struct _D3DLVERTEX {
    union {
    D3DVALUE     x;             /* Homogeneous coordinates */
    D3DVALUE     dvX;
    };
    union {
    D3DVALUE     y;
    D3DVALUE     dvY;
    };
    union {
    D3DVALUE     z;
    D3DVALUE     dvZ;
    };
    DWORD            dwReserved;
    union {
    D3DCOLOR     color;         /* Vertex color */
    D3DCOLOR     dcColor;
    };
    union {
    D3DCOLOR     specular;      /* Specular component of vertex */
    D3DCOLOR     dcSpecular;
    };
    union {
    D3DVALUE     tu;            /* Texture coordinates */
    D3DVALUE     dvTU;
    };
    union {
    D3DVALUE     tv;
    D3DVALUE     dvTV;
    };
#if (defined __cplusplus) && (defined D3D_OVERLOADS)
    _D3DLVERTEX() { }
    _D3DLVERTEX(const D3DVECTOR& v,
                D3DCOLOR _color, D3DCOLOR _specular,
                float _tu, float _tv)
        { x = v.x; y = v.y; z = v.z; dwReserved = 0;
          color = _color; specular = _specular;
          tu = _tu; tv = _tv;
        }
#endif
} D3DLVERTEX, *LPD3DLVERTEX;

/*
 * Untransformed/unlit vertices
 */

typedef struct _D3DVERTEX {
    union {
    D3DVALUE     x;             /* Homogeneous coordinates */
    D3DVALUE     dvX;
    };
    union {
    D3DVALUE     y;
    D3DVALUE     dvY;
    };
    union {
    D3DVALUE     z;
    D3DVALUE     dvZ;
    };
    union {
    D3DVALUE     nx;            /* Normal */
    D3DVALUE     dvNX;
    };
    union {
    D3DVALUE     ny;
    D3DVALUE     dvNY;
    };
    union {
    D3DVALUE     nz;
    D3DVALUE     dvNZ;
    };
    union {
    D3DVALUE     tu;            /* Texture coordinates */
    D3DVALUE     dvTU;
    };
    union {
    D3DVALUE     tv;
    D3DVALUE     dvTV;
    };
#if (defined __cplusplus) && (defined D3D_OVERLOADS)
    _D3DVERTEX() { }
    _D3DVERTEX(const D3DVECTOR& v, const D3DVECTOR& n, float _tu, float _tv)
        { x = v.x; y = v.y; z = v.z;
          nx = n.x; ny = n.y; nz = n.z;
          tu = _tu; tv = _tv;
        }
#endif
} D3DVERTEX, *LPD3DVERTEX;

#define D3DFVF_VERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )
#define D3DFVF_LVERTEX ( D3DFVF_XYZ | D3DFVF_RESERVED1 | D3DFVF_DIFFUSE | \
                         D3DFVF_SPECULAR | D3DFVF_TEX1 )
#define D3DFVF_TLVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | \
                          D3DFVF_TEX1 )

#endif // __TYPES_H__