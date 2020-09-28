#include <math.h>
#include <d3d8.h>
#include "Cubemap.h"

// Sphere information
#define SPH_NUM 20
#define PI ((float)3.14159265358979)
#define D2R ((float)0.0174532925199433)
#define R2D ((float)57.2957795130823)
#define SPH_TRIS (2*(SPH_NUM*2)+(SPH_NUM-2)*(SPH_NUM*4))
#define SPH_VTXS (SPH_TRIS*3)
#define SPH_RADIUS (2.0f)

void CCubeTest::LoadSphere()
{
    int iSph = 0;
    FLOAT fDAngY = ((180.0F/(FLOAT)SPH_NUM)*D2R);
    FLOAT fDAngX = ((360.0F/(FLOAT)(SPH_NUM*2))*D2R);
    FLOAT fDAngY0 = fDAngY;
    FLOAT fDAngY1 = fDAngY0 + fDAngY;

	nVertexMax = SPH_VTXS;
    pTVertex = new(TVertex[nVertexMax]);

    if (bTexGen)
	    pTGVertex = new(TGVertex[nVertexMax]);

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
