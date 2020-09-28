// FILE:        CSpline.cpp
// DESC:        spline class methods
// AUTHOR:      Todd M. Frost
// COMMENTS:    weighted v-spline interpolation
// REFERENCE:   Parametric Weighted v-Splines
//              ACM Transactions on Mathematical Software
//              vol. 13, no. 1, March 1987

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "CSpline.h"

#define DPF(s) Debug((s), (__FILE__), (__LINE__))

#define ZERO 1.0e-8

// NAME:        CSpline()
// DESC:        spline class constructor
// INPUT:       none
// OUTPUT:      none

CSpline::CSpline()
{
    m_dwFlags = CS_CHORD;
    m_nPoints = (UINT) 0;
    m_pPolygon = NULL;
}

// NAME:        ~CSpline()
// DESC:        spline class destructor
// INPUT:       none
// OUTPUT:      none

CSpline::~CSpline()
{
    PCONTROLPOINT pPolygon = m_pPolygon, pTemp;

    while (pPolygon)
    {
        pTemp = pPolygon;
        pPolygon = pPolygon->pNext;
        delete pTemp;
    }
}

// NAME:        GetKnot()
// DESC:        get knot for control point i
// INPUT:       i........control point index
//              pData....address of double knot value
// OUTPUT:      true.....if control point index is valid
//              false....otherwise

bool CSpline::GetKnot(UINT i, double *pData)
{
    PCONTROLPOINT pPoint = GetControlPoint(i);
   
    if (!pPoint)
        return false;
 
    if (!pData)
    {
        DPF("NULL input pointer");
        return false;
    }

    *pData = pPoint->k;
    return true;
}

// NAME:		GetTension()
// DESC:		get tension for control point i
// INPUT:		i........control point index
//              pData....address of double tension value
// OUTPUT:		true.....if control point index is valid
//			    false....otherwise

bool CSpline::GetTension(UINT i, double *pData)
{
    PCONTROLPOINT pPoint = GetControlPoint(i);

    if (!pPoint)
        return false;

    if (!pData)
    {
        DPF("NULL input pointer");
        return false;
    }

    *pData = pPoint->v;
    return true;
}

// NAME:        GetWeight()
// DESC:        get weight for control point i
// INPUT:       i........control point index
//              pData....address of double weight value
// OUTPUT:      true.....if control point index is valid
//              false....otherwise

bool CSpline::GetWeight(UINT i, double *pData)
{
    PCONTROLPOINT pPoint = GetControlPoint(i);

    if (!pPoint)
        return false;

    if (!pData)
    {
        DPF("NULL input pointer");
        return false;
    }

    *pData = pPoint->w;
    return true;
}

// NAME:        GetPosition()
// DESC:        get position for control point i
// INPUT:       i........control point index
//              pData....address of DATAPOINT
// OUTPUT:      true.....if control point index is valid
//              false....otherwise

bool CSpline::GetPosition(UINT i, PDATAPOINT pData)
{
    PCONTROLPOINT pPoint = GetControlPoint(i);

    if (!pPoint)
        return false;

    if (!pData)
    {
        DPF("NULL input pointer");
        return false;
    }

    pData->x = pPoint->Px;
    pData->y = pPoint->Py;
    pData->z = pPoint->Pz;
    return true;
}

// NAME:        GetInterval()
// DESC:        get interval corresponding to evaluation parameter
// INPUT:       u........evaluation parameter
// OUTPUT:      interval i such that k[i] <= u <= k[i+1]

UINT CSpline::GetInterval(double u)
{
    PCONTROLPOINT pPoint;
    UINT i, nInterval = (UINT) 0;

    if (m_nPoints < 2)
        return (UINT) 0;

    if (m_dwFlags & CS_RECOMPUTE)
        SetCoefficients(m_dwFlags);

    for (i = 0, pPoint = m_pPolygon; pPoint && pPoint->pNext; pPoint = pPoint->pNext, i++)
    {
        if ((pPoint->k <= u) && (u <= pPoint->pNext->k))
        {
            nInterval = i;
            break;
        }
    }

    return nInterval;
}

// NAME:		SetTension()
// DESC:		set tension value
// INPUT:		i........control point index
//              data.....tension value
// OUTPUT:      true.....if control point index is valid
//              false....otherwise

bool CSpline::SetTension(UINT i, double data)
{
    PCONTROLPOINT pPoint = GetControlPoint(i);

    if (!pPoint)
        return false;

    pPoint->v = data;
    m_dwFlags |= CS_RECOMPUTE;
    return true;
}

// NAME:        SetWeight()
// DESC:        set weight value
// INPUT:       i........control point index
//              data.....weight value
// OUTPUT:      true.....if control point index is valid
//              false....otherwise

bool CSpline::SetWeight(UINT i, double data)
{
    PCONTROLPOINT pPoint = GetControlPoint(i);

    if (!pPoint)
        return false;

    pPoint->w = data;
    m_dwFlags |= CS_RECOMPUTE;
    return true;
}

// NAME:        SetPosition()
// DESC:        set position value
// INPUT:       i........control point index
//              pData....address of DATAPOINT data
// OUTPUT:      true.....if control point index is valid
//              false....otherwise

bool CSpline::SetPosition(UINT i, PDATAPOINT pData)
{
    PCONTROLPOINT pPoint = GetControlPoint(i);

    if (!pPoint)
        return false;

    if (!pData)
    {
        DPF("NULL input pointer");
        return false;
    }

    pPoint->Px = pData->x;
    pPoint->Py = pData->y;
    pPoint->Pz = pData->z;
    m_dwFlags |= CS_RECOMPUTE;
    return true;
}

// NAME:        SetSpacing()
// DESC:        set spacing
// INPUT:       dwFlags..spacing flags
// OUTPUT:      existing spacing flags

DWORD CSpline::SetSpacing(DWORD dwFlags)
{
    DWORD dwReport = (m_dwFlags & CS_SPACING);

    m_dwFlags &= ~CS_SPACING;
    m_dwFlags |= (dwFlags & CS_SPACING);
    m_dwFlags |= CS_RECOMPUTE;
    return dwReport;
}

// NAME:        FixTangent()
// DESC:        fix tangent
// INPUT:       i........control point index
//              pData....address of DATAPOINT data
// OUTPUT:      true.....if control point index is valid
//              false....otherwise

bool CSpline::FixTangent(UINT i, PDATAPOINT pData)
{
    PCONTROLPOINT pPoint = GetControlPoint(i);

    if (!pPoint)
        return false;

    pPoint->Tx = pData->x;
    pPoint->Ty = pData->y;
    pPoint->Tz = pData->z;
    pPoint->dwFlags |= CS_FIXED;
    m_dwFlags |= CS_RECOMPUTE;
    return true;
}

// NAME:        FreeTangent()
// DESC:        free tangent
// INPUT:       i........control point index
// OUTPUT:      true.....if control point index is valid
//              false....otherwise

bool CSpline::FreeTangent(UINT i)
{
    PCONTROLPOINT pPoint = GetControlPoint(i);

    if (!pPoint)
        return false;

    pPoint->dwFlags &= ~CS_FIXED;
    m_dwFlags |= CS_RECOMPUTE;
    return true;
}

// NAME:        SetControlPoints()
// DESC:        set control points
// INPUT:       nPoints..number of points
//              pData....address of DATAPOINT data
// OUTPUT:      true.....if control points are set successfully
//              false....otherwise
// COMMENTS:    existing control points are deleted

bool CSpline::SetControlPoints(UINT nPoints, PDATAPOINT pData)
{
    PCONTROLPOINT pPolygon = m_pPolygon, pTemp;

    while (pPolygon)
    {
        pTemp = pPolygon;
        pPolygon = pPolygon->pNext;
        delete pTemp;
    }

    m_pPolygon = NULL;

    for (UINT i = 0; i < nPoints; i++)
        if (!AppendControlPoint((i ? (i - 1) : 0), (pData ? &pData[i] : NULL)))
            break;
    
    if (nPoints == m_nPoints)
        return true;
    else
        return false;
}

// NAME:        AppendControlPoint()
// DESC:        append control point
// INPUT:       i........control point index
//              pData....address of DATAPOINT data
// OUTPUT:      true.....if control point is successfully appended
//              false....otherwise
// COMMENTS:    default weight:   1 
//              default tension:  0
//              default position: (i, i, i)

bool CSpline::AppendControlPoint(UINT i, PDATAPOINT pData)
{
    PCONTROLPOINT pPolygon = (m_nPoints > 0) ? GetControlPoint(i) : NULL;
    PCONTROLPOINT pNew = new CONTROLPOINT;

    if (!pNew)
    {
        DPF("Unable to allocate new point");
        return false;
    }

    memset(pNew, 0, sizeof(CONTROLPOINT));
    pNew->w = 1.0;
    pNew->Px = pData ? pData->x : (double) i;
    pNew->Py = pData ? pData->y : (double) i;
    pNew->Pz = pData ? pData->z : (double) i;

    if (m_pPolygon)
    {
        if (!pPolygon)
        {
            DPF("Invalid control point index");
            delete pNew;
            return false;
        }
        else if (!pPolygon->pNext)
        {
            pPolygon->pNext = pNew;
            pNew->pPrev = pPolygon;
            m_nPoints++;
        }
        else
        {
            pNew->pPrev = pPolygon;
            pNew->pNext = pPolygon->pNext;
            pPolygon->pNext->pPrev = pNew;
            pPolygon->pNext = pNew;
            m_nPoints++;
        }
    }
    else
    {
        m_pPolygon = pNew;
        m_nPoints = (UINT) 1;
    }

    m_dwFlags |= CS_RECOMPUTE;
    return true;
}

// NAME:        DeleteControlPoint()
// DESC:        delete control point
// INPUT:       i........control point index
// OUTPUT:      true.....if control point is successfully deleted
//              false....otherwise

bool CSpline::DeleteControlPoint(UINT i)
{
    PCONTROLPOINT pPolygon = GetControlPoint(i);

    if (!pPolygon)
    {
        DPF("Invalid control point index");
        return false;
    }

    if (pPolygon->pPrev)
    {
        if (pPolygon->pNext)
        {
            pPolygon->pNext->pPrev = pPolygon->pPrev;
            pPolygon->pPrev->pNext = pPolygon->pNext;
        }
        else
        {
            pPolygon->pPrev->pNext = NULL;
        }
    }
    else if (pPolygon->pNext)
    {
        m_pPolygon = pPolygon->pNext;
        m_pPolygon->pPrev = NULL;
    }
    else
    {
        m_pPolygon = NULL;
    }

    delete pPolygon;
    m_nPoints--;
    m_dwFlags |= CS_RECOMPUTE;
    return true;
}

// NAME:        Position
// DESC:        position of spline at u
// INPUT:       u........spline parameter
//              pData....address of DATAPOINT data
//              bUnit....unit parameterization flag ([0,1] vs [k[0],k[n-1]])
// OUTPUT:      true.....if position evaluation successful
//              false....otherwise

bool CSpline::Position(double u, PDATAPOINT pData, bool bUnit)
{
    if (m_nPoints < 2)
    {
        DPF("No control polygon (must have at least two (2) points)");
        return false;
    }

    if (!pData)
    {
        DPF("NULL input pointer");
        return false;
    }

    if (m_dwFlags & CS_RECOMPUTE)
        SetCoefficients(m_dwFlags);

    double B0, B1, D0, D1, max;

    if (!GetKnot((m_nPoints - 1), &max))
    {
        DPF("Unable to get max knot value, using 1.0");
        max = 1.0;
    }

    u = (bUnit) ? u*max : u;
    u = (u < 0.0) ? 0.0 : ((u > max)  ? max : u);
    PCONTROLPOINT pPoint = GetControlPoint(GetInterval(u));

    if (!pPoint)
    {
        DPF("Invalid spline parameter");
        return false;
    }

    GetBasis((u - pPoint->k)/(pPoint->d), &B0, &B1, &D0, &D1);
    pData->x = pPoint->Px*B0 + pPoint->pNext->Px*B1 +
               pPoint->d*(pPoint->Tx*D0 + pPoint->pNext->Tx*D1);
    pData->y = pPoint->Py*B0 + pPoint->pNext->Py*B1 +
               pPoint->d*(pPoint->Ty*D0 + pPoint->pNext->Ty*D1);
    pData->z = pPoint->Pz*B0 + pPoint->pNext->Pz*B1 +
               pPoint->d*(pPoint->Tz*D0 + pPoint->pNext->Tz*D1);
    return true;
}

// NAME:        Tangent()
// DESC:        tangent of spline at u
// INPUT:       u........spline parameter
//              pData....address DATAPOINT data
//              bUnit....unit parameterization flag ([0,1] vs [k[0],k[n-1]])
// OUTPUT:      true.....if tangent evaluation successful
//              false....otherwise

bool CSpline::Tangent(double u, PDATAPOINT pData, bool bUnit)
{
    if (!pData)
    {
        DPF("NULL input pointer");
        return false;
    }

    if (!Derivative(u, pData, bUnit))
        return false;

    double temp = pData->x*pData->x + pData->y*pData->y + pData->z*pData->z;

    if (temp < ZERO)
    {
        pData->x = pData->y = pData->z = 0.0;
        return false;
    }
    else
    {
        temp = sqrt(temp);

        if (temp < ZERO)
        {
            pData->x = pData->y = pData->z = 0.0;
            return false;
        }
        else
        {
            pData->x /= temp;
            pData->y /= temp;
            pData->z /= temp;
        }
    }

    return true;
}

// NAME:        Derivative()
// DESC:        derivative of spline at u
// INPUT:       u........spline parameter
//              pData....address of DATAPOINT data
//              bUnit....unit parameterization flag ([0,1] vs [k[0],k[n-1]])
// OUTPUT:      true.....if derivative evaluation successful
//              false....otherwise

bool CSpline::Derivative(double u, PDATAPOINT pData, bool bUnit)
{
    if (!pData)
    {
        DPF("NULL input pointer");
        return false;
    }

    if (m_nPoints < 2)
    {
        DPF("No control polygon (must have at least two (2) points)");
        return false;
    }

    if (m_dwFlags & CS_RECOMPUTE)
        SetCoefficients(m_dwFlags);

    double DB0, DB1, DD0, DD1, max;

    if (!GetKnot((m_nPoints - 1), &max))
    {
        DPF("Unable to get max knot value, using 1.0");
        max = 1.0;
    }

    u = (bUnit) ? u*max : u;
    u = (u < 0.0) ? 0.0 : ((u > max)  ? max : u);
    PCONTROLPOINT pPoint = GetControlPoint(GetInterval(u));

    if (!pPoint)
    {
        DPF("Invalid spline parameter");
        return false;
    }

    GetBasisDerivative((u - pPoint->k)/(pPoint->d), &DB0, &DB1, &DD0, &DD1);
    pData->x = (pPoint->Px*DB0 + pPoint->pNext->Px*DB1)/pPoint->d +
               (pPoint->Tx*DD0 + pPoint->pNext->Tx*DD1);
    pData->y = (pPoint->Py*DB0 + pPoint->pNext->Py*DB1)/pPoint->d +
               (pPoint->Ty*DD0 + pPoint->pNext->Ty*DD1);
    pData->z = (pPoint->Pz*DB0 + pPoint->pNext->Pz*DB1)/pPoint->d +
               (pPoint->Tz*DD0 + pPoint->pNext->Tz*DD1);
    return true;
}

// NAME:        Nearest()
// DESC:        nearest point
// INPUT:       pData....address of DATAPOINT data
// OUTPUT:      true.....if nearest point computed successfully
//              false....otherwise

bool CSpline::Nearest(PDATAPOINT pData)
{
    if (!pData)
    {
        DPF("NULL input pointer");
        return false;
    }

    DPF("Nearest() not implemented yet");
    return false;
}

// NAME:        Linearize
// DESC:        linearize spline
// INPUT:       puCount..address of UINT number of points
//              angle....angle (radians)
// OUTPUT:      pPoints..address of DATAPOINT data if spline is linearized successfully
//              NULL.....otherwise

PDATAPOINT CSpline::Linearize(UINT *puCount, double angle)
{
    PDATAPOINT pData = NULL;

    if (!puCount)
    {
        DPF("NULL input pointer");
        return NULL;
    }

    DPF("Linearize() not implemented yet");
    return pData;
}

// NAME:        Plot()
// DESC:        plot
// INPUT:       nEval....number of evaluations over interval P[i]:P[i+1]
//              pPoints..address of DATAPOINT data
//              Handler..address of _PlotDataHandler function
// OUTPUT:      none

VOID CSpline::Plot(UINT nEval, PDATAPOINT pPoints, _PlotDataHandler Handler) 
{
    PCONTROLPOINT pPolygon = m_pPolygon;
    
    if (!pPolygon)
        return;

    if (nEval < 1)
        return;

    if ((nEval < 2) || !m_pPolygon->pNext)
    {
        pPoints[0].x = pPolygon->Px;
        pPoints[0].y = pPolygon->Py;
        pPoints[0].z = pPolygon->Pz;
        Handler((UINT) 1, pPoints);
        return;
    }

    if (m_dwFlags & CS_RECOMPUTE)
        SetCoefficients(m_dwFlags);

    // evaluate piecewise cubic segments and pass data to plot data handler

    for (pPolygon = m_pPolygon; pPolygon->pNext; pPolygon = pPolygon->pNext)
    {
        double B0, B1, D0, D1, epsilon, t;
        DATAPOINT pP[2], pT[2];
        UINT i;

        pPoints[0].x = pP[0].x = pPolygon->Px;
        pPoints[0].y = pP[0].y = pPolygon->Py;
        pPoints[0].z = pP[0].z = pPolygon->Pz;

        pPoints[nEval - 1].x = pP[1].x = pPolygon->pNext->Px;
        pPoints[nEval - 1].y = pP[1].y = pPolygon->pNext->Py;
        pPoints[nEval - 1].z = pP[1].z = pPolygon->pNext->Pz;

        pT[0].x = pPolygon->Tx*pPolygon->d;
        pT[0].y = pPolygon->Ty*pPolygon->d;
        pT[0].z = pPolygon->Tz*pPolygon->d;

        pT[1].x = pPolygon->pNext->Tx*pPolygon->d;
        pT[1].y = pPolygon->pNext->Ty*pPolygon->d;
        pT[1].z = pPolygon->pNext->Tz*pPolygon->d;

        epsilon = 1.0/((double) (nEval - 1));

        for (i = 1, t = epsilon; i < (nEval - 1); i++, t += epsilon)
        {
            GetBasis(t, &B0, &B1, &D0, &D1);
            pPoints[i].x = pP[0].x*B0 + pP[1].x*B1 + pT[0].x*D0 + pT[1].x*D1;
            pPoints[i].y = pP[0].y*B0 + pP[1].y*B1 + pT[0].y*D0 + pT[1].y*D1;
            pPoints[i].z = pP[0].z*B0 + pP[1].z*B1 + pT[0].z*D0 + pT[1].z*D1;
        }

        Handler(nEval, pPoints);
    }
}

// NAME:        GetControlPoint()
// DESC:        get control point i
// INPUT:       i.......control point index
// OUTPUT:      pPoint..if control point index is valid
//              NULL....otherwise

PCONTROLPOINT CSpline::GetControlPoint(UINT i)
{
    PCONTROLPOINT pPolygon;
    UINT j;

    if (m_nPoints <= i)
    {
        DPF("Invalid control point index");
        return NULL;
    }

    for (pPolygon = m_pPolygon, j = 0; pPolygon && (i != j); pPolygon = pPolygon->pNext, j++);
    return pPolygon;
}

// NAME:        GetBasis()
// DESC:        get Hermite basis functions
// INPUT:       t........evaluation parameter
//              pB0......address of double Hermite basis function B0
//              pB1......address of double Hermite basis function B1
//              pD0......address of double Hermite basis function D0
//              pD1......address of double Hermite basis function D1
// OUTPUT:      none

VOID CSpline::GetBasis(double t, double *pB0, double *pB1, double *pD0, double *pD1)
{
    if (pB0)
        *pB0 = (1.0 - t)*(1.0 - t)*(2.0*t + 1.0);

    if (pB1)
        *pB1 = t*t*(3.0 - 2.0*t);

    if (pD0)
        *pD0 = t*(1.0 - t)*(1.0 - t);

    if (pD1)
        *pD1 = t*t*(t - 1.0);
}

// NAME:        GetBasisDerivative()
// DESC:        get Hermite basis functions derivatives
// INPUT:       t........evaluation parameter
//              pDB0.....address of double derivative of Hermite basis function B0
//              pDB1.....address of double derivative of Hermite basis function B1
//              pDD0.....address of double derivative of Hermite basis function D0
//              pDD1.....address of double derivative of Hermite basis function D1
// OUTPUT:      none

VOID CSpline::GetBasisDerivative(double t, double *pDB0, double *pDB1, double *pDD0, double *pDD1)
{
    if (pDB0)
        *pDB0 = 6.0*t*(t - 1.0);

    if (pDB1)
        *pDB1 = 6.0*t*(1.0 - t);

    if (pDD0)
        *pDD0 = 1.0 + t*(3.0*t - 4.0);

    if (pDD1)
        *pDD1 = t*(3.0*t - 2.0);
}

// NAME:        SetCoefficients()
// DESC:        set coefficients
// INPUT:       dwFlags..knot flags
// OUTPUT:      true.....if coefficients set successfully
//              false....otherwise

bool CSpline::SetCoefficients(DWORD dwFlags)
{
    double f, r;
    DATAPOINT pTemp[2];
    PCONTROLPOINT pPolygon = m_pPolygon;

    if (!(m_dwFlags & CS_RECOMPUTE))
        return true;

    // special case for less than 2 control points

    if (m_nPoints < 2)
    {
        m_dwFlags &= ~CS_RECOMPUTE;
        return true;
    }

    // recompute knot vector

    SetKnots(m_dwFlags);

    // set up first row

    pPolygon->c = pPolygon->w/pPolygon->d;
    pPolygon->r = 0.5*pPolygon->v + 2.0*pPolygon->c;
    f = 3.0*pPolygon->c/pPolygon->d;
    pTemp[0].x = pPolygon->Tx = f*(pPolygon->pNext->Px - pPolygon->Px);
    pTemp[0].y = pPolygon->Ty = f*(pPolygon->pNext->Py - pPolygon->Py);
    pTemp[0].z = pPolygon->Tz = f*(pPolygon->pNext->Pz - pPolygon->Pz);

    // set up and decompose rows 2 thru (n - 1)

    for (pPolygon = pPolygon->pNext; pPolygon->pNext; pPolygon = pPolygon->pNext)
    {
        pPolygon->c = pPolygon->w/pPolygon->d;
        r = pPolygon->pPrev->c/pPolygon->pPrev->r;
        pPolygon->r = 0.5*pPolygon->v + 2.0*(pPolygon->pPrev->c + pPolygon->c) - pPolygon->pPrev->c*r;

        f = 3.0*pPolygon->c/pPolygon->d;
        pTemp[1].x = f*(pPolygon->pNext->Px - pPolygon->Px);
        pTemp[1].y = f*(pPolygon->pNext->Py - pPolygon->Py);
        pTemp[1].z = f*(pPolygon->pNext->Pz - pPolygon->Pz);
        pPolygon->Tx = pTemp[0].x + pTemp[1].x - r*pPolygon->pPrev->Tx;
        pPolygon->Ty = pTemp[0].y + pTemp[1].y - r*pPolygon->pPrev->Ty;
        pPolygon->Tz = pTemp[0].z + pTemp[1].z - r*pPolygon->pPrev->Tz;
        pTemp[0].x = pTemp[1].x;
        pTemp[0].y = pTemp[1].y;
        pTemp[0].z = pTemp[1].z;
    }

    // set up and decompose last row

    r = pPolygon->pPrev->c/pPolygon->pPrev->r;
    pPolygon->r = 0.5*pPolygon->v + 2.0*pPolygon->pPrev->c - pPolygon->pPrev->c*r;
    pPolygon->Tx = (pTemp[0].x - r*pPolygon->pPrev->Tx)/pPolygon->r;
    pPolygon->Ty = (pTemp[0].y - r*pPolygon->pPrev->Ty)/pPolygon->r;
    pPolygon->Tz = (pTemp[0].z - r*pPolygon->pPrev->Tz)/pPolygon->r;

    // solve using back substituion

    for (pPolygon = pPolygon->pPrev; pPolygon; pPolygon = pPolygon->pPrev)
    {
        pPolygon->Tx = (pPolygon->Tx - pPolygon->c*pPolygon->pNext->Tx)/pPolygon->r;
        pPolygon->Ty = (pPolygon->Ty - pPolygon->c*pPolygon->pNext->Ty)/pPolygon->r;
        pPolygon->Tz = (pPolygon->Tz - pPolygon->c*pPolygon->pNext->Tz)/pPolygon->r;
    }

    m_dwFlags &= ~CS_RECOMPUTE;
    return true;
}

// NAME:        SetKnots()
// DESC:        set knots
// INPUT:       dwFlags..knot flags
// OUTPUT:      true.....if knots set successfully
//              false....otherwise

bool CSpline::SetKnots(DWORD dwFlags)
{
    DATAPOINT D;
    PCONTROLPOINT pPoint;

    if (0 == m_nPoints || NULL == m_pPolygon)
    {
        DPF("No control points");
        return false; 
    }
    else if (1 == m_nPoints)
    {
        m_pPolygon->d = 0.0;
        m_pPolygon->k = 0.0;
        return true;
    }
    else if (!(dwFlags & CS_SPACING))
    {
        DPF("Invalid spacing flags, attempting CS_UNIFORM");
        return SetKnots(CS_UNIFORM);
    }
    else if (dwFlags & CS_UNIFORM)
    {
        for (pPoint = m_pPolygon; pPoint; pPoint = pPoint->pNext)
            pPoint->d = 1.0;
    }   // uniform
    else if (dwFlags & CS_CHORD)
    {
        for (pPoint = m_pPolygon; pPoint->pNext; pPoint = pPoint->pNext)
        {
            D.x = pPoint->pNext->Px - pPoint->Px;
            D.y = pPoint->pNext->Py - pPoint->Py;
            D.z = pPoint->pNext->Pz - pPoint->Pz;
            pPoint->d = sqrt(D.x*D.x + D.y*D.y + D.z*D.z);

            if (pPoint->d < ZERO)
            {
                DPF("Zero length span detected, attempting CS_UNIFORM");
                return SetKnots(CS_UNIFORM);
            }
        }
    }
    else if (dwFlags & CS_CENTRIPETAL)
    {
        for (pPoint = m_pPolygon; pPoint->pNext; pPoint = pPoint->pNext)
        {
            D.x = pPoint->pNext->Px - pPoint->Px;
            D.y = pPoint->pNext->Py - pPoint->Py;
            D.z = pPoint->pNext->Pz - pPoint->Pz;
            pPoint->d = D.x*D.x + D.y*D.y + D.z*D.z;

            if (pPoint->d < ZERO)
            {
                DPF("Zero length span detected, attempting CS_UNIFORM");
                return SetKnots(CS_UNIFORM);
            }
        }
    }
    else
    {
        DPF("Unknown knot spacing requested, attempting CS_CHORD");
        SetKnots(CS_CHORD);
    }

    // set knots and spacing flags

    m_pPolygon->k = 0.0;

    for (pPoint = m_pPolygon->pNext; pPoint; pPoint = pPoint->pNext)
        pPoint->k = pPoint->pPrev->k + pPoint->pPrev->d;

    m_dwFlags &= ~CS_SPACING;
    m_dwFlags |= (dwFlags & CS_SPACING);
    m_dwFlags |= CS_RECOMPUTE;
    return true;
}

// NAME:        Debug()
// DESC:        debug message
// INPUT:       szMsg....address of null terminated message string
//              szFile...address of null terminated file string
//              nLine....line number
// OUTPUT:      none

VOID CSpline::Debug(CHAR *szMsg, CHAR *szFile, UINT nLine)
{
    CHAR szBuffer[128];

    if (szMsg)
    {
        wsprintf(szBuffer, "CSpline: %s.\n", szMsg); 
        OutputDebugString(szBuffer);
    }

    if (szFile)
    {
        wsprintf(szBuffer, "  %s(%d)\n", szFile, nLine); 
        OutputDebugString(szBuffer);
    }
}
