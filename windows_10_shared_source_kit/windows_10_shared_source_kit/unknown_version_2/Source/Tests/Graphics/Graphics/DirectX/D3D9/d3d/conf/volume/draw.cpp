#include <math.h>
#include <d3d8.h>
#include "Volume.h"

// Sphere information
#define SPH_NUM 20
#define PI ((float)3.14159265358979)
#define D2R ((float)0.0174532925199433)
#define R2D ((float)57.2957795130823)
#define SPH_TRIS (2*(SPH_NUM*2)+(SPH_NUM-2)*(SPH_NUM*4))
#define SPH_VTXS (SPH_TRIS*3)
#define SPH_RADIUS (2.0f)

void CVolumeTest::LoadGrid(float u0, float v0, float w0, float u1, float v1, float w1)
{
    float fZeroU = u0;
    float fZeroV = v0;
    float fZeroW = w0;
    float fOneU = u1;
    float fOneV = v1;
    float fOneW = w1;

	VertexList[0].x = 9.5f;
	VertexList[0].y = 9.5f;
	VertexList[0].z = 0.9f;
	VertexList[0].rhw = 1.0f / 0.9f;
	VertexList[0].color = 0xffffffff;
	VertexList[0].tu = fZeroU;
	VertexList[0].tv = fZeroV;
	VertexList[0].tw = fZeroW;

	VertexList[1].x = 265.5f;
	VertexList[1].y = 9.5f;
	VertexList[1].z = 0.9f;
	VertexList[1].rhw = 1.0f / 0.9f;
	VertexList[1].color = 0xffffffff;
	VertexList[1].tu = fOneU;
	VertexList[1].tv = fZeroV;
	VertexList[1].tw = fOneW;

	VertexList[2].x = 265.5f;
	VertexList[2].y = 265.5f;
	VertexList[2].z = 0.9f;
	VertexList[2].rhw = 1.0f / 0.9f;
	VertexList[2].color = 0xffffffff;
	VertexList[2].tu = fOneU;
	VertexList[2].tv = fOneV;
	VertexList[2].tw = fOneW;

	VertexList[3].x = 9.5f;
	VertexList[3].y = 9.5f;
	VertexList[3].z = 0.9f;
	VertexList[3].rhw = 1.0f / 0.9f;
	VertexList[3].color = 0xffffffff;
	VertexList[3].tu = fZeroU;
	VertexList[3].tv = fZeroV;
	VertexList[3].tw = fZeroW;

	VertexList[4].x = 265.5f;
	VertexList[4].y = 265.5f;
	VertexList[4].z = 0.9f;
	VertexList[4].rhw = 1.0f / 0.9f;
	VertexList[4].color = 0xffffffff;
	VertexList[4].tu = fOneU;
	VertexList[4].tv = fOneV;
	VertexList[4].tw = fOneW;

	VertexList[5].x = 9.5f;
	VertexList[5].y = 265.5f;
	VertexList[5].z = 0.9f;
	VertexList[5].rhw = 1.0f / 0.9f;
	VertexList[5].color = 0xffffffff;
	VertexList[5].tu = fZeroU;
	VertexList[5].tv = fOneV;
	VertexList[5].tw = fZeroW;
}

void CVolumeTest::LoadSphere(void)
{
    int iSph = 0;
    FLOAT fDAngY = ((180.0F/(FLOAT)SPH_NUM)*D2R);
    FLOAT fDAngX = ((360.0F/(FLOAT)(SPH_NUM*2))*D2R);
    FLOAT fDAngY0 = fDAngY;
    FLOAT fDAngY1 = fDAngY0 + fDAngY;

	nVertexMax=SPH_VTXS;
    pTVertex=new(TVertex[nVertexMax]);

    if (bTexGen)
	    pTGVertex=new(TGVertex[nVertexMax]);

    // make middle
    int x, y;
    for(y = 0; y < (SPH_NUM-2); y++) {
        FLOAT y0 = (FLOAT)cos(fDAngY0);
        FLOAT y1 = (FLOAT)cos(fDAngY1);
        FLOAT Rad0 = (FLOAT)sin(fDAngY0);
        FLOAT Rad1 = (FLOAT)sin(fDAngY1);

        for(x = 0; x < (SPH_NUM*2); x++) {
            FLOAT fDAngX0 = (FLOAT)x*fDAngX;
            FLOAT fDAngX1;
            if (x == (SPH_NUM*2-1))
                fDAngX1 = 0.0;
            else
                fDAngX1 = (FLOAT)(x+1)*fDAngX;

            FLOAT x00 = Rad0*(FLOAT)sin(fDAngX0);
            FLOAT x01 = Rad0*(FLOAT)sin(fDAngX1);
            FLOAT x10 = Rad1*(FLOAT)sin(fDAngX0);
            FLOAT x11 = Rad1*(FLOAT)sin(fDAngX1);

            FLOAT z00 = Rad0*(FLOAT)cos(fDAngX0);
            FLOAT z01 = Rad0*(FLOAT)cos(fDAngX1);
            FLOAT z10 = Rad1*(FLOAT)cos(fDAngX0);
            FLOAT z11 = Rad1*(FLOAT)cos(fDAngX1);

            pTVertex[iSph].v.x = SPH_RADIUS*x00;  pTVertex[iSph].v.y = SPH_RADIUS*y0;  pTVertex[iSph].v.z = SPH_RADIUS*z00;
            pTVertex[iSph].n.x = x00; pTVertex[iSph].n.y = y0; pTVertex[iSph].n.z = z00;
            iSph++;

            pTVertex[iSph].v.x = SPH_RADIUS*x10;  pTVertex[iSph].v.y = SPH_RADIUS*y1;  pTVertex[iSph].v.z = SPH_RADIUS*z10;
            pTVertex[iSph].n.x = x10; pTVertex[iSph].n.y = y1; pTVertex[iSph].n.z = z10;
            iSph++;

            pTVertex[iSph].v.x = SPH_RADIUS*x11;  pTVertex[iSph].v.y = SPH_RADIUS*y1;  pTVertex[iSph].v.z = SPH_RADIUS*z11;
            pTVertex[iSph].n.x = x11; pTVertex[iSph].n.y = y1; pTVertex[iSph].n.z = z11;
            iSph++;


            pTVertex[iSph].v.x = SPH_RADIUS*x00;  pTVertex[iSph].v.y = SPH_RADIUS*y0;  pTVertex[iSph].v.z = SPH_RADIUS*z00;
            pTVertex[iSph].n.x = x00; pTVertex[iSph].n.y = y0; pTVertex[iSph].n.z = z00;
            iSph++;

            pTVertex[iSph].v.x = SPH_RADIUS*x11;  pTVertex[iSph].v.y = SPH_RADIUS*y1;  pTVertex[iSph].v.z = SPH_RADIUS*z11;
            pTVertex[iSph].n.x = x11; pTVertex[iSph].n.y = y1; pTVertex[iSph].n.z = z11;
            iSph++;

            pTVertex[iSph].v.x = SPH_RADIUS*x01;  pTVertex[iSph].v.y = SPH_RADIUS*y0;  pTVertex[iSph].v.z = SPH_RADIUS*z01;
            pTVertex[iSph].n.x = x01; pTVertex[iSph].n.y = y0; pTVertex[iSph].n.z = z01;
            iSph++;

        }
        fDAngY0 = fDAngY1;
        fDAngY1 += fDAngY;
    }

    // make top
    fDAngY1 = fDAngY;
    FLOAT y1 = (FLOAT)cos(fDAngY1);
    FLOAT Rad1 = (FLOAT)sin(fDAngY1);
    for(x = 0; x < (SPH_NUM*2); x++) {
        FLOAT fDAngX0 = (FLOAT)x*fDAngX;
        FLOAT fDAngX1;
        if (x == (SPH_NUM*2-1))
            fDAngX1 = 0.0F;
        else
            fDAngX1 = (FLOAT)(x+1)*fDAngX;

        FLOAT x10 = Rad1*(FLOAT)sin(fDAngX0);
        FLOAT x11 = Rad1*(FLOAT)sin(fDAngX1);

        FLOAT z10 = Rad1*(FLOAT)cos(fDAngX0);
        FLOAT z11 = Rad1*(FLOAT)cos(fDAngX1);

        pTVertex[iSph].v.x = 0.0;  pTVertex[iSph].v.y = SPH_RADIUS;  pTVertex[iSph].v.z = 0.0;
        pTVertex[iSph].n.x = 0.0; pTVertex[iSph].n.y = 1.0; pTVertex[iSph].n.z = 0.0;
        iSph++;

        pTVertex[iSph].v.x = SPH_RADIUS*x10;  pTVertex[iSph].v.y = SPH_RADIUS*y1;  pTVertex[iSph].v.z = SPH_RADIUS*z10;
        pTVertex[iSph].n.x = x10; pTVertex[iSph].n.y = y1; pTVertex[iSph].n.z = z10;
        iSph++;

        pTVertex[iSph].v.x = SPH_RADIUS*x11;  pTVertex[iSph].v.y = SPH_RADIUS*y1;  pTVertex[iSph].v.z = SPH_RADIUS*z11;
        pTVertex[iSph].n.x = x11; pTVertex[iSph].n.y = y1; pTVertex[iSph].n.z = z11;
        iSph++;
    }

    // make bottom
    fDAngY1 = fDAngY0;          // remember last value used, so there are no cracks
    y1 = (FLOAT)cos(fDAngY1);
    Rad1 = (FLOAT)sin(fDAngY1);
    for(x = 0; x < (SPH_NUM*2); x++) {
        FLOAT fDAngX0 = (FLOAT)x*fDAngX;
        FLOAT fDAngX1;
        if (x == (SPH_NUM*2-1))
            fDAngX1 = 0.0F;
        else
            fDAngX1 = (FLOAT)(x+1)*fDAngX;

        // to keep the same orientation
        FLOAT x11 = Rad1*(FLOAT)sin(fDAngX0);
        FLOAT x10 = Rad1*(FLOAT)sin(fDAngX1);

        FLOAT z11 = Rad1*(FLOAT)cos(fDAngX0);
        FLOAT z10 = Rad1*(FLOAT)cos(fDAngX1);

        pTVertex[iSph].v.x = 0.0;  pTVertex[iSph].v.y = -SPH_RADIUS;  pTVertex[iSph].v.z = 0.0;
        pTVertex[iSph].n.x = 0.0; pTVertex[iSph].n.y = -1.0; pTVertex[iSph].n.z = 0.0;
        iSph++;

        pTVertex[iSph].v.x = SPH_RADIUS*x10;  pTVertex[iSph].v.y = SPH_RADIUS*y1;  pTVertex[iSph].v.z = SPH_RADIUS*z10;
        pTVertex[iSph].n.x = x10; pTVertex[iSph].n.y = y1; pTVertex[iSph].n.z = z10;
        iSph++;

        pTVertex[iSph].v.x = SPH_RADIUS*x11;  pTVertex[iSph].v.y = SPH_RADIUS*y1;  pTVertex[iSph].v.z = SPH_RADIUS*z11;
        pTVertex[iSph].n.x = x11; pTVertex[iSph].n.y = y1; pTVertex[iSph].n.z = z11;
        iSph++;
    }

    if (bTexGen)
    {
    	for (int i=0;i<nVertexMax;i++)
        {
	    	pTGVertex[i].v = pTVertex[i].v;
	    	pTGVertex[i].n = pTVertex[i].n;
        }
    }
    else
    {
    	for (int i=0;i<nVertexMax;i++)
	    	pTVertex[i].t = pTVertex[i].n;
    }
}
