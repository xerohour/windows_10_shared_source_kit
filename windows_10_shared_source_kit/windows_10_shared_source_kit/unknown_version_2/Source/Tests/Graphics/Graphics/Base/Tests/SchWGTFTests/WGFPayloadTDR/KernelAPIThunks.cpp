/******************************Module*Header*******************************
* Module Name: KernelAPIThunks.cpp
*
* Class for thunk layer stuff
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#include "internal.h"

#include "KernelAPIThunkTypes.h"
#include "KernelAPIThunks.h"

#include "AdapterUtil.h"
using namespace DevX::GRFX;

//------------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// This class is used to get information through the thunk layer about the adapter
// and to get faults to the kernel.
//////////////////////////////////////////////////////////////////////////


CKernelAPIThunks::CKernelAPIThunks() :
    m_bren_software(false),
    m_bren_renderOnly(false),
    m_bren_displayOnly(false),
    m_bren_renderAndDisplay(false),
    m_bren_post(false),
    m_bren_basicDisplay(false),
    m_bdis_software(false),
    m_bdis_renderOnly(false),
    m_bdis_displayOnly(false),
    m_bdis_renderAndDisplay(false),
    m_bdis_post(false),
    m_bdis_basicDisplay(false)
{
};
CKernelAPIThunks::~CKernelAPIThunks()
{
};

HRESULT CKernelAPIThunks::QueryForCapabilities()
{
    HRESULT hr = S_OK;
    const AdapterDescriptor* pad = NULL;
    AdapterUtil* putil = NULL;

    putil = AdapterUtil::CreateAdapterUtil();
    if( NULL == putil )
    {
        // Failed to create our adapter type
        return E_FAIL;
    }
    pad = putil->GetPrimaryAdapter();
    if( NULL == putil )
    {
        // Failed to create our adapter descriptor
        return E_FAIL;
    }
    hr = pad->GetAdapterWDDM1_2Caps(m_WDDMCapsRen);
    if( S_OK != hr )
    {
        return hr;
    }

    // Other properties we might want.
    hr = pad->IsSoftwareDevice(m_bren_software);
    if( S_OK != hr )
    {
        // failed to get characteristic of primary device
        return hr;
    }
    hr = pad->IsRenderOnly(m_bren_renderOnly);
    if( S_OK != hr )
    {
        // failed to get characteristic of primary device
        return hr;
    }
    hr = pad->IsDisplayOnly(m_bren_displayOnly);
    if( S_OK != hr )
    {
        // failed to get characteristic of primary device
        return hr;
    }
    hr = pad->IsRenderAndDisplay(m_bren_renderAndDisplay);
    if( S_OK != hr )
    {
        // failed to get characteristic of primary device
        return hr;
    }
    hr = pad->IsPostDevice(m_bren_post);
    if( S_OK != hr )
    {
        // failed to get characteristic of primary device
        return hr;
    }
    hr = pad->IsBDAorBDD(m_bren_basicDisplay);
    if( S_OK != hr )
    {
        // failed to get characteristic of primary device
        return hr;
    }

    // We always assume that the display and render caps are the same. The only time
    // this is not the case is when we're dealing with a display only device. when that happens
    // we'll look for the render device and get specific render caps from it.
    m_WDDMCapsDis = m_WDDMCapsRen;
    m_bdis_software = m_bren_software;
    m_bdis_renderOnly = m_bren_renderOnly;
    m_bdis_displayOnly = m_bren_displayOnly;
    m_bdis_renderAndDisplay = m_bren_renderAndDisplay;
    m_bdis_post = m_bren_post;
    m_bdis_basicDisplay = m_bren_basicDisplay;

    // If we have a display only device, we'll need to find the render device associated 
    // with it. To do that, we'll loop through the list of adapters looking for the
    // correct match. The reason for this is that the render device has the correct
    // WDDM characteristics.
    if( m_bdis_displayOnly )
    {
        putil = AdapterUtil::CreateAdapterUtil();

        std::vector<AdapterDescriptor*> adapters = putil->GetAllAdapters();

        std::wstring BasicRender (L"ROOT\\BasicRender");

        //Enumerate all devices
        for (std::vector<AdapterDescriptor*>::iterator i = adapters.begin();
            i != adapters.end();
            ++i)
        {
            AdapterDescriptor* adapter = *i;
            DXGI_ADAPTER_DESC1 Desc = {0};

            std::vector<std::wstring> HardwareIds = adapter->GetHardwareIds();

            for(std::vector<std::wstring>::iterator y = HardwareIds.begin(); y != HardwareIds.end(); y++)
            {
                if(*y == BasicRender)
                {
                    // now that we've found our basic render device, get the render flags and
                    // capabilities
                    hr = adapter->IsSoftwareDevice(m_bren_software);
                    if( S_OK != hr )
                    {
                        // failed to get characteristic of primary device
                        return hr;
                    }
                    hr = adapter->IsRenderOnly(m_bren_renderOnly);
                    if( S_OK != hr )
                    {
                        // failed to get characteristic of primary device
                        return hr;
                    }
                    hr = adapter->IsDisplayOnly(m_bren_displayOnly);
                    if( S_OK != hr )
                    {
                        // failed to get characteristic of primary device
                        return hr;
                    }
                    hr = adapter->IsRenderAndDisplay(m_bren_renderAndDisplay);
                    if( S_OK != hr )
                    {
                        // failed to get characteristic of primary device
                        return hr;
                    }
                    hr = adapter->IsPostDevice(m_bren_post);
                    if( S_OK != hr )
                    {
                        // failed to get characteristic of primary device
                        return hr;
                    }
                    hr = adapter->IsBDAorBDD(m_bren_basicDisplay);
                    if( S_OK != hr )
                    {
                        // failed to get characteristic of primary device
                        return hr;
                    }

                    // here is where we want to query for capabilities
                    hr = adapter->GetAdapterWDDM1_2Caps(m_WDDMCapsRen);
                    if( S_OK != hr )
                    {
                        return hr;
                    }
                    break;
                }
            }
        }
    }
    return S_OK;
}

#define CASE_STR(_val_) case _val_: pValue = L#_val_; break;

wchar_t* CKernelAPIThunks::GraphicsPreemptionGranularityStr(bool _bRender)
{
    wchar_t* pValue=NULL;

    if( _bRender )
    {
        switch(m_WDDMCapsRen.PreemptionCaps.GraphicsPreemptionGranularity)
        {
            CASE_STR(D3DKMDT_GRAPHICS_PREEMPTION_NONE);
            CASE_STR(D3DKMDT_GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY);
            CASE_STR(D3DKMDT_GRAPHICS_PREEMPTION_PRIMITIVE_BOUNDARY);
            CASE_STR(D3DKMDT_GRAPHICS_PREEMPTION_TRIANGLE_BOUNDARY);
            CASE_STR(D3DKMDT_GRAPHICS_PREEMPTION_PIXEL_BOUNDARY);
            CASE_STR(D3DKMDT_GRAPHICS_PREEMPTION_SHADER_BOUNDARY);
        default:
            pValue = L"Unknown Graphics Preemption Level";
            break;
        }
    }
    else
    {
        switch(m_WDDMCapsDis.PreemptionCaps.GraphicsPreemptionGranularity)
        {
            CASE_STR(D3DKMDT_GRAPHICS_PREEMPTION_NONE);
            CASE_STR(D3DKMDT_GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY);
            CASE_STR(D3DKMDT_GRAPHICS_PREEMPTION_PRIMITIVE_BOUNDARY);
            CASE_STR(D3DKMDT_GRAPHICS_PREEMPTION_TRIANGLE_BOUNDARY);
            CASE_STR(D3DKMDT_GRAPHICS_PREEMPTION_PIXEL_BOUNDARY);
            CASE_STR(D3DKMDT_GRAPHICS_PREEMPTION_SHADER_BOUNDARY);
        default:
            pValue = L"Unknown Graphics Preemption Level";
            break;
        }
    }

    return pValue;
}

wchar_t* CKernelAPIThunks::ComputePreemptionGranularityStr(bool _bRender)
{
    wchar_t* pValue=NULL;

    if( _bRender )
    {
        switch(m_WDDMCapsRen.PreemptionCaps.ComputePreemptionGranularity)
        {
            CASE_STR(D3DKMDT_COMPUTE_PREEMPTION_NONE);
            CASE_STR(D3DKMDT_COMPUTE_PREEMPTION_DMA_BUFFER_BOUNDARY);
            CASE_STR(D3DKMDT_COMPUTE_PREEMPTION_DISPATCH_BOUNDARY);
            CASE_STR(D3DKMDT_COMPUTE_PREEMPTION_THREAD_GROUP_BOUNDARY);
            CASE_STR(D3DKMDT_COMPUTE_PREEMPTION_THREAD_BOUNDARY);
            CASE_STR(D3DKMDT_COMPUTE_PREEMPTION_SHADER_BOUNDARY);
        default:
            pValue = L"Unknown Compute Preemption Level";
            break;
        }
    }
    else
    {
        switch(m_WDDMCapsDis.PreemptionCaps.ComputePreemptionGranularity)
        {
            CASE_STR(D3DKMDT_COMPUTE_PREEMPTION_NONE);
            CASE_STR(D3DKMDT_COMPUTE_PREEMPTION_DMA_BUFFER_BOUNDARY);
            CASE_STR(D3DKMDT_COMPUTE_PREEMPTION_DISPATCH_BOUNDARY);
            CASE_STR(D3DKMDT_COMPUTE_PREEMPTION_THREAD_GROUP_BOUNDARY);
            CASE_STR(D3DKMDT_COMPUTE_PREEMPTION_THREAD_BOUNDARY);
            CASE_STR(D3DKMDT_COMPUTE_PREEMPTION_SHADER_BOUNDARY);
        default:
            pValue = L"Unknown Compute Preemption Level";
            break;
        }
    }
    return pValue;
}


