// FILE:        user.cpp
// DESC:        user clip class methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:

#include <math.h>
#include "sprite.h"

// NAME:        CSpriteUClip()
// DESC:        constructor for sprite user clip plane class
// INPUT:       none
// OUTPUT:      none

CSpriteUClip::CSpriteUClip()
{
    m_szTestName="User clip planes";
    m_szCommandKey="UClip";

    m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_HWVP | DEVICECREATE_PURE;

    m_Group.Replace(0);
    m_Group.Save();

}

// NAME:        ~CSpriteUClip()
// DESC:        destructor for sprite user clip plane class
// INPUT:       none
// OUTPUT:      none

CSpriteUClip::~CSpriteUClip()
{
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteUClip::TestInitialize(VOID)
{
    D3DXVECTOR3 P, N, Z(0.0f, 0.0f, 1.0f);
    MYD3DLVERTEX A, B;
    UINT uResult;

    if (0 == m_pSrcDevice->GetCaps()->wMaxUserClipPlanes)
    {
        WriteToLog("Src device does not support user defined clipping planes.\n");
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }

    if (0 == m_pRefDevice->GetCaps()->wMaxUserClipPlanes)
    {
        WriteToLog("Ref device does not support user defined clipping planes.\n");
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }
	
    uResult = CSprite::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    m_nVariationsPerFormat = NPLANES;
    SetTestRange((UINT) 1, m_nVariationsPerFormat*m_uCommonTextureFormats);
    Clear(m_dcBackground);
    CSprite::SetViewport(CLIPBORDER,
                         CLIPBORDER,
                         m_pCurrentMode->nBackBufferWidth - (CLIPBORDER*2),
                         m_pCurrentMode->nBackBufferHeight - (CLIPBORDER*2));
    m_Sprite.PointSize.f = 64.0f;// MIN_MAXPOINTSIZE;	

    memset(m_Verts, 0, sizeof(m_Verts));
   
    for (int i = 0; i < 4; i++)
        m_Verts[i].dvZ = (float) EPSILON;

    SetPositionWorld(TOPLEFT_INSIDE, &m_Verts[0].dvX, &m_Verts[0].dvY);
    SetPositionWorld(BOTLEFT_INSIDE, &m_Verts[1].dvX, &m_Verts[1].dvY);
    SetPositionWorld(TOPRIGHT_INSIDE, &m_Verts[2].dvX, &m_Verts[2].dvY);
    SetPositionWorld(BOTRIGHT_INSIDE, &m_Verts[3].dvX, &m_Verts[3].dvY);

    m_Verts[0].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
    m_Verts[0].dcSpecular = m_dcBackground;
    m_Verts[1].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(0,255,0,255);
    m_Verts[1].dcSpecular = m_dcBackground;
    m_Verts[2].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(0,0,255,255);
    m_Verts[2].dcSpecular = m_dcBackground;
    m_Verts[3].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(0,0,0,255);
    m_Verts[3].dcSpecular = m_dcBackground;
	
    memset(&A, 0, sizeof(A));
    memset(&B, 0, sizeof(B));
    A.dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(0,0,255,255);
    A.dcSpecular = m_dcBackground;
    B.dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(0,0,255,255);
    B.dcSpecular = m_dcBackground;

    // vertical plane

    SetPositionWorld(TOPCENTER_INSIDE, &A.dvX, &A.dvY);
    SetPositionWorld(BOTCENTER_INSIDE, &B.dvX, &B.dvY);
    A.dvZ = (float) EPSILON;
    B.dvZ = (float) EPSILON;
    P.x = B.dvX - A.dvX;
    P.y = B.dvY - A.dvY;
    P.z = 0.0f;
    GetUnitNormal(&P, &Z, &N);

    m_Planes[0][0] = N.x;
    m_Planes[0][1] = N.y;
    m_Planes[0][2] = N.z;
    m_Planes[0][3] = -((N.x*A.dvX) + (N.y*A.dvY) + (N.z*A.dvZ) - EPSILON);
    m_Lines[0][0] = A;
    m_Lines[0][1] = B;

    m_Planes[1][0] = -m_Planes[0][0];
    m_Planes[1][1] = -m_Planes[0][1];
    m_Planes[1][2] = -m_Planes[0][2];
    m_Planes[1][3] = -m_Planes[0][3] + (2*EPSILON);
    m_Lines[1][0] = A;
    m_Lines[1][1] = B;

	// horizontal plane

    SetPositionWorld(RIGHTCENTER_INSIDE, &A.dvX, &A.dvY);
    SetPositionWorld(LEFTCENTER_INSIDE, &B.dvX, &B.dvY);
    A.dvZ = (float) EPSILON;
    B.dvZ = (float) EPSILON;
    P.x = B.dvX - A.dvX;
    P.y = B.dvY - A.dvY;
    P.z = 0.0f;
    GetUnitNormal(&P, &Z, &N);

    m_Planes[2][0] = N.x;
    m_Planes[2][1] = N.y;
    m_Planes[2][2] = N.z;
    m_Planes[2][3] = -((N.x*A.dvX) + (N.y*A.dvY) + (N.z*A.dvZ) - EPSILON);
    m_Lines[2][0] = A;
    m_Lines[2][1] = B;

    m_Planes[3][0] = -m_Planes[2][0];
    m_Planes[3][1] = -m_Planes[2][1];
    m_Planes[3][2] = -m_Planes[2][2];
    m_Planes[3][3] = -m_Planes[2][3] + (2*EPSILON);
    m_Lines[3][0] = A;
    m_Lines[3][1] = B;

    DEVICEDESC *pRefDesc, *pSrcDesc; // device description after vertex processing behavior applied
    pRefDesc = m_pRefDevice->GetCaps();
    pSrcDesc = m_pSrcDevice->GetCaps();

    if (pSrcDesc->dwPrimitiveMiscCaps & D3DPMISCCAPS_CLIPPLANESCALEDPOINTS)
        WriteToLog("Src device supports D3DPMISCCAPS_CLIPPLANESCALEDPOINTS\n");
    else
        WriteToLog("Src device does NOT support D3DPMISCCAPS_CLIPPLANESCALEDPOINTS\n");
    
    if (pRefDesc->dwPrimitiveMiscCaps & D3DPMISCCAPS_CLIPPLANESCALEDPOINTS)
        WriteToLog("Ref device supports D3DPMISCCAPS_CLIPPLANESCALEDPOINTS\n");
    else
        WriteToLog("Ref device does NOT support D3DPMISCCAPS_CLIPPLANESCALEDPOINTS\n");

    if ((pSrcDesc->dwPrimitiveMiscCaps & D3DPMISCCAPS_CLIPPLANESCALEDPOINTS) &&
        (pRefDesc->dwPrimitiveMiscCaps & D3DPMISCCAPS_CLIPPLANESCALEDPOINTS))
    {
        // back slash

        SetPositionWorld(TOPLEFT_INSIDE, &A.dvX, &A.dvY);
        SetPositionWorld(BOTRIGHT_INSIDE, &B.dvX, &B.dvY);
        P.x = B.dvX - A.dvX;
        P.y = B.dvY - A.dvY;
        P.z = 0.0f;
        GetUnitNormal(&P, &Z, &N);
		m_Planes[4][0] = N.x;
		m_Planes[4][1] = N.y;
		m_Planes[4][2] = N.z;
		m_Planes[4][3] = -((N.x*A.dvX) + (N.y*A.dvY) + (N.z*A.dvZ) - EPSILON);
		m_Lines[4][0] = A;
		m_Lines[4][1] = B;
		m_Planes[5][0] = -m_Planes[4][0];
		m_Planes[5][1] = -m_Planes[4][1];
		m_Planes[5][2] = -m_Planes[4][2];
		m_Planes[5][3] = -(m_Planes[4][3]-(2*EPSILON));
		m_Lines[5][0] = A;
		m_Lines[5][1] = B;

		//forward slash

		SetPositionWorld(TOPRIGHT_INSIDE, &A.dvX, &A.dvY);
		SetPositionWorld(BOTLEFT_INSIDE, &B.dvX, &B.dvY);
		P.x = B.dvX - A.dvX;
		P.y = B.dvY - A.dvY;
        P.z = 0.0f;
		GetUnitNormal(&P, &Z, &N);
		m_Planes[6][0] = N.x;
		m_Planes[6][1] = N.y;
		m_Planes[6][2] = N.z;
		m_Planes[6][3] = -((N.x*A.dvX) + (N.y*A.dvY) + (N.z*A.dvZ) - EPSILON);
		m_Lines[6][0] = A;
		m_Lines[6][1] = B;
		m_Planes[7][0] = -m_Planes[6][0];
		m_Planes[7][1] = -m_Planes[6][1];
		m_Planes[7][2] = -m_Planes[6][2];
		m_Planes[7][3] = -(m_Planes[6][3]-(2*EPSILON));
		m_Lines[7][0] = A;
		m_Lines[7][1] = B;
	} // ensure user clip planes intersect with point sprites
	else
	{
		float fSpriteWorldSize = m_Sprite.PointSize.f*CalcWorldPixelSize()*0.75f;		

		// greater than (size/2)*sqrt(2) (i.e. diagnal of point sprite) 
		
		// back slash

		SetPositionWorld(TOPLEFT_INSIDE, &A.dvX, &A.dvY);
		SetPositionWorld(BOTRIGHT_INSIDE, &B.dvX, &B.dvY);
		P.x = B.dvX - A.dvX;
		P.y = B.dvY - A.dvY;
        P.z = 0.0f;
		GetUnitNormal(&P, &Z, &N);
		m_Planes[4][0] = N.x;
		m_Planes[4][1] = N.y;
		m_Planes[4][2] = N.z;
		A.dvX += (N.x*fSpriteWorldSize);
		A.dvY += (N.y*fSpriteWorldSize);
		A.dvZ += (N.z*fSpriteWorldSize);
		B.dvX += (N.x*fSpriteWorldSize);
		B.dvY += (N.y*fSpriteWorldSize);
		B.dvZ += (N.z*fSpriteWorldSize);
		m_Planes[4][3] = -((N.x*A.dvX) + (N.y*A.dvY) + (N.z*A.dvZ) - EPSILON);
		m_Lines[4][0] = A;
		m_Lines[4][1] = B;

		SetPositionWorld(TOPLEFT_INSIDE, &A.dvX, &A.dvY);
		SetPositionWorld(BOTRIGHT_INSIDE, &B.dvX, &B.dvY);
		N.x = -N.x;
		N.y = -N.y;
		N.z = -N.z;
		m_Planes[5][0] = N.x;
		m_Planes[5][1] = N.y;
		m_Planes[5][2] = N.z;
		A.dvX += (N.x*fSpriteWorldSize);
		A.dvY += (N.y*fSpriteWorldSize);
		A.dvZ += (N.z*fSpriteWorldSize);
		B.dvX += (N.x*fSpriteWorldSize);
		B.dvY += (N.y*fSpriteWorldSize);
		B.dvZ += (N.z*fSpriteWorldSize);		
		m_Planes[5][3] = -((N.x*A.dvX) + (N.y*A.dvY) + (N.z*A.dvZ) - EPSILON);
		m_Lines[5][0] = A;
		m_Lines[5][1] = B;

		// forward slash

		SetPositionWorld(TOPRIGHT_INSIDE, &A.dvX, &A.dvY);
		SetPositionWorld(BOTLEFT_INSIDE, &B.dvX, &B.dvY);
		P.x = B.dvX - A.dvX;
		P.y = B.dvY - A.dvY;
        P.z = 0.0f;
		GetUnitNormal(&P, &Z, &N);
		m_Planes[6][0] = N.x;
		m_Planes[6][1] = N.y;
		m_Planes[6][2] = N.z;
		A.dvX += (N.x*fSpriteWorldSize);
		A.dvY += (N.y*fSpriteWorldSize);
		A.dvZ += (N.z*fSpriteWorldSize);
		B.dvX += (N.x*fSpriteWorldSize);
		B.dvY += (N.y*fSpriteWorldSize);
		B.dvZ += (N.z*fSpriteWorldSize);
		m_Planes[6][3] = -((N.x*A.dvX) + (N.y*A.dvY) + (N.z*A.dvZ) - EPSILON);
		m_Lines[6][0] = A;
		m_Lines[6][1] = B;

		SetPositionWorld(TOPRIGHT_INSIDE, &A.dvX, &A.dvY);
		SetPositionWorld(BOTLEFT_INSIDE, &B.dvX, &B.dvY);
		N.x = -N.x;
		N.y = -N.y;
		N.z = -N.z;
		m_Planes[7][0] = N.x;
		m_Planes[7][1] = N.y;
		m_Planes[7][2] = N.z;
		A.dvX += (N.x*fSpriteWorldSize);
		A.dvY += (N.y*fSpriteWorldSize);
		A.dvZ += (N.z*fSpriteWorldSize);
		B.dvX += (N.x*fSpriteWorldSize);
		B.dvY += (N.y*fSpriteWorldSize);
		B.dvZ += (N.z*fSpriteWorldSize);
		m_Planes[7][3] = -((N.x*A.dvX) + (N.y*A.dvY) + (N.z*A.dvZ) - EPSILON);
		m_Lines[7][0] = A;
		m_Lines[7][1] = B;
	} // ensure user clip planes do NOT intersect with point sprites

	return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSpriteUClip::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    UINT nPlane;

	if (!CSprite::ExecuteTest(uTest))
        return false;

    sprintf(m_szBuffer, "User clip plane, Format %s", m_szFormat);
    BeginTestCase(m_szBuffer, uTest);

	if (BeginScene())
    {
		nPlane = (uTest - 1)%NPLANES;
		SetRenderState(D3DRS_POINTSPRITEENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_CLIPPLANEENABLE, (DWORD) D3DCLIPPLANE0);
		SetClipPlane(0, &m_Planes[nPlane][0]);
		
        if (!RenderPrimitive(D3DPT_POINTLIST, MYD3DFVF_LVERTEX, &m_Verts, (DWORD) 4, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() failed.\n");
            bResult = false;
        }

		if (!RenderPrimitive(D3DPT_LINELIST, MYD3DFVF_LVERTEX, &m_Lines[nPlane][0], (DWORD) 2, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() failed.\n");
            bResult = false;
        }
		
        if (!EndScene())
        {
            WriteToLog("EndScene() failed.\n");
            bResult = false;
        }
    }
    else
    {
        WriteToLog("BeginScene() failed.\n");
        bResult = false;
    }

	return bResult;
}

// NAME:        GetUnitNormal()
// DESC:        get unit normal
// INPUT:       pA...address of D3DXVECTOR3 vector
//              pB...address of D3DXVECTOR3 vector
//              pC...address of D3DXVECTOR3 unit normal from AxB
// OUTPUT:      none

void CSpriteUClip::GetUnitNormal(D3DXVECTOR3 *pA, D3DXVECTOR3 *pB, D3DXVECTOR3 *pC)
{
    float fSize;

    pC->x = (pA->y*pB->z) - (pA->z*pB->y);
    pC->y = (pA->z*pB->x) - (pA->x*pB->z);
    pC->z = (pA->x*pB->y) - (pA->y*pB->x);
    fSize = sqrt(pC->x*pC->x + pC->y*pC->y + pC->z*pC->z);

    if (0 == fSize)
    {
        WriteToLog("CSpriteUClip::GetUnitNormal(): Unable to compute unit normal.\n");
        return;
    }

    pC->x /= fSize;
    pC->y /= fSize;
    pC->z /= fSize;
}
