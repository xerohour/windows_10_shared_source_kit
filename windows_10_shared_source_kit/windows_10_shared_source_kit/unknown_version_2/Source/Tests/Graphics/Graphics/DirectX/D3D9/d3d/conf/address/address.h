/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Address.h
 *
 ***************************************************************************/

#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Defines
#define MAX_TESTS		40

// Class definition
class CAddressTest: public CD3DTest
{
    // Private Data
	private:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CnTexture *		pTexture;
#if FAST_TEST
    // The buffer overflows when you do multiple tests on a RT...
    TCHAR			szStatus[8000];
    TCHAR			msgString[8000];
#else //FAST_TEST
	TCHAR			szStatus[80];
	TCHAR			msgString[80];
#endif

    // Public Data
	public:
	bool			bWrapUV;
	bool			bBorder;
	LPCTSTR			szAddrU;
	LPCTSTR			szAddrV;
	DWORD			dwAddressCapU;
	DWORD			dwAddressCapV;
	bool			bIndependentUV;

	public:
	CAddressTest();
	~CAddressTest();

	// Framework functions
	virtual bool CapsCheck(void);
	virtual UINT TestInitialize(void);
	virtual bool Setup(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
    virtual void UpdateStatus(void);
	virtual void Cleanup(void);

#if FAST_TEST
    bool AddFormatModes(DISPLAYMODE *pMode, bool bWindowed);
#endif

	// Helper functions
	void DrawGrid(int nTest);
	void DrawWrapGrid(int nTest);
};

// Wrap U Class definitions
class CWrapUTest: public CAddressTest
{
	public:
	CWrapUTest();

	bool Setup(void);
};

// Wrap V Class definitions
class CWrapVTest: public CAddressTest
{
	public:
	CWrapVTest();

	bool Setup(void);
};

// Wrap UV Class definitions
class CWrapUVTest: public CAddressTest
{
	public:
	CWrapUVTest();

	bool Setup(void);
};

// Wrap Class definitions
class CWrapTest: public CAddressTest
{
	public:
	CWrapTest();

	bool Setup(void);
};

// Mirror Class definitions
class CMirrorTest: public CAddressTest
{
	public:
	CMirrorTest();

	bool Setup(void);
};

// Clamp Class definitions
class CClampTest: public CAddressTest
{
	public:
	CClampTest();

	bool Setup(void);
};

// Border Class definitions
class CBorderTest: public CAddressTest
{
	public:
	CBorderTest();

	bool Setup(void);
};

// MirrorOnce Class definitions
class CMirrorOnceTest: public CAddressTest
{
	public:
	CMirrorOnceTest();

	bool Setup(void);
};

// HalfBorder Class definitions
/*
class CHalfBorderTest: public CAddressTest
{
	public:
	CHalfBorderTest();

	bool Setup(void);
};
*/
// HalfBorderPoint Class definitions
/*
class CHalfBorderPointTest: public CAddressTest
{
	public:
	CHalfBorderPointTest();

	bool Setup(void);
};
*/
// WrapMirror Class definitions
class CWrapMirrorTest: public CAddressTest
{
	public:
	CWrapMirrorTest();

	bool Setup(void);
};

// WrapClamp Class definitions
class CWrapClampTest: public CAddressTest
{
	public:
	CWrapClampTest();

	bool Setup(void);
};

// WrapBorder Class definitions
class CWrapBorderTest: public CAddressTest
{
	public:
	CWrapBorderTest();

	bool Setup(void);
};

// WrapMirrorOnce Class definitions
class CWrapMirrorOnceTest: public CAddressTest
{
	public:
	CWrapMirrorOnceTest();

	bool Setup(void);
};

// WrapHalfBorder Class definitions
/*
class CWrapHalfBorderTest: public CAddressTest
{
	public:
	CWrapHalfBorderTest();

	bool Setup(void);
};
*/
// MirrorWrap Class definitions
class CMirrorWrapTest: public CAddressTest
{
	public:
	CMirrorWrapTest();

	bool Setup(void);
};

// MirrorClamp Class definitions
class CMirrorClampTest: public CAddressTest
{
	public:
	CMirrorClampTest();

	bool Setup(void);
};

// MirrorBorder Class definitions
class CMirrorBorderTest: public CAddressTest
{
	public:
	CMirrorBorderTest();

	bool Setup(void);
};

// MirrorMirrorOnce Class definitions
class CMirrorMirrorOnceTest: public CAddressTest
{
	public:
	CMirrorMirrorOnceTest();

	bool Setup(void);
};

// MirrorHalfBorder Class definitions
/*
class CMirrorHalfBorderTest: public CAddressTest
{
	public:
	CMirrorHalfBorderTest();

	bool Setup(void);
};
*/
// ClampWrap Class definitions
class CClampWrapTest: public CAddressTest
{
	public:
	CClampWrapTest();

	bool Setup(void);
};

// ClampMirror Class definitions
class CClampMirrorTest: public CAddressTest
{
	public:
	CClampMirrorTest();

	bool Setup(void);
};

// ClampBorder Class definitions
class CClampBorderTest: public CAddressTest
{
	public:
	CClampBorderTest();

	bool Setup(void);
};

// ClampMirrorOnce Class definitions
class CClampMirrorOnceTest: public CAddressTest
{
	public:
	CClampMirrorOnceTest();

	bool Setup(void);
};

// ClampHalfBorder Class definitions
/*
class CClampHalfBorderTest: public CAddressTest
{
	public:
	CClampHalfBorderTest();

	bool Setup(void);
};
*/
// BorderWrap Class definitions
class CBorderWrapTest: public CAddressTest
{
	public:
	CBorderWrapTest();

	bool Setup(void);
};

// BorderMirror Class definitions
class CBorderMirrorTest: public CAddressTest
{
	public:
	CBorderMirrorTest();
	bool Setup(void);
};

// BorderClamp Class definitions
class CBorderClampTest: public CAddressTest
{
	public:
	CBorderClampTest();

	bool Setup(void);
};

// BorderMirrorOnce Class definitions
class CBorderMirrorOnceTest: public CAddressTest
{
	public:
	CBorderMirrorOnceTest();

	bool Setup(void);
};

// BorderHalfBorder Class definitions
/*
class CBorderHalfBorderTest: public CAddressTest
{
	public:
	CBorderHalfBorderTest();

	bool Setup(void);
};
*/
// MirrorOnceWrap Class definitions
class CMirrorOnceWrapTest: public CAddressTest
{
	public:
	CMirrorOnceWrapTest();

	bool Setup(void);
};

// MirrorOnceMirror Class definitions
class CMirrorOnceMirrorTest: public CAddressTest
{
	public:
	CMirrorOnceMirrorTest();

	bool Setup(void);
};

// MirrorOnceClamp Class definitions
class CMirrorOnceClampTest: public CAddressTest
{
	public:
	CMirrorOnceClampTest();

	bool Setup(void);
};

// MirrorOnceBorder Class definitions
class CMirrorOnceBorderTest: public CAddressTest
{
	public:
	CMirrorOnceBorderTest();

	bool Setup(void);
};

// MirrorOnceHalfBorder Class definitions
/*
class CMirrorOnceHalfBorderTest: public CAddressTest
{
	public:
	CMirrorOnceHalfBorderTest();

	bool Setup(void);
};
*/
// HalfBorderWrap Class definitions
/*
class CHalfBorderWrapTest: public CAddressTest
{
	public:
	CHalfBorderWrapTest();

	bool Setup(void);
};
*/
// HalfBorderMirror Class definitions
/*
class CHalfBorderMirrorTest: public CAddressTest
{
	public:
	CHalfBorderMirrorTest();
	bool Setup(void);
};
*/
// HalfBorderClamp Class definitions
/*
class CHalfBorderClampTest: public CAddressTest
{
	public:
	CHalfBorderClampTest();

	bool Setup(void);
};
*/
// HalfBorderBorder Class definitions
/*
class CHalfBorderBorderTest: public CAddressTest
{
	public:
	CHalfBorderBorderTest();

	bool Setup(void);
};
*/
// HalfBorderMirrorOnce Class definitions
/*
class CHalfBorderMirrorOnceTest: public CAddressTest
{
	public:
	CHalfBorderMirrorOnceTest();

	bool Setup(void);
};
*/
#endif
