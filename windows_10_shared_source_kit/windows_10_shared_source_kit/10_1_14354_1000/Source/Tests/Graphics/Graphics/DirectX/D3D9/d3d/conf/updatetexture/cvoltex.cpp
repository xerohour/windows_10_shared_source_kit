#include "CUpdateTex.h"
#include "TestCases.h"

// It's impossible to test Volume texture the same way as to test mipmap and cubemap
// because there's no Corresponding CopyRect here. So instead, this test is only
// to make sure the updatetexture can be called, the returned texture is not tested.


CVolTex::CVolTex(
	CUpdateTexBase *pTestApp,
	CDevice* pDevice,
	D3DFORMAT Format, 
	D3DPOOL Pool,
	UINT uSize,
	UINT cLevels
)
:
	CBaseTex(pTestApp, pDevice, D3DRTYPE_VOLUME),
	m_pTexture(NULL)
{
	FORMAT FrameworkFormat((FMT)Format);

	bool bCreateVolumeTextureResult = m_pTestApp -> CreateVolumeTexture(
		uSize,
		uSize,
		uSize,
		cLevels,
		0,
		FrameworkFormat,
		Pool,
		&m_pTexture);
}


BOOL CVolTex::ValidateTexture ()
{
	return (m_pTexture!=NULL)?TRUE:FALSE;
}


CVolTex::~CVolTex()
{
	SafeRelease (m_pTexture);
}


CVolTex:: operator CBaseTexture* ()
{
	return (CBaseTexture*) m_pTexture;
}


LPBASETEXTURES CVolTex::GetLPTEXTURES()
{
	return (LPBASETEXTURES)m_pTexture;
}


BOOL CVolTex::SetSrcTex(CBaseTex *pTex)
{	
	m_pSrcTexture = (static_cast <CVolTex *> (pTex))->m_pTexture;
	return TRUE;
}


//
//	Fill the texture with pattern depending on Value
//

BOOL CVolTex::SetTexValue (SURFACEVALUE Value)
{
	m_OriginalValue = Value;
	return TRUE;
}


BOOL CVolTex::LockWithNULL(DWORD flag)
{
	DPF(1, _T("LockBox 00 ...\n"));

	LOCKED_BOX LockedBox;
	bool bLockBoxResult = m_pTexture->LockBox(
		0,
		&LockedBox,
		NULL,
		flag
		);

	if (bLockBoxResult == true)
	{
		bool bUnlockBoxResult = m_pTexture->UnlockBox(0);
		return TRUE;
	}

	return FALSE;
}


BOOL CVolTex::LockManyAreas(UINT TotalBox)
{
	BOOL bRet = FALSE;
	D3DBOX Box;

	VOLUME_DESC Desc;
	bool bGetLevelDescriptorResult = m_pTexture ->GetLevelDesc (0,&Desc);
	if (bGetLevelDescriptorResult != true)
	{
		m_pTestApp->WriteToLog (TEXT("GetLevelDesc failed in CVolTex::LockManyAreas"));
		return FALSE;
	}


	assert(TotalBox <= TOTALBOXES);


	bRet = TRUE;
	for (UINT iBox = 0; iBox < TotalBox; iBox++)
	{
		if (bIsDXTnFormat((Desc.Format).d3dfFormat))
			Box = CDXTnMULTIBOXES[iBox];
		else
			Box = CMULTIBOXES[iBox];

		BOX FrameworkBox =
		{
			Box.Left,
			Box.Top,
			Box.Right,
			Box.Bottom,
			Box.Front,
			Box.Back
		};

		DPF(1, _T("LockBox 01 ...\n"));

		LOCKED_BOX LockedBox;
		bool bLockBoxResult = m_pTexture->LockBox(
			0,
			&LockedBox,
			&FrameworkBox,
			0
			);

		if (bLockBoxResult != true)
		{
			bRet = FALSE;
			break;
		}

		bool bUnlockBoxResult = m_pTexture->UnlockBox(0);
	}

	return bRet;
}


BOOL CVolTex::AddDirtyWithNULL()
{
	bool bAddDirtyBoxResult = m_pTexture->AddDirtyBox(NULL);

	if (bAddDirtyBoxResult == true)
		return TRUE;

	return FALSE;
}


BOOL CVolTex::AddDirtyManyAreas(UINT TotalBox)
{
	BOOL bRet = FALSE;
	D3DBOX Box;

	VOLUME_DESC Desc;
	bool bGetLevelDescriptorResult = m_pTexture ->GetLevelDesc(0, &Desc);
	if (bGetLevelDescriptorResult != true)
	{
		m_pTestApp->WriteToLog (TEXT("GetLevelDesc failed in CVolTex::AddDirtyManyAreas"));
		return FALSE;
	}


	assert(TotalBox <= TOTALBOXES);

	bRet = TRUE;
	for (UINT iBox = 0; iBox < TotalBox; iBox++)
	{
		if (bIsDXTnFormat((Desc.Format).d3dfFormat))
			Box = CDXTnMULTIBOXES[iBox];
		else
			Box = CMULTIBOXES[iBox];

		BOX FrameworkBox =
		{
			Box.Left,
			Box.Top,
			Box.Right,
			Box.Bottom,
			Box.Front,
			Box.Back
		};


		bool bAddDirtyBoxResult = m_pTexture->AddDirtyBox(&FrameworkBox);
		if (bAddDirtyBoxResult != true)
		{
			bRet = FALSE;
			break;
		}
	}

	return bRet;
}


BOOL CVolTex::LockSubLevel ()
{
	DWORD cLevels = m_pTexture->GetLevelCount();

	for (UINT iLevel = 1; iLevel < cLevels; iLevel++)
	{
		DPF(1, _T("LockBox 02 ...\n"));

		LOCKED_BOX LockedBox;
		bool bLockBoxResult = m_pTexture->LockBox(
			iLevel,
			&LockedBox,
			NULL,
			0
			);

		if (bLockBoxResult == true)
		{
			bool bUnlockBoxResult = m_pTexture->UnlockBox(iLevel);
		}
		else 
			return FALSE;
	}

	return TRUE;
}


BOOL CVolTex::MakeNonSquare()
{
	VOLUME_DESC Desc;
	bool bGetLevelDescriptorResult = m_pTexture ->GetLevelDesc(0, &Desc);
	if (bGetLevelDescriptorResult != true)
	{
		m_pTestApp->WriteToLog (TEXT("GetLevelDesc failed in CVolTex::MakeNonSquare"));
		return FALSE;
	}

	UINT cLevels = m_pTexture->GetLevelCount();

	SafeRelease(m_pTexture);
	
	bool bCreateVolumeTextureResult = m_pTestApp->CreateVolumeTexture(
		Desc.Width,
		Desc.Height,
		Desc.Depth>>1,
		cLevels,
		Desc.Usage,
		Desc.Format,
		Desc.Pool,
		&m_pTexture);

	if (bCreateVolumeTextureResult != true)
		return FALSE;

	SetTexValue(m_OriginalValue);

	return TRUE;
}


BOOL CVolTex::LockWithoutUnlock()
{
	DPF(1, _T("LockBox 03 ...\n"));

	LOCKED_BOX LockedBox;
	bool bLockBoxResult = m_pTexture->LockBox(
		0,
		&LockedBox,
		NULL,
		0
		);

	if (bLockBoxResult == true)
		return TRUE;

	return FALSE;
}


BOOL CVolTex::Unlock()
{
	bool bUnlockBoxResult = m_pTexture->UnlockBox(0);

	if (bUnlockBoxResult == true)
		return TRUE;

	return FALSE;
}


BOOL CVolTex::CopyRects (UINT TotalRects,D3DPOOL Pool)
{
	return TRUE;
}


HRESULT CVolTex::Render(UINT uiMipLevel)
{
	return D3D_OK;
}


void CVolTex::DirtyEntireTexture()
{
}
