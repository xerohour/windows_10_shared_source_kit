//*************************************************************************************************
// File: LinearFilter.cpp
// 
// Copyright (C) 1994-2004 Microsoft Corporation.  All Rights Reserved.
//
// Author:      Cliff Owen (cliffo)
//*************************************************************************************************
#include "filter.h"
#include "d3d9.h"
#include "CEffect.h"

//*************************************************************************************************
// Float filtering, test specific behavior is managed by CLinearFloatBase. Linear min and mag
// filtering tests derive from this.
//
// The base test loads a PS 2.0 pixel shader to operate on. PS 2.0 is required for two reasons:
//
// 1. A pixel shader is the only way to modify a texel read larger than 0.0 to 1.0.
// 2. A PS 1.x pixel shader is limited in it's ability to handle values larger than 1.0 makes them
//    less than ideal for using floating point formats.
//
// The test then modifies the floating point values of a given texture (via ScaleFloatTexture)
// by scaling the texel values up or down, producing ranges from -16 to 0.0 for the negative case
// or 0.0 to 16.0 for the positive case. These are then normalized in the pixel shader to their
// appropriate values.
//*************************************************************************************************

class CLinearFloatBase: public CFilterTest
{
    public:
    
    CnEffect      * m_pEffect;
    TCHAR           m_szScalingString[80];
    
    //*************************************************************************************************
    
    CLinearFloatBase()
    {
	    m_Options.ModeOptions.TextureFmt = FMT_ALL;
	    m_Options.ModeOptions.dwTexInclude = PF_FLOATTEX;
	    m_Options.D3DOptions.fMinDXVersion = 9.0f;

	    m_pEffect = NULL;
	}
	
	//*************************************************************************************************
	
    virtual bool CapsCheck(void)
    {
        DWORD dwSupported = m_pSrcDevice->GetCaps()->dwPixelShaderVersion;
    
        // Check major version
    
        if ((D3DSHADER_VERSION_MAJOR(dwSupported) < 2))
        {
            WriteToLog( _T("Minimum pixel shader version of 2.0 required for this test.\n") );

            return false;
        }

        return CFilterTest::CapsCheck();
    }

    //*************************************************************************************************

    virtual void UpdateStatus()
    {
        CD3DTest::UpdateStatus();
        WriteStatus("$wScaling:\t",m_szScalingString);
        WriteStatus("$wSamp State:",m_szStatusString);
    }
    
    //*************************************************************************************************
    
	virtual void TestSpecificSetup(UINT uTestNum)
	{
	    CTexture * pTexture;
	    bool        bNeg = false;
	    
	    // Determine if using positive or negative scaling with this test.
	    
	    if (uTestNum > this->GetNumTests() / 2)
	        bNeg = true;
	        
	    // Update status window information

        if (bNeg)
            lstrcpy(m_szScalingString,"$aNegative");
        else
            lstrcpy(m_szScalingString,"$aPositive");

        // Scale the source or reference texture

	    m_pTexture->GetTexture(&pTexture,0);

	    if (!ScaleFloatTexture(pTexture,bNeg))
	    {
	        WriteToLog( _T("Failure scaling source texture.\n") );
	        Skip();
	        return;
	    }
	    
	    RELEASE(pTexture);
	    
	    m_pTexture->GetTexture(&pTexture,1);
	    
	    if (!ScaleFloatTexture(pTexture,bNeg))
	    {
	        WriteToLog( _T("Failure scaling source texture.\n") );
	        Skip();
	        return;
	    }
	    
	    RELEASE(pTexture);
	    
	    // Setup the appropriate pixel shader for positive or negative scaling

        LPD3DXBUFFER    pErrors;
	    
	    bool bResult = CreateEffectFromResource(NULL, "FloatTex.FX", NULL, NULL, 0, NULL, &m_pEffect, &pErrors);
	    
	    RELEASE(pErrors);
	    
	    D3DXEFFECT_DESC FXDesc;
	    m_pEffect->GetDesc(&FXDesc);

	    UINT uTechnique = 0;
	    
	    if (bNeg)
	        uTechnique = 1;
	        
	    m_pEffect->SetTechniqueByIndex(uTechnique);
	    m_pEffect->ValidateTechniqueByIndex(uTechnique);

        UINT uPasses;
        
        m_pEffect->SetTexture("Tex0",m_pTexture);
	    
	    m_pEffect->Begin(&uPasses,uTechnique);
	    m_pEffect->BeginPass(0);
	}
	
	//*************************************************************************************************
	
    virtual void TestSpecificCleanup()
    {
        m_pEffect->EndPass();
        m_pEffect->End();
        
        RELEASE(m_pEffect);
    }
    
    //*************************************************************************************************
    
    bool ScaleFloatTexture(CTexture * pTexture, bool bNegative)
    {
        CSurface * pSurface;
        
        // Get the surface information
        
        LRESULT lResult = pTexture->GetSurfaceLevel(0,&pSurface);
        
        if (FAILED(lResult))
            return false;

        SURFACEDESC Desc;
            
        pSurface->GetDesc(&Desc);
        
        // Define scaling

        float   f32BitScale = 16.0f;
        
        if (bNegative)
            f32BitScale *= -1.0f;

        bool    b16Bit = false;
        UINT    uChannels = 0;

        // Count up the channels and mark the bit count
        
        switch(Desc.Format.d3dfFormat)
        {
            case    D3DFMT_R16F:
                    {
                        b16Bit = true;
                        uChannels = 1;
                    }
                    break;
            case    D3DFMT_G16R16F:
                    {
                        b16Bit = true;
                        uChannels = 2;
                    }
                    break;
            case    D3DFMT_A16B16G16R16F:
                    {
                        b16Bit = true;
                        uChannels = 4;
                    }
                    break;
                    
            case    D3DFMT_R32F:
                    {
                        b16Bit = false;
                        uChannels = 1;
                    }
                    break;
            
            case    D3DFMT_G32R32F:
                    {
                        b16Bit = false;
                        uChannels = 2;
                    }
                    break;        
            case    D3DFMT_A32B32G32R32F:
                    {
                        b16Bit = false;
                        uChannels = 4;
                    }
                    break;

            // If it's not a floating point format, return false.
                    
            default:
                    return false;
        }
        
        // Get the texture information

        LOCKED_RECT rect;
        
        lResult = pSurface->LockRect(&rect,NULL,0);
        
        if (FAILED(lResult))
            return false;

        // Scale up the actual texture.

        for (UINT uY=0; uY<Desc.dwHeight; ++uY)
        {
            PBYTE   pTextureMemory = (PBYTE)rect.pBits;
            
            pTextureMemory += uY * rect.iPitch;
            
            for (UINT uX=0; uX<Desc.dwWidth; ++uX)
            {
                // 16 bit texture case.
                
                if (b16Bit)
                {
                    for (UINT uC = 0; uC<uChannels; ++uC)
                    {
                        CFloat16 Value;
                        
                        Value.v = *(PWORD) pTextureMemory;
                        
                        float fTemp = (float) Value;
                        
                        fTemp *= f32BitScale;
                        
                        Value = fTemp;
                        
                        *(PWORD) pTextureMemory = Value.v;
                        
                        pTextureMemory += sizeof(WORD);
                    }
                }
                else
                {
                    // 32 bit texture case.
                
                    for (UINT uC = 0; uC<uChannels; ++uC)
                    {
                        *(PFLOAT)pTextureMemory *= f32BitScale;
                        pTextureMemory += sizeof(float);
                    }
                }
            }
        }
        
        // All done. Unlock and release the surface.
        
        pSurface->UnlockRect();
        RELEASE(pSurface);
        
        return true;
    }

};

//*************************************************************************************************
// Linear Float Filtering, Minification
//*************************************************************************************************

class CLinearFloatMin: public CLinearFloatBase
{
    public:
    
    CLinearFloatMin()
    {
	    m_szTestName = "Texture Filter Linear Minify (float)";
	    m_szCommandKey = "LinearFloatMin";
	    
        FILTERLIST  FilterList[] = {
            // Width    Height      Texture Size    MinFilter       MagFilter
            
            // Positive scaling

            { 32,       32,         64,             TEXF_LINEAR,    TEXF_NONE},
            { 32,       32,         128,            TEXF_LINEAR,    TEXF_NONE},
            { 32,       32,         256,            TEXF_LINEAR,    TEXF_NONE},
            { 48,       48,         64,             TEXF_LINEAR,    TEXF_NONE},
            { 48,       48,         128,            TEXF_LINEAR,    TEXF_NONE},
            { 48,       48,         256,            TEXF_LINEAR,    TEXF_NONE},
            { 56,       56,         64,             TEXF_LINEAR,    TEXF_NONE},
            { 56,       56,         128,            TEXF_LINEAR,    TEXF_NONE},
            { 56,       56,         256,            TEXF_LINEAR,    TEXF_NONE},
            { 96,       96,         128,            TEXF_LINEAR,    TEXF_NONE},
            { 96,       96,         256,            TEXF_LINEAR,    TEXF_NONE},
            { 104,      104,        128,            TEXF_LINEAR,    TEXF_NONE},
            { 104,      104,        256,            TEXF_LINEAR,    TEXF_NONE},
            { 120,      120,        128,            TEXF_LINEAR,    TEXF_NONE},
            { 120,      120,        256,            TEXF_LINEAR,    TEXF_NONE},
            { 192,      192,        256,            TEXF_LINEAR,    TEXF_NONE},
            { 216,      216,        256,            TEXF_LINEAR,    TEXF_NONE},
            
            // Negative scaling
            
            { 32,       32,         64,             TEXF_LINEAR,    TEXF_NONE},
            { 32,       32,         128,            TEXF_LINEAR,    TEXF_NONE},
            { 32,       32,         256,            TEXF_LINEAR,    TEXF_NONE},
            { 48,       48,         64,             TEXF_LINEAR,    TEXF_NONE},
            { 48,       48,         128,            TEXF_LINEAR,    TEXF_NONE},
            { 48,       48,         256,            TEXF_LINEAR,    TEXF_NONE},
            { 56,       56,         64,             TEXF_LINEAR,    TEXF_NONE},
            { 56,       56,         128,            TEXF_LINEAR,    TEXF_NONE},
            { 56,       56,         256,            TEXF_LINEAR,    TEXF_NONE},
            { 96,       96,         128,            TEXF_LINEAR,    TEXF_NONE},
            { 96,       96,         256,            TEXF_LINEAR,    TEXF_NONE},
            { 104,      104,        128,            TEXF_LINEAR,    TEXF_NONE},
            { 104,      104,        256,            TEXF_LINEAR,    TEXF_NONE},
            { 120,      120,        128,            TEXF_LINEAR,    TEXF_NONE},
            { 120,      120,        256,            TEXF_LINEAR,    TEXF_NONE},
            { 192,      192,        256,            TEXF_LINEAR,    TEXF_NONE},
            { 216,      216,        256,            TEXF_LINEAR,    TEXF_NONE},
        };	

        // Set this data as the permanent data associated with the test.

        SetFilterList(FilterList,sizeof(FilterList)/sizeof(FILTERLIST));	
	}
};

CLinearFloatMin   LinearFloatMin;

//*************************************************************************************************
// Linear Float Filtering, Magnification
//*************************************************************************************************

class CLinearFloatMag: public CLinearFloatBase
{
    public:
    
    CLinearFloatMag()
    {
	    m_szTestName = "Texture Filter Linear Magnify (float)";
	    m_szCommandKey = "LinearFloatMag";

        FILTERLIST  FilterList[] = {
            // Width    Height      Texture Size    MinFilter       MagFilter
            
            // Positive scaling

            { 48,       48,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 56,       56,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 64,       64,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 76,       76,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 76,       76,         64,             TEXF_NONE,      TEXF_LINEAR},
            { 96,       96,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 96,       96,         64,             TEXF_NONE,      TEXF_LINEAR},
            { 102,      102,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 102,      102,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 120,      120,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 120,      120,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 156,      156,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 156,      156,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 156,      156,        128,            TEXF_NONE,      TEXF_LINEAR},
            { 192,      192,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 192,      192,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 192,      192,        128,            TEXF_NONE,      TEXF_LINEAR},
            { 216,      216,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 216,      216,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 216,      216,        128,            TEXF_NONE,      TEXF_LINEAR},
            
            // Negative scaling
            
            { 48,       48,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 56,       56,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 64,       64,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 76,       76,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 76,       76,         64,             TEXF_NONE,      TEXF_LINEAR},
            { 96,       96,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 96,       96,         64,             TEXF_NONE,      TEXF_LINEAR},
            { 102,      102,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 102,      102,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 120,      120,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 120,      120,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 156,      156,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 156,      156,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 156,      156,        128,            TEXF_NONE,      TEXF_LINEAR},
            { 192,      192,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 192,      192,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 192,      192,        128,            TEXF_NONE,      TEXF_LINEAR},
            { 216,      216,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 216,      216,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 216,      216,        128,            TEXF_NONE,      TEXF_LINEAR},            
        };	

        // Set this data as the permanent data associated with the test.

        SetFilterList(FilterList,sizeof(FilterList)/sizeof(FILTERLIST));	
	}
};

CLinearFloatMag   LinearFloatMag;

