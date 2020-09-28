#pragma once

#include "DynamicVB.h"

/************************************************************
 * Test specification										*
 ************************************************************/

class CStaticDynamicVBTest : public CDynamicVBTest
{
public:
	CStaticDynamicVBTest();
	virtual ~CStaticDynamicVBTest();
	
protected:
	UINT			m_StaticStride;				// dynamic vertex stride
	LPVERTEXBUFFERS	m_pStaticVertexBuffers;		// dynamic vertex buffers
	
	// helper methods
	virtual void	BuildTestVectors();				// virtual method for each test to initialize its vector list
	virtual void	CopyDynamicVertex( void* pDynamicVertex, D3DVERTEX* pShapeVertex, D3DCOLOR color );	// copy the shape vertex into the dynamic Vertex

	// SDK Framework methods
	virtual HRESULT ConfirmDevice(CAPS*,DWORD,FORMAT);	// Called during device initialization, this code checks the device for some minimum set of capabilities
	virtual HRESULT OneTimeSceneInit();					// Called during initial app startup, this function performs all the permanent initialization
	virtual HRESULT RestoreDeviceObjects();				// Restore device-memory objects and state after a device is created or resized.
	virtual HRESULT InvalidateDeviceObjects();			// Called when the device-dependent objects are about to be lost.
};
