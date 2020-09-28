//+-------------------------------------------------------------------------
//
//
// Copyright (C) Microsoft. All rights reserved.
//
//  File:       KSTest.cpp
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------;
// KSTest.cpp
// Copied over the test class from PCAudReq for testing using KS devices
//--------------------------------------------------------------------------;

#include "common.h"

//--------------------------------------------------------------------------;
//
//  CKsTest
//
//  Description:
//      Constructor
//
//  Arguments:
//      (void)
//
//  Return:
//      (void)
//
//--------------------------------------------------------------------------;
CKsTest::CKsTest () :
    m_pFilter(NULL)
{}

//--------------------------------------------------------------------------;
//
//  CKsTest
//
//  Description:
//      Destructor
//
//  Arguments:
//      (void)
//
//  Return:
//      (void)
//
//--------------------------------------------------------------------------;
CKsTest::~CKsTest ()
{
}

//--------------------------------------------------------------------------;
//
//  Initialize
//
//  Description:
//      Initialize
//
//  Arguments:
//      (void)
//
//  Return:
//      (void)
//
//--------------------------------------------------------------------------;
BOOL CKsTest::Initialize (CPCMAudioFilter* pFilter)
{
    if (NULL == pFilter)
    {
        XLOG(XMSG, eError, _T("Failed to find a Wave Filter"));
        return FALSE;
    }
 
    m_pFilter = pFilter;
    return TRUE;
} // Initialize

//--------------------------------------------------------------------------;
//
//  CreateRender
//
//  Description:
//      Creates the render pin.
//
//  Arguments:
//      (void)
//
//  Return:
//      (void)
//
//--------------------------------------------------------------------------;
BOOL CKsTest::CreateRender(DWORD dwStreamTime)
{
    // get a logger
    CComPtr<IBasicLog> pBasicLog;
    HRESULT hr = g_IShell->GetBasicLog(&pBasicLog);
    
    if (S_OK != hr)
    {
        XLOG(XFAIL,
            eError,
            _T("FAIL: Error getting basic log object: 0x%08x"), hr);
        return FALSE;
    }
    
    if(dwStreamTime <= 0)    { return FALSE; }
    m_dwStreamTime = dwStreamTime;

    if ((NULL == m_pFilter) || m_pFilter->m_listRenderSinkPins.IsEmpty())
    {
        XLOG(XMSG, eError, _T("Failed to find an appropriate pin for render"));
        return FALSE;
    }

    if(!m_pFilter->CreateRenderPin(m_dwStreamTime, m_pFilter->m_fRTPort)) 
    {
        XLOG(XMSG, eError, _T("Failed to create Render Pin"));
        return FALSE;
    }

    // Fill render buffer.
    WAVEFORMATEX *pWfx = ((CPCMAudioPin*)m_pFilter->m_pRenderPin)->GetFormat();

    hr = FillBufferWithSineSignal(
        pBasicLog, XFAIL,
        pWfx,
        1.0, // amplitude
        TEST_FREQUENCY,
        0.0, // initial phase,
        0.0, // dc
        Method_NoDithering,
        m_pFilter->m_pRenderPin->m_pbStreamData,
        m_pFilter->m_pRenderPin->m_cbStreamData / pWfx->nBlockAlign,
        m_pFilter->m_pRenderPin->m_cbStreamData
    );

    if (S_OK != hr)
    {
        XLOG(XFAIL,
            eError,
            _T("FAIL: Could not fill buffer with sine: 0x%08x"), hr);
        return FALSE;
    }

    return TRUE;
} // CreateRender

//--------------------------------------------------------------------------;
//
//  DestroyRender
//
//  Description:
//      Closes render pin
//
//  Arguments:
//      (void)
//
//  Return:
//      (void)
//
//--------------------------------------------------------------------------;
BOOL CKsTest::DestroyRender ()
{
    if ((NULL != m_pFilter) && (NULL != m_pFilter->m_pRenderPin))
    {
        m_pFilter->m_pRenderPin->ClosePin();
    }

    return TRUE;
} // DestroyRender

