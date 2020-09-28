#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include "dx8decl.h"
#include "types.h"
#include "CD3DTest.h"
#include "CShapes.h"
#include "CBuffers.h"
#include "CTextures.h"
#include <vector>
using namespace std;

#define		WIDTH	3
#define		HEIGHT	3

typedef struct
{
	UINT	OffsetToLock;
	UINT	SizeToLock;
	DWORD	Flags;
} Lock_Flags;

typedef vector<Lock_Flags> vectorLock_Flags;

/************************************************************
 * Test specification										*
 ************************************************************/

class CDynamicVBTest : public CD3DTest
{
public:
	CDynamicVBTest();
	virtual ~CDynamicVBTest();

	// framework virtual methods
	virtual void CommandLineHelp();					// command line help for the test
	virtual bool CapsCheck();						// Called once for each group before any test initialization has been done
	virtual UINT TestInitialize();					// Called once for each group if CapsCheck succeeds
	virtual bool TestTerminate();					// Called once for each group if TestInitialize succeeds
	virtual bool Setup();							// Called everytime devices are restored(group init, device lost, etc)
	virtual void Cleanup();							// Called everytime devices are restored(group init, device lost, etc)
	virtual bool ExecuteTest(UINT nTest);			// Called once for each test defined by SetTestRange()
	
protected:
	vector<DWORD>	m_arraydwLockNo;				// how many locks to do per frame
	vector<DWORD>	m_arraydwFlagsNo;				// which flags we want to test
	vector<DWORD>	m_arraydwOffsetNo;				// what are the locking offsets
	vector<DWORD>	m_arraydwVerticesNo;			// how many vertices do we lock each time
	UINT			m_TestIndex;					// index into the current test
	vector<vectorLock_Flags>	m_vectorTests;		// list of all the tests
	bool			m_bClearBetweenPresent;			// true to clear between present
	bool			m_bForceREFFailure;				// force ref failure for DISCARD cases

	CShapes			m_CubeShapes[WIDTH][HEIGHT];	// individual cube shapes
	CShapes			m_AllShapes;					// all the shapes in one composite shape

	LPINDEXBUFFERS	m_pIndexBuffers;				// index buffers
	LPTEXTURES		m_pTextures;					// dxlogo texture

	UINT			m_DynamicStride;				// dynamic vertex stride
	LPVERTEXBUFFERS	m_pDynamicVertexBuffers;		// dynamic vertex buffers
	CnVertexDeclaration *m_pVertexDeclarations;		// dynamic vertex declarations
	vector<VERTEXELEMENT>	m_VertexElements;		// dynamic vertex elements
	
	// DX8.1 emulation of declarations
	vector<DWORD>	m_p81VertexDeclaration;			// dx8.1 vertex declaration
	DWORD			m_p81VertexShaders;				// dx8.1 vertex shader

	// helper methods
	void			Check_HRFail( HRESULT hResult, const char* sMethod );	// if hResult is not D3D_OK, throws an exception
	void			BuildTestList();				// build test list
	virtual void	BuildTestVectors();				// virtual method for each test to initialize its vector list
	virtual void	CopyDynamicVertex( void* pDynamicVertex, D3DVERTEX* pShapeVertex, D3DCOLOR color );	// copy the shape vertex into the dynamic Vertex

	// SDK Framework methods
	virtual HRESULT ConfirmDevice(CAPS*,DWORD,FORMAT);	// Called during device initialization, this code checks the device for some minimum set of capabilities
    virtual HRESULT OneTimeSceneInit();					// Called during initial app startup, this function performs all the permanent initialization
    virtual HRESULT InitDeviceObjects();				// Initialize device-dependent objects. This is the place to create mesh and texture objects.
    virtual HRESULT RestoreDeviceObjects();				// Restore device-memory objects and state after a device is created or resized.
    virtual HRESULT FrameMove();						// Called once per frame, the call is the entry point for animating the scene.
    virtual HRESULT Render();							// Called once per frame, the call is the entry point for 3d rendering. This function sets up render states, clears the viewport, and renders the scene.
    virtual HRESULT InvalidateDeviceObjects();			// Called when the device-dependent objects are about to be lost.
    virtual HRESULT DeleteDeviceObjects();				// Called when the app is exiting, or the device is being changed, this function deletes any device dependent objects.
    virtual HRESULT FinalCleanup();						// Called before the app exits, this function gives the app the chance to cleanup after itself.
};
