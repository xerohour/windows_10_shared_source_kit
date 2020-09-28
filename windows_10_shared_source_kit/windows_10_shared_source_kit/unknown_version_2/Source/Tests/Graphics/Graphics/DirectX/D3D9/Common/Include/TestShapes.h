// ============================================================================
//
//  CTestShapes.h
//
//  Test shapes file for API tests.
//
//  Produces a number of shapes that aren't really interesting in any other
//  way except that they're good test shapes.
//
// ============================================================================

#ifndef _TESTSHAPES_H_
#define _TESTSHAPES_H_

#include "cshapes.h"

#define TS_NSHAPES 7
enum TS_SHAPES {
	TS_POINTGRID,
	TS_LINES,
	TS_RANDTRIS,
	TS_TRITHING,
	TS_FANBALL,
	TS_FANHORN,
	TS_WAVESTRIP
};

class CTestShapes : public CShapes
{
public:
	D3DPRIMITIVETYPE m_Type;
	DWORD            m_primCount;

	bool NewShape(
		TS_SHAPES Type,
		const int Divisions1 = 0,
		const int Divisions2 = 0,
		const int Divisions3 = 0);

	bool NewShape(
		CS_SHAPES Type,
		const int Divisions1 = 0,
		const int Divisions2 = 0,
		const int Divisions3 = 0);

protected:
	bool GeneratePointGrid(const int xDiv = 10, const int yDiv = 10, const int zDiv = 1);
	bool GenerateLines(const int nLines = 10);
	bool GenerateRandomTriangles(const DWORD dwTriangles = 10);
	bool GenerateTriThing();
	bool GenerateFanBall(const DWORD dwDivisions = 10, const DWORD dwRotations = 1);
	bool GenerateFanHorn(const DWORD dwDivisions = 10, const DWORD dwRotations = 1);
	bool GenerateWaveStrip(const DWORD dwDivisions = 10, const DWORD dwWaves = 1);
};

#endif
