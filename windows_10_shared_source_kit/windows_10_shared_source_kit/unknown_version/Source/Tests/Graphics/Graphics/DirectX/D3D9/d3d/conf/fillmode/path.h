// FILE:        Path.h
// DESC:        Path header
// AUTHOR:      Anthony W. Clarke
// COMMENTS:	

#include <d3d8.h>
#include <d3dx8.h>
#include "CD3DTest.h"
#include "math.h"

#ifndef _PATH
#define _PATH

#define DEFAULT_LEMNISFACTOR 2
#define DEFAULT_LEMNISSCALE 1

typedef enum _PATHTYPE {
    PATH_LINE = 1,
	PATH_CIRCLE = 2,
	PATH_LEMNISCATE = 3
} PATHTYPE;


class CPath
{
public:	
	void InitPathLine(LPD3DXVECTOR3 pvecStart, LPD3DXVECTOR3 pvecEnd, UINT uSteps, LPD3DXVECTOR3 pvecPlaneNormal = NULL);
	void InitPathCircle(LPD3DXVECTOR3 pvecCenter, LPD3DXVECTOR3 pvecStart, UINT uSteps, LPD3DXVECTOR3 pvecPlaneNormal = NULL);
	void InitLemniscate(LPD3DXVECTOR3 pvecCenter, UINT uSteps, float fLemnisScale, float fLemnisFactor, LPD3DXVECTOR3 pvecPlaneNormal = NULL);
	bool GetStep(int nStep, LPD3DXVECTOR3 pvecOrigin, LPD3DXVECTOR3 pvecX, LPD3DXVECTOR3 pvecY, LPD3DXVECTOR3 pvecZ );

private:
	PATHTYPE m_PathType;
	D3DXVECTOR3 m_vecStart, m_vecEnd;
	D3DXVECTOR3 m_vecCenter;
	D3DXVECTOR3 m_vecUnitPlaneNorm;
	UINT m_uMaxSteps;
	float m_fLength, m_fRadius, m_fLemnisScale, m_fLemnisFactor;
};

#endif