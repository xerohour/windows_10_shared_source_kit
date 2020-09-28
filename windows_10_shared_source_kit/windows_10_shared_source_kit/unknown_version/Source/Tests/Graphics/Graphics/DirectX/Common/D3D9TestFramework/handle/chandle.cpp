// ============================================================================
//
//  CHandle.cpp
//
//  Copyright (C) 2001 Microsoft Corporation
//
// ============================================================================

#include "CHandle.h"
#include "CDebugOut.h"
#include "HelperFunctions.h"

// ----------------------------------------------------------------------------

CHandle::CHandle()
{
	m_nGrowSize = 100;
	m_uAllocHandles = 0;
	m_uHandles = 0;
	m_uDevices = 0;
	m_pHandles = NULL;
}

// ----------------------------------------------------------------------------

CHandle::~CHandle()
{
	ReleaseAll();
}

// ----------------------------------------------------------------------------

bool CHandle::AllocArray(UINT uWidth, UINT uHeight, UINT_PTR **pOut)
{
	(*pOut) = new UINT_PTR[uWidth * uHeight];

	if((*pOut) == NULL)
	{
		DebugOut.Write(1, _T("CHandle::AllocArray() - Out of memory.\n"));
		return false;
	}

	memset((*pOut), 0, sizeof(UINT_PTR) * uWidth * uHeight);
    m_uAllocHandles = uHeight;

	return true;
}

// ----------------------------------------------------------------------------

bool CHandle::Setup(UINT nDevices, UINT nGrowSize)
{
	// Release any existing handles
	ReleaseAll();

	// Set the number of devices
	m_uDevices = nDevices;
	
	// Alloc the initial handles array
	return AllocArray(m_uDevices, nGrowSize, &m_pHandles);
}

// ----------------------------------------------------------------------------

void CHandle::ReleaseAll()
{
	SAFEDELETEA(m_pHandles);
	m_uAllocHandles = 0;
	m_uHandles = 0;
	m_uDevices = 0;
}

// ----------------------------------------------------------------------------

UINT CHandle::AddHandle(UINT_PTR** pHandles)
{
	if(m_uHandles >= m_uAllocHandles)
	{
		UINT_PTR *dwTemp = NULL;
		if(!AllocArray(m_uDevices, m_uAllocHandles + m_nGrowSize, &dwTemp))
		{
			return false;
		}
		else
		{
			memcpy(dwTemp, m_pHandles, sizeof(UINT_PTR) * m_uDevices * m_uHandles);
			SAFEDELETEA(m_pHandles);
			m_pHandles = dwTemp;
		}
	}
    
	*pHandles = (m_pHandles + (m_uDevices * m_uHandles));

	return ++m_uHandles;
}

// ----------------------------------------------------------------------------

UINT_PTR* CHandle::GetHandle(UINT uHandle)
{
	if(m_pHandles == NULL || uHandle == 0)
		return NULL;

	return (m_pHandles + (m_uDevices * (uHandle-1)));
}
	
// ----------------------------------------------------------------------------

UINT_PTR* CHandle::GetHandle(UINT uHandle, UINT uDevice)
{
	if(m_pHandles == NULL || uHandle == 0)
		return NULL;

	return (m_pHandles + (m_uDevices * (uHandle-1)) + uDevice);
}


// ----------------------------------------------------------------------------
//
//		CVertexShaderHandle
//
// ----------------------------------------------------------------------------

CVertexShaderHandle::CVertexShaderHandle()
{
}

// ----------------------------------------------------------------------------

CVertexShaderHandle::~CVertexShaderHandle()
{
}

// ----------------------------------------------------------------------------

UINT CVertexShaderHandle::AddHandle(UINT_PTR** pHandles)
{
	if(m_uHandles >= m_uAllocHandles)
	{
		UINT_PTR *dwTemp = NULL;
		if(!AllocArray(m_uDevices, m_uAllocHandles + m_nGrowSize, &dwTemp))
		{
			return false;
		}
		else
		{
			memcpy(dwTemp, m_pHandles, sizeof(UINT_PTR) * m_uDevices * m_uHandles);
			SAFEDELETEA(m_pHandles);
			m_pHandles = dwTemp;
		}
	}
    
	*pHandles = (m_pHandles + (m_uDevices * m_uHandles));

	m_uHandles++;

	return ((m_uHandles<<1) | 0x01);
}

// ----------------------------------------------------------------------------

UINT_PTR* CVertexShaderHandle::GetHandle(UINT uHandle)
{
	if(m_pHandles == NULL)
		return NULL;

	if ((uHandle & 0x01) == 0 || uHandle == 0)
		return NULL;

	return (m_pHandles + (m_uDevices * ((uHandle>>1)-1)));
}
	
// ----------------------------------------------------------------------------

UINT_PTR* CVertexShaderHandle::GetHandle(UINT uHandle, UINT uDevice)
{
	if(m_pHandles == NULL)
		return NULL;

	if ((uHandle & 0x01) == 0)
		return NULL;

	return (m_pHandles + (m_uDevices * ((uHandle>>1)-1)) + uDevice);
}
