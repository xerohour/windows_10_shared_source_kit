//////////////////////////////////////////////////////////////////////
// File:  D3D9AdapterTopology.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a helper object that keeps track of the Direct3D
// 9 adapter topology.
//
// History:
// 07 Jan 2007  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include "D3D9AdapterTopology.hpp"


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D9AdapterTopology
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D9AdapterTopology::CD3D9AdapterTopology()
:
	m_cAdapters( 0 )
{
	ZeroMemory( &m_AdapterOrdinalForAdapterSlot, sizeof( m_AdapterOrdinalForAdapterSlot ) );
};


///////////////////////////////////////////////////////
// Function:  EnumerateAdaptersAndOutputs
//
// Purpose:
// Enumerates Direct3D 9 adapters.
///////////////////////////////////////////////////////
HRESULT CD3D9AdapterTopology::EnumerateAdaptersAndOutputs
(
	IN IDirect3D9 * pEnumerator,
	D3DDEVTYPE DeviceType
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Validate parameters.
	//

	if ( pEnumerator == NULL )
	{
		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Initialize members.
	//

	m_cAdapters = pEnumerator->GetAdapterCount();

	if ( m_cAdapters >= ARRAYSIZE( m_AdapterDescriptors ) )
	{
		m_cAdapters = ARRAYSIZE( m_AdapterDescriptors );
	};


	//
	// Get the adapter identifiers and capabilities.
	//

	for
	(
		UINT iCurrentAdapterOrdinal = 0;
		iCurrentAdapterOrdinal < m_cAdapters;
		++iCurrentAdapterOrdinal
	)
	{
		m_AdapterOrdinalForAdapterSlot[ iCurrentAdapterOrdinal ] = iCurrentAdapterOrdinal;

		HRESULT hGetAdapterIdentifierResult = pEnumerator->GetAdapterIdentifier
		(
			iCurrentAdapterOrdinal,
			0,
			&m_AdapterDescriptors[ iCurrentAdapterOrdinal ]
		);

		if ( FAILED( hGetAdapterIdentifierResult ) )
		{
			hOverallResult = hGetAdapterIdentifierResult;
			goto Cleanup;
		};


		HRESULT hGetAdapterCapsResult = pEnumerator->GetDeviceCaps
		(
			iCurrentAdapterOrdinal,
			DeviceType,
			&m_AdapterCaps[ iCurrentAdapterOrdinal ]
		);

		if ( FAILED( hGetAdapterCapsResult ) )
		{
			hOverallResult = hGetAdapterCapsResult;
			goto Cleanup;
		};
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};
