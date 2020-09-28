//////////////////////////////////////////////////////////////////////
// Matrix.h: interface for the CMatrix class.
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#define D3D_OVERLOADS
#include <d3d.h>

#define PI ((float)3.14159265358979)

class CMatrix  
{
public:
	CMatrix();
	virtual ~CMatrix();

	void Clear();
	void Identity();
	void RotatePitch(float Deg);
	void RotateYaw(float Deg);
	void RotateRoll(float Deg);
	void Translate(D3DVECTOR Position);

	D3DMATRIX m_Matrix;
};
