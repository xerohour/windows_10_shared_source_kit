//////////////////////////////////////////////////////////////////////
// File:  DXGIAdapterTopology.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a helper object that keeps track of the DXGI
// adapter and output topology.
//
// History:
// 07 Jan 2007  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include "DXGIAdapterTopology.hpp"


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CDXGIAdapterTopology
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CDXGIAdapterTopology::CDXGIAdapterTopology()
:
	m_cAdapters( 0 ),
	m_cOutputs( 0 )
{
	ZeroMemory( m_AdapterOrdinalForAdapterSlot, sizeof( m_AdapterOrdinalForAdapterSlot ) );
	ZeroMemory( m_OutputsForAdapterSlot, sizeof( m_OutputsForAdapterSlot ) );
	ZeroMemory( m_AdapterDescriptors, sizeof( m_AdapterDescriptors ) );

	ZeroMemory( m_OutputOrdinalForOutputSlot, sizeof( m_OutputOrdinalForOutputSlot ) );
	ZeroMemory( m_AdapterSlotForOutputSlot, sizeof( m_AdapterSlotForOutputSlot ) );
	ZeroMemory( m_OutputDescriptors, sizeof( m_OutputDescriptors ) );
};


///////////////////////////////////////////////////////
// Function:  EnumerateAdaptersAndOutputs
//
// Purpose:
// Enumerates DXGI adapters and outputs, in order to
// establish the number of adapters, the number of
// outputs, the mapping from outputs to adapters, and
// an array of output descriptors.
///////////////////////////////////////////////////////
HRESULT CDXGIAdapterTopology::EnumerateAdaptersAndOutputs
(
	__inout DXGI_FACTORY_POINTER * pFactoryPointer,
	__in HMODULE hRasterizerDLL,
	IN D3D10_DRIVER_TYPE DriverType
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	UINT MaxAdapterOrdinal = 20;

	DXGI_ADAPTER_POINTER CurrentAdapter;
	ZeroMemory( &CurrentAdapter, sizeof( CurrentAdapter ) );

	IDXGIOutput * pCurrentOutput = NULL;


	//
	// Validate parameters.
	//

	if ( pFactoryPointer == NULL )
	{
		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Initialize members.
	//

	m_cAdapters = 0;
	m_cOutputs = 0;


	if ( pFactoryPointer->pFactory == NULL )
	{
		HRESULT hCreateFactoryResult = ( (CDXGITestFramework *) GetFramework() )->CreateDXGIFactory
		(
			pFactoryPointer
		);

		if ( FAILED( hCreateFactoryResult ) )
		{
			hOverallResult = hCreateFactoryResult;
			goto Cleanup;
		};

		if ( pFactoryPointer->pFactory == NULL )
		{
			hOverallResult = E_FAIL;
			goto Cleanup;
		};
	};


	switch ( DriverType )
	{
		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			MaxAdapterOrdinal = 20;
		} break;

		case D3D10_DRIVER_TYPE_SOFTWARE:
		case D3D10_DRIVER_TYPE_REFERENCE:
		case D3D10_DRIVER_TYPE_NULL:
		{
			MaxAdapterOrdinal = 0;
		} break;

		default:
		{
		} break;
	};


	for
	(
		UINT iCurrentAdapterOrdinal = 0;
		iCurrentAdapterOrdinal <= MaxAdapterOrdinal &&
			m_cAdapters < ARRAYSIZE( m_AdapterSlotForOutputSlot );
		++iCurrentAdapterOrdinal
	)
	{
		HRESULT hCreateAdapterResult = ( (CDXGITestFramework *) GetFramework() )->CreateAdapter
		(
			*pFactoryPointer,
			iCurrentAdapterOrdinal,
			&CurrentAdapter
		);

		if ( hCreateAdapterResult == DXGI_ERROR_NOT_FOUND )
		{
			// Systems should always have at least one adapter.  If not, we need to fail.
			if (iCurrentAdapterOrdinal==0)
			{
				hOverallResult = hCreateAdapterResult;
				goto Cleanup;
			}
			else
				break;
		}
		else if ( FAILED( hCreateAdapterResult ) )
		{
			hOverallResult = hCreateAdapterResult;
			goto Cleanup;
		};

		if ( CurrentAdapter.pAdapter == NULL )
		{
			hOverallResult = E_FAIL;
			goto Cleanup;
		};


		// Get a descriptor for the adapter.
		if ( ( (CDXGITestFramework *) GetFramework() )->m_DXGIOptions.Version == TEST_DXGI_VERSION_1_1 )
		{
			HRESULT hGetDescriptorResult = CurrentAdapter.pAdapter1->GetDesc1
			(
				&m_AdapterDescriptors[ m_cAdapters ]
			);

			if ( FAILED( hGetDescriptorResult ) )
			{
				hOverallResult = hGetDescriptorResult;
				goto Cleanup;
			};
		}
		else
		{
			DXGI_ADAPTER_DESC AdapterDescriptor;
			HRESULT hGetDescriptorResult = CurrentAdapter.pAdapter->GetDesc
			(
				&AdapterDescriptor
			);

			if ( FAILED( hGetDescriptorResult ) )
			{
				hOverallResult = hGetDescriptorResult;
				goto Cleanup;
			};

			memcpy
			(
				&m_AdapterDescriptors[ m_cAdapters ],
				&AdapterDescriptor,
				sizeof( DXGI_ADAPTER_DESC )
			);
		};

		m_OutputsForAdapterSlot[ m_cAdapters ] = 0;
		m_AdapterOrdinalForAdapterSlot[ m_cAdapters ] = iCurrentAdapterOrdinal;

		for
		(
			UINT iCurrentOutputOrdinal = 0;
			iCurrentOutputOrdinal < MAX_OUTPUTS;
			++iCurrentOutputOrdinal
		)
		{
			HRESULT hEnumerateOutputResult = CurrentAdapter.pAdapter->EnumOutputs
			(
				iCurrentOutputOrdinal,
				&pCurrentOutput
			);

			if ( hEnumerateOutputResult == DXGI_ERROR_NOT_FOUND )
			{
				break;
			}
			else if ( FAILED( hEnumerateOutputResult ) )
			{
				hOverallResult = hEnumerateOutputResult;
				goto Cleanup;
			};


			if ( pCurrentOutput == NULL )
			{
				hOverallResult = E_FAIL;
				goto Cleanup;
			};


			HRESULT hGetOutputDescResult = pCurrentOutput->GetDesc( &m_OutputDescriptors[ m_cOutputs ] );

			if ( FAILED( hGetOutputDescResult ) )
			{
				hOverallResult = hGetOutputDescResult;
				goto Cleanup;
			};


			++m_OutputsForAdapterSlot[ m_cAdapters ];
            // bogus warning 26000: Overflow using expression 'this->m_OutputOrdinalForOutputSlot[this->m_cOutputs]' 
            // m_cOutputs is limited by MAX_OUTPUTS
#pragma warning( push )
#pragma warning( disable: 26000 )
			m_OutputOrdinalForOutputSlot[ m_cOutputs ] = iCurrentOutputOrdinal;
			m_AdapterSlotForOutputSlot[ m_cOutputs ] = m_cAdapters;

			++m_cOutputs;
#pragma warning( pop )

			pCurrentOutput->Release();
			pCurrentOutput = NULL;
		};

		++m_cAdapters;

		CurrentAdapter.pAdapter->Release();
		CurrentAdapter.pAdapter = NULL;
	};


	hOverallResult = S_OK;


Cleanup:
	if ( pCurrentOutput != NULL )
	{
		pCurrentOutput->Release();
		pCurrentOutput = NULL;
	};

	if ( CurrentAdapter.pAdapter != NULL )
	{
		CurrentAdapter.pAdapter->Release();
		CurrentAdapter.pAdapter = NULL;
	};


	return hOverallResult;
};
