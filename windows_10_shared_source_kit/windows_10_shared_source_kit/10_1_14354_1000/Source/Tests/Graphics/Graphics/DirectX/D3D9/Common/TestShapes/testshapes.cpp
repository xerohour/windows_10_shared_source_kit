// ============================================================================
// 
//  TestShapes.cpp
//
// ============================================================================

#include "TestShapesPCH.h"

#include "TestShapes.h"
#include "d3d.h"

bool CTestShapes::NewShape(
	TS_SHAPES Type,	const int Divisions1, const int Divisions2,	const int Divisions3)
{
	// if old shape exists... destroy it first.
	if(m_pVertices)
	{
		Cleanup();
		Initialize();
	}
	m_primCount = 0L;

	// now create a shape of the correct type
	switch(Type)
	{
		case TS_POINTGRID:
			if(Divisions3 && Divisions2 && Divisions1)
				return GeneratePointGrid(Divisions1, Divisions2, Divisions3);
			else if(Divisions2 && Divisions1)
				return GeneratePointGrid(Divisions1, Divisions2);
			else if(Divisions1)
				return GeneratePointGrid(Divisions1);
			else
				return GeneratePointGrid();

		case TS_LINES:
			if(Divisions1)
				return GenerateLines(Divisions1);
			else
				return GenerateLines();

		case TS_RANDTRIS:
			if(Divisions1)
				return GenerateRandomTriangles(Divisions1);
			else
				return GenerateRandomTriangles();

		case TS_TRITHING:
			return GenerateTriThing();

		case TS_FANBALL:
			if(Divisions2 && Divisions1)
				return GenerateFanBall(Divisions1, Divisions2);
			else if(Divisions1)
				return GenerateFanBall(Divisions1);
			else
				return GenerateFanBall();

		case TS_FANHORN:
			if(Divisions2 && Divisions1)
				return GenerateFanHorn(Divisions1, Divisions2);
			else if(Divisions1)
				return GenerateFanHorn(Divisions1);
			else
				return GenerateFanHorn();

		case TS_WAVESTRIP:
			if(Divisions2 && Divisions1)
				return GenerateWaveStrip(Divisions1, Divisions2);
			else if(Divisions1)
				return GenerateWaveStrip(Divisions1);
			else
				return GenerateWaveStrip();

		default:
			return false;
	}
}

// ----------------------------------------------------------------------------

bool CTestShapes::NewShape(
	CS_SHAPES Type,	const int Divisions1, const int Divisions2,	const int Divisions3)
{
	bool bReturn;

	bReturn = CShapes::NewShape(Type, Divisions1, Divisions2, Divisions3);

	if(bReturn)
		m_Type = D3DPT_TRIANGLELIST;

	return bReturn;
}
