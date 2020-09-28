ub,
    __in
        PCONTROLLER_STOP_IDLE_CONTEXT StopIdleContext
    )
/*++

Routine Description:

    It is a wrapper around the synchronous version of WdfDeviceStopIdle(controller) 
    with the same semantics.
    
    This allows the hub driver to take a power ref on the controller FDO.

Return Value:

    NT status code.

--*/
{
    PUCXUSBDEVICE_PRIVCONTEXT    ucxUsbDeviceContext;
    PUCXCONTROLLER_PRIVCONTEXT   ucxControllerContext;
    NTSTATUS                     status;

    TRY {
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(Hub);
        ucxControllerContext = ucxUsbDeviceContext->UcxControllerContext;

        status = WdfDeviceStopIdle(ucxControllerContext->ParentWdfDevice, TRUE);

        if (!NT_SUCCESS(status)) {
            LogError(FlagController, "WdfDeviceStopIdle failed, controller 0x%p", ucxControllerContext);
            LEAVE;
        }

        StopIdleContext->powerReferenceAcquired = TRUE;

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_ResumeIdle(
    __in
        UCXUSBDEVICE                  Hub,
    __in
        PCONTROLLER_STOP_IDLE_CONTEXT StopIdleContext
    )
/*++

Routine Description:

    Drops the power reference acquired during Controller_StopIdleSynchronous, assuming the 
    Controller_StopIdleSynchronous returned success. Otherwise this is a no-op and is optional for 
    the hub driver to call it.

Return Value:

    NT status code.

--*/
{
    PUCXUSBDEVICE_PRIVCONTEXT    ucxUsbDeviceContext;
    PUCXCONTROLLER_PRIVCONTEXT   ucxControllerContext;

    TRY {

        ucxUsbDeviceContext = UcxUsbDeviceGetContext(Hub);
        ucxControllerContext = ucxUsbDeviceContext->UcxControllerContext;

        if (StopIdleContext->powerReferenceAcquired) {
            WdfDeviceResumeIdle(ucxControllerContext->ParentWdfDevice);
            StopIdleContext->powerReferenceAcquired = FALSE;
        }

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_GetInfo(
    __in
        UCXUSBDEVICE                Hub,
    __out
        PUCX_CONTROLLER_INFO        ControllerInfo
    )
/*++

Routine Description:
    
    Return the controller parent bus type (PCI or ACPI) and bus-specific information.

--*/
{
    PUCXUSBDEVICE_PRIVCONTEXT    ucxUsbDeviceContext;
    PUCXCONTROLLER_PRIVCONTEXT   ucxControllerContext;

    TRY {

        ucxUsbDeviceContext = UcxUsbDeviceGetContext(Hub);
        ucxControllerContext = ucxUsbDeviceContext->UcxControllerContext;

        RtlZeroMemory(ControllerInfo, sizeof(UCX_CONTROLLER_INFO));

        ControllerInfo->Type = ucxControllerContext->Config.ParentBusType;

        switch (ucxControllerContext->Config.ParentBusType) {

        case UcxControllerParentBusTypePci:

            ControllerInfo->Pci = ucxControllerContext->Config.PciDeviceInfo;
            break;

        case UcxControllerParentBusTypeAcpi:

            ControllerInfo->Acpi = ucxControllerContext->Config.AcpiDeviceInfo;
            break;

        default:
            //
            // Leave the bus-specifc members as zeroes.
            //
            break;
        }

    } FINALLY {

    }

    return;
}

#include "BaseVid.h"
#include "eviction.h"
#include "pixel.h"
#include "BackBufferCompare.h"
#include "Image.h"
#include <Winbase.h>

/*
  This is where we define the component
*/
CTestEviction::CTestEviction()
{
    /*
      First, we describe the purpose of this component for the spec
    */
    SetDescription(L"This component tests eviction.")  ;
}

/*
  This method is called each time we are to run a test case
*/
void CTestEviction::RunTest()
{   
    CBackBufferCompare backBufferCompare(m_pLog);
    CI3DSurface2Ptr pBackBuffer;
    CI3DSurface2Ptr pRefBackBuffer;
    bool bSimilar=true;
    HRESULT hr = S_OK;

    /*
        Here we create a triangle and apply a texture to it.
    */
    CImage image(1, m_pLog);			// use this image to render before and after eviction
    CImage EvictionImage(m_pLog);       // use this image to generate enough textures to cause eviction of image's textures

    hr = image.CreateImage(m_pDevice);

    if(S_OK == hr)
    {
        pRefBackBuffer = image.RenderImage(m_pDevice);

        /*
            Set this back buffer as reference for comparison
        */
        backBufferCompare.SetReferenceBuffer(pRefBackBuffer);

        H_CHECK( m_pDevice->Present());
    }
    else if(E_OUTOFMEMORY == hr)
    {
        m_pLog->Fail(L"Could not create an image with the required number of textures");

        return;
    }
    else
    {
        m_pLog->Skip(L"Device does not support pixel shader.. Test has not been executed");

        return;
    }


    if(SUCCEEDED(hr))
    {
        //
        //	Generate new textures to cause evictions
        // 
        hr = EvictionImage.CreateImage(m_pDevice);

        if(hr == S_OK || hr == E_OUTOFMEMORY)
        {
            EvictionImage.RenderImage(m_pDevice, true);
            H_CHECK( m_pDevice->Present());
            hr = S_OK;
        }
        else
        {
            m_pLog->Fail(L"Failed to create EvictionImage");
        }
    }
    

    if(SUCCEEDED(hr))
    {
        /*
            Render an image again, this will bring evicted textures back into memory
        */
        pBackBuffer = image.RenderImage(m_pDevice);

        /*
            Compare the two images.
        */
        if(backBufferCompare.CompareBufferToReference(pBackBuffer) != 0)
            bSimilar = false;
        
        H_CHECK(m_pDevice->Present());

        if(bSimilar == false)
            m_pLog->Fail(L"The eviction is not correct");
    }
}


void CTestEviction::EndTest()
{
}

// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Int16Create
//
// Description:
//  TestCase to test if AudioFormatFl32Int16 class is registered and the necessary interfaces can be obtained from it
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Int16Create()
{
    CAPOFormatFl32Int16Test APOFormatTest;
    return APOFormatTest.APOTestCreate(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_APOFormatFl32Int16Process
//
// Description:
//  TestCase to test if APO AudioFormatFl32Int16 behaves as expected
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_APOFormatFl32Int16Process()
{    
    CAPOFormatFl32Int16Test APOFormatTest;
    return APOFormatTest.APOFormatProcess();
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_BaseFormat_SetDitherState
//
// Description:
//  TestCase to test IAudioDither::SetDitherState
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_BaseFormat_SetDitherState()
{
    CAPOFormatFl32Int16Test APOFormatTest;
    return APOFormatTest.APOTestSetDitherState(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: 
// Method: Tc_BaseFormat_GetDitherState
//
// Description:
//  TestCase to test IAudioDither::GetDitherState
//  This is a global testcase function used in rgTestCaseInfo structure
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD Tc_BaseFormat_GetDitherState()
{
    CAPOFormatFl32Int16Test APOFormatTest;
    return APOFormatTest.APOTestGetDitherState(__uuidof(AudioFormatConvert));
}

//-------------------------------------------------------------------------
// Class: CAPOFormatFl32Int16Test
// Method: APOFormatProcess
//
// Description:
//   Functional testcase for FormatFl32Int16 APO
// Parameters:
//
// Return values (DWORD):
//     Return FNS_PASS on Success, FNS_FAIL on Failure
//
DWORD CAPOFormatFl32Int16Test::APOFormatProcess()
{
    HRESULT hr = S_OK;
    DWORD dw = FNS_PASS;
    DWORD dwRet = FNS_FAIL;
    IAudioProcessingObjectRTPtr pAPORT = NULL;    
    IAudioProcessingObjectConfigurationPtr pAPOConfig = NULL;
    CComPtr<IAudioDither> pAPODither = NULL;
    IAudioMediaType* pInAudioMediaType = NULL;
    IAudioMediaType* pOutAudioMediaType = NULL;
    APO_CONNECTION_PROPERTY* pInputConn = NULL;
    APO_CONNECTION_PROPERTY* pOutputConn = NULL;
    FLOAT32 *pfSineWave = NULL;
    FLOAT32 *pfOutput = NULL;
    DITHER_STATE DitState = DITHER_NONE;

    SLOG(eInfo1, "Creating FormatFl32Int16 APO");
    hr = pAPORT.CreateInstance(__uuidof(AudioFormatConvert));
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: APORT Object creation from AudioFormatFl32Int16 FAILED", dw, FNS_FAIL);
    
    SLOG(eInfo1, "Querying for IAPOConfig from FormatFl32Int16");
    hr = pAPORT.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &pAPOConfig);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAPOConfig from IAPORT FAILED", dw, FNS_FAIL);

    SLOG(eInfo1, "Querying for IAudioDither from FormatFl32Int16");
    hr = pAPORT.QueryInterface(__uuidof(IAudioDither), &pAPODither);
    METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: QueryInterface for IAudioDither from IAPORT FAILED", dw, FNS_FAIL);
        
    //For each dither state, run the tests
    for(UINT ii = 0; ii < 3; ii++)
    {
        //Check how the boundary conditions are handled
        // i.e. does the dynamic range of -1.0 to 1.0 map the entire dynamic range for 16 bit from -32768 to 32767
        //Test input exteremes, 0.0 and Out of bounds
        // 3 frames of input data        
        FLOAT32 fInput[8];
        fInput[0] = (FLOAT32)1.0; //+ve max
        fInput[1] = (FLOAT32)0.0;
        fInput[2] = (FLOAT32)0.5;
        fInput[3] = (FLOAT32)-1.0; //-ve max
        fInput[4] = (FLOAT32)1.01; //+ve Clipping
        fInput[5] = (FLOAT32)-1.01; //-ve Clipping
        fInput[6] = (FLOAT32)3.06e-5; // 1
        fInput[7] = (FLOAT32)-3.06e-5; // 1

        //2 frames of Output data
        INT16 Output[8];
        ZeroMemory(Output, sizeof(INT16) * 8);

        //Setup the AudioFormat for our test
        AUDIO_FRAME_FORMAT AudioFloatFormat;
        AUDIO_FRAME_FORMAT AudioIntFormat;
        BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
        BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 2, 2, 16, 48000);

        //Setup input and output Connections
        SAFE_DELETE_POINTER(pInputConn);    
        pInputConn = new APO_CONNECTION_PROPERTY;
        SAFE_DELETE_POINTER(pOutputConn);
        pOutputConn = new APO_CONNECTION_PROPERTY;
        if((NULL == pInputConn) || (NULL == pOutputConn))
        {
            SLOG(eError, "ERROR: Memory Allocation Failure");            
            dw = FNS_FAIL;
            goto exit;
        }

        BUILD_CONNECTION(pInputConn, fInput, 4, 0, BUFFER_VALID);
        BUILD_CONNECTION(pOutputConn, Output, 4, 0, BUFFER_SILENT);
        //Get IAudioMediaType for the current format
        SAFE_RELEASE_POINTER(pInAudioMediaType);
        SAFE_RELEASE_POINTER(pOutAudioMediaType);
        CAudioMediaType::AF2AMT(AudioFloatFormat, &pInAudioMediaType);
        CAudioMediaType::AF2AMT(AudioIntFormat, &pOutAudioMediaType);

        DitState = (DITHER_STATE)ii;
        SLOG(eInfo1, "Setting DitherState to 0x%X", (UINT)DitState);
        hr = pAPODither->SetDitherState(DitState);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Unable to SetDitherState on FormatConverter", dw, FNS_FAIL);

        SLOG(eInfo1, "Locking APO for Processing");
        hr = LockAPO(pAPORT, 1, pInAudioMediaType, 1, pOutAudioMediaType);
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: Lock for Process on the APO FAILED", dw, FNS_FAIL);

        //UINT uiControl = 0;
        //uiControl = _controlfp(0, 0); //get the floating point control word
        //_controlfp(_RC_NEAR, _MCW_RC ); //set the control word to round 
        SLOG(eInfo1, "Calling APO Process");
        pAPORT->APOProcess(1, &pInputConn, 1, &pOutputConn);
        //_controlfp(uiControl, 0xFFFFFFFF); //reset it back to the original control word
        SLOG(eInfo1, "UnLocking APO");
        hr = pAPOConfig->UnlockForProcess();
        METHOD_CHECKFAIL_SETVAL_EXIT(hr, "ERROR: UnLock for Process on the APO FAILED", dw, FNS_FAIL);

        //Check the output values
        if( Output[0] != INT16(32767) || Output[1] != INT16(0) || Output[2] != INT16(16384) || 
            Output[3] != INT16(-32768) || Output[4] != INT16(32767) || Output[5] != INT16(-32768) ||
            Output[6] != INT16(1) || Output[7] != INT16(-1)
            )
        {
            SLOG(eError, "ERROR: APO Process did not behave as expected");
            SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[0], Output[0]);
            SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[1], Output[1]);
            SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[2], Output[2]);
            SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[3], Output[3]);
            SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[4], Output[4]);
            SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[5], Output[5]);
            SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[6], Output[6]);
            SLOG(eError, "Input Float: %f        Output 16bit: %d", fInput[7], Output[7]);
            //dw = FNS_FAIL; //See bug 1348762
        }

        //Start with a FLOAT32 SineWave, Convert it to INT16 and reconvert it to FLOAT32
        // The output should be exactly equal to the input
        SAFE_FREE_POINTER(pfSineWave);
        SAFE_FREE_POINTER(pfOutput);
        pfSineWave = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);
        pfOutput = (FLOAT32*)LocalAlloc(LMEM_FIXED, sizeof(FLOAT32) * 480);        
        if((NULL == pfSineWave) || (NULL == pfOutput))
        {
            SLOG(eError, "ERROR: Memory Allocation Failure");            
            dw = FNS_FAIL;
            goto exit;
        }

        ZeroMemory(pfOutput, sizeof(FLOAT32) * 480);   
        BUILD_FORMAT(&AudioFloatFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000);
        BUILD_FORMAT(&AudioIntFormat, KSDATAFORMAT_SUBTYPE_PCM, 1, 2, 16, 48000);

        // This will create the necessary source data (1Khz sinewave) and push it through both the APO's and finally
        // collect in the Output buffer
        dwRet = APOTestProcess(pfSineWave, //Source data
            AudioFloatFormat, //Source format
            __uuidof(AudioFormatConvert), //Source CLSID
            pfOutput, //Destination buffer
            AudioIntFormat, //Intermediate Format INT16
            __uuidof(AudioFormatConvert), //Intermediate CLSID
            KSDATAFORMAT_SUBTYPE_IEEE_FLOAT //Source Data type
            );

        if(FNS_PASS != dwRet)
        {
            SLOG(eError, "ERROR: APOTestProcess FAILED, 0x%X", dw);            
            dw = FNS_FAIL;
            goto exit;
        }
        //At this point the sinewave has gone through a complete roundtrip from FLOAT32 to INT16 to FLOAT32
        //Verify if the Source and the final destination are the same
        for (UINT32 i = 0; i < 480; i++)
        {
            //if(! CCheck::InRange(pfOutput[i], pfSineWave[i],(FLOAT32)(0.5/32767)))
            if(! CCheck::InRange(pfOutput[i], pfSineWave[i],(FLOAT32)0.001))        
            {
                SLOG(eError, "ERROR: APO Process did not behave as expected");
                SLOG(eError, "Source Value%f            Dest Value%f,    Frame no: %d", pfSineWave[i], pfOutput[i], i);
                dw = FNS_FAIL;
            }
        }
    }//for((UINT)(DITHER_STATE)DitState = DITHER_NONE; DitState <= DITHER_TRIHOP_NOISE; DitState++)

exit:
    SAFE_FREE_POINTER(pfSineWave);
    SAFE_FREE_POINTER(pfOutput);
    SAFE_DELETE_POINTER(pInputConn);
    SAFE_DELETE_POINTER(pOutputConn);
    SAFE_RELEASE_POINTER(pInAudioMediaType);
    SAFE_RELEASE_POINTER(pOutAudioMediaType);
    return dw;
}
#include "include.fx"

Technique 
< 
	String	Name = "ps_1_1 texcoord : Level = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                texcoord t0
                mov r0,t0
        };
         
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_1_1 texcoord : Level = 1";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                texcoord t0
                mov r0,t0
        };
         
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcoord : Level = 2";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
		ps_1_1

	        texcoord t0
                mov r0,t0
        };
         
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcoord : Level = 3";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                texcoord t0
                mov r0,t0
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcoord : Level = 4";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
	MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                texcoord t0
                mov r0,t0
        };
         
		VertexShader = <TexTLVS>;
   }
}

//***************************************************************************************

Technique 
< 
	String	Name = "ps_1_1 texcrd : Level = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_4
                
                def 	c0,	0.0,	0.0,	0.0,	1.0
                
                texcrd r1.rgb, t0.xyz
                
                mov     r1.a, c0.a
                mov     r0, r1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcrd : Level = 1";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_4
                
                def 	c0,	0.0,	0.0,	0.0,	1.0
                
                texcrd r1.rgb, t0.xyz
                
                mov     r1.a, c0.a
                mov     r0, r1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcrd : Level = 2";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_4
                
                def 	c0,	0.0,	0.0,	0.0,	1.0
                
                texcrd r1.rgb, t0.xyz
                
                mov     r1.a, c0.a
                mov     r0, r1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcrd : Level = 3";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_4
                
                def 	c0,	0.0,	0.0,	0.0,	1.0
                
                texcrd r1.rgb, t0.xyz
                
                mov     r1.a, c0.a
                mov     r0, r1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcrd : Level = 4";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_4
                
                def 	c0,	0.0,	0.0,	0.0,	1.0
                
                texcrd r1.rgb, t0.xyz
                
                mov     r1.a, c0.a
                mov     r0, r1
        };
         
	VertexShader = <TexTLVS>;
   }
}
#include "TexturesPCH.h"

#include <d3d9.h>
#include "CTextures.h"


// --------------------------------------------------------------------------------------

bool CVolTexture::Init9(PVOID pTex)
{
    LPDIRECT3DVOLUMETEXTURE9 pTexture = (LPDIRECT3DVOLUMETEXTURE9)pTex;

	if(pTexture == NULL)
	{
		DPF(1, _T("CVolTexture::Init9() - invalid params.\n"));
		return false;
	}

	//This is important as Dx will accept 0 and create all levels.
	m_dwLevelCount = pTexture->GetLevelCount();
	
	m_pTexture9 = pTexture;
	m_pTexture9->AddRef();

	return true;
}

// --------------------------------------------------------------------------------------

void CVolTexture::ReleaseD3D9()
{
	RELEASE(m_pTexture9);	
	RELEASE(m_pSysTexture);
}

// --------------------------------------------------------------------------------------

HRESULT CVolTexture::AddDirtyBox9(CONST BOX* pDirtyBox)
{
	if( 0L == m_pTexture9 )
		return E_FAIL;

	D3DBOX d3dBox;
	memset( &d3dBox, 0, sizeof(d3dBox) );

    if ( ValidPointer( pDirtyBox ) )
    {
	    d3dBox.Left   = pDirtyBox->Left;
	    d3dBox.Top    = pDirtyBox->Top;
	    d3dBox.Right  = pDirtyBox->Right;
	    d3dBox.Bottom = pDirtyBox->Bottom;
	    d3dBox.Front  = pDirtyBox->Front;
	    d3dBox.Back   = pDirtyBox->Back;
    }

	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->AddDirtyBox( PassPointer( &d3dBox, pDirtyBox ) );
}

// --------------------------------------------------------------------------------------

DWORD CVolTexture::SetLOD9(DWORD dwLODNew)
{
	if (0L == m_pTexture9)
		return 0;
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->SetLOD(dwLODNew);
}

// --------------------------------------------------------------------------------------


DWORD CVolTexture::GetLOD9()
{
	if (0L == m_pTexture9)
		return 0;
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->GetLOD();
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::SetAutoGenFilterType9(DWORD FilterType)
{
	if (NULL == m_pTexture9)
		return E_FAIL;
	return m_pTexture9->SetAutoGenFilterType((D3DTEXTUREFILTERTYPE)FilterType);
}

// -------------------------------------------------------------------------------------

DWORD CVolTexture::GetAutoGenFilterType9(void)
{
	if (NULL == m_pTexture9)
		return 0;
	return m_pTexture9->GetAutoGenFilterType();
}

// -------------------------------------------------------------------------------------

void CVolTexture::GenerateMipSubLevels9(void)
{
	if (NULL == m_pTexture9)
		return;
	m_pTexture9->GenerateMipSubLevels();
}

// --------------------------------------------------------------------------------------

DWORD CVolTexture::SetPriority9(DWORD PriorityNew)
{
	if (0L == m_pTexture9)
		return 0;
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->SetPriority(PriorityNew);
}

// --------------------------------------------------------------------------------------

DWORD CVolTexture::GetPriority9()
{
	if (0L == m_pTexture9)
		return 0;
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->GetPriority();
}

// --------------------------------------------------------------------------------------

void CVolTexture::PreLoad9()
{
	if (0L == m_pTexture9)
		return;
	((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->PreLoad();
}

// --------------------------------------------------------------------------------------

HRESULT CVolTexture::GetLevelDesc9(UINT Level, VOLUME_DESC *pDesc)
{
	if (0L == m_pTexture9)
		return E_FAIL;

	D3DVOLUME_DESC d3dVolDesc;
	memset( &d3dVolDesc, 0, sizeof( d3dVolDesc ) );

	HRESULT hr = ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->GetLevelDesc(Level, PassPointer(&d3dVolDesc, pDesc));
	if( FAILED(hr) )
		return hr;

    if (ValidPointer(pDesc))
    {
	    pDesc->Format.d3dfFormat = d3dVolDesc.Format;
	    pDesc->Size = 0;
	    pDesc->Width = d3dVolDesc.Width;
	    pDesc->Height = d3dVolDesc.Height;
	    pDesc->Depth = d3dVolDesc.Depth;
	    pDesc->Usage = d3dVolDesc.Usage;	// SDK Docs "Not currently used, always returned as 0."
	    d3df2ddpf(d3dVolDesc.Format, &(pDesc->Format.ddpfPixelFormat));

	    switch( d3dVolDesc.Type )
	    {
	    case D3DRTYPE_SURFACE:
		    pDesc->Type = RTYPE_SURFACE;
		    break;
	    case D3DRTYPE_VOLUME:
		    pDesc->Type = RTYPE_VOLUME;			// the only valid case hopefully
		    break;
	    case D3DRTYPE_TEXTURE:
		    pDesc->Type = RTYPE_TEXTURE;
		    break;
	    case D3DRTYPE_VOLUMETEXTURE:
		    pDesc->Type = RTYPE_VOLUMETEXTURE;
		    break;
	    case D3DRTYPE_CUBETEXTURE:
		    pDesc->Type = RTYPE_CUBETEXTURE;
		    break;
	    case D3DRTYPE_VERTEXBUFFER:
		    pDesc->Type = RTYPE_VERTEXBUFFER;
		    break;
	    case D3DRTYPE_INDEXBUFFER:
		    pDesc->Type = RTYPE_INDEXBUFFER;
		    break;
	    default:
		    pDesc->Type = (RESOURCETYPE)0;
	    }
	    
	    switch( d3dVolDesc.Pool )
	    {
	    case D3DPOOL_DEFAULT:
		    pDesc->Pool = POOL_DEFAULT;
		    break;
	    case D3DPOOL_MANAGED:
		    pDesc->Pool = POOL_MANAGED;
		    break;
		case (D3DPOOL)6:
			pDesc->Pool = POOL_MANAGED_INTERNAL;
	    case D3DPOOL_SYSTEMMEM:
		    pDesc->Pool = POOL_SYSTEMMEM;
		    break;
	    default:
		    pDesc->Pool = (DWORD)0;
	    }
    }

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVolTexture::GetVolumeLevel9(UINT Level, LPVOLUME* ppVolumeLevel)
{
	HRESULT hr;
    LPDIRECT3DVOLUME9 pd3dVolume = NULL;
	LPVOLUME          pVolume = NULL;

	if (NULL == m_pTexture9)
		return E_FAIL;

    hr = ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->GetVolumeLevel(Level, PassPointer(&pd3dVolume, ppVolumeLevel) );
	if( FAILED(hr) )
		return hr;

    if (ValidPointer(ppVolumeLevel))
    {
	    pVolume = new CVolume;
	    if(pVolume == NULL || !pVolume->Init(pd3dVolume, m_pDevice, NULL))
	    {
		    hr = E_FAIL;
		    goto Exit;
	    }

	    *ppVolumeLevel = pVolume;
    }

Exit:
	
	if(FAILED(hr))
		RELEASE(pVolume)

	RELEASE(pd3dVolume);
	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVolTexture::LockBox9(UINT Level, LOCKED_BOX* pLockedVolume, CONST BOX* pBox, DWORD Flags)
{
	if (0L == m_pTexture9)
		return E_FAIL;

	D3DBOX d3dBox;
	memset(&d3dBox,0,sizeof(d3dBox));
	D3DLOCKED_BOX d3dLockedVolume;
	memset(&d3dLockedVolume, 0, sizeof(d3dLockedVolume));
	HRESULT hr;

    if ( ValidPointer( pBox ) )
    {
		d3dBox.Left   = pBox->Left;
		d3dBox.Top    = pBox->Top;
		d3dBox.Right  = pBox->Right;
		d3dBox.Bottom = pBox->Bottom;
		d3dBox.Front  = pBox->Front;
		d3dBox.Back   = pBox->Back;
    }

	hr = ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->LockBox(Level, 
                                                          PassPointer(&d3dLockedVolume, pLockedVolume), 
                                                          PassPointer(&d3dBox, pBox),
                                                          Flags);

	if( FAILED(hr) )
		return hr;

	pLockedVolume->RowPitch   = d3dLockedVolume.RowPitch;
	pLockedVolume->SlicePitch = d3dLockedVolume.SlicePitch;
	pLockedVolume->pBits      = d3dLockedVolume.pBits;

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CVolTexture::UnlockBox9(UINT Level)
{
	if (0L == m_pTexture9)
		return E_FAIL;
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->UnlockBox(Level);
}

// --------------------------------------------------------------------------------------

HRESULT CVolTexture::SetPalette9(PALETTEENTRY *pPalette)
{
	HRESULT hr = 0;
	PALETTEENTRY *pPal = pPalette == NULL ? m_pDevice->m_Palette8 : pPalette;

	switch(m_Format.d3dfFormat)
	{
	case D3DFMT_A8P8:
    case D3DFMT_P8:
        hr = m_pDevice->SetPaletteEntries(0, pPal);
		break;
	default:
		DPF(1, _T("CVolTexture::SetPalette9() - Format is not palettized.\n"));
		return E_FAIL;
	}

	if(FAILED(hr))
	{
		return hr;
	}

	hr = m_pDevice->SetCurrentTexturePalette(0);
    if(FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::GetDevice9(CDevice** ppDevice)
{
	IDirect3DDevice9 *pDev = NULL;
	HRESULT hr;

    hr = ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->GetDevice( PassPointer( &pDev, ppDevice) );

	if (SUCCEEDED(hr))
	{
		if (pDev != m_pDevice->GetDevicePointer9())
		{
			ppDevice = NULL;
		}

		if (ppDevice)
		{
			*ppDevice = m_pDevice; 
			m_pDevice->AddRef();
		}
		RELEASE(pDev);
		return S_OK; 
	}
	else
	{
		ppDevice = NULL;
		RELEASE(pDev);
		return hr;
	}
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::FreePrivateData9(REFGUID refguid)
{
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->FreePrivateData(refguid);
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::GetPrivateData9(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->GetPrivateData(refguid, pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CVolTexture::SetPrivateData9(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags)
{
	return ((LPDIRECT3DVOLUMETEXTURE9)m_pTexture9)->SetPrivateData(refguid, pData, dwSizeOfData, dwFlags);
}
#include "Test_Include.fx"

int Test_Count = 444;

string TestInfo
<
	string TestType = "PS";
>
= "ps_3_sw_inst_m3x3";

PS_CRITERIA PS_001_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_3_sw : m3x3 source reg combination v0, c0 is allowed";
string PS_001 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"m3x3 r0.rgb, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_3_sw : m3x3 source reg combination c0, c1 is allowed";
string PS_002 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_003_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_3_sw : m3x3 source reg combination -c0, c1 is allowed";
string PS_003 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, -c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_004_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_3_sw : m3x3 source reg combination c0_abs, c1 is allowed";
string PS_004 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0_abs, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_005_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_3_sw : m3x3 source reg combination -c0_abs, c1 is allowed";
string PS_005 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, -c0_abs, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_006_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_3_sw : m3x3 source reg combination c0, c1 is allowed";
string PS_006 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_007_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_3_sw : m3x3 source reg combination c0.r, c1 is allowed";
string PS_007 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.r, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_008_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_3_sw : m3x3 source reg combination c0.g, c1 is allowed";
string PS_008 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.g, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_009_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_3_sw : m3x3 source reg combination c0.b, c1 is allowed";
string PS_009 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.b, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_010_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_3_sw : m3x3 source reg combination c0.a, c1 is allowed";
string PS_010 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.a, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_011_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_3_sw : m3x3 source reg combination c0.gbra, c1 is allowed";
string PS_011 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.gbra, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_012_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_3_sw : m3x3 source reg combination c0.brga, c1 is allowed";
string PS_012 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.brga, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_013_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_3_sw : m3x3 source reg combination c0.abgr, c1 is allowed";
string PS_013 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.abgr, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_014_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_3_sw : m3x3 source reg combination c0.agrb, c1 is allowed";
string PS_014 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.agrb, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_015_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_3_sw : m3x3 source reg combination c0.rbga, c1 is allowed";
string PS_015 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.rbga, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_016_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_3_sw : m3x3 source reg combination c0.rgab, c1 is allowed";
string PS_016 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.rgab, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_017_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_3_sw : m3x3 source reg combination c0.bgr, c1 is allowed";
string PS_017 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.bgr, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_018_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_3_sw : m3x3 source reg combination c0.rbg, c1 is allowed";
string PS_018 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.rbg, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_019_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_3_sw : m3x3 source reg combination c0.gar, c1 is allowed";
string PS_019 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.gar, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_020_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_3_sw : m3x3 source reg combination c0.gr, c1 is allowed";
string PS_020 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.gr, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_021_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_3_sw : m3x3 source reg combination c0.ab, c1 is allowed";
string PS_021 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.ab, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_022_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_3_sw : m3x3 source reg combination c0.bg, c1 is allowed";
string PS_022 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0.bg, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_023_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_3_sw : m3x3 source reg combination r0, v0 is NOT allowed";
string PS_023 = 
	"ps_3_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"m3x3 r0.rgb, r0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_024_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_3_sw : m3x3 source reg combination r0, r0 is NOT allowed";
string PS_024 = 
	"ps_3_sw "
	"mov r0, c0 "
	"m3x3 r0.rgb, r0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_025_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_3_sw : m3x3 source reg combination r0, b0 is NOT allowed";
string PS_025 = 
	"ps_3_sw "
	"defb b0, true "
	"mov r0, c0 "
	"m3x3 r0.rgb, r0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_026_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_3_sw : m3x3 source reg combination r0, i0 is NOT allowed";
string PS_026 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"m3x3 r0.rgb, r0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_027_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_3_sw : m3x3 source reg combination r0, aL is NOT allowed";
string PS_027 = 
	"ps_3_sw "
	"mov r0, c0 "
	"m3x3 r0.rgb, r0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_028_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_3_sw : m3x3 source reg combination r0, p0 is NOT allowed";
string PS_028 = 
	"ps_3_sw "
	"mov r0, c0 "
	"m3x3 r0.rgb, r0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_029_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_3_sw : m3x3 source reg combination r0, s0 is NOT allowed";
string PS_029 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"mov r0, c0 "
	"m3x3 r0.rgb, r0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_3_sw : m3x3 source reg combination r0, vFace is NOT allowed";
string PS_030 = 
	"ps_3_sw "
	"mov r0, c0 "
	"m3x3 r0.rgb, r0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_3_sw : m3x3 source reg combination r0, vPos is NOT allowed";
string PS_031 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"mov r0, c0 "
	"m3x3 r0.rgb, r0, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_3_sw : m3x3 source reg combination r0, oC0 is NOT allowed";
string PS_032 = 
	"ps_3_sw "
	"mov r0, c0 "
	"m3x3 r0.rgb, r0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_3_sw : m3x3 source reg combination r0, oDepth is NOT allowed";
string PS_033 = 
	"ps_3_sw "
	"mov r0, c0 "
	"m3x3 r0.rgb, r0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_3_sw : m3x3 source reg combination b0, v0 is NOT allowed";
string PS_034 = 
	"ps_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"m3x3 r0.rgb, b0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_3_sw : m3x3 source reg combination b0, r0 is NOT allowed";
string PS_035 = 
	"ps_3_sw "
	"defb b0, true "
	"mov r0, c0 "
	"m3x3 r0.rgb, b0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_3_sw : m3x3 source reg combination b0, b0 is NOT allowed";
string PS_036 = 
	"ps_3_sw "
	"defb b0, true "
	"m3x3 r0.rgb, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_3_sw : m3x3 source reg combination b0, i0 is NOT allowed";
string PS_037 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"m3x3 r0.rgb, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_3_sw : m3x3 source reg combination b0, aL is NOT allowed";
string PS_038 = 
	"ps_3_sw "
	"defb b0, true "
	"m3x3 r0.rgb, b0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_3_sw : m3x3 source reg combination b0, p0 is NOT allowed";
string PS_039 = 
	"ps_3_sw "
	"defb b0, true "
	"m3x3 r0.rgb, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_3_sw : m3x3 source reg combination b0, s0 is NOT allowed";
string PS_040 = 
	"ps_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"m3x3 r0.rgb, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_3_sw : m3x3 source reg combination b0, vFace is NOT allowed";
string PS_041 = 
	"ps_3_sw "
	"defb b0, true "
	"m3x3 r0.rgb, b0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_3_sw : m3x3 source reg combination b0, vPos is NOT allowed";
string PS_042 = 
	"ps_3_sw "
	"defb b0, true "
	"dcl vPos.xy "
	"m3x3 r0.rgb, b0, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_3_sw : m3x3 source reg combination b0, oC0 is NOT allowed";
string PS_043 = 
	"ps_3_sw "
	"defb b0, true "
	"m3x3 r0.rgb, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_3_sw : m3x3 source reg combination b0, oDepth is NOT allowed";
string PS_044 = 
	"ps_3_sw "
	"defb b0, true "
	"m3x3 r0.rgb, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_3_sw : m3x3 source reg combination i0, v0 is NOT allowed";
string PS_045 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"m3x3 r0.rgb, i0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_3_sw : m3x3 source reg combination i0, r0 is NOT allowed";
string PS_046 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"m3x3 r0.rgb, i0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_3_sw : m3x3 source reg combination i0, b0 is NOT allowed";
string PS_047 = 
	"ps_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"m3x3 r0.rgb, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_048_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_3_sw : m3x3 source reg combination i0, i0 is NOT allowed";
string PS_048 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"m3x3 r0.rgb, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_049_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_3_sw : m3x3 source reg combination i0, aL is NOT allowed";
string PS_049 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"m3x3 r0.rgb, i0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_050_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_3_sw : m3x3 source reg combination i0, p0 is NOT allowed";
string PS_050 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"m3x3 r0.rgb, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_051_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_3_sw : m3x3 source reg combination i0, s0 is NOT allowed";
string PS_051 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"m3x3 r0.rgb, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_052_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_3_sw : m3x3 source reg combination i0, vFace is NOT allowed";
string PS_052 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"m3x3 r0.rgb, i0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_053_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_3_sw : m3x3 source reg combination i0, vPos is NOT allowed";
string PS_053 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl vPos.xy "
	"m3x3 r0.rgb, i0, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_054_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_3_sw : m3x3 source reg combination i0, oC0 is NOT allowed";
string PS_054 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"m3x3 r0.rgb, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_055_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_3_sw : m3x3 source reg combination i0, oDepth is NOT allowed";
string PS_055 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"m3x3 r0.rgb, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_056_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_3_sw : m3x3 source reg combination aL, v0 is NOT allowed";
string PS_056 = 
	"ps_3_sw "
	"dcl_fog v0 "
	"m3x3 r0.rgb, aL, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_057_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_3_sw : m3x3 source reg combination aL, r0 is NOT allowed";
string PS_057 = 
	"ps_3_sw "
	"mov r0, c0 "
	"m3x3 r0.rgb, aL, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_058_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_3_sw : m3x3 source reg combination aL, b0 is NOT allowed";
string PS_058 = 
	"ps_3_sw "
	"defb b0, true "
	"m3x3 r0.rgb, aL, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_059_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_3_sw : m3x3 source reg combination aL, i0 is NOT allowed";
string PS_059 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"m3x3 r0.rgb, aL, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_060_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_3_sw : m3x3 source reg combination aL, aL is NOT allowed";
string PS_060 = 
	"ps_3_sw "
	"m3x3 r0.rgb, aL, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_061_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_3_sw : m3x3 source reg combination aL, p0 is NOT allowed";
string PS_061 = 
	"ps_3_sw "
	"m3x3 r0.rgb, aL, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_062_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_3_sw : m3x3 source reg combination aL, s0 is NOT allowed";
string PS_062 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"m3x3 r0.rgb, aL, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_063_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_3_sw : m3x3 source reg combination aL, vFace is NOT allowed";
string PS_063 = 
	"ps_3_sw "
	"m3x3 r0.rgb, aL, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_064_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_3_sw : m3x3 source reg combination aL, vPos is NOT allowed";
string PS_064 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"m3x3 r0.rgb, aL, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_065_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_3_sw : m3x3 source reg combination aL, oC0 is NOT allowed";
string PS_065 = 
	"ps_3_sw "
	"m3x3 r0.rgb, aL, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_066_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_3_sw : m3x3 source reg combination aL, oDepth is NOT allowed";
string PS_066 = 
	"ps_3_sw "
	"m3x3 r0.rgb, aL, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_067_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_3_sw : m3x3 source reg combination p0, v0 is NOT allowed";
string PS_067 = 
	"ps_3_sw "
	"dcl_fog v0 "
	"m3x3 r0.rgb, p0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_068_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_3_sw : m3x3 source reg combination p0, r0 is NOT allowed";
string PS_068 = 
	"ps_3_sw "
	"mov r0, c0 "
	"m3x3 r0.rgb, p0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_069_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_3_sw : m3x3 source reg combination p0, b0 is NOT allowed";
string PS_069 = 
	"ps_3_sw "
	"defb b0, true "
	"m3x3 r0.rgb, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_070_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_3_sw : m3x3 source reg combination p0, i0 is NOT allowed";
string PS_070 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"m3x3 r0.rgb, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_071_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_3_sw : m3x3 source reg combination p0, aL is NOT allowed";
string PS_071 = 
	"ps_3_sw "
	"m3x3 r0.rgb, p0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_072_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_3_sw : m3x3 source reg combination p0, p0 is NOT allowed";
string PS_072 = 
	"ps_3_sw "
	"m3x3 r0.rgb, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_073_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_3_sw : m3x3 source reg combination p0, s0 is NOT allowed";
string PS_073 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"m3x3 r0.rgb, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_074_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_3_sw : m3x3 source reg combination p0, vFace is NOT allowed";
string PS_074 = 
	"ps_3_sw "
	"m3x3 r0.rgb, p0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_075_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_3_sw : m3x3 source reg combination p0, vPos is NOT allowed";
string PS_075 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"m3x3 r0.rgb, p0, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_076_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_3_sw : m3x3 source reg combination p0, oC0 is NOT allowed";
string PS_076 = 
	"ps_3_sw "
	"m3x3 r0.rgb, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_077_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_3_sw : m3x3 source reg combination p0, oDepth is NOT allowed";
string PS_077 = 
	"ps_3_sw "
	"m3x3 r0.rgb, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_078_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_3_sw : m3x3 source reg combination s0, v0 is NOT allowed";
string PS_078 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"dcl_fog v0 "
	"m3x3 r0.rgb, s0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_079_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_3_sw : m3x3 source reg combination s0, r0 is NOT allowed";
string PS_079 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"mov r0, c0 "
	"m3x3 r0.rgb, s0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_080_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_3_sw : m3x3 source reg combination s0, b0 is NOT allowed";
string PS_080 = 
	"ps_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"m3x3 r0.rgb, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_081_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_3_sw : m3x3 source reg combination s0, i0 is NOT allowed";
string PS_081 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"m3x3 r0.rgb, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_082_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_3_sw : m3x3 source reg combination s0, aL is NOT allowed";
string PS_082 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"m3x3 r0.rgb, s0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_083_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_3_sw : m3x3 source reg combination s0, p0 is NOT allowed";
string PS_083 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"m3x3 r0.rgb, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_084_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_3_sw : m3x3 source reg combination s0, s0 is NOT allowed";
string PS_084 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"m3x3 r0.rgb, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_085_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_3_sw : m3x3 source reg combination s0, vFace is NOT allowed";
string PS_085 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"m3x3 r0.rgb, s0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_086_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_3_sw : m3x3 source reg combination s0, vPos is NOT allowed";
string PS_086 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"dcl vPos.xy "
	"m3x3 r0.rgb, s0, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_087_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_3_sw : m3x3 source reg combination s0, oC0 is NOT allowed";
string PS_087 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"m3x3 r0.rgb, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_088_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_3_sw : m3x3 source reg combination s0, oDepth is NOT allowed";
string PS_088 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"m3x3 r0.rgb, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_089_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_3_sw : m3x3 source reg combination vFace, v0 is NOT allowed";
string PS_089 = 
	"ps_3_sw "
	"dcl_fog v0 "
	"m3x3 r0.rgb, vFace, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_090_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_3_sw : m3x3 source reg combination vFace, r0 is NOT allowed";
string PS_090 = 
	"ps_3_sw "
	"mov r0, c0 "
	"m3x3 r0.rgb, vFace, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_091_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_3_sw : m3x3 source reg combination vFace, b0 is NOT allowed";
string PS_091 = 
	"ps_3_sw "
	"defb b0, true "
	"m3x3 r0.rgb, vFace, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_092_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_3_sw : m3x3 source reg combination vFace, i0 is NOT allowed";
string PS_092 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"m3x3 r0.rgb, vFace, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_093_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_3_sw : m3x3 source reg combination vFace, aL is NOT allowed";
string PS_093 = 
	"ps_3_sw "
	"m3x3 r0.rgb, vFace, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_094_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_3_sw : m3x3 source reg combination vFace, p0 is NOT allowed";
string PS_094 = 
	"ps_3_sw "
	"m3x3 r0.rgb, vFace, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_095_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_3_sw : m3x3 source reg combination vFace, s0 is NOT allowed";
string PS_095 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"m3x3 r0.rgb, vFace, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_096_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_3_sw : m3x3 source reg combination vFace, vFace is NOT allowed";
string PS_096 = 
	"ps_3_sw "
	"m3x3 r0.rgb, vFace, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_097_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_3_sw : m3x3 source reg combination vFace, vPos is NOT allowed";
string PS_097 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"m3x3 r0.rgb, vFace, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_098_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_3_sw : m3x3 source reg combination vFace, oC0 is NOT allowed";
string PS_098 = 
	"ps_3_sw "
	"m3x3 r0.rgb, vFace, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_099_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_3_sw : m3x3 source reg combination vFace, oDepth is NOT allowed";
string PS_099 = 
	"ps_3_sw "
	"m3x3 r0.rgb, vFace, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_100_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_3_sw : m3x3 source reg combination vPos, v0 is NOT allowed";
string PS_100 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"dcl_fog v0 "
	"m3x3 r0.rgb, vPos, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_101_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_3_sw : m3x3 source reg combination vPos, r0 is NOT allowed";
string PS_101 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"mov r0, c0 "
	"m3x3 r0.rgb, vPos, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_102_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_3_sw : m3x3 source reg combination vPos, b0 is NOT allowed";
string PS_102 = 
	"ps_3_sw "
	"defb b0, true "
	"dcl vPos.xy "
	"m3x3 r0.rgb, vPos, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_103_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_3_sw : m3x3 source reg combination vPos, i0 is NOT allowed";
string PS_103 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl vPos.xy "
	"m3x3 r0.rgb, vPos, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_104_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_3_sw : m3x3 source reg combination vPos, aL is NOT allowed";
string PS_104 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"m3x3 r0.rgb, vPos, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_105_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_105_Desc = "ps_3_sw : m3x3 source reg combination vPos, p0 is NOT allowed";
string PS_105 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"m3x3 r0.rgb, vPos, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_106_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_106_Desc = "ps_3_sw : m3x3 source reg combination vPos, s0 is NOT allowed";
string PS_106 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"dcl_cube s0 "
	"m3x3 r0.rgb, vPos, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_107_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_107_Desc = "ps_3_sw : m3x3 source reg combination vPos, vFace is NOT allowed";
string PS_107 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"m3x3 r0.rgb, vPos, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_108_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_3_sw : m3x3 source reg combination vPos, vPos is NOT allowed";
string PS_108 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"m3x3 r0.rgb, vPos, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_109_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_3_sw : m3x3 source reg combination vPos, oC0 is NOT allowed";
string PS_109 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"m3x3 r0.rgb, vPos, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_110_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_3_sw : m3x3 source reg combination vPos, oDepth is NOT allowed";
string PS_110 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"m3x3 r0.rgb, vPos, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_111_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_3_sw : m3x3 source reg combination oC0, v0 is NOT allowed";
string PS_111 = 
	"ps_3_sw "
	"dcl_fog v0 "
	"m3x3 r0.rgb, oC0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_112_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_3_sw : m3x3 source reg combination oC0, r0 is NOT allowed";
string PS_112 = 
	"ps_3_sw "
	"mov r0, c0 "
	"m3x3 r0.rgb, oC0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_113_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_3_sw : m3x3 source reg combination oC0, b0 is NOT allowed";
string PS_113 = 
	"ps_3_sw "
	"defb b0, true "
	"m3x3 r0.rgb, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_114_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_3_sw : m3x3 source reg combination oC0, i0 is NOT allowed";
string PS_114 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"m3x3 r0.rgb, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_115_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_3_sw : m3x3 source reg combination oC0, aL is NOT allowed";
string PS_115 = 
	"ps_3_sw "
	"m3x3 r0.rgb, oC0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_116_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_116_Desc = "ps_3_sw : m3x3 source reg combination oC0, p0 is NOT allowed";
string PS_116 = 
	"ps_3_sw "
	"m3x3 r0.rgb, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_117_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_117_Desc = "ps_3_sw : m3x3 source reg combination oC0, s0 is NOT allowed";
string PS_117 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"m3x3 r0.rgb, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_118_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_118_Desc = "ps_3_sw : m3x3 source reg combination oC0, vFace is NOT allowed";
string PS_118 = 
	"ps_3_sw "
	"m3x3 r0.rgb, oC0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_119_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_119_Desc = "ps_3_sw : m3x3 source reg combination oC0, vPos is NOT allowed";
string PS_119 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"m3x3 r0.rgb, oC0, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_120_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_120_Desc = "ps_3_sw : m3x3 source reg combination oC0, oC0 is NOT allowed";
string PS_120 = 
	"ps_3_sw "
	"m3x3 r0.rgb, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_121_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_121_Desc = "ps_3_sw : m3x3 source reg combination oC0, oDepth is NOT allowed";
string PS_121 = 
	"ps_3_sw "
	"m3x3 r0.rgb, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_122_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_122_Desc = "ps_3_sw : m3x3 source reg combination oDepth, v0 is NOT allowed";
string PS_122 = 
	"ps_3_sw "
	"dcl_fog v0 "
	"m3x3 r0.rgb, oDepth, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_123_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_123_Desc = "ps_3_sw : m3x3 source reg combination oDepth, r0 is NOT allowed";
string PS_123 = 
	"ps_3_sw "
	"mov r0, c0 "
	"m3x3 r0.rgb, oDepth, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_124_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_124_Desc = "ps_3_sw : m3x3 source reg combination oDepth, b0 is NOT allowed";
string PS_124 = 
	"ps_3_sw "
	"defb b0, true "
	"m3x3 r0.rgb, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_125_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_125_Desc = "ps_3_sw : m3x3 source reg combination oDepth, i0 is NOT allowed";
string PS_125 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"m3x3 r0.rgb, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_126_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_126_Desc = "ps_3_sw : m3x3 source reg combination oDepth, aL is NOT allowed";
string PS_126 = 
	"ps_3_sw "
	"m3x3 r0.rgb, oDepth, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_127_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_127_Desc = "ps_3_sw : m3x3 source reg combination oDepth, p0 is NOT allowed";
string PS_127 = 
	"ps_3_sw "
	"m3x3 r0.rgb, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_128_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_128_Desc = "ps_3_sw : m3x3 source reg combination oDepth, s0 is NOT allowed";
string PS_128 = 
	"ps_3_sw "
	"dcl_cube s0 "
	"m3x3 r0.rgb, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_129_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_129_Desc = "ps_3_sw : m3x3 source reg combination oDepth, vFace is NOT allowed";
string PS_129 = 
	"ps_3_sw "
	"m3x3 r0.rgb, oDepth, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_130_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_130_Desc = "ps_3_sw : m3x3 source reg combination oDepth, vPos is NOT allowed";
string PS_130 = 
	"ps_3_sw "
	"dcl vPos.xy "
	"m3x3 r0.rgb, oDepth, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_131_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_131_Desc = "ps_3_sw : m3x3 source reg combination oDepth, oC0 is NOT allowed";
string PS_131 = 
	"ps_3_sw "
	"m3x3 r0.rgb, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_132_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_132_Desc = "ps_3_sw : m3x3 source reg combination oDepth, oDepth is NOT allowed";
string PS_132 = 
	"ps_3_sw "
	"m3x3 r0.rgb, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_133_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_133_Desc = "ps_3_sw : m3x3 source reg combination c0, -c1 is NOT allowed";
string PS_133 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, -c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_134_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_134_Desc = "ps_3_sw : m3x3 source reg combination c0, c1_abs is NOT allowed";
string PS_134 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_135_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_135_Desc = "ps_3_sw : m3x3 source reg combination c0, -c1_abs is NOT allowed";
string PS_135 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, -c1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_136_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_136_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.r is NOT allowed";
string PS_136 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.r "
	"mov oC0, c0 ";

PS_CRITERIA PS_137_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_137_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.g is NOT allowed";
string PS_137 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.g "
	"mov oC0, c0 ";

PS_CRITERIA PS_138_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_138_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.b is NOT allowed";
string PS_138 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.b "
	"mov oC0, c0 ";

PS_CRITERIA PS_139_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_139_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.a is NOT allowed";
string PS_139 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.a "
	"mov oC0, c0 ";

PS_CRITERIA PS_140_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_140_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.gbra is NOT allowed";
string PS_140 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.gbra "
	"mov oC0, c0 ";

PS_CRITERIA PS_141_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_141_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.brga is NOT allowed";
string PS_141 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.brga "
	"mov oC0, c0 ";

PS_CRITERIA PS_142_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_142_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.abgr is NOT allowed";
string PS_142 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.abgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_143_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_143_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.agrb is NOT allowed";
string PS_143 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.agrb "
	"mov oC0, c0 ";

PS_CRITERIA PS_144_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_144_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.rbga is NOT allowed";
string PS_144 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.rbga "
	"mov oC0, c0 ";

PS_CRITERIA PS_145_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_145_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.rgab is NOT allowed";
string PS_145 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.rgab "
	"mov oC0, c0 ";

PS_CRITERIA PS_146_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_146_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.bgr is NOT allowed";
string PS_146 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.bgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_147_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_147_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.rbg is NOT allowed";
string PS_147 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.rbg "
	"mov oC0, c0 ";

PS_CRITERIA PS_148_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_148_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.gar is NOT allowed";
string PS_148 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.gar "
	"mov oC0, c0 ";

PS_CRITERIA PS_149_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_149_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.gr is NOT allowed";
string PS_149 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.gr "
	"mov oC0, c0 ";

PS_CRITERIA PS_150_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_150_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.ab is NOT allowed";
string PS_150 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.ab "
	"mov oC0, c0 ";

PS_CRITERIA PS_151_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_151_Desc = "ps_3_sw : m3x3 source reg combination c0, c1.bg is NOT allowed";
string PS_151 = 
	"ps_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x3 r0.rgb, c0, c1.bg "
	"mov oC0, c0 ";

PS_CRITERIA PS_152_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_152_Desc = "ps_3_sw : m3x3 write mask .rgb is allowed";
string PS_152 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.rgb, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_153_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_153_Desc = "ps_3_sw : m3x3 write mask .r is NOT allowed";
string PS_153 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.r, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_154_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_154_Desc = "ps_3_sw : m3x3 write mask .g is NOT allowed";
string PS_154 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.g, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_155_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_155_Desc = "ps_3_sw : m3x3 write mask .b is NOT allowed";
string PS_155 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.b, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_156_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_156_Desc = "ps_3_sw : m3x3 write mask .a is NOT allowed";
string PS_156 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.a, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_157_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_157_Desc = "ps_3_sw : m3x3 write mask .rg is NOT allowed";
string PS_157 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.rg, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_158_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_158_Desc = "ps_3_sw : m3x3 write mask .gb is NOT allowed";
string PS_158 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.gb, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_159_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_159_Desc = "ps_3_sw : m3x3 write mask .ba is NOT allowed";
string PS_159 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.ba, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_160_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_160_Desc = "ps_3_sw : m3x3 write mask .rb is NOT allowed";
string PS_160 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.rb, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_161_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_161_Desc = "ps_3_sw : m3x3 write mask .ra is NOT allowed";
string PS_161 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.ra, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_162_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_162_Desc = "ps_3_sw : m3x3 write mask .ga is NOT allowed";
string PS_162 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.ga, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_163_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_163_Desc = "ps_3_sw : m3x3 write mask .gba is NOT allowed";
string PS_163 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.gba, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_164_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_164_Desc = "ps_3_sw : m3x3 write mask .rba is NOT allowed";
string PS_164 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.rba, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_165_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_165_Desc = "ps_3_sw : m3x3 write mask .rgba is NOT allowed";
string PS_165 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.rgba, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_166_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_166_Desc = "ps_3_sw : m3x3 write mask .yx is NOT allowed";
string PS_166 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.yx, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_167_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_167_Desc = "ps_3_sw : m3x3 write mask .zx is NOT allowed";
string PS_167 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.zx, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_168_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_168_Desc = "ps_3_sw : m3x3 write mask .zy is NOT allowed";
string PS_168 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.zy, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_169_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_169_Desc = "ps_3_sw : m3x3 write mask .wx is NOT allowed";
string PS_169 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.wx, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_170_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_170_Desc = "ps_3_sw : m3x3 write mask .wz is NOT allowed";
string PS_170 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.wz, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_171_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_171_Desc = "ps_3_sw : m3x3 write mask .wy is NOT allowed";
string PS_171 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.wy, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_172_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_172_Desc = "ps_3_sw : m3x3 write mask .zyx is NOT allowed";
string PS_172 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.zyx, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_173_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_173_Desc = "ps_3_sw : m3x3 write mask .wzy is NOT allowed";
string PS_173 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.wzy, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_174_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_174_Desc = "ps_3_sw : m3x3 write mask .wzx is NOT allowed";
string PS_174 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.wzx, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_175_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_175_Desc = "ps_3_sw : m3x3 write mask .wyx is NOT allowed";
string PS_175 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.wyx, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_176_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_176_Desc = "ps_3_sw : m3x3 write mask .yxw is NOT allowed";
string PS_176 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.yxw, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_177_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_177_Desc = "ps_3_sw : m3x3 write mask .wzyx is NOT allowed";
string PS_177 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.wzyx, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_178_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_178_Desc = "ps_3_sw : m3x3 write mask .zxwy is NOT allowed";
string PS_178 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x3 r0.zxwy, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_179_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_179_Desc = "ps_3_sw : m3x3 is allowed in a 1 level if b0 block";
string PS_179 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 "
	"	m3x3 r0.rgb, v0, c0 "
	"endif ";

PS_CRITERIA PS_180_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_180_Desc = "ps_3_sw : m3x3 is allowed in a 8 level if b0 block";
string PS_180 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								m3x3 r0.rgb, v0, c0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_181_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_181_Desc = "ps_3_sw : m3x3 is allowed in a 16 level if b0 block";
string PS_181 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																m3x3 r0.rgb, v0, c0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_182_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_182_Desc = "ps_3_sw : m3x3 is allowed in a 24 level if b0 block";
string PS_182 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								m3x3 r0.rgb, v0, c0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_183_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_183_Desc = "ps_3_sw : m3x3 is NOT allowed in a 25 level if b0 block";
string PS_183 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								if b0 "
	"																									m3x3 r0.rgb, v0, c0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_184_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_184_Desc = "ps_3_sw : m3x3 is allowed in a 1 level if b0 nop else block";
string PS_184 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 nop else "
	"	m3x3 r0.rgb, v0, c0 "
	"endif ";

PS_CRITERIA PS_185_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_185_Desc = "ps_3_sw : m3x3 is allowed in a 8 level if b0 nop else block";
string PS_185 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								m3x3 r0.rgb, v0, c0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_186_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_186_Desc = "ps_3_sw : m3x3 is allowed in a 16 level if b0 nop else block";
string PS_186 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																m3x3 r0.rgb, v0, c0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_187_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_187_Desc = "ps_3_sw : m3x3 is allowed in a 24 level if b0 nop else block";
string PS_187 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								m3x3 r0.rgb, v0, c0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_188_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_188_Desc = "ps_3_sw : m3x3 is NOT allowed in a 25 level if b0 nop else block";
string PS_188 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								if b0 nop else "
	"																									m3x3 r0.rgb, v0, c0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_189_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_189_Desc = "ps_3_sw : m3x3 is allowed in a 1 level loop aL, i0 block";
string PS_189 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	m3x3 r0.rgb, v0, c0 "
	"endloop ";

PS_CRITERIA PS_190_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_190_Desc = "ps_3_sw : m3x3 is allowed in a 2 level loop aL, i0 block";
string PS_190 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_191_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_191_Desc = "ps_3_sw : m3x3 is allowed in a 3 level loop aL, i0 block";
string PS_191 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			m3x3 r0.rgb, v0, c0 "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_192_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_192_Desc = "ps_3_sw : m3x3 is allowed in a 4 level loop aL, i0 block";
string PS_192 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				m3x3 r0.rgb, v0, c0 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_193_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_193_Desc = "ps_3_sw : m3x3 is NOT allowed in a 5 level loop aL, i0 block";
string PS_193 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					m3x3 r0.rgb, v0, c0 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_194_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_194_Desc = "ps_3_sw : m3x3 is allowed in a 1 level rep i0 block";
string PS_194 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 "
	"	m3x3 r0.rgb, v0, c0 "
	"endrep ";

PS_CRITERIA PS_195_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_195_Desc = "ps_3_sw : m3x3 is allowed in a 2 level rep i0 block";
string PS_195 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_196_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_196_Desc = "ps_3_sw : m3x3 is allowed in a 3 level rep i0 block";
string PS_196 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			m3x3 r0.rgb, v0, c0 "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_197_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_197_Desc = "ps_3_sw : m3x3 is allowed in a 4 level rep i0 block";
string PS_197 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				m3x3 r0.rgb, v0, c0 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_198_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_198_Desc = "ps_3_sw : m3x3 is NOT allowed in a 5 level rep i0 block";
string PS_198 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					m3x3 r0.rgb, v0, c0 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_199_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_199_Desc = "ps_3_sw : m3x3 is allowed in a 1 level if_ne c0.x, c0.y block";
string PS_199 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ne c0.x, c0.y "
	"	m3x3 r0.rgb, v0, c0 "
	"endif ";

PS_CRITERIA PS_200_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_200_Desc = "ps_3_sw : m3x3 is allowed in a 8 level if_ne c0.x, c0.y block";
string PS_200 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y "
	"		if_ne c0.x, c0.y "
	"			if_ne c0.x, c0.y "
	"				if_ne c0.x, c0.y "
	"					if_ne c0.x, c0.y "
	"						if_ne c0.x, c0.y "
	"							if_ne c0.x, c0.y "
	"								m3x3 r0.rgb, v0, c0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_201_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_201_Desc = "ps_3_sw : m3x3 is allowed in a 16 level if_ne c0.x, c0.y block";
string PS_201 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y "
	"		if_ne c0.x, c0.y "
	"			if_ne c0.x, c0.y "
	"				if_ne c0.x, c0.y "
	"					if_ne c0.x, c0.y "
	"						if_ne c0.x, c0.y "
	"							if_ne c0.x, c0.y "
	"								if_ne c0.x, c0.y "
	"									if_ne c0.x, c0.y "
	"										if_ne c0.x, c0.y "
	"											if_ne c0.x, c0.y "
	"												if_ne c0.x, c0.y "
	"													if_ne c0.x, c0.y "
	"														if_ne c0.x, c0.y "
	"															if_ne c0.x, c0.y "
	"																m3x3 r0.rgb, v0, c0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_202_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_202_Desc = "ps_3_sw : m3x3 is allowed in a 24 level if_ne c0.x, c0.y block";
string PS_202 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y "
	"		if_ne c0.x, c0.y "
	"			if_ne c0.x, c0.y "
	"				if_ne c0.x, c0.y "
	"					if_ne c0.x, c0.y "
	"						if_ne c0.x, c0.y "
	"							if_ne c0.x, c0.y "
	"								if_ne c0.x, c0.y "
	"									if_ne c0.x, c0.y "
	"										if_ne c0.x, c0.y "
	"											if_ne c0.x, c0.y "
	"												if_ne c0.x, c0.y "
	"													if_ne c0.x, c0.y "
	"														if_ne c0.x, c0.y "
	"															if_ne c0.x, c0.y "
	"																if_ne c0.x, c0.y "
	"																	if_ne c0.x, c0.y "
	"																		if_ne c0.x, c0.y "
	"																			if_ne c0.x, c0.y "
	"																				if_ne c0.x, c0.y "
	"																					if_ne c0.x, c0.y "
	"																						if_ne c0.x, c0.y "
	"																							if_ne c0.x, c0.y "
	"																								m3x3 r0.rgb, v0, c0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_203_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_203_Desc = "ps_3_sw : m3x3 is NOT allowed in a 25 level if_ne c0.x, c0.y block";
string PS_203 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y "
	"		if_ne c0.x, c0.y "
	"			if_ne c0.x, c0.y "
	"				if_ne c0.x, c0.y "
	"					if_ne c0.x, c0.y "
	"						if_ne c0.x, c0.y "
	"							if_ne c0.x, c0.y "
	"								if_ne c0.x, c0.y "
	"									if_ne c0.x, c0.y "
	"										if_ne c0.x, c0.y "
	"											if_ne c0.x, c0.y "
	"												if_ne c0.x, c0.y "
	"													if_ne c0.x, c0.y "
	"														if_ne c0.x, c0.y "
	"															if_ne c0.x, c0.y "
	"																if_ne c0.x, c0.y "
	"																	if_ne c0.x, c0.y "
	"																		if_ne c0.x, c0.y "
	"																			if_ne c0.x, c0.y "
	"																				if_ne c0.x, c0.y "
	"																					if_ne c0.x, c0.y "
	"																						if_ne c0.x, c0.y "
	"																							if_ne c0.x, c0.y "
	"																								if_ne c0.x, c0.y "
	"																									m3x3 r0.rgb, v0, c0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_204_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_204_Desc = "ps_3_sw : m3x3 is allowed in a 1 level if_ne c0.x, c0.y nop else block";
string PS_204 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ne c0.x, c0.y nop else "
	"	m3x3 r0.rgb, v0, c0 "
	"endif ";

PS_CRITERIA PS_205_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_205_Desc = "ps_3_sw : m3x3 is allowed in a 8 level if_ne c0.x, c0.y nop else block";
string PS_205 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ne c0.x, c0.y nop else "
	"	if_ne c0.x, c0.y nop else "
	"		if_ne c0.x, c0.y nop else "
	"			if_ne c0.x, c0.y nop else "
	"				if_ne c0.x, c0.y nop else "
	"					if_ne c0.x, c0.y nop else "
	"						if_ne c0.x, c0.y nop else "
	"							if_ne c0.x, c0.y nop else "
	"								m3x3 r0.rgb, v0, c0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_206_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_206_Desc = "ps_3_sw : m3x3 is allowed in a 16 level if_ne c0.x, c0.y nop else block";
string PS_206 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ne c0.x, c0.y nop else "
	"	if_ne c0.x, c0.y nop else "
	"		if_ne c0.x, c0.y nop else "
	"			if_ne c0.x, c0.y nop else "
	"				if_ne c0.x, c0.y nop else "
	"					if_ne c0.x, c0.y nop else "
	"						if_ne c0.x, c0.y nop else "
	"							if_ne c0.x, c0.y nop else "
	"								if_ne c0.x, c0.y nop else "
	"									if_ne c0.x, c0.y nop else "
	"										if_ne c0.x, c0.y nop else "
	"											if_ne c0.x, c0.y nop else "
	"												if_ne c0.x, c0.y nop else "
	"													if_ne c0.x, c0.y nop else "
	"														if_ne c0.x, c0.y nop else "
	"															if_ne c0.x, c0.y nop else "
	"																m3x3 r0.rgb, v0, c0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_207_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_207_Desc = "ps_3_sw : m3x3 is allowed in a 24 level if_ne c0.x, c0.y nop else block";
string PS_207 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ne c0.x, c0.y nop else "
	"	if_ne c0.x, c0.y nop else "
	"		if_ne c0.x, c0.y nop else "
	"			if_ne c0.x, c0.y nop else "
	"				if_ne c0.x, c0.y nop else "
	"					if_ne c0.x, c0.y nop else "
	"						if_ne c0.x, c0.y nop else "
	"							if_ne c0.x, c0.y nop else "
	"								if_ne c0.x, c0.y nop else "
	"									if_ne c0.x, c0.y nop else "
	"										if_ne c0.x, c0.y nop else "
	"											if_ne c0.x, c0.y nop else "
	"												if_ne c0.x, c0.y nop else "
	"													if_ne c0.x, c0.y nop else "
	"														if_ne c0.x, c0.y nop else "
	"															if_ne c0.x, c0.y nop else "
	"																if_ne c0.x, c0.y nop else "
	"																	if_ne c0.x, c0.y nop else "
	"																		if_ne c0.x, c0.y nop else "
	"																			if_ne c0.x, c0.y nop else "
	"																				if_ne c0.x, c0.y nop else "
	"																					if_ne c0.x, c0.y nop else "
	"																						if_ne c0.x, c0.y nop else "
	"																							if_ne c0.x, c0.y nop else "
	"																								m3x3 r0.rgb, v0, c0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_208_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_208_Desc = "ps_3_sw : m3x3 is NOT allowed in a 25 level if_ne c0.x, c0.y nop else block";
string PS_208 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ne c0.x, c0.y nop else "
	"	if_ne c0.x, c0.y nop else "
	"		if_ne c0.x, c0.y nop else "
	"			if_ne c0.x, c0.y nop else "
	"				if_ne c0.x, c0.y nop else "
	"					if_ne c0.x, c0.y nop else "
	"						if_ne c0.x, c0.y nop else "
	"							if_ne c0.x, c0.y nop else "
	"								if_ne c0.x, c0.y nop else "
	"									if_ne c0.x, c0.y nop else "
	"										if_ne c0.x, c0.y nop else "
	"											if_ne c0.x, c0.y nop else "
	"												if_ne c0.x, c0.y nop else "
	"													if_ne c0.x, c0.y nop else "
	"														if_ne c0.x, c0.y nop else "
	"															if_ne c0.x, c0.y nop else "
	"																if_ne c0.x, c0.y nop else "
	"																	if_ne c0.x, c0.y nop else "
	"																		if_ne c0.x, c0.y nop else "
	"																			if_ne c0.x, c0.y nop else "
	"																				if_ne c0.x, c0.y nop else "
	"																					if_ne c0.x, c0.y nop else "
	"																						if_ne c0.x, c0.y nop else "
	"																							if_ne c0.x, c0.y nop else "
	"																								if_ne c0.x, c0.y nop else "
	"																									m3x3 r0.rgb, v0, c0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_209_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_209_Desc = "ps_3_sw : m3x3 is allowed in a 1 level if p0.x block";
string PS_209 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	m3x3 r0.rgb, v0, c0 "
	"endif ";

PS_CRITERIA PS_210_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_210_Desc = "ps_3_sw : m3x3 is allowed in a 8 level if p0.x block";
string PS_210 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								m3x3 r0.rgb, v0, c0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_211_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_211_Desc = "ps_3_sw : m3x3 is allowed in a 16 level if p0.x block";
string PS_211 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																m3x3 r0.rgb, v0, c0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_212_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_212_Desc = "ps_3_sw : m3x3 is allowed in a 24 level if p0.x block";
string PS_212 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								m3x3 r0.rgb, v0, c0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_213_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_213_Desc = "ps_3_sw : m3x3 is NOT allowed in a 25 level if p0.x block";
string PS_213 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								if p0.x "
	"																									m3x3 r0.rgb, v0, c0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_214_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_214_Desc = "ps_3_sw : m3x3 is allowed in a 1 level if p0.x nop else block";
string PS_214 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x nop else "
	"	m3x3 r0.rgb, v0, c0 "
	"endif ";

PS_CRITERIA PS_215_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_215_Desc = "ps_3_sw : m3x3 is allowed in a 8 level if p0.x nop else block";
string PS_215 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								m3x3 r0.rgb, v0, c0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_216_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_216_Desc = "ps_3_sw : m3x3 is allowed in a 16 level if p0.x nop else block";
string PS_216 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																m3x3 r0.rgb, v0, c0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_217_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_217_Desc = "ps_3_sw : m3x3 is allowed in a 24 level if p0.x nop else block";
string PS_217 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								m3x3 r0.rgb, v0, c0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_218_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_218_Desc = "ps_3_sw : m3x3 is NOT allowed in a 25 level if p0.x nop else block";
string PS_218 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								if p0.x nop else "
	"																									m3x3 r0.rgb, v0, c0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_219_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_219_Desc = "ps_3_sw : m3x3 is allowed in a 1 level loop aL, i0 break block";
string PS_219 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	m3x3 r0.rgb, v0, c0 "
	"endloop ";

PS_CRITERIA PS_220_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_220_Desc = "ps_3_sw : m3x3 is NOT allowed in a 8 level loop aL, i0 break block";
string PS_220 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								m3x3 r0.rgb, v0, c0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_221_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_221_Desc = "ps_3_sw : m3x3 is NOT allowed in a 16 level loop aL, i0 break block";
string PS_221 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																m3x3 r0.rgb, v0, c0 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_222_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_222_Desc = "ps_3_sw : m3x3 is NOT allowed in a 24 level loop aL, i0 break block";
string PS_222 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								m3x3 r0.rgb, v0, c0 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_223_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_223_Desc = "ps_3_sw : m3x3 is NOT allowed in a 25 level loop aL, i0 break block";
string PS_223 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								loop aL, i0 break "
	"																									m3x3 r0.rgb, v0, c0 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_224_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_224_Desc = "ps_3_sw : m3x3 is allowed in a 1 level rep i0 break block";
string PS_224 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 break "
	"	m3x3 r0.rgb, v0, c0 "
	"endrep ";

PS_CRITERIA PS_225_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_225_Desc = "ps_3_sw : m3x3 is NOT allowed in a 8 level rep i0 break block";
string PS_225 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								m3x3 r0.rgb, v0, c0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_226_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_226_Desc = "ps_3_sw : m3x3 is NOT allowed in a 16 level rep i0 break block";
string PS_226 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																m3x3 r0.rgb, v0, c0 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_227_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_227_Desc = "ps_3_sw : m3x3 is NOT allowed in a 24 level rep i0 break block";
string PS_227 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								m3x3 r0.rgb, v0, c0 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_228_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_228_Desc = "ps_3_sw : m3x3 is NOT allowed in a 25 level rep i0 break block";
string PS_228 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								rep i0 break "
	"																									m3x3 r0.rgb, v0, c0 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_229_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_229_Desc = "ps_3_sw : m3x3 is allowed in a 1 level loop aL, i0 breakp p0.x block";
string PS_229 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 breakp p0.x "
	"	m3x3 r0.rgb, v0, c0 "
	"endloop ";

PS_CRITERIA PS_230_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_230_Desc = "ps_3_sw : m3x3 is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string PS_230 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								m3x3 r0.rgb, v0, c0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_231_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_231_Desc = "ps_3_sw : m3x3 is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string PS_231 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																m3x3 r0.rgb, v0, c0 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_232_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_232_Desc = "ps_3_sw : m3x3 is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string PS_232 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								m3x3 r0.rgb, v0, c0 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_233_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_233_Desc = "ps_3_sw : m3x3 is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string PS_233 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								loop aL, i0 breakp p0.x "
	"																									m3x3 r0.rgb, v0, c0 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_234_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_234_Desc = "ps_3_sw : m3x3 is allowed in a 1 level rep i0 breakp p0.x block";
string PS_234 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	m3x3 r0.rgb, v0, c0 "
	"endrep ";

PS_CRITERIA PS_235_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_235_Desc = "ps_3_sw : m3x3 is NOT allowed in a 8 level rep i0 breakp p0.x block";
string PS_235 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								m3x3 r0.rgb, v0, c0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_236_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_236_Desc = "ps_3_sw : m3x3 is NOT allowed in a 16 level rep i0 breakp p0.x block";
string PS_236 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																m3x3 r0.rgb, v0, c0 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_237_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_237_Desc = "ps_3_sw : m3x3 is NOT allowed in a 24 level rep i0 breakp p0.x block";
string PS_237 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								m3x3 r0.rgb, v0, c0 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_238_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_238_Desc = "ps_3_sw : m3x3 is NOT allowed in a 25 level rep i0 breakp p0.x block";
string PS_238 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								rep i0 breakp p0.x "
	"																									m3x3 r0.rgb, v0, c0 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_239_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_239_Desc = "ps_3_sw : m3x3 is allowed in a 1 level loop aL, i0 break_ne c0.x, c0.y block";
string PS_239 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	m3x3 r0.rgb, v0, c0 "
	"endloop ";

PS_CRITERIA PS_240_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_240_Desc = "ps_3_sw : m3x3 is NOT allowed in a 8 level loop aL, i0 break_ne c0.x, c0.y block";
string PS_240 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		loop aL, i0 break_ne c0.x, c0.y "
	"			loop aL, i0 break_ne c0.x, c0.y "
	"				loop aL, i0 break_ne c0.x, c0.y "
	"					loop aL, i0 break_ne c0.x, c0.y "
	"						loop aL, i0 break_ne c0.x, c0.y "
	"							loop aL, i0 break_ne c0.x, c0.y "
	"								m3x3 r0.rgb, v0, c0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_241_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_241_Desc = "ps_3_sw : m3x3 is NOT allowed in a 16 level loop aL, i0 break_ne c0.x, c0.y block";
string PS_241 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		loop aL, i0 break_ne c0.x, c0.y "
	"			loop aL, i0 break_ne c0.x, c0.y "
	"				loop aL, i0 break_ne c0.x, c0.y "
	"					loop aL, i0 break_ne c0.x, c0.y "
	"						loop aL, i0 break_ne c0.x, c0.y "
	"							loop aL, i0 break_ne c0.x, c0.y "
	"								loop aL, i0 break_ne c0.x, c0.y "
	"									loop aL, i0 break_ne c0.x, c0.y "
	"										loop aL, i0 break_ne c0.x, c0.y "
	"											loop aL, i0 break_ne c0.x, c0.y "
	"												loop aL, i0 break_ne c0.x, c0.y "
	"													loop aL, i0 break_ne c0.x, c0.y "
	"														loop aL, i0 break_ne c0.x, c0.y "
	"															loop aL, i0 break_ne c0.x, c0.y "
	"																m3x3 r0.rgb, v0, c0 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_242_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_242_Desc = "ps_3_sw : m3x3 is NOT allowed in a 24 level loop aL, i0 break_ne c0.x, c0.y block";
string PS_242 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		loop aL, i0 break_ne c0.x, c0.y "
	"			loop aL, i0 break_ne c0.x, c0.y "
	"				loop aL, i0 break_ne c0.x, c0.y "
	"					loop aL, i0 break_ne c0.x, c0.y "
	"						loop aL, i0 break_ne c0.x, c0.y "
	"							loop aL, i0 break_ne c0.x, c0.y "
	"								loop aL, i0 break_ne c0.x, c0.y "
	"									loop aL, i0 break_ne c0.x, c0.y "
	"										loop aL, i0 break_ne c0.x, c0.y "
	"											loop aL, i0 break_ne c0.x, c0.y "
	"												loop aL, i0 break_ne c0.x, c0.y "
	"													loop aL, i0 break_ne c0.x, c0.y "
	"														loop aL, i0 break_ne c0.x, c0.y "
	"															loop aL, i0 break_ne c0.x, c0.y "
	"																loop aL, i0 break_ne c0.x, c0.y "
	"																	loop aL, i0 break_ne c0.x, c0.y "
	"																		loop aL, i0 break_ne c0.x, c0.y "
	"																			loop aL, i0 break_ne c0.x, c0.y "
	"																				loop aL, i0 break_ne c0.x, c0.y "
	"																					loop aL, i0 break_ne c0.x, c0.y "
	"																						loop aL, i0 break_ne c0.x, c0.y "
	"																							loop aL, i0 break_ne c0.x, c0.y "
	"																								m3x3 r0.rgb, v0, c0 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_243_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_243_Desc = "ps_3_sw : m3x3 is NOT allowed in a 25 level loop aL, i0 break_ne c0.x, c0.y block";
string PS_243 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 break_ne c0.x, c0.y "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		loop aL, i0 break_ne c0.x, c0.y "
	"			loop aL, i0 break_ne c0.x, c0.y "
	"				loop aL, i0 break_ne c0.x, c0.y "
	"					loop aL, i0 break_ne c0.x, c0.y "
	"						loop aL, i0 break_ne c0.x, c0.y "
	"							loop aL, i0 break_ne c0.x, c0.y "
	"								loop aL, i0 break_ne c0.x, c0.y "
	"									loop aL, i0 break_ne c0.x, c0.y "
	"										loop aL, i0 break_ne c0.x, c0.y "
	"											loop aL, i0 break_ne c0.x, c0.y "
	"												loop aL, i0 break_ne c0.x, c0.y "
	"													loop aL, i0 break_ne c0.x, c0.y "
	"														loop aL, i0 break_ne c0.x, c0.y "
	"															loop aL, i0 break_ne c0.x, c0.y "
	"																loop aL, i0 break_ne c0.x, c0.y "
	"																	loop aL, i0 break_ne c0.x, c0.y "
	"																		loop aL, i0 break_ne c0.x, c0.y "
	"																			loop aL, i0 break_ne c0.x, c0.y "
	"																				loop aL, i0 break_ne c0.x, c0.y "
	"																					loop aL, i0 break_ne c0.x, c0.y "
	"																						loop aL, i0 break_ne c0.x, c0.y "
	"																							loop aL, i0 break_ne c0.x, c0.y "
	"																								loop aL, i0 break_ne c0.x, c0.y "
	"																									m3x3 r0.rgb, v0, c0 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_244_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_244_Desc = "ps_3_sw : m3x3 is allowed in a 1 level rep i0 break_ne c0.x, c0.y block";
string PS_244 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 break_ne c0.x, c0.y "
	"	m3x3 r0.rgb, v0, c0 "
	"endrep ";

PS_CRITERIA PS_245_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_245_Desc = "ps_3_sw : m3x3 is NOT allowed in a 8 level rep i0 break_ne c0.x, c0.y block";
string PS_245 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 break_ne c0.x, c0.y "
	"	rep i0 break_ne c0.x, c0.y "
	"		rep i0 break_ne c0.x, c0.y "
	"			rep i0 break_ne c0.x, c0.y "
	"				rep i0 break_ne c0.x, c0.y "
	"					rep i0 break_ne c0.x, c0.y "
	"						rep i0 break_ne c0.x, c0.y "
	"							rep i0 break_ne c0.x, c0.y "
	"								m3x3 r0.rgb, v0, c0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_246_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_246_Desc = "ps_3_sw : m3x3 is NOT allowed in a 16 level rep i0 break_ne c0.x, c0.y block";
string PS_246 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 break_ne c0.x, c0.y "
	"	rep i0 break_ne c0.x, c0.y "
	"		rep i0 break_ne c0.x, c0.y "
	"			rep i0 break_ne c0.x, c0.y "
	"				rep i0 break_ne c0.x, c0.y "
	"					rep i0 break_ne c0.x, c0.y "
	"						rep i0 break_ne c0.x, c0.y "
	"							rep i0 break_ne c0.x, c0.y "
	"								rep i0 break_ne c0.x, c0.y "
	"									rep i0 break_ne c0.x, c0.y "
	"										rep i0 break_ne c0.x, c0.y "
	"											rep i0 break_ne c0.x, c0.y "
	"												rep i0 break_ne c0.x, c0.y "
	"													rep i0 break_ne c0.x, c0.y "
	"														rep i0 break_ne c0.x, c0.y "
	"															rep i0 break_ne c0.x, c0.y "
	"																m3x3 r0.rgb, v0, c0 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_247_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_247_Desc = "ps_3_sw : m3x3 is NOT allowed in a 24 level rep i0 break_ne c0.x, c0.y block";
string PS_247 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break_ne c0.x, c0.y "
	"	rep i0 break_ne c0.x, c0.y "
	"		rep i0 break_ne c0.x, c0.y "
	"			rep i0 break_ne c0.x, c0.y "
	"				rep i0 break_ne c0.x, c0.y "
	"					rep i0 break_ne c0.x, c0.y "
	"						rep i0 break_ne c0.x, c0.y "
	"							rep i0 break_ne c0.x, c0.y "
	"								rep i0 break_ne c0.x, c0.y "
	"									rep i0 break_ne c0.x, c0.y "
	"										rep i0 break_ne c0.x, c0.y "
	"											rep i0 break_ne c0.x, c0.y "
	"												rep i0 break_ne c0.x, c0.y "
	"													rep i0 break_ne c0.x, c0.y "
	"														rep i0 break_ne c0.x, c0.y "
	"															rep i0 break_ne c0.x, c0.y "
	"																rep i0 break_ne c0.x, c0.y "
	"																	rep i0 break_ne c0.x, c0.y "
	"																		rep i0 break_ne c0.x, c0.y "
	"																			rep i0 break_ne c0.x, c0.y "
	"																				rep i0 break_ne c0.x, c0.y "
	"																					rep i0 break_ne c0.x, c0.y "
	"																						rep i0 break_ne c0.x, c0.y "
	"																							rep i0 break_ne c0.x, c0.y "
	"																								m3x3 r0.rgb, v0, c0 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_248_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_248_Desc = "ps_3_sw : m3x3 is NOT allowed in a 25 level rep i0 break_ne c0.x, c0.y block";
string PS_248 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break_ne c0.x, c0.y "
	"	rep i0 break_ne c0.x, c0.y "
	"		rep i0 break_ne c0.x, c0.y "
	"			rep i0 break_ne c0.x, c0.y "
	"				rep i0 break_ne c0.x, c0.y "
	"					rep i0 break_ne c0.x, c0.y "
	"						rep i0 break_ne c0.x, c0.y "
	"							rep i0 break_ne c0.x, c0.y "
	"								rep i0 break_ne c0.x, c0.y "
	"									rep i0 break_ne c0.x, c0.y "
	"										rep i0 break_ne c0.x, c0.y "
	"											rep i0 break_ne c0.x, c0.y "
	"												rep i0 break_ne c0.x, c0.y "
	"													rep i0 break_ne c0.x, c0.y "
	"														rep i0 break_ne c0.x, c0.y "
	"															rep i0 break_ne c0.x, c0.y "
	"																rep i0 break_ne c0.x, c0.y "
	"																	rep i0 break_ne c0.x, c0.y "
	"																		rep i0 break_ne c0.x, c0.y "
	"																			rep i0 break_ne c0.x, c0.y "
	"																				rep i0 break_ne c0.x, c0.y "
	"																					rep i0 break_ne c0.x, c0.y "
	"																						rep i0 break_ne c0.x, c0.y "
	"																							rep i0 break_ne c0.x, c0.y "
	"																								rep i0 break_ne c0.x, c0.y "
	"																									m3x3 r0.rgb, v0, c0 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_249_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_249_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and if b0 block";
string PS_249 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	if b0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_250_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_250_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and if b0 nop else block";
string PS_250 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	if b0 nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_251_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_251_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and loop aL, i0 block";
string PS_251 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	loop aL, i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_252_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_252_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and rep i0 block";
string PS_252 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	rep i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_253_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_253_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and if_ne c0.x, c0.y block";
string PS_253 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	if_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_254_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_254_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and if_ne c0.x, c0.y nop else block";
string PS_254 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	if_ne c0.x, c0.y nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_255_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_255_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and if p0.x block";
string PS_255 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	if p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_256_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_256_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and if p0.x nop else block";
string PS_256 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	if p0.x nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_257_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_257_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and loop aL, i0 break block";
string PS_257 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	loop aL, i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_258_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_258_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and rep i0 break block";
string PS_258 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	rep i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_259_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_259_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and loop aL, i0 breakp p0.x block";
string PS_259 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_260_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_260_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and rep i0 breakp p0.x block";
string PS_260 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_261_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_261_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and loop aL, i0 break_ne c0.x, c0.y block";
string PS_261 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_262_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_262_Desc = "ps_3_sw : m3x3 is allowed in a if b0 and rep i0 break_ne c0.x, c0.y block";
string PS_262 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 "
	"	rep i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_263_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_263_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and if b0 block";
string PS_263 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	if b0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_264_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_264_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and if b0 nop else block";
string PS_264 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_265_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_265_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and loop aL, i0 block";
string PS_265 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_266_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_266_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and rep i0 block";
string PS_266 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	rep i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_267_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_267_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and if_ne c0.x, c0.y block";
string PS_267 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	if_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_268_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_268_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and if_ne c0.x, c0.y nop else block";
string PS_268 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	if_ne c0.x, c0.y nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_269_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_269_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and if p0.x block";
string PS_269 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	if p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_270_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_270_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and if p0.x nop else block";
string PS_270 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_271_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_271_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and loop aL, i0 break block";
string PS_271 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_272_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_272_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and rep i0 break block";
string PS_272 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	rep i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_273_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_273_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string PS_273 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_274_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_274_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string PS_274 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_275_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_275_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and loop aL, i0 break_ne c0.x, c0.y block";
string PS_275 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_276_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_276_Desc = "ps_3_sw : m3x3 is allowed in a if b0 nop else and rep i0 break_ne c0.x, c0.y block";
string PS_276 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if b0 nop else "
	"	rep i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_277_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_277_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and if b0 block";
string PS_277 = 
	"ps_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	if b0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_278_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_278_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and if b0 nop else block";
string PS_278 = 
	"ps_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_279_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_279_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and loop aL, i0 block";
string PS_279 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_280_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_280_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and rep i0 block";
string PS_280 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	rep i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_281_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_281_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and if_ne c0.x, c0.y block";
string PS_281 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	if_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_282_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_282_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and if_ne c0.x, c0.y nop else block";
string PS_282 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	if_ne c0.x, c0.y nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_283_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_283_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and if p0.x block";
string PS_283 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	if p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_284_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_284_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and if p0.x nop else block";
string PS_284 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_285_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_285_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and loop aL, i0 break block";
string PS_285 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_286_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_286_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and rep i0 break block";
string PS_286 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	rep i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_287_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_287_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string PS_287 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_288_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_288_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string PS_288 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_289_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_289_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and loop aL, i0 break_ne c0.x, c0.y block";
string PS_289 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_290_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_290_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 and rep i0 break_ne c0.x, c0.y block";
string PS_290 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	rep i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_291_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_291_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and if b0 block";
string PS_291 = 
	"ps_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	if b0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_292_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_292_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and if b0 nop else block";
string PS_292 = 
	"ps_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	if b0 nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_293_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_293_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and loop aL, i0 block";
string PS_293 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	loop aL, i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_294_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_294_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and rep i0 block";
string PS_294 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	rep i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_295_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_295_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and if_ne c0.x, c0.y block";
string PS_295 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	if_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_296_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_296_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and if_ne c0.x, c0.y nop else block";
string PS_296 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	if_ne c0.x, c0.y nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_297_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_297_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and if p0.x block";
string PS_297 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	if p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_298_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_298_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and if p0.x nop else block";
string PS_298 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	if p0.x nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_299_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_299_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and loop aL, i0 break block";
string PS_299 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	loop aL, i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_300_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_300_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and rep i0 break block";
string PS_300 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	rep i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_301_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_301_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string PS_301 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_302_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_302_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and rep i0 breakp p0.x block";
string PS_302 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_303_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_303_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and loop aL, i0 break_ne c0.x, c0.y block";
string PS_303 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_304_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_304_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 and rep i0 break_ne c0.x, c0.y block";
string PS_304 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 "
	"	rep i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_305_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_305_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and if b0 block";
string PS_305 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	if b0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_306_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_306_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and if b0 nop else block";
string PS_306 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	if b0 nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_307_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_307_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and loop aL, i0 block";
string PS_307 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	loop aL, i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_308_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_308_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and rep i0 block";
string PS_308 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	rep i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_309_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_309_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and if_ne c0.x, c0.y block";
string PS_309 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_310_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_310_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and if_ne c0.x, c0.y nop else block";
string PS_310 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	if_ne c0.x, c0.y nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_311_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_311_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and if p0.x block";
string PS_311 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	if p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_312_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_312_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and if p0.x nop else block";
string PS_312 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	if p0.x nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_313_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_313_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and loop aL, i0 break block";
string PS_313 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	loop aL, i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_314_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_314_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and rep i0 break block";
string PS_314 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	rep i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_315_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_315_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_315 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_316_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_316_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and rep i0 breakp p0.x block";
string PS_316 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_317_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_317_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and loop aL, i0 break_ne c0.x, c0.y block";
string PS_317 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_318_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_318_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y and rep i0 break_ne c0.x, c0.y block";
string PS_318 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y "
	"	rep i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_319_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_319_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and if b0 block";
string PS_319 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"if_ne c0.x, c0.y nop else "
	"	if b0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_320_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_320_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and if b0 nop else block";
string PS_320 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	if b0 nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_321_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_321_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and loop aL, i0 block";
string PS_321 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_322_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_322_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and rep i0 block";
string PS_322 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	rep i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_323_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_323_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and if_ne c0.x, c0.y block";
string PS_323 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	if_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_324_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_324_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and if_ne c0.x, c0.y nop else block";
string PS_324 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	if_ne c0.x, c0.y nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_325_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_325_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and if p0.x block";
string PS_325 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	if p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_326_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_326_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and if p0.x nop else block";
string PS_326 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_327_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_327_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and loop aL, i0 break block";
string PS_327 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_328_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_328_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and rep i0 break block";
string PS_328 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	rep i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_329_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_329_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string PS_329 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_330_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_330_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and rep i0 breakp p0.x block";
string PS_330 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_331_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_331_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and loop aL, i0 break_ne c0.x, c0.y block";
string PS_331 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_332_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_332_Desc = "ps_3_sw : m3x3 is allowed in a if_ne c0.x, c0.y nop else and rep i0 break_ne c0.x, c0.y block";
string PS_332 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ne c0.x, c0.y nop else "
	"	rep i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_333_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_333_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and if b0 block";
string PS_333 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	if b0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_334_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_334_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and if b0 nop else block";
string PS_334 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	if b0 nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_335_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_335_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and loop aL, i0 block";
string PS_335 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	loop aL, i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_336_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_336_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and rep i0 block";
string PS_336 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	rep i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_337_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_337_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and if_ne c0.x, c0.y block";
string PS_337 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	if_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_338_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_338_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and if_ne c0.x, c0.y nop else block";
string PS_338 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	if_ne c0.x, c0.y nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_339_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_339_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and if p0.x block";
string PS_339 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	if p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_340_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_340_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and if p0.x nop else block";
string PS_340 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	if p0.x nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_341_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_341_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and loop aL, i0 break block";
string PS_341 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	loop aL, i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_342_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_342_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and rep i0 break block";
string PS_342 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	rep i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_343_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_343_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string PS_343 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_344_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_344_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and rep i0 breakp p0.x block";
string PS_344 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_345_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_345_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and loop aL, i0 break_ne c0.x, c0.y block";
string PS_345 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_346_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_346_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x and rep i0 break_ne c0.x, c0.y block";
string PS_346 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x "
	"	rep i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_347_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_347_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and if b0 block";
string PS_347 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if b0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_348_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_348_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and if b0 nop else block";
string PS_348 = 
	"ps_3_sw "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_349_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_349_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and loop aL, i0 block";
string PS_349 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_350_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_350_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and rep i0 block";
string PS_350 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	rep i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_351_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_351_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and if_ne c0.x, c0.y block";
string PS_351 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_352_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_352_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and if_ne c0.x, c0.y nop else block";
string PS_352 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if_ne c0.x, c0.y nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_353_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_353_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and if p0.x block";
string PS_353 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_354_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_354_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and if p0.x nop else block";
string PS_354 = 
	"ps_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_355_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_355_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and loop aL, i0 break block";
string PS_355 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_356_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_356_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and rep i0 break block";
string PS_356 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	rep i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_357_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_357_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string PS_357 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_358_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_358_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string PS_358 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_359_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_359_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and loop aL, i0 break_ne c0.x, c0.y block";
string PS_359 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_360_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_360_Desc = "ps_3_sw : m3x3 is allowed in a if p0.x nop else and rep i0 break_ne c0.x, c0.y block";
string PS_360 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	rep i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_361_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_361_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and if b0 block";
string PS_361 = 
	"ps_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if b0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_362_Criteria = { tr