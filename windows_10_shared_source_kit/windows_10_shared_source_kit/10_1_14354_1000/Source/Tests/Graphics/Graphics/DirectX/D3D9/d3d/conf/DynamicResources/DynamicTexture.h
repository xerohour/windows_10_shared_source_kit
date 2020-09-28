#pragma once

#include <d3d9.h>
#include <d3dx9.h>
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
	UINT	Level;
	DWORD	indexRectToLock;
	DWORD	indexCubeToLock;
	DWORD	Flags;
} Texture_Lock_Flags;

typedef vector<Texture_Lock_Flags> vectorTexture_Lock_Flags;

/************************************************************
 * Test specification										*
 ************************************************************/

class CDynamicTextureTest : public CD3DTest
{
public:
	CDynamicTextureTest();
	virtual ~CDynamicTextureTest();

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
	vector<DWORD>	m_arraydwIndexRectNo;			// what are the locking rects ?
	UINT			m_TestIndex;					// index into the current test
	vector<vectorTexture_Lock_Flags>	m_vectorTests;		// list of all the tests
	bool			m_bClearBetweenPresent;			// true to clear between present
	bool			m_bForceREFFailure;				// force ref failure for DISCARD cases

	CShapes			m_CubeShapes[WIDTH][HEIGHT];	// individual cube shapes
	CShapes			m_AllShapes;					// all the shapes in one composite shape

	LPINDEXBUFFERS	m_pIndexBuffers;				// index buffers
	LPTEXTURES		m_pTextures;					// test texture (4x4 colored rects)

	UINT			m_VBStride;						// vertex stride
	LPVERTEXBUFFERS	m_pVertexBuffers;				// vertex buffers
	CnVertexDeclaration *m_pVertexDeclarations;		// vertex declarations
	vector<VERTEXELEMENT>	m_VertexElements;		// vertex elements
	
	// DX8.1 emulation of declarations
	vector<DWORD>	m_p81VertexDeclaration;			// dx8.1 vertex declaration
	DWORD			m_p81VertexShaders;				// dx8.1 vertex shader

	// helper methods
	void			Check_HRFail( HRESULT hResult, const char* sMethod );	// if hResult is not D3D_OK, throws an exception
	void			BuildTestList();				// build test list
	virtual void	BuildTestVectors();				// virtual method for each test to initialize its vector list
	virtual void	CopyDynamicTexture( LOCKED_RECT *pLockedRect, bool bAbsoluteLockedAddress, RECT *pRect, D3DCOLOR color );	// update the texture

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
