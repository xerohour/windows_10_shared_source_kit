#include "stdafx.h"
#include "FusHelper.h"
#include <vector>

extern DWORD g_syncronizationTimeout;

//
// CodePathStep 3:
//
//  Create our object.
//
COwner::COwner(HWND hwnd) : 
    m_syncronizationTimeout(120000),
    m_pwszUserName(L"XPr0cCr0"),
    m_pwszPassword(L"pass123word456!!<>")
{
    m_hWnd      = hwnd;
    m_hUser     = NULL;
    m_hOwner    = NULL;
    m_pSharedObject = NULL;
    m_hRoundRobinOwner=NULL;
    m_Log = NULL;
    ZeroMemory(&m_TestCaseParams, sizeof(TESTCASE));
}

COwner::~COwner()
{
    delete m_Log;
}

HRESULT COwner::XProcTest_FUSTest(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    Message * msg = NULL;
    CI3DTexturePtr  pTexture;
    CI3DTexturePtr  pTexture2;
    CI3DSurface2Ptr pSurface;

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

    //synchronization notes:
        //it is assumed that this will be run in the FUS framework
        //this means that we should not be hitting race conditions on the following
        //conditionals. This is why they are not protected.

    if(msg->dwTestId == eXProcTest_FUSTest)
    {
        //this test has already been started
        //we should now assume that the resource has been created, so we will open it
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
            LOG_AND_DBG_MSG(m_Log,L"Failed to open shared texture in FUSTest, before a PNPStop (second process)");
            goto TESTFAIL;
        }
        
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);

        //we will sleep for n seconds to allow the user to throw PNPSTOP.
        Sleep(m_TestCaseParams.nNum_FUSSleepSeconds * 1000);

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

        //we will now try to open the texture again
        hr = m_pDevice->CreateTexture(&pTexture2.p,
                                      msg->uiWidth ,
                                      msg->uiHeight,
                                      msg->uiLevels,
                                      I3DSURF_SHAREDRESOURCE,
                                      msg->d3dFormat,
                                      D3DDDIPOOL_VIDEOMEMORY,
                                      msg->hTexHandle);    

        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Failed to open shared texture in FUSTest, after a PNPStop (second process)");
            goto TESTFAIL;
        }

        //with the texture now open, we will try to access the surface
        hr = pTexture2->GetSurface(0, &pSurface.p);
        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Failed to get texture surface in FUSTest, after a PNPStop (second process)");
        }
        msg->hrUser = hr;
        hr = S_OK;
        
        msg->lCount = 1; //set this to nonzero value so that the "owning" process knows when 
                         //it is safe to quit.
    }
    else
    {
        //this is the first process to run this test
        msg->dwTestId = eXProcTest_FUSTest;
        msg->lCount = 0; //set this to zero, we will poll it to see when the other session has quit.
        hr = I3DXCreateValidTexture(m_pDevice,
                                    0,
                                    I3DFORMAT_ARGB,
                                    I3DSURF_SHAREDRESOURCE,
                                    D3DDDIPOOL_VIDEOMEMORY,
                                    false,
                                    &pTexture.p);
        
        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Failed to create texture in FUSTest (first process)");
            goto TESTFAIL;
        }        
        hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetBaseSurfaceDimensions()");
            goto TESTFAIL;
        }
        hr = pTexture->GetSurfaceCount(&msg->uiLevels );
        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
            goto TESTFAIL;
        }
        msg->d3dFormat = pTexture->GetFormat(); 
        msg->hTexHandle = pTexture->GetSharedHandle();

        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);

        // We now need to check the lCount to see when it gets to zero. But, because we don't 
        // want lock contention, we have to sleep(), take the lock, check the value, release the lock
        // and then loop.
        PATH_TRAP();

        while(true)
        {
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

            if( msg->lCount == 0 )
            {
                PATH_TRAP();
                // note that we break under the lock and that this is the only way out
                // of this loop.
                break;
            }

            // Now free our lock
            m_pSharedObject->ReleaseLock(&msg);

            // now yield to the other process
            Sleep(1);
        }
    }

TESTFAIL:
    if(msg)
    {
        PATH_TRAP();
        if(FAILED(hr))
        {
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_FUSTest");
            m_Log->EndTest(testHandle);
        }
        else
        {
            PATH_TRAP();
            ThrowResult(msg, testHandle);
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_FUSTest");
        m_Log->EndTest(testHandle);
    }
    PATH_TRAP();
   
    return hr;

}

HRESULT COwner::XProcTest_IndexBuffer_Open(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    CI3DIndexBufferPtr pIB;
    //check caps
    D3DCAPS9 caps;
    Message* msg = NULL;

    if(FAILED(I3DGetD3D9Caps(m_pDevice, &caps)))
    {
        hr = E_FAIL;
        goto TESTFAIL;
    }    
    if(!I3DHWIBSupported(&caps))
    {
        hr = E_FAIL; //skip test
        goto TESTFAIL;
    }
    
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

    msg->dwTestId = eXProcTest_IndexBuffer_Open;
    msg->hrUser = S_OK;
    msg->hTexHandle = NULL;
    
    m_pDevice->CreateIndexBuffer(&pIB.p, 
                                 24, 
                                 I3DSURF_SHAREDRESOURCE, 
                                 D3DDDIFMT_INDEX16, 
                                 D3DDDIPOOL_VIDEOMEMORY, 
                                 NULL);
    if(FAILED(hr)) //call to Create failed (skip test: driver may not support this)
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create shared IB, driver may not support this");
        goto TESTFAIL;
    }        
    msg->hTexHandle = pIB->GetSharedHandle();
    if(msg->hTexHandle == NULL)
    {
        LOG_AND_DBG_MSG(m_Log,L"Driver does not support shared IB's but is not failing the call to create.");
        msg->hrUser = E_FAIL;
        hr = S_OK;
        goto TESTFAIL;
    }
   
    msg->lCount = 24;
    
    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    hr = CreateUserProcess();    
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }

    SetEvent(m_hOwner);
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        hr = E_FAIL;
        goto TESTFAIL;
    }

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

TESTFAIL:
    if( msg )
    {
        PATH_TRAP();
        if(FAILED(hr))
        {
            m_Log->Skip(testHandle, L"XProcTest_IndexBuffer_Open");
            PATH_TRAP();
            m_Log->EndTest(testHandle);
        }
        else
        {
            PATH_TRAP();
            ThrowResult(msg, testHandle);
        }

        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_IndexBuffer_Open");
        m_Log->EndTest(testHandle);
    }

    PATH_TRAP();
    return hr;
}


HRESULT COwner::XProcTest_VertexBuffer_Open(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    CI3DVertexBufferPtr pVB;
    Message * msg = NULL;

    D3DCAPS9 caps;
    if(FAILED(I3DGetD3D9Caps(m_pDevice, &caps)))
    {
        hr = E_FAIL;
        goto TESTFAIL;
    }
    if(!I3DHWVBSupported(&caps))
    {
        hr = E_FAIL; //skip test
        goto TESTFAIL;
    }
        
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
    
    msg->dwTestId = eXProcTest_VertexBuffer_Open;
    msg->hrUser = S_OK;
    msg->hTexHandle = NULL;

    m_pDevice->CreateVertexBuffer(&pVB.p, 
                                  VERTEXCOUNT * sizeof(MyVertex), 
                                  I3DSURF_SHAREDRESOURCE,
                                  MYFVF, 
                                  D3DDDIPOOL_VIDEOMEMORY, 
                                  NULL);
    if(FAILED(hr)) //call to Create failed (skip test: driver may not support this)
    {   
        LOG_AND_DBG_MSG(m_Log,L"Failed to create shared VB, driver may not support this");
        goto TESTFAIL;
    }        
    msg->hTexHandle = pVB->GetSharedHandle();
    if(msg->hTexHandle == NULL )
    {
        LOG_AND_DBG_MSG(m_Log,L"Driver does not support shared VB's but is not failing the call to create.");
        msg->hrUser = E_FAIL;
        hr = S_OK;
        goto TESTFAIL;
    }
   
    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    
    hr = CreateUserProcess();    
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }

    SetEvent(m_hOwner);
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        hr = E_FAIL;
        goto TESTFAIL;
    }
    
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

TESTFAIL:
    if( msg )
    {
        PATH_TRAP();
        if(FAILED(hr))
        {
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_VertexBuffer_Open");
            m_Log->EndTest(testHandle);
        }
        else
        {
            PATH_TRAP();
            ThrowResult(msg, testHandle);
        }

        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_VertexBuffer_Open");
        m_Log->EndTest(testHandle);
    }
    PATH_TRAP();
   
    return hr;
    
}

HRESULT  COwner::XProcTest_Basic_CreateOpen(HANDLE testHandle)
{   

    HRESULT hr = S_OK;
    Message * msg = NULL;
    CI3DTexturePtr pTexture;

    LOG_AND_DBG_MSG(m_Log,L"Enter");

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.
    
    msg->dwTestId = eXProcTest_Basic_CreateOpen;
    msg->hrUser = S_OK;

    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create texture");
        goto TESTFAIL;
    }
    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetBaseSurfaceDimensions()");
        goto TESTFAIL;
    }
    hr = pTexture->GetSurfaceCount(&msg->uiLevels );
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
        goto TESTFAIL;
    }
    msg->d3dFormat = pTexture->GetFormat();   
    msg->hTexHandle = pTexture->GetSharedHandle();

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    hr = CreateUserProcess();    
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed CreateUserProcess()");
        goto TESTFAIL;
    }

    SetEvent(m_hOwner);
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed WaitForSingleObject()");
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

TESTFAIL:
    if( msg )
    {
        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Skipping");
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_Basic_CreateOpen");
            m_Log->EndTest(testHandle);
        }
        else
        {
            ThrowResult(msg, testHandle);
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        m_Log->Skip(testHandle, L"XProcTest_Basic_CreateOpen");
        m_Log->EndTest(testHandle);
    }
    DBG_MSG( L"Leave");
    return hr;
}

HRESULT COwner::XProcTest_InvalidHandle_DecrementOne(HANDLE testHandle)
{

    HRESULT hr = S_OK;
    Message * msg = NULL;
    CI3DTexturePtr pTexture;

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

    msg->dwTestId = eXProcTest_InvalidHandle_DecrementOne;

    //create shared resource
    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create texture");
        goto TESTFAIL;
    }

    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetBaseSurfaceDimensions()");
        goto TESTFAIL;
    }
    hr = pTexture->GetSurfaceCount(&msg->uiLevels );
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
        goto TESTFAIL;
    }
    msg->d3dFormat  = pTexture->GetFormat();   
    msg->hTexHandle = pTexture->GetSharedHandle() - 1;

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    hr = CreateUserProcess();
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed CreateUserProcess()");
        goto TESTFAIL;
    }
    SetEvent(m_hOwner);
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed WaitForSingleObject()");
        hr = E_FAIL;
        goto TESTFAIL;
    }
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

TESTFAIL:
    if(msg)
    {
        PATH_TRAP();
        if(FAILED(hr))
        {
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_InvalidHandle_DecrementOne");
            m_Log->EndTest(testHandle);
        }
        else
        {
            PATH_TRAP();
            ThrowResult(msg, testHandle);
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_InvalidHandle_DecrementOne");
        m_Log->EndTest(testHandle);
    }
    PATH_TRAP();

    return hr;
    
}

HRESULT COwner::XProcTest_InvalidHandle_IncrementOne(HANDLE testHandle)
{

    HRESULT hr = S_OK;
    Message * msg = NULL;
    CI3DTexturePtr pTexture;

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

    msg->dwTestId = eXProcTest_InvalidHandle_IncrementOne;
    
    //create shared resource
    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create texture");
    }

    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetBaseSurfaceDimensions()");
        goto TESTFAIL;
    }
    hr = pTexture->GetSurfaceCount(&msg->uiLevels );
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
        goto TESTFAIL;
    }
    msg->d3dFormat  = pTexture->GetFormat();   
    msg->hTexHandle = pTexture->GetSharedHandle() + 1;
    
    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    hr = CreateUserProcess();
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }    
    SetEvent(m_hOwner);
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        hr = E_FAIL;
        goto TESTFAIL;
    }
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

TESTFAIL:
    if( msg )
    {
        PATH_TRAP();
        if(FAILED(hr))
        {
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_InvalidHandle_IncrementOne");
            m_Log->EndTest(testHandle);
        }
        else
        {
            PATH_TRAP();
            ThrowResult(msg, testHandle);
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_InvalidHandle_IncrementOne");
        m_Log->EndTest(testHandle);
    }

    return hr;  

}

HRESULT COwner::XProcTest_InvalidHeightDimension(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    Message * msg = NULL;
    CI3DTexturePtr pTexture;

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

    msg->dwTestId = eXProcTest_InvalidHeightDimension;
    
    //create shared resource
    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create texture");
        goto TESTFAIL;
    }

    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
        goto TESTFAIL;
    }
    hr = pTexture->GetSurfaceCount(&msg->uiLevels );
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
        goto TESTFAIL;
    }
    //change the height dimension
    msg->uiHeight   = msg->uiHeight / 2;
    msg->d3dFormat  = pTexture->GetFormat();   
    msg->hTexHandle = pTexture->GetSharedHandle();

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    hr = CreateUserProcess();
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }    
    SetEvent(m_hOwner);
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        hr = E_FAIL;
        goto TESTFAIL;
    }
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
    
TESTFAIL:
    if(msg)
    {
        PATH_TRAP();
        if(FAILED(hr))
        {
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_InvalidHeightDimension");
            m_Log->EndTest(testHandle);
        }
        else
        {
            PATH_TRAP();
            ThrowResult(msg, testHandle);
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_InvalidHeightDimension");
        m_Log->EndTest(testHandle);
    }
    PATH_TRAP();

    return hr;
}

HRESULT COwner::XProcTest_InvalidWidthDimension(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    Message * msg = NULL;
    CI3DTexturePtr pTexture;

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

    msg->dwTestId = eXProcTest_InvalidWidthDimension;
    
    //create shared resource
    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create texture");
        goto TESTFAIL;
    }

    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetBaseSurfaceDimensions()");
        goto TESTFAIL;
    }
    hr = pTexture->GetSurfaceCount(&msg->uiLevels );
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
        goto TESTFAIL;
    }
    //change the height dimension
    msg->uiWidth    = msg->uiWidth / 2;
    msg->d3dFormat  = pTexture->GetFormat();   
    msg->hTexHandle = pTexture->GetSharedHandle();

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    hr = CreateUserProcess();
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }     
    SetEvent(m_hOwner);
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        hr = E_FAIL;
        goto TESTFAIL;
    }
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
    
TESTFAIL:
    if(msg)
    {
        PATH_TRAP();
        if(FAILED(hr))
        {
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_InvalidWidthDimension");
            m_Log->EndTest(testHandle);
        }
        else
        {
            PATH_TRAP();
            ThrowResult(msg, testHandle);
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_InvalidWidthDimension");
        m_Log->EndTest(testHandle);
    }
    PATH_TRAP();

    return hr;
}

HRESULT COwner::XProcTest_InvalidFormat(HANDLE testHandle)
{

    HRESULT hr = S_OK;
    Message * msg = NULL;
    CI3DTexturePtr pTexture;

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

    msg->dwTestId = eXProcTest_InvalidFormat;
    
    hr = CreateUserProcess();
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }     
    //create shared resource
    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create texture");
        goto TESTFAIL;
    }

    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetBaseSurfaceDimensions()");
        goto TESTFAIL;
    }
    hr = pTexture->GetSurfaceCount(&msg->uiLevels );
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
        goto TESTFAIL;
    }
    msg->d3dFormat  = pTexture->GetFormat();   
    msg->hTexHandle = pTexture->GetSharedHandle();

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    SetEvent(m_hOwner);
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        hr = E_FAIL;
        goto TESTFAIL;
    }
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
    
TESTFAIL:
    if(msg)
    {
        PATH_TRAP();
        if(FAILED(hr))
        {
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_InvalidFormat");
            m_Log->EndTest(testHandle);
        }
        else
        {
            PATH_TRAP();
            ThrowResult(msg, testHandle);
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_InvalidFormat");
        m_Log->EndTest(testHandle);
    }
    PATH_TRAP();

    return hr;

}


HRESULT COwner::XProcTest_InvalidLevel(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    Message * msg = NULL;
    CI3DTexturePtr pTexture;

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

    msg->dwTestId = eXProcTest_InvalidLevel;

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    hr = CreateUserProcess();
    if(FAILED(hr))
    {
        goto TESTFAIL;
    }     
    //create shared resource
    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create texture");
        goto TESTFAIL;
    }

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetBaseSurfaceDimensions()");
        goto TESTFAIL;
    }        
    msg->d3dFormat  = pTexture->GetFormat();   
    msg->hTexHandle = pTexture->GetSharedHandle();
    hr = pTexture->GetSurfaceCount(&msg->uiLevels );
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
        goto TESTFAIL;
    }
    msg->uiLevels -= 1;

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    SetEvent(m_hOwner);
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        hr = E_FAIL;
        goto TESTFAIL;
    }

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

    
TESTFAIL:
    if(msg)
    {
        PATH_TRAP();
        if(FAILED(hr))
        {
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_InvalidLevel");
            m_Log->EndTest(testHandle);
        }
        else
        {
            PATH_TRAP();
            ThrowResult(msg, testHandle);
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_InvalidLevel");
        m_Log->EndTest(testHandle);
    }
    PATH_TRAP();

    return hr;

}


//The owner will be the user for this test and the user will be the owner
//the Owner will create a shared resource and the user will open it
//then the owning process will die and we will see if they user can still 
//access the resource
HRESULT COwner::XProcTest_OwnerGoesAway(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    CI3DTexturePtr  pTexture, pTempTexture;
    CI3DSurface2Ptr pSrf1, pSrf2;   
    D3DRECT         d3dr;
    D3DLOCKED_RECT  lr;
    RECT            rect;
    SColor          color;
    //open the message, it should be filled by the owner
    Message * msg = NULL;

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    msg->dwTestId = eXProcTest_OwnerGoesAway;

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    hr = CreateUserProcess(); //start the Owner
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed CreateUserProcess()");
        goto TESTFAIL;
    }     
    SetEvent(m_hOwner);  //signal the owner to go ahead
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout)) //wait on the owner to signal that the resource is created
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed WaitForSingleObject()");
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

    if(FAILED(msg->hrUser))
    {       
        LOG_AND_DBG_MSG(m_Log,L"Failed hrUser");
        goto TESTFAIL;
    }

    //open the resource 
    hr = m_pDevice->CreateTexture(  &pTexture.p,
                                    msg->uiWidth,
                                    msg->uiHeight,
                                    msg->uiLevels,
                                    I3DSURF_SHAREDRESOURCE,
                                    msg->d3dFormat,                                 
                                    D3DDDIPOOL_VIDEOMEMORY,
                                    msg->hTexHandle);

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    SetEvent(m_hOwner);//signal that the resource is opened
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))//wait for the owner to go away
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed WaitForSingleObject()");
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

    if(FAILED(hr))
    {   
        LOG_AND_DBG_MSG(m_Log,L"Failed hr");
        goto TESTFAIL;
    }

    //blit this texture into a lockable surface and check the color
    pTexture->GetSurface(0,&pSrf1.p);
    //analyze the surface
    hr = pSrf1->Lock(0,0,&lr); //should not be able to lock here
    if(SUCCEEDED(hr))
    {        
        LOG_AND_DBG_MSG(m_Log,L"Succeeded Lock()");
        pSrf1->Unlock();        
        goto TESTFAIL;
    }

    //copy the shared surface to a new surface and examine its contents

    //create new surface
    hr = m_pDevice->CreateTexture(  &pTempTexture.p,
                                    msg->uiWidth,
                                    msg->uiHeight,
                                    msg->uiLevels,
                                    I3DSURF_DYNAMIC,
                                    msg->d3dFormat,                                 
                                    D3DDDIPOOL_VIDEOMEMORY,
                                    NULL);

    if(FAILED(hr))
    {     
        LOG_AND_DBG_MSG(m_Log,L"failed CreateTexture()");
        goto TESTFAIL;
    }
    pTempTexture->GetSurface(0, &pSrf2.p);
    
    rect.top    = rect.left = 0;
    rect.right  = msg->uiWidth;
    rect.bottom = msg->uiHeight;


    //blt the shared surface to a new surface
    hr = m_pDevice->Blt(pSrf1.p,
                        pSrf2.p,
                        rect,
                        rect,
                        0,
                        0x00000000);
    if(FAILED(hr))
    {   
        LOG_AND_DBG_MSG(m_Log,L"failed Blt()");
        goto TESTFAIL;
    }

    d3dr.x1 = d3dr.y1 = 0;
    d3dr.x2 = d3dr.y2 = 1;  
    hr = UtilReadPixel(pSrf2, &d3dr, &color);
    if(FAILED(hr))
    {   
        LOG_AND_DBG_MSG(m_Log,L"failed UtilReadPixel()");
        goto TESTFAIL;
    }
    float deviation = UtilCalcDeviation(&color, FILL_COLOR.a, FILL_COLOR.r, FILL_COLOR.g,FILL_COLOR.b);
    if(deviation > 0.1f)
    {       
        LOG_AND_DBG_MSG(m_Log,L"failed UtilCalcDeviation()");
        msg->hrUser = E_FAIL;
        goto TESTFAIL;
    }

TESTFAIL:   
    if(msg)
    {
        if(FAILED(hr))
        {       
            LOG_AND_DBG_MSG(m_Log,L"Skipping");
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_OwnerGoesAway");
            m_Log->EndTest(testHandle);
        }
        else
        {
            ThrowResult(msg, testHandle);
        }   
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        m_Log->Skip(testHandle, L"XProcTest_OwnerGoesAway");
        m_Log->EndTest(testHandle);
    }
    return hr;
}
//
// CodePathStep 6:
//
//  At this point, we have our COwner object setup. Now we want to setup the CUser
// version. That's done in CreateUserProcess().
//
// When we return from CreateUserProcess() the CUser object is setup and the m_hUser
// event that was created in the COwner Initialize() routine has been signaled. 
// That informs us that the CUser object has read the msg->dwTestId and is waiting
// for us to signal the m_hOwner event. This gives us a little window to setup
// the test scenario. When we signal the m_hOwner event, the CUser object will wake
// back up and completes the test. After it does, in this case, it will run to completion
// and return a status code to us.
//
// When the CUser object completes, it signals the m_hUser event that wakes us back
// up. At this point, this test is done and we need to report the status. Thus, we
// look into the Message sturcture to see what to report and then unwind. The unwind
// will take us back to Run(), back to Go() and back through Winmain().
//
HRESULT COwner::XProcTest_UserTriesToLock(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    Message * msg = NULL;
    CI3DTexturePtr pTexture;

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    msg->dwTestId = eXProcTest_UserTriesToLock;

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    // When this is done, the CUser object will be waiting on the m_hOwner event
    // giving us the oppertunity to setup the test. Thus, we do that work just
    // below.
    hr = CreateUserProcess();
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"failed CreateUserProcess()");
        goto TESTFAIL;
    }     
    //create shared resource
    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE | I3DSURF_DYNAMIC,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create texture");
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

    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetBaseSurfaceDimensions()");
        goto TESTFAIL;
    }

    msg->d3dFormat  = pTexture->GetFormat();   
    msg->hTexHandle = pTexture->GetSharedHandle();
    pTexture->GetSurfaceCount(&msg->uiLevels);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
        goto TESTFAIL;
    }
    
    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    // We're done setting up. Notice that we've filled in the Message structure with
    // the information that the test needs. Now, we signal it to wake up and wait
    // for it to complete.
    SetEvent(m_hOwner);
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed WaitForSingleObject()");
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
    
TESTFAIL:
    if(msg)
    {
        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Skipping");
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_UserTriesToLock");
            m_Log->EndTest(testHandle);
        }
        else
        {
            ThrowResult(msg, testHandle);
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_UserTriesToLock");
        m_Log->EndTest(testHandle);
    }
    return hr;
}

//This test is only run on the owner
HRESULT COwner::XProcTest_OwnerTriesToLock(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    CI3DSurface2Ptr Srf;
    D3DLOCKED_RECT d3dRect;
    D3DRECT rect;
    CI3DTexturePtr pTexture;

    Message * msg = NULL; 

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    msg->dwTestId = eXProcTest_OwnerTriesToLock;    
    
    //create shared resource
    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE | I3DSURF_DYNAMIC,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Non-test failure: I3DXCreateValidTexture()");
        goto TESTFAIL;
    }
    
    hr = pTexture->GetSurface(0,&Srf.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Non-test failure: GetSurface()");
        goto TESTFAIL;
    }

    UINT x,y;
    hr = pTexture->GetBaseSurfaceDimensions(&x , &y);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetBaseSurfaceDimensions()");
        goto TESTFAIL;
    }
    
    rect.y1 = 0; rect.x1 = 0; rect.x2 = x; rect.y2 = y;

    msg->hrUser = Srf->Lock(0, &rect, &d3dRect);    
    if(SUCCEEDED(msg->hrUser))
    {
        Srf->Unlock();
    }
    else
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed Lock()");
        goto TESTFAIL;
    }

TESTFAIL:
    if(msg)
    {
        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Skipping");
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_OwnerTriesToLock");
            m_Log->EndTest(testHandle);
        }
        else
        {
            ThrowResult(msg, testHandle);
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_OwnerTriesToLock");
        m_Log->EndTest(testHandle);
    }
    return hr;
}


HRESULT COwner::XProcTest_ValidateTexture(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    CI3DSurface2Ptr Srf;
    D3DLOCKED_RECT d3dRect;
    D3DRECT rect;
    CI3DTexturePtr pTexture;
    
    Message * msg = NULL; 

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    msg->dwTestId = eXProcTest_ValidateTexture;    
    
    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    hr = CreateUserProcess();
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed CreateUserProcess()");
        goto TESTFAIL;
    } 

    //create shared resource
    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE | I3DSURF_DYNAMIC,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Non-test failure: I3DXCreateValidTexture()");
        goto TESTFAIL;
    }

    hr = pTexture->GetSurface(0,&Srf.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Non-test failure: GetSurface()");
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

    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetBaseSurfaceDimensions()");
        goto TESTFAIL;
    }

    msg->d3dFormat  = pTexture->GetFormat();   
    msg->hTexHandle = pTexture->GetSharedHandle();
    pTexture->GetSurfaceCount(&msg->uiLevels);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
        goto TESTFAIL;
    }

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    if(FAILED(I3DXFillTexture(m_pDevice,pTexture,I3DXFillSolid,(void*)&FILL_COLOR,0)))
    {       
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to I3DXFillTexture()");
        goto TESTFAIL;
    }
    SetEvent(m_hOwner);
    
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed WaitForSingleObject()");
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

TESTFAIL:
    if( msg )
    {
        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Skipping");
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_ValidateTexture");
            m_Log->EndTest(testHandle);
        }
        else
        {
            ThrowResult(msg, testHandle);
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_ValidateTexture");
        m_Log->EndTest(testHandle);
    }
    return hr;
}

HRESULT COwner::CreateUserProcessAndWait(BOOL bWindowed)
{
    HRESULT hr = S_OK;

    // We must NOT be under our lock.
    if( m_pSharedObject->UnderLock() )
    {
        PATH_TRAP();
        return TYPE_E_INVALIDSTATE;
    }

    hr = CreateUserProcess(bWindowed);

    if(SUCCEEDED(hr))
    {
        SetEvent(m_hOwner);

        if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
        {
            hr = E_FAIL;

            LOG_AND_DBG_MSG(m_Log,L"Wait for user to start timed out");
        }
    }
    else
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create user process");
    }

    return hr;
}


HRESULT COwner::LockTexture(UINT Flags, CI3DTexture* pTexture)
{
    HRESULT hr = S_OK;
    CI3DSurface2Ptr pSurf;
    D3DLOCKED_RECT Rect;

    hr = pTexture->GetSurface(0, &pSurf.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed: GetSurface()");
        goto TESTFAIL;
    }

    hr = pSurf->Lock(Flags, NULL, &Rect);	
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed: Unlock()");
        goto TESTFAIL;
    }
TESTFAIL:
    return hr;
}


HRESULT COwner::UnlockTexture(CI3DTexture* pTexture)
{
    HRESULT hr = S_OK;
    CI3DSurface2Ptr pSurf;

    hr = pTexture->GetSurface(0, &pSurf.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed: GetSurface()");
        goto TESTFAIL;
    }

    hr = pSurf->Unlock();
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed: Unlock()");
        goto TESTFAIL;
    }

TESTFAIL:
    return hr;
}

// Must be called under the lock.
HRESULT COwner::UseTexture(Message * msg, CI3DTexture* pTexture)
{
    HRESULT hr = S_OK;
    CI3DTexturePtr  pTempTexture;
    CI3DSurface2Ptr Srf1, Srf2;
    RECT            rect;

    // We must be under our lock.
    if( !m_pSharedObject->UnderLock() )
    {
        PATH_TRAP();
        return TYPE_E_INVALIDSTATE;
    }

    assert(pTexture);
    assert(msg);

    //create a lockable texture identical to the shared texture
    hr = m_pDevice->CreateTexture(  &pTempTexture.p,
                                    msg->uiWidth,
                                    msg->uiHeight,
                                    msg->uiLevels,
                                    I3DSURF_DYNAMIC,
                                    msg->d3dFormat,                                 
                                    D3DDDIPOOL_VIDEOMEMORY,
                                    NULL);

    if(SUCCEEDED(hr))
    {
        pTexture->GetSurface(0, &Srf1.p);
        pTempTexture->GetSurface(0,&Srf2.p);

        rect.top    = rect.left = 0;
        rect.right  = msg->uiWidth;
        rect.bottom = msg->uiHeight;


        //bit the shared surface to a new surface to make it resident in video memory
        hr = m_pDevice->Blt(Srf1.p,
                            Srf2.p,
                            rect,
                            rect,
                            0,
                            0x00000000);

        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Failed to use shared surface");
        }		
    }
    else
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create identical non-shared texture");
    }

    return hr;
}

// Must be called under the lock.
HRESULT COwner::CreateAndFillTexture(Message * msg, UINT Flags, CI3DTexture** ppTexture)
{
    assert(ppTexture);

    HRESULT hr = S_OK;
    CI3DSurface2Ptr Srf;

    // We must be under our lock.
    if( !m_pSharedObject->UnderLock() )
    {
        PATH_TRAP();
        return TYPE_E_INVALIDSTATE;
    }

    //create 1 MB shared resource
    hr = I3DXCreateValidTexture2(m_pDevice,
                                 1,
                                 I3DFORMAT_ARGB,
                                 Flags,
                                 D3DDDIPOOL_VIDEOMEMORY,
                                 false,
                                 1024,	// width
                                 512,	// height
                                 ppTexture);

    if(SUCCEEDED(hr))
    {
        CI3DTexture* pTexture = *ppTexture;

        hr = I3DXFillTexture(m_pDevice,pTexture,I3DXFillSolid,(void*)&FILL_COLOR,0);

        if(SUCCEEDED(hr))
        {
            if(msg)
            {
                hr = pTexture->GetSurface(0,&Srf.p);
    
                if(SUCCEEDED(hr))
                {
                    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);

                    if(SUCCEEDED(hr))
                    {
                        msg->d3dFormat  = pTexture->GetFormat();   
                        msg->hTexHandle = pTexture->GetSharedHandle();

                        hr = pTexture->GetSurfaceCount(&msg->uiLevels);
                        if(FAILED(hr))
                        {
                            LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
                        }
                    }
                    else
                    {
                        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetBaseSurfaceDimensions()");
                    }
                }
                else
                {
                    LOG_AND_DBG_MSG(m_Log,L"Non-test failure: GetSurface()");
                }
            }
        }
        else
        {
            LOG_AND_DBG_MSG(m_Log,L"Non-test failure: I3DXFillTexture()");
        }
    }
    else
    {
        LOG_AND_DBG_MSG(m_Log,L"Non-test failure: I3DXCreateValidTexture()");
    }

    return hr;
}

// must not be called under the lock.
HRESULT COwner::UserDoWork()
{
    HRESULT hr = S_OK;

    // We must NOT be under our lock.
    if( m_pSharedObject->UnderLock() )
    {
        PATH_TRAP();
        return TYPE_E_INVALIDSTATE;
    }
    //
    //	Set event to signal user to start doing its work
    //
    SetEvent(m_hOwner);

    //
    //	Wait for user to complete its work and signal us back
    //
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        hr = E_FAIL;
        
        LOG_AND_DBG_MSG(m_Log,L"User didn't finish its work in time");
    }		

    return hr;
}


HRESULT COwner::CheckTexturePattern(CI3DTexture* pTexture)
{
    HRESULT hr = S_OK;
    CI3DSurface2Ptr Srf;
    SColor          Ret;
    D3DRECT         TargetRect;

    pTexture->GetSurface(0, &Srf.p);

    TargetRect.x1 = TargetRect.y1 = 0;
    TargetRect.x2 = TargetRect.y2 = 1;

    hr = UtilReadPixel(Srf, &TargetRect, &Ret);

    if(SUCCEEDED(hr))
    {
        //check that the color is FILL_COLOR
        float deviation = UtilCalcDeviation(&Ret,FILL_COLOR.a, FILL_COLOR.r, FILL_COLOR.g, FILL_COLOR.b);
        if(deviation > 0.1f)
        {
            hr = E_FAIL;
        }
    }
    else
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to read pixels of shared surface");
    }
    
    return hr;
}


HRESULT COwner::CauseEviction()
{
    HRESULT hr = S_OK;
    const UINT TexCount = 768;
    CI3DTexturePtr pTex[TexCount];
    Message msg;

    ZeroMemory(&msg, sizeof(Message));

    I3DXGetValidResourceFormat(m_pDevice, I3DFORMAT_ARGB, I3DSURF_TEXTURE, false, &msg.d3dFormat);
    msg.uiWidth = 1024;
    msg.uiHeight = 1024;
    msg.uiLevels = 1;
    
    //
    //	Create and use 1.5 GB worth of allocations to cause evictions
    //
    for(UINT i = 0; i < TexCount; i++)
    {
        if(SUCCEEDED(hr))
        {
            hr = I3DXCreateValidTexture2(m_pDevice,
                                         msg.uiLevels,
                                         I3DFORMAT_ARGB,
                                         0,
                                         D3DDDIPOOL_VIDEOMEMORY,
                                         false,
                                         msg.uiWidth,	
                                         msg.uiHeight,	
                                         &pTex[i].p);

            if(SUCCEEDED(hr))
            {
                hr = UseTexture(&msg, pTex[i].p);
            }
        }
        else
        {
            break;
        }
    }

    return hr;
}


HRESULT COwner::XProcTest_EvictSharedAllocation(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    CI3DTexturePtr pTexture;
    Message * msg   = NULL; 

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    msg->dwTestId   = eXProcTest_EvictSharedAllocation; 

    hr = CreateAndFillTexture(msg, I3DSURF_SHAREDRESOURCE | I3DSURF_DYNAMIC, &pTexture.p);

    if(SUCCEEDED(hr))
    {
        hr = UseTexture(msg, pTexture);

        if(SUCCEEDED(hr))
        {
            // Now free our lock
            m_pSharedObject->ReleaseLock(&msg);

            hr = CreateUserProcessAndWait();

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

                if(SUCCEEDED(msg->hrUser))
                {
                    CauseEviction();

                    // Now free our lock
                    m_pSharedObject->ReleaseLock(&msg);

                    UserDoWork();

                    // We need to lock access to our message info.
                    msg = m_pSharedObject->AcquireLock();
                    if( !msg )
                    {
                        PATH_TRAP();
                        hr = STG_E_LOCKVIOLATION;
                        goto TESTFAIL;
                    }
                    // Now we're ready to perform our work.

                    if(SUCCEEDED(msg->hrUser))
                    {
                        //
                        //	Allocation should have been paged in because user used it.
                        //  Check allocation pattern.
                        //
                        hr = CheckTexturePattern(pTexture);

                        if(FAILED(hr))
                        {
                            LOG_AND_DBG_MSG(m_Log,L"Allocation pattern doesn't match after the eviction");
                        }
                    }
                    else
                    {
                        LOG_AND_DBG_MSG(m_Log,L"User failed to use shared allocation");
                    }
                }
                else
                {
                    LOG_AND_DBG_MSG(m_Log,L"User failed to open shared allocation");
                }
            }
            else
            {
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
            }
        }
        else
        {
            LOG_AND_DBG_MSG(m_Log,L"Failed to use shared texture");
        }
    }
    else
    {
        LOG_AND_DBG_MSG(m_Log,L"Owner failed to create shared surface");
    }
TESTFAIL:  
    if( msg )
    {
        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Skipping");
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_EvictSharedAllocation");
            m_Log->EndTest(testHandle);
        }
        else
        {
            ThrowResult(msg, testHandle);     
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProcTest_EvictSharedAllocation");
        m_Log->EndTest(testHandle);
    }
    return hr;
}


HRESULT COwner::XProcTest_CrossProcSwizzlingRangeContention(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    const UINT numTextures = 256;	// create enough allocations to claim all swizzling ranges
    CI3DTexturePtr pTexture[numTextures];
    bool bFailTest = false;

    Message * msg = NULL; 

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    msg->dwTestId   = eXProcTest_CrossProcSwizzlingRangeContention; 
    msg->lCount = m_TestCaseParams.nNum_SwizzlingRangeContentionSecondsToRun; // time to run this test in seconds

    m_syncronizationTimeout = 600000; // Set up for this test can take a while (create/use/lock a lot of allocations)

    //
    //	Create allocations in video memory
    //
    for(UINT i = 0; i < numTextures; i++)
    {
        hr = CreateAndFillTexture(msg, I3DSURF_DYNAMIC, &pTexture[i].p);

        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"SwizzlingRangeContention owner failed to create enough allocations");
            break;
        }

        hr = UseTexture(msg, pTexture[i].p);

        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"SwizzlingRangeContention owner failed to use enough allocations");
            break;
        }
    }

    if(SUCCEEDED(hr))
    {
        //
        //	Proceed to lock all allocation without unlocking them
        //
        UINT i = 0;
        for(i = 0; i < numTextures; i++)
        {
            hr = LockTexture(0, pTexture[i].p);

            if(FAILED(hr))
            {
                //
                //	We shouldn't fail to lock non-pinned allocations, this should be investigated
                //
                bFailTest = true;
                LOG_AND_DBG_MSG(m_Log,L"SwizzlingRangeContention owner failed to lock non-swizzled allocations");
                break;
            }
        }
    
        if(SUCCEEDED(hr))
        {
            // Now free our lock
            m_pSharedObject->ReleaseLock(&msg);

            //
            //	At this point swizzling ranges should be taken. Start user who will try to grab swizzling ranges from us
            //	User has to be full-screen in order to be able to create primaries
            //
            hr = CreateUserProcessAndWait(FALSE);

            // We need to lock access to our message info.
            msg = m_pSharedObject->AcquireLock();
            if( !msg )
            {
                PATH_TRAP();
                hr = STG_E_LOCKVIOLATION;
                goto TESTFAIL;
            }
            // Now we're ready to perform our work.

            if(SUCCEEDED(hr))
            {
                if(SUCCEEDED(msg->hrUser))
                {
                    //
                    //	Set event to signal user to start doing its work
                    //
                    SetEvent(m_hOwner);

                    PATH_TRAP();
                    //
                    //	We will run for number of seconds specified by the test configuration
                    //
                    time_t StartTime = time(NULL);
                    time_t CurrentTime = StartTime;

                    while((CurrentTime - StartTime) < msg->lCount)
                    {
                        // Now free our lock
                        m_pSharedObject->ReleaseLock(&msg);

                        PATH_TRAP();
                        //
                        //	If anything goes wrong on user it will complete its work and signal us,
                        //	so check for that event
                        //
                        if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, 0))
                        {
                            PATH_TRAP();
                            //
                            //	Now user is doing its work which results in our swizzling ranges being reclaimed
                            //  by the pinned allocations. To create condition of contention for swizzling ranges
                            //  between 2 processes, we need to also start unlocking (to temporarily release the range)
                            //  and re-locking right away (at which point we might end up in situation where both processes
                            //	will try to grab this range)
                            //
                            for(UINT y = 0; y < numTextures; y++)
                            {

                                hr = UnlockTexture(pTexture[y].p);

                                if(FAILED(hr))
                                {
                                    LOG_AND_DBG_MSG(m_Log,L"SwizzlingRangeContention owner failed to unlock non-swizzled allocations");
                                    bFailTest = true;
                                    break;
                                }

                                //
                                //	CheckTexturePattern will result in allocation being locked, its content analyzed 
                                //	and finally, allocation being unlocked
                                //
                                hr = CheckTexturePattern(pTexture[y].p);

                                if(FAILED(hr))
                                {
                                    LOG_AND_DBG_MSG(m_Log,L"SwizzlingRangeContention owner allocation pattern doesn't match");
                                    bFailTest = true;
                                    break;
                                }

                                //
                                //	We should never fail to lock non-pinned allocation
                                //
                                hr = LockTexture(0, pTexture[y].p);

                                if(FAILED(hr))
                                {
                                    LOG_AND_DBG_MSG(m_Log,L"SwizzlingRangeContention owner failed to lock non-swizzled allocations");
                                    bFailTest = true;
                                    break;
                                }
                            }

                            if(FAILED(hr))
                            {
                                break;
                            }

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
                        }
                        else
                        {
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
                            //
                            //	Make sure user error code is set to indicate failure
                            //
                            if(SUCCEEDED(msg->hrUser))
                            {
                                LOG_AND_DBG_MSG(m_Log,L"SwizzlingRangeContention: User error code wasn't set to reflect an error condition");
                                msg->hrUser = E_FAIL;
                            }
                        }

                        CurrentTime = time(NULL);
                    }

                    PATH_TRAP();
                    // Now free our lock
                    m_pSharedObject->ReleaseLock(&msg);

                    //
                    //	Signal user to exit and wait for him to finish
                    //
                    UserDoWork();
                }
                else
                {
                    LOG_AND_DBG_MSG(m_Log,L"SwizzlingRangeContention: User failed to start");
                    // user process could not get configured for testing
                    hr = E_FAIL;
                }
            }
            else
            {
                LOG_AND_DBG_MSG(m_Log,L"SwizzlingRangeContention: Failed to create user");
            }
        }

        if(SUCCEEDED(hr))
        {
            //
            //	Unlock all allocations we locked so far
            //
            while(i--)
            {
                hr = UnlockTexture(pTexture[i].p);
                if( FAILED(hr) )
                {
                    PATH_TRAP();
                    break;
                }
            }
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

    m_syncronizationTimeout = 120000; 
TESTFAIL:
    if( msg )
    {
        if(FAILED(hr))
        {
            if(!bFailTest)
            {
                m_Log->Skip(testHandle, L"XProcTest_CrossProcSwizzlingRangeContention");
                m_Log->EndTest(testHandle);
            }
            else
            {
                PATH_TRAP();
                m_Log->Fail(testHandle, L"XProcTest_CrossProcSwizzlingRangeContention");
                m_Log->EndTest(testHandle);
            }
        }
        else
        {
            ThrowResult(msg, testHandle);     
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        PATH_TRAP();
        if(FAILED(hr))
        {
            PATH_TRAP();
            if(!bFailTest)
            {
                PATH_TRAP();
                m_Log->Skip(testHandle, L"XProcTest_CrossProcSwizzlingRangeContention");
                m_Log->EndTest(testHandle);
            }
            else
            {
                PATH_TRAP();
                m_Log->Fail(testHandle, L"XProcTest_CrossProcSwizzlingRangeContention");
                m_Log->EndTest(testHandle);
            }
        }
        else
        {
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_CrossProcSwizzlingRangeContention");
            m_Log->EndTest(testHandle);
        }

    }
    return hr;
}


HRESULT COwner::XProcTest_MultiUserScenario(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    CI3DSurface2Ptr Srf;
    D3DLOCKED_RECT d3dRect;
    D3DRECT rect;
    CI3DTexturePtr pTexture;
    Message * msg = NULL; 

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    msg->dwTestId = eXProcTest_MultiUserScenario; 
    

    //create shared resource
    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE | I3DSURF_DYNAMIC,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Non-test failure: I3DXCreateValidTexture()");
        goto TESTFAIL;
    }
    
    hr = pTexture->GetSurface(0,&Srf.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Non-test failure: GetSurface()");
        goto TESTFAIL;
    }

    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetBaseSurfaceDimensions()");
        goto TESTFAIL;
    }

    msg->d3dFormat  = pTexture->GetFormat();   
    msg->hTexHandle = pTexture->GetSharedHandle();
    pTexture->GetSurfaceCount(&msg->uiLevels);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed on call to GetSurfaceCount()");
        goto TESTFAIL;
    }

    if(FAILED(I3DXFillTexture(m_pDevice,pTexture,I3DXFillSolid,(void*)&FILL_COLOR,0)))
    {       
        goto TESTFAIL;
    }
    int i, j = 0;

    for(i = 0; i < m_TestCaseParams.nNum_MultiUsers; i++)
    {
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);

        hr = CreateUserProcess();

        // We need to lock access to our message info.
        msg = m_pSharedObject->AcquireLock();
        if( !msg )
        {
            PATH_TRAP();
            hr = STG_E_LOCKVIOLATION;
            goto TESTFAIL;
        }
        // Now we're ready to perform our work.

        msg->dwTestId = eXProcTest_MultiUserScenario; 

        if(FAILED(hr))
        {
            // If we fail to create a user process, it may not be a bad thing, thus
            // we'll continue if we have at least one.
            if( msg->lCount )
            {
                LOG_AND_DBG_MSG(m_Log,L"Failed to create user process, ignoring");
                hr= S_OK;
            }
            else
            {
                LOG_AND_DBG_MSG(m_Log,L"Failed to create user process, skipping test");
            }
            break;
        }
        else
        {
            msg->lCount++;
        }
    }

    // If we failed to create a process above we could have broken out of the for
    // loop while holding the lock. Thus, we'll unlock here.
    if( msg )
    {
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }

    //all processes should be waiting for an event at this point
    //because this is an autoreset event; we need to call SetEvent once
    //for each waiting process
    for(j = 0; j < i; j++)
    {
        SetEvent(m_hOwner);
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

    // Now wait for all our user's to exit.
    while( msg->lCount )
    {
#ifdef DBG
        {
            // Output our current lCount
            WCHAR wszDbg[MAX_PATH];
            StringCchPrintf(wszDbg,MAX_PATH,L"Count: %d",msg->lCount);
            DBG_MSG(wszDbg);
        }
#endif
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);

        DWORD dwRet = WaitForSingleObject(m_hUser, m_syncronizationTimeout);

        // We need to lock access to our message info.
        msg = m_pSharedObject->AcquireLock();
        if( !msg )
        {
            PATH_TRAP();
            hr = STG_E_LOCKVIOLATION;
            goto TESTFAIL;
        }
        // Now we're ready to perform our work.

        if(dwRet != WAIT_OBJECT_0 )
        {
            // If we timed out here, we ran tests for at least m_syncronizationTimeout milliseconds.
            // At the same time, at the m_syncronizationTimeout millisecond threshold,
            // no new user process will run (they will have timed out). Thus, we don't
            // need to keep waiting. Let's process the test results.
            goto TESTFAIL;
        }
    }

TESTFAIL:
    if(msg)
    {
        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Skipping");
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProcTest_MultiUserScenario");
            m_Log->EndTest(testHandle);
        }
        else
        {
            ThrowResult(msg, testHandle);     
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        m_Log->Skip(testHandle, L"XProcTest_MultiUserScenario");
        m_Log->EndTest(testHandle);
    }
    return hr;
}

HRESULT COwner::XProcTest_RoundRobin(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    Message * msg = NULL;

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        goto TESTFAIL;
    }
    // Now we're ready to perform our work.

    msg->dwTestId   = eXProcTest_RoundRobin;
    msg->lCount     = 0;
    msg->lCount2    = m_TestCaseParams.nNum_RoundRobins;
    msg->hrUser     = S_OK;

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    hr = CreateUserProcess();    
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed CreateUserProcess()");
        goto TESTFAIL;
    }
    SetEvent(m_hOwner);    
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hRoundRobinOwner, m_syncronizationTimeout))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed WaitForSingleObject()");
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

    hr = msg->hrUser;

TESTFAIL:
    if( msg )
    {
        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Skipping");
            m_Log->Skip(testHandle, L"XProcTest_RoundRobin");
            m_Log->EndTest(testHandle);
        }
        else
        {
            ThrowResult(msg, testHandle);
        }
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
    }
    else
    {
        m_Log->Skip(testHandle, L"XProcTest_RoundRobin");
        m_Log->EndTest(testHandle);
    }
    return hr;
}

HRESULT COwner::XProcTest_OpenClose_RaceCondition(HANDLE testHandle)
{
    HRESULT hr = S_OK;
    Message * msg = NULL;

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

    msg->dwTestId   = eXProcTest_OpenClose_RaceCondition;

    for(int i = 0; i < m_TestCaseParams.nNum_OpenClose; i++)
    {
        msg->lCount     = TRUE; //boolean to tell the new process that it should take ownership role
        msg->hrUser     = S_OK;

        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);

        CreateUserProcess();

        SetEvent(m_hOwner);    
        if(WAIT_OBJECT_0 != WaitForSingleObject(m_hRoundRobinOwner, m_syncronizationTimeout))
        {
            hr = E_FAIL;
            LOG_AND_DBG_MSG(m_Log,L"Syncronization timeout occurred.\n");
            goto TESTFAIL;
        }

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
    }
TESTFAIL:
    if( msg )
    {
        PATH_TRAP();
        if(FAILED(hr))
        {
            PATH_TRAP();
            m_Log->Skip(testHandle, L"XProc_OpenClose_RaceCondition");
            m_Log->EndTest(testHandle);
        }
        else
        {
            PATH_TRAP();
            m_Log->EndTest(testHandle); 
        }
    }
    else
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"XProc_OpenClose_RaceCondition");
        m_Log->EndTest(testHandle);
    }

    return hr;
}


//
// CodePathStep 5:
//
//  Here ParseINI() has determined what test we're going to run, thus we go run it
// here. To continue this example, we're going to follow XProcTest_UserTriesToLock.
//
HRESULT  COwner::Run()
{        
    HRESULT hr = S_OK;
    HANDLE testHandle = NULL;
    
    DBG_MSG(L"Enter");
    if(m_TestCaseParams.fIB_Basic_CreateOpen)
    {
        DBG_MSG(L"Running XProcTest_IndexBuffer_Open()");
        testHandle = m_Log->StartTest(L"XProcTest_IndexBuffer_Open");
        XProcTest_IndexBuffer_Open(testHandle);
    }
    if(m_TestCaseParams.fVB_Basic_CreateOpen)
    {
        DBG_MSG(L"Running XProcTest_VertexBuffer_Open()");
        testHandle = m_Log->StartTest(L"XProcTest_VertexBuffer_Open");
        XProcTest_VertexBuffer_Open(testHandle);
    }    
    if(m_TestCaseParams.fTex_FUSTest)
    {
        DBG_MSG(L"Running XProcTest_FUSTest()");
        testHandle = m_Log->StartTest(L"XProcTest_FUSTest");
        XProcTest_FUSTest(testHandle);
    }
    if(m_TestCaseParams.fTex_Basic_CreateOpen)
    {
        DBG_MSG(L"Running XProcTest_Basic_CreateOpen()");
        testHandle = m_Log->StartTest(L"XProcTest_Basic_CreateOpen");
        XProcTest_Basic_CreateOpen(testHandle);        
    }
    if(m_TestCaseParams.fTex_InvalidFormat )
    {
        DBG_MSG(L"Running XProcTest_InvalidFormat()");
        testHandle = m_Log->StartTest(L"XProcTest_InvalidFormat");
        XProcTest_InvalidFormat(testHandle);        
    }
    if(m_TestCaseParams.fTex_InvalidHandle_DecrementOne)
    {
        DBG_MSG(L"Running XProcTest_InvalidHandle_DecrementOne()");
        testHandle = m_Log->StartTest(L"XProcTest_InvalidHandle_DecrementOne");
        XProcTest_InvalidHandle_DecrementOne(testHandle);        
    }
    if(m_TestCaseParams.fTex_InvalidHandle_IncrementOne)
    {
        DBG_MSG(L"Running XProcTest_InvalidHandle_IncrementOne()");
        testHandle = m_Log->StartTest(L"XProcTest_InvalidHandle_IncrementOne");
        XProcTest_InvalidHandle_IncrementOne(testHandle); 
    }
    if(m_TestCaseParams.fTex_InvalidHeightDimension)
    {
        DBG_MSG(L"Running XProcTest_InvalidHeightDimension()");
        testHandle = m_Log->StartTest(L"XProcTest_InvalidHeightDimension");
        XProcTest_InvalidHeightDimension(testHandle);        
    }
    if(m_TestCaseParams.fTex_InvalidLevel )
    {
        DBG_MSG(L"Running XProcTest_InvalidLevel()");
        testHandle = m_Log->StartTest(L"XProcTest_InvalidLevel");
        XProcTest_InvalidLevel(testHandle);        
    }
    if(m_TestCaseParams.fTex_OpenClose_RaceCondition)
    {        
        DBG_MSG(L"Running XProcTest_OpenClose_RaceCondition()");
        testHandle = m_Log->StartTest(L"XprocTest_OpenClose_RaceCondition");
        XProcTest_OpenClose_RaceCondition(testHandle);
    }
    if(m_TestCaseParams.fTex_InvalidWidthDimension )
    {
        DBG_MSG(L"Running XProcTest_InvalidWidthDimension()");
        testHandle = m_Log->StartTest(L"XProcTest_InvalidWidthDimension");
        XProcTest_InvalidWidthDimension(testHandle);        
    }
    if(m_TestCaseParams.fTex_OwnerGoesAway)
    {
        DBG_MSG(L"Running XProcTest_OwnerGoesAway()");
        testHandle = m_Log->StartTest(L"XProcTest_OwnerGoesAway");
        XProcTest_OwnerGoesAway(testHandle);        
    }
    if(m_TestCaseParams.fTex_OwnerTriesToLock)
    {
        DBG_MSG(L"Running XProcTest_OwnerTriesToLock()");
        testHandle = m_Log->StartTest(L"XProcTest_OwnerTriesToLock");
        XProcTest_OwnerTriesToLock(testHandle);        
    }
    if(m_TestCaseParams.fTex_UserTriesToLock )
    {
        DBG_MSG(L"Running XProcTest_UserTriesToLock()");
        testHandle = m_Log->StartTest(L"XProcTest_UserTriesToLock");
        XProcTest_UserTriesToLock(testHandle);        
    }
    if(m_TestCaseParams.fTex_ValidateTexture)
    {
        DBG_MSG(L"Running XProcTest_ValidateTexture()");
        testHandle = m_Log->StartTest(L"XProcTest_ValidateTexture");
        XProcTest_ValidateTexture(testHandle);       
    }
    if(m_TestCaseParams.fTex_EvictSharedAllocation)
    {
        DBG_MSG(L"Running XProcTest_EvictSharedAllocation()");
        testHandle = m_Log->StartTest(L"XProcTest_EvictSharedAllocation");
        XProcTest_EvictSharedAllocation(testHandle);
    }
    if(m_TestCaseParams.fTex_MultiUserScenario)
    {
        DBG_MSG(L"Running XProcTest_MultiUserScenario()");
        testHandle = m_Log->StartTest(L"XProcTest_MultiUserScenario");
        XProcTest_MultiUserScenario(testHandle);        
    }
    if(m_TestCaseParams.fTex_OpenClose_RaceCondition)
    {        
        DBG_MSG(L"Running XProcTest_OpenClose_RaceCondition()");
        testHandle = m_Log->StartTest(L"XprocTest_OpenClose_RaceCondition");
        XProcTest_OpenClose_RaceCondition(testHandle);
    }
    if(m_TestCaseParams.fTex_CrossAdapterNotShare)
    {
        DBG_MSG(L"Running XProcTest_CrossAdapterNotShare()");
        testHandle = m_Log->StartTest(L"XProcTest_CrossAdapterNotShare");
        XProcTest_CrossAdapterNotShare(testHandle);
    }
    if(m_TestCaseParams.fTex_CrossSessionNotShare)
    {
        DBG_MSG(L"Running XProcTest_CrossSessionNotShare()");
        testHandle = m_Log->StartTest(L"XProcTest_CrossSessionNotShare");
        XProcTest_CrossSessionNotShare(testHandle);
    }
    if(m_TestCaseParams.fTex_CrossProcSwizzlingRangeContention)
    {
        DBG_MSG(L"Running XProcTest_CrossProcSwizzlingRangeContention()");
        testHandle = m_Log->StartTest(L"XProcTest_CrossProcSwizzlingRangeContention");
        XProcTest_CrossProcSwizzlingRangeContention(testHandle);
    }
    //
    //	Moving RoundRobin test to the end. There seems to be some issue with synchronyzing between
    //	different users which might result in user still being created after the owner is exited 
    //  and the next test has started (which caused this latest user to fail to CreateDC when this 
    //  test was followed by the FUS tests, which switched away to another session)
    //
    if(m_TestCaseParams.fTex_RoundRobin)
    {
        DBG_MSG(L"Running XProcTest_RoundRobin()");
        testHandle = m_Log->StartTest(L"XprocTest_RoundRobin");
        XProcTest_RoundRobin(testHandle);
    }    
    DBG_MSG(L"Leave");
    return hr;
}

//
// CodePathStep 4:
//
//  Here we're setting up the configuration of the first instance of CrossProc.
// We allocate the different events that will be used to synchronize between the
// two instances along with the Message structure (m_lpMapAddr) that will be accessed
// by both. Note that ParseINI() also determines what tests will be run.
//
HRESULT COwner::Initialize(BOOL bWindowed)
{    
    HRESULT hr = S_OK;
    DeviceCreateOptions Opts;
    PSECURITY_DESCRIPTOR pSD = NULL;
    bool bLDDM=false;

    // WTT Logger initialization
    m_Log = new CCTLLogger(L"crossproc.wtl");
    if(m_Log == NULL)
    {
        hr = E_NOINTERFACE;
        goto INITFAIL;
    }

    ParseINI();

    LOG_AND_DBG_MSG(m_Log,L"Enter");

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
        LOG_AND_DBG_MSG(m_Log,L"Skipping due to: Failed I3DIsLDDM() check (owner)");
        goto INITFAIL;
    }

    if(false == bLDDM)
    {
        LOG_AND_DBG_MSG(m_Log,L"Display adapter is not using WDDM drivers");
        hr = E_OUTOFMEMORY;
        goto INITFAIL;
    }

    //Create a security descriptor and set DACL to NULL which will 
    //allow all processes to access the events and file mapping.  
    pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, 
                             SECURITY_DESCRIPTOR_MIN_LENGTH); 
    if(NULL == pSD)
    {
        LOG_AND_DBG_MSG(m_Log,L"LocalAlloc failed");
        hr = E_OUTOFMEMORY;
        goto INITFAIL;
    }

    if(!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
    {
        LOG_AND_DBG_MSG(m_Log,L"InitializeSecurityDescriptor failed");
        hr = E_FAIL;
        goto INITFAIL;
    }

    //Make sure we have a null DACL so anyone can access this
    // OACR warning 6248 : Setting a SECURITY_DESCRIPTOR's DACL to NULL will result in an unprotected object
    // but that was this test wants. Turn the warning off

#pragma warning( push )
#pragma warning( disable: 6248 )
    if(!SetSecurityDescriptorDacl(pSD, TRUE, NULL, FALSE))
#pragma warning( pop )
    {
        LOG_AND_DBG_MSG(m_Log,L"Setting of NULL DACL failed");
        hr = E_FAIL;
        goto INITFAIL;
    }

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = pSD;
    sa.bInheritHandle = false;

    m_hOwner = CreateEvent(&sa, FALSE, FALSE, L"Global\\CrossProc::Event_Owner");
    if(m_hOwner == NULL)
    {
        LOG_AND_DBG_MSG(m_Log,L"Unable to create owner event");
        hr = E_UNEXPECTED;
        goto INITFAIL;
    }

    m_hUser  = CreateEvent(&sa, FALSE, FALSE, L"Global\\CrossProc::Event_User");
    if(m_hUser == NULL)
    {
        LOG_AND_DBG_MSG(m_Log,L"Unable to create user event");
        hr = E_UNEXPECTED;
        goto INITFAIL;
    }

    m_hRoundRobinOwner = CreateEvent(NULL, FALSE, FALSE, L"CrossProc::Event_RoundRobinOwner");
    if(m_hRoundRobinOwner == NULL)
    {
        LOG_AND_DBG_MSG(m_Log,L"Unable to create RoundRobin owner event");
        hr = E_UNEXPECTED;
        goto INITFAIL;
    }

    m_pSharedObject = new sSharedMemory();
    if(m_pSharedObject == NULL)
    {
        PATH_TRAP();
        LOG_AND_DBG_MSG(m_Log,L"Unable to create shared memory region object");
        hr = E_OUTOFMEMORY;
        goto INITFAIL;
    }
    hr = m_pSharedObject->Initialize(&sa,g_syncronizationTimeout);
    if(FAILED(hr))
    {
        PATH_TRAP();
        LOG_AND_DBG_MSG(m_Log,L"Failed to Initialize shared memory object");
        hr = E_OUTOFMEMORY;
        goto INITFAIL;
    }

    try
    {
        // CreateI3D can throw on failure if the primary device is not found. Thus
        // we're going to try to safely get through this and return an error.
        hr = CreateI3D(&m_p3D.p, NULL);
    }
    catch(...)
    {
        hr = E_NOTIMPL;
    }
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create I3D");
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
        LOG_AND_DBG_MSG(m_Log,L"Failed to GetValidDeviceCreateOptions");
        goto INITFAIL;
    }
   
    hr = m_p3D->CreateDevice(&Opts, &m_pDevice.p);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to CreateDevice");
        goto INITFAIL;
    }

    if(pSD != NULL) 
    {
        LocalFree(pSD);
        pSD = NULL;
    }
    return hr;

INITFAIL:
    if(pSD != NULL) 
    {
        LocalFree(pSD);
        pSD = NULL;
    }
    DBG_MSG(L"Leave");
    return hr;
}

HRESULT COwner::Cleanup()
{
    HRESULT hr = S_OK;

    if( m_pSharedObject )
    {
        delete m_pSharedObject;
        m_pSharedObject = NULL;
    }

    if( m_hOwner )
        CloseHandle(m_hOwner);
    if( m_hUser )
        CloseHandle(m_hUser);
    if( m_hRoundRobinOwner )
        CloseHandle(m_hRoundRobinOwner);

    return hr;
}

//----------------------------------------------------------
//This function evaluates the HRESULT for the calling function
//and determines if it is correct. If it is not then an 
//exception is thrown.
//                                                                                                          
//----------------------------------------------------------
// Must be called under the lock.
void COwner::ThrowResult(Message * msg, HANDLE testHandle)
{
    DBG_MSG(L"Enter");

    // We must be under our lock.
    if( !m_pSharedObject->UnderLock() )
    {
        PATH_TRAP();
        return;
    }

    UtilPrintMessage(msg,m_Log);

    // If we recorded something from the User process, we can dump it here.
    if(msg->bErrorLogged)
    {
        TCHAR szMessage[MAX_PATH];
        StringCchPrintf(szMessage,MAX_PATH, 
                        L"TestCase: %d %s %d %s",
                        msg->dwTestId,
                        msg->wszFunction,
                        msg->dwLine,
                        msg->wszData);
        m_Log->Message(szMessage);
        DBG_MSG(szMessage);
        msg->bErrorLogged=false;
    }

    switch(msg->dwTestId )
    {
    
        case eXProcTest_FUSTest:
            if(FAILED(msg->hrUser))
            {               
            }
            else
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_VertexBuffer_Open:
            if(FAILED(msg->hrUser))
            {
                
                m_Log->Fail(testHandle, L"");               
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break;             
        case eXProcTest_IndexBuffer_Open:
            if(FAILED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break;        
        case eXProcTest_Basic_CreateOpen:
            if(FAILED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_InvalidHandle_IncrementOne:
            if(SUCCEEDED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break; 
        case eXProcTest_InvalidHandle_DecrementOne:
            if(SUCCEEDED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_InvalidHeightDimension:
            if(SUCCEEDED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_InvalidWidthDimension:
            if(SUCCEEDED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_InvalidFormat:
            if(SUCCEEDED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_InvalidLevel:
            if(SUCCEEDED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_UserTriesToLock:
            if(SUCCEEDED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_OwnerTriesToLock:
            if(FAILED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();           
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_OwnerGoesAway:     
            if(FAILED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();           
            }
            m_Log->EndTest(testHandle);
            break;        
        case eXProcTest_ValidateTexture:
            if(FAILED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();           
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_EvictSharedAllocation:
            if(FAILED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_CrossProcSwizzlingRangeContention:
            if(FAILED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_MultiUserScenario:
            if(FAILED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();           
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_RoundRobin:
            if(FAILED(msg->hrUser))
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();           
            }
            m_Log->EndTest(testHandle);
            break;
        case eXProcTest_CrossAdapterNotShare:
            //This test is a bit different from the others, in this test if the result that
            //was returned is a failed result then the test passed, because you should
            //not be able to access a shared resource across different adapters.
            if(FAILED(msg->hrUser))
            {
            }
            else
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break;

        case eXProcTest_CrossSessionNotShare:

           //This test is a bit different from the others, in this test if the result that
           //was returned is a failed result then the test passed, because you should
           //not be able to access a shared resource across sessions.
            if(FAILED(msg->hrUser))
            {
            }
            else
            {
                m_Log->Fail(testHandle, L"");
                DebugBreak();
            }
            m_Log->EndTest(testHandle);
            break;
                
        default:
            PATH_TRAP();
            break;

    }
    DBG_MSG(L"Leave");
}


// Must not be called under the lock.
HRESULT COwner::CreateUserProcess(BOOL bWindowed)
{
    HRESULT hr = S_OK;  
    BOOL fCreated = FALSE;
    PROCESS_INFORMATION ProcInfo;
    STARTUPINFOA StartInfo;
    ZeroMemory( &StartInfo, sizeof(STARTUPINFO) );
    ZeroMemory( &ProcInfo, sizeof(PROCESS_INFORMATION) );

    DBG_MSG(L"Enter");
    StartInfo.cb = sizeof(STARTUPINFO);

    // We must NOT be under our lock.
    if( m_pSharedObject->UnderLock() )
    {
        PATH_TRAP();
        return TYPE_E_INVALIDSTATE;
    }
    //
    // If you need to debug the test running windbg.exe, you might want to enable
    // the following define. That way if you originally launch windbg like 
    // "c:\debuggers\windbg.exe crossproc.exe" you'll get the debugger a second
    // time when the second process loads.
    //
//#define WANT_TO_DEBUG_SECOND_PROCESS 1

    char * szArgs;
#ifdef WANT_TO_DEBUG_SECOND_PROCESS
    if(!bWindowed)
    {
        szArgs = " crossproc.exe user fs";
    }
    else
    {
        szArgs = " crossproc.exe user";
    }

    fCreated = CreateProcessA("c:\\debuggers\\windbg.exe",
#else
    if(!bWindowed)
    {
        szArgs = " user fs";
    }
    else
    {
        szArgs = " user";
    }

    fCreated = CreateProcessA("crossproc.exe", 
#endif
                  szArgs,
                  NULL,
                  NULL,
                  FALSE,
                  0,
                  NULL,
                  NULL,
                  &StartInfo,
                  &ProcInfo);
    if(fCreated == FALSE)
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create user process.");
        PATH_TRAP();
    }
    CloseHandle(ProcInfo.hProcess);
    CloseHandle(ProcInfo.hThread);
    
    DBG_MSG(L"Going to wait for m_hUser");
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
    {
        LOG_AND_DBG_MSG(m_Log,L"failed WaitForSingleObject");
        hr = E_FAIL;
        PATH_TRAP();
    }
    
    DBG_MSG(L"Leave");
    return hr;
}

HRESULT COwner::ParseINI()
{
    HRESULT hr = S_OK;

    //read test parameters from crossproc.ini
    m_TestCaseParams.fGlobal_BreakOnFail = GetPrivateProfileInt(L"Global",
                                                              L"BreakOnFail",
                                                              0, 
                                                              L"./crossproc.ini");

    m_TestCaseParams.fIB_Basic_CreateOpen = GetPrivateProfileInt(L"TestCases",
                                                              L"IB_Basic_CreateOpen",
                                                              0,
                                                              L"./crossproc.ini");
                                                              
    m_TestCaseParams.fVB_Basic_CreateOpen = GetPrivateProfileInt(L"TestCases",
                                                              L"VB_Basic_CreateOpen",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_Basic_CreateOpen = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_Basic_CreateOpen",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_InvalidFormat = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_InvalidFormat",
                                                              0,
                                                              L"./crossproc.ini"); 

                                                              

    m_TestCaseParams.fTex_InvalidHandle_DecrementOne  = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_InvalidHandle_DecrementOne",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_InvalidHandle_IncrementOne = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_InvalidHandle_IncrementOne",
                                                              0,
                                                              L"./crossproc.ini");


    m_TestCaseParams.fTex_InvalidHeightDimension = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_InvalidHeightDimension",
                                                              0,
                                                              L"./crossproc.ini");
    
    m_TestCaseParams.fTex_InvalidLevel  = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_InvalidLevel",
                                                              0,
                                                              L"./crossproc.ini"); 

    m_TestCaseParams.fTex_InvalidWidthDimension = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_InvalidWidthDimension",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_OwnerTriesToLock  = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_OwnerTriesToLock",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_OwnerGoesAway  = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_OwnerGoesAway",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_ReadWrite = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_ReadWrite",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_UserTriesToLock  = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_UserTriesToLock",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_ValidateTexture  = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_ValidateTexture",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_EvictSharedAllocation = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_EvictSharedAllocation",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_MultiUserScenario  = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_MultiUserScenario",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_RoundRobin  = GetPrivateProfileInt(L"TestCases",
                                                              L"Tex_RoundRobin",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_OpenClose_RaceCondition = GetPrivateProfileInt(L"TestCases",
                                                              L"Stress_OpenClose",
                                                              0,
                                                              L"./crossproc.ini");

    m_TestCaseParams.fTex_FUSTest = GetPrivateProfileInt(L"TestCases",
                                                                L"Tex_FUSTest",
                                                                0,
                                                                L"./crossproc.ini");

    m_TestCaseParams.fTex_CrossAdapterNotShare = GetPrivateProfileInt(L"TestCases",
                                                                L"Tex_CrossAdapterNotShare",
                                                                0,
                                                                L"./crossproc.ini");

    m_TestCaseParams.fTex_CrossSessionNotShare = GetPrivateProfileInt(L"TestCases",
                                                                L"Tex_CrossSessionNotShare",
                                                                0,
                                                                L"./crossproc.ini");

    m_TestCaseParams.fTex_CrossProcSwizzlingRangeContention = GetPrivateProfileInt(L"TestCases",
                                                                                   L"Tex_CrossProcSwizzlingRangeContention",
                                                                                   0,
                                                                                   L"./crossproc.ini");

    m_TestCaseParams.nNum_MultiUsers   = GetPrivateProfileInt(L"TestCaseData",
                                                              L"Num_MultiUsers",
                                                              10,
                                                              L"./crossproc.ini");

    m_TestCaseParams.nNum_RoundRobins   = GetPrivateProfileInt(L"TestCaseData",
                                                              L"Num_RoundRobins",
                                                              10,
                                                              L"./crossproc.ini");

    m_TestCaseParams.nNum_OpenClose     = GetPrivateProfileInt(L"TestCaseData",
                                                               L"Num_OpenClose",
                                                               10,
                                                               L"./crossproc.ini");

    m_TestCaseParams.nNum_FUSSleepSeconds = GetPrivateProfileInt(L"TestCaseData",
                                                                L"Num_FUSSleepSeconds",
                                                                0,
                                                                L"./crossproc.ini");
    m_TestCaseParams.nNum_SwizzlingRangeContentionSecondsToRun = GetPrivateProfileInt(L"TestCaseData",
                                                                                      L"Num_SwizzlingRangeContentionSecondsToRun",
                                                                                      60,
                                                                                      L"./crossproc.ini");
    return hr;
}

//Checks to see if any error occurred during initialization of a test, if so then
//the error is logged and a debug string is written.
bool COwner::ErrorOccurredDuringInitialization(HRESULT hr, 
                                               LPWSTR pwszTestCaseName, 
                                               LPWSTR pwszDebugErrorMessage, 
                                               HANDLE testHandle)
{
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,pwszDebugErrorMessage);
        LOG_AND_DBG_MSG(m_Log,L"Skipping");
        m_Log->Skip(testHandle, L"");
        m_Log->EndTest(testHandle);
    }

    return FAILED(hr);
}

//The purpose of this test is to verify that a resource created on one session
//cannot be accessed by a process in a different session.
HRESULT COwner::XProcTest_CrossSessionNotShare(HANDLE testHandle)
{            
    HRESULT hr = S_OK;
    LPWSTR pwszTestName = L"XProcTest_CrossSessionNotShare";
    //CR: [davidfl] This allows for direct access to the shared memory!!
    Message * msg = NULL;

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        m_Log->Fail(testHandle, L"");
        m_Log->EndTest(testHandle);
        return hr;
    }
    // Now we're ready to perform our work.

    CI3DTexturePtr pTexture;
    
    msg->dwTestId = eXProcTest_CrossSessionNotShare;
    msg->hrUser = S_OK;
  
    //Create a valid texture that the test will attempt to share 
    //across different user sessions.
    hr = I3DXCreateValidTexture(m_pDevice,
                                0,
                                I3DFORMAT_ARGB,
                                I3DSURF_SHAREDRESOURCE,
                                D3DDDIPOOL_VIDEOMEMORY,
                                false,
                                &pTexture.p);

    if(ErrorOccurredDuringInitialization(hr, pwszTestName, L"Failed to initialize texture.\n", testHandle))
    {
        PATH_TRAP();
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
        return hr;
    }

    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
    if(ErrorOccurredDuringInitialization(hr, pwszTestName, L"Failed call to GetBaseSurfaceDimension.\n", testHandle))
    {
        PATH_TRAP();
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
        return hr;
    }

    hr = pTexture->GetSurfaceCount(&msg->uiLevels );
    if(ErrorOccurredDuringInitialization(hr, pwszTestName, L"Failed call to GetSurfaceCount.\n", testHandle))
    {
        PATH_TRAP();
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
        return hr;
    }

    //Set up shared message parameters for user process.
    msg->d3dFormat = pTexture->GetFormat();   
    msg->hTexHandle = pTexture->GetSharedHandle();

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    //Create new user, switch to new user and then run another instance
    //of crossproc.exe with user flag set which will attempt to access the shared resource
    CFusHelper fusHelper(m_Log);

    hr = fusHelper.InitializeFusFramework();
    if(ErrorOccurredDuringInitialization(hr, pwszTestName, L"Call to InitFusFramework failed.\n", testHandle))
    {
        return hr;
    }

    hr = fusHelper.AddUser(m_pwszUserName, m_pwszPassword);
    if(ErrorOccurredDuringInitialization(hr, pwszTestName, L"Call to AddUser failed.\n", testHandle))
    {
        return hr;
    }
    
    hr = fusHelper.SwitchToUser(m_pwszUserName, m_pwszPassword);
    if(ErrorOccurredDuringInitialization(hr, pwszTestName, L"Call to SwitchToUser failed.\n", testHandle))
    {
        return hr;
    }

    //Need to get the directory where this test can be run from
    TCHAR lpszCurrentDirectory[MAX_PATH];
    DWORD cchCurDir = 0;
    cchCurDir = GetCurrentDirectory(MAX_PATH, lpszCurrentDirectory);
    if(ErrorOccurredDuringInitialization(StringCchCat(lpszCurrentDirectory, MAX_PATH, L"\\crossproc.exe"),
            pwszTestName, L"Unable to get executable path for crossproc.exe.\n", testHandle))
    {
        return hr;
    }

    //At this point the FUS framework is setup and the switch has happened to the different
    //user.  The result of the user version of crossproc.exe will be inside msg->hrUser
    hr = fusHelper.CreateProcessInSession(m_pwszUserName, lpszCurrentDirectory);
    if(ErrorOccurredDuringInitialization(hr, pwszTestName, L"Call to CreateProcessInSession failed.\n", testHandle))
    {
        return hr;
    }
    else
    {
        //Wait for user version of crossproc.exe to finish initializing
        DBG_MSG(L"Owner waiting for user to finish initializing.\n");
        if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
        {
            LOG_AND_DBG_MSG(m_Log,L"Skipping WaitForSingleObject() failed");
            hr = E_FAIL;
            m_Log->Skip(testHandle, L"XProcTest_CrossSessionNotShare");
            m_Log->EndTest(testHandle);
            return hr;
        }

        //User has initialized, no more processing to do for the owner object, let the user
        //attempt to access the texture.
        DBG_MSG(L"Owner waiting for user to attempt to access texture.\n");
        SetEvent(m_hOwner);
        if(WAIT_OBJECT_0 != WaitForSingleObject(m_hUser, m_syncronizationTimeout))
        {
            hr = E_FAIL;
            m_Log->Fail(testHandle, L"");
            m_Log->EndTest(testHandle);
            return hr;
        }

        //At this point the user will have tried to access the texture, the result of that 
        //will be in msg->hrUser
        DBG_MSG(L"User completed, owner cleaning up.\n");

        //Restore back to original user and remove the user we added
        fusHelper.RestoreUser();
        fusHelper.RemoveUser(m_pwszUserName);
    }

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        hr = STG_E_LOCKVIOLATION;
        m_Log->Fail(testHandle, L"");
        m_Log->EndTest(testHandle);
        return hr;
    }
    // Now we're ready to perform our work.

    //Record the outcome of the overall test
    ThrowResult(msg, testHandle);

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    return hr;
}


// The purpose of this test is to verify that a shared resource created on one 
// adapter cannot be accessed by another adapter.
HRESULT COwner::XProcTest_CrossAdapterNotShare(HANDLE testHandle)
{	
    HRESULT hr = S_OK;
    LPWSTR pwszTestName = L"XProcTest_CrossAdpaterNoShare";
    CI3DTexturePtr pTexture = NULL;
    Message * msg = NULL;

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        PATH_TRAP();
        m_Log->Skip(testHandle, L"");
        m_Log->EndTest(testHandle);
        return STG_E_LOCKVIOLATION;
    }
    // Now we're ready to perform our work.

    msg->dwTestId = eXProcTest_CrossAdapterNotShare;
    msg->hrUser = S_OK;

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);

    //Get a vector of all the displays currently attached
    //to desktops
    I3DDisplayVector attachedDisplays;
    I3DGetDisplayInfo(attachedDisplays);

    //For each of the displays we want to find the LUID of the associated
    //adapter to try and find all of the unique adapters.
    std::vector<AdapterDisplayInfo> prunedAdapters;
    vector<I3DDisplayInfo>::iterator displaysIterator;
    for(displaysIterator=attachedDisplays.begin(); displaysIterator != attachedDisplays.end(); ++displaysIterator)
    {
        //For each display check to see if we have already have an adapter
        //which is connected to this display.
        hr = GetUniqueAdapterList(displaysIterator->sName, prunedAdapters);
        if(FAILED(hr))
        {
            LOG_AND_DBG_MSG(m_Log,L"Error while attempting to fetch display adapter information.\n");
            m_Log->Skip(testHandle, L"");
            m_Log->EndTest(testHandle);
            return hr;
        }
    }    

    
    //Need two adapters for this test, if not we should skip the test
    if(prunedAdapters.size() < 2)
    {
        LOG_AND_DBG_MSG(m_Log,L"Two adapters not found (adapters must have an output connected to a desktop to be counted); test to be skipped.\n");
        m_Log->Skip(testHandle, L"");
        m_Log->EndTest(testHandle);
        return hr;
    }   

    //Creates an CI3D object and a CI3DDevice object on the first adapater
    CI3DPtr p3D1 = NULL;
    CI3DDevicePtr pDevice1 = NULL;
    DBG_MSG(L"Creating first adapter objects.\n");
    hr = InitializeD3DObjects(&p3D1, &pDevice1, prunedAdapters[0].GetDisplayName().c_str());
    if(ErrorOccurredDuringInitialization(hr, pwszTestName, L"Call to InitializeD3DObjects failed for display adapter 0.\n", testHandle))
    {
        return hr;
    }

    //Creates a CI3D and CI3DDevice object on the second adapter
    CI3DPtr p3D2 = NULL;
    CI3DDevicePtr pDevice2 = NULL;
    DBG_MSG(L"Creating second adapter objects.\n");
    hr = InitializeD3DObjects(&p3D2, &pDevice2, prunedAdapters[1].GetDisplayName().c_str());
    if(ErrorOccurredDuringInitialization(hr, pwszTestName, L"Call to InitializeD3DObjects failed for display adapter 1.\n", testHandle))
    {
        return hr;
    }

    // We need to lock access to our message info.
    msg = m_pSharedObject->AcquireLock();
    if( !msg )
    {
        m_Log->Skip(testHandle, L"");
        m_Log->EndTest(testHandle);
        return STG_E_LOCKVIOLATION;
    }
    // Now we're ready to perform our work.

    //Create a resource which we will attempt to share across the two adapters
    DBG_MSG(L"Creating texture on first adapter.\n");
    HRESULT hrTexture;
    hrTexture = I3DXCreateValidTexture(
                            pDevice1, 
                            0, 
                            I3DFORMAT_ARGB, 
                            I3DSURF_SHAREDRESOURCE,
                            D3DDDIPOOL_VIDEOMEMORY , 
                            false, 
                            &pTexture.p);

    if(ErrorOccurredDuringInitialization(hrTexture, pwszTestName, L"Failed to create texture.\n", testHandle))
    {
        PATH_TRAP();
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
        return hr;
    }

    //Get texture attributes.
    hr = pTexture->GetBaseSurfaceDimensions(&msg->uiWidth, &msg->uiHeight);
    if(ErrorOccurredDuringInitialization(hr, pwszTestName, L"GetBaseSurfaceDimensions failed.\n", testHandle))
    {
        PATH_TRAP();
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
        return hr;
    }
    
    hr = pTexture->GetSurfaceCount(&msg->uiLevels );
    if(ErrorOccurredDuringInitialization(hr, pwszTestName, L"GetSurfaceCount failed.\n", testHandle))
    {
        PATH_TRAP();
        // Now free our lock
        m_pSharedObject->ReleaseLock(&msg);
        return hr;
    }

    CI3DTexturePtr pTexture2 = NULL;
    hr = pDevice2->CreateTexture(&pTexture2.p,
                                  msg->uiWidth ,
                                  msg->uiHeight,
                                  msg->uiLevels,
                                  I3DSURF_SHAREDRESOURCE,
                                  pTexture->GetFormat(),
                                  D3DDDIPOOL_VIDEOMEMORY,
                                  pTexture->GetSharedHandle());

    //Store result of test and process
    msg->hrUser = hr;
    ThrowResult(msg, testHandle);

    // Now free our lock
    m_pSharedObject->ReleaseLock(&msg);
  
    return hr;
}


HRESULT COwner::InitializeD3DObjects(CI3DPtr* p3D, CI3DDevicePtr* pDevice, LPCWSTR pwszAdapterName)
{   
    HRESULT hr = S_OK;

    DBG_MSG(L"Creating device for adapter=");
    DBG_MSG((LPTSTR)pwszAdapterName);
    hr = CreateI3D(&((*p3D).p), pwszAdapterName);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to create I3D object.\n");
        return hr;
    }

    DeviceCreateOptions createOptions;
    hr = (*p3D)->GetValidDeviceCreateOptions
                            (true,
                            m_hWnd, 
                            WIDTH, 
                            HEIGHT, 
                            1, 
                            true, 
                            false,
                            NULL, 
                            &createOptions);
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to get valid options for device.\n");
        return hr;
    }
    
    hr = (*p3D)->CreateDevice(&createOptions, &((*pDevice).p));
    if(FAILED(hr))
    {
        LOG_AND_DBG_MSG(m_Log,L"Failed to get create device.\n");
        return hr;
    }

    return hr;
}


void COwner::DebugBreak()
{
    m_Log->Message(L"Debug breaking if enabled ...");
    if(m_TestCaseParams.fGlobal_BreakOnFail)
    {
        ::DebugBreak();
    }
}


//Hook for CreateAllocation()
//We will tack on an additional allocation to the supplied resource
HRESULT COwner::OnAllocate(D3DDDICB_ALLOCATE* pInfo, bool* pContinue)
{
    return S_OK;
}


//Gets a list of all the unique adapters in the system.
HRESULT COwner::GetUniqueAdapterList(const std::wstring displayName, std::vector<AdapterDisplayInfo>& adapterList)
{
    HRESULT hr = S_OK;
    D3DKMT_OPENADAPTERFROMHDC openAdapterFromHdc;
    NTSTATUS status;

    // Get entry points needed to get adapter list
    HINSTANCE hInstThunk = LoadLibrary(L"gdi32.dll" );
    if (hInstThunk == NULL)
    {
        return E_FAIL;
    }

    //This function allows us to find the LUID for an adapter given the 
    //display name
    PFND3DKMT_OPENADAPTERFROMHDC pfnKTOpenAdapterFromHdc = NULL;
    pfnKTOpenAdapterFromHdc = (PFND3DKMT_OPENADAPTERFROMHDC)GetProcAddress((HMODULE)hInstThunk, "D3DKMTOpenAdapterFromHdc");

    FreeLibrary(hInstThunk);
    if(pfnKTOpenAdapterFromHdc == NULL)
    {
        return E_FAIL;
    }

    HDC hDc = CreateDC(displayName.c_str(), NULL, NULL, NULL);
    if(hDc == NULL)
    {
        return E_FAIL;
    }

    ZeroMemory(&openAdapterFromHdc, sizeof(openAdapterFromHdc));
    openAdapterFromHdc.hDc = hDc;
    status = pfnKTOpenAdapterFromHdc(&openAdapterFromHdc);
    if (status < 0)
    {
        DeleteDC(hDc);
        return E_FAIL;
    }

    //If we don't already have this LUID then add it to the vector
    bool foundLuid = false;
    std::vector<AdapterDisplayInfo>::iterator adapterIterator;
    for(adapterIterator = adapterList.begin(); adapterIterator != adapterList.end(); ++adapterIterator)
    {
        LUID luid = adapterIterator->GetLuid();
        if((luid.HighPart == openAdapterFromHdc.AdapterLuid.HighPart) &&
           (luid.LowPart == openAdapterFromHdc.AdapterLuid.LowPart))
        {
            foundLuid = true;
            break;
        }
    }

    if(!foundLuid)
    {
        AdapterDisplayInfo newInfo(openAdapterFromHdc.AdapterLuid, displayName);
        adapterList.push_back(newInfo);
    }

    DeleteDC(hDc);
    return hr;
}
