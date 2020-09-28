#include "DevicePCH.h"

#include <d3d.h>
#include "CDevice.h"
#include "CBuffers.h"

/*
// Copy of GUIDs from DX7 header to put them against new device identifier
DEFINE_GUID( IID_IDirect3DTnLHalDevice, 0xf5049e78, 0x4861, 0x11d2, 0xa4, 0x7, 0x0, 0xa0, 0xc9, 0x6, 0x29, 0xa8);
DEFINE_GUID( IID_IDirect3DHALDevice,    0x84E63dE0, 0x46AA, 0x11CF, 0x81, 0x6F,0x00,0x00, 0xC0,0x20,0x15,0x6E );
DEFINE_GUID( IID_IDirect3DRGBDevice,    0xA4665C60, 0x2673, 0x11CF, 0xA3, 0x1A,0x00,0xAA, 0x00,0xB9,0x33,0x56 );
DEFINE_GUID( IID_IDirect3DRefDevice,    0x50936643, 0x13e9, 0x11d1, 0x89, 0xaa,0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
*/


void CDevice::InitializeAPICycling()
{
    m_dwRPCurrentAPI = RP_DP;
    
    m_pRPVertexArray = NULL;
    m_dwRPVertexArraySize = 0;
    m_pRPIndexArray = NULL;
    m_dwRPIndexArraySize = 0;

    m_bRPLighting = true;
	m_bRPClipping = true;
}

void CDevice::ReleaseAPICycling()
{
    if (NULL != m_pRPVertexArray)
        delete [] m_pRPVertexArray;

    if (NULL != m_pRPIndexArray)
        delete [] m_pRPIndexArray;
}

void CDevice::SetAPI(RENDERPRIMITIVEAPI dwAPI)
{
    m_dwRPCurrentAPI = dwAPI;
}
    
RENDERPRIMITIVEAPI CDevice::GetAPI(void)
{
    return m_dwRPCurrentAPI;
}

void CDevice::SetDestinationFVF(DWORD dwDestinationFVF)
{
    m_dwDestinationFVF = dwDestinationFVF;
}

DWORD CDevice::GetDestinationFVF(void)
{
    return m_dwDestinationFVF;
}




bool CDevice::RenderPrimitive(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD dwVertexTypeDesc,
							  LPVOID lpVertices, DWORD dwVertexCount, LPVOID lpIndices, 
							  DWORD dwIndexCount, RPOPTIONS *pOptions)
{
    DWORD       dwAPI = RemapAPI(m_dwRPCurrentAPI, d3dptPrimitiveType, dwVertexTypeDesc);
    DWORD       dwVertexSize = GetVertexSize(dwVertexTypeDesc);
    bool        bRet = true;
	bool		bIs32BitIndices;
	DWORD		dwFlags;
	DWORD		dwUsage = 0;
	DWORD		dwPool = 0;

    LPVOID      pVertices = NULL;
    DWORD       dwVCount;
    LPVOID      pIndices = NULL;
    DWORD       dwICount;
   
	// Decode options
	if (pOptions)
	{
		bIs32BitIndices = (pOptions->uIBBitCount == 32) ? true : false;
		dwFlags = pOptions->Flags;
		dwUsage = pOptions->Usage;
		dwPool = pOptions->Pool;

		// Determine Pool value in case FMWK_DEFAULT
		if ( (dwPool == FMWK_DEFAULT && (m_dwDevType == DEVICETYPE_SW || 
			 ((dwVertexTypeDesc & D3DFVF_POSITION_MASK) != D3DFVF_XYZRHW && ( (m_dwBehaviors & DEVICECREATE_SWVP) || ((m_dwBehaviors & DEVICECREATE_MIXED) && !(m_dwBehaviors & DEVICECREATE_HWVP))) ) ||
			 (m_fInterface < 8.f && m_dwDevType == DEVICETYPE_REF)))
			|| (m_fInterface == 7.0f && (dwVertexTypeDesc & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW) )

		{
			dwPool = POOL_SYSTEMMEM;
		}
	}
	else
	{
		bIs32BitIndices = false;
		dwFlags = 0;

		// Determine Usage and Pool values
		if (m_dwDevType == DEVICETYPE_SW || 
			((dwVertexTypeDesc & D3DFVF_POSITION_MASK) != D3DFVF_XYZRHW && ( (m_dwBehaviors & DEVICECREATE_SWVP) || ((m_dwBehaviors & DEVICECREATE_MIXED) && !(m_dwBehaviors & DEVICECREATE_HWVP))) ) ||
			(m_fInterface < 8.f && m_dwDevType == DEVICETYPE_REF) ||
			(m_fInterface == 7.0f && (dwVertexTypeDesc & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW) )
		{
			dwPool = POOL_SYSTEMMEM;
		}
		else
		{
			dwPool = FMWK_DEFAULT;
		}
		if ((m_dwBehaviors & DEVICECREATE_MIXED) != 0 && (m_dwBehaviors & DEVICECREATE_HWVP) == 0)
			dwUsage |= USAGE_SOFTWAREPROCESSING;
	}


    // Filling local vertices and indices pointers/counters
    // with correct data for selected API
    switch(dwAPI)
    {
    case RP_BE:
    case RP_DP:
    case RP_DPS:
    case RP_DPVB:
    case RP_DPVB_OPT:
    case RP_DPVB_PV:
    case RP_DPVB_OPT_PV:
    case RP_DPVB_PVS:
        dwVCount = dwVertexCount;
        pVertices = lpVertices;
        pIndices = NULL;
        dwICount = 0;

        // Non-indexed API selected: need to dereference vertices in case of indexed call
        if (NULL != lpIndices)
        {
			if (dwVertexCount > 0xffff)
				bIs32BitIndices = true;
            dwVCount = dwIndexCount;
            pVertices = CreateVertexArray(lpVertices, dwVertexCount, dwVertexSize, 
                                         lpIndices, dwIndexCount, bIs32BitIndices);
            if (NULL == pVertices)
                bRet = false;
        }
        break;
    case RP_BIE:
    case RP_DIP:
    case RP_DIPS:
    case RP_DIPVB:
    case RP_DIPVB_OPT:
    case RP_DIPVB_PV:
    case RP_DIPVB_OPT_PV:
    case RP_DIPVB_PVS:
        dwVCount = dwVertexCount;
        pVertices = lpVertices;
        dwICount = dwIndexCount;
        pIndices = lpIndices;
        
        // Indexed API selected: need to build index array in case of non-indexed call
        if (NULL == lpIndices)
        {
			if (dwVertexCount > 0xffff)
				bIs32BitIndices = true;
            dwICount = dwVertexCount;
            pIndices = CreateIndexArray(dwICount, bIs32BitIndices);
            if (NULL == pIndices)
                bRet = false;
        }
        break;
    default:
        // Incorrect API
        bRet = false;
        break;
    }		
    
    // Choose and call low level rendering function
    if (bRet)
    {
        switch (dwAPI)
        {
        case RP_BE:
        case RP_BIE:
            if (FAILED(BeginEnd6(d3dptPrimitiveType, dwVertexTypeDesc, pVertices, dwVCount, (WORD*)pIndices, dwICount, dwFlags)))
				bRet = false;
            break;
        case RP_DP:
        case RP_DIP:
            bRet = DrawPrimitiveUserMemory(d3dptPrimitiveType, dwVertexTypeDesc, 
						pVertices, dwVCount, pIndices, dwICount, dwFlags, bIs32BitIndices);
            break;
        case RP_DPS:
        case RP_DIPS:
            {
                DRAWPRIMITIVESTRIDEDDATA     VertexData;
            
                // Filled strided data structure
                ConvertToStridedFormat(&VertexData, dwVertexTypeDesc, pVertices, dwVertexSize);

                // Pass filled structure to the low-level function 
                bRet = DrawPrimitiveStridedData(d3dptPrimitiveType, dwVertexTypeDesc, &VertexData,
                                            dwVCount, (LPWORD)pIndices, dwICount, dwFlags, dwUsage, dwPool,
											bIs32BitIndices);
            }
            break;
        case RP_DPVB:
        case RP_DIPVB:
        case RP_DPVB_OPT:
        case RP_DIPVB_OPT:
            {
                CVertexBuffer   *pVB = NULL;
                DWORD           dwVBFlags = 0x00000800l;	//D3DVBCAPS_SYSTEMMEMORY;
                PVOID           pPtr = NULL;
				DWORD			dwLenght = dwVCount * dwVertexSize;			

                // Vertex buffer
                if (FAILED(ConvertToVertexBuffer(dwVertexTypeDesc, dwVCount, pVertices, dwVertexSize, dwUsage /*| USAGE_WRITEONLY*/, dwPool, true, &pVB)))
                    goto dpvb_error;

                // Optimization
                // Note: do not optimize transformed vertises or when using DX7 Reference
                if ( (RP_DPVB_OPT == m_dwRPCurrentAPI || RP_DIPVB_OPT == m_dwRPCurrentAPI) &&
                     ((dwVertexTypeDesc & D3DFVF_POSITION_MASK) != D3DFVF_XYZRHW) &&
                     (m_fInterface != 7.0f || (m_Guid != IID_IDirect3DRefDevice && m_Guid != IID_IDirect3DTnLHalDevice)))
                {
                    if (FAILED(pVB->Optimize()))
                        goto dpvb_error;
                }

				// Generate clip codes for TL data (post DX6 only)
                if (m_fInterface == 7.0f && (dwVertexTypeDesc & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW)
                {
                    if (m_bRPClipping)
                        if (FAILED(pVB->ProcessVertices(VOP_CLIP, 0, dwVCount, NULL, 0, 0)))
                            goto dpvb_error;
                }

                // Rendering
                bRet = DrawPrimitiveVertexBuffer(d3dptPrimitiveType, pVB, 0, dwVCount, 
                                      (LPWORD)pIndices, dwICount, dwFlags, dwUsage, dwPool, bIs32BitIndices);

                if (pVB)		pVB->Release();
                break;

                // Cleanup after error
            dpvb_error:
                if (pVB)		pVB->Release();
                bRet = false;
            }
            break;
/*
        case RP_DPVB_PV:
        case RP_DIPVB_PV:
        case RP_DPVB_OPT_PV:
        case RP_DIPVB_OPT_PV:
            {
                CVertexBuffer   *pSrcSrcVB = NULL;
                CVertexBuffer   *pDstSrcVB = NULL;
                CVertexBuffer   *pSrcRefVB = NULL;
                CVertexBuffer   *pDstRefVB = NULL;
                DWORD           dwDstVertexTypeDesc;
                DWORD           dwVBFlags = D3DVBCAPS_SYSTEMMEMORY;
                DWORD           dwOp = 0;
                PVOID           pPtr = NULL;
                DWORD           dwSize;
                bool            bLighting;

                // Set destination vertex type
                if (m_dwDestinationFVF)
                    dwDstVertexTypeDesc = m_dwDestinationFVF;
                else
                {
                    dwDstVertexTypeDesc = (dwVertexTypeDesc & ~(D3DFVF_POSITION_MASK | D3DFVF_NORMAL)) | 
                                            (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR);
                }

                // Source vertex buffer for the source device
                pSrcSrcVB = CreateVertexBuffer(dwVertexTypeDesc, dwVCount, dwVBFlags, false, dwUsage, dwPool);
                if (NULL == pSrcSrcVB)
                    goto dpvbpv_error;
                if (false == pSrcSrcVB->Lock(&pPtr, &dwSize))
                    goto dpvbpv_error;
                if (dwSize != dwVCount*dwVertexSize)
                    goto dpvbpv_error;
                memcpy(pPtr, pVertices, dwSize);
                pSrcSrcVB->Unlock();

                // Destination vertex buffer for the source device
                pDstSrcVB = CreateVertexBuffer(dwDstVertexTypeDesc, dwVCount, dwVBFlags, false, dwUsage, dwPool);
                if (NULL == pDstSrcVB)
                    goto dpvbpv_error;

                // Optimization
                if (RP_DPVB_OPT == dwAPI || RP_DIPVB_OPT == dwAPI)
                {
                    if (FAILED(pSrcSrcVB->Optimize(false)))
                        goto dpvbpv_error;
                }

                // Detect if lighting is required
                if (0x0700 > m_dwVersion)
                    bLighting = (dwVertexTypeDesc & D3DFVF_NORMAL) ? true : false;
                else
                    bLighting = m_bRPSrcLighting;
                // ProcessVertices
                if (false == pDstSrcVB->ProcessVertices(D3DVOP_TRANSFORM | D3DVOP_CLIP |
                    ( (bLighting) ? D3DVOP_LIGHT : NULL),
                    0, dwVCount, pSrcSrcVB, 0, false, 0))
                {
                    goto dpvbpv_error;
                }

                if (IsReference())
                {
                    // Source vertex buffer for the reference device
                    pSrcRefVB = CreateVertexBuffer(dwVertexTypeDesc, dwVCount, dwVBFlags, true, dwUsage, dwPool);
                    if (NULL == pSrcRefVB)
                        goto dpvbpv_error;
                    if (false == pSrcRefVB->Lock(&pPtr, &dwSize))
                        goto dpvbpv_error;
                    if (dwSize != dwVCount*dwVertexSize)
                        goto dpvbpv_error;
                    memcpy(pPtr, pVertices, dwSize);
                    pSrcRefVB->Unlock();

                    // Destination vertex buffer for the reference device
                    pDstRefVB = CreateVertexBuffer(dwDstVertexTypeDesc, dwVCount, dwVBFlags, true, dwUsage, dwPool);
                    if (NULL == pDstRefVB)
                        goto dpvbpv_error;

                    // Optimization
                    if (RP_DPVB_OPT == dwAPI || RP_DIPVB_OPT == dwAPI)
                    {
                        if (FAILED(pSrcRefVB->Optimize(true)))
                            goto dpvbpv_error;
                    }

                    // Detect if lighting is required
                    if (0x0700 > m_dwVersion)
                        bLighting = (dwVertexTypeDesc & D3DFVF_NORMAL) ? true : false;
                    else
                        bLighting = m_bRPRefLighting;

                    // ProcessVertices
                    if (false == pDstRefVB->ProcessVertices(D3DVOP_TRANSFORM | D3DVOP_CLIP | 
                        ( (bLighting) ? D3DVOP_LIGHT : NULL),
                        0, dwVCount, pSrcRefVB, 0, true, 0))
                    {
                        goto dpvbpv_error;
                    }
                }

                // Rendering
                bRet = DrawPrimitiveVB(d3dptPrimitiveType, pDstSrcVB, pDstRefVB, 0, dwVCount, 
                                      pIndices, dwICount, dwFlags);

                if (pSrcSrcVB)     pSrcSrcVB->Release();
                if (pDstSrcVB)     pDstSrcVB->Release();
                if (pSrcRefVB)     pSrcRefVB->Release();
                if (pDstRefVB)     pDstRefVB->Release();
                break;

                // Cleanup after error
            dpvbpv_error:
                if (pSrcSrcVB)     pSrcSrcVB->Release();
                if (pDstSrcVB)     pDstSrcVB->Release();
                if (pSrcRefVB)     pSrcRefVB->Release();
                if (pDstRefVB)     pDstRefVB->Release();
                bRet = false;
            }
            break;    
        case RP_DPVB_PVS:
        case RP_DIPVB_PVS:
            {
                D3DDRAWPRIMITIVESTRIDEDDATA     VertexData;
                CVertexBuffer                   *pDstSrcVB = NULL;
                CVertexBuffer                   *pDstRefVB = NULL;
                DWORD                           dwDstVertexTypeDesc;
                DWORD                           dwVBFlags = D3DVBCAPS_SYSTEMMEMORY;
            
                // Filled strided data structure
                ConvertToStridedFormat(&VertexData, dwVertexTypeDesc, pVertices, dwVertexSize);

                // Set destination vertex type
                if (m_dwDestinationFVF)
                    dwDstVertexTypeDesc = m_dwDestinationFVF;
                else
                {
                    dwDstVertexTypeDesc = (dwVertexTypeDesc & ~(D3DFVF_POSITION_MASK | D3DFVF_NORMAL)) | 
                                            (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR);
                }

                // Destination vertex buffer for the source device
                pDstSrcVB = CreateVertexBuffer(dwDstVertexTypeDesc, dwVCount, dwVBFlags, false, dwUsage, dwPool);
                if (NULL == pDstSrcVB)
                    goto dpvbpvs_error;

                // ProcessVertices
                if (false == pDstSrcVB->ProcessVerticesStrided(D3DVOP_TRANSFORM | D3DVOP_CLIP | 
                    ( (m_bRPSrcLighting) ? D3DVOP_LIGHT : NULL),
                    0, dwVCount, &VertexData, dwVertexTypeDesc, false, 0))
                {
                    goto dpvbpvs_error;
                }

                if (IsReference())
                {
                    // Destination vertex buffer for the reference device
                    pDstRefVB = CreateVertexBuffer(dwDstVertexTypeDesc, dwVCount, dwVBFlags, true, dwUsage, dwPool);
                    if (NULL == pDstRefVB)
                        goto dpvbpvs_error;

                    // ProcessVertices
                    if (false == pDstRefVB->ProcessVerticesStrided(D3DVOP_TRANSFORM | D3DVOP_CLIP |
                        ( (m_bRPSrcLighting) ? D3DVOP_LIGHT : NULL),
                        0, dwVCount, &VertexData, dwVertexTypeDesc, true, 0))
                    {
                        goto dpvbpvs_error;
                    }
                }

                // Rendering
                bRet = DrawPrimitiveVB(d3dptPrimitiveType, pDstSrcVB, pDstRefVB, 0, dwVCount, 
                                       pIndices, dwICount, dwFlags);

                if (pDstSrcVB)     pDstSrcVB->Release();
                if (pDstRefVB)     pDstRefVB->Release();
                break;

                // Cleanup after error
            dpvbpvs_error:
                if (pDstSrcVB)     pDstSrcVB->Release();
                if (pDstRefVB)     pDstRefVB->Release();
                bRet = false;
            }
            break;
*/
        default:
            // Incorrect API
            bRet = false;
            break;
        }
    }

    return bRet;
}


LPVOID CDevice::CreateVertexArray(LPVOID lpVertices, DWORD  dwVertexCount, DWORD dwVertexSize,
                                  LPVOID lpIndices, DWORD  dwIndexCount, bool bIs32BitIndices)
{
    DWORD   dwRequiredMemory = dwIndexCount * dwVertexSize;
    LPBYTE  p;

    // Check size of currenty allocated memory and reallocate in necessary
    if (m_dwRPVertexArraySize < dwRequiredMemory)
    {
        if (NULL != m_pRPVertexArray)
            delete [] m_pRPVertexArray;
        m_pRPVertexArray = new BYTE [dwRequiredMemory];
        m_dwRPVertexArraySize = dwRequiredMemory;
        if (NULL == m_pRPVertexArray) 
        {
            m_dwRPVertexArraySize = 0;
            return NULL;
        }
    }

    // Dereference vertices to the allocated memory
    p = (LPBYTE)m_pRPVertexArray;
    if (bIs32BitIndices)
	{
		LPDWORD		lpdwIndices = (LPDWORD)lpIndices;
		for (DWORD i = 0; i < dwIndexCount; i++)
		{
			if (dwVertexCount < lpdwIndices[i])
				return NULL;
			memcpy(p, (((LPBYTE)lpVertices) + dwVertexSize*lpdwIndices[i]), 
				   dwVertexSize);
			p += dwVertexSize;
		}
	}
	else
	{
		LPWORD		lpwIndices = (LPWORD)lpIndices;
		for (DWORD i = 0; i < dwIndexCount; i++)
		{
			if (dwVertexCount < lpwIndices[i])
				return NULL;
			memcpy(p, (((LPBYTE)lpVertices) + dwVertexSize*lpwIndices[i]), 
				   dwVertexSize);
			p += dwVertexSize;
		}
	}
    return m_pRPVertexArray;
}

LPWORD CDevice::CreateIndexArray(DWORD dwSize, bool bIs32BitIndices)
{
    if (m_dwRPIndexArraySize < dwSize)
    {
        if (NULL != m_pRPIndexArray)
            delete [] m_pRPIndexArray;
        
		if (bIs32BitIndices)
		{
			m_pRPIndexArray = (LPWORD)new DWORD [dwSize];
			m_dwRPIndexArraySize = dwSize;
			if (NULL == m_pRPIndexArray)
			{
				m_dwRPIndexArraySize = 0;
				return NULL;
			}

			for (DWORD i = 0; i < dwSize; i++)
				((LPDWORD)m_pRPIndexArray)[i] = i;
		}
		else
		{
			m_pRPIndexArray = new WORD [dwSize];
			m_dwRPIndexArraySize = dwSize;
			if (NULL == m_pRPIndexArray)
			{
				m_dwRPIndexArraySize = 0;
				return NULL;
			}

			for (unsigned short i = 0; i < dwSize; i++)
				m_pRPIndexArray[i] = i;
		}
    }
    return m_pRPIndexArray;
}

void CDevice::ConvertToStridedFormat(LPDRAWPRIMITIVESTRIDEDDATA pVertexData, DWORD dwVertexTypeDesc,
                                      PVOID pVertices, DWORD dwVertexSize)
{
    static const DWORD              dwTextureSize[4] = {2*sizeof(D3DVALUE), 3*sizeof(D3DVALUE), 4*sizeof(D3DVALUE), sizeof(D3DVALUE)};
    LPBYTE                          pPtr = (LPBYTE)pVertices;
    DWORD                           dwCount;
    DWORD                           dwNumTexCoords;
    DWORD                           dwTextureFormats;

    
    // Fill strided structure
    ZeroMemory(pVertexData, sizeof(DRAWPRIMITIVESTRIDEDDATA));
    
    if (0 != (dwVertexTypeDesc & D3DFVF_RESERVED0))
        pPtr += sizeof(DWORD);
    
    pVertexData->position.lpvData = pPtr;
    pVertexData->position.dwStride = dwVertexSize;

    switch (dwVertexTypeDesc & D3DFVF_POSITION_MASK)
    {
        case    D3DFVF_XYZ:
                pPtr += sizeof(D3DVALUE) * 3;
                break;
        case    D3DFVF_XYZB1:
                pPtr += sizeof(D3DVALUE) * 4;
                break;
        case    D3DFVF_XYZB2:
                pPtr += sizeof(D3DVALUE) * 5;
                break;
        case    D3DFVF_XYZB3:
                pPtr += sizeof(D3DVALUE) * 6;
                break;
        case    D3DFVF_XYZB4:
                pPtr += sizeof(D3DVALUE) * 7;
                break;
        case    D3DFVF_XYZB5:
                pPtr += sizeof(D3DVALUE) * 8;
                break;
    }

    // Starting from DX8 RESERVED1 became PSIZE and we need to 
    // put it in this stream as well
    if (0 != (dwVertexTypeDesc & (D3DFVF_NORMAL | D3DFVF_RESERVED1)))
    {
        pVertexData->normal.lpvData = pPtr;
        pVertexData->normal.dwStride = dwVertexSize;
        if (0 != (dwVertexTypeDesc & D3DFVF_NORMAL))
            pPtr += sizeof(D3DVALUE) * 3;
        if (0 != (dwVertexTypeDesc & D3DFVF_RESERVED1))
            pPtr += sizeof(D3DVALUE);
    }

    if (0 != (dwVertexTypeDesc & D3DFVF_DIFFUSE))
    {
        pVertexData->diffuse.lpvData = pPtr;
        pVertexData->diffuse.dwStride = dwVertexSize;
        pPtr += sizeof(DWORD);
    }

    if (0 != (dwVertexTypeDesc & D3DFVF_SPECULAR))
    {
        pVertexData->specular.lpvData = pPtr;
        pVertexData->specular.dwStride = dwVertexSize;
        pPtr += sizeof(DWORD);
    }

    dwNumTexCoords = (dwVertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    dwTextureFormats = dwVertexTypeDesc >> CDIRECT3D_TEXTUREFORMATS_SHIFT;
    for (dwCount=0; dwCount < dwNumTexCoords; dwCount++)
    {
        pVertexData->textureCoords[dwCount].lpvData = pPtr;
        pVertexData->textureCoords[dwCount].dwStride = dwVertexSize;
        pPtr += dwTextureSize[dwTextureFormats & CDIRECT3D_TEXTUREFORMAT_MASK];
        dwTextureFormats >>= CDIRECT3D_TEXTUREFORMAT_SIZE;
    }
}


HRESULT CDevice::ConvertToVertexBuffer(DWORD dwFVF, DWORD dwVertexCount, PVOID pVertexData, DWORD dwStride, DWORD dwUsage, DWORD dwPool, bool bFVFVB, CVertexBuffer** ppVB)
{
    HRESULT			hr;
	BYTE*			pSrc = (BYTE*)pVertexData;
    BYTE*			pDst = NULL;
    DWORD			dwComponentSize = GetVertexSize(dwFVF);
    DWORD			dwCount = 0;
	CVertexBuffer*	pVB = NULL;
    
    hr = CreateVertexBuffer(dwComponentSize * dwVertexCount, dwUsage, ((bFVFVB) ? dwFVF : 0), dwPool, &pVB);
	if (FAILED(hr))
        return hr;
    hr = pVB->Lock(0, 0, &pDst, 0);
	if (FAILED(hr))
    {
        RELEASE(pVB);
        return hr;
    }
    for (; dwCount < dwVertexCount; dwCount++)
    {
        memcpy(pDst, pSrc, dwComponentSize);
        pDst += dwComponentSize;
        pSrc += dwStride;
    }
    pVB->Unlock();
	pVB->SetVertexFormat(dwFVF);
	*ppVB = pVB;
    return hr;
} 


DWORD CDevice::GetVertexSize(DWORD dwVertexTypeDesc)
{
    static const DWORD  dwTextureSize[4] = {2*sizeof(D3DVALUE), 3*sizeof(D3DVALUE), 4*sizeof(D3DVALUE), sizeof(D3DVALUE)};
    DWORD               dwVertexSize = 0;

    if (0 != (dwVertexTypeDesc & D3DFVF_POSITION_MASK))
    {
        switch (dwVertexTypeDesc & D3DFVF_POSITION_MASK)
        {
            case D3DFVF_XYZ:
                dwVertexSize += sizeof(D3DVALUE) * 3;
                break;
            case D3DFVF_XYZRHW:
                dwVertexSize += sizeof(D3DVALUE) * 4;
                break;
            case D3DFVF_XYZB1:
                dwVertexSize += sizeof(D3DVALUE) * 4;
                break;
            case D3DFVF_XYZB2:
                dwVertexSize += sizeof(D3DVALUE) * 5;
                break;
            case D3DFVF_XYZB3:
                dwVertexSize += sizeof(D3DVALUE) * 6;
                break;
            case D3DFVF_XYZB4:
                dwVertexSize += sizeof(D3DVALUE) * 7;
                break;
            case D3DFVF_XYZB5:
                dwVertexSize += sizeof(D3DVALUE) * 8;
                break;
            default:
                break;
        }
    }

    if (0 != (dwVertexTypeDesc & D3DFVF_RESERVED0))
        dwVertexSize += sizeof(DWORD);

    if (0 != (dwVertexTypeDesc & D3DFVF_RESERVED1))
        dwVertexSize += sizeof(DWORD);

    if (0 != (dwVertexTypeDesc & D3DFVF_NORMAL))
        dwVertexSize += sizeof(D3DVALUE) * 3;

    if (0 != (dwVertexTypeDesc & D3DFVF_DIFFUSE))
        dwVertexSize += sizeof(DWORD);

    if (0 != (dwVertexTypeDesc & D3DFVF_SPECULAR))
        dwVertexSize += sizeof(DWORD);

    // Texture formats:
	// 00 - D3DFVF_TEXTUREFORMAT2
	// 01 - D3DFVF_TEXTUREFORMAT3
	// 10 - D3DFVF_TEXTUREFORMAT4
	// 11 - D3DFVF_TEXTUREFORMAT1
    {
		DWORD dwTextureFormats = dwVertexTypeDesc >> CDIRECT3D_TEXTUREFORMATS_SHIFT;
		DWORD dwTexCoordSize = 0;
		DWORD dwNumTexCoords = ((dwVertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);
		if (0 == dwTextureFormats)
		{
			dwTexCoordSize = sizeof(D3DVALUE) * 2 * dwNumTexCoords;
		}
		else
		{
			for (DWORD i=0; i < dwNumTexCoords; i++)
			{
				dwTexCoordSize += dwTextureSize[dwTextureFormats & CDIRECT3D_TEXTUREFORMAT_MASK];
				dwTextureFormats >>= CDIRECT3D_TEXTUREFORMAT_SIZE;
			}
		}
		dwVertexSize += dwTexCoordSize; 
	}
    
    return dwVertexSize;
}


DWORD CDevice::GetPrimitiveCount(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD dwVertexCount)
{
    switch(d3dptPrimitiveType)
    {
    case D3DPT_TRIANGLELIST:        return dwVertexCount / 3;
    case D3DPT_TRIANGLESTRIP:       return dwVertexCount - 2;
    case D3DPT_TRIANGLEFAN:         return dwVertexCount - 2;
    case D3DPT_LINELIST:            return dwVertexCount / 2;
    case D3DPT_LINESTRIP:           return dwVertexCount - 1;
    case D3DPT_POINTLIST:           return dwVertexCount;
    default:                        return 0;
    }       
}

// Remap invalid API combinations
// - APIs not supported in current version
// - Indexed to non-indexed for point primitives
// - APIs that require untransformed vertices

DWORD CDevice::RemapAPI(DWORD dwCurrentAPI, D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD dwVertexTypeDesc)
{
    DWORD   dwAPI = dwCurrentAPI;

    // Do not change API if it was set with command line
    //if (m_bRPCommandLineOverwrite)
    //    return dwAPI;

    //DX5: Remap DX6 and later APIs to DrawPtimitive and DrawIndexedPrimitive
    if (5.f == m_fInterface)
    {
        switch (dwAPI)
        {
        case RP_DPS:
        case RP_DPVB:
        case RP_DPVB_PV:
        case RP_DPVB_OPT:
        case RP_DPVB_OPT_PV:
        case RP_DPVB_PVS:
            dwAPI = RP_DP;
            break;
        case RP_DIPS:
        case RP_DIPVB:
        case RP_DIPVB_PV:
        case RP_DIPVB_OPT:
        case RP_DIPVB_OPT_PV:
        case RP_DIPVB_PVS:    
            dwAPI = RP_DIP;
            break;
        }
    }

    // DX6: Remap DX7 and later APIs to DrawPrimitive and DrawIndexedPrimitive
    if (6.f == m_fInterface)
    {
        switch (dwAPI)
        {
        case RP_DPVB_PVS:   dwAPI = RP_DP;      break;
        case RP_DIPVB_PVS:  dwAPI = RP_DIP;     break;
        }
    }


    // DX7: Remap old interfaces to DrawPrimitive and DrawIndexedPrimitive
    if (7.f == m_fInterface)
    {
        switch (dwAPI)
        {
        case RP_BE:     dwAPI = RP_DP;      break;
        case RP_BIE:    dwAPI = RP_DIP;     break;
        }
    }

    // DX8: Remap old interfaces to DrawPrimitive and DrawIndexedPrimitive
    if (8.f <= m_fInterface)
    {
        switch (dwAPI)
        {
        case RP_BE:
        case RP_DPVB_PV:
        case RP_DPVB_OPT:
        case RP_DPVB_OPT_PV:
        case RP_DPVB_PVS:
            dwAPI = RP_DP;
            break;
        case RP_BIE:
        case RP_DIPVB_PV:
        case RP_DIPVB_OPT:
        case RP_DIPVB_OPT_PV:
        case RP_DIPVB_PVS:
            dwAPI = RP_DIP;
            break;
        }
    }

    // DX8: Remap multi-stream APIs to single-stream if devices do not support
    // enough streams
    if (8.f <= m_fInterface)
    {
        DWORD   dwStreams = 0;
        if (0 != (dwVertexTypeDesc & D3DFVF_POSITION_MASK))
            dwStreams += 1;
        if (0 != (dwVertexTypeDesc & (D3DFVF_NORMAL | D3DFVF_RESERVED1)))
            dwStreams += 1;
        if (0 != (dwVertexTypeDesc & D3DFVF_DIFFUSE))
            dwStreams += 1;
        if (0 != (dwVertexTypeDesc & D3DFVF_SPECULAR))
            dwStreams += 1;
        dwStreams += (dwVertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;

        if (m_Desc.dwMaxStreams < dwStreams)
        {
            switch(dwAPI)
            {
            case RP_DPS:
                dwAPI = RP_DPVB;
                break;
            case RP_DIPS:
                dwAPI = RP_DIPVB;
                break;
            }
        }
    }

    // Remap indexed APIs to non-indexed analogs for POINTLIST primitive
    if (D3DPT_POINTLIST == d3dptPrimitiveType)
    {
        switch (dwAPI)
        {
        case RP_BIE:            dwAPI = RP_BE;              break;
        case RP_DIP:            dwAPI = RP_DP;              break;
        case RP_DIPS:           dwAPI = RP_DPS;             break;
        case RP_DIPVB:          dwAPI = RP_DPVB;            break;
        case RP_DIPVB_PV:       dwAPI = RP_DPVB_PV;         break;
        case RP_DIPVB_OPT:      dwAPI = RP_DPVB_OPT;        break;
        case RP_DIPVB_OPT_PV:   dwAPI = RP_DPVB_OPT_PV;     break;
        case RP_DIPVB_PVS:      dwAPI = RP_DPVB_PVS;        break;
        }
    }

    // Remap APIs that require untransfortmed vertices
    if (8.f <= m_fInterface)
    {
        if (D3DFVF_XYZRHW == (dwVertexTypeDesc & D3DFVF_POSITION_MASK))
        {
            switch (dwAPI)
            {
            case RP_DPS:
            case RP_DPVB_PVS:
            case RP_DPVB_PV:
            case RP_DPVB_OPT:
            case RP_DPVB_OPT_PV:
                dwAPI = RP_DP;
                break;
            case RP_DIPS:
            case RP_DIPVB_PVS:
            case RP_DIPVB_PV:
            case RP_DIPVB_OPT:
            case RP_DIPVB_OPT_PV:
                dwAPI = RP_DIP;
                break;
            }
        }
    }
    else
    {
        if (D3DFVF_XYZRHW == (dwVertexTypeDesc & D3DFVF_POSITION_MASK))
        {
            switch (dwAPI)
            {
            case RP_DPS:
            case RP_DPVB_PVS:
                dwAPI = RP_DP;
                break;
            case RP_DIPS:
            case RP_DIPVB_PVS:
                dwAPI = RP_DIP;
                break;
            case RP_DPVB_PV:
            case RP_DPVB_OPT:
            case RP_DPVB_OPT_PV:
                dwAPI = RP_DPVB;
                break;
            case RP_DIPVB_PV:
            case RP_DIPVB_OPT:
            case RP_DIPVB_OPT_PV:
                dwAPI = RP_DIPVB;
                break;
            }
        }
    }

	// Disable ProcessVertices and ProcessVerticesStrided APIs
	switch (dwAPI)
	{
    case RP_DPVB_PV:
    case RP_DPVB_OPT_PV:
    case RP_DPVB_PVS:
		dwAPI = RP_DP;
		break;

    case RP_DIPVB_PV:
    case RP_DIPVB_OPT_PV:
    case RP_DIPVB_PVS:
		dwAPI = RP_DIP;
		break;
	}

    return dwAPI;
}

DWORD CDevice::ProcessDrawPrimitiveFlags(DWORD dwFlags)
{
    DWORD	dwValue;
    if (m_fInterface >= 7.0f)
    {
        // Clipping render state
        if (D3DDP_DONOTCLIP & dwFlags)
        {
			SetLastError(GetRenderState(D3DRENDERSTATE_CLIPPING, &dwValue));
            if (TRUE == dwValue)
            {
                SetRenderState(D3DRENDERSTATE_CLIPPING, (DWORD)FALSE);
                m_fClippingRSChanged = true;
            }
            dwFlags &= ~D3DDP_DONOTCLIP;
        }
        
		// Lighting render state
		if (D3DDP_DONOTLIGHT & dwFlags)
		{
			SetLastError(GetRenderState(D3DRENDERSTATE_LIGHTING, &dwValue));
			if (TRUE == dwValue)
			{
				SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)FALSE);
				m_fLightingRSChanged = true;
			}
			dwFlags &= ~D3DDP_DONOTLIGHT;
        }
        
        // Extents render state
        if (D3DDP_DONOTUPDATEEXTENTS & dwFlags)
        {
			SetLastError(GetRenderState(D3DRENDERSTATE_EXTENTS, &dwValue));
			if (TRUE == dwValue)
			{
				SetRenderState(D3DRENDERSTATE_EXTENTS, (DWORD)FALSE);
				m_fExtentsRSChanged = true;
			}
			dwFlags &= ~D3DDP_DONOTUPDATEEXTENTS;
        }
    }

    return dwFlags;
}

void CDevice::RestoreDrawPrimitiveFlags(void)
{
    if (m_fInterface >= 7.0f)
    {
        if (m_fClippingRSChanged)
        {
            SetRenderState(D3DRENDERSTATE_CLIPPING, (DWORD)TRUE);
            m_fClippingRSChanged = false;
        }
        if (m_fLightingRSChanged)
        {
            SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)TRUE);
            m_fLightingRSChanged = false;
        }
        if (m_fExtentsRSChanged)
        {
            SetRenderState(D3DRENDERSTATE_EXTENTS, (DWORD)TRUE);
            m_fExtentsRSChanged = false;
        }
    }
}


