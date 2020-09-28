// ============================================================================
//
//  RectSurf.cpp
//  Written by robald.
//
// ============================================================================

#include "HOSurf.h"
#include "types.h"

float g_RectTess[][4] = {
	{ 1.f, 1.f, 1.f, 1.f },
    { 5.f, 5.f, 5.f, 5.f },
    { 2.f, 4.f, 6.f, 8.f },
    { 9.1f, 2.5f, 7.75f, 5.3f },
	{ 15.f, 15.f, 15.f, 15.f}
};
int g_nRectTess = sizeof(g_RectTess) / (sizeof(float) * 4);

HOTEST g_RectTests[] = {
	  { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,  2,  2,  0, false, TEX_COORD1,  ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,    5.0f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,  2,  2,  0, false, TEX_COORD0, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,    5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,  2,  2,  0, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,    5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,  2,  2,  1, false, TEX_NONE, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS,  5.f, 0 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,  2,  2,  2, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,     5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,  4,  4,  3, false, TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,     5.5f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,  4,  4,  5, false, TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_CONTINUOUS,  5.5f, 0 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,  4,  4,  0, true,  TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,     5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,  4,  4,  1, true,  TEX_COORD1, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   4,  4,  2, false, TEX_COORD0, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   4,  4,  3, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   4,  4,  5, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.5f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   4,  4,  0, true,  TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE,   5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   4,  4,  1, true,  TEX_COORD0, ONEPATCH, WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   4,  4,  1, true,  TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   7,  7,  2, false, TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   7,  7,  3, false, TEX_NONE, NPATCH,   PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 0 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   7,  7,  5, true,  TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   7,  7,  1, true,  TEX_COORD0, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   7,  7,  1, true,  TEX_COORD1, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   7,  7,  1, true,  TEX_COORD1, NPATCH,   WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 6,  6,  2, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 6,  6,  3, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 0 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 6,  6,  5, true,  TEX_COORD0, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.2f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 6,  6,  5, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.2f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 6,  6,  1, true,  TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 6,  6,  1, true,  TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,11, 11,  2, false, TEX_COORD0, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,11, 11,  3, false, TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,11, 11,  5, true,  TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   2.9f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,11, 11,  1, true,  TEX_COORD1, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,11, 11,  1, true,  TEX_COORD0, NPATCH,   WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 2 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR,  2,  2,  2, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR,  2,  2,  3, false, TEX_NONE, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 0 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR,  2,  2,  5, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   9.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR,  3,  3,  0, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 0 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR,  3,  3,  1, false, TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR,  3,  3,  2, false, TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 0 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR,  3,  3,  3, true,  TEX_COORD0, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR,  3,  3,  3, true,  TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR,  3,  3,  5, true,  TEX_COORD1, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   6.55f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR, 10, 10,  0, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR, 10, 10,  1, false, TEX_NONE, NPATCH,   PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 0 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR, 10, 10,  2, true,  TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR, 10, 10,  1, true,  TEX_COORD0, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR, 10, 10,  1, true,  TEX_COORD1, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
	  { D3DBASIS_BSPLINE,     D3DDEGREE_LINEAR, 10, 10,  5, true,  TEX_COORD1, NPATCH,   WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   4,  4,  0, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   4,  4,  1, false, TEX_NONE, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 0 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   4,  4,  2, false, TEX_COORD0, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   4,  4,  2, false, TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   4,  4,  3, true,  TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   4,  4,  5, true,  TEX_COORD1, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   5.f, 1 },
	  { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   4,  4,  1, true,  TEX_COORD0, NPATCH,   WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 2 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   4,  4,  1, true,  TEX_COORD1, NPATCH,   WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 2 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   7,  7,  1, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   7,  7,  2, false, TEX_NONE, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,  5.f, 0 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   7,  7,  3, true,  TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
	  { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   7,  7,  5, true,  TEX_COORD1, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
	  { D3DBASIS_BSPLINE,     D3DDEGREE_CUBIC,   7,  7,  1, true,  TEX_COORD1, NPATCH,   WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_QUINTIC, 6,  6,  1, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_QUINTIC, 6,  6,  2, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 0 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_QUINTIC, 6,  6,  3, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_QUINTIC, 6,  6,  5, true,  TEX_COORD0, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_QUINTIC,11, 11,  0, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_QUINTIC,11, 11,  4, false, TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
	  { D3DBASIS_BSPLINE,     D3DDEGREE_QUINTIC,11, 11,  5, false, TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_CONTINUOUS, 12.f, 2 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_QUINTIC, 8,  8,  3, false, TEX_NONE, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,  5.f, 0 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_QUINTIC, 8,  8,  5, true,  TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_QUINTIC, 8,  8,  1, true,  TEX_COORD0, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_BSPLINE,     D3DDEGREE_QUINTIC, 8,  8,  1, true,  TEX_COORD1, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
	  { D3DBASIS_BSPLINE,     D3DDEGREE_QUINTIC, 8,  8,  1, true,  TEX_COORD1, NPATCH,   WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   4,  4,  2, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   4,  4,  3, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 0 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   4,  4,  5, false, TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   4,  4,  0, true,  TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   4,  4,  1, true,  TEX_COORD1, NPATCH,   WART  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
	  { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   4,  4,  1, true,  TEX_COORD1, NPATCH,   WAVE  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   4,  4,  1, true,  TEX_COORD0, NPATCH,   WAVE  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   5,  5,  3, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   5,  5,  5, false, TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 0 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   5,  5,  0, true,  TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   5,  5,  1, true,  TEX_COORD1, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
	  { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   5,  5,  1, true,  TEX_COORD1, NPATCH,   WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
	  { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   7,  7,  3, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   7,  7,  5, false, TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   7,  7,  0, true,  TEX_COORD0, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   7,  7,  0, true,  TEX_COORD1, NPATCH,   PLANAR, D3DPATCHEDGE_DISCRETE,   5.f, 2 },
      { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   7,  7,  1, true,  TEX_COORD1, NPATCH,   WART  , D3DPATCHEDGE_DISCRETE,   5.f, 2 },
	  { D3DBASIS_CATMULL_ROM, D3DDEGREE_CUBIC,   7,  7,  1, true,  TEX_COORD1, NPATCH,   WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },

	  { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,  2,  2,  1, true, TEX_COORD1,  ONEPATCH, DEGEN0, D3DPATCHEDGE_DISCRETE,    5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   4,  4,  1, true, TEX_COORD1,  ONEPATCH, DEGEN0, D3DPATCHEDGE_CONTINUOUS,  5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 6,  6,  1, true, TEX_COORD1,  ONEPATCH, DEGEN0, D3DPATCHEDGE_CONTINUOUS,  5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,  24,  1,  1, true, TEX_NONE,SEQUENTIAL, UTAHTEAPOT, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,  24,  1,  5, true, TEX_NONE,SEQUENTIAL, UTAHTEAPOT, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,  24,  1,  1, false, TEX_NONE,SEQUENTIAL, UTAHTEAPOT, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,  24,  1,  5, false, TEX_NONE,SEQUENTIAL, UTAHTEAPOT, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 }
};
int g_nRectTests = sizeof(g_RectTests) / sizeof(HOTEST);


const int nUtahGrid = 16;
const int nUtahPatches = 24;
const int nUtahPoints = nUtahPatches * nUtahGrid;
float UtahTeapot[nUtahPoints][3] =
{
	{ 1.40000f, 0.00000f, 2.40000f  },
	{ 1.40000f, -0.78400f, 2.40000f },
	{ 0.78400f, -1.40000f, 2.40000f },
	{ 0.00000f, -1.40000f, 2.40000f },	
	{ 1.33750f, 0.00000f, 2.53125f  },
	{ 1.33750f, -0.74900f, 2.53125f },
	{ 0.74900f, -1.33750f, 2.53125f },
	{ 0.00000f, -1.33750f, 2.53125f },
	{ 1.43750f, 0.00000f, 2.53125f  },
	{ 1.43750f, -0.80500f, 2.53125f },
	{ 0.80500f, -1.43750f, 2.53125f },
	{ 0.00000f, -1.43750f, 2.53125f },
	{ 1.50000f, 0.00000f, 2.40000f  },
	{ 1.50000f, -0.84000f, 2.40000f },
	{ 0.84000f, -1.50000f, 2.40000f },
	{ 0.00000f, -1.50000f, 2.40000f },

	{ 0.00000f, -1.40000f, 2.40000f  },
	{ -0.78400f, -1.40000f, 2.40000f },
	{ -1.40000f, -0.78400f, 2.40000f },
	{ -1.40000f, 0.00000f, 2.40000f  },
	{ 0.00000f, -1.33750f, 2.53125f  },
	{ -0.74900f, -1.33750f, 2.53125f },
	{ -1.33750f, -0.74900f, 2.53125f },
	{ -1.33750f, 0.00000f, 2.53125f  },
	{ 0.00000f, -1.43750f, 2.53125f  },
	{ -0.80500f, -1.43750f, 2.53125f },
	{ -1.43750f, -0.80500f, 2.53125f },
	{ -1.43750f, 0.00000f, 2.53125f  },
	{ 0.00000f, -1.50000f, 2.40000f  },
	{ -0.84000f, -1.50000f, 2.40000f },
	{ -1.50000f, -0.84000f, 2.40000f },
	{ -1.50000f, 0.00000f, 2.40000f  },

	{ -1.40000f, 0.00000f, 2.40000f },
	{ -1.40000f, 0.78400f, 2.40000f },
	{ -0.78400f, 1.40000f, 2.40000f },
	{ 0.00000f, 1.40000f, 2.40000f  },
	{ -1.33750f, 0.00000f, 2.53125f },
	{ -1.33750f, 0.74900f, 2.53125f },
	{ -0.74900f, 1.33750f, 2.53125f },
	{ 0.00000f, 1.33750f, 2.53125f  },
	{ -1.43750f, 0.00000f, 2.53125f },
	{ -1.43750f, 0.80500f, 2.53125f },
	{ -0.80500f, 1.43750f, 2.53125f },
	{ 0.00000f, 1.43750f, 2.53125f  },
	{ -1.50000f, 0.00000f, 2.40000f },
	{ -1.50000f, 0.84000f, 2.40000f },
	{ -0.84000f, 1.50000f, 2.40000f },
	{ 0.00000f, 1.50000f, 2.40000f  },
	
	{ 0.00000f, 1.40000f, 2.40000f },
	{ 0.78400f, 1.40000f, 2.40000f },
	{ 1.40000f, 0.78400f, 2.40000f },
	{ 1.40000f, 0.00000f, 2.40000f },
	{ 0.00000f, 1.33750f, 2.53125f },
	{ 0.74900f, 1.33750f, 2.53125f },
	{ 1.33750f, 0.74900f, 2.53125f },
	{ 1.33750f, 0.00000f, 2.53125f },
	{ 0.00000f, 1.43750f, 2.53125f },
	{ 0.80500f, 1.43750f, 2.53125f },
	{ 1.43750f, 0.80500f, 2.53125f },
	{ 1.43750f, 0.00000f, 2.53125f },
	{ 0.00000f, 1.50000f, 2.40000f },
	{ 0.84000f, 1.50000f, 2.40000f },
	{ 1.50000f, 0.84000f, 2.40000f },
	{ 1.50000f, 0.00000f, 2.40000f },
	
	{ 1.50000f, 0.00000f, 2.40000f },
	{ 1.50000f, -0.84000f, 2.40000f },
	{ 0.84000f, -1.50000f, 2.40000f },
	{ 0.00000f, -1.50000f, 2.40000f },
	{ 1.75000f, 0.00000f, 1.87500f },
	{ 1.75000f, -0.98000f, 1.87500f },
	{ 0.98000f, -1.75000f, 1.87500f },
	{ 0.00000f, -1.75000f, 1.87500f },
	{ 2.00000f, 0.00000f, 1.35000f },
	{ 2.00000f, -1.12000f, 1.35000f },
	{ 1.12000f, -2.00000f, 1.35000f },
	{ 0.00000f, -2.00000f, 1.35000f },
	{ 2.00000f, 0.00000f, 0.90000f },
	{ 2.00000f, -1.12000f, 0.90000f },
	{ 1.12000f, -2.00000f, 0.90000f },
	{ 0.00000f, -2.00000f, 0.90000f },
	
	{ 0.00000f, -1.50000f, 2.40000f },
	{ -0.84000f, -1.50000f, 2.40000f },
	{ -1.50000f, -0.84000f, 2.40000f },
	{ -1.50000f, 0.00000f, 2.40000f },
	{ 0.00000f, -1.75000f, 1.87500f },
	{ -0.98000f, -1.75000f, 1.87500f },
	{ -1.75000f, -0.98000f, 1.87500f },
	{ -1.75000f, 0.00000f, 1.87500f },
	{ 0.00000f, -2.00000f, 1.35000f },
	{ -1.12000f, -2.00000f, 1.35000f },
	{ -2.00000f, -1.12000f, 1.35000f },
	{ -2.00000f, 0.00000f, 1.35000f },
	{ 0.00000f, -2.00000f, 0.90000f },
	{ -1.12000f, -2.00000f, 0.90000f },
	{ -2.00000f, -1.12000f, 0.90000f },
	{ -2.00000f, 0.00000f, 0.90000f },
	
	{ -1.50000f, 0.00000f, 2.40000f },
	{ -1.50000f, 0.84000f, 2.40000f },
	{ -0.84000f, 1.50000f, 2.40000f },
	{ 0.00000f, 1.50000f, 2.40000f },
	{ -1.75000f, 0.00000f, 1.87500f },
	{ -1.75000f, 0.98000f, 1.87500f },
	{ -0.98000f, 1.75000f, 1.87500f },
	{ 0.00000f, 1.75000f, 1.87500f },
	{ -2.00000f, 0.00000f, 1.35000f },
	{ -2.00000f, 1.12000f, 1.35000f },
	{ -1.12000f, 2.00000f, 1.35000f },
	{ 0.00000f, 2.00000f, 1.35000f },
	{ -2.00000f, 0.00000f, 0.90000f },
	{ -2.00000f, 1.12000f, 0.90000f },
	{ -1.12000f, 2.00000f, 0.90000f },
	{ 0.00000f, 2.00000f, 0.90000f },
	
	{ 0.00000f, 1.50000f, 2.40000f },
	{ 0.84000f, 1.50000f, 2.40000f },
	{ 1.50000f, 0.84000f, 2.40000f },
	{ 1.50000f, 0.00000f, 2.40000f },
	{ 0.00000f, 1.75000f, 1.87500f },
	{ 0.98000f, 1.75000f, 1.87500f },
	{ 1.75000f, 0.98000f, 1.87500f },
	{ 1.75000f, 0.00000f, 1.87500f },
	{ 0.00000f, 2.00000f, 1.35000f },
	{ 1.12000f, 2.00000f, 1.35000f },
	{ 2.00000f, 1.12000f, 1.35000f },
	{ 2.00000f, 0.00000f, 1.35000f },
	{ 0.00000f, 2.00000f, 0.90000f },
	{ 1.12000f, 2.00000f, 0.90000f },
	{ 2.00000f, 1.12000f, 0.90000f },
	{ 2.00000f, 0.00000f, 0.90000f },
	
	{ 2.00000f, 0.00000f, 0.90000f },
	{ 2.00000f, -1.12000f, 0.90000f },
	{ 1.12000f, -2.00000f, 0.90000f },
	{ 0.00000f, -2.00000f, 0.90000f },
	{ 2.00000f, 0.00000f, 0.45000f },
	{ 2.00000f, -1.12000f, 0.45000f },
	{ 1.12000f, -2.00000f, 0.45000f },
	{ 0.00000f, -2.00000f, 0.45000f },
	{ 1.50000f, 0.00000f, 0.22500f },
	{ 1.50000f, -0.84000f, 0.22500f },
	{ 0.84000f, -1.50000f, 0.22500f },
	{ 0.00000f, -1.50000f, 0.22500f },
	{ 1.50000f, 0.00000f, 0.15000f },
	{ 1.50000f, -0.84000f, 0.15000f },
	{ 0.84000f, -1.50000f, 0.15000f },
	{ 0.00000f, -1.50000f, 0.15000f },
	
	{ 0.00000f, -2.00000f, 0.90000f },
	{ -1.12000f, -2.00000f, 0.90000f },
	{ -2.00000f, -1.12000f, 0.90000f },
	{ -2.00000f, 0.00000f, 0.90000f },
	{ 0.00000f, -2.00000f, 0.45000f },
	{ -1.12000f, -2.00000f, 0.45000f },
	{ -2.00000f, -1.12000f, 0.45000f },
	{ -2.00000f, 0.00000f, 0.45000f },
	{ 0.00000f, -1.50000f, 0.22500f },
	{ -0.84000f, -1.50000f, 0.22500f },
	{ -1.50000f, -0.84000f, 0.22500f },
	{ -1.50000f, 0.00000f, 0.22500f },
	{ 0.00000f, -1.50000f, 0.15000f },
	{ -0.84000f, -1.50000f, 0.15000f },
	{ -1.50000f, -0.84000f, 0.15000f },
	{ -1.50000f, 0.00000f, 0.15000f },
	
	{ -2.00000f, 0.00000f, 0.90000f },
	{ -2.00000f, 1.12000f, 0.90000f },
	{ -1.12000f, 2.00000f, 0.90000f },
	{ 0.00000f, 2.00000f, 0.90000f },
	{ -2.00000f, 0.00000f, 0.45000f },
	{ -2.00000f, 1.12000f, 0.45000f },
	{ -1.12000f, 2.00000f, 0.45000f },
	{ 0.00000f, 2.00000f, 0.45000f },
	{ -1.50000f, 0.00000f, 0.22500f },
	{ -1.50000f, 0.84000f, 0.22500f },
	{ -0.84000f, 1.50000f, 0.22500f },
	{ 0.00000f, 1.50000f, 0.22500f },
	{ -1.50000f, 0.00000f, 0.15000f },
	{ -1.50000f, 0.84000f, 0.15000f },
	{ -0.84000f, 1.50000f, 0.15000f },
	{ 0.00000f, 1.50000f, 0.15000f },
	
	{ 0.00000f, 2.00000f, 0.90000f },
	{ 1.12000f, 2.00000f, 0.90000f },
	{ 2.00000f, 1.12000f, 0.90000f },
	{ 2.00000f, 0.00000f, 0.90000f },
	{ 0.00000f, 2.00000f, 0.45000f },
	{ 1.12000f, 2.00000f, 0.45000f },
	{ 2.00000f, 1.20000f, 0.45000f },
	{ 2.00000f, 0.00000f, 0.45000f },
	{ 0.00000f, 1.50000f, 0.22500f },
	{ 0.84000f, 1.50000f, 0.22500f },
	{ 1.50000f, 0.84000f, 0.22500f },
	{ 1.50000f, 0.00000f, 0.22500f },
	{ 0.00000f, 1.50000f, 0.15000f },
	{ 0.84000f, 1.50000f, 0.15000f },
	{ 1.50000f, 0.84000f, 0.15000f },
	{ 1.50000f, 0.00000f, 0.15000f },
	
	{ -1.60000f, 0.00000f, 2.02500f },
	{ -1.60000f, -0.30000f, 2.02500f },
	{ -1.50000f, -0.30000f, 2.25000f },
	{ -1.50000f, 0.00000f, 2.25000f },
	{ -2.30000f, 0.00000f, 2.02500f },
	{ -2.30000f, -0.30000f, 2.02500f },
	{ -2.50000f, -0.30000f, 2.25000f },
	{ -2.50000f, 0.00000f, 2.25000f },
	{ -2.70000f, 0.00000f, 2.02500f },
	{ -2.70000f, -0.30000f, 2.02500f },
	{ -3.00000f, -0.30000f, 2.25000f },
	{ -3.00000f, 0.00000f, 2.25000f },
	{ -2.70000f, 0.00000f, 1.80000f },
	{ -2.70000f, -0.30000f, 1.80000f },
	{ -3.00000f, -0.30000f, 1.80000f },
	{ -3.00000f, 0.00000f, 1.80000f },
	
	{ -1.50000f, 0.00000f, 2.25000f },
	{ -1.50000f, 0.30000f, 2.25000f },
	{ -1.60000f, 0.30000f, 2.02500f },
	{ -1.60000f, 0.00000f, 2.02500f },
	{ -2.50000f, 0.00000f, 2.25000f },
	{ -2.50000f, 0.30000f, 2.25000f },
	{ -2.30000f, 0.30000f, 2.02500f },
	{ -2.30000f, 0.00000f, 2.02500f },
	{ -3.00000f, 0.00000f, 2.25000f },
	{ -3.00000f, 0.30000f, 2.25000f },
	{ -2.70000f, 0.30000f, 2.02500f },
	{ -2.70000f, 0.00000f, 2.02500f },
	{ -3.00000f, 0.00000f, 1.80000f },
	{ -3.00000f, 0.30000f, 1.80000f },
	{ -2.70000f, 0.30000f, 1.80000f },
	{ -2.70000f, 0.00000f, 1.80000f },

	{ -2.70000f, 0.00000f, 1.80000f },
	{ -2.70000f, -0.30000f, 1.80000f },
	{ -3.00000f, -0.30000f, 1.80000f },
	{ -3.00000f, 0.00000f, 1.80000f },
	{ -2.70000f, 0.00000f, 1.57500f },
	{ -2.70000f, -0.30000f, 1.57500f },
	{ -3.00000f, -0.30000f, 1.35000f },
	{ -3.00000f, 0.00000f, 1.35000f },
	{ -2.50000f, 0.00000f, 1.12500f },
	{ -2.50000f, -0.30000f, 1.12500f },
	{ -2.65000f, -0.30000f, 0.93750f },
	{ -2.65000f, 0.00000f, 0.93750f },
	{ -2.00000f, 0.00000f, 0.90000f },
	{ -2.00000f, -0.30000f, 0.90000f },
	{ -1.90000f, -0.30000f, 0.60000f },
	{ -1.90000f, 0.00000f, 0.60000f },
	
	{ -3.00000f, 0.00000f, 1.80000f },
	{ -3.00000f, 0.30000f, 1.80000f },
	{ -2.70000f, 0.30000f, 1.80000f },
	{ -2.70000f, 0.00000f, 1.80000f },
	{ -3.00000f, 0.00000f, 1.35000f },
	{ -3.00000f, 0.30000f, 1.35000f },
	{ -2.70000f, 0.30000f, 1.57500f },
	{ -2.70000f, 0.00000f, 1.57500f },
	{ -2.65000f, 0.00000f, 0.93750f },
	{ -2.65000f, 0.30000f, 0.93750f },
	{ -2.50000f, 0.30000f, 1.12500f },
	{ -2.50000f, 0.00000f, 1.12500f },
	{ -1.90000f, 0.00000f, 0.60000f },
	{ -1.90000f, 0.30000f, 0.60000f },
	{ -2.00000f, 0.30000f, 0.90000f },
	{ -2.00000f, 0.00000f, 0.90000f },
	
	{ 1.70000f, 0.00000f, 1.42500f },
	{ 1.70000f, -0.66000f, 1.42500f },
	{ 1.70000f, -0.66000f, 0.60000f },
	{ 1.70000f, 0.00000f, 0.60000f },
	{ 2.60000f, 0.00000f, 1.42500f },
	{ 2.60000f, -0.66000f, 1.42500f },
	{ 3.10000f, -0.66000f, 0.82500f },
	{ 3.10000f, 0.00000f, 0.82500f },
	{ 2.30000f, 0.00000f, 2.10000f },
	{ 2.30000f, -0.25000f, 2.10000f },
	{ 2.40000f, -0.25000f, 2.02500f },
	{ 2.40000f, 0.00000f, 2.02500f },
	{ 2.70000f, 0.00000f, 2.40000f },
	{ 2.70000f, -0.25000f, 2.40000f },
	{ 3.30000f, -0.25000f, 2.40000f },
	{ 3.30000f, 0.00000f, 2.40000f },
	
	{ 1.70000f, 0.00000f, 0.60000f },
	{ 1.70000f, 0.66000f, 0.60000f },
	{ 1.70000f, 0.66000f, 1.42500f },
	{ 1.70000f, 0.00000f, 1.42500f },
	{ 3.10000f, 0.00000f, 0.82500f },
	{ 3.10000f, 0.66000f, 0.82500f },
	{ 2.60000f, 0.66000f, 1.42500f },
	{ 2.60000f, 0.00000f, 1.42500f },
	{ 2.40000f, 0.00000f, 2.02500f },
	{ 2.40000f, 0.25000f, 2.02500f },
	{ 2.30000f, 0.25000f, 2.10000f },
	{ 2.30000f, 0.00000f, 2.10000f },
	{ 3.30000f, 0.00000f, 2.40000f },
	{ 3.30000f, 0.25000f, 2.40000f },
	{ 2.70000f, 0.25000f, 2.40000f },
	{ 2.70000f, 0.00000f, 2.40000f },
	
	{ 2.70000f, 0.00000f, 2.40000f },
	{ 2.70000f, -0.25000f, 2.40000f },
	{ 3.30000f, -0.25000f, 2.40000f },
	{ 3.30000f, 0.00000f, 2.40000f },
	{ 2.80000f, 0.00000f, 2.47500f },
	{ 2.80000f, -0.25000f, 2.47500f },
	{ 3.52500f, -0.25000f, 2.49375f },
	{ 3.52500f, 0.00000f, 2.49375f },
	{ 2.90000f, 0.00000f, 2.47500f },
	{ 2.90000f, -0.15000f, 2.47500f },
	{ 3.45000f, -0.15000f, 2.51250f },
	{ 3.45000f, 0.00000f, 2.51250f },
	{ 2.80000f, 0.00000f, 2.40000f },
	{ 2.80000f, -0.15000f, 2.40000f },
	{ 3.20000f, 0.00000f, 2.40000f },
	{ 3.20000f, 0.00000f, 2.40000f },
	
	{ 3.30000f, 0.00000f, 2.40000f },
	{ 3.30000f, 0.25000f, 2.40000f },
	{ 2.70000f, 0.25000f, 2.40000f },
	{ 2.70000f, 0.00000f, 2.40000f },
	{ 3.52500f, 0.00000f, 2.49375f },
	{ 3.52500f, 0.25000f, 2.49375f },
	{ 2.80000f, 0.25000f, 2.47500f },
	{ 2.80000f, 0.00000f, 2.47500f },
	{ 3.45000f, 0.00000f, 2.51250f },
	{ 3.45000f, 0.15000f, 2.51250f },
	{ 2.90000f, 0.15000f, 2.47500f },
	{ 2.90000f, 0.00000f, 2.47500f },
	{ 3.20000f, 0.00000f, 2.40000f },
	{ 3.20000f, 0.15000f, 2.40000f },
	{ 2.80000f, 0.15000f, 2.40000f },
	{ 2.80000f, 0.00000f, 2.40000f },
/*	
	{ 0.00000f, 0.00000f, 3.15000f },
	{ 0.00000f, 0.00000f, 3.15000f },
	{ 0.00000f, 0.00000f, 3.15000f },
	{ 0.00000f, 0.00000f, 3.15000f },
	{ 0.80000f, 0.00000f, 3.15000f },
	{ 0.80000f, -0.45000f, 3.15000f },
	{ 0.45000f, -0.80000f, 3.15000f },
	{ 0.00000f, -0.80000f, 3.15000f },
	{ 0.00000f, 0.00000f, 2.85000f },
	{ 0.00000f, 0.00000f, 2.85000f },
	{ 0.00000f, 0.00000f, 2.85000f },
	{ 0.00000f, 0.00000f, 2.85000f },
	{ 0.20000f, 0.00000f, 2.70000f },
	{ 0.20000f, -0.11200f, 2.70000f },
	{ 0.11200f, -0.20000f, 2.70000f },
	{ 0.00000f, -0.20000f, 2.70000f },
	
	{ 0.00000f, 0.00000f, 3.15000f  },
	{ 0.00000f, 0.00000f, 3.15000f  },
	{ 0.00000f, 0.00000f, 3.15000f  },
	{ 0.00000f, 0.00000f, 3.15000f  },
	{ 0.00000f, -0.80000f, 3.15000f  },
	{ -0.45000f, -0.80000f, 3.15000f  },
	{ -0.80000f, -0.45000f, 3.15000f  },
	{ -0.80000f, 0.00000f, 3.15000f  },
	{ 0.00000f, 0.00000f, 2.85000f  },
	{ 0.00000f, 0.00000f, 2.85000f  },
	{ 0.00000f, 0.00000f, 2.85000f  },
	{ 0.00000f, 0.00000f, 2.85000f  },
	{ 0.00000f, -0.20000f, 2.70000f  },
	{ -0.11200f, -0.20000f, 2.70000f  },
	{ -0.20000f, -0.11200f, 2.70000f  },
	{ -0.20000f, 0.00000f, 2.70000f  },
	
	{ 0.00000f, 0.00000f, 3.15000f },
	{ 0.00000f, 0.00000f, 3.15000f },
	{ 0.00000f, 0.00000f, 3.15000f },
	{ 0.00000f, 0.00000f, 3.15000f },
	{ -0.80000f, 0.00000f, 3.15000f },
	{ -0.80000f, 0.45000f, 3.15000f },
	{ -0.45000f, 0.80000f, 3.15000f },
	{ 0.00000f, 0.80000f, 3.15000f },
	{ 0.00000f, 0.00000f, 2.85000f },
	{ 0.00000f, 0.00000f, 2.85000f },
	{ 0.00000f, 0.00000f, 2.85000f },
	{ 0.00000f, 0.00000f, 2.85000f },
	{ -0.20000f, 0.00000f, 2.70000f },
	{ -0.20000f, 0.11200f, 2.70000f },
	{ -0.11200f, 0.20000f, 2.70000f },
	{ 0.00000f, 0.20000f, 2.70000f },
	
	{ 0.00000f, 0.00000f, 3.15000f },
	{ 0.00000f, 0.00000f, 3.15000f },
	{ 0.00000f, 0.00000f, 3.15000f },
	{ 0.00000f, 0.00000f, 3.15000f },
	{ 0.00000f, 0.80000f, 3.15000f },
	{ 0.45000f, 0.80000f, 3.15000f },
	{ 0.80000f, 0.45000f, 3.15000f },
	{ 0.80000f, 0.00000f, 3.15000f },
	{ 0.00000f, 0.00000f, 2.85000f },
	{ 0.00000f, 0.00000f, 2.85000f },
	{ 0.00000f, 0.00000f, 2.85000f },
	{ 0.00000f, 0.00000f, 2.85000f },
	{ 0.00000f, 0.20000f, 2.70000f },
	{ 0.11200f, 0.20000f, 2.70000f },
	{ 0.20000f, 0.11200f, 2.70000f },
	{ 0.20000f, 0.00000f, 2.70000f },
*/	
	{ 0.20000f, 0.00000f, 2.70000f },
	{ 0.20000f, -0.11200f, 2.70000f },
	{ 0.11200f, -0.20000f, 2.70000f },
	{ 0.00000f, -0.20000f, 2.70000f },
	{ 0.40000f, 0.00000f, 2.55000f },
	{ 0.40000f, -0.22400f, 2.55000f },
	{ 0.22400f, -0.40000f, 2.55000f },
	{ 0.00000f, -0.40000f, 2.55000f },
	{ 1.30000f, 0.00000f, 2.55000f },
	{ 1.30000f, -0.72800f, 2.55000f },
	{ 0.72800f, -1.30000f, 2.55000f },
	{ 0.00000f, -1.30000f, 2.55000f },
	{ 1.30000f, 0.00000f, 2.40000f },
	{ 1.30000f, -0.72800f, 2.40000f },
	{ 0.72800f, -1.30000f, 2.40000f },
	{ 0.00000f, -1.30000f, 2.40000f },
	
	{ 0.00000f, -0.20000f, 2.70000f },
	{ -0.11200f, -0.20000f, 2.70000f },
	{ -0.20000f, -0.11200f, 2.70000f },
	{ -0.20000f, 0.00000f, 2.70000f },
	{ 0.00000f, -0.40000f, 2.55000f },
	{ -0.22400f, -0.40000f, 2.55000f },
	{ -0.40000f, -0.22400f, 2.55000f },
	{ -0.40000f, 0.00000f, 2.55000f },
	{ 0.00000f, -1.30000f, 2.55000f },
	{ -0.72800f, -1.30000f, 2.55000f },
	{ -1.30000f, -0.72800f, 2.55000f },
	{ -1.30000f, 0.00000f, 2.55000f },
	{ 0.00000f, -1.30000f, 2.40000f },
	{ -0.72800f, -1.30000f, 2.40000f },
	{ -1.30000f, -0.72800f, 2.40000f },
	{ -1.30000f, 0.00000f, 2.40000f },
	
	{ -0.20000f, 0.00000f, 2.70000f },
	{ -0.20000f, 0.11200f, 2.70000f },
	{ -0.11200f, 0.20000f, 2.70000f },
	{ 0.00000f, 0.20000f, 2.70000f },
	{ -0.40000f, 0.00000f, 2.55000f },
	{ -0.40000f, 0.22400f, 2.55000f },
	{ -0.22400f, 0.40000f, 2.55000f },
	{ 0.00000f, 0.40000f, 2.55000f },
	{ -1.30000f, 0.00000f, 2.55000f },
	{ -1.30000f, 0.72800f, 2.55000f },
	{ -0.72800f, 1.30000f, 2.55000f },
	{ 0.00000f, 1.30000f, 2.55000f },
	{ -1.30000f, 0.00000f, 2.40000f },
	{ -1.30000f, 0.72800f, 2.40000f },
	{ -0.72800f, 1.30000f, 2.40000f },
	{ 0.00000f, 1.30000f, 2.40000f },
	
	{ 0.00000f, 0.20000f, 2.70000f },
	{ 0.11200f, 0.20000f, 2.70000f },
	{ 0.20000f, 0.11200f, 2.70000f },
	{ 0.20000f, 0.00000f, 2.70000f },
	{ 0.00000f, 0.40000f, 2.55000f },
	{ 0.22400f, 0.40000f, 2.55000f },
	{ 0.40000f, 0.22400f, 2.55000f },
	{ 0.40000f, 0.00000f, 2.55000f },
	{ 0.00000f, 1.30000f, 2.55000f },
	{ 0.72800f, 1.30000f, 2.55000f },
	{ 1.30000f, 0.72800f, 2.55000f },
	{ 1.30000f, 0.00000f, 2.55000f },
	{ 0.00000f, 1.30000f, 2.40000f },
	{ 0.72800f, 1.30000f, 2.40000f },
	{ 1.30000f, 0.72800f, 2.40000f },
	{ 1.30000f, 0.00000f, 2.40000f },
/*	
	{ 0.00000f, 0.00000f, 0.00000f },
	{ 0.00000f, 0.00000f, 0.00000f },
	{ 0.00000f, 0.00000f, 0.00000f },
	{ 0.00000f, 0.00000f, 0.00000f },
	{ 1.42500f, 0.00000f, 0.00000f },
	{ 1.42500f, 0.79800f, 0.00000f },
	{ 0.79800f, 1.42500f, 0.00000f },
	{ 0.00000f, 1.42500f, 0.00000f },
	{ 1.50000f, 0.00000f, 0.07500f },
	{ 1.50000f, 0.84000f, 0.07500f },
	{ 0.84000f, 1.50000f, 0.07500f },
	{ 0.00000f, 1.50000f, 0.07500f },
	{ 1.50000f, 0.00000f, 0.15000f },
	{ 1.50000f, 0.84000f, 0.15000f },
	{ 0.84000f, 1.50000f, 0.15000f },
	{ 0.00000f, 1.50000f, 0.15000f },
	
	{ 0.00000f, 0.00000f, 0.00000f },
	{ 0.00000f, 0.00000f, 0.00000f },
	{ 0.00000f, 0.00000f, 0.00000f },
	{ 0.00000f, 0.00000f, 0.00000f },
	{ 0.00000f, 1.42500f, 0.00000f },
	{ -0.79800f, 1.42500f, 0.00000f },
	{ -1.42500f, 0.79800f, 0.00000f },
	{ -1.42500f, 0.00000f, 0.00000f },
	{ 0.00000f, 1.50000f, 0.07500f },
	{ -0.84000f, 1.50000f, 0.07500f },
	{ -1.50000f, 0.84000f, 0.07500f },
	{ -1.50000f, 0.00000f, 0.07500f },
	{ 0.00000f, 1.50000f, 0.15000f },
	{ -0.84000f, 1.50000f, 0.15000f },
	{ -1.50000f, 0.84000f, 0.15000f },
	{ -1.50000f, 0.00000f, 0.15000f },
	
	{ 0.00000f, 0.00000f, 0.00000f },
	{ 0.00000f, 0.00000f, 0.00000f },
	{ 0.00000f, 0.00000f, 0.00000f },
	{ 0.00000f, 0.00000f, 0.00000f },
	{ -1.42500f, 0.00000f, 0.00000f },
	{ -1.42500f, -0.79800f, 0.00000f },
	{ -0.79800f, -1.42500f, 0.00000f },
	{ 0.00000f, -1.42500f, 0.00000f },
	{ -1.50000f, 0.00000f, 0.07500f },
	{ -1.50000f, -0.84000f, 0.07500f },
	{ -0.84000f, -1.50000f, 0.07500f },
	{ 0.00000f, -1.50000f, 0.07500f },
	{ -1.50000f, 0.00000f, 0.15000f },
	{ -1.50000f, -0.84000f, 0.15000f },
	{ -0.84000f, -1.50000f, 0.15000f },
	{ 0.00000f, -1.50000f, 0.15000f },
	
	{ 0.00000f, 0.00000f, 0.00000f  },
	{ 0.00000f, 0.00000f, 0.00000f  },
	{ 0.00000f, 0.00000f, 0.00000f  },
	{ 0.00000f, 0.00000f, 0.00000f  },
	{ 0.00000f, -1.42500f, 0.00000f  },
	{ 0.79800f, -1.42500f, 0.00000f  },
	{ 1.42500f, -0.79800f, 0.00000f  },
	{ 1.42500f, 0.00000f, 0.00000f  },
	{ 0.00000f, -1.50000f, 0.07500f  },
	{ 0.84000f, -1.50000f, 0.07500f  },
	{ 1.50000f, -0.84000f, 0.07500f  },
	{ 1.50000f, 0.00000f, 0.07500f  },
	{ 0.00000f, -1.50000f, 0.15000f  },
	{ 0.84000f, -1.50000f, 0.15000f  },
	{ 1.50000f, -0.84000f, 0.15000f  },
	{ 1.50000f, 0.00000f, 0.15000f  }
*/
};

// ----------------------------------------------------------------------------
   
CRectSurf::CRectSurf()
{
    m_szTestName= _T("HOSurf - RectSurface tests");
	m_szCommandKey= _T("RECT");
    m_dwUsage = D3DUSAGE_RTPATCHES;
}

// ----------------------------------------------------------------------------

bool CRectSurf::CheckCaps()
{

    if(!(m_pSrcDevice->GetCaps()->dwDevCaps & D3DDEVCAPS_RTPATCHES))
	{
		WriteToLog(_T("CTriSurf::CheckCaps() - D3DDEVCAPS_RTPATCHES not set.\n"));
		return false;
	}

	if(m_bD3DXTessellation)
	{
		m_bQuintic = true;
	}
	else
	{
		m_bQuintic = m_pSrcDevice->GetCaps()->dwDevCaps & D3DDEVCAPS_QUINTICRTPATCHES ? true : false;
	}

	return true;
}

// ----------------------------------------------------------------------------

void CRectSurf::TweakGeometry(HOTEST *pTest, D3DVERTEX *pVertices)
{
	switch(pTest->PatchType)
	{
	case UTAHTEAPOT:
   		for(int i=0; i<nUtahPoints; i++)
		{
			pVertices[i].x = UtahTeapot[i][0];
			pVertices[i].y = UtahTeapot[i][1];
			pVertices[i].z = UtahTeapot[i][2];
		}
        break;
	default:
		CHOSurf::TweakGeometry(pTest, pVertices);
		break;
	}
}

// ----------------------------------------------------------------------------

bool CRectSurf::LoadGeometry()
{
    CTestShapes Shape;

	for(int nTest=0; nTest<g_nRectTests; nTest++)
	{
		// Generate the geometry
		Shape.NewShape(CS_MESH, g_RectTests[nTest].nWidth-1, g_RectTests[nTest].nHeight-1);
		Shape.Scale(2,2,2);
		TweakGeometry(&g_RectTests[nTest], Shape.m_pVertices);
        	
        if(g_RectTests[nTest].PatchType == UTAHTEAPOT)
		{
			Shape.Rotate(pi/2.f, 0.f, 0.f);
			Shape.Translate(0.f, -1.5f, 0.0f);
		}
		
        // Create the surface and buffers
		m_pSurfs[m_nSurfCount] = CreateHOSurf(&Shape, &(g_RectTests[nTest]), D3DFVF_VERTEX);
		if(m_pSurfs[m_nSurfCount])
		{
			// Set the info
			_stprintf(m_pSurfs[m_nSurfCount]->tcsSurf, _T("%dx%d %s Rect patch(s)"),
					  m_pSurfs[m_nSurfCount]->pTest->nWidth, m_pSurfs[m_nSurfCount]->pTest->nHeight,
					  m_pSurfs[m_nSurfCount]->pTest->DrawMode == ONEPATCH || m_pSurfs[m_nSurfCount]->pTest->DrawMode == SEQUENTIAL ? _T("Individual") : _T("Mosaic"));

			if(m_pSurfs[m_nSurfCount]->pTest->nShader == 0)
				_tcscpy(m_pSurfs[m_nSurfCount]->tcsShader, _T("Custom"));
			else if(m_pSurfs[m_nSurfCount]->pTest->nShader == 1)
				_tcscpy(m_pSurfs[m_nSurfCount]->tcsShader, _T("Fixed(generated TexCoords)"));
            else if(m_pSurfs[m_nSurfCount]->pTest->nShader == 2)
				_tcscpy(m_pSurfs[m_nSurfCount]->tcsShader, _T("Fixed(user-defined TexCoords)"));
			else
                _tcscpy(m_pSurfs[m_nSurfCount]->tcsShader, _T("Unknown"));

			// TODO: For D3D9 there is not D3DRS_PATCHSEGMENTS, so eliminate this branch
			if(m_pSurfs[m_nSurfCount]->pTest->nTessIndex >= g_nRectTess)
			{
					_stprintf(m_pSurfs[m_nSurfCount]->tcsTess, _T("%.2f set in D3DRS_PATCHSEGMENTS"), 
							  m_pSurfs[m_nSurfCount]->pTest->fRSTess);
			}
			else
			{
					_stprintf(  m_pSurfs[m_nSurfCount]->tcsTess, _T("%.2f,%.2f,%.2f,%.2f set in PATCH_INFO"), 
								g_RectTess[m_pSurfs[m_nSurfCount]->pTest->nTessIndex][0], 
								g_RectTess[m_pSurfs[m_nSurfCount]->pTest->nTessIndex][1],
								g_RectTess[m_pSurfs[m_nSurfCount]->pTest->nTessIndex][2],
								g_RectTess[m_pSurfs[m_nSurfCount]->pTest->nTessIndex][3]);
			}

			m_nSurfCount++; 
		}
	}
		
	return true;
}

UINT CRectSurf::TestInitialize(void)
{
	UINT uRes = CHOSurf::TestInitialize();

	if(uRes == D3DTESTINIT_RUN)
	{
		SkipTestRange(469,469);
		SkipTestRange(474,474);
	}
	
	return uRes;
}

// ----------------------------------------------------------------------------

bool CRectSurf::PreExecuteTest(UINT uTestNumber)
{
	if(!CHOSurf::PreExecuteTest(uTestNumber))
		return false;
	else
	{
		if(m_bD3DXTessellation)
		{
/*			if(m_pSurfs[m_nCurrentSurf]->pTest->HODegree == D3DDEGREE_LINEAR)		// we can't do linear right now, so fail.  Remove this when linear is implemented
			{
	            SkipTestRange(uTestNumber, uTestNumber+m_nViewMatrix-1);
				return false;
			}
*/		}
	}

	return true;		// continue with test
}

// ----------------------------------------------------------------------------

bool CRectSurf::DrawSurface(HOSURFACE *pSurf)
{
	HRESULT hr;
	bool bRes = true;
	int nCached = 0;
    D3DRECTPATCH_INFO RectInfo, *pSurfInfo;
    float *f = pSurf->pTest->nTessIndex >= g_nRectTess ? NULL : g_RectTess[pSurf->pTest->nTessIndex];

	DWORD dwDeviceFlag = m_bD3DXTessellation ? CD3D_REF : CD3D_ALL;

	float fConst[] = {1.f, 1.f, 0.f, 0.f};
    SetVertexShaderConstant(4, fConst, 1);

	// D3DX Tessellation takes 4 edge values
	float pfEdgeTess[4];
	for(int i=0; i<4; i++)
		pfEdgeTess[i] = pSurf->pTest->fRSTess;

	float* pfEitherTess = pSurf->pTest->nTessIndex >= g_nRectTess ? pfEdgeTess : g_RectTess[pSurf->pTest->nTessIndex];

	// Set the patch style
	SetRenderState(D3DRS_PATCHEDGESTYLE, (DWORD)pSurf->pTest->HOPStyle, dwDeviceFlag);
   	if(m_dwVersion < 0x900)
	{
		// D3DRS_PATCHSEGMENTS = 164		TODO: Put this in framework header somewhere
		SetRenderState((D3DRENDERSTATETYPE)164, (DWORD)*((unsigned*)&(pSurf->pTest->fRSTess)), dwDeviceFlag);
	}
	else
	{
		// no longer allowed to specify D3DRS_PATCHSEGMENTS and null under tess level here.  This makes sure
		// there is always a tess level specified here:
		f = pfEitherTess;
	}

	if(pSurf->pTest->PatchType == UTAHTEAPOT)
	{
		SetRenderState(D3DRS_SPECULARENABLE, (DWORD) FALSE);
		if(!pSurf->pTest->bSpecular)
		{
			SetMaterial(&m_TeapotMat);
		}
	}

	if(pSurf->pTest->DrawMode == NPATCH)
	{
		int nMove = pSurf->pTest->HOBasis == D3DBASIS_BEZIER ? ((int)pSurf->pTest->HODegree) : 1;
		int nRectsX = pSurf->pTest->HOBasis == D3DBASIS_BEZIER ? (pSurf->pTest->nWidth-1)  / nMove : 
		                                                         (pSurf->pTest->nWidth- (int)pSurf->pTest->HODegree);
		int nRectsY = pSurf->pTest->HOBasis == D3DBASIS_BEZIER ? (pSurf->pTest->nHeight-1) / nMove : 
																 (pSurf->pTest->nHeight- (int)pSurf->pTest->HODegree);

		// Fill in the RECTSURF structure
		memset(&RectInfo, 0, sizeof(D3DRECTPATCH_INFO));
		RectInfo.Width = ((int)pSurf->pTest->HODegree)+1;
		RectInfo.Height= ((int)pSurf->pTest->HODegree)+1;
		RectInfo.Stride = pSurf->pTest->nWidth;
		RectInfo.Basis =  pSurf->pTest->HOBasis;
		RectInfo.Degree =  pSurf->pTest->HODegree;
	
		for(int i=0; i<nRectsY; i++)
		for(int j=0; j<nRectsX; j++)
		{
			pSurfInfo = NULL;
	
			RectInfo.StartVertexOffsetWidth = j * nMove;
			RectInfo.StartVertexOffsetHeight = i * nMove;

			if(m_nCurrentSurf != m_nLastSurf)
			{
				pSurfInfo = &RectInfo;
				m_nSurfHandles++;
			}
		   
			// Draw the rect surface
			bRes &= DrawRectPatch(++nCached, f, pSurfInfo, dwDeviceFlag);

			if(m_bD3DXTessellation)
			{
				// Now draw to src using D3DX to tessellate:
				bRes &= DrawD3DXRectPatch(m_pSurfs[m_nCurrentSurf]->pVB, pfEitherTess, m_pSurfs[m_nCurrentSurf]->pTest->nShader, &RectInfo, CD3D_SRC);
			}
		}
	}
	else if(pSurf->pTest->DrawMode == ONEPATCH)
	{
    	// Fill in the RECTSURF structure
		memset(&RectInfo, 0, sizeof(D3DRECTPATCH_INFO));
		RectInfo.Width = pSurf->pTest->nWidth;
		RectInfo.Height= pSurf->pTest->nHeight;
		RectInfo.Stride = pSurf->pTest->nWidth;
		RectInfo.Basis =  pSurf->pTest->HOBasis;
		RectInfo.Degree =  pSurf->pTest->HODegree;

        // Draw the rect surface
		bRes &= DrawRectPatch(0, f, &RectInfo, dwDeviceFlag);

		if(m_bD3DXTessellation)
		{
			// Now draw to src using D3DX to tessellate:
			bRes &= DrawD3DXRectPatch(m_pSurfs[m_nCurrentSurf]->pVB, pfEitherTess, m_pSurfs[m_nCurrentSurf]->pTest->nShader, &RectInfo, CD3D_SRC);
		}
	}
	else if(pSurf->pTest->DrawMode == SEQUENTIAL)
	{
        memset(&RectInfo, 0, sizeof(D3DRECTPATCH_INFO));
		RectInfo.Width = ((int)pSurf->pTest->HODegree)+1;
		RectInfo.Height= ((int)pSurf->pTest->HODegree)+1;
		RectInfo.Stride= RectInfo.Width;
		RectInfo.Basis =  pSurf->pTest->HOBasis;
		RectInfo.Degree =  pSurf->pTest->HODegree;

		for(int i=0; i<pSurf->pTest->nWidth; i++)
		{
			pSurfInfo = NULL;
	
			RectInfo.StartVertexOffsetWidth = 0;
			RectInfo.StartVertexOffsetHeight = 4*i;

			if(m_nCurrentSurf != m_nLastSurf)
			{
				pSurfInfo = &RectInfo;
				m_nSurfHandles++;
			}

			// Draw the rect surface
			bRes &= DrawRectPatch(++nCached, f, pSurfInfo, dwDeviceFlag);

			if(m_bD3DXTessellation)
			{
				// Now draw to src using D3DX to tessellate:
				bRes &= DrawD3DXRectPatch(m_pSurfs[m_nCurrentSurf]->pVB, pfEitherTess, m_pSurfs[m_nCurrentSurf]->pTest->nShader, &RectInfo, CD3D_SRC);
			}
		}
        /*// Fill in the RECTSURF structure
		memset(&RectInfo, 0, sizeof(D3DRECTPATCH_INFO));
		RectInfo.Width = ((int)pSurf->pTest->HODegree)+1;
		RectInfo.Height= ((int)pSurf->pTest->HODegree)+1;
		RectInfo.Stride= pSurf->pTest->nWidth * 4;
		RectInfo.Basis =  pSurf->pTest->HOBasis;
		RectInfo.Degree =  pSurf->pTest->HODegree;

		for(int i=0; i<pSurf->pTest->nWidth; i++)
		{
			pSurfInfo = NULL;
	
			if(m_nCurrentSurf != m_nLastSurf)
			{
				RectInfo.StartVertexOffsetWidth = i * 4;
				RectInfo.StartVertexOffsetHeight = 0;
				pSurfInfo = &RectInfo;
			}
			// Draw the rect surface
			bRes &= DrawRectPatch(++m_nSurfHandles, f, pSurfInfo);
		}
		*/
	}
	else
	{
		bRes = false;
		WriteToLog(_T("CRectSurf:DrawSurface() - unknown DrawMode."));
	}

	return bRes;
}
