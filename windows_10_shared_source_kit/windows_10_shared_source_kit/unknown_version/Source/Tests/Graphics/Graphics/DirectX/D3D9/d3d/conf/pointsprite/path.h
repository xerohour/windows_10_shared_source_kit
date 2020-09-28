// FILE:        path.h
// DESC:        path class header for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:

#define DEFAULT_FACTOR 2.0
#define DEFAULT_SCALE 1.0

#define PATH_EPSILON 1.0e-3

typedef enum _PATHTYPE
{
    PATH_LINE = 1,
	PATH_CIRCLE = 2,
	PATH_LEMNISCATE = 3,
    FORCE_DWORD = 0xffffffff
} PATHTYPE;

class CPath
{
    private:    
        float m_fFactor;      // lemniscate factor
    	float m_fLength;      // line length
        float m_fRadius;      // circle radius
        float m_fScale;       // lemniscate scale
    	D3DXVECTOR3 m_Start;  // line start
        D3DXVECTOR3 m_End;    // line end
    	D3DXVECTOR3 m_Center; // circle or lemniscate center
    	D3DXVECTOR3 m_Normal; // normal of circle or lemniscate plane
        D3DXMATRIX m_Matrix;  // matrix
	    PATHTYPE m_Type;      // path type
    	UINT m_nMaxSteps;     // max number of steps thru path

    public:
        CPath();
        ~CPath();
        void Line(LPD3DXVECTOR3 pStart, LPD3DXVECTOR3 pEnd, UINT nSteps);
        void Circle(LPD3DXVECTOR3 pCenter, float fRadius, UINT nSteps);
        void Lemniscate(LPD3DXVECTOR3 pCenter, float fScale, float fFactor, UINT nSteps);
        void SetCurrentRotXYZ(float fRadX, float fRadY, float fRadZ);
        bool GetStep(int nStep, LPD3DXVECTOR3 pOrigin, LPD3DXVECTOR3 pX, LPD3DXVECTOR3 pY, LPD3DXVECTOR3 pZ);
};

