#include "BuffersPCH.h"

#define STRICT
#include "CBuffers.h"


// -------------------------------------------------------------------------------------

CnIndexBuffer::CnIndexBuffer()
{
	m_pIndexBuffers = NULL;
	m_uDevices = 0;
	m_pHeadLock= NULL;
}

// --------------------------------------------------------------------------------------

CnIndexBuffer::~CnIndexBuffer()
{
	if( !m_pIndexBuffers )
		return;

	for( UINT i= 0; i< m_uDevices; i++ )
	{
		RELEASE( m_pIndexBuffers[i] );
	}
	SAFEDELETEA(m_pIndexBuffers);
}

// -------------------------------------------------------------------------------------

bool CnIndexBuffer::SkipDevice(UINT uDevice, DWORD dwFlags)
{
	DWORD dwMask = 1;

	for(int i=0; i<uDevice; i++)
		dwMask <<= 1;

	return (dwMask & dwFlags) == 0;
}

// -------------------------------------------------------------------------------------

bool CnIndexBuffer::Init(UINT uDevices)
{
	m_pIndexBuffers = new CIndexBuffer*[uDevices];
	
	if( m_pIndexBuffers == NULL )
		return false;

	m_uDevices = uDevices;
	memset( m_pIndexBuffers, 0, uDevices * sizeof( CIndexBuffer*) );
	return true;
}

// -------------------------------------------------------------------------------------

bool CnIndexBuffer::GetIndexBuffer(CIndexBuffer **ppIndexBuffer, UINT n)
{
	if( n >= m_uDevices || ppIndexBuffer == NULL )
		return false;

	m_pIndexBuffers[n]->AddRef();
	*ppIndexBuffer = m_pIndexBuffers[n];
	return true;
}

// -------------------------------------------------------------------------------------

bool CnIndexBuffer::SetIndexBuffer(CIndexBuffer *pIndexBuffer, UINT n)
{
	if( n >= m_uDevices )
		return false;

	if( m_pIndexBuffers[n] != NULL )
		return false;

	m_pIndexBuffers[n] = pIndexBuffer;
	return true;
}

// -------------------------------------------------------------------------------------

bool CnIndexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags, DWORD dwFlags)
{
	bool bRes = true;
	DWORD Mask = 1;
	DWORD dwLockedFlags = 0;
	HRESULT hr;
	UINT uFirstFound = (UINT)-1;
	
	BYTE**	pLockedData = new BYTE*[m_uDevices];	// deferenced as an array
	UINT*	nSizes = new UINT[m_uDevices];			// deferenced as an array, doesn't look "right" as pnSizes[i]=256
	BOOL*	bReadOnly = new BOOL[m_uDevices];		// ""

	if( pLockedData == NULL || nSizes == NULL || bReadOnly == NULL)
	{
		DPF(1, "CnIndexBuffer::Lock() out of memory error." );
		if( pLockedData )
			delete[] pLockedData;
		if( nSizes )
			delete[] nSizes;
		if( bReadOnly )
			delete[] bReadOnly;
		return false;
	}

	memset( pLockedData, 0, sizeof(BYTE*) * m_uDevices );
	memset( nSizes,	 0, sizeof(UINT) * m_uDevices );
	memset( bReadOnly, 0, sizeof(BOOL) * m_uDevices );

	BYTE* pTempData;

	for(UINT i= 0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
		{
			Mask <<= 1;
			continue;
		}

		if(m_pIndexBuffers[i] == NULL)
		{
			Mask <<= 1;
			continue;
		}

		bReadOnly[i] = (Flags & LOCK_READONLY) ? TRUE : FALSE;
		nSizes[i] = SizeToLock;

		hr =  m_pIndexBuffers[i]->Lock(OffsetToLock, SizeToLock, &pTempData, Flags);

		if( SUCCEEDED( hr ) )
		{
			if( uFirstFound == (UINT)-1 )
				uFirstFound = i;
			dwLockedFlags |= Mask;
			pLockedData[i] = pTempData;
		}
		else
		{
			bRes = false;
			break;
		}

		Mask <<= 1;
	}	

	LOCK_LIST* pThis = new LOCK_LIST;

	if( !bRes || pThis == NULL)
	{
		if( pThis != NULL)
			delete pThis;
		
		DPF(1, "CnIndexBuffer::Lock() failed." );

		// unlock everything that was locked, no way to unlock later
		for( UINT i= 0; i<m_uDevices; i++)
		{
			if( SkipDevice( i, dwLockedFlags ) )
				continue;
			hr =  m_pIndexBuffers[i]->Unlock();
		}
		delete[] pLockedData;
		delete[] nSizes;
		delete[] bReadOnly;
		return bRes;
	}

	memset( pThis, 0, sizeof(LOCK_LIST) );
	pThis->uFirstFound = uFirstFound;
	pThis->nSizes = nSizes;
	pThis->pLockedData = pLockedData;
	pThis->bReadOnly = bReadOnly;
	pThis->pNext = m_pHeadLock;
	m_pHeadLock = pThis;
	
	*ppbData = pLockedData[uFirstFound];

	return true;
}

// -------------------------------------------------------------------------------------

bool CnIndexBuffer::Unlock(DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr;

	// locate the record for the equivalent lock call
	LOCK_LIST* pThis = m_pHeadLock;
	if( pThis == NULL )
	{
		DPF(1, _T("CnIndexBuffer:Unlock - no associated Lock call found!\n") );
		return false;
	}

	// pop it off the list
	m_pHeadLock = pThis->pNext;

	// locate the LOCKED_RECT structs of the devices for this texture level
	BYTE**	pLockedData = pThis->pLockedData;
	UINT*	nSizes   = pThis->nSizes;
	BOOL*	bReadOnly = pThis->bReadOnly;

	UINT	nTempSize = (UINT)-1;
	UINT	nFirstFoundSize = (UINT)-1;

	// loop through each 
	// the significance of the "uFirstFound" member is that it corresponds to the device for which
	// the origional Lock provided a pointer for.  If we copy data across devices we must use
	// the original pointer given to the caller.
	for(UINT i= pThis->uFirstFound; i<m_uDevices; i++)
	{
		// skip levels not specified by the caller
		if( SkipDevice( i, dwFlags ) )
			continue;

		// avoid this show stopper
		if(m_pIndexBuffers[i] == NULL)
			continue;

		nTempSize = (nSizes[i] == 0) ? m_pIndexBuffers[i]->GetSize() : nSizes[i];
		
		// if this isn't the first level locked, copy the contents from the first one.
		if( i != pThis->uFirstFound )
		{
			if( !(bReadOnly[i]) && (nTempSize == nFirstFoundSize ) )
				memcpy( (BYTE*)pLockedData[i], (BYTE*)pLockedData[pThis->uFirstFound], nTempSize); 
		}
		else
			nFirstFoundSize = nTempSize;
	}

	for(UINT i= pThis->uFirstFound; i<m_uDevices; i++)
	{
		// skip levels not specified by the caller
		if( SkipDevice( i, dwFlags ) )
			continue;

		// avoid this show stopper
		if(m_pIndexBuffers[i] == NULL)
			continue;

		// wow, now we can unlock it!
		hr =  m_pIndexBuffers[i]->Unlock();

		if( FAILED( hr ) )
			bRes = false;
	}	
	
	//  delete the array of LOCKED_RECT structs
	delete[] pLockedData;
	delete[] nSizes;
	delete[] bReadOnly;

	//  delete this LOCK_RECT structure we removed from the list earlier
	delete   pThis;

	if( !bRes)
	{
		// for now, don't even bother handling a failed unlock.
		// all devices MUST be kept in sync so should we should re-Lock the area?
		// most likely failure is surface wasn't locked, anyways!
		DPF(1, _T("CnTexture:UnlockRect UnlockRect failed!\n") );
		return bRes;
	}

	return bRes;
}

// -------------------------------------------------------------------------------------

bool CnIndexBuffer::GetDesc(PINDEXBUFFERDESC pDesc, DWORD dwFlags)
{
	// find the first specified vertex buffer that exists
	for(UINT i=0; i<m_uDevices; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if( m_pIndexBuffers[i] == NULL )
			continue;

		return SUCCEEDED(m_pIndexBuffers[i]->GetDesc(pDesc));
	}
	return false;
}

// -------------------------------------------------------------------------------------

DWORD CnIndexBuffer::GetPriority(DWORD dwFlags)
{
	// return 1st device that matches the device flags
	for (int i = 0; i < m_uDevices; i++)
	{
        if (SkipDevice(i, dwFlags))
			continue;

		return m_pIndexBuffers[i]->GetPriority();
	}
		
	return (DWORD)0;
}

// -------------------------------------------------------------------------------------

DWORD CnIndexBuffer::SetPriority(DWORD PriorityNew, DWORD dwFlags)
{
	bool bSet = false;
    DWORD PriorityOld = 0;
	// set the priority for all devices that match the device flags then return true
	for (int i = 0; i < m_uDevices; i++)
	{
        if (SkipDevice(i, dwFlags))
			continue;

        bSet = false;
		PriorityOld = m_pIndexBuffers[i]->SetPriority(PriorityNew);
	}

    if (!bSet)
        DPF(1, _T("CnIndexBuffer::SetPriority() - No priority set on any device!\n"));	
    	
	return PriorityOld; //returns the last old priority from the list of buffers specified by dwFlags
}

// -------------------------------------------------------------------------------------

void CnIndexBuffer::PreLoad(DWORD dwFlags)
{
	for (int i = 0; i < m_uDevices; i++)
	{
        if (SkipDevice(i, dwFlags))
			continue;

		m_pIndexBuffers[i]->PreLoad();
	}
}
