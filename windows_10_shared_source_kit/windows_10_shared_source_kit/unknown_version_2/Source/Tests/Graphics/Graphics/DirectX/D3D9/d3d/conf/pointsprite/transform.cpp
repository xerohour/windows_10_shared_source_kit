// FILE:        WrapTransform.cpp
// DESC:        class methods for point sprite conformance test
// AUTHOR:      cliffo
// COMMENTS:    checks texture addressing has no effect on point sprites

#include "sprite.h"

#define WrapModes   2
#define ScaleModes  2
#define RotateModes 2

// NAME:        CSpriteWrapTrans()
// DESC:        constructor for sprite cull class
// INPUT:       none
// OUTPUT:      none

CSpriteWrapTrans::CSpriteWrapTrans()
{
    m_szTestName="Transform mode";
    m_szCommandKey="Trans";

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_LATEST;

	m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        ~CSpriteWrapTrans()
// DESC:        constructor for sprite cull class
// INPUT:       none
// OUTPUT:      none

CSpriteWrapTrans::~CSpriteWrapTrans()
{
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteWrapTrans::TestInitialize(VOID)
{
    UINT uResult = CSpriteMipMapMulti::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    SetTestRange((UINT) 1, 7);

    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise

bool CSpriteWrapTrans::ExecuteTest(UINT uTest)
{
    // Set texture transform

    D3DMATRIX   Transform;

    ZeroMemory(&Transform,sizeof(D3DMATRIX));
    Transform._11 = 0.1f * uTest;
    Transform._22 = 0.1f * uTest;
    Transform._31 = 0.1f * uTest;
    Transform._32 = 0.1f * uTest;

    SetTransform(D3DTS_TEXTURE0,&Transform);
    SetTransform(D3DTS_TEXTURE1,&Transform);
    SetTransform(D3DTS_TEXTURE2,&Transform);
    SetTransform(D3DTS_TEXTURE3,&Transform);
    SetTransform(D3DTS_TEXTURE4,&Transform);
    SetTransform(D3DTS_TEXTURE5,&Transform);
    SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
    SetTextureStageState(1,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
    SetTextureStageState(2,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
    SetTextureStageState(3,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
    SetTextureStageState(4,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
    SetTextureStageState(5,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
    
    return CSpriteMipMapMulti::ExecuteTest(uTest);
}
