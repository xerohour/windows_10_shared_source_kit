//////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 2000.
//
// multi_swapchain.h
//
// CSwpChainTest class - Renders different views to different swap chains.
//
// History: 12/18/2000 Bobby Paulose     - Created
//
//////////////////////////////////////////////////////////////////////


#ifndef __SWP_CHAIN_TEST_H__
#define __SWP_CHAIN_TEST_H__

#include "CD3DTest.h"
#include "d3d8.h"
#include "d3d8types.h"
//#include "3dmath.h"
#include "D3dx8math.h"
#include "CSwapChain.h"
#include "CBase3dObject.h"



// Get access to the framework
extern	CD3DWindowFramework* g_pFramework;



#define MAX_OBJECTS 5
#define MAX_TEXTURES 10

#define PI (float(22.0/7.0))
#define IMGHEIGHT	256
#define IMGWIDTH	256
#define NUM_SWPCHN	3
#define TAGHEIGHT	12 /* max 20 */
#define TAGWIDTH	85

struct OBJECT
{
	CBase3dObject *pObject;
	D3DMATRIX World, View, Projection;
};

// Class definition
class CSwpChainTest: public CD3DTest
{
	public:

	UINT numObjects;
	struct OBJECT objects[MAX_OBJECTS];

	UINT numTextures;
	CnTexture *pTextures[MAX_TEXTURES];

	CSwapChain* pAdditionalSrcSwpChn[NUM_SWPCHN];
	CSwapChain* pAdditionalRefSwpChn[NUM_SWPCHN];
	D3DMATRIX ViewSwpChn[NUM_SWPCHN];


	CSwpChainTest();
	~CSwpChainTest();
	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame();
	virtual bool DisplayFrame(void);
	virtual bool TestTerminate(void);
	
	virtual bool Setup();
	virtual void Cleanup();


	//helper fns
	//void DisplaySetup(void);
};

#endif  //__SWP_CHAIN_TEST_H__

