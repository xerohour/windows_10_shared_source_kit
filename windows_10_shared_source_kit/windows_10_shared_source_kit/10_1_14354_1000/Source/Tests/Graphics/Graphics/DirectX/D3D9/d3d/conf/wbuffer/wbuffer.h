/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	WBuffer.h
 *
 ***************************************************************************/

#ifndef __WBUFFER_H__
#define __WBUFFER_H__

#include "CD3DTest.h"

// Defines
#define MAX_TESTS	100

// Base Class definitions
class CWBufferTest: public CD3DTest
{
    // Data
    private:

    struct LVERTEX {
        D3DVALUE     x;             /* Homogeneous coordinates */
        D3DVALUE     y;
        D3DVALUE     z;
        D3DCOLOR     color;         /* Vertex color */
        D3DCOLOR     specular;      /* Specular component of vertex */
        D3DVALUE     tu;            /* Texture coordinates */
        D3DVALUE     tv;

        LVERTEX() { }
        LVERTEX(const D3DVECTOR& v,
                    D3DCOLOR _color, D3DCOLOR _specular,
                    float _tu, float _tv)
            { x = v.x; y = v.y; z = v.z; 
              color = _color; specular = _specular;
              tu = _tu; tv = _tv;
            }
    };

	LVERTEX		WaffleList[2][42];
	LVERTEX		PlaneList[4];
	TCHAR		msgString[80];

	public:
	CWBufferTest();
	~CWBufferTest();

	// Framework functions
	virtual bool CapsCheck(void);
	virtual bool SetDefaultRenderStates(void);
	virtual bool SetDefaultMatrices(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT uTestNum);
	virtual void SceneRefresh(void);
};

#endif
