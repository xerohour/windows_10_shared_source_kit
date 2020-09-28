#include "stdafx.h"

extern DWORD g_syncronizationTimeout;


//
// CodePathStep 9: 
//
//  Create our object.
//
CUser::CUser(HWND hwnd) : m_syncronizationTimeout(120000)
{
    m_hUser     = NULL;
    m_hOwner    = NULL;
    m_pSharedObject = NULL;
    m_hWnd      = hwnd;
    m_hRoundRobinUser = NULL;
    m_hRoundRobinOwner = NULL;
}


//
// CodePathStep 10:
//
//  Creating the same set of events as done in the COwner object gets us the shared
// events that we can use. As with the COwner object, CUser gets a pointer to the 
// Message structure (m_lpMapAddr). When done, the CUser object should be ready to
// Run.
//
HRESULT CUser::Initialize(BOOL bWindowed)
{    
    HRESULT hr = S_OK;
    DeviceCreateOptions Opts;
    bool bLDDM = false;

    DBG_MSG(L"Enter");

    // Check to see if it is an LDDM machine. 
    try
    {
        bLDDM = I3DIsLDDM();
    }
    catch(...)
    {
        hr = ERROR_GRAPHICS_INVALID_DRIVER_MODEL;
    }
    if(FAILED(hr))
    {
        goto INITFAIL;
    }

    if(false == bLDDM)
    {
        hr = E_OUTOFMEMORY;
        goto INITFAIL;
    }

    m_hOwner = CreateEvent(NULL, FALSE, FALSE, L"Global\\CrossProc::Event_Owner");
    if(m_hOwner == NULL)
    {
        DBG_MSG(L"Unable to create owner event\n");
        hr = E_UNEXPECTED;
        goto INITFAIL;
    }

    m_hUser  = CreateEvent(NULL, FALSE, FALSE, L"Global\\CrossProc::Event_User");
    if(m_hUser == NULL)
    {
        DBG_MSG(L"Unable to create user event \n");
        hr = E_UNEXPECTED;
        goto INITFAIL;
    }

    m_hRoundRobinUser = CreateEvent(NULL, FALSE, FALSE, L"CrossProc::Event_RoundRobinUser");
    if(m_hRoundRobinUser == NULL)
    {
        DBG_MSG(L"Unable to create RoundRobin user event \n");
        hr = E_UNEXPECTED;
        goto INITFAIL;
    }
    
    m_hRoundRobinOwner = CreateEvent(NULL, FALSE, FALSE, L"CrossProc::Event_RoundRobinOwner");
    if(m_hRoundRobinOwner == NULL)
    {
        DBG_MSG(L"Unable to create RoundRobin owner event \n");
        hr = E_UNEXPECTED;
        goto INITFAIL;
    }

    // Now get our shared memory object.
    m_pSharedObject = new sSharedMemory();
    if(m_pSharedObject == NULL)
    {
        PATH_TRAP();
        DBG_MSG(L"Unable to create shared memory region object");
        hr = E_OUTOFMEMORY;
        goto INITFAIL;
    }
    hr = m_pSharedObject->Initialize(NULL,g_syncronizationTimeout);
    if(FAILED(hr))
    {
        PATH_TRAP();
        DBG_MSG(L"Failed to Initialize shared memory object");
        hr = E_FAIL;
        goto INITFAIL;
    }

    try
    {
        hr = CreateI3D(&m_p3D.p, NULL);
    }
    catch(...)
    {
        hr = E_NOTIMPL;
    }

    if(FAILED(hr))
    {
        DBG_MSG(L"Failed to create I3D \n");
        goto INITFAIL;
    }
    
    hr = m_p3D->GetValidDeviceCreateOptions( bWindowed ? true : false,//run in windowed mode
                                             m_hWnd,//the render window
                                             WIDTH,//the back buffer width
                                             HEIGHT,//the back buffer height
                                             1,//the back buffer count
                                             true,//yes, we want a depth buffer
                                             false,
                                             NULL,
                                             &Opts);

    if(FAILED(hr))
    {
        DBG_MSG(L"Failed to GetValidDeviceCreateOptions \n");
        goto INITFAIL;
    }
    
    hr = m_p3D->CreateDevice(&Opts, &m_pDevice.p);
    if(FAILED(hr))
    {   
        DBG_MSG(L"Failed to CreateDevice");
        goto INITFAIL;
    }

INITFAIL:

    //we need to go ahead and SetEvents here that the creating process is expecting to receive
    if(FAILED(hr))
    {
        SetEvent(m_hUser); 
        if(WAIT_OBJECT_0 != WaitForSingleObject(m_hOwner, m_syncronizationTimeout))
        {
            hr = E_FAIL;
        }
        else
        {
            SetEvent(m_hUser); 
        }
    }
    DBG_MSG(L"Leave");
    return hr;
}


HRESULT CUser::Cleanup()
{
    HRESULT hr = S_OK;

    if( m_pSharedObject )
    {
        delete m_pSharedObject;
        m_pSharedObject = NULL;
    }

    if(m_hOwner)
        CloseHandle(m_hOwner);
    if(m_hUser)
        CloseHandle(m_hUser);
    if(m_hRoundRobinOwner)
        CloseHandle(m_hRoundRobinOwner);
    if(m_hRoundRobinUser)
        CloseHandle(m_hRoundRobinUser);
    return hr;

}


HRESULT CUser::XProcTest_IndexBuffer_Open()
{
    HRESULT hr = S_OK;      
    CI3DIndexBufferPtr pIB;

    PATH_TRAP();
    // We need to lock access to our message info.
    Message*msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    hr = m_pDevice->CreateIndexBuffer(&pIB.p, 
                                      msg->lCount, 
                                      I3DSURF_SHAREDRESOURCE, 
                                      D3DDDIFMT_INDEX16, 
                                      D3DDDIPOOL_VIDEOMEMORY, 
                                      msg->hTexHandle);
    msg->hrUser = hr;

TESTFAIL:
    if( msg )
    {
        if( FAILED(hr) )
        {
            USER_RET_ERR(msg,L"Failed CreateIndexBuffer()");
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    return hr;                                      
}


HRESULT CUser::XProcTest_VertexBuffer_Open()
{
    HRESULT hr = S_OK;      
    CI3DVertexBufferPtr pVB;

    PATH_TRAP();
    // We need to lock access to our message info.
    Message*msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    hr = m_pDevice->CreateVertexBuffer(&pVB.p, 
                                       VERTEXCOUNT * sizeof(MyVertex), 
                                       I3DSURF_SHAREDRESOURCE, 
                                       MYFVF, 
                                       D3DDDIPOOL_VIDEOMEMORY, 
                                       msg->hTexHandle);
    msg->hrUser = hr;
    
TESTFAIL:
    if( msg )
    {
        if( FAILED(hr) )
        {
            USER_RET_ERR(msg,L"Failed CreateVertexBuffer()");
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    return hr;                                      
}


HRESULT CUser::XProcTest_Basic_Open()
{
    DBG_MSG(L"User::XProcTest_Basic_Open called");
    
    HRESULT hr = S_OK;      

    CI3DTexturePtr pTexture;

    // We need to lock access to our message info.
    Message* msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    hr = m_pDevice->CreateTexture(&pTexture.p,
                                  msg->uiWidth ,
                                  msg->uiHeight,
                                  msg->uiLevels,
                                  I3DSURF_SHAREDRESOURCE,
                                  msg->d3dFormat,
                                  D3DDDIPOOL_VIDEOMEMORY,
                                  msg->hTexHandle);

    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"XProcTest_Basic_Open failed to create texture");
    }
    else
    {
        DBG_MSG(L"crossproc.exe :: XProcTest_Basic_Open created texture successfully");
    }
    
    msg->hrUser = hr;

TESTFAIL:
    if( msg )
    {
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    return hr;
}


HRESULT CUser::XProcTest_InvalidFormat()
{
    HRESULT hr = S_OK;
    CI3DTexturePtr pTexture;

    PATH_TRAP();
    // We need to lock access to our message info.
    Message* msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    hr = m_pDevice->CreateTexture(&pTexture.p,
                                  msg->uiWidth ,
                                  msg->uiHeight,
                                  msg->uiLevels,
                                  I3DSURF_SHAREDRESOURCE,
                                  msg->d3dFormat,
                                  D3DDDIPOOL_VIDEOMEMORY,
                                  msg->hTexHandle);
    msg->hrUser = hr;

TESTFAIL:
    if( msg )
    {
        PATH_TRAP();
        if( FAILED(hr) )
        {
            USER_RET_ERR(msg,L"Failed CreateTexture()");
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    PATH_TRAP();
    return hr;
}


HRESULT CUser::XProcTest_InvalidLevel()
{
    HRESULT hr = S_OK;
    
    CI3DTexturePtr pTexture;

    // We need to lock access to our message info.
    Message* msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    hr = m_pDevice->CreateTexture(&pTexture.p,
                                  msg->uiWidth ,
                                  msg->uiHeight,
                                  msg->uiLevels,
                                  I3DSURF_SHAREDRESOURCE,
                                  msg->d3dFormat,
                                  D3DDDIPOOL_VIDEOMEMORY,
                                  msg->hTexHandle);
    msg->hrUser = hr;

TESTFAIL:
    if( msg )
    {
        PATH_TRAP();
        if( FAILED(hr) )
        {
            USER_RET_ERR(msg,L"Failed CreateTexture()");
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    PATH_TRAP();
    return hr;
}


HRESULT CUser::XProcTest_OwnerGoesAway()
{
    HRESULT hr = S_OK;
    CI3DTexturePtr pTexture;

    Message* msg = NULL;

    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"Failed I3DXCreateValidTexture()");
        goto TESTFAIL;
    }

    msg->hTexHandle = pTexture.p->GetSharedHandle();

    //color fill this texture
    hr = I3DXFillTexture(m_pDevice,pTexture.p,I3DXFillSolid,(void*)&FILL_COLOR,0);
    if(FAILED(hr))
    {       
        USER_RET_ERR(msg,L"Failed I3DXFillTexture()");
        goto TESTFAIL;
    }

TESTFAIL:
    if(msg)
    {
        msg->hrUser = hr;
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    SetEvent(m_hUser); //signal to the using process that the resource was created
    WaitForSingleObject(m_hOwner, m_syncronizationTimeout); //wait for using process to open resource, then we go away
    return hr;
}


//
// CodePathStep 12:
//
//  Now let's perform the second part of the test. the COwner object created (setup)
// a test environment for us to use, use it here. When done, we set our status
// in the Message sturcture for COwner to read when we complete. 
//
HRESULT CUser::XProcTest_UserTriesToLock()
{
    HRESULT hr = S_OK;
    
    CI3DTexturePtr pTexture;
    D3DLOCKED_RECT d3dRect;
    CI3DSurface2Ptr Srf;
    D3DRECT rect; 

    // We need to lock access to our message info.
    Message* msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    hr = m_pDevice->CreateTexture(&pTexture.p,
                                  msg->uiWidth ,
                                  msg->uiHeight,
                                  msg->uiLevels,
                                  I3DSURF_SHAREDRESOURCE | I3DSURF_DYNAMIC,
                                  msg->d3dFormat,
                                  D3DDDIPOOL_VIDEOMEMORY,
                                  msg->hTexHandle);
    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"Non-test failure: CreateTexture()");
        goto TESTFAIL;  
    }

    hr = pTexture->GetSurface(0,&Srf.p);
    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"failed GetSurface()");
        goto TESTFAIL;  
    }

    rect.x1 = 0;
    rect.y1 = 0;
    rect.y2 = msg->uiHeight;
    rect.x2 = msg->uiWidth;

    hr = Srf->Lock(0, &rect, &d3dRect);   
    if(SUCCEEDED(hr))
    {
        Srf->Unlock();
    }    
TESTFAIL:
    if(msg)
    {
        msg->hrUser = hr;
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    return hr;
}


HRESULT CUser::XProcTest_ValidateTexture()
{
    HRESULT         hr = S_OK;
    CI3DTexturePtr  pTempTex, pTexture;
    CI3DSurface2Ptr Srf1, Srf2;
    RECT            rect;
    SColor          Ret;
    D3DRECT         TargetRect;

    // We need to lock access to our message info.
    Message* msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    //open the shared texture and render it to a render target
    hr = m_pDevice->CreateTexture(&pTempTex.p,
                                  msg->uiWidth ,
                                  msg->uiHeight,
                                  msg->uiLevels,
                                  I3DSURF_SHAREDRESOURCE | I3DSURF_DYNAMIC,
                                  msg->d3dFormat,
                                  D3DDDIPOOL_VIDEOMEMORY,
                                  msg->hTexHandle);

    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"failed CreateTexture()");
        goto TESTFAIL;
    }

    pTempTex->GetSurface(0, &Srf1.p);
    

    //create a lockable texture identical to the shared texture
    hr = m_pDevice->CreateTexture(  &pTexture.p,
                                    msg->uiWidth,
                                    msg->uiHeight,
                                    msg->uiLevels,
                                    I3DSURF_DYNAMIC,
                                    msg->d3dFormat,                                 
                                    D3DDDIPOOL_VIDEOMEMORY,
                                    NULL);

    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"failed CreateTexture()");
        goto TESTFAIL;
    }

    pTexture->GetSurface(0,&Srf2.p);

    rect.top    = rect.left = 0;
    rect.right  = msg->uiWidth;
    rect.bottom = msg->uiHeight;


    //bit the shared surface to a new surface
    hr = m_pDevice->Blt(Srf1.p,
                        Srf2.p,
                        rect,
                        rect,
                        0,
                        0x00000000);
    
    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"failed Blt()");
        goto TESTFAIL;
    }
    TargetRect.x1 = TargetRect.y1 = 0;
    TargetRect.x2 = TargetRect.y2 = 1;
    hr = UtilReadPixel(Srf2, &TargetRect, &Ret);
    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"failed UtilReadPixel()");
        goto TESTFAIL;
    }
    //check that the color is FILL_COLOR
    float deviation = UtilCalcDeviation(&Ret,FILL_COLOR.a, FILL_COLOR.r, FILL_COLOR.g, FILL_COLOR.b);
    if(deviation > 0.1f)
    {
        USER_RET_ERR(msg,L"failed: deviation greater than zero point one");
        hr = E_FAIL;
        goto TESTFAIL;
    }

TESTFAIL:
    if(msg)
    {
        msg->hrUser = hr;
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    return hr;
}

// This routine must be called under the lock.
HRESULT CUser::OpenSharedAllocation(Message * msg, CI3DTexture** ppTexture)
{
    HRESULT hr = S_OK;

    // We must be under our lock.
    if( !m_pSharedObject->UnderLock() )
    {
        PATH_TRAP();
        return TYPE_E_INVALIDSTATE;
    }
    
    hr = m_pDevice->CreateTexture(ppTexture,
                                  msg->uiWidth ,
                                  msg->uiHeight,
                                  msg->uiLevels,
                                  I3DSURF_SHAREDRESOURCE | I3DSURF_DYNAMIC,
                                  msg->d3dFormat,
                                  D3DDDIPOOL_VIDEOMEMORY,
                                  msg->hTexHandle);

    return hr;
}

// This routine must be called under the lock.
HRESULT CUser::UseSharedAllocation(Message* msg, CI3DTexture* pTexture)
{
    HRESULT hr = S_OK;
    CI3DTexturePtr  pTempTex;
    CI3DSurface2Ptr Srf1, Srf2;
    RECT            rect;
    SColor          Ret;
    D3DRECT         TargetRect;   

    // We must be under our lock.
    if( !m_pSharedObject->UnderLock() )
    {
        PATH_TRAP();
        return TYPE_E_INVALIDSTATE;
    }
    //
    //	Create temporary allocation identical to shared allocation
    //
    hr = m_pDevice->CreateTexture(  &pTempTex.p,
                                    msg->uiWidth,
                                    msg->uiHeight,
                                    msg->uiLevels,
                                    I3DSURF_DYNAMIC,
                                    msg->d3dFormat,                                 
                                    D3DDDIPOOL_VIDEOMEMORY,
                                    NULL);
    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"failed: CreateTexture()");
    }

    if(SUCCEEDED(hr))
    { 
        pTexture->GetSurface(0, &Srf1.p);
        pTempTex->GetSurface(0, &Srf2.p);

        rect.top    = rect.left = 0;
        rect.right  = msg->uiWidth;
        rect.bottom = msg->uiHeight;

        //
        //	Use shared allocation by blitting it to temporary allocation
        //
        hr = m_pDevice->Blt(Srf1.p,
                            Srf2.p,
                            rect,
                            rect,
                            0,
                            0x00000000);
        if(FAILED(hr))
        {
            USER_RET_ERR(msg,L"failed: Blt()");
        }

        if(SUCCEEDED(hr))
        {
            //
            //	Check allocation pattern
            //
            TargetRect.x1 = TargetRect.y1 = 0;
            TargetRect.x2 = TargetRect.y2 = 1;
            hr = UtilReadPixel(Srf2, &TargetRect, &Ret);
            if(FAILED(hr))
            {
                USER_RET_ERR(msg,L"failed: UtilReadPixel()");
            }
            if(SUCCEEDED(hr))
            {
                //check that the color is FILL_COLOR
                float deviation = UtilCalcDeviation(&Ret,FILL_COLOR.a, FILL_COLOR.r, FILL_COLOR.g, FILL_COLOR.b);
                if(deviation > 0.1f)
                {
                    USER_RET_ERR(msg,L"failed: deviation greater than zero point one");
                    hr = E_FAIL;
                }
            }
        }
    }

    return hr;
}

// This routine must not be called under the lock.
HRESULT CUser::OwnerDoWork()
{
    HRESULT hr = S_OK;

    // We must NOT be under our lock.
    if( m_pSharedObject->UnderLock() )
    {
        PATH_TRAP();
        return TYPE_E_INVALIDSTATE;
    }
    //
    //	Set event to signal owner to start doing its work
    //
    SetEvent(m_hUser); 

    //
    //	Wait for owner to complete its work and signal us back
    //
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hOwner, m_syncronizationTimeout))
    {
        DBG_MSG(L"failed: WaitForSingleObject()");
        hr = E_FAIL;
    }

    return hr;
}


HRESULT CUser::XProcTest_EvictSharedAllocation()
{
    HRESULT hr = S_OK;
    CI3DTexturePtr pTexture;

    Message* msg = NULL;

    m_syncronizationTimeout = 600000; // eviction might take a long time

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    hr = OpenSharedAllocation(msg, &pTexture.p);

    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"failed: OpenSharedAllocation()");
    }
    msg->hrUser = hr;

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    if(SUCCEEDED(hr))
    {
        hr = OwnerDoWork();

        if(SUCCEEDED(hr))
        {
            // We need to lock access to our message info.
            msg = m_pSharedObject->AcquireLock();
            if( !msg )
            {
                PATH_TRAP();
                hr = STG_E_LOCKVIOLATION;
                goto TESTFAIL;
            }

            // Now we're ready to perform our work.
            hr= UseSharedAllocation(msg, pTexture);

            msg->hrUser = hr;

            if(FAILED(hr))
            {
                USER_RET_ERR(msg,L"failed: UseSharedAllocation()");
            }
            // Now free our lock
            m_pSharedObject->ReleaseLock(&msg);
        }
    }

    SetEvent(m_hUser);

    m_syncronizationTimeout = 120000;
TESTFAIL:
    if(msg)
    {
        PATH_TRAP();
        msg->hrUser = hr;
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    return hr;
}


HRESULT CUser::CreatePrimary(CI3DNonMipped2DResource** pRes)
{
    HRESULT hr = S_OK;

    CI3DSwapChainPtr pSwapChain;
    CI3DSurface2Ptr pFrontBuffer;
    DeviceCreateOptions Opts;

    // We must NOT be under our lock.
    if( m_pSharedObject->UnderLock() )
    {
        hr= TYPE_E_INVALIDSTATE;
        goto TESTFAIL;
    }

    hr = m_pDevice->GetSwapChain(&pSwapChain.p);
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }

    hr =pSwapChain->GetFrontBuffer(&pFrontBuffer.p);
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }

    //
    //	Make sure we are in full screen mode
    //
    m_pDevice->GetCreationOptions(&Opts);
    assert(!Opts.bWindowed);

    UINT Width = 0, Height = 0;
    pFrontBuffer->GetSize(&Width, &Height);

    hr = m_pDevice->CreatePrimaryResource(pRes,
                                          Width,
                                          Height,
                                          pFrontBuffer->GetOutputIndex(),
                                          pFrontBuffer->GetRefreshRate(),
                                          I3DSURF_PRIMARYSURFACE,
                                          pFrontBuffer->GetFormat(),
                                          pFrontBuffer->GetMultiSampleType(),
                                          pFrontBuffer->GetMultiSampleQuality());
TESTFAIL:
    return hr;
}


HRESULT CUser::UsePrimary(CI3DNonMipped2DResource* pRes)
{
    HRESULT hr = S_OK;
    CI3DSurface2Ptr pSurf;

    // We must NOT be under our lock.
    if( m_pSharedObject->UnderLock() )
    {
        PATH_TRAP();
        hr =TYPE_E_INVALIDSTATE;
        goto TESTFAIL;
    }

    hr = pRes->GetSurface(&pSurf.p);
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }

    UINT nWidth;
    UINT nHeight;

    hr = pSurf->GetSize(&nWidth, &nHeight);
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }

    RECT rect;
    rect.left = 0;
    rect.top =0;
    rect.right=nWidth;
    rect.bottom=nHeight;

    //Use Color fill with GREEN color
    hr = m_pDevice->ColorFill(pSurf, rect, D3DCOLOR_ARGB(255, 0, 255, 0));
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }
TESTFAIL:
    return hr;
}


HRESULT CUser::LockPrimary(CI3DNonMipped2DResource* pRes)
{
    HRESULT hr = S_OK;
    CI3DSurface2Ptr pSurf;
    D3DLOCKED_RECT Rect;

    PATH_TRAP();
    hr = pRes->GetSurface(&pSurf.p);
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }

    hr = pSurf->Lock(0, NULL, &Rect);
TESTFAIL:
    return hr;
}


HRESULT CUser::UnlockPrimary(CI3DNonMipped2DResource* pRes)
{
    HRESULT hr = S_OK;
    CI3DSurface2Ptr pSurf;

    PATH_TRAP();
    hr = pRes->GetSurface(&pSurf.p);
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }

    hr = pSurf->Unlock();
TESTFAIL:
    return hr;
}


HRESULT CUser::XProcTest_CrossProcSwizzlingRangeContention()
{
    HRESULT hr = S_OK;
    const UINT maxPrimCount = 8;	// keep number of primaries down to avoid running out of video memory
    CI3DNonMipped2DResourcePtr pPrimaries[maxPrimCount];
    bool bFailTest = false;

    Message* msg = NULL;
    m_syncronizationTimeout = 600000; // Set up for this test can take a while (create/use/lock a lot of allocations)

    //
    //	Create primaries
    //
    for(UINT i = 0; i < maxPrimCount; i++)
    {
        hr = CreatePrimary(&pPrimaries[i].p);
        
        if(SUCCEEDED(hr))
        {
            hr = UsePrimary(pPrimaries[i].p);
        }

        if(FAILED(hr))
        {
            break;
        }
    }

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    msg->hrUser = hr;
    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"failed: Either CreatePrimary() or UsePrimary()");
    }

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);


    if(SUCCEEDED(hr))
    {
        //
        //	Synchronize with the owner
        //
        OwnerDoWork();

        while(WAIT_OBJECT_0 != WaitForSingleObject(m_hOwner, 0))
        {
            UINT cLockedPrimaries = 0;

            //
            //	Start locking primaries until locking failed
            //
            for(UINT i = 0; i < maxPrimCount; i++)
            {
                hr = LockPrimary(pPrimaries[i].p);				

                if(FAILED(hr))
                {
                    PATH_TRAP();
                    // We need to lock access to our message info.
                    msg = m_pSharedObject->AcquireLock();
                    if( !msg )
                    {
                        hr = STG_E_LOCKVIOLATION;
                        goto TESTFAIL;
                    }
                    // Now we're ready to perform our work.

                    msg->hrUser = hr;
                    USER_RET_ERR(msg,L"failed: LockPrimary()");

                    // Now free our lock
                    m_pSharedObject->ReleaseLock(&msg);

                    bFailTest = true;
                    break;
                }
                else
                {
                    cLockedPrimaries++;
                }
            }

            //
            // Unlock all primaries currently locked
            //
            while(cLockedPrimaries--)
            {
                UnlockPrimary(pPrimaries[cLockedPrimaries].p);
            }

            if(bFailTest)
            {
                break;
            }
        }
    }	
TESTFAIL:
    SetEvent(m_hUser);
    m_syncronizationTimeout = 120000; 
    return hr;
}


HRESULT CUser::XProcTest_MultiUserScenario()
{
    HRESULT hr = S_OK;
    CI3DTexturePtr  pTempTex, pTexture;
    CI3DSurface2Ptr Srf1, Srf2;
    RECT            rect;
    SColor          Ret;
    D3DRECT         TargetRect;    

    Message* msg = NULL;
    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    //open the shared texture and render it to a render target
    hr = m_pDevice->CreateTexture(&pTempTex.p,
                                  msg->uiWidth ,
                                  msg->uiHeight,
                                  msg->uiLevels,
                                  I3DSURF_SHAREDRESOURCE | I3DSURF_DYNAMIC,
                                  msg->d3dFormat,
                                  D3DDDIPOOL_VIDEOMEMORY,
                                  msg->hTexHandle);

    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"failed: CreateTexture()");
        goto TESTFAIL;
    }

    pTempTex->GetSurface(0, &Srf1.p);
    

    //create a lockable texture identical to the shared texture
    hr = m_pDevice->CreateTexture(  &pTexture.p,
                                    msg->uiWidth,
                                    msg->uiHeight,
                                    msg->uiLevels,
                                    I3DSURF_DYNAMIC,
                                    msg->d3dFormat,                                 
                                    D3DDDIPOOL_VIDEOMEMORY,
                                    NULL);

    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"failed: CreateTexture()");
        goto TESTFAIL;
    }    

    pTexture->GetSurface(0,&Srf2.p);

    rect.top    = rect.left = 0;
    rect.right  = msg->uiWidth;
    rect.bottom = msg->uiHeight;


    //bit the shared surface to a new surface
    hr = m_pDevice->Blt(Srf1.p,
                        Srf2.p,
                        rect,
                        rect,
                        0,
                        0x00000000);
    
    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"failed: Blt()");
        goto TESTFAIL;
    }
    TargetRect.x1 = TargetRect.y1 = 0;
    TargetRect.x2 = TargetRect.y2 = 1;
    hr = UtilReadPixel(Srf2, &TargetRect, &Ret);
    if(FAILED(hr))
    {
        USER_RET_ERR(msg,L"failed: UtilReadPixel()");
        goto TESTFAIL;
    }
    //check that the color is FILL_COLOR
    float deviation = UtilCalcDeviation(&Ret,FILL_COLOR.a, FILL_COLOR.r, FILL_COLOR.g, FILL_COLOR.b);
    if(deviation > 0.1f)
    {
        USER_RET_ERR(msg,L"failed: deviation greater than zero point one");
        hr = E_FAIL;
        goto TESTFAIL;
    }

TESTFAIL:
    if( msg )
    {
        if(FAILED(hr))
        {        
            msg->hrUser = hr;
        }
        // Reduce the outstanding count by one
        msg->lCount--;
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
    }
    SetEvent(m_hUser);
    return hr;
}


HRESULT CUser::XProcTest_RoundRobin() 
{
    HRESULT hr = S_OK;
    CI3DTexturePtr pTexture;
    CI3DSurface2Ptr pSrf;
    D3DLOCKED_RECT  lr;

    DBG_MSG(L"Enter");
    Message* msg = NULL;
    try
    {
        // We need to lock access to our message info.
        msg = m_pSharedObject->AcquireLock();
        if( !msg )
        {
            PATH_TRAP();
            hr = STG_E_LOCKVIOLATION;
            goto TESTFAIL;
        }
        // Now we're ready to perform our work.

        msg->lCount++;

#ifdef DBG
        {
            WCHAR wszDbg[MAX_PATH];
            StringCchPrintf(wszDbg,MAX_PATH,L"In RoundRobin: lCount=%d",msg->lCount);
            DBG_MSG(wszDbg);
        }
#endif
        if(msg->lCount == 1)
        {
            hr = I3DXCreateValidTexture(m_pDevice,
                                        0,
                                        I3DFORMAT_ARGB,
                                        I3DSURF_SHAREDRESOURCE  | I3DSURF_DYNAMIC,
                                        D3DDDIPOOL_VIDEOMEMORY,
                                        false,
                                        &pTexture.p);

            if(FAILED(hr))
            {
                USER_RET_ERR(msg,L"failed: I3DXCreateValidTexture()");
                goto TESTFAIL;
            }

            hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
            if(FAILED(hr))
            {
                USER_RET_ERR(msg,L"failed: GetBaseSurfaceDimensions()");
                goto TESTFAIL;
            }
            hr = pTexture->GetSurfaceCount(&msg->uiLevels );
            if(FAILED(hr))
            {
                USER_RET_ERR(msg,L"failed: GetSurfaceCount()");
                goto TESTFAIL;
            }
            msg->d3dFormat  = pTexture->GetFormat();   
            msg->hTexHandle = pTexture->GetSharedHandle();

            // Now free our lock
            m_pSharedObject->ReleaseLock(&msg);

            // Not under lock while creating the user process.
            hr = UtilCreateUserProcess();
            if(FAILED(hr))
            {
                goto TESTFAIL;
            }
            if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))//new process is created
            {
                hr = E_FAIL;
                goto TESTFAIL;
            }
            SetEvent(m_hOwner); //new process is waiting on our signal to start test    
            if(WAIT_OBJECT_0 != WaitForSingleObject(m_hRoundRobinUser, m_syncronizationTimeout)) //wait on the next process to grab the resource
            {
                hr = E_FAIL;
                goto TESTFAIL;
            }

            // We need to lock access to our message info.
            msg = m_pSharedObject->AcquireLock();
            if( !msg )
            {
                PATH_TRAP();
                hr = STG_E_LOCKVIOLATION;
                goto TESTFAIL;
            }
            // Now we're ready to perform our work.
        }
        else
        {
            //Open the resource
            hr = m_pDevice->CreateTexture(&pTexture.p,
                                        msg->uiWidth ,
                                        msg->uiHeight,
                                        msg->uiLevels,
                                        I3DSURF_SHAREDRESOURCE,
                                        msg->d3dFormat,
                                        D3DDDIPOOL_VIDEOMEMORY,
                                        msg->hTexHandle);
            SetEvent(m_hRoundRobinUser);
            if(FAILED(hr))
            {        
                WCHAR szwDbg[256];
                StringCchPrintf(szwDbg,256,L"Process %i failed to open shared texture\n", msg->lCount);
                USER_RET_ERR(msg,szwDbg);
                goto TESTFAIL;
            }    

            //lCount is the number of processes, including this one, that have been run
            //lCount2 is the number of processes that this test needs to run (as specified in crossproc.ini Num_RoundRobins)
            if(msg->lCount < msg->lCount2 )
            {
                // Now free our lock
                m_pSharedObject->ReleaseLock(&msg);

                hr = UtilCreateUserProcess();
                if(FAILED(hr))
                {
                    goto TESTFAIL;
                }
                if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout)) //wait for next process to be created
                {
                    hr = E_FAIL;
                    goto TESTFAIL;
                }

                SetEvent(m_hOwner); //next process is waiting on this to start                 
                if(WAIT_OBJECT_0 != WaitForSingleObject(m_hRoundRobinUser, m_syncronizationTimeout)) //wait for next process to grab the resource
                {
                    hr = E_FAIL;
                    goto TESTFAIL;
                }

                // We need to lock access to our message info.
                msg = m_pSharedObject->AcquireLock();
                if( !msg )
                {
                    PATH_TRAP();
                    hr = STG_E_LOCKVIOLATION;
                    goto TESTFAIL;
                }
                // Now we're ready to perform our work.
            }
            else
            {
                SetEvent(m_hRoundRobinOwner); //this will signal to the main process that this test has completed
            }
        }
    
TESTFAIL:
        if( msg )
        {
            if(FAILED(hr))
            {
                msg->hrUser = hr;        
                SetEvent(m_hRoundRobinOwner);
            }

            // Now free our lock
            m_pSharedObject->ReleaseLock(&msg);
        }
        else
        {
            if(FAILED(hr))
            {
                PATH_TRAP();
                SetEvent(m_hRoundRobinOwner);
            }
        }
    }
    catch(...)
    {
        cout << "Problem in RoundRobin, returning E_ACCESSDENIED\n";
        PATH_TRAP();

        // We need to lock access to our message info.
        msg = m_pSharedObject->AcquireLock();
        if( msg )
        {
            PATH_TRAP();
            msg->hrUser = E_ACCESSDENIED;        
            // Now free our lock
            m_pSharedObject->ReleaseLock(&msg);
        }
    }
    DBG_MSG(L"Leave");
    return hr;
}


HRESULT CUser::XProcTest_OpenClose_RaceCondition() 
{
    HRESULT hr = S_OK;
    CI3DTexturePtr pTexture;
    CI3DSurface2Ptr pSrf;
    D3DLOCKED_RECT  lr;
    
    Message* msg = NULL;
    PATH_TRAP();
    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    if(msg->lCount == TRUE)
    {
        msg->lCount = FALSE;   
        hr = I3DXCreateValidTexture(m_pDevice,
                                    0,
                                    I3DFORMAT_ARGB,
                                    I3DSURF_SHAREDRESOURCE  | I3DSURF_DYNAMIC,
                                    D3DDDIPOOL_VIDEOMEMORY,
                                    false,
                                    &pTexture.p);
        
        if(FAILED(hr))
        {
            USER_RET_ERR(msg,L"Failed: I3DXCreateValidTexture()");
            goto TESTFAIL;
        }
        hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
        if(FAILED(hr))
        {
            USER_RET_ERR(msg,L"Failed: GetBaseSurfaceDimensions()");
            goto TESTFAIL;
        }
        hr = pTexture->GetSurfaceCount(&msg->uiLevels );
        if(FAILED(hr))
        {
            USER_RET_ERR(msg,L"Failed: GetSurfaceCount()");
            goto TESTFAIL;
        }
        msg->d3dFormat  = pTexture->GetFormat();   
        msg->hTexHandle = pTexture->GetSharedHandle();

        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);

        hr = UtilCreateUserProcess();
        if(FAILED(hr))
        {
            goto TESTFAIL;
        }
        if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout)) //new process is created
        {
            goto TESTFAIL;
        }
        
        SetEvent(m_hOwner); //new process is waiting on our signal to start test       
        //instead of waiting for the new process to finish, we will end here. 
        //this is the point of contention for the race condition
    }
    else
    {
        //Open the resource
        m_pDevice->CreateTexture(&pTexture.p,
                                 msg->uiWidth ,
                                 msg->uiHeight,
                                 msg->uiLevels,
                                 I3DSURF_SHAREDRESOURCE,
                                 msg->d3dFormat,
                                 D3DDDIPOOL_VIDEOMEMORY,
                                 msg->hTexHandle);        

        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
        
        //we don't care if we pass or fail the above call, we just don't want to break
    }

TESTFAIL:
    SetEvent(m_hRoundRobinOwner); //this will signal to the main process that this test has completed
    if(msg)
    {
        PATH_TRAP();
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    PATH_TRAP();
    return hr;
}


//
// CodePathStep 11:
//
//  We're still nested within CreateWindow and the CUser object is waiting for CUser
// to get setup. Thus, here, we are ready to recieve message info from COwner and
// execute it's wishes. Thus the first step is to wake COwner up so it can do the
// first steps of the test preperation. The key thing to note here is that this completes
// the CreateUserProcess() call for COwner. That routine was waiting for us to get
// here. So, in the example that we're tracking, XProcTest_UserTriesToLock just returned
// from CreateUserProcess() and we're now going to wait for XProcTest_UserTriesToLock
// to signal m_hOwner - which it does after setting up the test and placing key
// info in the Message structure (shared memory).
//
// After XProcTest_UserTriesToLock finishes, we wake up and read the Message sturcture.
// we find the dwTestId eXProcTest_UserTriesToLock. 
// 
// Before leaving, we signal m_hUser to wake the COwner object back up. The unwind
// now happens we leave Run(), Drop out of Go() and leave Winmain() thus terminating
// our instance of CrossProc().
//
HRESULT CUser::Run()
{    
    HRESULT hr = S_OK;

    DBG_MSG(L"Enter");
    //this lets the calling proc know that 
    //we have started and ready to go
    SetEvent(m_hUser); 

    //Give the owner process an oppurtunity to do some initializing
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hOwner, m_syncronizationTimeout))
    {
        DBG_MSG(L"Failed WaitForSingleObject()");
        DBG_MSG(L"Leave");
        return E_FAIL;
    }

    // Now we need to fetch our dwTestId.
    Message* pMsg=NULL;
    pMsg = m_pSharedObject->AcquireLock();
    if( !pMsg )
    {
        DBG_MSG(L"Failed AcquireLock()");
        PATH_TRAP();
        SetEvent(m_hUser); 
        DBG_MSG(L"Leave");
        return E_FAIL;
    }
#ifdef _DEBUG   
    // We want to print out our message in debug
    UtilPrintMessage(pMsg);
#endif
    DWORD dwTestId = pMsg->dwTestId;
    m_pSharedObject->ReleaseLock(&pMsg);

    switch(dwTestId)
    {
        case eXProcTest_VertexBuffer_Open:
            XProcTest_VertexBuffer_Open();
            break;    
        case eXProcTest_IndexBuffer_Open:
            XProcTest_IndexBuffer_Open();
            break;
        case eXProcTest_Basic_CreateOpen:
        case eXProcTest_InvalidHandle_IncrementOne:
        case eXProcTest_InvalidHandle_DecrementOne:
        case eXProcTest_InvalidHeightDimension:
        case eXProcTest_InvalidWidthDimension:
        case eXProcTest_InvalidFormat:
        case eXProcTest_InvalidLevel:
        case eXProcTest_RoundRobinExt:
        case eXProcTest_CrossSessionNotShare:
        case eXProcTest_CrossAdapterNotShare:
            XProcTest_Basic_Open();
            break;
        case eXProcTest_UserTriesToLock:            
            XProcTest_UserTriesToLock();
            break;
        case eXProcTest_ValidateTexture:
            XProcTest_ValidateTexture();
            break;
        case eXProcTest_OwnerGoesAway:
            XProcTest_OwnerGoesAway();
            break;
        case eXProcTest_EvictSharedAllocation:
            XProcTest_EvictSharedAllocation();
            break;
        case eXProcTest_MultiUserScenario:
            XProcTest_MultiUserScenario();
            break;
        case eXProcTest_RoundRobin:
            XProcTest_RoundRobin(); 
            break;
        case eXProcTest_OpenClose_RaceCondition:
            XProcTest_OpenClose_RaceCondition();
            break;
        case eXProcTest_CrossProcSwizzlingRangeContention:
            XProcTest_CrossProcSwizzlingRangeContention();
            break;
        default:
            break;
    }
    
    //let the owning process know that we are finished with the test and going to cleanup
    SetEvent(m_hUser);
    DBG_MSG(L"Leave");
    return hr;
}



