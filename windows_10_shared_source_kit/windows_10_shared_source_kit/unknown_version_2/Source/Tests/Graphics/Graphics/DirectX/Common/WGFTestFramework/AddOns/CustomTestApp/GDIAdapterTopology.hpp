//////////////////////////////////////////////////////////////////////
// File:  GDIIAdapterTopology.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a helper object to keep track of the GDI adapter
// and output topology.
//
// History:
// 07 Jan 2007  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Pragmas
//
/////////////////////////////////////////////////////////////////

#pragma once


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// C runtime headers
//

#include <assert.h>


//
// Win32 headers
//

#include <windows.h>


//
// Project headers
//

#include "CustomTestApp.hpp"


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CGDIAdapterTopology
//
// Purpose:
// Manages information about the GDI adapters and
// outputs connected to a machine.
///////////////////////////////////////////////////////
class CGDIAdapterTopology
{
	public:
		CGDIAdapterTopology();

		virtual ~CGDIAdapterTopology();


	public:
		void AttachLogger
		(
			__in ISimpleLog * pLogger
		);


		HRESULT EnumerateAdaptersAndOutputs
		(
			BOOL bIncludeDetachedDevices = FALSE
		);


		HRESULT CauseModeChange
		(
			UINT Monitor = MONITOR_UNSPECIFIED,
			DWORD Width = 0,
			DWORD Height = 0,
			DWORD BitDepth = 0,
			DWORD RefreshRate = 0,
			DWORD Orientation = DMDO_DEFAULT,
			DWORD Flags = CDS_UPDATEREGISTRY
		);


		HRESULT RestoreOriginalMode
		(
			UINT Monitor = MONITOR_UNSPECIFIED
		);


	public:
		ISimpleLog * m_pLogger;

		UINT m_cDevices;

		DISPLAY_DEVICEW m_DeviceDescriptors[ 30 ];

		UINT m_DeviceOrdinalForDeviceSlot[ 30 ];
		UINT m_DeviceSlotForMonitorSlot[ MONITOR_SLOTS ];

		DEVMODEW m_DeviceModes[ 30 ];
};
