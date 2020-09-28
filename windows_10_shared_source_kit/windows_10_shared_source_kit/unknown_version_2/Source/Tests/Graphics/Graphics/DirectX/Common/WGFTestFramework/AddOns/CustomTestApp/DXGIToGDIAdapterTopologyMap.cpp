//////////////////////////////////////////////////////////////////////
// File:  DXGIToGDIAdapterTopologyMap.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a helper object that creates a map between a
// DXGI adapter and output topology and a GDI adapter and output
// topology.
//
// History:
// 07 Jan 2007  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include "DXGIToGDIAdapterTopologyMap.hpp"


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CDXGIToGDIAdapterTopologyMap
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CDXGIToGDIAdapterTopologyMap::CDXGIToGDIAdapterTopologyMap()
{
	ZeroMemory( &m_GDIDeviceSlotForDXGIAdapterSlot, sizeof( m_GDIDeviceSlotForDXGIAdapterSlot ) );
	ZeroMemory( &m_GDIDeviceSlotForDXGIOutputSlot, sizeof( m_GDIDeviceSlotForDXGIOutputSlot ) );
	ZeroMemory( &m_DXGIAdapterSlotForGDIDeviceSlot, sizeof( m_DXGIAdapterSlotForGDIDeviceSlot ) );
	ZeroMemory( &m_DXGIOutputSlotForGDIDeviceSlot, sizeof( m_DXGIOutputSlotForGDIDeviceSlot ) );
};


///////////////////////////////////////////////////////
// Function:  MapAdapterTopologies
//
// Purpose:
// Takes a GDI adapter topology and a DXGI adapter
// topology, and creates a mapping between them using
// the names of the underlying GDI display devices as
// the common piece of information.
///////////////////////////////////////////////////////
HRESULT CDXGIToGDIAdapterTopologyMap::MapAdapterTopologies
(
	CGDIAdapterTopology & GDITopology,
	CDXGIAdapterTopology & DXGITopology
)
{
	HRESULT hOverallResult = E_FAIL;


	for
	(
		UINT iCurrentDXGIOutputSlot = 0;
		iCurrentDXGIOutputSlot < DXGITopology.m_cOutputs;
		++iCurrentDXGIOutputSlot
	)
	{
		for
		(
			UINT iCurrentGDIDeviceSlot = 0;
			iCurrentGDIDeviceSlot < GDITopology.m_cDevices;
			++iCurrentGDIDeviceSlot
		)
		{
			if
			(
				_wcsicmp
				(
					DXGITopology.m_OutputDescriptors[ iCurrentDXGIOutputSlot ].DeviceName,
					GDITopology.m_DeviceDescriptors[ iCurrentGDIDeviceSlot ].DeviceName
				) == 0
			)
			{
				m_DXGIOutputSlotForGDIDeviceSlot[ iCurrentGDIDeviceSlot ] = iCurrentDXGIOutputSlot;
				m_DXGIAdapterSlotForGDIDeviceSlot[ iCurrentDXGIOutputSlot ] = DXGITopology.m_AdapterSlotForOutputSlot[ iCurrentDXGIOutputSlot ];

				m_GDIDeviceSlotForDXGIOutputSlot[ iCurrentDXGIOutputSlot ] = iCurrentGDIDeviceSlot;
				m_GDIDeviceSlotForDXGIAdapterSlot[ DXGITopology.m_AdapterSlotForOutputSlot[ iCurrentDXGIOutputSlot ] ] = iCurrentGDIDeviceSlot;

				break;
			};
		};
	};


	hOverallResult = S_OK;

//Cleanup:
	return hOverallResult;
};
