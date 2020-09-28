// FILE:        path.cpp
// DESC:        path class methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:

#include <d3d9.h>
#include <d3dx9.h>
#include "path.h"

// NAME:        CPath()
// DESC:        constrctor for path class
// INPUT:       none
// OUTPUT:      none

CPath::CPath()
{
    D3DXMatrixIdentity(&m_Matrix);

    m_Normal.x = 0.0f;
    m_Normal.y = 0.0f;
    m_Normal.z = 1.0f;
}

// NAME:        ~CPath()
// DESC:        destructor for path class
// INPUT:       none
// OUTPUT:      none

CPath::~CPath()
{
}

// NAME:        Line()
// DESC:        line initialization
// INPUT:       pStart...address of D3DXVECTOR3 start point
//              pEnd.....address of D3DXVECTOR3 end point
//              nSteps...number of steps thru line path
// OUTPUT:      none

void CPath::Line(LPD3DXVECTOR3 pStart, LPD3DXVECTOR3 pEnd, UINT nSteps)
{
    D3DXVECTOR3 Temp;

    m_nMaxSteps = nSteps;
    m_Start = *pStart;
    m_End = *pEnd;
    m_fLength = D3DXVec3Length(D3DXVec3Subtract(&Temp, &m_End, &m_Start));
    m_Type = PATH_LINE;
}

// NAME:        Circle()
// DESC:        circle initialization
// INPUT:       pCenter .circle center
//              fRadius..circle radius
//              nSteps...number of steps thru circle path
// OUTPUT:      none

void CPath::Circle(LPD3DXVECTOR3 pCenter, float fRadius, UINT nSteps)
{
    D3DXVECTOR3 Temp;

    m_nMaxSteps = nSteps;
    m_Center = *pCenter;
    m_fRadius = fRadius;
    m_Type = PATH_CIRCLE;
    D3DXMatrixTranslation(&m_Matrix, m_Center.x, m_Center.y, m_Center.z);
}

// NAME:        Lemniscate()
// DESC:        lemniscate initialization
// INPUT:       pCenter..lemniscate center
//              fScale...lemniscate scale
//              fFactor..lemniscate factor
//              nSteps...number of steps thru lemniscate
// OUTPUT:      none

void CPath::Lemniscate(LPD3DXVECTOR3 pCenter, float fScale, float fFactor, UINT nSteps)
{    
    m_nMaxSteps = nSteps;
    m_Center = *pCenter;

    if (0 == fFactor)
        m_fFactor = DEFAULT_FACTOR;
    else
        m_fFactor = fFactor;

    if (0 == fScale)
        m_fScale = DEFAULT_SCALE;
    else
        m_fScale = fScale;

    m_Type = PATH_LEMNISCATE;
    D3DXMatrixTranslation(&m_Matrix, m_Center.x, m_Center.y, m_Center.z);
}

// NAME:        SetCurrentRotXYZ()
// DESC:        set current rotation matrix
// INPUT:       fRadX....rotation in x (radians)
//              fRadY....rotation in y (radians)
//              fRadZ....rotation in z (radians)
// OUTPUT:      none

void CPath::SetCurrentRotXYZ(float fRadX, float fRadY, float fRadZ)
{   
    D3DXMATRIX Temp;

    D3DXMatrixRotationX(&m_Matrix, fRadX);
    D3DXMatrixRotationY(&Temp, fRadY); 
    D3DXMatrixMultiply(&m_Matrix, &m_Matrix, &Temp);
    D3DXMatrixRotationZ(&Temp, fRadZ); 
    D3DXMatrixMultiply(&m_Matrix, &m_Matrix, &Temp);
    D3DXMatrixTranslation(&Temp, m_Center.x, m_Center.y, m_Center.z);
    D3DXMatrixMultiply(&m_Matrix, &m_Matrix, &Temp);
}

// NAME:        GetStep()
// DESC:        get step
// INPUT:       nStep....step number
//              pOrigin..address of D3DXVECTOR3 origin
//              pX.......address of D3DXVECTOR3 x axis
//              pY.......address of D3DXVECTOR3 y axis
//              pZ.......address of D3DXVECTOR3 z axis
// OUTPUT:      true.....if step computed successfully
//              false....otherwise

bool CPath::GetStep(int nStep, LPD3DXVECTOR3 pOrigin, LPD3DXVECTOR3 pX, LPD3DXVECTOR3 pY, LPD3DXVECTOR3 pZ)
{
    D3DXVECTOR3 Temp;
    float fMoveDist;

    if (nStep < 0)
    {
        nStep = -nStep;
        nStep = nStep % m_nMaxSteps;
        nStep = m_nMaxSteps - nStep;
    }

    nStep = nStep % m_nMaxSteps;
    fMoveDist = ((float) nStep)/((float) m_nMaxSteps);

    switch(m_Type)
    {
        case PATH_LINE:
            fMoveDist *= m_fLength;
            D3DXVec3Normalize(pZ, D3DXVec3Subtract(&Temp, &m_End, &m_Start));
            D3DXVec3Add(pOrigin, &m_Start, D3DXVec3Scale(&Temp, pZ, fMoveDist));
            *pY = m_Normal;
            D3DXVec3Cross(pX, pZ, pY);
        break;

        case PATH_CIRCLE:
            fMoveDist *= (float) (2.0*D3DX_PI);
            pOrigin->x = (float) (m_fRadius*cos(fMoveDist));
            pOrigin->y = (float) (m_fRadius*sin(fMoveDist));
            pOrigin->z = 0.0;
            D3DXVec3Normalize(pX, D3DXVec3Subtract(&Temp, &m_Center, pOrigin));
            *pY = m_Normal;
            D3DXVec3Cross(pZ, pY, pX);
        break;

        case PATH_LEMNISCATE:
            fMoveDist *= (float) (2.0*D3DX_PI);
            m_fRadius = (float) (m_fScale*sin(m_fFactor*fMoveDist));
            pOrigin->x = (float) (m_fRadius*cos(fMoveDist));
            pOrigin->y = (float) (m_fRadius*sin(fMoveDist));
            pOrigin->z = 0.0f;
            pZ->x = (float) (-sin(fMoveDist)*sin(m_fFactor*fMoveDist));
            pZ->x += (float) (m_fFactor*cos(fMoveDist)*cos(m_fFactor*fMoveDist));
            pZ->x *= m_fScale;
            pZ->y = (float) (cos(fMoveDist)*sin(m_fFactor*fMoveDist));
            pZ->y += (float) (m_fFactor*sin(fMoveDist)*cos(m_fFactor*fMoveDist));
            pZ->y *= m_fScale;
            pZ->z = 0.0f;
            D3DXVec3Normalize(pZ, pZ);    
            *pY = m_Normal;
            D3DXVec3Cross(pX, pY, pZ);
        break;
    }

    // transform to current coord system

    D3DXVec3TransformCoord(pOrigin, pOrigin, &m_Matrix);
    D3DXVec3TransformCoord(pX, pX, &m_Matrix);
    D3DXVec3TransformCoord(pY, pY, &m_Matrix);
    D3DXVec3TransformCoord(pZ, pZ, &m_Matrix);
    return true;
}
