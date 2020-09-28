#include <d3d9.h>
#include "clear.h"

bool CClearTest::CreateOffScrRendTrgts(int nTexFormat)
{
    HRESULT hr;

/*    if (m_dwVersion <= 0x0700)
    {
		// this part to be removed.
		return false;
    }
    else // m_dwVersion >= 0x0800
*/  {
        hr = m_pSrcDevice->CreateRenderTarget( IMGWIDTH, 
                                               IMGHEIGHT,
                                               m_pCommonTextureFormats[nTexFormat],
                                               MULTISAMPLE_NONE,
                                               true,
                                               &m_pOffSrcSurf); 
        if (FAILED(hr))
        {
	        WriteToLog("Src CreateRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
		    return false;
	    }

    }


/*    if (m_dwVersion <= 0x0700)
    {
		// this part to be removed.
		return false;
    }
    else // m_dwVersion >= 0x0800
*/  {
        hr = m_pRefDevice->CreateRenderTarget(IMGWIDTH, 
											   IMGHEIGHT,
                                               m_pCommonTextureFormats[nTexFormat],
                                               MULTISAMPLE_NONE,
                                               true,
                                               &m_pOffRefSurf); 
        if (FAILED(hr))
        {
	        WriteToLog("Ref CreateRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
		    RELEASE(m_pOffSrcSurf);
		    return false;
	    }

    }


	return true;
}










bool CClearTest::CreateZStencil(int nZFormat)
{
	HRESULT hr;

/*	if (m_dwVersion <= 0x0700)
	{
		//to be removed
		return false;
	} 
	else // m_dwVersion >= 0x0800
*/	{
		hr = m_pSrcDevice->CreateDepthStencilSurface(IMGWIDTH, 
													  IMGHEIGHT, 
													  m_pCommonZBufferFormats[nZFormat], 
													  MULTISAMPLE_NONE, 
													  &m_pSrcZ);
		if (FAILED(hr))
		{
    		WriteToLog("Src CreateDepthStencilSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
			return false;
		}
	}



/*	if (m_dwVersion <= 0x0700)
	{
		//to be removed
		return false;
	} 
	else // m_dwVersion >= 0x0800
*/	{
		hr = m_pRefDevice->CreateDepthStencilSurface(IMGWIDTH, 
													  IMGHEIGHT, 
													  m_pCommonZBufferFormats[nZFormat], 
													  MULTISAMPLE_NONE, 
													  &m_pRefZ);
		if (FAILED(hr))
		{
    		WriteToLog("Ref CreateDepthStencilSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
			RELEASE(m_pSrcZ);
			return false;
		}
	}


	return true;
}



bool CClearTest::CopyToTexture(void)
{
    HRESULT hr;

    RECT Rect;
	CnSurface *pCnS=NULL;
	CSurface *pTexSrc=NULL, *pTexRef=NULL;

    Rect.left = 0;
	Rect.top  = 0;
	Rect.right  = IMGWIDTH;
	Rect.bottom = IMGHEIGHT;

	m_pTextureRT->GetSurfaceLevel( 0, &pCnS);
	if (pCnS)
	{
		pCnS->GetSurface(&pTexSrc, 0);
		pCnS->GetSurface(&pTexRef, 1);
		RELEASE(pCnS);
	}
	else
	{
		WriteToLog("(FATAL ERROR): GetSurfaceLevel from CnTexture failed!\n");
		return FALSE;
	}

	if ( (pTexSrc==NULL) || (pTexRef==NULL) )
	{
		RELEASE(pTexSrc);
		RELEASE(pTexRef);
		return FALSE;
	}

    hr = m_pSrcDevice->CopyRects(m_pOffSrcSurf, &Rect, 1, pTexSrc, NULL);

    if (FAILED(hr))
    {
        WriteToLog("Src CopyRects() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
		RELEASE(pTexSrc);
		RELEASE(pTexRef);
        return false;
    }

    hr = m_pRefDevice->CopyRects(m_pOffRefSurf, &Rect, 1, pTexRef, NULL);

    if (FAILED(hr))
    {
        WriteToLog("Ref CopyRects() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
		RELEASE(pTexSrc);
		RELEASE(pTexRef);
        return false;
    }

	RELEASE(pTexSrc);
	RELEASE(pTexRef);

    return true;
}
