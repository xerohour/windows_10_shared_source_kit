//////////////////////////////////////////////////////////////////////
// File:  D3D9ToGDIAdapterTopologyMap.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a helper object that creates a map between a
// Direct3D 9 adapter topology and a GDI adapter and output topology.
//
// History:
// 07 Jan 2007  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include "D3D9ToGDIAdapterTopologyMap.hpp"


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D9ToGDIAdapterTopologyMap
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D9ToGDIAdapterTopologyMap::CD3D9ToGDIAdapterTopologyMap()
{
	FillMemory( &m_GDIDeviceSlotForD3D9AdapterSlot, sizeof( m_GDIDeviceSlotForD3D9AdapterSlot ), 0xFF );
	FillMemory( &m_D3D9AdapterSlotForGDIDeviceSlot, sizeof( m_D3D9AdapterSlotForGDIDeviceSlot ), 0xFF );
};


///////////////////////////////////////////////////////
// Function:  MapAdapterTopologies
//
// Purpose:
// Takes a GDI adapter topology and a Direct3D 9
// adapter topology, and creates a mapping between
// them using the names of the underlying GDI display
// devices as the common piece of information.
///////////////////////////////////////////////////////
HRESULT CD3D9ToGDIAdapterTopologyMap::MapAdapterTopologies
(
	CGDIAdapterTopology & GDITopology,
	CD3D9AdapterTopology & D3D9Topology
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Initialize our adapter topology maps.
	//

	FillMemory( &m_GDIDeviceSlotForD3D9AdapterSlot, sizeof( m_GDIDeviceSlotForD3D9AdapterSlot ), 0xFF );
	FillMemory( &m_D3D9AdapterSlotForGDIDeviceSlot, sizeof( m_D3D9AdapterSlotForGDIDeviceSlot ), 0xFF );


	//
	// Fill in our adapter topology maps.
	//

	for
	(
		UINT iCurrentD3D9AdapterSlot = 0;
		iCurrentD3D9AdapterSlot < D3D9Topology.m_cAdapters &&
			iCurrentD3D9AdapterSlot < ARRAYSIZE( D3D9Topology.m_AdapterDescriptors );
		++iCurrentD3D9AdapterSlot
	)
	{
		for
		(
			UINT iCurrentGDIDeviceSlot = 0;
			iCurrentGDIDeviceSlot < GDITopology.m_cDevices &&
				iCurrentGDIDeviceSlot < ARRAYSIZE( GDITopology.m_DeviceDescriptors );
			++iCurrentGDIDeviceSlot
		)
		{
			WCHAR ConvertedDeviceName[ 32 ] = L"";
            // warning 28931: Unused assignment of variable nConvertedDeviceName
            int nConvertedDeviceName = MultiByteToWideChar
			(
				CP_ACP,
				0,
				D3D9Topology.m_AdapterDescriptors[ iCurrentD3D9AdapterSlot ].DeviceName,
				32,
				ConvertedDeviceName,
				ARRAYSIZE( ConvertedDeviceName )
			);

            if ( nConvertedDeviceName == 0 )
            {
                hOverallResult = HRESULT_FROM_WIN32( GetLastError() );
                goto Cleanup;
            };

			if
			(    
				_wcsicmp
				(
					ConvertedDeviceName,
					GDITopology.m_DeviceDescriptors[ iCurrentGDIDeviceSlot ].DeviceName
				) == 0
			)
			{
				m_D3D9AdapterSlotForGDIDeviceSlot[ iCurrentGDIDeviceSlot ] = iCurrentD3D9AdapterSlot;
				m_GDIDeviceSlotForD3D9AdapterSlot[ iCurrentD3D9AdapterSlot ] = iCurrentGDIDeviceSlot;
				break;
			};
		};
	};


	hOverallResult = S_OK;

Cleanup:
	return hOverallResult;
};
