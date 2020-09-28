#define STRSAFE_NO_DEPRECATE
#include <etwConsumersRequire.h>
#include <provider.h>

#include <lddmcoreeventdefs.h>
#include <d3dkmthk.h>
#include <d3dkmthk_p.h>
#include <d3dukmdt.h>
#include <dmmdiag.h>
#include <dxetwevt.hxx>

#include <DxgKernelConsumer.h>
#include <DxgKernelLegConsumer.h>
#include <Controller.h>
#include <etwStatistics.h>
#include <DXGKernelStatistics.h>

#include <etwLibEvents.h>

#include "TestRunner.h"
#include "testcomponent.h"

#include "BaseVid.h"

#include <PreemptionCaps.h>
#include <CrossProcSharing.h>

using namespace std;

#include "PreemptionEvent.h"
#include <Quadrant.h>


extern CProvider etwProvider;


//------------------------------------------------------------------------------
CBasePreemptionEvent::CBasePreemptionEvent() :
    ulCount(0),
    ulRecordingTime(0),
    ulLoopIterations(0),
    m_SupportsGraphicsPreemption(false),
    m_SupportsComputePreemption(false),
    m_bSoftwareDevice(false)
{
    // Basic sync functionality
    AddParam<std::string>(L"SyncKeyString", L"Unique string to define global shared memory name");
    AddParam<UINT>(L"CountOfProcesses", L"Number of applications that will be monitored including manager");
    AddParam<UINT>(L"RecordingTime", L"Number of seconds to listen for events");
    AddParam<UINT>(L"LoopIterations", L"Number of times to retry calling the workhorse client");
    AddParam<UINT>(L"QuadrantLoc", L"Number either 1,2,3 or 4 - denotes window quadrant to use");
    
    // Set the default for these command line arguments. 
    SetParamTestDomainSingleValue<std::string>(L"SyncKeyString", "UnknownTest");
    SetParamTestDomainSingleValue<UINT>(L"CountOfProcesses", 0);
    SetParamTestDomainSingleValue<UINT>(L"RecordingTime", 30);
    SetParamTestDomainSingleValue<UINT>(L"LoopIterations", 0);
    SetParamTestDomainSingleValue<UINT>(L"QuadrantLoc", 0);
}


CBasePreemptionEvent::~CBasePreemptionEvent()
{
}


void CBasePreemptionEvent::ReadBaseCommandLine()
{
    SyncKey = GetParam<std::string>(L"SyncKeyString");
    ulCount = GetParam<UINT>(L"CountOfProcesses");
    ulRecordingTime = GetParam<UINT>(L"RecordingTime");
    ulLoopIterations = GetParam<UINT>(L"LoopIterations");
    m_uiQuadrantLoc = GetParam<UINT>(L"QuadrantLoc");
}


// It's expected that the last character will be \n.
void CBasePreemptionEvent::LogMessage(WCHAR* _pMsg)
{
    OutputDebugString(_pMsg);
    OutputDebugString(L"\r");
    m_pLog->Message(_pMsg);
}

void CBasePreemptionEvent::GetPreemptionCapabilties()
{
    // Need to create the class and then call the query routine.
    CQueryPreemptionCaps VidHardwareCaps(m_pLog);
    HRESULT hr = VidHardwareCaps.QueryForCapabilities(m_pDevice->GetDisplayName());
    if( S_OK == hr )
    {
        //successfully got capabilities
        if( VidHardwareCaps.IsPreemptableGraphics() )
        {
            LogMessageF(m_pLog, TEXT("This adapter (%s) supports Graphics Preemption"),
                        m_pDevice->GetDisplayName());

            // Log the level
            m_GraphicsPreemptionGranularity = VidHardwareCaps.GetGraphicsPreemptionGranularity();
            LogMessageF(m_pLog, TEXT("Graphics Preemption level %d"),m_GraphicsPreemptionGranularity);

            m_SupportsGraphicsPreemption=true;
        }
        else
        {
            LogMessageF(m_pLog, TEXT("This adapter (%s) does not support Graphics Preemption"),
                        m_pDevice->GetDisplayName());
        }

        if( VidHardwareCaps.IsPreemptableCompute() )
        {
            LogMessageF(m_pLog, TEXT("This adapter (%s) supports Compute Preemption"),
                        m_pDevice->GetDisplayName());

            // log the level
            m_ComputePreemptionGranularity = VidHardwareCaps.GetComputePreemptionGranularity();
            LogMessageF(m_pLog, TEXT("Compute Preemption level %d"),m_ComputePreemptionGranularity);

            m_SupportsComputePreemption=true;
        }
        else
        {
            LogMessageF(m_pLog, TEXT("This adapter (%s) does not support Compute Preemption"),
                        m_pDevice->GetDisplayName());
        }

        // Now, are we running on a software device or not. If software, we will not get
        // our preemption events
        m_bSoftwareDevice = VidHardwareCaps.IsSoftwareDevice();
    }
}


//------------------------------------------------------------------------------
//
// Sample command line might look like:
//
// SchManager.exe ListenForPreemptionEvent MissPagingCommand:30 Time:20 SyncWith:XYZ ProcCount:2 Windowed:true
//
// This basically says, that this class should count PreemptionAttemptMissPagingCommand events after xProc syncing
// to XYZ with only one client and after signaling them to move into the run state, count for 20 seconds. If we
// recieve 30 of these events before the 20 seconds elapse, we'll signal the client to terminate and log a success.
// if the 20 seconds pass and we do not count 30 of these events, the application will fail.
// 
// Note that the class can only monitor for one type of preemption event at a time, but it 
// will log the results of all types at the end of the test.
//
CXProcListenForPreemptionEvent::CXProcListenForPreemptionEvent() :
        m_ulSuccess(0),
        m_ulMissNoCommand(0),
        m_ulMissNotEnabled(0),
        m_ulMissNextFence(0),
        m_ulSuccessNextFence(0),
        m_ulMissPagingCommand(0),
        m_ulSuccessPagingCommand(0),
        m_ulMissSplittedCommand(0),
        m_ulSuccessSplittedCommand(0),
        m_ulMissFenceCommand(0),
        m_ulMissRenderPendingFlip(0),
        m_ulSuccessRenderPendingFlip(0),
        m_ulMissNotMakingProgress(0),
        m_ulMissLessPriority(0),
        m_ulMissRemainingQuantum(0),
        m_ulMissRemainingPreemptionQuantum(0),
        m_ulMissAlreadyPreempting(0),
        m_ulMissGlobalBlock(0),
        m_ulSuccessGlobalBlock(0),
        m_ulMissAlreadyRunning(0),
        m_ulSuccessHigherInProcPriority(0)
{
    WCHAR* pDesc = L"Type of Preemption Event to count";

    // Specific items to test for
    AddParam<UINT>(L"vSuccess", pDesc);
    AddParam<UINT>(L"vMissNoCommand", pDesc);
    AddParam<UINT>(L"vMissNotEnabled", pDesc);
    AddParam<UINT>(L"vMissNextFence", pDesc);
    AddParam<UINT>(L"vSuccessNextFence", pDesc);
    AddParam<UINT>(L"vMissPagingCommand", pDesc);
    AddParam<UINT>(L"vSuccessPagingCommand", pDesc);
    AddParam<UINT>(L"vMissSplittedCommand", pDesc);
    AddParam<UINT>(L"vSuccessSplittedCommand", pDesc);
    AddParam<UINT>(L"vMissFenceCommand", pDesc);
    AddParam<UINT>(L"vMissRenderPendingFlip", pDesc);
    AddParam<UINT>(L"vSuccessRenderPendingFlip", pDesc);
    AddParam<UINT>(L"vMissNotMakingProgress", pDesc);
    AddParam<UINT>(L"vMissLessPriority", pDesc);
    AddParam<UINT>(L"vMissRemainingQuantum", pDesc);
    AddParam<UINT>(L"vMissRemainingPreemptionQuantum", pDesc);
    AddParam<UINT>(L"vMissAlreadyPreempting", pDesc);
    AddParam<UINT>(L"vMissGlobalBlock", pDesc);
    AddParam<UINT>(L"vSuccessGlobalBlock", pDesc);
    AddParam<UINT>(L"vMissAlreadyRunning", pDesc);
    AddParam<UINT>(L"vSuccessHigherInProcPriority", pDesc);

    AddParam<bool>(L"bTrackChangeEvents", L"If true, we track ChangeInProcessPriority events");
    AddParam<bool>(L"bWantIHVHardware", L"Skip if we're not running in IHV hardware");
    AddParam<bool>(L"bStability", L"Run as stability test, don't fail on low counts");


    SetParamTestDomainSingleValue<UINT>(L"vSuccess", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissNoCommand", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissNotEnabled", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissNextFence", 0);
    SetParamTestDomainSingleValue<UINT>(L"vSuccessNextFence", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissPagingCommand", 0);
    SetParamTestDomainSingleValue<UINT>(L"vSuccessPagingCommand", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissSplittedCommand", 0);
    SetParamTestDomainSingleValue<UINT>(L"vSuccessSplittedCommand", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissFenceCommand", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissRenderPendingFlip", 0);
    SetParamTestDomainSingleValue<UINT>(L"vSuccessRenderPendingFlip", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissNotMakingProgress", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissLessPriority", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissRemainingQuantum", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissRemainingPreemptionQuantum", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissAlreadyPreempting", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissGlobalBlock", 0);
    SetParamTestDomainSingleValue<UINT>(L"vSuccessGlobalBlock", 0);
    SetParamTestDomainSingleValue<UINT>(L"vMissAlreadyRunning", 0);
    SetParamTestDomainSingleValue<UINT>(L"vSuccessHigherInProcPriority", 0);

    SetParamTestDomainSingleValue<bool>(L"bTrackChangeEvents", false);
    SetParamTestDomainSingleValue<bool>(L"bWantIHVHardware", false);
    SetParamTestDomainSingleValue<bool>(L"bStability", true);
}

#define LOG_NUMBERS(_Name_) { \
WCHAR wszBuf[MAX_PATH]; \
StringCchPrintf(wszBuf,MAX_PATH,L#_Name_ L"=%d",etwPreemptionCounterConsumer.##_Name_()); \
m_pLog->Message(wszBuf); }

// See command line info above.
void CXProcListenForPreemptionEvent::RunTest()
{
    bool bPassTest=false;
    ReadBaseCommandLine();

    DeviceCreateOptions dcOptions;
    m_pDevice->GetCreationOptions(&dcOptions);

    // Set the window location
    CQuadrant WindowLoc(dcOptions.hWnd,m_uiQuadrantLoc,SIDE_PIXELS);

#define READ_VALUE(_Name_) m_ul##_Name_ = GetParam<UINT>(L"v" L#_Name_)

    READ_VALUE(Success);
    READ_VALUE(MissNoCommand);
    READ_VALUE(MissNotEnabled);
    READ_VALUE(MissNextFence);
    READ_VALUE(SuccessNextFence);
    READ_VALUE(MissPagingCommand);
    READ_VALUE(SuccessPagingCommand);
    READ_VALUE(MissSplittedCommand);
    READ_VALUE(SuccessSplittedCommand);
    READ_VALUE(MissFenceCommand);
    READ_VALUE(MissRenderPendingFlip);
    READ_VALUE(SuccessRenderPendingFlip);
    READ_VALUE(MissNotMakingProgress);
    READ_VALUE(MissLessPriority);
    READ_VALUE(MissRemainingQuantum);
    READ_VALUE(MissRemainingPreemptionQuantum);
    READ_VALUE(MissAlreadyPreempting);
    READ_VALUE(MissGlobalBlock);
    READ_VALUE(SuccessGlobalBlock);
    READ_VALUE(MissAlreadyRunning);
    READ_VALUE(SuccessHigherInProcPriority);

    bool bLookingForChangeEvents = GetParam<bool>(L"bTrackChangeEvents");
    bool bWantIHVHardware = GetParam<bool>(L"bWantIHVHardware");
    bool bStability = GetParam<bool>(L"bStability");

    // Before we can really do anything we need to query for the hardware capabilities.
    // We basically need to find out if the hardware supports preemption. If so, we
    // can look for our new preemption functionality, if not, well, we can not.
    GetPreemptionCapabilties();

    // If the user requested IHV hardware and we're running MS software, then we're going
    // to skip this test.
    if( bWantIHVHardware && m_bSoftwareDevice )
    {
        CXProcSyncManager xProcSync((char*)SyncKey.c_str(),ulCount); 

        // We'll only get a true back if all the workhorse clients move into the running state. If that
        // fails for some reason, we'll get false.
        LogMessage(TEXT("Manager: Waiting for clients to register to terminate them.\n"));
        if( xProcSync.TransitionClientsTo(eMoveToTerminate) )
        {
            LogMessage(TEXT("Manager: Running on Software only device.\n"));
        }
        m_pLog->Skip(L"User requested IHV Hardware, but we're running on a software device.");
        return;
    }

    // Now we need to determine if we're going to find what we're looking for on this system.
    // Basically, if the hardware does not support preemption, we will not get any of the 
    // preemption specific events on the machine. So, we'll guard against false positives.

#define IS_REQUESTED(_Name_) ( 0 != m_ul##_Name_ )

    if( IS_REQUESTED(SuccessNextFence) ||
        IS_REQUESTED(SuccessPagingCommand) ||
        IS_REQUESTED(SuccessSplittedCommand) ||
        IS_REQUESTED(SuccessRenderPendingFlip) ||
        IS_REQUESTED(SuccessGlobalBlock) )
    {
        // user is asking us to track an event that only occurs on preemption capable hardware.
        // if the hardware doesn't support preemption, we need to shutdown correctly
        if( !m_SupportsGraphicsPreemption && !m_SupportsComputePreemption )
        {
            // the driver is not preemption aware, gracefully shutdown - no failure.
            // Create our xProc Sharing class and instruct our workhorse client to move into
            // a running state. As soon as we get notification that they've moved states, we'll
            // start our counter and count for RecordingTime seconds.
            CXProcSyncManager xProcSync((char*)SyncKey.c_str(),ulCount); 

            // We'll only get a true back if all the workhorse clients move into the running state. If that
            // fails for some reason, we'll get false.
            LogMessage(TEXT("Manager: Waiting for clients to register to terminate them.\n"));
            if( xProcSync.TransitionClientsTo(eMoveToTerminate) )
            {
                LogMessage(TEXT("Manager: Hardware does not support preemption.\n"));
            }
            m_pLog->Skip(L"Test requires preemption support.");
            return;
        }
    }


    // Now that we're read our conditions in, let's create the infrastructure that
    // we need for counting and syncing with the workhorse client.

    // setup our event capturing infrastructure, add our counter and start events flowing
    CController etwController;
    CPreemptionCounter etwPreemptionCounterConsumer(&etwController);
    etwController.AddConsumer(&etwPreemptionCounterConsumer);
    etwController.StartProcessing();

    // Create our xProc Sharing class and instruct our workhorse client to move into
    // a running state. As soon as we get notification that they've moved states, we'll
    // start our counter and count for RecordingTime seconds.
    CXProcSyncManager xProcSync((char*)SyncKey.c_str(),ulCount); 

    // We'll only get a true back if all the workhorse clients move into the running state. If that
    // fails for some reason, we'll get false.
    LogMessage(TEXT("Manager: Waiting for clients to register and move into a running state.\n"));
    if( xProcSync.TransitionClientsTo(eMoveToRunning) )
    {
        // Here is where we're going to want to loop. We're going to make this so that it
        // askes the client "LoopIterations" times to increase it's workload until the workload is
        // enough to generate the events that we're looking for.
        ULONG ulNumberOfTries=ulLoopIterations;
        UINT uiSelf = xProcSync.GetIndex();
        ULONG ulCurrentTimeMS;
        bool bClientTerminated = false;
        do
        {
            LogMessage(TEXT("Starting counting process!\n"));
            // Set our counters to zero.
            etwPreemptionCounterConsumer.Reset();

            etwPreemptionCounterConsumer.Start();

            // now we're going to loop counting the particular preemption events that we're looking for.
            // if we find the target number, we'll terminate. Or, if we count for the full RecordingTime
            // worth of seconds, we'll bail and fail.

            ULONG ulStartingTimeMS = GetTickCount();

            do
            {
                // We'll wake up every 5th of a second to check our variables
                Sleep(200);

#define CHECK_NUMBERS(_Name_) if( 0 != m_ul##_Name_ && ( etwPreemptionCounterConsumer.##_Name_() > m_ul##_Name_ ) ) \
{ WCHAR wszBuf[MAX_PATH]; \
StringCchPrintf(wszBuf,MAX_PATH,L"Breaking based on count: " L#_Name_ L"=%d\n\r",etwPreemptionCounterConsumer.##_Name_()); \
m_pLog->Message(wszBuf); \
break; }

                CHECK_NUMBERS(Success);
                CHECK_NUMBERS(MissNoCommand);
                CHECK_NUMBERS(MissNotEnabled);
                CHECK_NUMBERS(MissNextFence);
                CHECK_NUMBERS(SuccessNextFence);
                CHECK_NUMBERS(MissPagingCommand);
                CHECK_NUMBERS(SuccessPagingCommand);
                CHECK_NUMBERS(MissSplittedCommand);
                CHECK_NUMBERS(SuccessSplittedCommand);
                CHECK_NUMBERS(MissFenceCommand);
                CHECK_NUMBERS(MissRenderPendingFlip);
                CHECK_NUMBERS(SuccessRenderPendingFlip);
                CHECK_NUMBERS(MissNotMakingProgress);
                CHECK_NUMBERS(MissLessPriority);
                CHECK_NUMBERS(MissRemainingQuantum);
                CHECK_NUMBERS(MissRemainingPreemptionQuantum);
                CHECK_NUMBERS(MissAlreadyPreempting);
                CHECK_NUMBERS(MissGlobalBlock);
                CHECK_NUMBERS(SuccessGlobalBlock);
                CHECK_NUMBERS(MissAlreadyRunning);
                CHECK_NUMBERS(SuccessHigherInProcPriority);

                // Get the current time to see if we've exceeded our recording threshold
                ulCurrentTimeMS = GetTickCount();

                // Now double check to make sure our clients haven't left. If they have, then
                // we'll want to shut down too.  Just have to make sure that we overlook ourself.

                if( ((ulCurrentTimeMS-ulStartingTimeMS)/1000) > 5 ) // after five seconds, we look to see if clients have termianted.
                {
                    for(UINT ui=0;ui<ulCount;ui++)
                    {
                        if( ui != uiSelf )
                        {
                            STATE_TYPE stClient = xProcSync.GetClientState(ui);
                
                            // now, if our client has terminated, we should be able to see that here
                            if( eStateTerminate == stClient )
                            {
                                // This client is done and gone. we should not loop any longer.
                                bClientTerminated = true;
                            }
                        }
                    }
                }

            }
            while( ((ulCurrentTimeMS-ulStartingTimeMS)/1000) < ulRecordingTime && 
                   !bClientTerminated);

            etwPreemptionCounterConsumer.Stop();
            LogMessage(TEXT("Stop counting process!\n"));

#ifdef DBG

#define DBG_LOG_NUMBERS(_Name_) { \
WCHAR wszBuf[MAX_PATH]; \
StringCchPrintf(wszBuf,MAX_PATH,L#_Name_ L"=%d\n\r",etwPreemptionCounterConsumer.##_Name_()); \
OutputDebugString(wszBuf); }


            OutputDebugString(L"Results of one pass:\n\r");

            DBG_LOG_NUMBERS(Success);
            DBG_LOG_NUMBERS(MissNoCommand);
            DBG_LOG_NUMBERS(MissNotEnabled);
            DBG_LOG_NUMBERS(MissNextFence);
            DBG_LOG_NUMBERS(SuccessNextFence);
            DBG_LOG_NUMBERS(MissPagingCommand);
            DBG_LOG_NUMBERS(SuccessPagingCommand);
            DBG_LOG_NUMBERS(MissSplittedCommand);
            DBG_LOG_NUMBERS(SuccessSplittedCommand);
            DBG_LOG_NUMBERS(MissFenceCommand);
            DBG_LOG_NUMBERS(MissRenderPendingFlip);
            DBG_LOG_NUMBERS(SuccessRenderPendingFlip);
            DBG_LOG_NUMBERS(MissNotMakingProgress);
            DBG_LOG_NUMBERS(MissLessPriority);
            DBG_LOG_NUMBERS(MissRemainingQuantum);
            DBG_LOG_NUMBERS(MissRemainingPreemptionQuantum);
            DBG_LOG_NUMBERS(MissAlreadyPreempting);
            DBG_LOG_NUMBERS(MissGlobalBlock);
            DBG_LOG_NUMBERS(SuccessGlobalBlock);
            DBG_LOG_NUMBERS(MissAlreadyRunning);
            DBG_LOG_NUMBERS(SuccessHigherInProcPriority);

#endif

            // If we've timed out because we didn't count enough of the events that we're 
            // looking for, we're going to transition the client into the Done state and then
            // back to ready so we can make another pass.

            if( ((ulCurrentTimeMS-ulStartingTimeMS)/1000) >= ulRecordingTime )
            {
                LogMessage(TEXT("Transitioning to a ready state to loop again!\n"));
                // We exceeded the amount of time that the user asked for. Let's get the client
                // ready for another go at it.
                xProcSync.TransitionClientsTo(eMoveToReady);
            }
            else
            {
                if( bClientTerminated )
                {
                    LogMessage(TEXT("Client terminated before we could reach our count!\n"));

                    // Only display this message if we're tracking SuccessHigherInProcPriority events.
                    if( m_ulSuccessHigherInProcPriority )
                    {
                        WCHAR wszBuf[MAX_PATH];
                        StringCchPrintf(wszBuf,MAX_PATH,L"Wanted=%d events, received=%d",
                                        m_ulSuccessHigherInProcPriority,
                                        etwPreemptionCounterConsumer.SuccessHigherInProcPriority());
                        m_pLog->Message(wszBuf);
                    }
                    break;
                }
                else
                {
                    // we counted the events that we wanted to see!
                    LogMessage(TEXT("Passing test based on count!\n"));
                    bPassTest=true;
                    break;
                }
            }

            // If we don't pass the test the first time, loop the number of times the caller
            // wants us too.
            ulNumberOfTries--;
            if( 0 == ulNumberOfTries )
            {
                LogMessage(TEXT("Bailing based on tries!\n"));
                // we give up.
                break;
            }

        }
        while(xProcSync.TransitionClientsTo(eMoveToRunning));

        // If we're tracking ChangeInProcessPriority events, validate that we got some here.
        if( bLookingForChangeEvents )
        {
            WCHAR wszBuf[MAX_PATH];
            ULONG ulHitCount = etwPreemptionCounterConsumer.GetChangeInPriorityCount();
            if( 0 == ulHitCount )
            {
                m_pLog->Fail(L"Did not count enough ChangeInProcessPriority events.");
            }
            StringCchPrintf(wszBuf,MAX_PATH, L"Test counted %d ChangeInProcessPriority events.\n\r",ulHitCount);
            m_pLog->Message(wszBuf);
        }

        // Now, do we pass the test or not?
        if( !bPassTest )
        {
            if( bStability )
            {
                // We exceeded the amount of time that the user asked for. This must be a failure.
                m_pLog->Message(L"Did not count enough events during the given time period.");
            }
            else
            {
                // We exceeded the amount of time that the user asked for. This must be a failure.
                m_pLog->Fail(L"Did not count enough events during the given time period.");
            }
        }
        else
        {
            m_pLog->Message(L"Test counted at least the given number of events during the given time period.");
        }

        // We are done counting, let's move the workhorse client into a terminate state.
        xProcSync.TransitionClientsTo(eMoveToTerminate);

        // Now stop events from flowing
        etwController.StopProcessing();

        // Now, before we exit, we'll log out all the preemption information

        LOG_NUMBERS(Success);
        LOG_NUMBERS(MissNoCommand);
        LOG_NUMBERS(MissNotEnabled);
        LOG_NUMBERS(MissNextFence);
        LOG_NUMBERS(SuccessNextFence);
        LOG_NUMBERS(MissPagingCommand);
        LOG_NUMBERS(SuccessPagingCommand);
        LOG_NUMBERS(MissSplittedCommand);
        LOG_NUMBERS(SuccessSplittedCommand);
        LOG_NUMBERS(MissFenceCommand);
        LOG_NUMBERS(MissRenderPendingFlip);
        LOG_NUMBERS(SuccessRenderPendingFlip);
        LOG_NUMBERS(MissNotMakingProgress);
        LOG_NUMBERS(MissLessPriority);
        LOG_NUMBERS(MissRemainingQuantum);
        LOG_NUMBERS(MissRemainingPreemptionQuantum);
        LOG_NUMBERS(MissAlreadyPreempting);
        LOG_NUMBERS(MissGlobalBlock);
        LOG_NUMBERS(SuccessGlobalBlock);
        LOG_NUMBERS(MissAlreadyRunning);
        LOG_NUMBERS(SuccessHigherInProcPriority);
    }
    else
    {
        // Now stop events from flowing
        etwController.StopProcessing();

        m_pLog->Fail(L"Clients failed to move to a running state");
    }
   
}
void CXProcListenForPreemptionEvent::EndTest()
{
}


//------------------------------------------------------------------------------
//
// Sample command line might look like:
//
// SchManager.exe ListenForPreemptionTime Time:5 SyncWith:XYZ ProcCount:2 PoliceMS:true Loop:20 Windowed:true
//
// This basically says that the class should monitor the time it takes for Preemption packets
// to complete in the GPU hardware queue for a duration of 5 seconds before looping and performing 
// the same checks. It will loop 20 times and all the while, police that we never get a preemption
// packet longer then the spec'd time of 4 milliseconds (on preemption aware hardware). If the 
// code finds that a preemption packet has taken longer then 4 milliseconds, it will fail. This
// class also syncs with the clients using the SynWith and ProcCount options - which are required.
// 
CXProcListenForPreemptionTime::CXProcListenForPreemptionTime()
{
    AddParam<bool>(L"PoliceMS", L"Fail if maximum millisecond threshold is exceeded");
    AddParam<bool>(L"EnforcePreemptionSupport", L"Fail if hardware doesn't support preemption");
    
    // Set the default for these command line arguments. 
    SetParamTestDomainSingleValue<bool>(L"PoliceMS", false);
    SetParamTestDomainSingleValue<bool>(L"EnforcePreemptionSupport", false);
}

// The basic idea behind this test is to count the time it takes preemption packets once
// they have been submitted to the hardware queue. We'll count for as long as the 
// user requests and then log out statistics at the end of the pass. We'll reset
// after each loop.
void CXProcListenForPreemptionTime::RunTest()
{
    bool bPassTest=true;
    ReadBaseCommandLine();

    DeviceCreateOptions dcOptions;
    m_pDevice->GetCreationOptions(&dcOptions);

    // Set the window location
    CQuadrant WindowLoc(dcOptions.hWnd,m_uiQuadrantLoc,SIDE_PIXELS);

    bool bPoliceMS = GetParam<bool>(L"PoliceMS");
    bool bEnforcePreemptionSupport = GetParam<bool>(L"EnforcePreemptionSupport");

    GetPreemptionCapabilties();

    // We can only fail when we detect long preemption packets on hardware that claims
    // to be preemption aware. Thus, if the user instructed this code to police this
    // time, we need to validate against the hardware.
    if( bPoliceMS )
    {
        // if the hardware doesn't support preemption, we can not police the longest time.
        if( !m_SupportsGraphicsPreemption && !m_SupportsComputePreemption )
        {
            LogMessage(TEXT("Manager: turning off PoliceMS functionality.\n"));
            bPoliceMS = false;
        }
    }

    // if the machine doesn't support preemption yet we are being run iwth the 
    // bEnforcePreemptionSupport option, it means that we expect that the preemption 
    // support level must be DMA boundary or greater (100 or greater).
    if( bEnforcePreemptionSupport )
    {
        if(  D3DKMDT_GRAPHICS_PREEMPTION_NONE == m_GraphicsPreemptionGranularity|| 
             D3DKMDT_COMPUTE_PREEMPTION_NONE == m_ComputePreemptionGranularity )
        {
            // Move workhorse client to terminate
            CXProcSyncManager xProcSync((char*)SyncKey.c_str(),ulCount); 

            // We'll only get a true back if all the workhorse clients move into the running state. If that
            // fails for some reason, we'll get false.
            LogMessage(TEXT("Manager: Waiting for clients to register to terminate them.\n"));
            if( xProcSync.TransitionClientsTo(eMoveToTerminate) )
            {
                LogMessage(TEXT("Manager: Hardware does not support preemption.\n"));
            }
            m_pLog->Fail(L"Test requires driver to report non-zero preemption values.");
            return;
        }
    }

    // if the machine doesn't support preemption, and the user is asking to test with it,
    // then we need to skip-out.
    if( !m_SupportsGraphicsPreemption && !m_SupportsComputePreemption )
    {
        // the driver is not preemption aware, gracefully shutdown - no failure.
        // Create our xProc Sharing class and instruct our workhorse client to move into
        // a running state. As soon as we get notification that they've moved states, we'll
        // start our counter and count for RecordingTime seconds.
        CXProcSyncManager xProcSync((char*)SyncKey.c_str(),ulCount); 

        // We'll only get a true back if all the workhorse clients move into the running state. If that
        // fails for some reason, we'll get false.
        LogMessage(TEXT("Manager: Waiting for clients to register to terminate them.\n"));
        if( xProcSync.TransitionClientsTo(eMoveToTerminate) )
        {
            LogMessage(TEXT("Manager: Hardware does not support preemption.\n"));
        }
        m_pLog->Skip(L"Test requires preemption support.");
        return;
    }

    // Now that we're read our conditions in, let's create the infrastructure that
    // we need for counting and syncing with the workhorse client.

    // setup our event capturing infrastructure, add our counter and start events flowing
    CController etwController;
    CDMAPreemptionTime etwPreemptionTimeConsumer(&etwController);
    CDMAPreemptionNextCompletionTime etwPreemptionCompletionConsumer(&etwController);
    CDMAStandardTime etwDMAStandardTimeConsumer(&etwController,false,false); // measure time spent running on hardware);
    etwController.AddConsumer(&etwPreemptionTimeConsumer);
    etwController.AddConsumer(&etwPreemptionCompletionConsumer);
    etwController.AddConsumer(&etwDMAStandardTimeConsumer);
    etwDMAStandardTimeConsumer.MonitorSelf(false);
    etwController.StartProcessing();

    // Create our xProc Sharing class and instruct our workhorse client to move into
    // a running state. As soon as we get notification that they've moved states, we'll
    // start our counter and count for RecordingTime seconds.
    CXProcSyncManager xProcSync((char*)SyncKey.c_str(),ulCount); 

    // We'll only get a true back if all the workhorse clients move into the running state. If that
    // fails for some reason, we'll get false.
    LogMessage(TEXT("Manager: Waiting for clients to register and move into a running state.\n"));
    if( xProcSync.TransitionClientsTo(eMoveToRunning) )
    {
        // Here is where we're going to want to loop. We're going to make this so that it
        // askes the client "LoopIterations" times to increase it's workload until the workload is
        // enough to generate the events that we're looking for.
        ULONG ulNumberOfTries=ulLoopIterations;
        ULONG uiLoop=0;
        do
        {
            ULONGLONG ullLongestTimens=0;
            LogMessage(TEXT("Starting counting process!\n"));
            // Set our counters to zero.
            etwPreemptionTimeConsumer.Reset();
            etwPreemptionCompletionConsumer.Reset();
            etwDMAStandardTimeConsumer.Reset();
            etwPreemptionTimeConsumer.Start();
            etwPreemptionCompletionConsumer.Start();
            etwDMAStandardTimeConsumer.Start();

            // now we're going to loop counting the particular preemption events that we're looking for.

            ULONG ulStartingTimeMS = GetTickCount();
            ULONG ulCurrentTimeMS;
            bool bClientTerminated = false;
            UINT uiSelf = xProcSync.GetIndex();

            do
            {
                // We'll wake up every 5th of a second to check our variables
                Sleep(200);

                // Can only police hardware that actually preempts inside a DMA packet, thus they 
                // must report that they are mid-DMA or better.
                if( bPoliceMS &&
                    (m_GraphicsPreemptionGranularity >= D3DKMDT_GRAPHICS_PREEMPTION_PRIMITIVE_BOUNDARY) &&
                    (m_ComputePreemptionGranularity >= D3DKMDT_COMPUTE_PREEMPTION_DISPATCH_BOUNDARY) )
                {
                    // Look to see if we've captured a preemption event that's over 4 milliseconds in length
                    ullLongestTimens = etwPreemptionTimeConsumer.LongestTime();
                    // Convert the time into milliseconds
                    ullLongestTimens = etwController.TimeStampToMicroseconds(ullLongestTimens);

                    if( ullLongestTimens > LONGEST_SPECED_TIME_NS )
                    {
                        WCHAR wszTermStats[MAX_PATH];
                        bPassTest = false;

                        StringCchPrintf(wszTermStats,MAX_PATH,
                                        L"Manager: terminating due to excessive Premption Packet length! Time:%dns",
                                        ullLongestTimens);
                        LogMessage(wszTermStats);
                    }
                }

                // Get the current time to see if we've exceeded our recording threshold
                ulCurrentTimeMS = GetTickCount();

                // Now double check to make sure our clients haven't left. If they have, then
                // we'll want to shut down too.  Just have to make sure that we overlook ourself.

                if( ((ulCurrentTimeMS-ulStartingTimeMS)/1000) > 5 ) // after five seconds, we look to see if clients have termianted.
                {
                    for(UINT ui=0;ui<ulCount;ui++)
                    {
                        if( ui != uiSelf )
                        {
                            STATE_TYPE stClient = xProcSync.GetClientState(ui);
                
                            // now, if our client has terminated, we should be able to see that here
                            if( eStateTerminate == stClient )
                            {
                                // This client is done and gone. we should not loop any longer.
                                bClientTerminated = true;
                            }
                        }
                    }
                }

            }
            while( ((ulCurrentTimeMS-ulStartingTimeMS)/1000) < ulRecordingTime && 
                   bPassTest && 
                   !bClientTerminated);

            etwPreemptionTimeConsumer.Stop();
            etwPreemptionCompletionConsumer.Stop();
            etwDMAStandardTimeConsumer.Stop();
            WCHAR wszMsg[MAX_PATH];
            StringCchPrintf(wszMsg,MAX_PATH,L"Stop counting process!, TimeDelta=%d, pPassTest=%s, bClientTerminated=%s\n",
                            ((ulCurrentTimeMS-ulStartingTimeMS)/1000),
                            (bPassTest?L"true":L"false"),
                            (bClientTerminated?L"true":L"false"));
            LogMessage(wszMsg);

            // Increment our loop counter.
            uiLoop++;

#ifdef DBG
            OutputDebugString(L"Results of one pass:\n\r");
#endif
            // We've finished a loop, log out loop statistics here.

            // The first set of numbers will be form the PreemptionTime consumer.
            // This will give us how long the preemptions took.
            ULONGLONG ullTotalCount = etwPreemptionTimeConsumer.GetTotalCount();
            ULONGLONG ullTotalValue = etwPreemptionTimeConsumer.GetTotalValue();

            // Now convert the time value into Millisecond values.
            ULONGLONG ullTotalCountms = etwController.TimeStampToMilliseconds(ullTotalValue);

            ullLongestTimens = etwPreemptionTimeConsumer.LongestTime();
            // Convert the time into microseconds
            ullLongestTimens = etwController.TimeStampToMicroseconds(ullLongestTimens);

            float fAve=0.0;
            if( ullTotalCount )
            {
                // Average time per millisecond.
                fAve = (float) ullTotalCountms / (float)ullTotalCount;
            }

            WCHAR wszStats[MAX_PATH];
            StringCchPrintf(wszStats,MAX_PATH,
L"Loop(%d) Preemption: TotalCount=%I64d, TotalValue=%I64d(%I64dms) Longest=%I64dns, Ave=%fms",
                            uiLoop,ullTotalCount,ullTotalValue,ullTotalCountms,ullLongestTimens,fAve);
            LogMessage(wszStats);


            // The second set of numbers will be from the Premption to Completion consumer
            // which will give us the time it takes to wait for preemption and then
            // the following DMA packet that executes.

            ullTotalCount = etwPreemptionCompletionConsumer.GetTotalCount();
            ullTotalValue = etwPreemptionCompletionConsumer.GetTotalValue();

            // Now convert the time value into Microsecond values.
            ullTotalCountms = etwController.TimeStampToMilliseconds(ullTotalValue);

            fAve=0.0;
            if( ullTotalCount )
            {
                // Average time per millisecond.
                fAve = (float) ullTotalCountms / (float)ullTotalCount;
            }

            StringCchPrintf(wszStats,MAX_PATH,
L"Loop(%d) Completion: TotalCount=%I64d, TotalValue=%I64d(%I64dms) Ave=%fms",
                            uiLoop,ullTotalCount,ullTotalValue,ullTotalCountms,fAve);
            LogMessage(wszStats);

            if( ullLongestTimens > LONGEST_ALLOWED_TIME_NS )
            {
                // our preemption packets are getting too long, time to stop testing.
                break;
            }

            // If we've policing the packets and we failed the test, we don't really need to go any further.
            if( !bPassTest )
            {
                break;
            }

            // If we didn't recieve any ETW events, something went wrong. Note that we're using
            // a statistical class that tracks DMA packets rather than Preemption packets. We do this
            // because we will always get DMA packets, but we might not get preemption packets.
            if( 0 == etwDMAStandardTimeConsumer.GetTotalCount() )
            {
                LogMessage(TEXT("Did not recieve enough ETW events for test! recieved 0.\n"));
                bPassTest = false;
                break;
            }


            LogMessage(TEXT("Transitioning to a ready state to loop again!\n"));
            // We exceeded the amount of time that the user asked for. Let's get the client
            // ready for another go at it.
            xProcSync.TransitionClientsTo(eMoveToReady);

            // Once the client moves into a ready state, we're going to check the return status
            // from the work done in the loop above to see if we should terminate. If the clients
            // all return S_OK, we'll keep looping until we're done. This allows the client
            // to tell us that they are having problems or the data sets have grown too large.
            HRESULT hr = xProcSync.GetResults();
            if( hr != S_OK )
            {
                PATH_TRAP();
                LogMessage(TEXT("Client not in S_OK state, terminating test!\n"));
                break;
            }

            // Loop the number of times the caller wants us too.
            ulNumberOfTries--;
            if( 0 == ulNumberOfTries )
            {
                LogMessage(TEXT("Done looping!\n"));
                // we give up.
                break;
            }
        }
        while(xProcSync.TransitionClientsTo(eMoveToRunning));

        // Now, do we pass the test or not?
        if( !bPassTest )
        {
            // We exceeded the amount of time that the user asked for. This must be a failure.
            m_pLog->Fail(L"Found condition that causes failure (see log info above).");
        }
        else
        {
            m_pLog->Message(L"Test completed it's monitoring of the Preemption Packets.");
        }

        // We are done counting, let's move the workhorse client into a terminate state.
        xProcSync.TransitionClientsTo(eMoveToTerminate);

        // Now stop events from flowing
        etwController.StopProcessing();
    }
    else
    {
        // Now stop events from flowing
        etwController.StopProcessing();

        m_pLog->Fail(L"Clients failed to move to a running state");
    }
}


void CXProcListenForPreemptionTime::EndTest()
{
}

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//
// Sample command line might look like:
//
// SchManager.exe ListenForTime Time:5 SyncWith:XYZ ProcCount:2 PoliceMS:true Loop:20 Windowed:true
//
// This class loops for the giving number of seconds. It also syncs with the clients 
// using the SynWith and ProcCount options - which are required.
// 
CXProcListenForTime::CXProcListenForTime()
{
}

// The basic idea behind this test is to loop for a particular amount of time.
void CXProcListenForTime::RunTest()
{
    ReadBaseCommandLine();

    DeviceCreateOptions dcOptions;
    m_pDevice->GetCreationOptions(&dcOptions);

    // Set the window location
    CQuadrant WindowLoc(dcOptions.hWnd,m_uiQuadrantLoc,SIDE_PIXELS);

    // Now that we're read our conditions in, let's create the infrastructure that
    // we need for counting and syncing with the workhorse client.

    // Create our xProc Sharing class and instruct our workhorse client to move into
    // a running state. As soon as we get notification that they've moved states, we'll
    // start our counter and count for RecordingTime seconds.
    CXProcSyncManager xProcSync((char*)SyncKey.c_str(),ulCount); 

    // We'll only get a true back if all the workhorse clients move into the running state. If that
    // fails for some reason, we'll get false.
    LogMessage(TEXT("Manager: Waiting for clients to register and move into a running state.\n"));
    if( xProcSync.TransitionClientsTo(eMoveToRunning) )
    {
        // Here is where we're going to want to loop. We're going to make this so that it
        // askes the client "LoopIterations" times to increase it's workload until the workload is
        // enough to generate the events that we're looking for.
        ULONG ulNumberOfTries=ulLoopIterations;
        ULONG uiLoop=0;
        do
        {
            LogMessage(TEXT("Starting timing process!\n"));
            // Set our counters to zero.

            ULONG ulStartingTimeMS = GetTickCount();
            ULONG ulCurrentTimeMS;
            bool bClientTerminated = false;
            UINT uiSelf = xProcSync.GetIndex();

            do
            {
                // We'll wake up every 5th of a second to check our variables
                Sleep(200);

                // Get the current time to see if we've exceeded our recording threshold
                ulCurrentTimeMS = GetTickCount();

                // Now double check to make sure our clients haven't left. If they have, then
                // we'll want to shut down too.  Just have to make sure that we overlook ourself.

                if( ((ulCurrentTimeMS-ulStartingTimeMS)/1000) > 5 ) // after five seconds, we look to see if clients have termianted.
                {
                    for(UINT ui=0;ui<ulCount;ui++)
                    {
                        if( ui != uiSelf )
                        {
                            STATE_TYPE stClient = xProcSync.GetClientState(ui);
                
                            // now, if our client has terminated, we should be able to see that here
                            if( eStateTerminate == stClient )
                            {
                                // This client is done and gone. we should not loop any longer.
                                bClientTerminated = true;
                            }
                        }
                    }
                }

            }
            while( ((ulCurrentTimeMS-ulStartingTimeMS)/1000) < ulRecordingTime && 
                   !bClientTerminated);

            WCHAR wszMsg[MAX_PATH];
            StringCchPrintf(wszMsg,MAX_PATH,L"Stop timing process!, TimeDelta=%d, bClientTerminated=%s\n",
                            ((ulCurrentTimeMS-ulStartingTimeMS)/1000),
                            (bClientTerminated?L"true":L"false"));
            LogMessage(wszMsg);

            // Increment our loop counter.
            uiLoop++;

            LogMessage(TEXT("Transitioning to a ready state to loop again!\n"));
            // We exceeded the amount of time that the user asked for. Let's get the client
            // ready for another go at it.
            xProcSync.TransitionClientsTo(eMoveToReady);

            // Once the client moves into a ready state, we're going to check the return status
            // from the work done in the loop above to see if we should terminate. If the clients
            // all return S_OK, we'll keep looping until we're done. This allows the client
            // to tell us that they are having problems.
            HRESULT hr = xProcSync.GetResults();
            if( hr != S_OK )
            {
                PATH_TRAP();
                LogMessage(TEXT("Client not in S_OK state, terminating test!\n"));
                break;
            }

            // Loop the number of times the caller wants us too.
            ulNumberOfTries--;
            if( 0 == ulNumberOfTries )
            {
                LogMessage(TEXT("Done looping!\n"));
                // we give up.
                break;
            }
        }
        while(xProcSync.TransitionClientsTo(eMoveToRunning));

        // We are done timing, let's move the workhorse client into a terminate state.
        xProcSync.TransitionClientsTo(eMoveToTerminate);
    }
    else
    {
        m_pLog->Fail(L"Clients failed to move to a running state");
    }
}


void CXProcListenForTime::EndTest()
{
}


