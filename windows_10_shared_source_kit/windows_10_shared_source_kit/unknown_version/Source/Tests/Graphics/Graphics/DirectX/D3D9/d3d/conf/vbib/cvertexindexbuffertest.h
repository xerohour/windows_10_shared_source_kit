//////////////////////////////////////////////////////////////////////
// File:  CVertexIndexBuffer.h
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for vertex and index buffer conformance tests.
//
// History:
// 27 Oct 2005  SEdmison  Cleaned up.
//////////////////////////////////////////////////////////////////////

#ifndef __CVertexIndexBufferTest_H__
#define __CVertexIndexBufferTest_H__


/////////////////////////////////////////////////////////////////
//
// Build flags
//
/////////////////////////////////////////////////////////////////

#define TESTDXVER 0x0800

#define STRICT


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////


//
// Win32 headers
//

#include <d3d8.h>
#include <tchar.h>
#include <windows.h>
#include <types.h>


//
// Framework headers
//

#include <CD3DTest.h>
#include <TestUtils.h>
#include <CShapes.h>


/////////////////////////////////////////////////////////////////
//
// Macros
//
/////////////////////////////////////////////////////////////////

#define COUNTOF(a) (sizeof(a)/sizeof((a)[0]))

#define ELEMENT_AND_DESCRIPTION(a) {a,#a}

#define CLOSEHANDLE(h) { if ((h) != NULL) { CloseHandle(h); (h) = NULL;}; }


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

typedef struct _DiscardDynamicTestCase
{
	UINT nTest;

	DWORD Usage;
	DWORD Pool;
	DWORD LockFlag1;
	DWORD LockFlag2;
} DiscardDynamicTestCase;


typedef struct __D3DTEST_NORMALBUFFER
{
	LPTSTR szName;

	CShapes *shape;

	DWORD VBFVF;
	FMT IBFormat;

	DWORD Usage;
	DWORD Pool;
} D3DTEST_NORMALBUFFER, *LPD3DTEST_NORMALBUFFER;


typedef struct __D3DTEST_OPTIMIZEDBUFFER
{
	LPTSTR szName;

	CShapes *shape;
	UINT iReps;

	DWORD VBFVF;
	FMT IBFormat;

	DWORD SrcUsage;
	DWORD SrcPool;

	DWORD RefUsage;
	DWORD RefPool;
} D3DTEST_OPTIMIZEDBUFFER, *LPD3DTEST_OPTIMIZEDBUFFER;


typedef struct _HugeBufferTestCase
{
	LPTSTR szName;

	CShapes *shape;
	UINT iReps;

	DWORD VBFVF;
	FMT IBFormat;

	DWORD Usage;
	DWORD Pool;
} HugeBufferTestCase;


// Test case data for InternalPointerAlias.
typedef struct _InternalPointerAliasTestCase
{
    UINT nTest;

    UINT Usage;
    UINT Pool;
    UINT LockFlag;
} InternalPointerAliasTestCase;


typedef struct _LockActiveTestStage
{
	LPCTSTR szName;
	BOOL fCreate;
	BOOL fAppendVB;
	BOOL fAppendIB;
	BOOL fDraw1;
	BOOL fDraw2;
} LockActiveTestStage;


typedef struct _LockActiveTestCase
{
	UINT nTest;

	UINT Usage;
	UINT Pool;
	UINT LockFlag;
	UINT TestStage;
	bool bCompleted;
} LockActiveTestCase;


typedef struct _LockPointerAliasTestCase
{
	UINT nTest;

    UINT Usage;
    UINT Pool;
    UINT LockFlags;
    UINT DoIndex;
} LockPointerAliasTestCase;


typedef struct _LostOnResetTestCase
{
	UINT nTest;

	UINT Usage;
	UINT Pool;
} LostOnResetTestCase;


typedef struct _MultipleLocksTestCase
{
	UINT nTest;

	UINT Usage;
	UINT Pool;
	UINT LockFlags;
} MultipleLocksTestCase;


typedef struct _NormalBufferTestCase
{
	UINT nTest;

    UINT Usage;
    UINT Pool;
} NormalBufferTestCase;


typedef struct _PriorityTestCase
{
	UINT nTest;

    UINT Usage;
    UINT Priority;
	FMT IndexFormat;
} PriorityTestCase;


typedef struct _RelockDynamicTestCase
{
	UINT nTest;

	UINT Usage;
	UINT Pool;
	UINT LockFlag1;
	UINT LockFlag2;
} RelockDynamicTestCase;


typedef struct _SwitchRenderStateTestCase
{
	UINT nTest;

    UINT Usage;
    UINT Pool;
    UINT TestStage;
} SwitchRenderStateTestCase;


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CVertexIndexBufferTest
//
// Purpose:
// Base class for vertex buffer and index buffer test
// groups.
///////////////////////////////////////////////////////
class CVertexIndexBufferTest
:
	public CD3DTest
{
	//
	// Construction / destruction
	//

	public:
		CVertexIndexBufferTest();


	//
	// Helper methods
	//

	protected:
		SIZE_T GetAvailMemory( DWORD Pool );

	//
	// Static helper methods
	//

	protected:
		// Determines which types of exceptions are eligible by handling in
		// the application's __except block.
		static int ExceptionFilter
		(
			unsigned int ExceptionCode,
			_EXCEPTION_POINTERS * pExceptionDetails
		);

		// Builds a human-readable string describing the specified adapter.
		static bool GetDisplayString
		(
			UINT Adapter,
			LPTSTR pDeviceName,
			UINT nDeviceName
		);

#if FAST_TEST
        UINT m_totalNumTestCases;
        template<typename DrawFunctionType>
        bool DrawMultiTests(DrawFunctionType pfnDraw)
        {
            bool pass = true;

            const DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
            const DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
            UINT numColumns = (UINT)ceil(sqrt((float)m_totalNumTestCases));
            UINT numRows = (UINT)ceil(sqrt((float)m_totalNumTestCases));;
            const float widthPerOp = float(dwWidth) / float(numColumns);
            const float heightPerOp = float(dwHeight) / float(numRows);

            for (size_t i = 0; i < m_totalNumTestCases; i++)
            {
                VIEWPORT viewport;
                viewport.X = (i % numColumns) * UINT(widthPerOp);
                viewport.Y = (i / numColumns) * UINT(heightPerOp);
                viewport.Width = UINT(widthPerOp);
                viewport.Height = UINT(heightPerOp);
                viewport.MinZ = 0.0;
                viewport.MaxZ = 1.0;

                SetViewport(&viewport);

                pass = pfnDraw(i + 1);//Plus 1 because tests are 1 indexed
                if (pass == false)
                {
                    break;
                }
            }

            return pass;
        }
#endif//FAST_TEST

	//
	// Member data
	//

	public:
		CShapes m_shape;
};


///////////////////////////////////////////////////////
// Class:  DiscardDynamic
//
// Purpose:
//
///////////////////////////////////////////////////////
class DiscardDynamic
:
	public CVertexIndexBufferTest
{
	//
	// Construction / destruction
	//

	public:
		DiscardDynamic();


	//
	// Framework override methods
	//

	public:
		bool ExecuteTest(UINT nTest);

		UINT TestInitialize();


	//
	// Helper methods
	//

	protected:
		D3DTESTINIT TestInitReal
		(
			DWORD dwFlags = CD3D_ALL
		);

		BOOL ExecuteReal
		(
			DWORD Usage,
			DWORD Pool,
			DWORD LockFlags1,
			DWORD LockFlags2,
			BOOL fMixedMode,
			DWORD dwFlags = CD3D_ALL
		);


	//
	// Member data
	//

	protected:
		CShapes m_shape2;


	//
	// Static data
	//

	public:
		static const DWORD Usages[];

		static const DWORD Pools[];

		static const DWORD LockFlags[];
};


///////////////////////////////////////////////////////
// Class:  HugeBuffer
//
// Purpose:
//
///////////////////////////////////////////////////////
class HugeBuffer
:
	public CVertexIndexBufferTest
{
	//
	// Construction / destruction
	//

	public:
		HugeBuffer();


	//
	// Framework method overrides
	//

	public:
		bool ExecuteTest(UINT nTest);

		UINT TestInitialize();


	//
	// Helper methods
	//

	protected:
		D3DTESTINIT TestInitReal
		(
			DWORD dwFlags = CD3D_ALL
		);

		BOOL ExecuteReal
		(
			const HugeBufferTestCase *lpCIB,
			BOOL fMixedMode,
			DWORD dwFlags = CD3D_ALL
		);

#if FAST_TEST
        HugeBufferTestCase m_pTestCases[15];
#endif


	//
	// Member data
	//

	protected:
		CShapes m_shape15bit;
		CShapes m_shape16bit;

		SIZE_T m_MemSize;

		HugeBufferTestCase HugeBufferTestCases[ 15 ];
};


///////////////////////////////////////////////////////
// Class:  InternalPointerAlias
//
// Purpose:
//
///////////////////////////////////////////////////////
class InternalPointerAlias
:
	public CVertexIndexBufferTest
{
	//
	// Construction / destruction
	//

	public:
		InternalPointerAlias();
		~InternalPointerAlias();


	//
	// Framework override methods
	//

	public:
		bool ExecuteTest(UINT nTest);

		UINT TestInitialize();


		DWORD ThreadProc();


	//
	// Static helper methods
	//

	public:
		static DWORD WINAPI StaticThreadProc
		(
			VOID * pContext
		);


	//
	// Static data
	//

	public:
		static const DWORD InternalPointerAlias::Usages[];

		static const DWORD InternalPointerAlias::Pools[];

		static const DWORD InternalPointerAlias::LockFlags[];

		static TCHAR s_ModeThreadError[ MAX_PATH ];

		static DEVMODE s_OriginalMode;

		static DEVMODE s_AlternateMode;

		static TCHAR s_DeviceName[ 32 ];


	//
	// Member data
	//

	protected:
		CRITICAL_SECTION m_ResetLock;

		HANDLE m_phEvents[ 3 ];

		DWORD m_dwBehaviorOverride;

		BOOL m_bSWVP;
};


///////////////////////////////////////////////////////
// Class:  LockActive
//
// Purpose:
//
///////////////////////////////////////////////////////
class LockActive
:
	public CVertexIndexBufferTest
{
	//
	// Construction / destruction
	//
	public:
		LockActive();


	//
	// Framework override methods
	//

	public:
		bool ExecuteTest(UINT nTest);

		UINT TestInitialize();

		bool TestTerminate();


	//
	// Helper methods
	//

	protected:
		D3DTESTINIT TestInitReal( DWORD dwFlags );

		BOOL ExecuteReal
		(
			LPVERTEXBUFFERS *ppVB,
			LPINDEXBUFFERS *ppIB,
			DWORD Usage,
			DWORD Pool,
			DWORD LockFlags,
			UINT iTestStage,
			BOOL fMixedMode,
			DWORD dwFlags
		);


	//
	// Member data
	//

	protected:
		LPVERTEXBUFFERS m_pVB;
		LPINDEXBUFFERS  m_pIB;

		CShapes m_shape2;


	public:
	    static const LockActiveTestStage Stages[];

		static const DWORD Usages[];

		static const DWORD Pools[];

		static const DWORD LockFlags[];
};


///////////////////////////////////////////////////////
// Class:  LockPointerAlias
//
// Purpose:
//
///////////////////////////////////////////////////////
class LockPointerAlias
:
	public CVertexIndexBufferTest
{
	//
	// Construction / destruction
	//

	public:
		LockPointerAlias();


	//
	// Framework override methods
	//

	public:
		UINT TestInitialize();
		bool ExecuteTest(UINT nTest);


	//
	// Member data
	//

	protected:
		DWORD m_dwBehaviorOverride;
		BOOL  m_dwSW;
		DWORD m_dwAdapterID;
		DWORD m_DevType;


	//
	// Static data
	//

	public:
		static const DWORD Usages[];

		static const DWORD Pools[];

		static const DWORD LockFlags[];
};


///////////////////////////////////////////////////////
// Class:  LostOnReset
//
// Purpose:
// Tests for losing vertex and index buffers after
// a call to Reset.
///////////////////////////////////////////////////////
class LostOnReset
:
	public CVertexIndexBufferTest
{
	//
	// Construction / destruction
	//

	public:
		LostOnReset();


	//
	// Framework override methods
	//

	public:
		bool ExecuteTest(UINT nTest);

		UINT TestInitialize();

		bool TestTerminate();


	//
	// Helper methods
	//

	protected:
		D3DTESTINIT TestInitReal();

		BOOL ExecuteReal
		(
			const D3DTEST_NORMALBUFFER *lpCIB
		);


	//
	// Member data
	//

	protected:
		PRESENT_PARAMETERS  m_SrcParam;
		LPDEVICE            m_pDevice;
		bool                m_bMixedMode;
		BOOL                m_dwSW;
		DWORD               m_dwDeviceID;
		DWORD               m_DevType;
		DWORD               m_dwBehaviorFlags;


	//
	// Static data
	//

	public:
		static const DWORD Usages[];

		static const DWORD Pools[];
};


///////////////////////////////////////////////////////
// Class:  MultipleLocks
//
// Purpose:
//
///////////////////////////////////////////////////////
class MultipleLocks
:
	public CVertexIndexBufferTest
{
	//
	// Construction / destruction
	//

	public:
		MultipleLocks();


	//
	// Framework override methods
	//

	public:
		bool ExecuteTest(UINT nTest);

		UINT TestInitialize();


	//
	// Helper methods
	//

	protected:
		D3DTESTINIT TestInitReal
		(
			DWORD dwFlags = CD3D_ALL
		);

		BOOL ExecuteReal
		(
			DWORD Usage,
			DWORD Pool,
			DWORD LockFlags,
			BOOL fMixedMode,
			DWORD dwFlags = CD3D_ALL
		);


	//
	// Static data
	//

	public:
		static const DWORD Usages[];

		static const DWORD Pools[];

		static const DWORD LockFlags[];
};


///////////////////////////////////////////////////////
// Class:  NormalBuffer
//
// Purpose:
//
///////////////////////////////////////////////////////
class NormalBuffer
:
	public CVertexIndexBufferTest
{
	//
	// Construction / destruction
	//

	public:
		NormalBuffer();


	//
	// Framework override methods
	//

	public:
		bool ExecuteTest(UINT nTest);

		UINT TestInitialize();


	//
	// Helper methods
	//

	protected:
		D3DTESTINIT TestInitReal
		(
			IN DWORD dwFlags = CD3D_ALL
		);

		BOOL ExecuteReal
		(
			IN const D3DTEST_NORMALBUFFER *lpCIB,
			IN DWORD dwFlags = CD3D_ALL
		);

		bool ConvertToVertexBuffer
		(
			CnVertexBuffer **pNewBuffer,
			D3DTEST_NORMALBUFFER lpBufInfo,
			LPVOID lpVertices,
			DWORD dwVertexCount,
			DWORD dwVertexSize,
			LPVOID lpIndices,
			DWORD  dwIndexCount,
			DWORD dwFlags
		);


	//
	// Static data
	//

	public:
		static const DWORD Usages[];

		static const DWORD Pools[];
};


///////////////////////////////////////////////////////
// Class:  OptimizedBuffer
//
// Purpose:
//
///////////////////////////////////////////////////////
class OptimizedBuffer
:
	public CVertexIndexBufferTest
{
	//
	// Construction / destruction
	//

	public:
		OptimizedBuffer();


	//
	// Framework override methods
	//

	public:
		bool ExecuteTest(UINT nTest);

		UINT TestInitialize();


	//
	// Helper methods
	//

	protected:
		BOOL ShapeReplicate(UINT iTimes);

		D3DTESTINIT TestInitReal
		(
			DWORD dwFlags = CD3D_ALL
		);

		BOOL ExecuteReal
		(
			const D3DTEST_NORMALBUFFER *lpCIB,
			UINT iReps,
			BOOL fMixedMode,
			DWORD dwFlags = CD3D_ALL
		);


	//
	// Member data
	//

	protected:
		LARGE_INTEGER m_liFreq;
		CShapes m_shape15bit;
		CShapes m_shape16bit;
};


///////////////////////////////////////////////////////
// Class:  Priority
//
// Purpose:
//
///////////////////////////////////////////////////////
class Priority
:
	public CVertexIndexBufferTest
{
	//
	// Construction / destruction
	//

	public:
		Priority();


	//
	// Framework override methods
	//

	public:
		bool ExecuteTest(UINT nTest);

		UINT TestInitialize();


	//
	// Helper methods
	//

	protected:
		D3DTESTINIT TestInitReal( DWORD dwFlags = CD3D_ALL );

		BOOL ExecuteReal
		(
			const D3DTEST_NORMALBUFFER *lpCIB,
			DWORD dwThisPriority,
			BOOL fMixedMode,
			DWORD dwFlags = CD3D_ALL
		);

		LPVOID CreateVertexArray
		(
			LPVOID lpNewVertices,
			LPVOID lpVertices,
			DWORD  dwVertexCount,
			DWORD dwVertexSize, 
			LPVOID lpIndices,
			DWORD dwIndexCount
		);

		bool ConvertToVertexBuffer
		(
			CnVertexBuffer **pNewBuffer,
			D3DTEST_NORMALBUFFER lpBufInfo,
			LPVOID lpVertices,
			DWORD dwVertexCount,
			DWORD dwVertexSize,
			LPVOID lpIndices,
			DWORD dwIndexCount,
			BOOL fMixedMode,
			DWORD dwFlags
		);


	//
	// Static data
	//

	public:
		static const DWORD Usages[];

		static const DWORD Priorities[];

		static const FMT IndexFormats[];
};


///////////////////////////////////////////////////////
// Class:  RelockDynamic
//
// Purpose:
// Tests for re-locking of dynamic buffers.
///////////////////////////////////////////////////////
class RelockDynamic
:
	public CVertexIndexBufferTest
{
	//
	// Construction / destruction
	//

	public:
		RelockDynamic();


	//
	// Framework override methods
	//

	public:
		bool ExecuteTest(UINT nTest);

		UINT TestInitialize();


	//
	// Helper methods
	//

	protected:
		D3DTESTINIT TestInitReal
		(
			DWORD dwFlags = CD3D_ALL
		);

		BOOL ExecuteReal
		(
			DWORD Usage,
			DWORD Pool,
			DWORD LockFlags1,
			DWORD LockFlags2,
			BOOL fMixedMode,
			DWORD dwFlags = CD3D_ALL
		);


	//
	// Member data
	//

	protected:
		CShapes m_shape2;


	//
	// Static data
	//

	public:
		static const DWORD Usages[];

		static const DWORD Pools[];

		static const DWORD LockFlags[];
};


///////////////////////////////////////////////////////
// Class:  SwitchRenderState
//
// Purpose:
//
///////////////////////////////////////////////////////
class SwitchRenderState
:
	public CVertexIndexBufferTest
{
	//
	// Construction / destruction
	//

	public:
		SwitchRenderState();


	//
	// Framework override methods
	//

	public:
		bool ExecuteTest(UINT nTest);

		UINT TestInitialize();


	//
	// Helper methods
	//

	protected:
		D3DTESTINIT TestInitReal
		(
			DWORD dwFlags = CD3D_SRC
		);

		BOOL ExecuteReal
		(
			const D3DTEST_NORMALBUFFER *curtest,
			BOOL fReference,
			DWORD dwFlags = CD3D_SRC
		);


	//
	// Static data
	//

	public:
		static const DWORD Usages[];

		static const DWORD Pools[];

		static const D3DTEST_NORMALBUFFER TestStages[];
};


#endif

