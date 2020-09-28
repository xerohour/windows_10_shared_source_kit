// FILE:        CSpline.h
// DESC:        spline class definitions
// AUTHOR:      Todd M. Frost
// COMMENTS:    weighted v-spline interpolation

#ifndef _CSPLINE_
#define _CSPLINE_

#include <windows.h>

const DWORD CS_UNIFORM     = 0x00000001;
const DWORD CS_CHORD       = 0x00000002;
const DWORD CS_CENTRIPETAL = 0x00000004;
const DWORD CS_SPACING     = 0x00000007;

const DWORD CS_FIXED       = 0x40000000;

const DWORD CS_RECOMPUTE   = 0x80000000;

typedef struct _DataPoint
{
    double x;
    double y;
    double z;
} DATAPOINT, *PDATAPOINT;

typedef struct _ControlPoint
{
    double Px;                   // x position
    double Py;		             // y position
    double Pz;		             // z position
    double Tx;		             // x tangent
    double Ty;		             // y tangent 
    double Tz;		             // z tangent
    double w;		             // weight (per interval)
    double v;	            	 // tension
    double r;                    // ratio factor
    double c;                    // coefficient factor
    double k;                    // knot
    double d;		             // forward difference (k[i+1] - k[i])
    DWORD dwFlags;               // flags
    struct _ControlPoint *pPrev; // prev control point
    struct _ControlPoint *pNext; // next control point
} CONTROLPOINT, *PCONTROLPOINT;

typedef VOID (*_PlotDataHandler)(UINT uCount, PDATAPOINT pPoints);

class CSpline
{
    private:
        DWORD m_dwFlags;          // flags
        UINT m_nPoints;           // number of control polygon points
        PCONTROLPOINT m_pPolygon; // control polygon
		
    public:
        CSpline();
        ~CSpline();

        bool GetKnot(UINT i, double *pData);
        bool GetTension(UINT i, double *pData);
        bool GetWeight(UINT i, double *pData);
        bool GetPosition(UINT i, PDATAPOINT pData);
        DWORD GetSpacing(VOID) { return (m_dwFlags & CS_SPACING); };
        UINT GetInterval(double u);

        bool SetTension(UINT i, double data);
        bool SetWeight(UINT i, double data);
        bool SetPosition(UINT i, PDATAPOINT pData);
        DWORD SetSpacing(DWORD dwFlags);

        bool FixTangent(UINT i, PDATAPOINT pData);
        bool FreeTangent(UINT i);

        UINT GetControlPoints(VOID) { return m_nPoints; };
        bool SetControlPoints(UINT nPoints, PDATAPOINT pData = NULL);
        bool AppendControlPoint(UINT i, PDATAPOINT pData = NULL);
        bool DeleteControlPoint(UINT i);

        bool Position(double u, PDATAPOINT pData, bool bUnit = false);
        bool Tangent(double u, PDATAPOINT pData, bool bUnit = false);
        bool Derivative(double u, PDATAPOINT pData, bool bUnit = false);

        bool Nearest(PDATAPOINT pData);
        PDATAPOINT Linearize(UINT *puCount, double angle);

        VOID Plot(UINT nEval, PDATAPOINT pPoints, _PlotDataHandler Handler);

    private:
        PCONTROLPOINT GetControlPoint(UINT i);
        VOID GetBasis(double t, double *pB0, double *pB1, double *pD0, double *pD1);
        VOID GetBasisDerivative(double t, double *pDB0, double *pDB1, double *pDD0, double *pDD1);
        bool SetCoefficients(DWORD dwFlags);
        bool SetKnots(DWORD dwFlags);
        VOID Debug(char *szMsg, char *szFile, UINT nLine);
};

#endif
