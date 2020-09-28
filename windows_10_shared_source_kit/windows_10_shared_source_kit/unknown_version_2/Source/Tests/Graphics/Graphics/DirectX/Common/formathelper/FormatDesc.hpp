 (m_Port.Left() - 1)     , (float) (m_Port.CenterY())        },
        { (float) (m_Port.Left() - fSize) , (float) (m_Port.Top() - fSize)    }, // outside
        { (float) (m_Port.CenterX())      , (float) (m_Port.Top() - fSize)    },
        { (float) (m_Port.Right() + fSize), (float) (m_Port.Top() - fSize)    },
        { (float) (m_Port.Right() + fSize), (float) (m_Port.CenterY())        },
        { (float) (m_Port.Right() + fSize), (float) (m_Port.Bottom() + fSize) },
        { (float) (m_Port.CenterX())      , (float) (m_Port.Bottom() + fSize) },
        { (float) (m_Port.Left() - fSize) , (float) (m_Port.Bottom() + fSize) },
        { (float) (m_Port.Left() - fSize) , (float) (m_Port.CenterY())        },
    };
    UINT n = (nPosition < NCLIPTESTS) ? nPosition : 0;

    *pfX = fPosition[n][0];
    *pfY = fPosition[n][1];
}

// NAME:        SetPositionWorld()
// DESC:        Sets the world co-ordinate(x,y)
// INPUT:       nPosition....position index
//              pfX..........x coordinate
//              pfY..........y coordinate
// OUTPUT:      pfX and pfY set to requested world coords for nPosition

void CSprite::SetPositionWorld(UINT nPosition, float *pfX, float *pfY)
{
    D3DXVECTOR3 dvectPos;

    SetPosition(nPosition, pfX, pfY);
    dvectPos.x = *pfX;
    dvectPos.y = *pfY;
    dvectPos.z = 0.0f;
    AdjustVertex(&dvectPos);
    *pfX = dvectPos.x;
    *pfY = dvectPos.y;
}

// NAME:        AdjustVertex()
// DESC:        Adjust vertex (x,y) to avoid precision errors in sprite placement
// INPUT:       pVector..screen coords to be adjusted
// OUTPUT:      true.....if vertex is appropriately adjusted
//              false....otherwise
// COMMENTS:    point size must be an even number in order to avoid all possible precision issues
//              (see AdjustVertexAttenuated()

bool CSprite::AdjustVertex(D3DXVECTOR3 *pVector)
{
    bool bSuccess = true, bIncX = true, bIncY = true;
    float fEpsilon, fPosInPixel, fWorldPixelSize;
    UINT i;

    if (pVector->x < m_Port.Left())
        bIncX = false;

    if (pVector->y < m_Port.Top())
        bIncY = false;

    D3DXVec3TransformCoord(pVector, pVector, &m_ScreenToWorld);
    fWorldPixelSize = CalcWorldPixelSize();
    fEpsilon = fWorldPixelSize/10.0f;

    for (i = 0; i < MAX_ADJUSTMENTS; i++)
    {
        fPosInPixel = pVector->x/fWorldPixelSize;
        fPosInPixel = fPosInPixel - (long) fPosInPixel;
        fPosInPixel *= fWorldPixelSize;
        fPosInPixel = fabs(fPosInPixel);

        if ((fPosInPixel < fEpsilon) ||
             (fPosInPixel > (fWorldPixelSize - fEpsilon)) ||
             ((fPosInPixel > (0.5f*fWorldPixelSize - fEpsilon)) &&
              (fPosInPixel < (0.5f*fWorldPixelSize + fEpsilon))))
        {
            if (bIncX)
                pVector->x += (float) fEpsilon;
            else
                pVector->x -= (float) fEpsilon;
        }
        else
            break;
    }

    bSuccess = (MAX_ADJUSTMENTS == i) ? false : bSuccess;
    
    for (i = 0; i < MAX_ADJUSTMENTS; i++)
    {
        fPosInPixel = pVector->y/fWorldPixelSize;
        fPosInPixel = fPosInPixel - (long) fPosInPixel;
        fPosInPixel *= fWorldPixelSize;
        fPosInPixel = fabs(fPosInPixel);

        if ((fPosInPixel < fEpsilon) ||
            (fPosInPixel > (fWorldPixelSize - fEpsilon)) ||
            ((fPosInPixel > (0.5f*fWorldPixelSize - fEpsilon)) &&
             (fPosInPixel < (0.5f*fWorldPixelSize + fEpsilon))))
        {
            if (bIncY)
                pVector->y += (float) fEpsilon;
            else
                pVector->y -= (float) fEpsilon;
        }
        else
            break;
    }

    bSuccess = (MAX_ADJUSTMENTS == i) ? false : bSuccess;
    return bSuccess;
}

// NAME:        CalcWorldPixelSize()
// DESC:        calculate size of pixel in world coords
// INPUT:       none
// OUTPUT:      size of pixel in world coords

float CSprite::CalcWorldPixelSize(void)
{
    D3DXVECTOR3 dvecPixel1(0.0f, 0.0f, 0.0f), dvecPixel2(1.0f, 0.0f, 0.0f);
    
    D3DXVec3TransformCoord(&dvecPixel1, &dvecPixel1, &m_ScreenToWorld);
    D3DXVec3TransformCoord(&dvecPixel2, &dvecPixel2, &m_ScreenToWorld);
    dvecPixel1.x -= dvecPixel2.x;
    dvecPixel1.y -= dvecPixel2.y;
    dvecPixel1.z -= dvecPixel2.z;
    return sqrt((dvecPixel1.x*dvecPixel1.x) + (dvecPixel1.y*dvecPixel1.y) + (dvecPixel1.z*dvecPixel1.z));     
}

// NAME:        GetWorldPosition()
// DESC:        get world position from screen position and screen to world xform
// INPUT:       pScreen..address of D3DXVECTOR3 screen point
// OUTPUT:      none

void CSprite::GetWorldPosition(D3DXVECTOR3 *pScreen)
{
    D3DXVec3TransformCoord(pScreen, pScreen, &m_ScreenToWorld);
}

// NAME:        UpdateScreenToWorldMatrix()
// DESC:        update screen to world matrix
// INPUT:       none
// OUTPUT:      none
// COMMENTS:    s = w*(M*P*V*S*T)
//              w = s*(M*P*V*S*T)inverse
//
//              M = model matrix
//              P = projection matrix
//              V = view matrix
//              S = viewport scale matrix*
//              T = viewport translation matrix*
//
//              Mi = M = I assumed
//
//              *see Dx8.1 SDK on viewport scaling

void CSprite::UpdateScreenToWorldMatrix(void)
{
    D3DXMATRIX C, S, T;

    D3DXMatrixScaling(&S, (float) (m_Port.Width() >> 1),
                          (float) -(m_Port.Height() >> 1),
                          1.0f);
    D3DXMatrixTranslation(&T, (float) (m_Port.Left() + (m_Port.Width() >> 1)),
                              (float) (m_Port.Top() + (m_Port.Height() >> 1)),
                              -1.0f);    
    D3DXMatrixMultiply(&C, &S, &T);                // S*T
    D3DXMatrixMultiply(&C, &m_View, &C);           // V*S*T
    D3DXMatrixMultiply(&C, &m_Projection, &C);     // M*P*V*S*T
    D3DXMatrixInverse(&m_ScreenToWorld, NULL, &C); // (M*P*V*S*T)inverse
}

// NAME:        AdjustVertexAttenuated()
// DESC:        adjust vertex for attenuated point sprite
// INPUT:       pWorld...address of D3DXVECTOR3 world point
//              dvSi.....per vertex value or render state point size
// OUTPUT:      true.....if vertex is appropriately adjusted
//              false....otherwise
// COMMENTS:    m_Sprite coefficients must be set
//                  Ss = Vh*Si*sqrt(1/(A + B*De + C*(De*De))
//                  Si is per vertex value or RS point size or vertex point size
//
// AdjustVertexAttenuated is designed to alleviate problems that arise from
// round off error during rasterization. Values that cause these errors are
// values that are close to integer and values close to 1/2 that is any value X.5.
// The reason that these values are problematic is that they can induce
// different rounding error in the ref and hal devices. This is not an error in
// either device it is just a reflection of their precision.
//
// ASSUMPTION: (x,y) position is at a known non-problematic position and not adjusted

bool CSprite::AdjustVertexAttenuated(D3DXVECTOR3 *pWorld, D3DVALUE dvSi)
{
    D3DVALUE dvSize, dvTemp, dvLower, dvUpper;
    D3DXVECTOR3 V;
    UINT i, j;

    for (i = 0; i < MAX_ADJUSTMENTS; i++)
    {
        D3DXVec3TransformCoord(&V, pWorld, &m_View);
        dvTemp = sqrt(V.x*V.x + V.y*V.y + V.z*V.z); // distance from eye to point
        dvTemp = m_Sprite.Ka.f + dvTemp*(m_Sprite.Kb.f+ dvTemp*m_Sprite.Kc.f);
        dvTemp = sqrt(1.0f/dvTemp);
        dvSize = dvSi*dvTemp*((D3DVALUE) m_pCurrentMode->nBackBufferHeight);

        // avoid log boundaries eg 2**4.5 or 2**5.5 (X.5 powers are gray areas for mipmapping)
        // ASSUMPTION: mipmap textures are assumed to be square

        if (m_Flags.Check(CF_MIPMAP))
        {
            float t;

            for (j = 0; j < MIPLEVELS; j++)
            {
                t = (float) j;
                dvLower = pow(2.0f, (t+0.4f));
                dvUpper = pow(2.0f, (t+0.6f));

                if ((dvLower <= dvSize) && (dvSize <= dvUpper))
                {
                    pWorld->z += 1.0; 
                    break;
                }
            }

            if (j != MIPLEVELS)
                continue;
        }

        dvSize /= 2.0f;

        if (dvSize < 1.0f)
            return false;

        dvTemp = (int) (dvSize + 1.0f);

        if ((dvTemp - dvSize) < (MINDIFF) ||
            (dvTemp - dvSize) > (1.0f - MINDIFF) ||
            ((dvTemp - dvSize) > (0.5f - MINDIFF) &&
             (dvTemp - dvSize) < (0.5f + MINDIFF)))
            pWorld->z += DISTINC;
        else
        {
            sprintf(m_szStatus[10][0], "$gPixelSize");
            sprintf(m_szStatus[10][1], "$y%f", 2.0f*dvSize);
            return true;
        }
    }

    return false;
}

// NAME:        SetViewport()
// DESC:        set viewport
// INPUT:       nX.......x coordinate
//              nY.......y coordinate
//              nWidth...viewport width
//              nHeight..viewport height
// OUTPUT:      CD3DTest::SetViewport()
// COMMENTS:    save viewport extents before calling CD3DTest::SetViewport()

bool CSprite::SetViewport(UINT nX, UINT nY, UINT nWidth, UINT nHeight)
{
    bool bResult;
    VIEWPORT Viewport;

    Viewport.X = (DWORD) nX;
    Viewport.Y = (DWORD) nY;
    Viewport.Width = (DWORD) nWidth;
    Viewport.Height = (DWORD) nHeight;
    Viewport.MinZ = 0.0f;
    Viewport.MaxZ = 1.0f;

    m_Port.SetPort(nX, nY, nWidth, nHeight);
    bResult = CD3DTest::SetViewport(&Viewport);
    UpdateScreenToWorldMatrix();
    return bResult;
}

// NAME:        DrawScissor()
// DESC:        draw scissor
// INPUT:       none
// OUTPUT:      none

void CSprite::DrawScissor(void)
{
    DWORD dwColor = D3DCOLOR_RGBA(0, 0, 0, 255);
    MYD3DTLVERTEX Vertex[5];

    for (UINT i = 0; i < 5; i++)
    {
        Vertex[i].dvSZ = 0.5f;
        Vertex[i].dvRHW = 2.0f;
        Vertex[i].dcColor = dwColor;
        Vertex[i].dcSpecular = dwColor;
        Vertex[i].dvTU = 0.5f;
        Vertex[i].dvTV = 0.5f;
    }

    Vertex[0].dvSX = (float) m_Rect.left;
    Vertex[0].dvSY = (float) m_Rect.top;

    Vertex[1].dvSX = (float) m_Rect.right;
    Vertex[1].dvSY = (float) m_Rect.top;

    Vertex[2].dvSX = (float) m_Rect.right;
    Vertex[2].dvSY = (float) m_Rect.bottom;

    Vertex[3].dvSX = (float) m_Rect.left;
    Vertex[3].dvSY = (float) m_Rect.bottom;

    Vertex[4].dvSX = (float) m_Rect.left;
    Vertex[4].dvSY = (float) m_Rect.top;

    if (!RenderPrimitive(D3DPT_LINESTRIP, MYD3DFVF_TLVERTEX, Vertex, (DWORD) 5, NULL, 0, NULL))
        WriteToLog("RenderPrimitive() failed: %s\n", GLE(GetLastError()));
}

// NAME:        DrawViewport()
// DESC:        draw viewport
// INPUT:       none
// OUTPUT:      none

void CSprite::DrawViewport(void)
{
    DWORD dwColor = D3DCOLOR_RGBA(0, 0, 0, 255);
    MYD3DTLVERTEX Vertex[5];

    for (UINT i = 0; i < 5; i++)
    {
        Vertex[i].dvSZ = 0.5f;
        Vertex[i].dvRHW = 2.0f;
        Vertex[i].dcColor = dwColor;
        Vertex[i].dcSpecular = dwColor;
        Vertex[i].dvTU = 0.5f;
        Vertex[i].dvTV = 0.5f;
    }

    Vertex[0].dvSX = (float) m_Port.Left();
    Vertex[0].dvSY = (float) m_Port.Top();

    Vertex[1].dvSX = (float) m_Port.Right();
    Vertex[1].dvSY = (float) m_Port.Top();

    Vertex[2].dvSX = (float) m_Port.Right();
    Vertex[2].dvSY = (float) m_Port.Bottom();

    Vertex[3].dvSX = (float) m_Port.Left();
    Vertex[3].dvSY = (float) m_Port.Bottom();

    Vertex[4].dvSX = (float) m_Port.Left();
    Vertex[4].dvSY = (float) m_Port.Top();

    if (!RenderPrimitive(D3DPT_LINESTRIP, MYD3DFVF_TLVERTEX, Vertex, (DWORD) 5, NULL, 0, NULL))
        WriteToLog("RenderPrimitive() failed: %s\n", GLE(GetLastError()));
}

// NAME:        GetLastError()
// DESC:        get last error and filter per invalid parameter testing
// INPUT:       none
// OUTPUT:      NOERROR..if invalid parameter testing
//              CD3DTest::GetLastError() otherwise

HRESULT CSprite::GetLastError(void)
{
    if (m_Flags.Check(CF_INVALID))
        return NOERROR;
    else
        return CD3DTest::GetLastError();
}

// NAME:        DisableScissorTest()
// DESC:        disable scissor test
// INPUT:       none
// OUTPUT:      true.....if scissor test disabled successfully
//              false....otherwise

bool CSprite::DisableScissorTest(void)
{
    m_dcBackground = RGB_MAKE(255, 255, 255); // normal clear color for next clear

    m_Rect.top = 0;
    m_Rect.left = 0;
    m_Rect.bottom = m_pCurrentMode->nBackBufferHeight;
    m_Rect.right = m_pCurrentMode->nBackBufferWidth;
    
    return SetRenderState(D3DRS_SCISSORTESTENABLE, (DWORD) FALSE);
}

// NAME:        EnableScissorTest()
// DESC:        enable scissor test
// INPUT:       pRect....address of RECT scissor rect
// OUTPUT:      true.....if scissor test enabled successfully
//              false....otherwise

bool CSprite::EnableScissorTest(RECT *pRect)
{
    RECT Src, Ref;

    m_dcBackground = RGB_MAKE(0, 0, 255); // alternate clear color for next clear

    if (!pRect)
    {
        WriteToLog("ERROR: EnableScissorTest(): NULL scissor rect pointer.\n");
        return false;
    }

    // can call GetScissorRect() and SetScissorRect() from CD3DTest or,
    // call them from the src and ref devices directly (as below)

    if (!((m_pSrcDevice->GetBehaviors() & DEVICECREATE_PURE) ||
          (m_pRefDevice->GetBehaviors() & DEVICECREATE_PURE)))
    {
        if (D3D_OK != m_pSrcDevice->GetScissorRect(&Src))
        {
            WriteToLog("ERROR: GetScissorRect() failed on src device: %s.\n", GLE(GetLastError()));
            return false;
        }

        if (D3D_OK != m_pRefDevice->GetScissorRect(&Ref))
        {
            WriteToLog("ERROR: GetScissorRect() failed on ref device: %s.\n", GLE(GetLastError()));
            return false;
        }

        if ((Src.top != Ref.top) || (Src.left != Ref.left) ||
            (Src.bottom != Ref.bottom) || (Src.right != Ref.right))
        {
            WriteToLog("ERROR: Src and Ref scissor rects not identical.\n");
            WriteToLog("Src rect(%d,%d,%d,%d)\n", Src.top, Src.left, Src.bottom, Src.right);
            WriteToLog("Ref rect(%d,%d,%d,%d)\n", Ref.top, Ref.left, Ref.bottom, Ref.right);
            return false;
        }
    }

    m_Rect.top = pRect->top;
    m_Rect.left = pRect->left;
    m_Rect.bottom = pRect->bottom;
    m_Rect.right = pRect->right;

    if (D3D_OK != m_pSrcDevice->SetScissorRect(&m_Rect))
    {
        WriteToLog("ERROR: SetScissorRect() failed on src device: %s.\n", GLE(GetLastError()));
        return false;
    }
    else if (D3D_OK != m_pRefDevice->SetScissorRect(&m_Rect))
    {
        WriteToLog("ERROR: SetScissorRect() failed on ref device: %s.\n", GLE(GetLastError()));
        return false;
    }

    return SetRenderState(D3DRS_SCISSORTESTENABLE, (DWORD) TRUE);
}
/******************************Module*Header*******************************
* Module Name: VidMMWDDM1_2.h
*
* D3D-based tests that verifies UMD WDDM1.2 VidMM requirements
*
* Author: Marina Meleshenko [marmel]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#pragma once

#include "wgfvidmm.h"

enum eRESIDENCY
{
    RESIDENCY_NOT_RESIDENT = 0,
    RESIDENCY_RESIDENT_VIDEO = 1,
    RESIDENCY_RESIDENT_APERTURE = 2
};

DECLARE_NAMED_VALUES(eWGFVIDMM_RESOURCE_TYPE);

BEGIN_NAMED_VALUES(eWGFVIDMM_RESOURCE_TYPE)
    NAMED_VALUE(_T("WGFVIDMM_TEXTURE"), WGFVIDMM_TEXTURE)
    NAMED_VALUE(_T("WGFVIDMM_BUFFER"), WGFVIDMM_BUFFER)
END_NAMED_VALUES(eWGFVIDMM_RESOURCE_TYPE)


class CVidMMUMDTest : public CWGFOfferReclaimTest
{
public:
    CVidMMUMDTest();
    ~CVidMMUMDTest(){OutputDebugString("~CVidMMUMDTest()\n");};

protected:
    bool LogListenerErrors();

    eRESIDENCY GetAllocationResidency(ID3D11Texture2D* pTex);
    eALLOCATION_OFFER_STATE GetOfferState(HANDLE pAllocHandle);
    eALLOCATION_OFFER_PRIORITY GetAllocationOfferPriority(HANDLE pAllocHandle);

    bool                    m_bVerifyUMDAllocInfo;
};

////////////////////// Listeners /////////////////////////////////

class CUMDLoggingInternalSurfaceETW : public CVidMmEtwMaster
{
public:
    CUMDLoggingInternalSurfaceETW()
        : m_bNewStagingBuffer(false),
          m_bReclaimedStagingBuffer(false),
          m_bReclaimedWrongSemantic(false),
          m_bNewWrongSemantic(false),
          m_bReadyForCustomETWHandler(false)
    { 
        m_tstrError = _T("");
        OutputDebugString("CUMDLoggingInternalSurfaceETW()\n");
    }

    ~CUMDLoggingInternalSurfaceETW(){OutputDebugString("~CUMDLoggingInternalSurfaceETW()\n");}

    bool IsNewStagingBuffer(){return m_bNewStagingBuffer;}
    bool IsReclaimedStagingBuffer(){return m_bReclaimedStagingBuffer;}
    bool IsNewWrongSemantic(){return m_bNewWrongSemantic;}
    bool IsReclaimedWrongSemantic(){return m_bReclaimedWrongSemantic;}
    void ReadyForCustomETWHandler(bool bReady) {m_bReadyForCustomETWHandler = bReady;}
    tstring& GetErrorString(){return m_tstrError;}

private:
    bool m_bNewStagingBuffer;
    bool m_bReclaimedStagingBuffer;
    bool m_bNewWrongSemantic;
    bool m_bReclaimedWrongSemantic;
    bool m_bReadyForCustomETWHandler;
    tstring m_tstrError;

    virtual void HandleUMDAllocation(PEVENT_RECORD pEvent);
    virtual void HandleReclaimAllocation(PEVENT_RECORD pEvent);
};


class COfferReclaimPendingETW : public CVidMmEtwMaster
{
public:
    COfferReclaimPendingETW()
        : m_numOffers(0),
          m_hD3D11AppResource(NULL)
    {OutputDebugString("COfferReclaimPendingETW()\n");}

    ~COfferReclaimPendingETW(){OutputDebugString("~COfferReclaimPendingETW()\n");}

    UINT GetOfferCount(){return m_numOffers;}
    void SetResourceToWatch(HANDLE hResource){m_hD3D11AppResource = hResource;}
    void CleanupCounters();

private:
    UINT m_numOffers;
    HANDLE m_hD3D11AppResource;

    virtual void HandleOfferAllocation(PEVENT_RECORD pEvent);
};


class COfferReclaimDefferedDeletionETW : public CVidMmEtwMaster
{
public:
    COfferReclaimDefferedDeletionETW()
        : m_numOffers(0),
          m_numTerminates(0),
          m_numKernelDestroys(0),
          m_hD3D11AppResource(NULL),
          m_hGlobalAlloc(NULL),
          m_numD3D11Destroys(0)
    {OutputDebugString("COfferReclaimDefferedDeletionETW()\n");}

    ~COfferReclaimDefferedDeletionETW(){OutputDebugString("~COfferReclaimDefferedDeletionETW()\n");}

    UINT GetOfferCount(){return m_numOffers;}
    UINT GetTerminateCount(){return m_numTerminates;}
    UINT GetD3D11DestroyCount(){return m_numD3D11Destroys;}
    UINT GetKernelDestroyCount() {return m_numKernelDestroys;}
    void SetResourceToWatch(HANDLE hResource);
    void CleanupCounters();

private:
    UINT m_numOffers;
    UINT m_numTerminates;
    UINT m_numKernelDestroys;
    HANDLE m_hD3D11AppResource;
    HANDLE m_hGlobalAlloc;
    UINT m_numD3D11Destroys;

    virtual void HandleOfferAllocation(PEVENT_RECORD pEvent);
    virtual void HandleTerminateAllocation(PEVENT_RECORD pEvent);
    virtual void HandleAdapterAllocation(PEVENT_RECORD pEvent);
    virtual void HandleD3D11Texture2D(PEVENT_RECORD pEvent);
    virtual void HandleD3D11Buffer(PEVENT_RECORD pEvent);
};


///////////////////////////////////////////// Internal Surfaces //////////////////////////////////////////
class CInternalSurfaceTest : public CVidMMUMDTest
{
public:
    CInternalSurfaceTest();
    ~CInternalSurfaceTest(){OutputDebugString("~CInternalSurfaceTest()\n");};

    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults(){return RESULT_BLOCKED;} // child classes should implement this method

protected:
    ID3D11Texture2D*        m_pVidMemTex;
    ID3D11Texture2D*        m_pStagingTex;
};

class CUMDLoggingInternalSurfaceTest : public CInternalSurfaceTest
{
public: 
    CUMDLoggingInternalSurfaceTest()
    {
        m_pETWMaster = &m_CustomETWMaster;
        m_bVerifyUMDAllocInfo = true;

        OutputDebugString("CUMDLoggingInternalSurfaceTest()\n");
    }

    ~CUMDLoggingInternalSurfaceTest()
    {
        OutputDebugString("~CUMDLoggingInternalSurfaceTest()\n");
    }

    virtual TEST_RESULT VerifyResults();
    virtual TEST_RESULT ExecuteTestCase();

private:
    CUMDLoggingInternalSurfaceETW m_CustomETWMaster;
};

class COfferReclaimStagingSurfaceTest : public CInternalSurfaceTest
{
public:
    COfferReclaimStagingSurfaceTest(){OutputDebugString("COfferReclaimStagingSurfaceTest()\n");}
    ~COfferReclaimStagingSurfaceTest(){OutputDebugString("~COfferReclaimStagingSurfaceTest()\n");}

    virtual TEST_RESULT VerifyResults();
};

////////////////////////////////////////////// Packed allocations ///////////////////////////////////////////////
class CPackedSurfaceTest : public CVidMMUMDTest
{
public:
    CPackedSurfaceTest();
    ~CPackedSurfaceTest();

    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults(){return RESULT_BLOCKED;} // child classes should implement this method
    virtual void CleanupTestCase();
    virtual int GetNumRenderWindows() const { return 1;} // this will result in rendering window being created. This is needed for CPresentHelper

protected:
    HANDLE			 		m_pPackedAllocs[MAX_RESOURCE_COUNT];		// these will point to textures in m_pTex, which are marked as Packed
    IDXGIResource*          m_pPackedResources[MAX_RESOURCE_COUNT];		// these will point to resources in m_pResources, which are marked as Packed
    UINT                    m_PackedCount;
    bool                    m_bNoAllocationLogging;
    eWGFVIDMM_RESOURCE_TYPE m_ResourceType;
    testfactor::RFactor     m_Factor;
    bool                    m_bSharedProcessStarted;
    CPresentHelper          m_PresentHelper;
};

class CUMDLoggingPackedSurfaceTest : public CPackedSurfaceTest
{
public: 
    CUMDLoggingPackedSurfaceTest()
    {
        m_bVerifyUMDAllocInfo = true;
        OutputDebugString("CUMDLoggingPackedSurfaceTest()\n");
    }

    ~CUMDLoggingPackedSurfaceTest(){OutputDebugString("~CUMDLoggingPackedSurfaceTest()\n");}

    virtual TEST_RESULT VerifyResults();
};

enum eOFFER_RECLAIM_PACKED_SURFACE_TEST
{
    OFFER_RECLAIM_PACKED_SURFACE_TEST_SINGLE = 0,
    OFFER_RECLAIM_PACKED_SURFACE_TEST_MULTIPLE = 1,
    OFFER_RECLAIM_PACKED_SURFACE_TEST_ALL = 2
};
DECLARE_NAMED_VALUES(eOFFER_RECLAIM_PACKED_SURFACE_TEST);

class COfferReclaimPackedSurfaceTest : public CPackedSurfaceTest
{
public:
    COfferReclaimPackedSurfaceTest(){OutputDebugString("COfferReclaimPackedSurfaceTest()\n");}
    ~COfferReclaimPackedSurfaceTest(){OutputDebugString("~COfferReclaimPackedSurfaceTest()\n");}

    virtual void InitTestParameters();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults();

private:
    eOFFER_RECLAIM_PACKED_SURFACE_TEST        m_TestType;

    TEST_RESULT ExecuteTestCaseSingle();
    TEST_RESULT ExecuteTestCaseMultiple();
    TEST_RESULT ExecuteTestCaseAll();
};

/////////////////////////////////////////// Renaming ////////////////////////////////////////////////
class CRenamedSurfaceTest : public CVidMMUMDTest
{
public:
    CRenamedSurfaceTest();
    ~CRenamedSurfaceTest();

    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults(){return RESULT_BLOCKED;} // child classes should implement this method
    virtual int GetNumRenderWindows() const { return 1;} // this will result in rendering window being created. This is needed for CPresentHelper


protected:
    UINT                    m_TimesToRename;
    CPresentHelper          m_PresentHelper;
    eWGFVIDMM_RESOURCE_TYPE m_ResourceType;
    testfactor::RFactor     m_Factor;

    HRESULT RenameTexture(ID3D11Texture2D* pTex, ID3D11ShaderResourceView* pShaderView);
    HRESULT RenameBuffer(ID3D11Buffer* pBuf);
};

class CUMDLoggingRenamedSurfaceTest : public CRenamedSurfaceTest
{
public: 
    CUMDLoggingRenamedSurfaceTest()
    {
        m_bVerifyUMDAllocInfo = true;
        OutputDebugString("CUMDLoggingRenamedSurfaceTest()\n");
    }
    ~CUMDLoggingRenamedSurfaceTest(){OutputDebugString("~CUMDLoggingRenamedSurfaceTest()\n");};

    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults();
};

enum eOFFER_RECLAIM_RENAMED_SURFACE_TEST
{
    OFFER_RECLAIM_RENAMED_SURFACE_TEST_OFFERED = 0,
    OFFER_RECLAIM_RENAMED_SURFACE_TEST_RECLAIMED = 1,
    OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED = 2,
    OFFER_RECLAIM_RENAMED_SURFACE_TEST_RETIRED_OFFERED = 3
};
DECLARE_NAMED_VALUES(eOFFER_RECLAIM_RENAMED_SURFACE_TEST);

class COfferReclaimRenamedSurfaceTest : public CRenamedSurfaceTest
{
public:
    COfferReclaimRenamedSurfaceTest(){OutputDebugString("COfferReclaimRenamedSurfaceTest()\n");}
    ~COfferReclaimRenamedSurfaceTest(){OutputDebugString("~COfferReclaimRenamedSurfaceTest()\n");}

    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults();

private:
    eOFFER_RECLAIM_RENAMED_SURFACE_TEST m_TestType;
};

//////////////////////////////////////////////////////////////////////////////////////////
enum eCREATE_DESTROY_TEST
{
    CREATE_DESTROY_TEST_BASIC = 0,
    CREATE_DESTROY_TEST_RECYCLE = 1,
    CREATE_DESTROY_TEST_REUSE = 2
};
DECLARE_NAMED_VALUES(eCREATE_DESTROY_TEST);

class CUMDLoggingCreateDestroyTest : public CVidMMUMDTest
{
public:
    CUMDLoggingCreateDestroyTest();
    ~CUMDLoggingCreateDestroyTest();

    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT VerifyResults();

protected:
    eCREATE_DESTROY_TEST      m_TestType;
};

//////////////////////////////////////////////////////////////////////////////
DECLARE_NAMED_VALUES(eALLOCATION_OFFER_PRIORITY);

class COfferReclaimCreateDestroyTest : public CVidMMUMDTest
{
public:
    COfferReclaimCreateDestroyTest(){OutputDebugString("COfferReclaimCreateDestroyTest()\n");}
    ~COfferReclaimCreateDestroyTest(){OutputDebugString("~COfferReclaimCreateDestroyTest()\n");}
    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    TEST_RESULT VerifyAllocationsNotOffered();
    TEST_RESULT VerifyAllocationsOffered();
    TEST_RESULT VerifyAllocationNotOffered(UINT Index);
    TEST_RESULT VerifyAllocationOffered(UINT Index);
    TEST_RESULT ExecuteBasicTestCase();
    TEST_RESULT ExecuteRecycleTestCase();
    TEST_RESULT ExecuteReuseTestCase();
    virtual int GetNumRenderWindows() const { return 1;} // this will result in rendering window being created. This is needed for CPresentHelper

protected:
    eALLOCATION_OFFER_PRIORITY   m_OfferPriority;
    CPresentHelper          m_PresentHelper;
    eCREATE_DESTROY_TEST      m_TestType;
    bool                      m_bReclaim;
};

class COfferReclaimCreateDestroyReuseChild : public CWGFVidMMResourceChild
{
public:
    COfferReclaimCreateDestroyReuseChild(){m_bNoKeyedMutex = false;}
    TEST_RESULT ExecuteTestCase();
};

//////////////////////////////////////////////////////////////////////////////
class COfferReclaimDeferredDeletionsTest : public CVidMMUMDTest
{
public:
    COfferReclaimDeferredDeletionsTest() 
    {
        m_pETWMaster = &m_CustomETWMaster;
        m_ResourceType = WGFVIDMM_TEXTURE;
        m_bShared = false;
        OutputDebugString("COfferReclaimDeferredDeletionsTest()\n");
    }
    ~COfferReclaimDeferredDeletionsTest(){OutputDebugString("~COfferReclaimDeferredDeletionsTest()\n");}
    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual int GetNumRenderWindows() const { return 1;} // this will result in rendering window being created. This is needed for CPresentHelper

protected:
    CPresentHelper          m_PresentHelper;
    COfferReclaimDefferedDeletionETW         m_CustomETWMaster;
    eWGFVIDMM_RESOURCE_TYPE      m_ResourceType;
    UINT GetOfferCount();
    UINT GetTerminateCount();
    UINT GetD3D11DestroyCount();
    UINT GetKernelDestroyCount();

};

//////////////////////////////////////////////////////////////////////////////
enum eOFFER_RECLAIM_PENDING_TEST
{
    OFFER_RECLAIM_PENDING_OFFER_TEST = 0,
    OFFER_RECLAIM_PENDING_CANCEL_TEST = 1
};
DECLARE_NAMED_VALUES(eOFFER_RECLAIM_PENDING_TEST);

class COfferReclaimPendingTest : public CVidMMUMDTest
{
public:
    COfferReclaimPendingTest()
    {
        m_ValidpDiscarded = true;
        m_ResourceType = WGFVIDMM_TEXTURE;
        m_bShared = false;
        m_OfferPriority = OFFER_PRIORITY_NORMAL;
        m_pETWMaster = &m_CustomETWMaster;
        OutputDebugString("COfferReclaimPendingTest()\n");
    }
    ~COfferReclaimPendingTest(){OutputDebugString("~COfferReclaimPendingTest()\n");}
    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual int GetNumRenderWindows() const { return 1;} // this will result in rendering window being created. This is needed for CPresentHelper

protected:
    CPresentHelper          m_PresentHelper;
    eOFFER_RECLAIM_PENDING_TEST     m_TestType;
    COfferReclaimPendingETW         m_CustomETWMaster;
    eWGFVIDMM_RESOURCE_TYPE         m_ResourceType;
    eALLOCATION_OFFER_PRIORITY      m_OfferPriority;
    bool                            m_ValidpDiscarded;

    UINT GetOfferCount();
};



////////////////////////////// Coherency //////////////////////////////////
class CCoherencyTest : public CVidMMUMDTest
{
public:
    CCoherencyTest();
    ~CCoherencyTest();

    virtual void InitTestParameters();
    virtual TEST_RESULT SetupTestCase();
    virtual void CleanupTestCase();
    virtual TEST_RESULT ExecuteTestCase();

    TEST_RESULT ResourceContentAndValidation(ID3D11Texture2D* pTex, BYTE Pattern, bool bFill);
    TEST_RESULT ValidateAllocationCreationParameters();

protected:
    ID3D11Texture2D*        m_pStagingReadOnly;
    ID3D11Texture2D*        m_pStagingReadWrite;
    ID3D11Texture2D*        m_pStagingWriteOnly;
    ID3D11Texture2D*        m_pTexture;
    bool                    m_bHWCacheCoherencyAvailable;
};



//////////////////////////////////////////////////////////////////////////////
class CVidMMUMDTestApp : public CD3D11TestFramework
{
public:
    CVidMMUMDTestApp() : m_bPresent( false ) {}
    void Present() { if ( m_bPresent ) CD3D11TestFramework::Present( 0, 0, 0, 0, 0, 0 );	}
    bool Present(RECT *pSrc, RECT *pDest, void *pSourceDirtyRegion, void *pDestClipRegion, UINT SyncInterval, UINT dwFlags)
    {
        if ( m_bPresent )
            return CD3D11TestFramework::Present( pSrc, pDest, pSourceDirtyRegion, pDestClipRegion, SyncInterval, dwFlags );

        return true;
    }
    bool getPresent() { return m_bPresent; }
    void setPresent(bool bPresent) {m_bPresent = bPresent;}
protected:
    TESTTYPE GetTestType() const { return TESTTYPE_CONF; }

    virtual void InitOptionVariables();
    virtual bool InitTestGroups();

    bool m_bPresent;
};


/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include <shell98.rc>
#include "resource.h"
#include "windows.h"
#include "ntverp.h"

//#include "winver.h"


//
// Version
//
#define VER_FILETYPE				VFT_APP
#define VER_FILESUBTYPE				VFT2_UNKNOWN
#define VER_FILEDESCRIPTION_STR 		"UAA Test Application"
#define VER_INTERNALNAME_STR			"uaatest.exe"
#define VER_ORIGINALFILENAME_STR		"uaatest.exe"

#include "common.ver"


//
// Icon
//

// Icon with lowest ID value placed first to ensure application icon
// remains consistent on all systems.
IDI_UAATEST             ICON    DISCARDABLE     "uaatest.ico"
/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  Content: Graphics device - Resource format helper
 *
 ***************************************************************************/
#pragma once

#include <d3d10_1.h>
#include <cassert>

// ----------------------------------------------------------------------------
// Some enumerations used in the D3D10_FORMAT_DETAIL structure
// ----------------------------------------------------------------------------
typedef enum D3D10_REQUIREMENTS
{
    D3D10R_UND    = 0, // undecided
    D3D10R_NA     = -1, // disallowed or not applicable
    D3D10R_REQ    = 1, // required
    D3D10R_OPT    = -2, // optional
    // Note, 2 bits allocated for this in FORMAT_DETAIL below. If you add fields here, add bits...
    // NOTE SIGNED VALUES ARE USED SINCE COMPILER MAKES ENUMS SIGNED, AND BITFIELDS ARE SIGN EXTENDED ON READ
} D3D10_REQUIREMENTS;

typedef enum D3D10_FORMAT_LAYOUT
{
    D3D10FL_STANDARD = 0, // standard layout
    D3D10FL_CUSTOM   = -1  // custom layout
    // Note, 1 bit allocated for this in FORMAT_DETAIL below. If you add fields here, add bits...
    // NOTE SIGNED VALUES ARE USED SINCE COMPILER MAKES ENUMS SIGNED, AND BITFIELDS ARE SIGN EXTENDED ON READ
} D3D10_FORMAT_LAYOUT;

typedef enum D3D10_FORMAT_TYPE_LEVEL
{
    D3D10FTL_NO_TYPE      = 0,
    D3D10FTL_PARTIAL_TYPE = -2,
    D3D10FTL_FULL_TYPE    = -1,
    // Note, 2 bits allocated for this in FORMAT_DETAIL below. If you add fields here, add bits...
    // NOTE SIGNED VALUES ARE USED SINCE COMPILER MAKES ENUMS SIGNED, AND BITFIELDS ARE SIGN EXTENDED ON READ
} D3D10_FORMAT_TYPE_LEVEL;

typedef enum D3D10_FORMAT_COMPONENT_NAME
{
    D3D10FCN_R     = -4,
    D3D10FCN_G     = -3,
    D3D10FCN_B     = -2,
    D3D10FCN_A     = -1,
    D3D10FCN_D     = 0,
    D3D10FCN_S     = 1,
    D3D10FCN_X     = 2,
    // Note, 3 bits allocated for this in FORMAT_DETAIL below. If you add fields here, add bits...
    // NOTE SIGNED VALUES ARE USED SINCE COMPILER MAKES ENUMS SIGNED, AND BITFIELDS ARE SIGN EXTENDED ON READ
} D3D10_FORMAT_COMPONENT_NAME;

typedef enum D3D10_FORMAT_COMPONENT_INTERPRETATION
{
    D3D10FCI_TYPELESS           = 0,
    D3D10FCI_FLOAT              = -4,
    D3D10FCI_SNORM              = -3,
    D3D10FCI_UNORM              = -2,
    D3D10FCI_SINT               = -1,
    D3D10FCI_UINT               = 1,
    D3D10FCI_UNORM_SRGB         = 2,
    D3D10FCI_BIASED_FIXED_2_8   = 3,
    // Note, 3 bits allocated for this in FORMAT_DETAIL below. If you add fields here, add bits...
    // NOTE SIGNED VALUES ARE USED SINCE COMPILER MAKES ENUMS SIGNED, AND BITFIELDS ARE SIGN EXTENDED ON READ
} D3D10_FORMAT_COMPONENT_INTERPRETATION;

// ----------------------------------------------------------------------------
//
// CD3D10FormatHelper_Test
// Despite the class name, this class represents D3D10.1 level functionality.
//
// ----------------------------------------------------------------------------
class CD3D10FormatHelper_Test
{
private:
    // ----------------------------------------------------------------------------
    // Information describing everything about a D3D10 Resource Format
    // ----------------------------------------------------------------------------
    typedef struct FORMAT_DETAIL
    {
        DXGI_FORMAT                 DXGIFormat;
        UINT8                       BitsPerComponent[4]; // only used for D3D10FTL_PARTIAL_TYPE or FULL_TYPE
        UINT8                       BitsPerElement;
        BOOL                        SRGBFormat : 1;
        UINT                        WidthAlignment : 4;      // number of texels to align to in a mip level.
        UINT                        HeightAlignment : 3;     // Top level dimensions must be a multiple of these
        UINT                        DepthAlignment : 1;      // values.
        D3D10_FORMAT_LAYOUT         Layout : 1;
        D3D10_FORMAT_TYPE_LEVEL     TypeLevel : 2;
        D3D10_FORMAT_COMPONENT_NAME ComponentName0 : 3; // RED    ... only used for D3D10FTL_PARTIAL_TYPE or FULL_TYPE
        D3D10_FORMAT_COMPONENT_NAME ComponentName1 : 3; // GREEN  ... only used for D3D10FTL_PARTIAL_TYPE or FULL_TYPE
        D3D10_FORMAT_COMPONENT_NAME ComponentName2 : 3; // BLUE   ... only used for D3D10FTL_PARTIAL_TYPE or FULL_TYPE
        D3D10_FORMAT_COMPONENT_NAME ComponentName3 : 3; // ALPHA  ... only used for D3D10FTL_PARTIAL_TYPE or FULL_TYPE
        D3D10_FORMAT_COMPONENT_INTERPRETATION ComponentInterpretation0 : 3; // only used for D3D10FTL_FULL_TYPE
        D3D10_FORMAT_COMPONENT_INTERPRETATION ComponentInterpretation1 : 3; // only used for D3D10FTL_FULL_TYPE
        D3D10_FORMAT_COMPONENT_INTERPRETATION ComponentInterpretation2 : 3; // only used for D3D10FTL_FULL_TYPE
        D3D10_FORMAT_COMPONENT_INTERPRETATION ComponentInterpretation3 : 3; // only used for D3D10FTL_FULL_TYPE
        bool                        bDX9VertexOrIndexFormat : 1;
        bool                        bDX9TextureFormat : 1;
		bool                        bFloatNormFormat : 1;
        bool                        bPlanar : 1;
        bool                        bYUV : 1;
    } FORMAT_DETAIL;

    typedef struct FORMAT_REQUIREMENTS
    {
        DXGI_FORMAT                 DXGIFormat;
        DXGI_FORMAT                 ParentFormat;
        DXGI_FORMAT*                pFormatCastSet;
        BOOL                        APIFormatExists : 1;
        D3D10_REQUIREMENTS          Buffer : 2;
        D3D10_REQUIREMENTS          InputAssemblerVertexBuffer : 2;
        D3D10_REQUIREMENTS          InputAssemblerIndexBuffer : 2;
        D3D10_REQUIREMENTS          StreamOutputBuffer : 2;
        D3D10_REQUIREMENTS          Texture1D : 2;
        D3D10_REQUIREMENTS          Texture2D : 2;
        D3D10_REQUIREMENTS          Texture3D : 2;
        D3D10_REQUIREMENTS          TextureCube : 2;
        D3D10_REQUIREMENTS          ld : 2;
        D3D10_REQUIREMENTS          sample : 2;
        D3D10_REQUIREMENTS          sample_c : 2;
        D3D10_REQUIREMENTS          sample_mono : 2;
        D3D10_REQUIREMENTS          gather4 : 2;
        D3D10_REQUIREMENTS          MipMap : 2;
        D3D10_REQUIREMENTS          MipMapAutoGen : 2;
        D3D10_REQUIREMENTS          RenderTarget : 2;
        D3D10_REQUIREMENTS          BlendableRenderTarget : 2;
        D3D10_REQUIREMENTS          DepthStencilTarget : 2;
        D3D10_REQUIREMENTS          CPULockable : 2;
        D3D10_REQUIREMENTS          MultisampleRenderTarget : 2;
        D3D10_REQUIREMENTS          MultisampleLoad : 2;
        D3D10_REQUIREMENTS          MultisampleResolve : 2;
        D3D10_REQUIREMENTS          DisplayScanOut : 2;
        D3D10_REQUIREMENTS          CastWithinBitLayout : 2;
    } FORMAT_REQUIREMENTS;
    static const FORMAT_DETAIL      s_FormatDetail[];
    static const FORMAT_REQUIREMENTS      s_D3D10_FormatRequirements[];
    static const FORMAT_REQUIREMENTS      s_D3D10_BGR_FormatRequirements[];
    static const FORMAT_REQUIREMENTS      s_D3D10_Win7_FormatRequirements[];
    static const FORMAT_REQUIREMENTS      s_D3D10_1_FormatRequirements[];
    static const FORMAT_REQUIREMENTS      s_D3D10_1_Win7_FormatRequirements[];
    static LPCSTR             s_FormatNames[]; // separate from above structure so it can be compiled out of runtime.
    static const UINT               s_NumFormats;

public:
    typedef enum { e_None, e_BGR, e_AllFormatFeatures } eExtendedFormatFeatures;
    static bool CanCastBackBuffers(eExtendedFormatFeatures);
    static DXGI_FORMAT GetFormat(SIZE_T Index, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static bool FormatExists(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static bool APIFormatExists(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static LPCSTR GetName(DXGI_FORMAT Format);
    static bool IsSRGBFormat(DXGI_FORMAT Format);
	static bool Is16BitBGRFormat(DXGI_FORMAT Format);
    static UINT GetBitsPerStencil(DXGI_FORMAT  Format);
    static void GetFormatReturnTypes(
                    DXGI_FORMAT                            Format,
                    D3D10_FORMAT_COMPONENT_INTERPRETATION* pInterpretations ); // return array of 4 components
    static UINT  GetNumComponentsInFormat( DXGI_FORMAT  Format );
    // Converts the sequential component index (range from 0 to GetNumComponentsInFormat()) to
    // the absolute component index (range 0 to 3).
    static UINT  Sequential2AbsoluteComponentIndex(DXGI_FORMAT  Format, UINT SequentialComponentIndex);
    static bool CheckFormatCastability( DXGI_FORMAT a, DXGI_FORMAT b, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features ); // check for casting from a to b or vice versa
    static DXGI_FORMAT                          GetParentFormat(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static DXGI_FORMAT*                         GetFormatCastSet(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static bool                                 CanBeCastEvenFullyTyped(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_FORMAT_LAYOUT                  GetLayout(DXGI_FORMAT Format);
    static D3D10_FORMAT_TYPE_LEVEL              GetTypeLevel(DXGI_FORMAT Format);
    static UINT                                 GetBitsPerElementThrow(DXGI_FORMAT Format);
    static UINT                                 GetBitsPerElement(DXGI_FORMAT Format);
    static UINT                               GetWidthAlignment(DXGI_FORMAT Format);
    static UINT                               GetHeightAlignment(DXGI_FORMAT Format);
    static UINT                               GetDepthAlignment(DXGI_FORMAT Format);
    static D3D10_FORMAT_COMPONENT_NAME          GetComponentName(DXGI_FORMAT Format, UINT AbsoluteComponentIndex);
    static UINT                                 GetBitsPerComponent(DXGI_FORMAT Format, UINT AbsoluteComponentIndex);
    static D3D10_FORMAT_COMPONENT_INTERPRETATION    GetFormatComponentInterpretation(DXGI_FORMAT Format, UINT AbsoluteComponentIndex);
    static D3D10_REQUIREMENTS                   BufferSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   IAVertexBufferSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   IAIndexBufferSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   SOBufferSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   Texture1DSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   Texture2DSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   Texture3DSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   TextureCubeSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   ldSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   sampleSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   sample_cSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   sample_monoSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   gather4Support(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   MipMapSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   MipMapAutogenSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   RenderTargetSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   MultisampleRenderTargetSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   MultisampleLoadSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   BlendableRenderTargetSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   DepthStencilTargetSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Featu