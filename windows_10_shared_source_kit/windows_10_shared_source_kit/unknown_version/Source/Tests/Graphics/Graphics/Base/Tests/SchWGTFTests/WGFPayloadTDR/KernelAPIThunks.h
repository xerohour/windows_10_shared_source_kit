/******************************Module*Header*******************************
* Module Name: WGFPayloadTDR.h
*
* D3D11 Graphics and Compute TDR workload test
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#pragma once


// When this object is created, it opens the adapter so the caller can send it escapes.
// It uses thunks out of gdi32.       
class CKernelAPIThunks
{
public:
    CKernelAPIThunks();
    ~CKernelAPIThunks();

    HRESULT QueryForCapabilities();

    bool IsSoftwareDevice(bool _bRender)
    {
        if( _bRender )
        {
            return m_bren_software;
        }
        return m_bdis_software;
    }
    bool IsRenderOnly(bool _bRender)
    {
        if( _bRender )
        {
            return m_bren_renderOnly;
        }
        return m_bdis_renderOnly;
    }
    bool IsDisplayOnly(bool _bRender)
    {
        if( _bRender )
        {
            return m_bren_displayOnly;
        }
        return m_bdis_displayOnly;
    }
    bool IsRenderAndDisplay(bool _bRender)
    {
        if( _bRender )
        {
            return m_bren_renderAndDisplay;
        }
        return m_bdis_renderAndDisplay;
    }
    bool IsPostDevice(bool _bRender)
    {
        if( _bRender )
        {
            return m_bren_post;
        }
        return m_bdis_post;
    }
    bool IsBDAorBDD(bool _bRender)
    {
        if( _bRender )
        {
            return m_bren_basicDisplay;
        }
        return m_bdis_basicDisplay;
    }

    // helper functions for preemption functionality
    D3DKMDT_GRAPHICS_PREEMPTION_GRANULARITY GetGraphicsPreemptionGranularity(bool _bRender)
    {
        if( _bRender )
        {
            return m_WDDMCapsRen.PreemptionCaps.GraphicsPreemptionGranularity;
        }
        return m_WDDMCapsDis.PreemptionCaps.GraphicsPreemptionGranularity;
    }
    D3DKMDT_COMPUTE_PREEMPTION_GRANULARITY GetComputePreemptionGranularity(bool _bRender)
    {
        if( _bRender )
        {
            return m_WDDMCapsRen.PreemptionCaps.ComputePreemptionGranularity;
        }
        return m_WDDMCapsDis.PreemptionCaps.ComputePreemptionGranularity;
    }
    wchar_t* GraphicsPreemptionGranularityStr(bool _bRender);
    wchar_t* ComputePreemptionGranularityStr(bool _bRender);

    bool SupportPerEngineTDR(bool _bRender)
    {
        if( _bRender )
        {
            return (m_WDDMCapsRen.SupportPerEngineTDR ? true:false);
        }
        return (m_WDDMCapsDis.SupportPerEngineTDR ? true:false);
    }

    void GetCaps(D3DKMT_WDDM_1_2_CAPS* _pCaps,bool _bRender)
    {
        if( _bRender )
        {
            *_pCaps = m_WDDMCapsRen;
        }
        else
        {
            *_pCaps = m_WDDMCapsDis;
        }
    }

private:

    // The Default caps will always be Render. If we're running on a software device,
    // we will also have capabilities for the display device.
    D3DKMT_WDDM_1_2_CAPS m_WDDMCapsDis;
    bool m_bdis_software;
    bool m_bdis_renderOnly;
    bool m_bdis_displayOnly;
    bool m_bdis_renderAndDisplay;
    bool m_bdis_post;
    bool m_bdis_basicDisplay;

    D3DKMT_WDDM_1_2_CAPS m_WDDMCapsRen;
    bool m_bren_software;
    bool m_bren_renderOnly;
    bool m_bren_displayOnly;
    bool m_bren_renderAndDisplay;
    bool m_bren_post;
    bool m_bren_basicDisplay;
};
 
 