#include "TexturesPCH.h"

//#define STRICT
#include "CTextures.h"

// --------------------------------------------------------------------------------------
// CnBaseTexture
// --------------------------------------------------------------------------------------
CnBaseTexture::CnBaseTexture()
{
	m_pHeadLock = NULL;
	m_uHeadLockSize = 0;
	m_ResourceType = (RESOURCETYPE)0;
	m_uDevices = 0;
	m_pTextures = NULL;
	m_pVolTextures = NULL;
	m_pCubeTextures = NULL;
}

// --------------------------------------------------------------------------------------

CnBaseTexture::~CnBaseTexture()
{

}

// --------------------------------------------------------------------------------------

bool CnBaseTexture::SkipDevice(UINT uDevice, DWORD dwFlags)
{
	DWORD dwMask = 1;

	for(int i=0; i<uDevice; i++)
		dwMask <<= 1;

	return (dwMask & dwFlags) == 0;
}

// --------------------------------------------------------------------------------------

bool CnBaseTexture::Init(UINT uDevices)
{
	m_uDevices = uDevices;

	if( GetType() == RTYPE_TEXTURE )
		m_pTextures = new CTexture*[uDevices];
	else if( GetType() == RTYPE_CUBETEXTURE )
		m_pCubeTextures = new CCubeTexture*[uDevices];
	else if( GetType() == RTYPE_VOLUMETEXTURE )
		m_pVolTextures = new CVolTexture*[uDevices];
	else
		return false;

	if( m_pTextures == NULL && m_pCubeTextures == NULL && m_pVolTextures == NULL)
		return false;

	if( GetType() == RTYPE_TEXTURE )
		memset( m_pTextures, 0, uDevices * sizeof( CTexture*) );
	else if( GetType() == RTYPE_VOLUMETEXTURE )
		memset( m_pVolTextures, 0, uDevices * sizeof( CVolTexture*) );
	else if( GetType() == RTYPE_CUBETEXTURE )
		memset( m_pCubeTextures, 0, uDevices * sizeof( CCubeTexture*) );
	else
		return false;

	m_pHeadLock = NULL;
	return true;
}


// --------------------------------------------------------------------------------------

void CnBaseTexture::GenerateMipSubLevels(DWORD dwFlags)
{
	if( GetType() == RTYPE_TEXTURE )
	{
		for(UINT i=0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwFlags ) )
				continue;

			if( m_pTextures[i] == NULL )
				continue;

			m_pTextures[i]->GenerateMipSubLevels();
		}
	}
	else if( GetType() == RTYPE_VOLUMETEXTURE )
	{
		for(UINT i=0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwFlags ) )
				continue;

			if( m_pVolTextures[i] == NULL )
				continue;

			m_pVolTextures[i]->GenerateMipSubLevels();
		}
	}
	else if( GetType() == RTYPE_CUBETEXTURE )
	{
		for(UINT i=0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwFlags ) )
				continue;

			if( m_pCubeTextures[i] == NULL )
				continue;

			m_pCubeTextures[i]->GenerateMipSubLevels();
		}
	}
}

// --------------------------------------------------------------------------------------

bool CnBaseTexture::SetAutoGenFilterType(DWORD FilterType, DWORD dwFlags)
{
	if( GetType() == RTYPE_TEXTURE )
	{
		for(UINT i=0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwFlags ) )
				continue;

			if( m_pTextures[i] == NULL )
				continue;

			return SUCCEEDED(m_pTextures[i]->SetAutoGenFilterType(FilterType));
		}
	}
	else if( GetType() == RTYPE_VOLUMETEXTURE )
	{
		for(UINT i=0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwFlags ) )
				continue;

			if( m_pVolTextures[i] == NULL )
				continue;

			return SUCCEEDED(m_pVolTextures[i]->SetAutoGenFilterType(FilterType));
		}
	}
	else if( GetType() == RTYPE_CUBETEXTURE )
	{
		for(UINT i=0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwFlags ) )
				continue;

			if( m_pCubeTextures[i] == NULL )
				continue;

			return SUCCEEDED(m_pCubeTextures[i]->SetAutoGenFilterType(FilterType));
		}
	}
	return false;
}

// --------------------------------------------------------------------------------------

DWORD CnBaseTexture::GetAutoGenFilterType(DWORD dwFlags)
{
	if( GetType() == RTYPE_TEXTURE )
	{
		for(UINT i=0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwFlags ) )
				continue;

			if( m_pTextures[i] == NULL )
				continue;

			return m_pTextures[i]->GetAutoGenFilterType();
		}
	}
	else if( GetType() == RTYPE_VOLUMETEXTURE )
	{
		for(UINT i=0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwFlags ) )
				continue;

			if( m_pVolTextures[i] == NULL )
				continue;

			return m_pVolTextures[i]->GetAutoGenFilterType();
		}
	}
	else if( GetType() == RTYPE_CUBETEXTURE )
	{
		for(UINT i=0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwFlags ) )
				continue;

			if( m_pCubeTextures[i] == NULL )
				continue;

			return m_pCubeTextures[i]->GetAutoGenFilterType();
		}
	}
	return 0;
}

// --------------------------------------------------------------------------------------

bool CnBaseTexture::GetBaseTexture(CBaseTexture **ppTexture, UINT n)
{
	if( n >= m_uDevices || ppTexture == NULL )
		return false;
	
	if( GetType() == RTYPE_TEXTURE )
	{
		m_pTextures[n]->AddRef();
		*ppTexture = (LPBASETEXTURE)m_pTextures[n];
	}
	else if( GetType() == RTYPE_VOLUMETEXTURE )
	{
		m_pVolTextures[n]->AddRef();
		*ppTexture = (LPBASETEXTURE)m_pVolTextures[n];
	}
	else if( GetType() == RTYPE_CUBETEXTURE )
	{
		m_pCubeTextures[n]->AddRef();
		*ppTexture = (LPBASETEXTURE)m_pCubeTextures[n];
	}
	else
		return false;

	return true;
}

// --------------------------------------------------------------------------------------

bool CnBaseTexture::SetBaseTexture(CBaseTexture *pTexture, UINT n)
{
	if( n >= m_uDevices )
		return false;


	if( pTexture->GetType() != GetType() )
		return false;

	if( GetType() == RTYPE_TEXTURE )
	{
		// old texture must be properly RELEASEd before taking a new one!
		if( m_pTextures[n] != NULL )
			return false;

		m_pTextures[n] = (CTexture*)pTexture;
	}
	else if( GetType() == RTYPE_VOLUMETEXTURE )
	{
		if( m_pVolTextures[n] != NULL )
			return false;
		m_pVolTextures[n] = (CVolTexture*)pTexture;
	}
	else if( GetType() == RTYPE_CUBETEXTURE )
	{
		if( m_pCubeTextures[n] != NULL )
			return false;
		m_pCubeTextures[n] = (CCubeTexture*)pTexture;
	}
	else
		return false;

	return true;
}

// --------------------------------------------------------------------------------------

LOCK_LIST**	CnBaseTexture::GetHead(UINT uLevel)
{
	UINT uGrowSize = 8;

	if( uLevel >= m_uHeadLockSize )
	{
		uGrowSize = (uLevel >= m_uHeadLockSize + uGrowSize) ? (uLevel - m_uHeadLockSize + 1) : uGrowSize;
		LOCK_LIST** pNewList;
		pNewList = new LOCK_LIST*[ m_uHeadLockSize + uGrowSize];
		
		// just AV right here if this alloc failed, no way for caller to recover anyways
		memset( pNewList, 0, sizeof(LOCK_LIST*) * (m_uHeadLockSize + uGrowSize) );

		if( m_pHeadLock )
		{
			memcpy( pNewList, m_pHeadLock, m_uHeadLockSize * sizeof(LOCK_LIST*) );
			delete[] m_pHeadLock;
		}
	
		m_pHeadLock = pNewList;
		m_uHeadLockSize += uGrowSize;
	}

	return (m_pHeadLock + uLevel);
}

// --------------------------------------------------------------------------------------
// CnTexture
// --------------------------------------------------------------------------------------

CnTexture::~CnTexture()
{
	UINT i;
	for( i= 0; i<m_uHeadLockSize; i++)
	{
		if( *(GetHead(i)) != NULL)
			DPF(1, "Orphaned lock record found.");
	}
	SAFEDELETEA(m_pHeadLock);
	
	if(m_pTextures)
	{
		for( i= 0; i< m_uDevices; i++ )
		{
			RELEASE( m_pTextures[i] );
		}

		SAFEDELETEA(m_pTextures);
	}
}

// --------------------------------------------------------------------------------------

bool CnTexture::GetTexture(CTexture **ppTexture, UINT n)
{
	return GetBaseTexture( (CBaseTexture**)ppTexture, n);
}

// --------------------------------------------------------------------------------------

bool CnTexture::SetTexture(CTexture *pTexture, UINT n)
{
	return SetBaseTexture( (CBaseTexture*)pTexture, n);
}

// --------------------------------------------------------------------------------------

bool CnTexture::LoadTexture(UINT uLevel, LPCTSTR pFilename, float fAlphaOverride, DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pTextures[i] == NULL)
			continue;

		if(FAILED(m_pTextures[i]->LoadTexture(uLevel, pFilename, fAlphaOverride, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnTexture::LoadStripes(UINT uLevel, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal, DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pTextures[i] == NULL)
			continue;

		if(FAILED(m_pTextures[i]->LoadStripes(uLevel, dwW, dwH, dwStripes, dwColorArray, bHorizontal, bDiagonal, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnTexture::LoadGradient(UINT uLevel, DWORD dwW,DWORD dwH, DWORD dwColors[4], DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pTextures[i] == NULL)
			continue;

		if(FAILED(m_pTextures[i]->LoadGradient(uLevel, dwW, dwH, dwColors, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnTexture::LoadImage(UINT uLevel, CImage **pImg, DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pTextures[i] == NULL)
			continue;

		if(FAILED(m_pTextures[i]->LoadImage(uLevel, pImg, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnTexture::LoadImage(UINT uLevel, CImage *pImg, DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pTextures[i] == NULL)
			continue;

		if(FAILED(m_pTextures[i]->LoadImage(uLevel, pImg, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

DWORD CnTexture::GetLevelCount(DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pTextures[i] == NULL )
			continue;

		return m_pTextures[i]->GetLevelCount();
	}

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

void CnTexture::PreLoad(DWORD dwFlags)
{
	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pTextures[i] == NULL)
			continue;

		m_pTextures[i]->PreLoad();
	}	
}

// --------------------------------------------------------------------------------------

DWORD CnTexture::SetLOD(DWORD dwLODNew, DWORD dwFlags)
{
	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pTextures[i] == NULL)
			continue;

		m_pTextures[i]->SetLOD(dwLODNew);
	}	

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

DWORD CnTexture::GetLOD(DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pTextures[i] == NULL )
			continue;

		return m_pTextures[i]->GetLOD();
	}

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

DWORD CnTexture::SetPriority(DWORD dwNewPriority, DWORD dwFlags)
{
	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pTextures[i] == NULL)
			continue;

		m_pTextures[i]->SetPriority(dwNewPriority);
	}	

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

DWORD CnTexture::GetPriority(DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pTextures[i] == NULL )
			continue;

		return m_pTextures[i]->GetPriority();
	}

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

bool CnTexture::GetLevelDesc(UINT uLevel, PSURFACEDESC pDesc, DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pTextures[i] == NULL )
			continue;

		return SUCCEEDED(m_pTextures[i]->GetLevelDesc(uLevel,pDesc));
	}
	return false;
}

// --------------------------------------------------------------------------------------

bool CnTexture::GetSurfaceLevel(UINT uLevel, LPSURFACES* ppSurfaceLevel, DWORD dwFlags)
{
	bool bRes=true;

	if( ppSurfaceLevel == NULL )
	{
		DPF(1, _T("CnTexture:GetSurfaceLevel invalid parameter.\n") );
		return false;
	}

	LPSURFACES pSurfaces= new CnSurface;
	if(  pSurfaces == NULL )
	{
		DPF(1, _T("CnTexture:GetSurfaceLevel out of memory error.\n") );
		return false;
	}

	if(false == pSurfaces->Init( m_uDevices ) )
	{
		DPF(1, _T("CnTexture:GetSurfaceLevel out of memory error\n") );
		return false;
	}

	LPSURFACE pSurface;
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pTextures[i] == NULL )
			continue;
		
		if( FAILED (m_pTextures[i]->GetSurfaceLevel(uLevel, &pSurface)) )
		{
			DPF(1, _T("CnTexture:GetSurfaceLevel GetSurfaceLevel failed\n") );
			bRes = false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
		RELEASE( pSurfaces );
	
	if( bRes )
		*ppSurfaceLevel = pSurfaces;
	
	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnTexture::AddDirtyRect(CONST RECT* pDirtyRect, DWORD dwFlags)
{
	bool bRes = true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pTextures[i] == NULL)
			continue;

		bRes &= SUCCEEDED(m_pTextures[i]->AddDirtyRect(pDirtyRect));
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

// obviously not thread safe
bool CnTexture::LockRect(UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect,DWORD Flags, DWORD dwFlags)
{
	bool bRes = true;
	DWORD Mask = 1;
	DWORD dwLockedFlags = 0;
	HRESULT hr;
	UINT uFirstFound = (UINT)-1;
	
	LOCKED_RECT* pLockedRects = new LOCKED_RECT[m_uDevices];
	RECT*		 pRects = new RECT[m_uDevices];
	BOOL*		 bNullRect = new BOOL[m_uDevices];  // deferenced as an array, doesn't look "right" as pbNullRect
	BOOL*		 bReadOnly = new BOOL[m_uDevices];  // different array, same deal

	if( pLockedRects == NULL || pRects == NULL || bReadOnly == NULL || bNullRect == NULL)
	{
		DPF(1, "CnTexture::LockRect out of memory error." );
		if( pLockedRects )
			delete[] pLockedRects;
		if( pRects )
			delete[] pRects;
		if( bNullRect )
			delete[] bNullRect;
		if( bReadOnly )
			delete[] bReadOnly;
		return false;
	}

	memset( pLockedRects, 0, sizeof(LOCKED_RECT) * m_uDevices );
	memset( pRects, 0, sizeof(RECT) * m_uDevices );
	memset( bNullRect, 0, sizeof(BOOL) * m_uDevices );
	memset( bReadOnly, 0, sizeof(BOOL) * m_uDevices );

	LOCKED_RECT rcTempRect;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
		{
			Mask <<= 1;
			continue;
		}

		if(m_pTextures[i] == NULL)
		{
			Mask <<= 1;
			continue;
		}

		bReadOnly[i] = (Flags & LOCK_READONLY) ? TRUE : FALSE;
		bNullRect[i] = (pRect == NULL) ? TRUE : FALSE;
		if( pRect != NULL )
			memcpy( pRects + i, pRect, sizeof(RECT));

		hr =  m_pTextures[i]->LockRect(uLevel,&rcTempRect,pRect,Flags);
		if( FAILED( hr ) )
		{
			bRes = false;
			break;
		}
		else
		{
			if( uFirstFound == (UINT)-1 )
				uFirstFound = i;
			dwLockedFlags |= Mask;
			memcpy( pLockedRects + i, &rcTempRect, sizeof(LOCKED_RECT) );
		}

		Mask <<= 1;
	}	

	LOCK_LIST* pThis = new LOCK_LIST;

	if( !bRes || pThis == NULL)
	{
		if( pThis != NULL)
			delete pThis;
		
		DPF(1, "CnTexture::LockRect() failed.\n" );

		// unlock everything that was locked, no way to unlock later
		for( UINT i= 0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwLockedFlags ) )
				continue;
			hr =  m_pTextures[i]->UnlockRect(uLevel);
		}
		delete[] pLockedRects;
		delete[] pRects;
		delete[] bReadOnly;
		delete[] bNullRect;
		return bRes;
	}

	memset( pThis, 0, sizeof(LOCK_LIST) );
	pThis->uFirstFound = uFirstFound;
	pThis->pRects = pRects;
	pThis->pLockedRects = pLockedRects;
	pThis->bReadOnly = bReadOnly;
	pThis->bNullRect = bNullRect;
	pThis->pNext = *(GetHead(uLevel));
	*(GetHead(uLevel)) = pThis;
	
	memcpy(pLockedRect, pLockedRects + uFirstFound, sizeof( LOCKED_RECT ) );

	return true;
}

// --------------------------------------------------------------------------------------

// obviously not thread safe
bool CnTexture::UnlockRect(UINT uLevel, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr;
	UINT uFirstScanLineWidth = (UINT)-1;
	UINT uScanLineWidth;

	// Need format- and surface dimension details.
	SURFACEDESC sdDesc;
	GetLevelDesc(uLevel,&sdDesc);

	// locate the record for the equivalent lock call
	LOCK_LIST* pThis = *(GetHead(uLevel));
	if( pThis == NULL )
	{
		DPF(1, _T("CnTexture:UnlockRect - no associated Lock call found!\n") );
		return false;
	}

	// pop it off the list
	*(GetHead(uLevel)) = pThis->pNext;

	// locate the LOCKED_RECT structs of the devices for this texture level
	LOCKED_RECT* pLockedRects = pThis->pLockedRects;
	RECT*		 rcRects = pThis->pRects;
	BOOL*		 bNullRect = pThis->bNullRect;
	BOOL*		 bReadOnly = pThis->bReadOnly;
	
	// loop through each 
	// the significance of the "uFirstFound" member is that it corresponds to the device for which
	// the origional LockRect provided a pointer for.  If we copy data across devices we must use
	// the original pointer given to the caller.
	for(UINT i= pThis->uFirstFound; i<m_uDevices; i++)
	{
		RECT rc;
		memset( &rc, 0, sizeof(rc) );
	
		// skip levels not specified by the caller
		if( SkipDevice( i, dwFlags ) )
			continue;

		// avoid this show stopper
		if(m_pTextures[i] == NULL)
			continue;

		// if null was specified previously, let's copy the entire texture level
		if( pThis->bNullRect[i] )
			SetRect( &rc, 0, 0, (INT)sdDesc.dwWidth, (INT)sdDesc.dwHeight );		
		else
			memcpy( &rc, pThis->pRects + i, sizeof( RECT ) );
	
		// will need scanline width to determine how many bytes to memcpy
		uScanLineWidth = (sdDesc.Format.ddpfPixelFormat.dwRGBBitCount * (rc.right - rc.left))>>3;

		// if this isn't the first level locked, copy the contents from the first one.
		if( i != pThis->uFirstFound )
		{
			if( (uFirstScanLineWidth == uScanLineWidth) && !(bReadOnly[i]) )
			{
				UINT uiNoOfScanLines = (rc.bottom-rc.top) / sdDesc.Format.ddpfPixelFormat.dwRowsPerByteSequence;
				for( UINT uScanLine= 0; uScanLine<uiNoOfScanLines; uScanLine++ )
				{
					memcpy( ((BYTE*)(pLockedRects[i].pBits))+((UINT_PTR)uScanLine*(pLockedRects[i].iPitch)),
						((BYTE*)(pLockedRects[pThis->uFirstFound].pBits))+((UINT_PTR)uScanLine * pLockedRects[pThis->uFirstFound].iPitch),
						uScanLineWidth); 
				}
			}
		}
		else
		{
			// record this scanline width to compare against later levels
			uFirstScanLineWidth = uScanLineWidth;
		}
	}

	// only now it is safe to UnlockRect!!!
	// attempting to UnlockRect and THEN use the pBits pointer may lead to memory corruption!!!
	for(UINT i= pThis->uFirstFound; i<m_uDevices; i++)
	{
		// skip levels not specified by the caller
		if( SkipDevice( i, dwFlags ) )
			continue;

		// avoid this show stopper
		if( m_pTextures[i] == NULL )
			continue;

		// wow, now we can unlock it!
		hr = m_pTextures[i]->UnlockRect(uLevel);

		if( FAILED( hr ) )
			bRes = false;
	}
	
	//  delete the array of LOCKED_RECT structs
	delete[] pLockedRects;
	delete[] rcRects;
	delete[] bNullRect;
	delete[] bReadOnly;

	//  delete this LOCK_RECT structure we removed from the list earlier
	delete   pThis;

	if( !bRes)
	{
		// for now, don't even bother handling a failed unlock.
		// all devices MUST be kept in sync so should we re-LockRect the area?
		// most likely failure is surface wasn't locked, anyways!
		DPF(1, _T("CnTexture:UnlockRect UnlockRect failed!\n") );
		return bRes;
	}

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnTexture::IsPalettized(DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pTextures[i] == NULL )
			continue;

		return m_pTextures[i]->IsPalettized();
	}

	return false;
}

// --------------------------------------------------------------------------------------

bool CnTexture::SetPalette(PALETTEENTRY *pPalette, DWORD dwFlags)
{
	bool bRes = true;
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pTextures[i] == NULL )
			continue;

		if(FAILED(m_pTextures[i]->SetPalette(pPalette)))
			bRes &= false;
	}

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnTexture::SetDDrawPalette(LPDIRECTDRAWPALETTE pPalette, DWORD dwFlags)
{
	bool bRes = true;
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pTextures[i] == NULL )
			continue;

		if(FAILED(m_pTextures[i]->SetDDrawPalette(pPalette)))
			bRes &= false;
	}

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnTexture::SetColorKey(DWORD dwColorKeyFlags, DWORD *pdwColorKey, DWORD dwFlags)
{
	bool bRes = true;
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pTextures[i] == NULL )
			continue;

		if(FAILED(m_pTextures[i]->SetColorKey(dwColorKeyFlags, pdwColorKey)))
			bRes &= false;
	}

	return bRes;
}

// --------------------------------------------------------------------------------------
// CnCubeTexture
// --------------------------------------------------------------------------------------

CnCubeTexture::~CnCubeTexture()
{
	UINT i;
	for( i= 0; i<m_uHeadLockSize; i++)
	{
		if( *(GetHead(i)) != NULL)
			DPF(1, "Orphaned lock record found.");
	}
	SAFEDELETEA(m_pHeadLock);
	
	if(m_pCubeTextures)
	{
		for( i= 0; i< m_uDevices; i++ )
		{
			RELEASE( m_pCubeTextures[i] );
		}

		SAFEDELETEA(m_pCubeTextures);
	}
}

// --------------------------------------------------------------------------------------

bool CnCubeTexture::GetCubeTexture(CCubeTexture **ppCubeTexture, UINT n)
{
	return GetBaseTexture( (CBaseTexture**)ppCubeTexture, n);
}

// --------------------------------------------------------------------------------------

bool CnCubeTexture::SetCubeTexture(CCubeTexture *pCubeTexture, UINT n)
{
	return SetBaseTexture( (CBaseTexture*)pCubeTexture, n);
}

// --------------------------------------------------------------------------------------

DWORD CnCubeTexture::GetLevelCount(DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pCubeTextures[i] == NULL )
			continue;

		return m_pCubeTextures[i]->GetLevelCount();
	}

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

void CnCubeTexture::PreLoad(DWORD dwFlags)
{
	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pCubeTextures[i] == NULL)
			continue;

		m_pCubeTextures[i]->PreLoad();
	}	
}

// --------------------------------------------------------------------------------------

DWORD CnCubeTexture::SetLOD(DWORD dwLODNew, DWORD dwFlags)
{
	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pCubeTextures[i] == NULL)
			continue;

		m_pCubeTextures[i]->SetLOD(dwLODNew);
	}	

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

DWORD CnCubeTexture::GetLOD(DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pCubeTextures[i] == NULL )
			continue;

		return m_pCubeTextures[i]->GetLOD();
	}

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

DWORD CnCubeTexture::SetPriority(DWORD dwNewPriority, DWORD dwFlags)
{
	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pCubeTextures[i] == NULL)
			continue;

		m_pCubeTextures[i]->SetPriority(dwNewPriority);
	}	

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

DWORD CnCubeTexture::GetPriority(DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pCubeTextures[i] == NULL )
			continue;

		return m_pCubeTextures[i]->GetPriority();
	}

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

bool CnCubeTexture::GetLevelDesc(UINT uLevel, PSURFACEDESC pDesc, DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pCubeTextures[i] == NULL )
			continue;

		return SUCCEEDED(m_pCubeTextures[i]->GetLevelDesc(uLevel,pDesc));
	}
	return false;
}

// --------------------------------------------------------------------------------------

bool CnCubeTexture::GetCubeMapSurface(CUBEMAP_FACES FaceType, UINT uLevel, LPSURFACES* ppSurfaceLevel, DWORD dwFlags)
{
	bool bRes=true;

	if( ppSurfaceLevel == NULL )
	{
		DPF(1, _T("CnCubeTexture:GetSurfaceLevel invalid parameter.\n") );
		return false;
	}

	LPSURFACES pSurfaces= new CnSurface;
	if(  pSurfaces == NULL )
	{
		DPF(1, _T("CnCubeTexture:GetSurfaceLevel out of memory error.\n") );
		return false;
	}

	if(false == pSurfaces->Init( m_uDevices ) )
	{
		DPF(1, _T("CnCubeTexture:GetSurfaceLevel out of memory error\n") );
		return false;
	}

	LPSURFACE pSurface;
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pCubeTextures[i] == NULL )
			continue;
		
		if( FAILED (m_pCubeTextures[i]->GetCubeMapSurface(FaceType, uLevel, &pSurface)) )
		{
			DPF(1, _T("CnCubeTexture:GetCubeMapSurface GetCubeMapSurface failed\n") );
			bRes = false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
		RELEASE( pSurfaces );
	
	if( bRes )
		*ppSurfaceLevel = pSurfaces;
	
	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnCubeTexture::AddDirtyRect(CUBEMAP_FACES FaceType, CONST RECT* pDirtyRect, DWORD dwFlags)
{
	bool bRes = true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pCubeTextures[i] == NULL)
			continue;

		bRes &= SUCCEEDED(m_pCubeTextures[i]->AddDirtyRect(FaceType, pDirtyRect));
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

// obviously not thread safe
bool CnCubeTexture::LockRect(CUBEMAP_FACES FaceType, UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect,DWORD Flags, DWORD dwFlags)
{
	bool bRes = true;
	DWORD Mask = 1;
	DWORD dwLockedFlags = 0;
	HRESULT hr;
	UINT uFirstFound = (UINT)-1;
	
	LOCKED_RECT* pLockedRects = new LOCKED_RECT[m_uDevices];
	RECT*		 pRects = new RECT[m_uDevices];
	BOOL*		 bNullRect = new BOOL[m_uDevices];  // deferenced as an array, doesn't look "right" as pbNullRect
	BOOL*		 bReadOnly = new BOOL[m_uDevices];  // different array, same deal

	if( pLockedRects == NULL || pRects == NULL || bReadOnly == NULL || bNullRect == NULL)
	{
		DPF(1, "CnCubeTexture::LockRect out of memory error." );
		if( pLockedRects )
			delete[] pLockedRects;
		if( pRects )
			delete[] pRects;
		if( bNullRect )
			delete[] bNullRect;
		if( bReadOnly )
			delete[] bReadOnly;
		return false;
	}

	memset( pLockedRects, 0, sizeof(LOCKED_RECT) * m_uDevices );
	memset( pRects, 0, sizeof(RECT) * m_uDevices );
	memset( bNullRect, 0, sizeof(BOOL) * m_uDevices );
	memset( bReadOnly, 0, sizeof(BOOL) * m_uDevices );

	LOCKED_RECT rcTempRect;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
		{
			Mask <<= 1;
			continue;
		}

		if(m_pCubeTextures[i] == NULL)
		{
			Mask <<= 1;
			continue;
		}

		bReadOnly[i] = (Flags & LOCK_READONLY) ? TRUE : FALSE;
		bNullRect[i] = (pRect == NULL) ? TRUE : FALSE;
		if( pRect != NULL )
			memcpy( pRects + i, pRect, sizeof(RECT));

		hr =  m_pCubeTextures[i]->LockRect(FaceType,uLevel,&rcTempRect,pRect,Flags);
		if( FAILED( hr ) )
		{
			bRes = false;
			break;
		}
		else
		{
			if( uFirstFound == (UINT)-1 )
				uFirstFound = i;
			dwLockedFlags |= Mask;
			memcpy( pLockedRects + i, &rcTempRect, sizeof(LOCKED_RECT) );
		}

		Mask <<= 1;
	}	

	LOCK_LIST* pThis = new LOCK_LIST;

	if( !bRes || pThis == NULL)
	{
		if( pThis != NULL)
			delete pThis;
		
		DPF(1, "CnCubeTexture::LockRect() failed." );

		// unlock everything that was locked, no way to unlock later
		for( UINT i= 0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwLockedFlags ) )
				continue;
			hr =  m_pCubeTextures[i]->UnlockRect(FaceType, uLevel);
		}
		delete[] pLockedRects;
		delete[] pRects;
		delete[] bReadOnly;
		delete[] bNullRect;
		return bRes;
	}

	memset( pThis, 0, sizeof(LOCK_LIST) );
	pThis->uFirstFound = uFirstFound;
	pThis->pRects = pRects;
	pThis->pLockedRects = pLockedRects;
	pThis->bReadOnly = bReadOnly;
	pThis->bNullRect = bNullRect;
	pThis->pNext = *(GetHead(uLevel));
	*(GetHead(uLevel)) = pThis;
	
	memcpy(pLockedRect, pLockedRects + uFirstFound, sizeof( LOCKED_RECT ) );

	return true;
}

// --------------------------------------------------------------------------------------

// obviously not thread safe
bool CnCubeTexture::UnlockRect(CUBEMAP_FACES FaceType, UINT uLevel, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr;
	UINT uFirstScanLineWidth = (UINT)-1;
	UINT uScanLineWidth;

	// Need format- and surface dimension details.
	SURFACEDESC sdDesc;
	GetLevelDesc(uLevel,&sdDesc);

	// locate the record for the equivalent lock call
	LOCK_LIST* pThis = *(GetHead(uLevel));
	if( pThis == NULL )
	{
		DPF(1, _T("CnCubeTexture:UnlockRect - no associated Lock call found!\n") );
		return false;
	}

	// pop it off the list
	*(GetHead(uLevel)) = pThis->pNext;

	// locate the LOCKED_RECT structs of the devices for this texture level
	LOCKED_RECT* pLockedRects = pThis->pLockedRects;
	RECT*		 rcRects = pThis->pRects;
	BOOL*		 bNullRect = pThis->bNullRect;
	BOOL*		 bReadOnly = pThis->bReadOnly;
	
	// loop through each 
	// the significance of the "uFirstFound" member is that it corresponds to the device for which
	// the origional LockRect provided a pointer for.  If we copy data across devices we must use
	// the original pointer given to the caller.
	for(UINT i= pThis->uFirstFound; i<m_uDevices; i++)
	{
		RECT rc;
		memset( &rc, 0, sizeof(rc) );
	
		// skip levels not specified by the caller
		if( SkipDevice( i, dwFlags ) )
			continue;

		// avoid this show stopper
		if(m_pCubeTextures[i] == NULL)
			continue;
		
		// if null was specified previously, let's copy the entire texture level
		if( pThis->bNullRect[i] )
			SetRect( &rc, 0, 0, (INT)sdDesc.dwWidth, (INT)sdDesc.dwHeight );		
		else
			memcpy( &rc, pThis->pRects + i, sizeof( RECT ) );
	
		// will need scanline width to determine how many bytes to memcpy
		uScanLineWidth = (sdDesc.Format.ddpfPixelFormat.dwRGBBitCount * (rc.right - rc.left))>>3;

		// if this isn't the first level locked, copy the contents from the first one.
		if( i != pThis->uFirstFound )
		{
			if( (uFirstScanLineWidth == uScanLineWidth) && !(bReadOnly[i]) )
			{
				UINT uiNoOfScanLines = (rc.bottom-rc.top) / sdDesc.Format.ddpfPixelFormat.dwRowsPerByteSequence;
				for( UINT uScanLine= 0; uScanLine<uiNoOfScanLines; uScanLine++ )
				{
					memcpy( ((BYTE*)(pLockedRects[i].pBits))+((UINT_PTR)uScanLine*(pLockedRects[i].iPitch)),
						((BYTE*)(pLockedRects[pThis->uFirstFound].pBits))+((UINT_PTR)uScanLine * pLockedRects[pThis->uFirstFound].iPitch),
						uScanLineWidth); 
				}
			}
		}
		else
		{
			// record this scanline width to compare against later levels
			uFirstScanLineWidth = uScanLineWidth;
		}

		// wow, now we can unlock it!
		hr =  m_pCubeTextures[i]->UnlockRect(FaceType, uLevel);

		if( FAILED( hr ) )
			bRes = false;
	}	
	
	//  delete the array of LOCKED_RECT structs
	delete[] pLockedRects;
	delete[] rcRects;
	delete[] bNullRect;
	delete[] bReadOnly;

	//  delete this LOCK_RECT structure we removed from the list earlier
	delete   pThis;

	if( !bRes)
	{
		// for now, don't even bother handling a failed unlock.
		// all devices MUST be kept in sync so should we re-LockRect the area?
		// most likely failure is surface wasn't locked, anyways!
		DPF(1, _T("CnCubeTexture:UnlockRect UnlockRect failed!\n") );
		return bRes;
	}

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnCubeTexture::LoadImage(UINT uLevel, CImage **pImg, DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pCubeTextures[i] == NULL)
			continue;

		if(FAILED(m_pCubeTextures[i]->LoadImage(uLevel, pImg, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnCubeTexture::LoadImage(CImage ***pImg, DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pCubeTextures[i] == NULL)
			continue;

		if(FAILED(m_pCubeTextures[i]->LoadImage(pImg, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnCubeTexture::IsPalettized(DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pCubeTextures[i] == NULL )
			continue;

		return m_pCubeTextures[i]->IsPalettized();
	}

	return false;
}

// --------------------------------------------------------------------------------------

bool CnCubeTexture::SetPalette(PALETTEENTRY *pPalette, DWORD dwFlags)
{
	bool bRes = true;
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pCubeTextures[i] == NULL )
			continue;

		if(FAILED(m_pCubeTextures[i]->SetPalette(pPalette)))
			bRes &= false;
	}

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnCubeTexture::LoadTexture(UINT uLevel, CUBEMAP_FACES Face, LPCTSTR pFilename, float fAlphaOverride, DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pCubeTextures[i] == NULL)
			continue;

		if(FAILED(m_pCubeTextures[i]->LoadTexture(uLevel, Face, pFilename, fAlphaOverride, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnCubeTexture::LoadStripes(UINT uLevel, CUBEMAP_FACES Face, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal, DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pCubeTextures[i] == NULL)
			continue;

		if(FAILED(m_pCubeTextures[i]->LoadStripes(uLevel, Face, dwW, dwH, dwStripes, dwColorArray, bHorizontal, bDiagonal, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnCubeTexture::LoadGradient(UINT uLevel, CUBEMAP_FACES Face, DWORD dwW,DWORD dwH,DWORD dwColors[4], DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pCubeTextures[i] == NULL)
			continue;

		if(FAILED(m_pCubeTextures[i]->LoadGradient(uLevel, Face, dwW, dwH, dwColors, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------
// CnVolTexture
// --------------------------------------------------------------------------------------


CnVolTexture::~CnVolTexture()
{
	UINT i;
	for( i= 0; i<m_uHeadLockSize; i++)
	{
		if( *(GetHead(i)) != NULL)
			DPF(1, "Orphaned lock record found.");
	}
	SAFEDELETEA(m_pHeadLock);
	
	if(m_pVolTextures)
	{
		for( i= 0; i< m_uDevices; i++ )
		{
			RELEASE( m_pVolTextures[i] );
		}

		SAFEDELETEA(m_pVolTextures);
	}
}

// --------------------------------------------------------------------------------------

bool CnVolTexture::GetVolumeTexture(CVolTexture **ppVolumeTexture, UINT n)
{
	return GetBaseTexture( (CBaseTexture**)ppVolumeTexture, n);
}

// --------------------------------------------------------------------------------------

bool CnVolTexture::SetVolumeTexture(CVolTexture *pVolumeTexture, UINT n)
{
	return SetBaseTexture( (CBaseTexture*)pVolumeTexture, n);
}

// --------------------------------------------------------------------------------------

DWORD CnVolTexture::GetLevelCount(DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pVolTextures[i] == NULL )
			continue;

		return m_pVolTextures[i]->GetLevelCount();
	}

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

void CnVolTexture::PreLoad(DWORD dwFlags)
{
	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pVolTextures[i] == NULL)
			continue;

		m_pVolTextures[i]->PreLoad();
	}	
}

// --------------------------------------------------------------------------------------

DWORD CnVolTexture::SetLOD(DWORD dwLODNew, DWORD dwFlags)
{
	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pVolTextures[i] == NULL)
			continue;

		m_pVolTextures[i]->SetLOD(dwLODNew);
	}	

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

DWORD CnVolTexture::GetLOD(DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pVolTextures[i] == NULL )
			continue;

		return m_pVolTextures[i]->GetLOD();
	}

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

DWORD CnVolTexture::SetPriority(DWORD dwNewPriority, DWORD dwFlags)
{
	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pVolTextures[i] == NULL)
			continue;

		m_pVolTextures[i]->SetPriority(dwNewPriority);
	}	

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

DWORD CnVolTexture::GetPriority(DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pVolTextures[i] == NULL )
			continue;

		return m_pVolTextures[i]->GetPriority();
	}

	return (DWORD)0;
}

// --------------------------------------------------------------------------------------

bool CnVolTexture::GetLevelDesc(UINT uLevel, LPVOLUME_DESC pDesc, DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pVolTextures[i] == NULL )
			continue;

		return SUCCEEDED(m_pVolTextures[i]->GetLevelDesc(uLevel,pDesc));
	}
	return false;
}

// --------------------------------------------------------------------------------------

bool CnVolTexture::GetVolumeLevel(UINT uLevel, LPVOLUME* ppVolume, DWORD dwFlags)
{
	bool bRes=true;

/*	if( ppSurfaceLevel == NULL )
	{
		DPF(1, _T("CnVolTexture:GetSurfaceLevel invalid parameter.\n") );
		return false;
	}

	LPSURFACES pSurfaces= new CnSurface;
	if(  pSurfaces == NULL )
	{
		DPF(1, _T("CnVolTexture:GetSurfaceLevel out of memory error.\n") );
		return false;
	}

	if(false == pSurfaces->Init( m_uDevices ) )
	{
		DPF(1, _T("CnVolTexture:GetSurfaceLevel out of memory error\n") );
		return false;
	}

	LPSURFACE pSurface;
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pVolTextures[i] == NULL )
			continue;
		
		if( FAILED (m_pVolTextures[i]->GetCubeMapSurface(FaceType, uLevel, &pSurface)) )
		{
			DPF(1, _T("CnVolTexture:GetCubeMapSurface GetCubeMapSurface failed\n") );
			bRes = false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
		RELEASE( pSurfaces );
	
	if( bRes )
		*ppSurfaceLevel = pSurfaces;
*/	
	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnVolTexture::AddDirtyBox(CONST BOX* pDirtyBox, DWORD dwFlags)
{
	bool bRes = true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pVolTextures[i] == NULL)
			continue;

		bRes &= SUCCEEDED(m_pVolTextures[i]->AddDirtyBox(pDirtyBox));
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

// obviously not thread safe
bool CnVolTexture::LockBox(UINT uLevel, LOCKED_BOX* pLockedBox, CONST BOX* pBox,DWORD Flags, DWORD dwFlags)
{
	bool bRes = true;
	DWORD Mask = 1;
	DWORD dwLockedFlags = 0;
	HRESULT hr;
	UINT uFirstFound = (UINT)-1;
	
	LOCKED_BOX* pLockedBoxs = new LOCKED_BOX[m_uDevices];
	BOX*		 pBoxs = new BOX[m_uDevices];
	BOOL*		 bNullBox = new BOOL[m_uDevices];  // deferenced as an array, doesn't look "right" as pbNullBox
	BOOL*		 bReadOnly = new BOOL[m_uDevices];  // different array, same deal

	if( pLockedBoxs == NULL || pBoxs == NULL || bReadOnly == NULL || bNullBox == NULL)
	{
		DPF(1, "CnVolTexture::LockBox out of memory error." );
		if( pLockedBoxs )
			delete[] pLockedBoxs;
		if( pBoxs )
			delete[] pBoxs;
		if( bNullBox )
			delete[] bNullBox;
		if( bReadOnly )
			delete[] bReadOnly;
		return false;
	}

	memset( pLockedBoxs, 0, sizeof(LOCKED_BOX) * m_uDevices );
	memset( pBoxs, 0, sizeof(BOX) * m_uDevices );
	memset( bNullBox, 0, sizeof(BOOL) * m_uDevices );
	memset( bReadOnly, 0, sizeof(BOOL) * m_uDevices );

	LOCKED_BOX rcTempBox;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
		{
			Mask <<= 1;
			continue;
		}

		if(m_pVolTextures[i] == NULL)
		{
			Mask <<= 1;
			continue;
		}

		bReadOnly[i] = (Flags & LOCK_READONLY) ? TRUE : FALSE;
		bNullBox[i] = (pBox == NULL) ? TRUE : FALSE;
		if( pBox != NULL )
			memcpy( pBoxs + i, pBox, sizeof(BOX));

		hr =  m_pVolTextures[i]->LockBox(uLevel,&rcTempBox,pBox,Flags);
		if( FAILED( hr ) )
		{
			bRes = false;
			break;
		}
		else
		{
			if( uFirstFound == (UINT)-1 )
				uFirstFound = i;
			dwLockedFlags |= Mask;
			memcpy( pLockedBoxs + i, &rcTempBox, sizeof(LOCKED_BOX) );
		}

		Mask <<= 1;
	}	

	LOCK_LIST* pThis = new LOCK_LIST;

	if( !bRes || pThis == NULL)
	{
		if( pThis != NULL)
			delete pThis;
		
		DPF(1, "CnVolTexture::LockBox() failed.\n" );

		// unlock everything that was locked, no way to unlock later
		for( UINT i= 0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwLockedFlags ) )
				continue;
			hr =  m_pVolTextures[i]->UnlockBox(uLevel);
		}
		delete[] pLockedBoxs;
		delete[] pBoxs;
		delete[] bReadOnly;
		delete[] bNullBox;
		return bRes;
	}

	
	memset( pThis, 0, sizeof(LOCK_LIST) );
	pThis->uFirstFound = uFirstFound;
	pThis->pBoxs = pBoxs;
	pThis->pLockedBoxs = pLockedBoxs;
	pThis->bReadOnly = bReadOnly;
	pThis->bNullBox = bNullBox;
	pThis->pNext = *(GetHead(uLevel));
	*(GetHead(uLevel)) = pThis;
	
	memcpy(pLockedBox, pLockedBoxs + uFirstFound, sizeof( LOCKED_BOX ) );

	return true;
}

// --------------------------------------------------------------------------------------

// obviously not thread safe
bool CnVolTexture::UnlockBox(UINT uLevel, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr;
	UINT uFirstScanLineWidth = (UINT)-1;
	UINT uScanLineWidth;

	// Need format- and surface dimension details.
	VOLUME_DESC sdDesc;
	GetLevelDesc(uLevel,&sdDesc);

	// locate the record for the equivalent lock call
	LOCK_LIST* pThis = *(GetHead(uLevel));
	if( pThis == NULL )
	{
		DPF(1, _T("CnVolTexture:UnlockBox - no associated Lock call found!\n") );
		return false;
	}

	// pop it off the list
	*(GetHead(uLevel)) = pThis->pNext;

	// locate the LOCKED_BOX structs of the devices for this texture level
	LOCKED_BOX* pLockedBoxs = pThis->pLockedBoxs;
	BOX*		 rcBoxs = pThis->pBoxs;
	BOOL*		 bNullBox = pThis->bNullBox;
	BOOL*		 bReadOnly = pThis->bReadOnly;
	
	// loop through each 
	// the significance of the "uFirstFound" member is that it corresponds to the device for which
	// the origional LockBox provided a pointer for.  If we copy data across devices we must use
	// the original pointer given to the caller.
	for(UINT i= pThis->uFirstFound; i<m_uDevices; i++)
	{
		BOX rc;
		memset( &rc, 0, sizeof(rc) );
	
		// skip levels not specified by the caller
		if( SkipDevice( i, dwFlags ) )
			continue;

		// avoid this show stopper
		if(m_pVolTextures[i] == NULL)
			continue;
		
		// if null was specified previously, let's copy the entire texture level
		if( pThis->bNullBox[i] )
		{
			rc.Left = 0;
			rc.Top = 0;
			rc.Front = 0;
			rc.Right = (INT)sdDesc.Width;
			rc.Bottom = (INT)sdDesc.Height;
			rc.Back = (INT)sdDesc.Depth;
		}
		else
			memcpy( &rc, pThis->pBoxs + i, sizeof( BOX ) );
	
		// will need scanline width to determine how many bytes to memcpy
		uScanLineWidth = (sdDesc.Format.ddpfPixelFormat.dwRGBBitCount * (rc.Right - rc.Left))>>3;

		// if this isn't the first level locked, copy the contents from the first one.
		UINT uDest;
		UINT uSrc;
		if( i != pThis->uFirstFound )
		{
			if( (uFirstScanLineWidth == uScanLineWidth) && !(bReadOnly[i]) )
			{
				UINT uiNoOfScanLines = (rc.Bottom-rc.Top) / sdDesc.Format.ddpfPixelFormat.dwRowsPerByteSequence;
				for( UINT uScanSlice= 0; uScanSlice<(rc.Back-rc.Front); uScanSlice++)
				{
					for( UINT uScanLine= 0; uScanLine<uiNoOfScanLines; uScanLine++ )
					{
						uDest = uScanSlice*(pLockedBoxs[i].SlicePitch) + 
								uScanLine*(pLockedBoxs[i].RowPitch);
						uSrc  = uScanSlice*(pLockedBoxs[pThis->uFirstFound].SlicePitch) + 
								uScanLine*(pLockedBoxs[pThis->uFirstFound].RowPitch);
						memcpy( ((BYTE*)(pLockedBoxs[i].pBits)) + uDest,
							((BYTE*)(pLockedBoxs[pThis->uFirstFound].pBits)) + uSrc,
							uScanLineWidth); 
					}
				}
			}
		}
		else
		{
			// record this scanline width to compare against later levels
			uFirstScanLineWidth = uScanLineWidth;
		}

		// wow, now we can unlock it!
		hr =  m_pVolTextures[i]->UnlockBox(uLevel);

		if( FAILED( hr ) )
			bRes = false;
	}	
	
	//  delete the array of LOCKED_BOX structs
	delete[] pLockedBoxs;
	delete[] rcBoxs;
	delete[] bNullBox;
	delete[] bReadOnly;

	//  delete this LOCK_BOX structure we removed from the list earlier
	delete   pThis;

	if( !bRes)
	{
		// for now, don't even bother handling a failed unlock.
		// all devices MUST be kept in sync so should we re-LockBox the area?
		// most likely failure is surface wasn't locked, anyways!
		DPF(1, _T("CnVolTexture:UnlockBox UnlockBox failed!\n") );
		return bRes;
	}

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnVolTexture::LoadImage(UINT uLevel, CImage **pImg, DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pVolTextures[i] == NULL)
			continue;

		if(FAILED(m_pVolTextures[i]->LoadImage(uLevel, pImg, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnVolTexture::LoadImage(CImage ***pImg, DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pVolTextures[i] == NULL)
			continue;

		if(FAILED(m_pVolTextures[i]->LoadImage(pImg, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnVolTexture::LoadTexture(UINT uLevel, UINT uSlice, LPCTSTR pFilename, float fAlphaOverride, DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pVolTextures[i] == NULL)
			continue;

		if(FAILED(m_pVolTextures[i]->LoadTexture(uLevel, uSlice, pFilename, fAlphaOverride, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnVolTexture::LoadStripes(UINT uLevel, UINT uSlice, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal, DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pVolTextures[i] == NULL)
			continue;

		if(FAILED(m_pVolTextures[i]->LoadStripes(uLevel, uSlice, dwW, dwH, dwStripes, dwColorArray, bHorizontal, bDiagonal, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnVolTexture::LoadGradient(UINT uLevel, UINT uSlice, DWORD dwW,DWORD dwH,DWORD dwColors[4], DWORD dwTexLoad, DWORD dwFlags)
{
	bool bRes= true;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if(m_pVolTextures[i] == NULL)
			continue;

		if(FAILED(m_pVolTextures[i]->LoadGradient(uLevel, uSlice, dwW, dwH, dwColors, dwTexLoad)))
		   bRes = false;
	}	

	return bRes;
}

// --------------------------------------------------------------------------------------

bool CnVolTexture::IsPalettized(DWORD dwFlags)
{
	// find the first specified texture that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pVolTextures[i] == NULL )
			continue;

		return m_pVolTextures[i]->IsPalettized();
	}

	return false;
}

// --------------------------------------------------------------------------------------

bool CnVolTexture::SetPalette(PALETTEENTRY *pPalette, DWORD dwFlags)
{
	bool bRes = true;
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pVolTextures[i] == NULL )
			continue;

		if(FAILED(m_pVolTextures[i]->SetPalette(pPalette)))
			bRes &= false;
	}

	return bRes;
}

