#define D3D_OVERLOADS
#include <d3d9.h>
#include <d3dx9.h>
#include "VTVolume.h"
#include "testutils.h"

CMipVolTest		    MipVol;     // Volume mipmap texture test cases

CVolumeTest::CVolumeTest()
{
	// Initialize some variables
	pTVertex = NULL;
	pVolTexture = NULL;
	nVertexMax = 0;

	SetTestType(TESTTYPE_CONF);

	//m_Options.D3DOptions.fMinDXVersion = 8.f;
	//m_Options.D3DOptions.fMaxDXVersion = 15.f;
	m_Options.TestOptions.bCompareSrcRef = true;

    // Tell framework that which devices we want to test
	//m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL; // | DEVICETYPE_TNL | DEVICETYPE_PURE;
}

CVolumeTest::~CVolumeTest()
{
}

bool CVolumeTest::SetDefaultMaterials(void)
{
	MATERIAL Material;

	ZeroMemory(&Material,sizeof(MATERIAL));

    // No alpha
	Material.Diffuse.a = 1.0f;

    // White 
    Material.Ambient.r = 1.0f;
	Material.Ambient.g = 1.0f;
	Material.Ambient.b = 1.0f;

	if (!SetMaterial(&Material))
	{
		//WriteToLog("SetMaterial() failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
        return false;
	}

	return true;
}

bool CVolumeTest::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRS_AMBIENT,(DWORD)0xFFFFFFFF);

	return true;
}

bool CVolumeTest::SetDefaultMatrices(void)
{
	D3DXMATRIX Matrix;

	D3DXMatrixPerspectiveFovLH(&Matrix, pi/2.0f,320.0f/280.0f,1.0f,50.0f);

	if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&Matrix))
	{
		//WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
        return false;
	}

	D3DXMatrixIdentity(&Matrix);

	if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *)&Matrix))
	{
		//WriteToLog("SetTransform(WorldMatrix) failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
        return false;
	}

	D3DXMatrixIdentity(&Matrix);

	if (!SetTransform(D3DTS_VIEW, (D3DMATRIX *)&Matrix))
	{
		//WriteToLog("SetTransform(ViewMatrix) failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
        return false;
	}

	return true;
}


void CVolumeTest::SceneRefresh(void)
{
	if (BeginScene())
	{
        RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0),pTVertex,nVertexMax,NULL,0,NULL);

        EndScene();
	}	
}

bool CVolumeTest::TestTerminate(void)
{
    // Cleanup vertex data
	if (pTVertex!=NULL) 
	{
		delete []pTVertex;
		pTVertex = NULL;
	}

    // Cleanup texture
	SetTexture(0, NULL);
    RELEASE(pVolTexture);

	return true;
}

void CVolumeTest::UpdateStatus(bool bResult)
{
	if (bResult)
		Pass();
	else
		Fail();
}

#if FAST_TEST
bool CVolumeTest::AddFormatModes(DISPLAYMODE *pMode, bool bWindowed)
{
    UINT uTexture = 0;
    UINT uZBuffer = 0;
    UINT uCube = 0;
    UINT uVolume = 0;
    UINT uMS = 0;
    UINT uBB = 0;

    // Convert the formats into indexes into the common lists
    if (!GetFormatIndex(m_Options.ModeOptions.TextureFmt, m_pCommonTextureFormats, &uTexture) ||
        !GetZBufferFormatIndex(m_Options.ModeOptions.ZBufferFmt, m_pCommonZBufferFormats, &(pMode->Format), &uZBuffer) ||
        !GetFormatIndex(m_Options.ModeOptions.CubeFmt, m_pCommonCubeFormats, &uCube) ||
        !GetFormatIndex(m_Options.ModeOptions.VolumeFmt, m_pCommonVolumeFormats, &uVolume) ||
        !GetFormatIndex(m_Options.ModeOptions.MultisampType, m_pCommonMultiSampleFormats, m_uCommonMultiSampleFormats, &uMS) ||
        !GetBackBufferFormatIndex(m_Options.ModeOptions.BackBufferFmt, m_pCommonBackBufferFormats, &(pMode->Format), &uBB) ||
        // We need to validate MS here.  If the device doens't support this MS type, we don't want to add it as a mode
        (m_pCommonMultiSampleFormats && m_pCommonZBufferFormats &&
        (FAILED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, bWindowed, m_pCommonMultiSampleFormats[uMS], NULL)) ||
        FAILED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonZBufferFormats[uZBuffer], bWindowed, m_pCommonMultiSampleFormats[uMS], NULL)))))
    {
        DPF(2, _T("Requested fmt not supported by device.\n"));
        return false;
    }

    // If only one mode was requested, add it and return
    if (m_Options.ModeOptions.TextureFmt != FMT_ALL &&
        m_Options.ModeOptions.ZBufferFmt != FMT_ALL &&
        m_Options.ModeOptions.CubeFmt != FMT_ALL &&
        m_Options.ModeOptions.VolumeFmt != FMT_ALL &&
        m_Options.ModeOptions.BackBufferFmt != FMT_ALL &&
        m_Options.ModeOptions.MultisampType != MULTISAMPLE_ALL)
    {
        return AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB);
    }

    if (m_Options.ModeOptions.BackBufferFmt == FMT_ALL)
    {
        for (UINT nBB = 0; nBB < m_uCommonBackBufferFormats; nBB++)
        {
            // Filter out unsupported BB modes based on ModeOptions
            if (m_pD3D->GetVersion() < 8.f)
            {
                if (pMode->Format.d3dfFormat != m_pCommonBackBufferFormats[nBB].d3dfFormat)
                    continue;
            }
            else if (FAILED(m_pD3D->CheckDeviceType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format,
                m_pCommonBackBufferFormats[nBB], bWindowed)))
                continue;

            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, uVolume, uMS, 0, nBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.TextureFmt == FMT_ALL)
    {
        for (UINT nTexture = 0; nTexture < m_uCommonTextureFormats; nTexture++)
        {
            if (m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_A8R8G8B8 ||
                m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_DXT1)
            {
                if (!AddMode(pMode, bWindowed, nTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
            }
        }
    }

    //static const FMT formats[] = { FMT_A8R8G8B8, FMT_DXT1 };
    //
    //for (FMT fmt : formats)
    //{
    //    if (!AddMode(pMode, bWindowed, fmt, uZBuffer, uCube, uVolume, uMS, 0, uBB))
    //    {
    //        DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
    //        return false;
    //    }
    //}

    if (m_Options.ModeOptions.ZBufferFmt == FMT_ALL)
    {
        for (UINT nZBuffer = 0; nZBuffer < m_uCommonZBufferFormats; nZBuffer++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, nZBuffer, uCube, uVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.CubeFmt == FMT_ALL)
    {
        for (UINT nCube = 0; nCube < m_uCommonCubeFormats; nCube++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, nCube, uVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
    {
        for (UINT nVolume = 0; nVolume < m_uCommonVolumeFormats; nVolume++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, nVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.MultisampType == FMT_ALL)
    {
        DWORD dwMSQuality = 0;

        for (UINT nMS = 0; nMS < m_uCommonMultiSampleFormats; nMS++)
        {
            int nValidBB = -1, nValidZBuffer = -1;

            // For each MS type, find a BB and ZBuffer that support that type
            for (UINT nBB = 0; nBB < m_uCommonBackBufferFormats; nBB++)
            for (UINT nZBuffer = 0; nZBuffer < m_uCommonZBufferFormats; nZBuffer++)
            {
                // If specific modes are requested, only allow those modes to be matched.
                if (m_Options.ModeOptions.BackBufferFmt != FMT_ALL && m_Options.ModeOptions.BackBufferFmt != FMT_ANY &&
                    nBB != uBB)
                    continue;
                if (m_Options.ModeOptions.ZBufferFmt != FMT_ALL && m_Options.ModeOptions.ZBufferFmt != FMT_ANY &&
                    nZBuffer != uZBuffer)
                    continue;

                // Filter out unsupported BB modes based on ModeOptions
                if (SUCCEEDED(m_pD3D->CheckDeviceType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, m_pCommonBackBufferFormats[nBB], bWindowed)) &&
                    SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonBackBufferFormats[nBB], bWindowed, m_pCommonMultiSampleFormats[nMS], &dwMSQuality)) &&
                    SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonZBufferFormats[nZBuffer], bWindowed, m_pCommonMultiSampleFormats[nMS], &dwMSQuality)))
                {
                    nValidBB = nBB;
                    nValidZBuffer = nZBuffer;
                    break;
                }
            }

            if (nValidBB == -1 || nValidZBuffer == -1)
                continue;

            for (UINT nMSQ = 0; nMSQ < dwMSQuality; nMSQ++)
            {
                if (!AddMode(pMode, bWindowed, uTexture, nValidZBuffer, uCube, uVolume, nMS, nMSQ, nValidBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
            }
        }
    }

    return true;
}
#endif // FAST_TEST