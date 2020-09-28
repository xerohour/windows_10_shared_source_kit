// FILE:        Path.cpp
// DESC:        Path class implementation.
//				The idea of this class is to define an entity that will be used as
//				a path. Then with each call to step return a postion on this path in
//				terms of an origin and 3 unit vectors.
// AUTHOR:      Anthony Clarke

#include "Path.h"

void CPath::InitPathLine(LPD3DXVECTOR3 pvecStart, LPD3DXVECTOR3 pvecEnd, UINT uSteps, LPD3DXVECTOR3 pvecPlaneNormal)
{
	D3DXVECTOR3 vecTemp;
	
	m_vecStart = *pvecStart;
	m_vecEnd = *pvecEnd;
	m_uMaxSteps = uSteps;
	
	m_fLength = D3DXVec3Length( D3DXVec3Subtract(&vecTemp, &m_vecEnd, &m_vecStart) ); 
	if (NULL == pvecPlaneNormal)
	{
		m_vecUnitPlaneNorm.x = 0;
		m_vecUnitPlaneNorm.y = 0;
		m_vecUnitPlaneNorm.z = 1.0;
	}
	else
	{
		D3DXVec3Normalize (&m_vecUnitPlaneNorm, pvecPlaneNormal);
	}
	m_PathType = PATH_LINE;
}

void CPath::InitPathCircle(LPD3DXVECTOR3 pvecCenter, LPD3DXVECTOR3 pvecStart, UINT uSteps, LPD3DXVECTOR3 pvecPlaneNormal)
{
	D3DXVECTOR3 vecTemp;

	m_vecEnd = m_vecStart = *pvecStart;
	m_vecCenter = *pvecCenter;
	m_uMaxSteps = uSteps;
	m_fRadius = D3DXVec3Length (D3DXVec3Subtract(&vecTemp, &m_vecCenter, &m_vecStart));

	if (NULL == pvecPlaneNormal)
	{
		m_vecUnitPlaneNorm.x = 0;
		m_vecUnitPlaneNorm.y = 0;
		m_vecUnitPlaneNorm.z = 1.0;
	}
	else
	{
		D3DXVec3Normalize (&m_vecUnitPlaneNorm, pvecPlaneNormal);
	}
	m_PathType = PATH_CIRCLE;
	return;
}

void CPath::InitLemniscate(LPD3DXVECTOR3 pvecCenter, UINT uSteps, float fLemnisScale, float fLemnisFactor, LPD3DXVECTOR3 pvecPlaneNormal)
{
	m_vecCenter = *pvecCenter;
	m_uMaxSteps = uSteps;
	//r = sin(2*Theta)
	//r = scale * ( sin ( factor * Theta) )	
	if (0 == fLemnisFactor)
		m_fLemnisFactor = DEFAULT_LEMNISFACTOR;
	else
		m_fLemnisFactor = fLemnisFactor;

	if (0 == fLemnisScale)
		m_fLemnisScale = DEFAULT_LEMNISSCALE;
	else
		m_fLemnisScale = fLemnisScale;
	
	if (NULL == pvecPlaneNormal)
	{
		m_vecUnitPlaneNorm.x = 0;
		m_vecUnitPlaneNorm.y = 0;
		m_vecUnitPlaneNorm.z = 1.0;
	}
	else
	{
		D3DXVec3Normalize (&m_vecUnitPlaneNorm, pvecPlaneNormal);
	}
	m_PathType = PATH_LEMNISCATE;
	return;
}


bool CPath::GetStep(int nStep, LPD3DXVECTOR3 pvecOrigin, LPD3DXVECTOR3 pvecX, LPD3DXVECTOR3 pvecY, LPD3DXVECTOR3 pvecZ )
{
	D3DXVECTOR3 vecTemp;
	float fMoveDist;

	if ( nStep < 0 )
	{
		nStep = abs(nStep);
		nStep = nStep % m_uMaxSteps;
		nStep = m_uMaxSteps - nStep;		
	}
		
	nStep = nStep % m_uMaxSteps;
	fMoveDist = float(nStep)/float(m_uMaxSteps);
	switch (m_PathType)
	{
		case PATH_LINE :
		{
			fMoveDist *= m_fLength;
			D3DXVec3Normalize (pvecZ, D3DXVec3Subtract(&vecTemp, &m_vecEnd, &m_vecStart) );
			D3DXVec3Add(pvecOrigin, &m_vecStart, D3DXVec3Scale(&vecTemp, pvecZ, fMoveDist) );
			*pvecY = m_vecUnitPlaneNorm;
			D3DXVec3Cross( pvecX, pvecZ, pvecY );
			break;
		}
		case PATH_CIRCLE :
		{
			//Still 2D needs work.
			fMoveDist *= (2*pi);
			pvecOrigin->x = m_vecCenter.x + (m_fRadius * cos(fMoveDist));
			pvecOrigin->y = m_vecCenter.y + (m_fRadius * sin(fMoveDist));
			pvecOrigin->z = 0.5;
			//Get the tangent normal
			D3DXVec3Normalize ( pvecX, D3DXVec3Subtract(&vecTemp, &m_vecCenter, pvecOrigin) );
			*pvecY = m_vecUnitPlaneNorm;
			//Get tangent to circle
			D3DXVec3Cross( pvecZ, pvecY, pvecX );
			break;
		}
		case PATH_LEMNISCATE :
		{
			//Still 2D needs work.
			fMoveDist *= (2.0f*pi);
			//r = scale * ( sin ( factor * Theta) )
			m_fRadius = m_fLemnisScale * ( sin (m_fLemnisFactor * fMoveDist) );
			pvecOrigin->x = m_vecCenter.x + (m_fRadius * cos(fMoveDist));
			pvecOrigin->y = m_vecCenter.y + (m_fRadius * sin(fMoveDist));
			pvecOrigin->z = 0.5;			
			//Get reasonable x value for tangent 
			pvecZ->x = -sin (fMoveDist) * sin (m_fLemnisFactor * fMoveDist);
			pvecZ->x += m_fLemnisFactor * cos (fMoveDist) * cos ( m_fLemnisFactor * fMoveDist );
			pvecZ->x *= m_fLemnisScale;
			pvecZ->y = cos (fMoveDist) * sin ( m_fLemnisFactor * fMoveDist );
			pvecZ->y += m_fLemnisFactor * sin (fMoveDist) * cos ( m_fLemnisFactor * fMoveDist );
			pvecZ->y *= m_fLemnisScale;
			pvecZ->z = 0.5;

			D3DXVec3Normalize ( pvecZ, pvecZ);	
			*pvecY = m_vecUnitPlaneNorm;
			//Get tangent normal
			D3DXVec3Cross( pvecX, pvecY, pvecZ );
			break;
		}
	}
	return true;
}
