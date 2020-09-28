// -----------------------------------------------------------------------------
// File:    TestCases.cpp
//
// Desc:    This file contains the definition of the classes
//          that handle the testing of the KS Topology Device API.
//          There are two main classes that do the actual testing:
//          1. CNodeReq (checks for node requirements)
//          2. CPropDescReq (checks for property description requiremenst)
//
//          Both these classes inheret from CKSTopoDevLoop, which is a
//          class that contains code for looping Filters; Filters, Nodes;
//          Filters, Nodes, Connections and running a specific function to 
//          act on the objects.
//
//          The CNodeReq and CPropDescReq have interface functions that 
//          usually just call one of the looping functions and pass as an 
//          argument a function pointer to a static private member function.
//          In order to be able to see the member variables, a 'this'
//          pointer is passed to the private static functions.
//
//          IMPORTANT: These classes are NOT THREAD-SAFE on one object!!
//                     They can be run in parallel with different objects.
//
//
// Copyright (C) Microsoft Corporation. All rights reserved.
// -----------------------------------------------------------------------------

#include "common.h"
#include <testdevice.h>
#include <testdevnode.h>
#include <audiodevnodeset.h>
#include <hrstring.h>

#ifdef BUILD_TAEF
extern IBasicLog * g_pBasicLog;
#endif //#ifdef BUILD_TAEF

CKSTopoDevLoop::CKSTopoDevLoop()
{ 
	m_pFilter = NULL; 
	m_pNode = NULL; 
	m_pPropDesc = NULL;
	m_pPinFactory = NULL;
	m_iFilter = m_iNode = m_iPropDesc = m_iPin = 0;
}

// copied and modified from msdn:
// http://support.microsoft.com/kb/138813
HRESULT AnsiToUnicode(LPCSTR pszA, __out LPWSTR *ppszW)
{
	size_t cCharacters;
	DWORD dwError;
	LPWSTR pszUnicode = NULL;

	if (NULL == ppszW)
	{
		return E_POINTER;
	}

	// If input is null then just return the same.
	if (NULL == pszA)
	{
		*ppszW = NULL;
		return S_OK;
	}

	// Determine number of wide characters to be allocated for the
	// Unicode string.
	cCharacters =  strlen(pszA)+1;

	// Use of the OLE allocator is required if the resultant Unicode
	// string will be passed to another COM component and that
	// component will free it.
	pszUnicode = (LPWSTR) CoTaskMemAlloc(cCharacters*2);

	if (NULL == pszUnicode)
	{
		return E_OUTOFMEMORY;
	}

	// Covnert to Unicode.
	if (0 == MultiByteToWideChar(CP_ACP, 0, pszA, (int)cCharacters, pszUnicode, (int)cCharacters))
	{
		dwError = GetLastError();
		CoTaskMemFree(pszUnicode);
		*ppszW = NULL;
		return HRESULT_FROM_WIN32(dwError);
	}

	*ppszW = pszUnicode;
	return S_OK;
}

HRESULT CKSTopoDevLoop::IsUSBAudio10Device(bool &fUSBAudio10)
{
	fUSBAudio10 = FALSE;

	if (NULL == m_pFilter)
	{
		XLOG(XFAIL, eError, "ERROR: test has not instantiated the test filter object");
		return E_POINTER;
	}

	HRESULT hr = S_OK;
	CComPtr<IBasicLog> pBasicLog;
#ifndef BUILD_TAEF
	hr = g_pShell->GetBasicLog(&pBasicLog);
	if (FAILED(hr))
	{
		XLOG(XFAIL, eError, "ERROR: failed to get IBasicLog pointer (hr = %s)", GetHRString(hr));
		return E_POINTER;
	}
#else //#ifndef BUILD_TAEF
	pBasicLog = g_pBasicLog;
#endif //#ifndef BUILD_TAEF

	CComHeapPtr<WCHAR> wszInstanceId;
	hr = AnsiToUnicode(m_pFilter->GetInstanceID(), &wszInstanceId);
	if (FAILED(hr))
	{
		XLOG(XFAIL, eError, "ERROR: failed to convert ANSI string (\"%s\") to unicode (hr = %s)", m_pFilter->GetInstanceID(), GetHRString(hr));
		return E_POINTER;
	}

	GUID deviceType = GUID_NULL;
	hr = GetAudioDevNodeTypeFromInstanceId(pBasicLog, wszInstanceId, &deviceType);
	if (FAILED(hr))
	{
		XLOG(XFAIL, eError, "ERROR: failed to get device type from instance Id (\"%s\") (hr = %s)", m_pFilter->GetInstanceID(), GetHRString(hr));
		return hr;
	}

	if (AudioDevNode::GUID_AUDDEVNODE_UNKNOWN == deviceType)
	{
		XLOG(XFAIL, eError, "ERROR: device type is unknown (\"%s\") (hr = %s)", m_pFilter->GetInstanceID(), GetHRString(hr));
		return hr;
	}

	if (AudioDevNode::GUID_AUDDEVNODE_USB_AUDIO_10 == deviceType)
	{
		fUSBAudio10 = true;
		SLOG(eInfo1, "Found a USB Audio 1.0 device (\"%s\")", m_pFilter->GetInstanceID());
	}

	return S_OK;
}

// -----------------------------------------------------------------------------
// Name:    CKSTopoDevLoop::LoopFilters()
//
// Desc:    Loop through each filter of the device and call the function
//          pFnCheck(pArg). 
//
// Parameters:
//          pDevice         Pointer to the audio device to loop
//          pFnCheck         Function pointer called in each loop
//          pArg            Pointer to the argument sent to pFnCheck
//
// -----------------------------------------------------------------------------
DWORD WINAPI CKSTopoDevLoop::LoopFilters(CDevice *pDevice, FnCheck pFnCheck, void *pArg)
{
	bool    fRet = true;
	UINT    cFilter = 0;

	if(NULL == pFnCheck)         return FNS_PASS;

	if (NULL == pDevice)
	{
		XLOG(XFAIL, eError, "ERROR: Unable to obtain test device.");
		return (FNS_FAIL);
	}
	SLOG(eInfo1, "Device: %s (%s)", pDevice->GetFriendlyName(), pDevice->GetInterfaceId());

	// Loop through all filters for each device
	cFilter = pDevice->GetFilterCount();
	for (m_iFilter = 0; m_iFilter < cFilter; ++m_iFilter)
	{
		m_pFilter = pDevice->GetFilter(m_iFilter);
		if ( (NULL == m_pFilter) || !(m_pFilter->IsValid()) )
		{
			XLOG(XFAIL, eError, " ERROR: Unable to obtain Filter[%d].", m_iFilter);
			fRet = false;
			//Check other filters
			continue;           
		}
		SLOG(eInfo1, " Filter: %s", m_pFilter->GetFriendlyName());

		fRet = ( (*pFnCheck)(pArg) ? fRet : false );

	}// For Loop m_iFilter
	return (fRet ? FNS_PASS : FNS_FAIL);
}

// -----------------------------------------------------------------------------
// Name:    CKSTopoDevLoop::LoopFiltersNodes()
//
// Desc:    Loop through each node of each filter of the device and call 
//          the function pFnCheck(pArg). 
//
// Parameters:
//          pDevice         Pointer to the audio device to loop
//          pFnCheck         Function pointer called in each loop
//          pArg            Pointer to the argument sent to pFnCheck
//
// -----------------------------------------------------------------------------
DWORD WINAPI CKSTopoDevLoop::LoopFiltersNodes(CDevice *pDevice, FnCheck pFnCheck, void *pArg)
{
	bool    fRet = true;
	UINT    cFilter=0, cNode=0;

	if (NULL == pDevice)
	{
		XLOG(XFAIL, eError, "ERROR: Unable to obtain test device.");
		return (FNS_FAIL);
	}
	SLOG(eInfo1, "Device: %s (%s)", pDevice->GetFriendlyName(), pDevice->GetInterfaceId());

	// Loop through all filters for each device
	cFilter = pDevice->GetFilterCount();
	for (m_iFilter = 0; m_iFilter < cFilter; ++m_iFilter)
	{
		m_pFilter = pDevice->GetFilter(m_iFilter);
		if ( (NULL == m_pFilter) || !(m_pFilter->IsValid()) )
		{
			XLOG(XFAIL, eError, " ERROR: Unable to obtain Filter[%d].", m_iFilter);
			fRet = false;
			//Continue to next Filter
			continue;
		}
		SLOG(eInfo1, " Filter: %s", m_pFilter->GetFriendlyName());

		// Loop through all the Nodes
		cNode = m_pFilter->GetCountNodes();
		for(m_iNode = 0; m_iNode < cNode; ++m_iNode)
		{
			m_pNode = m_pFilter->GetNode(m_iNode);
			if( (NULL == m_pNode) || !(m_pNode->IsValid()) )
			{
				XLOG(XFAIL, eError, "  ERROR: Unable to obtain reference to Node[%d]", m_iNode);
				fRet = false;
				// Stop checking Nodes and continue to next Filter
				break;  
			}
			SLOG(eBlab2, "  Node[%02d] (%s)", m_iNode, GetKSGuidName(*m_pNode->GetType()));

			// Actual Check Code
			fRet = ( (*pFnCheck)(pArg) ? fRet : false );

		}// For Loop m_iNode
	}// For Loop m_iFilter

	return (fRet ? FNS_PASS : FNS_FAIL);
}

// -----------------------------------------------------------------------------
// Name:    CKSTopoDevLoop::LoopFiltersNodesProps()
//
// Desc:    Loop through each property descriptor of each node of each filter of
//          the device and call the function pFnCheck(pArg). 
//
// Parameters:
//          pDevice         Pointer to the audio device to loop
//          pFnCheck         Function pointer called in each loop
//          pArg            Pointer to the argument sent to pFnCheck
//
// -----------------------------------------------------------------------------
DWORD WINAPI CKSTopoDevLoop::LoopFiltersNodesProps(CDevice * pDevice, FnCheck pFnCheck, void *pArg)
{
	bool    fRet = true;
	UINT     iFilter=0;
	UINT	 iNode=0;
	UINT     iPropDesc=0;
	CPropDescReq *pThis = static_cast<CPropDescReq*>(pArg);
	HANDLE hPinHandle = INVALID_HANDLE_VALUE;
	HRESULT hr = S_OK;
	CPinInstance *pCPinInstance = NULL;

	if(NULL == pDevice)
	{
		XLOG(XFAIL, 
			eError, 
			"ERROR: Unable to obtain test device.");
		return (FNS_FAIL);
	}

	SLOG(eInfo1, 
		"Device: %s (%s)", 
		pDevice->GetFriendlyName(), 
		pDevice->GetInterfaceId());

	// Loop through all filters for each device
	iFilter = pDevice->GetFilterCount();
	for(m_iFilter = 0; m_iFilter < iFilter; ++m_iFilter)
	{
		m_pFilter = pDevice->GetFilter(m_iFilter);
		if((NULL == m_pFilter) || 
			!(m_pFilter->IsValid()))
		{
			XLOG(XFAIL, 
				eError, 
				" ERROR: Unable to obtain Filter[%d].", 
				m_iFilter);
			fRet = false;

			//Continue to next Filter
			continue;
		}

		SLOG(eInfo1, 
			" Filter: %s", 
			m_pFilter->GetFriendlyName());

		// Loop through all the Nodes
		iNode = m_pFilter->GetCountNodes();
		for(m_iNode = 0; m_iNode < iNode; ++m_iNode)
		{
			m_pNode = m_pFilter->GetNode(m_iNode);
			if((NULL == m_pNode) || 
				!(m_pNode->IsValid()))
			{
				XLOG(XFAIL, 
					eError, 
					"  ERROR: Unable to obtain reference to Node[%d]", 
					m_iNode);
				fRet = false;
				// Stop checking Nodes and continue to next Filter
				break;  
			}
			SLOG(eBlab2, 
				"  Node[%02d] (%s)", 
				m_iNode, 
				GetKSGuidName(*m_pNode->GetType()));

			pThis->m_fIsFilterProperty = true;

			// Loop through all the Filter Property Descriptors
			iPropDesc = m_pNode->GetCountFilterProperties();
			for(m_iPropDesc = 0; m_iPropDesc < iPropDesc; ++m_iPropDesc)
			{
				m_pPropDesc = m_pNode->GetFilterProperty(m_iPropDesc);
				if( (NULL == m_pPropDesc) || !(m_pPropDesc->IsValid()) )
				{
					XLOG(XFAIL, 
						eError, 
						"   ERROR: Unable to obtain reference to Filter Property Descriptor[%02d]", 
						m_iPropDesc);
					fRet = false;
					break;
				}

				SLOG(eBlab2, 
					"   Filter Property Descriptor[%02d] (%s)", 
					m_iPropDesc, m_pPropDesc->GetPropertyName());

				// Actual Check Code
				fRet = ((*pFnCheck)(pArg) ? fRet : false);

			}// For Loop m_iPropDesc

			pThis->m_fIsFilterProperty = false;

			// Loop through all the Pin Property Descriptors
			hr = m_pNode->GetPinOnStreamPath();            
			if (hr == S_OK)
			{
				pCPinInstance = m_pNode->GetPinInstance();
				hPinHandle = pCPinInstance->GetPinHandle();

				//Get the properties corresponding to the pin handle obtained.
				m_pNode->GetPinProperties(hPinHandle);
				
                iPropDesc = pCPinInstance->GetCountPinProperties();
				for(m_iPropDesc = 0; m_iPropDesc < iPropDesc; ++m_iPropDesc)
				{
					m_pPropDesc = pCPinInstance->GetPinProperty(m_iPropDesc);
					if( (NULL == m_pPropDesc) || !(m_pPropDesc->IsValid()) )
					{
						XLOG(XFAIL, eError, "   ERROR: Unable to obtain reference to Pin Property Descriptor[%02d]", m_iPropDesc);
						fRet = false;
						break;
					}
					SLOG(eBlab2, "   Pin Property Descriptor[%02d] (%s)", m_iPropDesc, m_pPropDesc->GetPropertyName() );
					// Actual Check Code
					fRet = ( (*pFnCheck)(pArg) ? fRet : false );

				}// For Loop m_iPropDesc                
				if (hPinHandle != INVALID_HANDLE_VALUE)
				{                    
					hPinHandle = INVALID_HANDLE_VALUE;
					pCPinInstance->ClosePinHandle();
					delete pCPinInstance;
				}

			}

		}// For Loop m_iNode
	}// For Loop m_iFilter

	return (fRet ? FNS_PASS : FNS_FAIL);
}


// -----------------------------------------------------------------------------
// Name:    CKSTopoDevLoop::LoopFiltersPins()
//
// Desc:    Loop through each node of each filter of the device and call 
//          the function pFnCheck(pArg). g
//
// Parameters:
//          pDevice         Pointer to the audio device to loop
//          pFnCheck         Function pointer called in each loop
//          pArg            Pointer to the argument sent to pFnCheck
//
// -----------------------------------------------------------------------------
DWORD WINAPI CKSTopoDevLoop::LoopFiltersPins(CDevice *pDevice, FnCheck pFnCheck, void *pArg)
{
	bool    fRet    = true;
	UINT    cFilter = 0;
	UINT    cPin    = 0;

	if (NULL == pDevice)
	{
		XLOG(XFAIL, 
			eError, 
			"ERROR: Unable to obtain test device.");
		return (FNS_FAIL);
	}
	SLOG(eInfo1, 
		"Device: %s (%s)", 
		pDevice->GetFriendlyName(), 
		pDevice->GetInterfaceId());

	// Loop through all filters for each device
	cFilter = pDevice->GetFilterCount();
	for (m_iFilter = 0; m_iFilter < cFilter; ++m_iFilter)
	{
		m_pFilter = pDevice->GetFilter(m_iFilter);
		if ( (NULL == m_pFilter) || 
			!(m_pFilter->IsValid()) )
		{
			XLOG(XFAIL, 
				eError, 
				" ERROR: Unable to obtain Filter[%d].", 
				m_iFilter);
			fRet = false;
			//Continue to next Filter
			continue;
		}
		SLOG(eInfo1, 
			" Filter: %s", 
			m_pFilter->GetFriendlyName());

		// Loop through all the Nodes
		cPin = m_pFilter->GetCountPins();
		for(m_iPin = 0; m_iPin < cPin; ++m_iPin)
		{
			m_pPinFactory = m_pFilter->GetPinFactory(m_iPin);
			if( (NULL == m_pPinFactory) || 
				!(m_pPinFactory->IsValid()) )
			{
				XLOG(XFAIL, 
					eError, 
					"  ERROR: Unable to obtain reference to Pin[%d]", 
					m_iPin);
				fRet = false;
				// Stop checking Pins and continue to next Filter
				break;  
			}
			SLOG(eBlab2, 
				"  Pin[%02d] (%s)", 
				m_iPin, m_pPinFactory->GetFriendlyName());

			// Actual Check Code
			fRet = ( (*pFnCheck)(pArg) ? fRet : false );

		}// For Loop m_iPin
	}// For Loop m_iFilter

	return (fRet ? FNS_PASS : FNS_FAIL);
}
