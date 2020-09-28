AllocationFlags;
    GENERIC_BUFFER DescriptorBuffer;
    PMEMORY_DESCRIPTOR MemoryRange;
    PLIST_ENTRY NextEntry;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;
    UNPACKED_MEMORY_TYPE UnpackedMemoryType;
    PVOID VirtualAddress;

    ReturnStatus = STATUS_SUCCESS;

    //
    // Obtain a memory map of all allocated memory ranges.
    //

    AllocationFlags =
        MEMORYMAP_TYPE_MAPPED_ALLOCATED |
        MEMORYMAP_TYPE_UNMAPPED_ALLOCATED;

    BlInitGenericBuffer(&DescriptorBuffer, NULL, 0);
    Status = BlMmGetMemoryMap(&AllocatedMemoryMap,
                              &DescriptorBuffer,
                              AllocationFlags,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Walk the allocated memory map in search of allocations made by the boot
    // application.  For each application allocation, free the the memory
    // range.
    //

    for (NextEntry = AllocatedMemoryMap.Flink;
         NextEntry != &AllocatedMemoryMap;
         NextEntry = NextEntry->Flink) {

        MemoryRange = CONTAINING_RECORD(NextEntry,
                                        MEMORY_DESCRIPTOR,
                                        ListEntry);

        UnpackedMemoryType.MemoryType = MemoryRange->MemoryType;
        if ((UnpackedMemoryType.Class != MEMORY_CLASS_APPLICATION) ||
            (UnpackedMemoryType.SubType <
             MEMORY_APPLICATION_TYPE_DEFINABLE_START)) {

            continue;
        }

        if (MemoryRange->MappedBasePage != 0) {
            VirtualAddress = PAGE_TO_ADDRESS(MemoryRange->MappedBasePage);
            Status = BlMmFreePages(VirtualAddress);

        } else {
            PhysicalAddress.QuadPart =
                PAGE_TO_PHYSICAL_ADDRESS(MemoryRange->BasePage);

            Status = BlMmFreePhysicalPages(PhysicalAddress);
        }

        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }
    }

    BlMmFreeHeap(DescriptorBuffer.Buffer);
    return ReturnStatus;
}

NTSTATUS
MmpFreeLibraryAllocations (
    VOID
    )

/*++

Routine Description:

    This routine will free memory allocated by the boot library.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    LIST_ENTRY AllocatedMemoryMap;
    ULONG AllocationFlags;
    ULONG AvoidLibraryFreeAttributes;
    GENERIC_BUFFER DescriptorBuffer;
    PMEMORY_DESCRIPTOR MemoryRange;
    PLIST_ENTRY NextEntry;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;
    UNPACKED_MEMORY_TYPE UnpackedMemoryType;
    PVOID VirtualAddress;

    ReturnStatus = STATUS_SUCCESS;

    //
    // During page allocator destruction the free page APIs will not zero
    // memory, therefore, since one of the goals of this routine is to make
    // sure that any outstanding library allocations are not left containing
    // sensitive data, insist that the page allocator is not in destruction.
    //

    ASSERT(MmPaModuleInitializationStatus() != ModuleInDestruction);

    //
    // Obtain a memory map of all allocated memory ranges.
    //

    AllocationFlags =
        MEMORYMAP_TYPE_MAPPED_ALLOCATED |
        MEMORYMAP_TYPE_UNMAPPED_ALLOCATED;

    BlInitGenericBuffer(&DescriptorBuffer, NULL, 0);
    Status = BlMmGetMemoryMap(&AllocatedMemoryMap,
                              &DescriptorBuffer,
                              AllocationFlags,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Walk the allocated memory map in search of allocations made by the boot
    // library.  For each library allocation, free the the memory range.
    //

    for (NextEntry = AllocatedMemoryMap.Flink;
         NextEntry != &AllocatedMemoryMap;
         NextEntry = NextEntry->Flink) {

        MemoryRange = CONTAINING_RECORD(NextEntry,
                                        MEMORY_DESCRIPTOR,
                                        ListEntry);

        //
        // Don't free library allocations that have the library only free
        // or free last attributes as this could result in persistent data
        // getting zero'd, or prevent a clean shutdown of the library.
        //

        AvoidLibraryFreeAttributes =
            MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_FREE_LAST |
            MEMORY_ATTRIBUTE_DESCRIPTION_LIBRARY_ONLY_FREE;

        UnpackedMemoryType.MemoryType = MemoryRange->MemoryType;
        if ((UnpackedMemoryType.Class != MEMORY_CLASS_LIBRARY) ||
            ((MemoryRange->Attributes & AvoidLibraryFreeAttributes) != 0)) {

            continue;
        }

        if (MemoryRange->MappedBasePage != 0) {
            VirtualAddress = PAGE_TO_ADDRESS(MemoryRange->MappedBasePage);
            Status = BlMmFreePages(VirtualAddress);

        } else {
            PhysicalAddress.QuadPart =
                PAGE_TO_PHYSICAL_ADDRESS(MemoryRange->BasePage);

            Status = BlMmFreePhysicalPages(PhysicalAddress);
        }

        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }
    }

    BlMmFreeHeap(DescriptorBuffer.Buffer);
    return ReturnStatus;
}

!include $(GRAPHICS_TST_ROOT)\DirectX\directx.mk

TARGETNAME=WGFTestFramework 
TARGETTYPE=LIBRARY


USE_MSVCRT=1

USE_NATIVE_EH=1

USE_STL=1
STL_VER=100


PRECOMPILED_INCLUDE=pch.h
PRECOMPILED_CXX=1


INCLUDES=\
	.\; \
	$(DIRECTX_TST_ROOT)\Common\Include; \
	$(DIRECTX_TST_ROOT)\Common\WGFTestFramework\Core\Include; \
	$(DIRECTX_TST_ROOT)\Common\WGFTestFramework\D3D; \
	$(WINDOWS_TST_ROOT)\Common\ExceptionTrace; \
	$(WINDOWS_TST_ROOT)\Common\SimpleLog; \
	$(WINDOWS_TST_ROOT)\Common\SimpleLog\DXGLog; \
	$(SDKTOOLS_INC_PATH)\wtt22; \
	$(INCLUDES) \


SOURCES=\
	commandline.cpp \
        error.cpp \
        flagcombinations.cpp \
	framework.cpp \
	fwtypes.cpp \
        logger.cpp \
	memmgr.cpp \
	shell.cpp \
	Test.cpp \
	testclassfactory.cpp \
	testgroupmanager.cpp \
	floatutils.cpp \


!if !($(ARM)) && !($(ARM64))
OBJLIBFILES = $(OBJLIBFILES) \
	$(SDKTOOLS_LIB_PATH)\wextest\common\sfi.lib \
	$(WINDOWS_LIB_PATH)\sfidx.lib
!endif
#include "CheckTabletNativeResolution.h"
#include <regex>
#include <shellapi.h>
#include <functional>

typedef std::basic_string<TCHAR> tstring;
typedef std::basic_regex<TCHAR> tregex;
typedef std::match_results<tstring::const_iterator> tsmatch;

CheckTabletNativeResolution::CheckTabletNativeResolution( LoggerWrapper* pLogger )
    : AbstractTest( pLogger )
{
    ccd::Log::SetLogMessagePFN( &LogMessage );
    ccd::Log::SetLogWarningPFN( &LogWarning );
    ccd::Log::SetLogErrorPFN( &LogError );
}

void CheckTabletNativeResolution::InitializeParameters()
{
	UINT32 width = 1366u;
	UINT32 height = 768u;

	// command line parameters to line up against test case parameters
	typedef std::function<bool( tstring const& )> Func;
	std::pair<TCHAR const*, Func> params[] = 
	{
		std::make_pair( _T("MinimumDisplayPixelWidth"), Func( [&]( tstring const& str ) -> bool { return FromString( &width, str ); } ) ),
		std::make_pair( _T("MinimumDisplayPixelHeight"), Func( [&]( tstring const& str ) -> bool { return FromString( &height, str ); } ) )
	};

	// create the test case parameters
    CTestCaseParameter<UINT32>* pWidthParam = AddParameter<UINT32>( params[0].first, &m_MinimumDisplayPixelWidth );
    CTestCaseParameter<UINT32>* pHeightParam = AddParameter<UINT32>( params[1].first, &m_MinimumDisplayPixelHeight );

	// parentheticals are used for capture groups.
    tregex param( _T("^/p:(\\w+)=(\\w+)$") );

	// scan the command line for parameters and apply any that we come across
    if( m_ppArgv )
    {
        tsmatch matches;
        for( int i = 1; i < m_Argc; ++i )
        {
            tstring arg( m_ppArgv[i] );
            if( std::regex_match( arg, matches, param ) )
            {
                for( size_t j = 0; j < (sizeof(params)/sizeof(*params)); ++j )
                {
                    if( 0 == _tcsicmp( params[j].first, matches[1].str().c_str() ) )
                    {
						(params[j].second)( matches[2].str() );
                        break;
                    }
                }
            }
        }
    }

    testfactor::RFactor rfWidth = AddParameterValue<UINT32>( pWidthParam, width );
    testfactor::RFactor rfHeight = AddParameterValue<UINT32>( pHeightParam, height );
    testfactor::RFactor testcase = rfWidth * rfHeight;
    SetRootTestFactor( testcase );
}

TestResult::Type CheckTabletNativeResolution::SetupTest()
{
    m_DisplayTopology.PushDisplaySettings( QDC_ONLY_ACTIVE_PATHS );
    return TestResult::Pass;
}

TestResult::Type CheckTabletNativeResolution::SetupTestCase()
{
    return TestResult::Pass;
}

TestResult::Type CheckTabletNativeResolution::ExecuteTestCase()
{
    // Switch to Internal display only
    if( DISP_CHANGE_SUCCESSFUL != m_DisplayTopology.SetDisplayConfig( 0, nullptr, 0, nullptr, DISPLAYCONFIG_TOPOLOGY_INTERNAL | SDC_APPLY ) )
    {
        return TestResult::Fail;
    }

    // get the native resolution of the internal panel
    DISPLAY_DEVICE dd;
    memset( &dd, 0, sizeof(dd) );
    dd.cb = sizeof(dd);

    DEVMODE dm;
    memset( &dm, 0, sizeof(dm) );
    dm.dmSize = sizeof(dm);
    bool found = false;

    for( DWORD i = 0; EnumDisplayDevices( nullptr, i, &dd, 0 ); ++i )
    {
        if( dd.StateFlags & DISPLAY_DEVICE_ACTIVE && dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE )
        {
            #define ENUM_NATIVE_SETTINGS -3
            if( !EnumDisplaySettingsEx( dd.DeviceName, ENUM_NATIVE_SETTINGS, &dm, 0 ) )
            {
                LOG_ERROR( E_FAIL, _T("Failed to get the native resolution for the internal display %s"), dd.DeviceName );
                return TestResult::Fail;
            }
            found = true;
            break;
        }
    }

    if( !found )
    {
        LOG_ERROR( E_FAIL, _T("Failed to find the internal active display.") );
        return TestResult::Fail;
    }

    // swap the dimensions if the device prefers portrait instead of landscape
    if( dm.dmPelsHeight > dm.dmPelsWidth )
    {
        LOG_MESSAGE( _T("Swapping dimensions for portrait device.") );
        UINT32 temp = m_MinimumDisplayPixelWidth;
        m_MinimumDisplayPixelWidth = m_MinimumDisplayPixelHeight;
        m_MinimumDisplayPixelHeight = temp;
    }

    TestResult::Type result = TestResult::Pass;
    // Verify that the dimensions of the device meet the minimum resolution requirement
    if( dm.dmPelsWidth < m_MinimumDisplayPixelWidth )
    {
        LOG_ERROR( E_FAIL, _T("Display native width of %d does not meet the minimum requirement of %d."), dm.dmPelsWidth, m_MinimumDisplayPixelWidth );
        result = TestResult::Fail;
    }

    if( dm.dmPelsHeight < m_MinimumDisplayPixelHeight )
    {
        LOG_ERROR( E_FAIL, _T("Display native height of %d does not meet the minimum requirement of %d."), dm.dmPelsHeight, m_MinimumDisplayPixelHeight );
        result = TestResult::Fail;
    }

    if( TestResult::Pass != result )
    {
        return result;
    }

    // Test the native resolution
    DEVMODE mode;
    memset( &mode, 0, sizeof(mode) );
    mode.dmSize = sizeof(mode);
    mode.dmPelsWidth = dm.dmPelsWidth;
    mode.dmPelsHeight = dm.dmPelsHeight;
    mode.dmFields |= DM_PELSWIDTH | DM_PELSHEIGHT;

    LONG ret = m_DisplayTopology.ChangeDisplaySettingsEx(
            dd.DeviceName,
            &mode,
            0,
            CDS_RESET,
            0 );

    if( ret != DISP_CHANGE_SUCCESSFUL )
    {
        // error message already provided
        return TestResult::Fail;
    }
    return TestResult::Pass;
}

void CheckTabletNativeResolution::CleanupTestCase()
{
}

void CheckTabletNativeResolution::CleanupTest()
{
    m_DisplayTopology.PopDisplaySettings( QDC_ONLY_ACTIVE_PATHS );
}

tstring CheckTabletNativeResolution::GetTestName()
{
    return _T("Verify primary/internal tablet display minimum resolution requirements.");
}
/////////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Microsoft Corporation
//
//	Module Name:
//	   HybridPresentETWValidation.h
//
//	Abstract:
//	This file contains declarations for ETW consumer classes used to validate Hybrid functionality
//
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once
// ETW-related headers
#include <etwConsumersRequire.h>
#include "DxgKernelEventHelper.h"

#include <lddmcoreeventdefs.h>
// This is a private version of the file that DxgKernel used to use when it logged Classic 
// events.

//-------------------------------------------------------------
// Definitions for D3D kernel mode DDI
//
typedef struct _MDL {
    struct _MDL *MdlNext;
    short MdlSize;
    short MdlFlags;
    struct _EPROCESS *Process;
    ULONG *lpMappedSystemVa;
    ULONG *lpStartVa;
    ULONG ByteCount;
    ULONG ByteOffset;
} MDL;
typedef MDL *PMDL;

typedef LONG NTSTATUS;

#ifndef PHYSICAL_ADDRESS
typedef LARGE_INTEGER PHYSICAL_ADDRESS;
#endif
//This is the DxgkEscape DRT Escape type signature
const UINT TEST_DRT_ESCAPE_SIGNATURE = 'XtrD';

#include "d3dkmddi.h"
#include "d3dkmthk.h"
#include "d3dkmthk_p.h"
#include "d3dukmdt.h"
#include "dmmdiag.h"
#include "dxetwevt.hxx"

#include "DxgKernelConsumer.h"
#include <TFrameworkEventHelper.h>
#include "TFrameworkConsumer.h"
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include "Controller.h"

#include "ETWProvider.h"

using namespace std;

#define HYBRID_POWER_MANAGEMENT_VALIDATE L"HybridPowerManagementValidate"
#define HYBRID_PRESENT_WAIT_FOR_EVENTS L"HybridPresentWaitForEvents"
#define HYBRID_PRESENT_WAIT_FOR_EVENTS_TIMEOUT 5000

#define DXGK_ALLOCATIONINFOFLAGS15 0x100000 //defined in d3dkmddi_pext.h

class CSLocker;

class CritSection
{
    friend class CSLocker;
public: 
    CritSection() :m_RefCount(0){InitializeCriticalSection(&m_CritSection);}
    ~CritSection() { DeleteCriticalSection(&m_CritSection); }
    void Lock();
    void Unlock();

private:
    CRITICAL_SECTION m_CritSection;
    UINT m_RefCount;

    CSLocker Enter();
};

class CSLocker
{
private:
    CritSection& m_Target;
    bool         m_bOwns;
        
public:
    CSLocker(CritSection& Target);
    CSLocker(CSLocker& RHS);
    ~CSLocker();
};



///////////////////////////////////////////////////////////////////////
// Tracking structures
///////////////////////////////////////////////////////////////////////

class CGPUPowerComponent;
class CAdapter;

typedef std::list<HANDLE> HandleList;

class CAllocation
{
public:
    CAllocation(HANDLE hDevice, 
                            UINT Flags, 
                            ULONGLONG Size,
                            ULONG Alignment,
                            UINT SupportedSegmentSet,
                            UINT PreferredSegments,
                            ULONG Priority,
                            HANDLE hVidMmGlobalHandle,
                            HANDLE hDxgAllocationHandle)
        :m_hDevice(hDevice),
         m_Size(Size),
         m_Alignment(Alignment),
         m_SupportedSegmentSet(SupportedSegmentSet),
         m_PreferredSegments(PreferredSegments),
         m_Priority(Priority),
         m_hVidMMGlobalHandle(hVidMmGlobalHandle),
         m_hDxgAllocationHandle(hDxgAllocationHandle),
         m_bDelete(false),
         m_bCommitted(false)
    {
        m_Flags.Value = Flags;
    }
    
    ~CAllocation(){};

    HANDLE m_hDevice;
    DXGKETW_ALLOCATIONFLAGS m_Flags;
    ULONGLONG m_Size;
    ULONG m_Alignment;
    UINT m_SupportedSegmentSet;
    UINT m_PreferredSegments;
    ULONG m_Priority;
    HANDLE m_hVidMMGlobalHandle;
    HANDLE m_hDxgAllocationHandle;
    bool m_bDelete;
    bool m_bCommitted;
};


typedef CAllocation CCrossAdapterAllocation;

class CMemorySegment
{
public:
    CMemorySegment(UINT ID, SIZE_T Size, UINT Flags)
        : m_ID(ID),
          m_Size(Size),
          m_BytesAllocated(0),
          m_pPC(NULL)
    {
        m_Flags.Value = Flags;
    }
    ~CMemorySegment(){};

    UINT m_ID;
    SIZE_T m_Size;
    DXGK_SEGMENTFLAGS m_Flags;
    ULONGLONG m_BytesAllocated;
    CGPUPowerComponent* m_pPC;
};

class CDevice
{
public:
    CDevice(HANDLE hDevice, HANDLE hProcess)
        :m_hDeviceHandle(hDevice),
         m_hProcess(hProcess)
    {}
    ~CDevice();
    HANDLE m_hDeviceHandle;
    HANDLE m_hProcess;
    HandleList m_hContextList;

    HANDLE FindContext(HANDLE hContext);
};

class CNode
{
public:
    CNode(UINT NodeOrdinal)
        : m_NodeOrdinal(NodeOrdinal),
          m_bActive(false),
          m_pPC(NULL)
    {}

    ~CNode();

    UINT m_NodeOrdinal;
    bool m_bActive;
    CGPUPowerComponent* m_pPC;
    HandleList m_hContextList;
};


class CDState
{
public:
    CDState()
        : m_PowerState(PowerDeviceUnspecified),
          m_TransitionToPowerState(PowerDeviceUnspecified),
          m_TransitionCountFromD3(0),
          m_MinActualLatencyFromD3(0),
          m_MaxActualLatencyFromD3(0),
          m_TotalActualLatencyFromD3(0),
          m_TransitionCountToD3(0),
          m_MinActualLatencyToD3(0),
          m_MaxActualLatencyToD3(0),
          m_TotalActualLatencyToD3(0),
          m_TransitionStartTS(0),
          m_TransitionStartDPCTotalTime(0),
          m_TransitionStartInterruptTotalTime(0)
    {
    }

    DEVICE_POWER_STATE m_PowerState;
    DEVICE_POWER_STATE m_TransitionToPowerState;

    UINT m_TransitionCountFromD3;
    ULONGLONG m_MinActualLatencyFromD3;
    ULONGLONG m_MaxActualLatencyFromD3;
    ULONGLONG m_TotalActualLatencyFromD3;

    UINT m_TransitionCountToD3;
    ULONGLONG m_MinActualLatencyToD3;
    ULONGLONG m_MaxActualLatencyToD3;
    ULONGLONG m_TotalActualLatencyToD3;

    ULONGLONG m_TransitionStartTS;
    ULONGLONG m_TransitionStartDPCTotalTime;
    ULONGLONG m_TransitionStartInterruptTotalTime;
};

class CFState
{
public:
    CFState(UINT FState, UINT ComponentIndex, ULONGLONG ReportedLatency)
        : m_FState(FState),
          m_ComponentIndex(ComponentIndex),
          m_ReportedLatency(ReportedLatency),
          m_TransitionCountFromF0(0),
          m_MinActualLatencyFromF0(0),
          m_MaxActualLatencyFromF0(0),
          m_TotalActualLatencyFromF0(0),
          m_TransitionCountToF0(0),
          m_MinActualLatencyToF0(0),
          m_MaxActualLatencyToF0(0),
          m_TotalActualLatencyToF0(0)
    {
    };

    UINT m_FState;
    UINT m_ComponentIndex;
    ULONGLONG m_ReportedLatency;

    ULONGLONG m_TransitionCountFromF0;
    ULONGLONG m_MinActualLatencyFromF0;
    ULONGLONG m_MaxActualLatencyFromF0;
    ULONGLONG m_TotalActualLatencyFromF0;

    ULONGLONG m_TransitionCountToF0;
    ULONGLONG m_MinActualLatencyToF0;
    ULONGLONG m_MaxActualLatencyToF0;
    ULONGLONG m_TotalActualLatencyToF0;
};

// Latency tolerances (all times in 100s ns)
// Engine
#define LT_ENGINE_MONITORON_INITIAL 80	* 10
#define LT_ENGINE_MONITORON_IDLE 15000	* 10	
#define LT_ENGINE_MONITORON_NOCONTEXT 35000	* 10
#define LT_ENGINE_MONITOROFF_INITIAL 2000 * 10
#define LT_ENGINE_MONITOROFF_IDLE 50000	* 10
#define LT_ENGINE_MONITOROFF_NOCONTEXT 100000 * 10
//Memory
#define LT_MEMORY_CONTEXT 15000	* 10
#define LT_MEMORY_NOCONTEXT 30000 * 10
//MemoryRefresh
#define LT_MEMORYREFRESH_MONITORON_INITIAL 80 * 10
#define LT_MEMORYREFRESH_MONITORON_IDLE 15000 * 10
#define LT_MEMORYREFRESH_MONITORON_NOCONTEXT 30000 * 10
#define LT_MEMORYREFRESH_MONITOROFF_INITIAL 80 * 10
#define LT_MEMORYREFRESH_MONITOROFF_IDLE 80000 * 10
#define LT_MEMORYREFRESH_MONITOROFF_NOCONTEXT 80000 * 10
//D3 transition
#define LT_D3TRANSITION_MONITORON_INITIAL 80 * 10
#define LT_D3TRANSITION_MONITORON_IDLE 80000 * 10
#define LT_D3TRANSITION_MONITORON_NOCONTEXT 250000 * 10
#define LT_D3TRANSITION_MONITOROFF_INITIAL 80 * 10
#define LT_D3TRANSITION_MONITOROFF_IDLE 250000 * 10
#define LT_D3TRANSITION_MONITOROFF_NOCONTEXT 250000 * 10

//Idle times that trigger latency tolerance changes (all times in 100s ns) * 10% error margine
// Engine
#define ENGINE_IDLE_TIME 200000 * 10 * 0.9f
#define ALL_ENGINES_IDLE_TIME_1 10000000 * 10 * 1.1f //10s
#define ALL_ENGINES_IDLE_TIME_2 20000000 * 10 * 1.1f //20s
// MemoryRefresh
#define MEMORYREFRESH_IDLE_TIME 2000000 * 10 * 0.9f


class CLatencyToleranceState
{
public:
    CLatencyToleranceState(ULONGLONG InitialLatencyTolerance)
        : m_ActiveTimeStamp(0),
          m_IdleTimeStamp(0),
          m_bMonitorOn(TRUE),
          m_ActiveContextsCount(0),
          m_LatencyTolerance(InitialLatencyTolerance),
          m_LatencyToleranceTimeStamp(0),
          m_ExpectedLatencyTolerance(0),
          m_CurrentTimeStamp(0)
    {}

    ULONGLONG m_ActiveTimeStamp;
    ULONGLONG m_IdleTimeStamp;
    BOOL m_bMonitorOn;
    UINT m_ActiveContextsCount;
    ULONGLONG m_LatencyTolerance;
    ULONGLONG m_LatencyToleranceTimeStamp;
    ULONGLONG m_ExpectedLatencyTolerance;
    ULONGLONG m_CurrentTimeStamp;
};

enum ePowerComponentState
{
    Idle = 0,
    Active = 1
};

typedef std::list<CFState*> FStateList;
typedef std::list<CLatencyToleranceState*> LatencyToleranceViolationList;

class CHybridETWMaster;

class CGPUPowerComponent
{
public:
    CGPUPowerComponent(CAdapter* pAdapter, UINT Type, UINT Index, UINT ComponentTypeId, UINT FStateCount);
    ~CGPUPowerComponent();

    UINT m_Type;
    UINT m_Index;
    UINT m_ComponentTypeId; //mapping
    void* m_pComponent;	// component that this power component is mapped to
    UINT m_CurrentFState;
    UINT m_FStateCount;
    ULONGLONG m_CurrentFStateTimeStamp;
    UINT m_RequestedFState;
    ULONGLONG m_RequestedFStateTimeStamp;
    ePowerComponentState m_State;
    ULONGLONG m_StateTimeStamp;
    ULONGLONG m_LatencyTolerance;
    FStateList m_FStateList;
    CLatencyToleranceState* m_pLatencyToleranceState;
    LatencyToleranceViolationList m_LatencyToleranceViolationList;
    CAdapter* m_pAdapter;
    DXGK_POWER_COMPONENT_FLAGS m_Flags;
    UINT m_ComponentTypeNameIndex;

    CFState* GetFState(UINT FState);
    void ValidateLatencyTolerance(ULONGLONG Timestamp, CHybridETWMaster* pETWMaster);
    void UpdateActiveStatus(ULONGLONG Timestamp);
};


typedef std::list<CCrossAdapterAllocation*> CrossAdapterAllocationList;
typedef std::list<CAllocation*> AllocationList;
typedef std::list<CMemorySegment*> MemorySegmentList;
typedef std::list<CDevice*> DeviceList;
typedef std::list<CGPUPowerComponent*> PowerComponentList;
typedef std::list<CNode*> NodeList;
typedef std::list<WCHAR*> StringList;

class CAdapter
{
public: 
    CAdapter(HANDLE hAdapter, bool bDiscrete)
        :m_hAdapter(hAdapter),
         m_bDiscrete(bDiscrete),
         m_bBDD(false),
         m_bAdapterState(FALSE),
         m_AdapterStateTimestamp(0),
         m_PagingNodeOrdinal(0xFFFFFFFF),
         m_numActiveContexts(0),
         m_hMiniportContext(NULL)
    {}
    ~CAdapter();
    bool IsDiscrete(){return m_bDiscrete;}
    bool IsBDD(){return m_bBDD;}
    bool IsApertureOnly(UINT SegmentSet);
    CDevice* FindTrackedDevice(HANDLE hDevice);
    HANDLE FindDevice(HANDLE hDevice);
    CNode* GetNode(UINT Node);
    CMemorySegment* GetMemorySegment(UINT SegmentID);
    CAllocation* FindAllocation(HANDLE hGlobalAlloc);
    CGPUPowerComponent* GetPowerComponent(UINT Index);
    bool AddProcessName(WCHAR* strName);

    HANDLE m_hAdapter;
    HANDLE m_hMiniportContext;
    DeviceList m_DeviceList; // tracked devices
    HandleList m_AllDevices; // all devices created on this adapter
    NodeList m_NodeList;
    int m_PagingNodeOrdinal;
    UINT m_numActiveContexts;
    CDState m_DState;	// Current D-State of the GPU


    // Cross-Adapter allocations
    CrossAdapterAllocationList m_CrossAdapterAllocationList;

    // All allocations - these are used to track segment content
    AllocationList m_AllocationList;
    PowerComponentList m_PowerComponentList;
    BOOL m_bAdapterState;	// TRUE if active, FALSE if idle (D3 state)
    ULONGLONG m_AdapterStateTimestamp;	// Timestamp of the state change
    StringList m_ProcessNameList;	// names of processes that ran on this adapter during test execution

    // Segments
    MemorySegmentList m_MemorySegmentList;
    bool m_bDiscrete;
    bool m_bBDD;
};


////////////////////////////////////////////////////////////////////////////////
// ETW Master
// ETW Master manages multiple ETW consumers as well as Test Provider. 
// It processes data from the events that consumers receive.
////////////////////////////////////////////////////////////////////////////////
class CDxgKrnlHybridConsumer;
class CTestHybridConsumer;
class CHybridETWProvider;

typedef std::list<CAdapter*> AdapterList;

class CHybridETWMaster
{
public:
    CHybridETWMaster();
    ~CHybridETWMaster();
    UINT GetCrossAdapterPresentsCount() {return m_CrossAdapterPresentsCount;}
    UINT GetRegularPresentsCount() {return m_RegularPresentsCount;}
    void ResetPresentCounters();
    HRESULT TrackProcess(HANDLE hProcess);
    void StartListening();
    void StopListening();
    bool WaitForEvents();
    bool WaitForEvents(UINT Timeout);
    bool PowerManagementValidate();
    HANDLE* GetProcessIDByName(CHAR* szName);
    WCHAR* GetProcessNameByHandle(HANDLE hProcess);
    void CleanupAllocations();
    CAdapter* GetDGpu();
    void Cleanup();

    UINT m_CrossAdapterPresentsCount;
    UINT m_RegularPresentsCount;

    AdapterList m_Adapters;
    D3DKMT_PRESENT_MODEL m_PresentModel;
    bool m_bConsumersAdded;
    ULONGLONG m_InterruptStartTS;
    ULONGLONG m_InterruptTimeTotal;
    ULONGLONG m_DPCStartTS;
    ULONGLONG m_DPCTimeTotal;

    CritSection m_CritSection;

    // event handlers
    void HandleAdapterAllocation(PEVENT_RECORD pEvent);
    void HandleReportSegment(PEVENT_RECORD pEvent);
    void HandleAdapter(PEVENT_RECORD pEvent);
    void HandleDevice(PEVENT_RECORD pEvent);
    void HandleContext(PEVENT_RECORD pEvent);
    void HandlePresent(PEVENT_RECORD pEvent);
    void HandlePresentHistory(PEVENT_RECORD pEvent);
    void HandlePresentHistoryDetailed(PEVENT_RECORD pEvent);
    void HandleRuntimePowerManagement(PEVENT_RECORD pEvent);
    void HandlePagingOpFill(PEVENT_RECORD pEvent);
    void HandlePagingOpDiscard(PEVENT_RECORD pEvent);
    void HandlePagingOpTransfer(PEVENT_RECORD pEvent);
    void HandleReportCommittedGlobalAllocation(PEVENT_RECORD pEvent);
    void HandleDdiSetPowerState(PEVENT_RECORD pEvent);
    void HandleNodeMetadata(PEVENT_RECORD pEvent);
    void HandleTotalBytesResidentInSegment(PEVENT_RECORD pEvent);
    void HandleProfiler(PEVENT_RECORD pEvent);

    void HandleGraphicsTestMessage(PEVENT_RECORD pEvent);

    void PrintAdapters();
    void ProcessPowerEvents(bool bProcess) {m_bProcessPowerEvents = bProcess;}

    void LogTestETW(WCHAR* str);

private:
    CDxgKrnlHybridConsumer* m_pDxgKrnlListener;
    CTestHybridConsumer* m_pTestListener;
    CHybridETWProvider* m_pTestProvider;
    bool m_bListening;
    HANDLE m_hListeningThread;
    HANDLE m_hWaitForEvents;
    bool m_bProcessPowerEvents;
    CController m_etwController;
    static DWORD EtwListeningThreadProc(void* context);

    CAdapter* GetAdapter(HANDLE hAdapter);

    // structures to only track events that belong to our test
    HandleList           m_TrackedProcessList;

    bool IsTrackedDevice(HANDLE hDevice);
    bool IsTrackedProcess(HANDLE hProcess);
    bool IsTrackedContext(HANDLE hContext);
};

struct ETWListeningThreadProcArgs
{
    CHybridETWMaster* pMaster;
    CDxgKrnlHybridConsumer* pDxgKrnlListener;
    CTestHybridConsumer* pTestListener;
};

///////////////////////////////////////////////////////////////////////////////
//	ETW Consumers
//	Consumers receive events and re-direct them to ETW Master for processing
///////////////////////////////////////////////////////////////////////////////
class CDxgKrnlHybridConsumer : public CDxgKernelConsumer
{
public:
    CDxgKrnlHybridConsumer(CHybridETWMaster* pMaster):m_pETWMaster(pMaster){};

private:
    CHybridETWMaster* m_pETWMaster;

    void AdapterAllocation_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleAdapterAllocation(pEvent);}
    void ReportSegment_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleReportSegment(pEvent);}
    void Adapter_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleAdapter(pEvent);} 
    void Device_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleDevice(pEvent);}
    void Present_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePresent(pEvent);}
    void Context_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleContext(pEvent);}
    void PresentHistory_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePresentHistory(pEvent);}
    void PresentHistoryDetailed_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePresentHistoryDetailed(pEvent);}
    void RuntimePowerManagement_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleRuntimePowerManagement(pEvent);}
    void PagingOpFill_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePagingOpFill(pEvent);}
    void PagingOpDiscard_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePagingOpDiscard(pEvent);}
    void PagingOpTrgansfer_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePagingOpTransfer(pEvent);}
    void PagingOpSpecialLockTransfer_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePagingOpTransfer(pEvent);}
    void ReportCommittedGlobalAllocation_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleReportCommittedGlobalAllocation(pEvent);}
    void DdiSetPowerState_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleDdiSetPowerState(pEvent);}
    void NodeMetadata_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleNodeMetadata(pEvent);}
    void TotalBytesResidentInSegment_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleTotalBytesResidentInSegment(pEvent);}
    void Profiler_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleProfiler(pEvent);}
};


class CTestHybridConsumer : public CTFrameworkConsumer
{
public:
    CTestHybridConsumer(CHybridETWMaster* pMaster):m_pETWMaster(pMaster){};

private:
    CHybridETWMaster* m_pETWMaster;
    void GraphicsTestMessage_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleGraphicsTestMessage(pEvent);}
};

//////////////////////////////////////////////////////////////////////////////
// Test ETW provider
//////////////////////////////////////////////////////////////////////////////
class CHybridETWProvider : public CEtwProvider
{
public:
    ULONG WriteToETW(__in const LPWSTR pwszMessage, ...);
};

///////////////////////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////////////////////
inline WCHAR* GetArgs(const WCHAR *pwszFmt, va_list argList)
{
    const int   nAllocSize = 128;
    UINT        nAlloc     = nAllocSize;
    bool        bSuccess   = false;
    WCHAR*      pwszOut = NULL;

    // preprocess the string with the values from the variable argument list.
    while(!bSuccess)
    {
        pwszOut = new WCHAR[nAlloc];

        if(!pwszOut)
            return L"Error formatting string. Out of memory.";

        int nChars = _vsnwprintf(pwszOut, nAlloc, pwszFmt, argList);

        if((nChars < 0) || (nChars >= (int)nAlloc-1)) // we need to alloc more memory
        {
            delete[] pwszOut;
            nAlloc += nAllocSize;
        }
        else
        {
            //add \0 to the string
            pwszOut[nChars] = '\0';
            wcscat(pwszOut, L"\n");
            bSuccess = true;
        }
    }
    
    return pwszOut;
}

inline void DBGPrint( __in const LPWSTR pwszMessage, ...)
{
    va_list    argList;
    va_start(argList, pwszMessage);
    WCHAR *pwszOut = GetArgs(pwszMessage, argList);
    va_end(argList);

    OutputDebugStringW(pwszOut);

    if( pwszOut )
    {
        delete[] pwszOut;
    }
}

﻿///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using Microsoft.Test.Networking;
using Microsoft.Test.Networking.Wireless;
using Microsoft.Test.Networking.RemoteControl;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using System.Threading.Tasks;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.Devices.Enumeration;
using Windows.Devices.WiFiDirect.Services;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    public class WFDSvcWrapperHandle
    {
        private static int next = 1;

        public WFDSvcWrapperHandle()
        {
            this.Value = next++;
        }

        public int Value
        {
            get;
            // set needs to be public for serialization to work
            set;
        }

        public override string ToString()
        {
            return Value.ToString(CultureInfo.InvariantCulture);
        }

        public override int GetHashCode()
        {
            // Provide a hashcode that minimizes collisions and spreads out over many buckets
            // This will allow efficient Dictionary lookups
            // Multiply value (which just increments linearly) by a large prime
            unchecked
            {
                return Value * 486187739;
            }
        }

        /// <summary>
        /// When using WFDSvcWrapperHandle in dictionary, need custom comparer
        /// </summary>
        public class EqualityComparer : IEqualityComparer<WFDSvcWrapperHandle>
        {
            public bool Equals(WFDSvcWrapperHandle lhs, WFDSvcWrapperHandle rhs)
            {
                if (lhs == null && rhs == null)
                {
                    return true;
                }
                else if (lhs == null || rhs == null)
                {
                    return false;
                }
                return lhs.Value == rhs.Value;
            }

            public int GetHashCode(WFDSvcWrapperHandle h)
            {
                return (h == null) ? 0 : h.GetHashCode();
            }
        }
    }

    /// <summary>
    /// Result of asynchronous operation
    /// 
    /// Includes any error information from execution
    /// </summary>
    internal class AsyncResult
    {
        internal AsyncResult(
            bool success,
            Exception error = null
            )
        {
            Success = success;
            Error = error;
        }

        public bool Success { get; private set; }
        public Exception Error { get; private set; }
    }

    /// <summary>
    /// Result of asynchronous operation that returns a handle
    /// </summary>
    internal class AsyncHandleResult : AsyncResult
    {
        internal AsyncHandleResult(
            WFDSvcWrapperHandle handle,
            bool success,
            Exception error = null
            ) : base(success, error)
        {
            Handle = handle;
        }

        public WFDSvcWrapperHandle Handle { get; private set; }
    }

    /// <summary>
    /// Result of asynchronous operation that returns a list of handles
    /// </summary>
    internal class AsyncHandleListResult : AsyncResult
    {
        internal AsyncHandleListResult(
            List<WFDSvcWrapperHandle> handles,
            bool success,
            Exception error = null
            )
            : base(success, error)
        {
            Handles = handles;
        }

        public List<WFDSvcWrapperHandle> Handles { get; private set; }
    }

    /// <summary>
    /// Result of asynchronous operation that returns a WiFiDirectServiceProvisioningInfo
    /// </summary>
    internal class AsyncProvisioningInfoResult : AsyncResult
    {
        internal AsyncProvisioningInfoResult(
            ProvisioningInfoWrapper info,
            bool success,
            Exception error = null
            )
            : base(success, error)
        {
            ProvisioningInfo = info;
        }

        public ProvisioningInfoWrapper ProvisioningInfo { get; private set; }
    }

    internal class AsyncStringResult : AsyncResult
    {
        internal AsyncStringResult(
            string str,
            bool success,
            Exception error = null
            ) : base(success, error)
        {
            this.String = str;
        }

        public string String { get; private set; }
    }

    /// <summary>
    /// Data from Windows.Devices.WiFiDirect.Services.WiFiDirectServiceAdvertiser
    /// which can easily be serialized to pass over remote control
    /// </summary>
    public class ServiceAdvertiserInfo
    {
        public ServiceAdvertiserInfo()
        {
            PreferredConfigMethods = new List<WiFiDirectServiceConfigurationMethod>();
        }

        /// <summary>
        /// Build the info from the actual object
        /// </summary>
        /// <param name="advertiser"></param>
        internal ServiceAdvertiserInfo(WiFiDirectServiceAdvertiser advertiser)
        {
            PreferGO = advertiser.PreferGroupOwnerMode;
            PreferredConfigMethods = new List<WiFiDirectServiceConfigurationMethod>(advertiser.PreferredConfigurationMethods);
            ServiceName = advertiser.ServiceName;
            ServiceStatus = advertiser.ServiceStatus;

            if (advertiser.ServiceInfo != null && advertiser.ServiceInfo.Length > 0)
            {
                using (DataReader serviceInfoDataReader = Windows.Storage.Streams.DataReader.FromBuffer(advertiser.ServiceInfo))
                {
                    ServiceInfo = serviceInfoDataReader.ReadString(advertiser.ServiceInfo.Length);
                }
            }
            else
            {
                ServiceInfo = "";
            }

            if (advertiser.DeferredSessionInfo != null && advertiser.DeferredSessionInfo.Length > 0)
            {
                using (DataReader sessionInfoDataReader = Windows.Storage.Streams.DataReader.FromBuffer(advertiser.DeferredSessionInfo))
                {
                    DeferredSessionInfo = sessionInfoDataReader.ReadString(advertiser.DeferredSessionInfo.Length);
                }
            }
            else
            {
                DeferredSessionInfo = "";
            }
        }

        public bool PreferGO { get; set; }
        public List<WiFiDirectServiceConfigurationMethod> PreferredConfigMethods { get; set; }
        public string ServiceName { get; set; }
        public WiFiDirectServiceStatus ServiceStatus { get; set; }
        // Note: this is set by the controller, don't have the MAC in the WiFiDirectServiceAdvertiser
        public DOT11_MAC_ADDRESS ServiceAddress { get; set; }
        public string ServiceInfo { get; set; }
        public string DeferredSessionInfo { get; set; }
    }

    /// <summary>
    /// Wraps Windows.Devices.WiFiDirect.Services.WiFiDirectServiceAdvertiser
    /// </summary>
    internal class ServiceAdvertiser : IDisposable
    {
        private WiFiDirectServiceAdvertiser advertiser;
        private WFDSvcWrapperHandle handle = new WFDSvcWrapperHandle();

        private IWiFiDirectServicesSessionManager sessionManager;
        private IWiFiDirectServicesErrorHandler errorHandler;
        private IWiFIDirectServicesPinManager pinManager;
        
        private bool declineNextRequest = false;
        private bool expectSessionRequestFail = false;

        private CancellationTokenSource connectCts;

        private AutoResetEvent serviceStartedEvent = new AutoResetEvent(false);
        private AutoResetEvent autoAcceptConnectedEvent = new AutoResetEvent(false);
        private AutoResetEvent sessionRequestedEvent = new AutoResetEvent(false);

        // Keep track of sessions connected via events so we can directly access them
        private WFDSvcWrapperHandle lastConnectedSession = null;
        private bool declinedLastRequest = false;
        private bool failedLastRequest = false;

        internal ServiceAdvertiser(
            WiFiDirectServiceAdvertiser advertiser,
            IWiFiDirectServicesSessionManager sessionManager,
            IWiFiDirectServicesErrorHandler errorHandler,
            IWiFIDirectServicesPinManager pinManager
            )
        {
            this.advertiser = advertiser;
            this.sessionManager = sessionManager;
            this.errorHandler = errorHandler;
            this.pinManager = pinManager;
            
            this.advertiser.AdvertisementStatusChanged += OnAdvertisementStatusChanged;
            this.advertiser.AutoAcceptSessionConnected += OnAutoAcceptSessionConnected;
            this.advertiser.SessionRequested += OnSessionRequested;

            WiFiDirectTestLogger.Log("Created ServiceAdvertiser wrapper (Advertiser={0})", handle);
        }

        #region Properties
        public WFDSvcWrapperHandle Handle
        {
            get { return handle; }
        }

        public WiFiDirectServiceAdvertiser Advertiser
        {
            get { return advertiser; }
        }

        public WFDSvcWrapperHandle LastConnectedSession
        {
            get { return lastConnectedSession; }
        }

        public bool DeclineNextSessionRequest
        {
            set { declineNextRequest = value; }
        }

        public bool ExpectSessionRequestFailure
        {
            set { expectSessionRequestFail = value; }
        }

        public bool DidDeclineLastSessionRequest()
        {
            return declinedLastRequest;
        }

        public bool DidFailLastSessionRequest()
        {
            return failedLastRequest;
        }
        #endregion

        #region Events
        private async void OnSessionRequested(WiFiDirectServiceAdvertiser advertiser, WiFiDirectServiceSessionRequestedEventArgs args)
        {
            try
            {
                ThrowIfDisposed();
                sessionRequestedEvent.Reset();

                WiFiDirectTestLogger.Log("Session Requested (Advertiser={0})", handle);

                WiFiDirectServiceSession session = null;

                string sessionInfo = "";
                if (args.GetSessionRequest().SessionInfo != null && args.GetSessionRequest().SessionInfo.Length > 0)
                {
                    using (DataReader sessionInfoDataReader = Windows.Storage.Streams.DataReader.FromBuffer(args.GetSessionRequest().SessionInfo))
                    {
                        sessionInfo = sessionInfoDataReader.ReadString(args.GetSessionRequest().SessionInfo.Length);
                        WiFiDirectTestLogger.Log("Received Session Info: {1} (Discovery={0})", handle, WiFiDirectTestUtilities.GetTruncatedString(sessionInfo));
                    }
                }

                if (declineNextRequest)
                {
                    declineNextRequest = false;
                    WiFiDirectTestLogger.Log("Declining incoming request (Advertiser={0})", handle);

                    lastConnectedSession = null;
                    declinedLastRequest = true;
                    args.GetSessionRequest().Dispose();
                }
                else
                {
                    WiFiDirectTestLogger.Log("Connecting to service: ConnectAsync... (Advertiser={0})", handle);

                    // Need to allow cancellation in case seeker fails and test is trying to continue to next test case
                    connectCts = new CancellationTokenSource();

                    if (args.GetSessionRequest().ProvisioningInfo.IsGroupFormationNeeded &&
                        args.GetSessionRequest().ProvisioningInfo.SelectedConfigurationMethod != WiFiDirectServiceConfigurationMethod.Default)
                    {
                        if (args.GetSessionRequest().ProvisioningInfo.SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod.PinDisplay)
                        {
                            // Generate a PIN, set event
                            this.pinManager.GeneratePin();
                        }
                        else
                        {
                            // Wait on PIN entry event, get displayed PIN
                            this.pinManager.WaitForPinEntry();
                        }

                        string pin = this.pinManager.RetrievePin();

                        WiFiDirectTestLogger.Log(
                            "Provisioning is required, config method={1}, using pin={2} (Advertiser={0})",
                            handle,
                            args.GetSessionRequest().ProvisioningInfo.SelectedConfigurationMethod,
                            pin
                            );

                        session = await advertiser.ConnectAsync(args.GetSessionRequest().DeviceInformation, pin).AsTask(connectCts.Token);
                    }
                    else
                    {
                        session = await advertiser.ConnectAsync(args.GetSessionRequest().DeviceInformation).AsTask(connectCts.Token);
                    }
                    WiFiDirectTestLogger.Log("Connecting to service: ConnectAsync DONE (Advertiser={0})", handle);

                    connectCts = null;

                    // Handle case where object is disposed during connect
                    ThrowIfDisposed();

                    ServiceSession sessionWrapper = new ServiceSession(session, sessionManager, errorHandler, deferredSessionInfo: sessionInfo);
                    sessionManager.AddSession(sessionWrapper);

                    WiFiDirectTestLogger.Log("Added session to list (Advertiser={0}, Session={1})", handle, sessionWrapper.Handle);

                    lastConnectedSession = sessionWrapper.Handle;
                    declinedLastRequest = false;
                }
            }
            catch (TaskCanceledException)
            {
                WiFiDirectTestLogger.Log("Connect was canceled by user, failure should have already happened so swallowing this error (Advertiser={0})", handle);
            }
            catch (Exception ex)
            {
                failedLastRequest = true;
                if (!expectSessionRequestFail)
                {
                    WiFiDirectTestLogger.Error("Unexpected exception in OnSessionRequested: {1} (Advertiser={0})", handle, ex.Message);
                    errorHandler.SetError(ex);
                }
                else
                {
                    WiFiDirectTestLogger.Log("Got expected session request failure (Advertiser={0})", handle);
                }
            }
            finally
            {
                if (!disposed)
                {
                    sessionRequestedEvent.Set();
                }
            }
        }

        private void OnAutoAcceptSessionConnected(WiFiDirectServiceAdvertiser sender, WiFiDirectServiceAutoAcceptSessionConnectedEventArgs args)
        {
            try
            {
                ThrowIfDisposed();

                WiFiDirectTestLogger.Log("Auto-Accept Session Connected (Advertiser={0})", handle);

                string sessionInfo = "";
                if (args.SessionInfo != null && args.SessionInfo.Length > 0)
                {
                    using (DataReader sessionInfoDataReader = Windows.Storage.Streams.DataReader.FromBuffer(args.SessionInfo))
                    {
                        sessionInfo = sessionInfoDataReader.ReadString(args.SessionInfo.Length);
                        WiFiDirectTestLogger.Log("Received Session Info: {1} (Discovery={0})", handle, WiFiDirectTestUtilities.GetTruncatedString(sessionInfo));
                    }
                }

                ServiceSession sessionWrapper = new ServiceSession(args.Session, sessionManager, errorHandler, deferredSessionInfo: sessionInfo);
                sessionManager.AddSession(sessionWrapper);

                WiFiDirectTestLogger.Log("Added session to list (Advertiser={0}, Session={1})", handle, sessionWrapper.Handle);

                lastConnectedSession = sessionWrapper.Handle;
                autoAcceptConnectedEvent.Set();
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("Unexpected exception in OnAutoAcceptSessionConnected: {1} (Advertiser={0})", handle, ex.Message);
                errorHandler.SetError(ex);
            }
        }

        private void OnAdvertisementStatusChanged(WiFiDirectServiceAdvertiser sender, object o)
        {
            try
            {
                ThrowIfDisposed();

                WiFiDirectServiceAdvertisementStatus status = advertiser.AdvertisementStatus;

                WiFiDirectTestLogger.Log(
                    "Advertisement Status Changed to {1} (Advertiser={0})",
                    handle,
                    status.ToString()
                    );

                if (status == WiFiDirectServiceAdvertisementStatus.Started)
                {
                    serviceStartedEvent.Set();
                }
                else if (status == WiFiDirectServiceAdvertisementStatus.Aborted)
                {
                    WiFiDirectTestLogger.Error(
                        "Advertisement aborted, error: {1} (Advertiser={0})",
                        handle,
                        advertiser.ServiceError
                        );
                    errorHandler.SetError(new Exception("Advertiser aborted unexpectedly"));
                }

            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Log("Unexpected exception in OnAdvertisementStatusChanged: {1} (Advertiser={0})", handle, ex.Message);
            }
        }
        #endregion

        #region Synchronization
        public void WaitForServiceStarted(int msTimeout = 1000)
        {
            ThrowIfDisposed();
            if (!serviceStartedEvent.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for service to start");
            }
        }

        public void WaitForAutoAcceptSessionConnected(int msTimeout = 10000)
        {
            ThrowIfDisposed();
            if (!autoAcceptConnectedEvent.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for auto accept session to connect");
            }
        }

        public void WaitForSessionRequested(int msTimeout = 10000)
        {
            ThrowIfDisposed();
            if (!sessionRequestedEvent.WaitOne(msTimeout))
            {
                throw new Exception("Timed out waiting for session request");
            }
        }
        #endregion

        #region Dispose
        bool disposed = false;

        ~ServiceAdvertiser()
        {
            Dispose(false);
        }
        
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        
        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }
            
            if (disposing)
            {
                // Stop advertising
                // If for some reason the advertiser is stopping between the time that we check
                // the AdvertisementStatus and when we call Stop, this will throw
                try
                {
                    if (advertiser.AdvertisementStatus == WiFiDirectServiceAdvertisementStatus.Started)
                    {
                        advertiser.Stop();
                    }
                }
                catch (Exception)
                {
                    // Stop can throw if it is already stopped or stopping, ignore
                }

                // Remove event handlers
                advertiser.AdvertisementStatusChanged -= OnAdvertisementStatusChanged;
                advertiser.AutoAcceptSessionConnected -= OnAutoAcceptSessionConnected;
                advertiser.SessionRequested -= OnSessionRequested;
                
                // Cancel ongoing connect, if it is not complete
                if (connectCts != null)
                {
                    WiFiDirectTestLogger.Log("Attempt to cancel connection attempt (Advertiser={0})", handle);
                    connectCts.Cancel();
                }

                serviceStartedEvent.Dispose();
                autoAcceptConnectedEvent.Dispose();
                sessionRequestedEvent.Dispose();
            }
            
            disposed = true;
        }

        private void ThrowIfDisposed()
        {
            if (disposed)
            {
                throw new ObjectDisposedException("ServiceAdvertiser");
            }
        }
        #endregion
    }

    class WiFiDirectServicesWatcher : IDisposable
    {
        private DeviceWatcher watcher = null;

        private IWiFiDirectServicesSessionManager sessionManager;
        private IWiFiDirectServicesDiscoveryManager discoveryManager;
        private IWiFiDirectServicesErrorHandler errorHandler;
        private IWiFIDirectServicesPinManager pinManager;

        private IList<ServiceAdvertiserInfo> expectations = new List<ServiceAdvertiserInfo>();
        private IDictionary<string, DiscoveredService> discovered = new Dictionary<string, DiscoveredService>();

        private AutoResetEvent stoppedEvent = new AutoResetEvent(false);
        private AutoResetEvent enumCompletedEvent = new AutoResetEvent(false);
        private AutoResetEvent foundAllEvent = new AutoResetEvent(false);

        public WiFiDirectServicesWatcher(
            IWiFiDirectServicesSessionManager sessionManager,
            IWiFiDirectServicesDiscoveryManager discoveryManager,
            IWiFiDirectServicesErrorHandler errorHandler,
            IWiFIDirectServicesPinManager pinManager
            )
        {
            this.sessionManager = sessionManager;
            this.discoveryManager = discoveryManager;
            this.errorHandler = errorHandler;
            this.pinManager = pinManager;
        }

        public void AddExpectedService(ServiceAdvertiserInfo service)
        {
            expectations.Add(service);
        }

        public bool HasExpectations()
        {
            return expectations.Count > 0;
        }

        public void Start(string serviceName, string serviceInfo = "")
        {
            ThrowIfDisposed();

            ResetWatcher();
            foundAllEvent.Reset();
            enumCompletedEvent.Reset();
            stoppedEvent.Reset();

            discoveryManager.ClearDiscoveries();

            WiFiDirectTestLogger.Log("Start Services Watcher... (ServiceName={0}, ServiceInfo={1})", serviceName, serviceInfo);

            string serviceSelector = "";
            if (serviceInfo == "")
            {
                serviceSelector = WiFiDirectService.GetSelector(serviceName);
            }
            else
            {
                using (var serviceInfoDataWriter = new Windows.Storage.Streams.DataWriter(new Windows.Storage.Streams.InMemoryRandomAccessStream()))
                {
                    serviceInfoDataWriter.WriteString(serviceInfo);
                    serviceSelector = WiFiDirectService.GetSelector(serviceName, serviceInfoDataWriter.DetachBuffer());
                }
            }

            WiFiDirectTestLogger.Log("DeviceInformation.CreateWatcher... Using Selector: {0}", serviceSelector);

            List<string> additionalProperties = new List<string>();
            additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceAddress");
            additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceName");
            additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceInformation");
            additionalProperties.Add("System.Devices.WiFiDirectServices.AdvertisementId");
            additionalProperties.Add("System.Devices.WiFiDirectServices.ServiceConfigMethods");

            watcher = DeviceInformation.CreateWatcher(serviceSelector, additionalProperties);

            watcher.Added += OnAdded;
            watcher.EnumerationCompleted += OnEnumerationComplete;
            watcher.Removed += OnRemoved;
            watcher.Stopped += OnStopped;
            watcher.Updated += OnUpdated;

            watcher.Start();
        }

        public void Stop()
        {
            if (watcher != null)
            {
                watcher.Stop();
            }
        }

        #region Synchronization
        public void WaitForFindAll(int msTimeout = 15000)
        {
            ThrowIfDisposed();
            WaitHandle[] handles = new WaitHandle[] { foundAllEvent, stoppedEvent };
            int waitResult = WaitHandle.WaitAny(handles, msTimeout);
            if (waitResult == WaitHandle.WaitTimeout)
            {
                throw new Exception("Timed out waiting for watcher to find service");
            }
            else if (waitResult == 1)
            {
                throw new Exception("Watcher stopped before finding all services");
            }
        }
        #endregion

        private void CheckExpectation(DiscoveredServiceInfo discoveredServiceInfo)
        {
            if (expectations.Count > 0)
            {
                for (int i = 0; i < expectations.Count; ++i)
                {
                    if (expectations[i].ServiceName == discoveredServiceInfo.ServiceName &&
                        expectations[i].ServiceAddress == discoveredServiceInfo.ServiceAddress)
                    {
                        if (expectations[i].ServiceInfo != discoveredServiceInfo.ServiceInfo)
                        {
                            WiFiDirectTestLogger.Log(
                                "Expecting Service Info:\n\t{0}\nReceived:\n\t{1}",
                                WiFiDirectTestUtilities.GetTruncatedString(expectations[i].ServiceInfo, 32),
                                WiFiDirectTestUtilities.GetTruncatedString(discoveredServiceInfo.ServiceInfo, 32)
                                );
                            // Allow multiple services with same name/different service info
                            // Skip if service info match fails, will fail if no service info found
                            continue;
                        }

                        WiFiDirectTestLogger.Log("Found Expected Service!");
                        expectations.RemoveAt(i);
                    }
                }

                if (expectations.Count == 0)
                {
                    WiFiDirectTestLogger.Log("Found All Expected Services!");
                    foundAllEvent.Set();
                }
            }
        }

        #region events
        private void OnAdded(DeviceWatcher w, DeviceInformation devInfo)
        {
            try
            {
                WiFiDirectTestLogger.Log("Service Added: {0}", devInfo.Id);

                DiscoveredService discoveredService = new DiscoveredService(devInfo, sessionManager, errorHandler, pinManager);
                discovered.Add(devInfo.Id, discoveredService);
                discoveryManager.AddDiscovery(discoveredService);

                if (expectations.Count > 0)
                {
                    DiscoveredServiceInfo discoveredServiceInfo = new DiscoveredServiceInfo(devInfo);
                    CheckExpectation(discoveredServiceInfo);
                }
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("Unexpected exception in OnAdded: {0}", ex.Message);
                errorHandler.SetError(ex);
            }
        }

        private void OnUpdated(DeviceWatcher w, DeviceInformationUpdate update)
        {
            try
            {
                WiFiDirectTestLogger.Log("Service Updated: {0}", update.Id);

                // Find the existing discovery and update it
                if (discovered.ContainsKey(update.Id))
                {
                    (discovered[update.Id]).DeviceInfo.Update(update);

                    if (expectations.Count > 0)
                    {
                        DiscoveredServiceInfo discoveredServiceInfo = new DiscoveredServiceInfo((discovered[update.Id]).DeviceInfo);
                        CheckExpectation(discoveredServiceInfo);
                    }
                }
                else
                {
                    WiFiDirectTestLogger.Log("Discovered service not found in our map!!!");
                }
            }
            catch (Exception ex)
            {
                WiFiDirectTestLogger.Error("Unexpected exception in OnUpdated: {0}", ex.Message);
                errorHandler.SetError(ex);
            }
        }

        private void OnRemoved(DeviceWatcher w, DeviceInformationUpdate update)
        {
            WiFiDirectTestLogger.Log("Service Removed: {0}", update.Id);

            DiscoveredService discoveredService = discovered[update.Id];
            discoveryManager.RemoveDiscovery(discoveredService.Handle);
            discovered.Remove(update.Id);
        }

        private void OnEnumerationComplete(DeviceWatcher w, Object o)
        {
            WiFiDirectTestLogger.Log("Enumeration Complete!");
            enumCompletedEvent.Set();
        }

        private void OnStopped(DeviceWatcher w, Object o)
        {
            WiFiDirectTestLogger.Log("Watcher Stopped!");
            stoppedEvent.Set();
        }
        #endregion

        #region Dispose
        bool disposed = false;

        public void ResetWatcher()
        {
            if (watcher != null)
            {
                if (watcher.Status == DeviceWatcherStatus.Started || watcher.Status == DeviceWatcherStatus.EnumerationCompleted)
                {
                    watcher.Stop();
                }

                watcher.Added -= OnAdded;
                watcher.EnumerationCompleted -= OnEnumerationComplete;
                watcher.Removed -= OnRemoved;
                watcher.Stopped -= OnStopped;
                watcher.Updated -= OnUpdated;

                watcher = null;
            }
        }

        public void ResetState()
        {
            ResetWatcher();

            foreach (var discovery in discovered.Values.ToList())
            {
                discovery.Dispose();
            }
            discovered.Clear();

            expectations.Clear();
        }

        ~WiFiDirectServicesWatcher()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                ResetState();

                stoppedEvent.Dispose();
                enumCompletedEvent.Dispose();
                foundAllEvent.Dispose();
            }

            disposed = true;
        }

        private void ThrowIfDisposed()
        {
            if (disposed)
            {
                throw new ObjectDisposedException("WiFiDirectServicesWatcher");
            }
        }
        #endregion
    }

    /// <summary>
    /// Data from DeviceInformation which can be serialized to pass over remote control
    /// </summary>
    public class DiscoveredServiceInfo
    {
        public DiscoveredServiceInfo()
        { }

        internal DiscoveredServiceInfo(DeviceInformation deviceInfo)
        {
            object serviceName;
            if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.ServiceName", out serviceName))
            {
                ServiceName = serviceName.ToString();
            }
            else
            {
                ServiceName = "";
            }

            object advertisementId;
            if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.AdvertisementId", out advertisementId))
            {
                AdvertisementId = Convert.ToUInt32(advertisementId, CultureInfo.InvariantCulture);
            }
            else
            {
                AdvertisementId = 0;
            }

            object configMethods;
            if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.ServiceConfigMethods", out configMethods))
            {
                ConfigMethods = Convert.ToUInt32(configMethods, CultureInfo.InvariantCulture);
            }
            else
            {
                ConfigMethods = 0;
            }

            object serviceAddress;
            if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.ServiceAddress", out serviceAddress))
            {
                byte[] macBuffer = (byte[])serviceAddress;
                ServiceAddress = new DOT11_MAC_ADDRESS(macBuffer);
            }
            else
            {
                ServiceAddress = new DOT11_MAC_ADDRESS();
            }

            ServiceInfo = "";
            object serviceInfo;
            if (deviceInfo.Properties.TryGetValue("System.Devices.WiFiDirectServices.ServiceInformation", out serviceInfo))
            {
                if (serviceInfo != null)
                {
                    IBuffer serviceInfoBuffer;
                    using (var serviceInfoDataWriter = new Windows.Storage.Streams.DataWriter(new Windows.Storage.Streams.InMemoryRandomAccessStream()))
                    {
                        serviceInfoDataWriter.WriteBytes((Byte[])serviceInfo);
                        serviceInfoBuffer = serviceInfoDataWriter.DetachBuffer();
                    }

                    using (DataReader serviceInfoDataReader = Windows.Storage.Streams.DataReader.FromBuffer(serviceInfoBuffer))
                    {
                        ServiceInfo = serviceInfoDataReader.ReadString(serviceInfoBuffer.Length);
                    }
                    WiFiDirectTestLogger.Log("Got Service Info: {0}",
                        WiFiDirectTestUtilities.GetTruncatedString(ServiceInfo, 256));
                }
            }
        }

        public string ServiceName { get; set; }
        public DOT11_MAC_ADDRESS ServiceAddress { get; set; }
        public UInt32 AdvertisementId { get; set; }
        public UInt32 ConfigMethods { get; set; }
        public string ServiceInfo { get; set; }
    }

    /// <summary>
    /// Data from WiFiDirectServiceProvisioningInfo which can be serialized to pass over remote control
    /// </summary>
    public class ProvisioningInfoWrapper
    {
        public ProvisioningInfoWrapper()
        {
        }

        public ProvisioningInfoWrapper(WiFiDirectServiceProvisioningInfo provisioningInfo)
        {
            if (provisioningInfo == null)
            {
                throw new ArgumentNullException("provisioningInfo");
            }
            IsGroupFormationNeeded = provisioningInfo.IsGroupFormationNeeded;
            SelectedConfigMethod = provisioningInfo.SelectedConfigurationMethod;
        }

        public bool IsGroupFormationNeeded { get; set; }
        public WiFiDirectServiceConfigurationMethod SelectedConfigMethod { get; set; }
    }

    /// <summary>
    /// Wraps Windows.Devices.Enumeration.DeviceInformation
    /// </summary>
    internal class DiscoveredService : IDisposable
    {
        private DeviceInformation deviceInfo;
        private WFDSvcWrapperHandle handle = new WFDSvcWrapperHandle();

        // When we start opening/connecting to a session we store this (only used for a single connect)
        private WiFiDirectService service = null;
        // Store config method so we know where PIN comes from (if needed)
        private WiFiDirectServiceConfigurationMethod selectedConfigMethod = WiFiDirectServiceConfigurationMethod.Default;

        private IWiFiDirectServicesSessionManager sessionManager;
        private IWiFiDirectServicesErrorHandler errorHandler;
        private IWiFIDirectServicesPinManager pinManager;

        private bool wasDeferred = false;
        private string deferredSessionInfo = "";

        internal DiscoveredService(
            DeviceInformation deviceInfo,
            IWiFiDirectServicesSessionManager sessionManager,
            IWiFiDirectServicesErrorHandler errorHandler,
            IWiFIDirectServicesPinManager pinManager
            )
        {
            this.deviceInfo = deviceInfo;
            this.sessionManager = sessionManager;
            this.errorHandler = errorHandler;
            this.pinManager = pinManager;
            
            WiFiDirectTestLogger.Log("Created DiscoveredService wrapper (Discovery={0})", handle);
        }

        #region Properties
        public WFDSvcWrapperHandle Handle
        {
            get { return handle; }
        }

        public DeviceInformation DeviceInfo
        {
            get { return deviceInfo; }
        }
        #endregion

        #region Actions
        /// <summary>
        /// Calls WiFiDirectService.FromId to open the seeker session
        /// Caller can then use this.Service to modify settings before connecting
        /// </summary>
        public void OpenSession()
        {
            ThrowIfDisposed();

            if (service != null)
            {
                throw new Exception("Attempted to connect to discovered service a second time!");
            }

            WiFiDirectTestLogger.Log("Connecting to service: Open Session (Discovery={0})", handle);

            wasDeferred = false;
            deferredSessionInfo = "";

            var serviceTask = WiFiDirectService.FromIdAsync(deviceInfo.Id).AsTask();
            serviceTask.Wait();
            service = serviceTask.Result;

            service.SessionDeferred += OnSessionDeferred;
        }

        public void SetServiceOptions(bool preferGO, string sessionInfo = "")
        {
            ThrowIfDisposed();

            if (service == null)
            {
                OpenSession();
            }

            WiFiDirectTestLogger.Log(
                "Setting Connect options, preferGO={1}, sessionInfo={2} (Discovery={0})",
                handle,
                preferGO,
                WiFiDirectTestUtilities.GetTruncatedString(sessionInfo)
                );

            service.PreferGroupOwnerMode = preferGO;

            if (sessionInfo != "")
            {
                using (var sessionInfoDataWriter = new Windows.Storage.Streams.DataWriter(new Windows.Storage.Streams.InMemoryRandomAccessStream()))
                {
                    sessionInfoDataWriter.WriteString(sessionInfo);
                    service.SessionInfo = sessionInfoDataWriter.DetachBuffer();
                }
            }
            else
            {
                service.SessionInfo = null;
            }
        }

        /// <summary>
        /// Calls service.GetProvisioningInfoAsync to begin provision discovery and
        /// determine if a PIN will be needed to connect
        /// </summary>
        /// <param name="configMethod">Selected Config Method to use</param>
        public async Task<WiFiDirectServiceProvisioningInfo> GetProvisioningInfoAsync(
            WiFiDirectServiceConfigurationMethod configMethod
            )
        {
            ThrowIfDisposed();

            if (service == null)
            {
                OpenSession();
            }

            WiFiDirectTestLogger.Log("Connecting to service: GetProvisioningInfoAsync... (Discovery={0})", handle);

            if (service == null)
            {
                throw new Exception("OpenSession called but service is still null, this is a fatal error, need to bail out.");
            }

            WiFiDirectServiceProvisioningInfo provisioningInfo = await service.GetProvisioningInfoAsync(configMethod);

            WiFiDirectTestLogger.Log("Connecting to service: GetProvisioningInfoAsync DONE (Discovery={0})", handle);
            if (provisioningInfo.IsGroupFormationNeeded)
            {
                selectedConfigMethod = provisioningInfo.SelectedConfigurationMethod;
                if (provisioningInfo.SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod.PinDisplay)
                {
                    WiFiDirectTestLogger.Log(
                        "Group Formation is needed, should DISPLAY PIN (Discovery={0})",
                        handle
                        );
                    this.pinManager.GeneratePin();
                }
                else if (provisioningInfo.SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod.PinEntry)
                {
                    WiFiDirectTestLogger.Log(
                        "Group Formation is needed, should ENTER PIN (Discovery={0})",
                        handle
                        );
                }
                else
                {
                    WiFiDirectTestLogger.Log(
                        "Group Formation is needed, no PIN (Discovery={0})",
                        handle
                        );
                }
            }

            return provisioningInfo;
        }

        /// <summary>
        /// Calls service.ConnectAsync() to connect the session
        /// If the session is not already opened, it calls OpenSession
        /// </summary>
        /// <returns>handle to the connected session</returns>
        public async Task<WFDSvcWrapperHandle> ConnectAsync()
        {
            ThrowIfDisposed();

            if (service == null)
            {
                OpenSession();
            }

            WiFiDirectServiceSession session = null;

            string pin = "";

            if (selectedConfigMethod == WiFiDirectServiceConfigurationMethod.PinDisplay)
            {
                // Just retrieve the PIN we generated to pass to API
                pin = this.pinManager.RetrievePin();
            }
            else if (selectedConfigMethod == WiFiDirectServiceConfigurationMethod.PinEntry)
            {
                // Wait on PIN entry event, retrieve PIN from service
                this.pinManager.WaitForPinEntry();
                pin = this.pinManager.RetrievePin();
            }

            WiFiDirectTestLogger.Log("Connecting to service: ConnectAsync... (pin={1}) (Discovery={0})", handle, pin);
            
            if (service == null)
            {
                throw new Exception("OpenSession called but service is still null, this is a fatal error, need to bail out.");
            }

            if (pin.Length > 0)
            {
                session = await service.ConnectAsync(pin);
            }
            else
            {
                session = await service.ConnectAsync();
            }

            WiFiDirectTestLogger.Log("Connecting to service: ConnectAsync DONE (Discovery={0})", handle);
            
            // Now we are done with this WiFiDirectService instance
            // Clear state so a new connection can be started
            service.SessionDeferred -= OnSessionDeferred;
            service = null;

            ServiceSession sessionWrapper = new ServiceSession(session, sessionManager, errorHandler, wasDeferred, deferredSessionInfo);
            sessionManager.AddSession(sessionWrapper);
            
         