// Copyright(C) Microsoft.All rights reserved.

#pragma once

#define INITGUID    
#include <windows.h>	
#include <tchar.h>
#ifndef BUILD_TAEF
#include <shlflags.h>
#include <shltypes.h>
#include <shell98.h>
#else
#include <module_taef.h>
#endif
#include <TaefModuleAdapter.h>

#include <mmsystem.h>
#include <ks.h>
#include <ksmediap.h>
#include <dmusprop.h>
#include <dsound.h>

#include <api2ks.h>
#include <time.h>
#include <devaudio.h>
#include <dvmodule.h>
#include <stdio.h>
#include <devioctl.h>
#include <atlbase.h>
#include <atlconv.h>
#include <arrayp.h>
#include <kslib.h>

#include "math.h"
#include "resource.h"
#include "testcase.h"
#include "utils.h"
#include "helper.h"
#include "audio.h"
#include "audioengine.h"
#include "kmixer.h"
#include "pins.h"
#include "connection.h"
#include "topology.h"
#include "allocator.h"
#include "sysaudio.h"
#include "ksio.h"
#include "createp.h"
#include "wdmaudio.h"
#include "synth.h"
#include "synthdls.h"
#include "drmaudio.h"
#include "audiogfx.h"
#include "ds3dbuffer.h"
#include "ds3dlistener.h"
#include "topologynode.h"


#undef _NTDDK_
typedef ULONGLONG DWORDLONG;
#define KS_NO_CREATE_FUNCTIONS

#include <crtdbg.h>
#include <limits.h>
#include <tchar.h>
#include <arrayp.h>

// ------------------------------------------------------------------------------
// Create pin helper prototype from ksuser
typedef DWORD
( WINAPI *KSCREATEPIN)(
    IN HANDLE FilterHandle,
    IN PKSPIN_CONNECT Connect,
    IN ACCESS_MASK DesiredAccess,
    OUT PHANDLE ConnectionHandle
    );

// ------------------------------------------------------------------------------
// Create clock prototype from ksuser
typedef DWORD
(WINAPI *KSCREATECLOCK)
(
    HANDLE, 
    PKSCLOCK_CREATE, 
    PHANDLE
);

// ------------------------------------------------------------------------------
// Device definition
class CGauditDevice : public CDeviceDescriptor
{
public:
    CKsFilter*  m_pFilter;	// device under test
    DWORD     m_dwFlags;  // device flags
public:
    CGauditDevice(CKsFilter* pFilter, DWORD dwFlags);
    ~CGauditDevice();
};

class CFramework;

// ----------------------------------------------------------
// Test Module Class declaration
class CGauditModule : public CDeviceTestModule
{
    CGauditDevice*         m_pDevice;  // current device 
    PGAUDITTESTDEF	 m_pTestDef; // current test def
    HINSTANCE		 m_hInstance;

    HMODULE                  m_hKsUserDll;
    KSCREATEPIN           m_fnCreatePin;
    KSCREATECLOCK      m_fnCreateClock;
public:
#ifndef BUILD_TAEF
    CGauditModule(CTestShell* pShell, HINSTANCE hInstDLL);
#else
    CGauditModule(DV_TCINFO * ptcInfo);
#endif
    virtual ~CGauditModule(void);

    // CTestModule virtuals
    virtual DWORD   OnInitialUpdate(void);
    virtual void         SetTestDevice(CDeviceDescriptor* pDevice);
    virtual DWORD   EndTestRun(void);
    virtual DWORD   StartTestRun(void);
    virtual DWORD   RunTest(DWORD dwTestCase);
    
    // Properties
    CGauditDevice*      GetDevice()  const   { return m_pDevice;  }
    PGAUDITTESTDEF   GetTestDef() const   { return m_pTestDef; }
    CFramework*         GetFramework() const;
    HMODULE               GetKsUserHandle() const { return m_hKsUserDll; }
    KSCREATEPIN         GetCreatePinFn() const { return m_fnCreatePin; }
    KSCREATECLOCK   GetCreateClockFn() const { return m_fnCreateClock; }

protected:
    // Enumerate all devices to test
    BOOL    EnumerateDevices(void);
	
    // Profile based filtering helpers
    void    LoadSettings(void);
};


// Framework class
class CFramework
{
	// KSLIB globals 
	CKsFilter* m_pFilter;
	CKsPin*    m_pPin;
	CKsNode*   m_pNode;
	ULONG      m_ulDeviceId;

public:
	CFramework();


	// Operations
	void Reset();
	void NewFilter(CKsFilter*);
	void NewPin(CKsPin*);
	void NewNode(CKsNode*);
	void NewDevice(ULONG);
	void NewNodePin(CKsNode*);

	// Accessors
	CKsFilter* GetFilter() const { return m_pFilter; }
	CKsPin* GetPin() const { return m_pPin; }
	CKsNode* GetNode() const { return m_pNode; }
	ULONG GetDevice() const { return m_ulDeviceId; }

};


// Macros
#define INVALID_HANDLE (HANDLE)-1

#define SAFE_CLOSE_HANDLE(p)  { if (p != NULL && p != INVALID_HANDLE)   \
                                    CloseHandle(p);     \
                                p = NULL;  \
                              } \

#define SAFE_DELETE(p)  { if (p) delete p; p = NULL; }
#define SAFE_DELETEA(p) { if (p) delete[] p; p = NULL; }
#define REFER_FORMAL_PARAM(x) x

//	Memory allocation macros
#define ALIGN_32    3
#define ALIGN_64    7

#define K1   1024
#define PAGE 2* 4 * K1

// External globals from gaudit.cpp
#ifndef BUILD_TAEF
extern ITestShell*                 g_IShell;
#else
extern CTaefShellStub*             g_IShell;
#endif
extern CGauditModule*              g_pGauditModule;
extern ULONG g_nIndentLevel;

extern KSDATAFORMAT_WAVEFORMATEX AudioPinFormat;
extern KSDATAFORMAT_DSOUND DsoundPinFormat;
extern KSDATAFORMAT MidiPinFormat;  
extern ULONG PinType; // make it enum type 
// if pintype = 1 means ausiopin with the format = AudioPinFormat
// if pintype = 2 means ausiopin with the format = DsoundPinFormat
// if pintype = 3 means ausiopin with the format = MidiPinFormat
// List helpers
#define FOR_EACH_FILTER(list, node)	\
	for(node = list.GetHead(); node; node = list.GetNext(node)) {

#define END_FOR_EACH()	}

#define CLEANUP_FILTER_LIST(list)	\
	{	\
		for(TTNode<CKsFilter>* pnFilter = list.GetHead(); pnFilter; pnFilter = list.GetNext(pnFilter))	\
		{	\
			delete pnFilter->pData;	\
		}	\
		list.Empty();				\
	}

// Device type
typedef enum { UNKNOWN_DEVICE=0, SYSAUDIO_DEVICE, KMIXER_DEVICE, AEC_DEVICE, WDMAUD_DEVICE } eDevice;


// Shell macros
#define INCREMENT_INDENT() { UINT indent = 0; indent = g_IShell->IncrementIndent(); g_nIndentLevel++; }
#define DECREMENT_INDENT() { UINT indent = 0 ; indent = g_IShell->DecrementIndent(); g_nIndentLevel--; }
#ifndef SLOG
#define SLOG			   g_IShell->Log
#endif


// Trace redefinition
#define LOG_LEVEL_VERBOSE   eInfo2
#define LOG_LEVEL_TERSE     eInfo1
#define LOG_LEVEL_ERROR     eError


// Accessors for framework variables
#define FRAMEWORK()			g_pGauditModule->GetFramework()
#define FILTER()			FRAMEWORK()->GetFilter()
#define PIN()				FRAMEWORK()->GetPin()
#define NODE()				FRAMEWORK()->GetNode()
#define DEVICE()			FRAMEWORK()->GetDevice()

#define DisplayGUID(g)	g.Data1, g.Data2,g.Data3,g.Data4[0],g.Data4[1],g.Data4[2],g.Data4[3],g.Data4[4],g.Data4[5],g.Data4[6],g.Data4[7]

#define STATIC_KSPIN_INVALID_MEDIUM \
    0xe1ceb2a1, 0x5bf, 0x4533, 0x93, 0x7d, 0xe6, 0xc8, 0x6b, 0x26, 0x54, 0x79
DEFINE_GUIDSTRUCT("E1CEB2A1-05BF-4533-937D-E6C86B265479", KSPIN_INVALID_MEDIUM);
#define KSPIN_INVALID_MEDIUM DEFINE_GUIDNAMED(KSPIN_INVALID_MEDIUM)



