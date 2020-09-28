#include <d3d9.h>
#include "MRT.h"

#define MRT_BLACK   D3DRGBA(0.0, 0.0, 0.0, 1.0)
#define MRT_GREEN   D3DRGBA(0.0, 1.0, 0.0, 0.0)
#define MRT_RED     D3DRGBA(1.0, 0.0, 0.0, 1.0)
#define MRT_CYAN	D3DRGBA(0.0, 1.0, 1.0, 1.0)
#define MRT_BLUE    D3DRGBA(0.0, 0.0, 1.0, 0.5)
#define MRT_BLUENA  D3DRGBA(0.0, 0.0, 1.0, 1.0)
#define MRT_WHITE   D3DRGBA(1.0, 1.0, 1.0, 0.0)
#define MRT_WHITENA D3DRGBA(1.0, 1.0, 1.0, 1.0)

#define MRT_FOG		D3DRGBA(0.0, 0.0, 0.0, 0.0)
#define MRT_NOFOG	D3DRGBA(0.0, 0.0, 0.0, 1.0)

void CMRT::DrawGrid(int nRTs)
{
	DWORD dwBlue = (bBlend) ? MRT_BLUE : MRT_BLUENA;

	// Initialize the Triangles
	MRT_TLVERTEX Temp0 = {              0.5f, nHeightRT   + 0.5f, 0.1f, 10.0f, (bColorWrite) ? MRT_WHITENA : MRT_RED,   MRT_NOFOG, 0.0f, 1.0f };
	MRT_TLVERTEX Temp1 = { nWidthRT/2 + 0.5f,               0.5f, 0.1f, 10.0f, (bColorWrite) ? MRT_WHITE   : MRT_GREEN, MRT_NOFOG, 0.5f, 0.0f };
	MRT_TLVERTEX Temp2 = { nWidthRT   + 0.5f, nHeightRT   + 0.5f, 0.1f, 10.0f, (bColorWrite) ? MRT_WHITENA : dwBlue,    MRT_FOG,   1.0f, 1.0f };
	MRT_TLVERTEX Temp3 = {              2.5f, nHeightRT/2 + 0.5f, 0.1f, 10.0f, MRT_CYAN, MRT_NOFOG, 0.0f, 1.0f };
	MRT_TLVERTEX Temp4 = { nWidthRT/4 + 0.5f,               2.5f, 0.1f, 10.0f, MRT_CYAN, MRT_NOFOG, 0.5f, 0.0f };
	MRT_TLVERTEX Temp5 = { nWidthRT/2 + 0.5f,               0.5f, 0.1f, 10.0f, MRT_CYAN, MRT_NOFOG, 1.0f, 1.0f };

	Tri[0] = Temp0;
	Tri[1] = Temp1;
	Tri[2] = Temp2;
	Tri[3] = Temp3;
	Tri[4] = Temp4;
	Tri[5] = Temp5;

	// Initialize the meshes
    MRT_TLVERTEX Mesh0 = { 0.0f, 0.0f, 0.1f, 10.0f, MRT_WHITENA, MRT_NOFOG, 0.0f, 1.0f };
    MRT_TLVERTEX Mesh1 = { 0.0f, 0.0f, 0.1f, 10.0f, MRT_WHITENA, MRT_NOFOG, 0.0f, 0.0f };
    MRT_TLVERTEX Mesh2 = { 0.0f, 0.0f, 0.1f, 10.0f, MRT_WHITENA, MRT_NOFOG, 1.0f, 1.0f };
    MRT_TLVERTEX Mesh3 = { 0.0f, 0.0f, 0.1f, 10.0f, MRT_WHITENA, MRT_NOFOG, 1.0f, 0.0f };

	for (int i=0; i<nRTs; i++)
	{
		switch(nRTs)
		{
			case 1: Mesh0.x =   0.0f; Mesh0.y = 279.5f;
					Mesh1.x =   0.0f; Mesh1.y =   0.0f;
					Mesh2.x = 319.5f; Mesh2.y = 279.5f;
					Mesh3.x = 319.5f; Mesh3.y =   0.0f;
					break;
			case 2: Mesh0.x = (i == 0) ?   0.0f : 160.0f; Mesh0.y = 279.5f;
					Mesh1.x = (i == 0) ?   0.0f : 160.0f; Mesh1.y =   0.0f;
					Mesh2.x = (i == 0) ? 159.5f : 319.5f; Mesh2.y = 279.5f;
					Mesh3.x = (i == 0) ? 159.5f : 319.5f; Mesh3.y =   0.0f;
					break;
			case 3: if (i == 0)
					{
						Mesh0.x =   0.0f; Mesh0.y = 139.5f;
						Mesh1.x =   0.0f; Mesh1.y =   0.0f;
						Mesh2.x = 159.5f; Mesh2.y = 139.5f;
						Mesh3.x = 159.5f; Mesh3.y =   0.0f;
					}
					if (i == 1)
					{
						Mesh0.x = 160.0f; Mesh0.y = 139.5f;
						Mesh1.x = 160.0f; Mesh1.y =   0.0f;
						Mesh2.x = 319.5f; Mesh2.y = 139.5f;
						Mesh3.x = 319.5f; Mesh3.y =   0.0f;
					}
					if (i == 2)
					{
						Mesh0.x =  80.0f; Mesh0.y = 279.5f;
						Mesh1.x =  80.0f; Mesh1.y = 140.0f;
						Mesh2.x = 239.5f; Mesh2.y = 279.5f;
						Mesh3.x = 239.5f; Mesh3.y = 140.0f;
					}
					break;
			case 4: Mesh0.x = (i%2 == 0) ?   0.0f : 160.0f; Mesh0.y = (i/2 == 0) ? 139.5f : 279.5f;
				    Mesh1.x = (i%2 == 0) ?   0.0f : 160.0f; Mesh1.y = (i/2 == 0) ?   0.0f : 140.0f;
					Mesh2.x = (i%2 == 0) ? 159.5f : 319.5f; Mesh2.y = (i/2 == 0) ? 139.5f : 279.5f;
					Mesh3.x = (i%2 == 0) ? 159.5f : 319.5f; Mesh3.y = (i/2 == 0) ?   0.0f : 140.0f;
					break;
		}

		Quad[i][0] = Mesh0;
		Quad[i][1] = Mesh1;
		Quad[i][2] = Mesh2;
		Quad[i][3] = Mesh3;
	}
}
