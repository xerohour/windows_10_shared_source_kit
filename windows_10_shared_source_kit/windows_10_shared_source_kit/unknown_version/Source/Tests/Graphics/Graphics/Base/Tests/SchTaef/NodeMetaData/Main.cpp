/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*
* This is an example of an non-exception based test case.
* Exceptions are NOT enabled in the sources file by
* NOT specifying USE_NATIVE_EH = 1
*
* This example Creates a device for rendering and a window. It also shows how to get
* information off the Taef Command line.
*/
// The ActionMacros.h file requires the following three headers
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
                
// Included for friendly macros
#include <ActionMacros.h>

#include <tchar.h>
#include <dxgi.h>

#include <windows.h>
#include <wmistr.h>
#include <evntrace.h>
#include <strsafe.h>
#include <Evntcons.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include <tdh.h>
#include <EventHelper.h>
#include <etwLibEvents.h>
#include <Provider.h>
#include "etwLibEventHelper.h"

#include <D3Dx9math.h>
#include <D3D11.h>
#include <D3DCompiler.h>

#include "WexTestClass.h"

#include <vector>
#include "dxEtwHelper.h"
#include "XperfHelper.h"
#include "SchedPerfCommon.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace WEX::Common;
using namespace SchedPerfCommon;

// Bring in the following to see the names of the engines.
//#define WANT_DEBUG

#define _TMP(property, value) \
    TEST_METHOD_PROPERTY(property, value) \

namespace WEX 
{
    enum eWorkType 
    {
        eWorkType_ValidateForm,            // Validate meta data form
        eWorkType_Validate3D,              // Validate 3d engien was used.
        eWorkType_ValidateDecodeEngine,    // Validate Decode Engine was used
        eWorkType_ValidateEncodeEngine,    // N/A
        eWorkType_ValidateVideoProcessing, // Validate Video Processing engine was used.
        eWorkType_ValidateSceneAssembly,   // N/A
        eWorkType_ValidateCopy,            // Validate Copy (or 3d) engine was used.
        eWorkType_ValidateOverlay          // N/A
    };

    class NodeMetaData
    {
    public:
        // Declare this class as a TestClass, and supply metadata if necessary.
        BEGIN_TEST_CLASS(NodeMetaData)
            TEST_CLASS_PROPERTY(L"Kits.RunElevated", L"true")

            //Info that is global to all tests in this modele.
            //Description
            TEST_CLASS_PROPERTY(L"Kits.SupportedOS",L"Windows v10.0 Client x86")
            TEST_CLASS_PROPERTY(L"Kits.SupportedOS",L"Windows v10.0 Client x64")
            TEST_CLASS_PROPERTY(L"Kits.SupportedOS",L"Windows v10.0 Server x64")
            TEST_CLASS_PROPERTY(L"Kits.MinRelease",L"TH1")
            TEST_CLASS_PROPERTY(L"Kits.CorePackageComposition",L"Full")
            //Job Attributes:
            TEST_CLASS_PROPERTY(L"Kits.Specification", L"Device.Graphics.WDDM13.Render.GPUNode")
            TEST_CLASS_PROPERTY(L"Kits.IsManual", L"false")
            TEST_CLASS_PROPERTY(L"Kits.ProvidesAssessmentResult", L"false")
            TEST_CLASS_PROPERTY(L"Kits.HasSupplementalContent", L"false")
            TEST_CLASS_PROPERTY(L"Kits.DevelopmentPhase", L"Tuning and Validation")
            TEST_CLASS_PROPERTY(L"Kits.PublishingOrganization", L"Microsoft Corporation")
            TEST_CLASS_PROPERTY(L"Kits.TestType", L"Development")
			TEST_CLASS_PROPERTY(L"Kits.DeploymentItem", L"NTTEST\\windowstest\\graphics\\PerfX2\\perf_dx.dll")
			TEST_CLASS_PROPERTY(L"Kits.DeploymentItem", L"NTTEST\\windowstest\\graphics\\wddm\\bin\\addremove_perfdx.ps1")
			
			// TestTask[Setup] 1: Create a copy of perfcore.ini and then add perf_dx.dll to perfcore.ini
			TEST_CLASS_PROPERTY(L"Kits.TestTask", L"AddPerfDx")
			TEST_CLASS_PROPERTY(L"Kits.TestTask.AddPerfDx.Stage", L"Setup")
			TEST_CLASS_PROPERTY(L"Kits.TestTask.AddPerfDx.CommandLine", L"Cmd /C powershell.exe -ExecutionPolicy Bypass -file [WTTRunWorkingDir]\\addremove_perfdx.ps1 -add")
			TEST_CLASS_PROPERTY(L"Kits.TestTask.AddPerfDx.RunElevated", L"True")
			TEST_CLASS_PROPERTY(L"Kits.TestTask.AddPerfDx.Order", L"1")
			TEST_CLASS_PROPERTY(L"Kits.TestTask.AddPerfDx.ExpectedExitCode", L"0")

			// TestTask[Cleanup] 1: Copy vanilla perfcore.ini back to original location
			TEST_CLASS_PROPERTY(L"Kits.TestTask", L"RemovePerfDx")
			TEST_CLASS_PROPERTY(L"Kits.TestTask.RemovePerfDx.Stage", L"Cleanup")
			TEST_CLASS_PROPERTY(L"Kits.TestTask.RemovePerfDx.CommandLine", L"Cmd /C powershell.exe -ExecutionPolicy Bypass -file [WTTRunWorkingDir]\\addremove_perfdx.ps1 -remove")
			TEST_CLASS_PROPERTY(L"Kits.TestTask.RemovePerfDx.RunElevated", L"True")
			TEST_CLASS_PROPERTY(L"Kits.TestTask.RemovePerfDx.Order", L"1")
			TEST_CLASS_PROPERTY(L"Kits.TestTask.RemovePerfDx.ExpectedExitCode", L"0")
        END_TEST_CLASS()

        // Add module-level setup/cleanup methods
        TEST_CLASS_SETUP(ClassSetup)
        {
            Log::Comment(String().Format(L"%S Node Meta Data Setup", __FUNCTION__));
            return true;
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
            Log::Comment(String().Format(L"%S Node Meta Data Cleanup", __FUNCTION__));
            return true;
        }

        BEGIN_TEST_METHOD(ValidateEngineTypes)
            //For HLK
            _TMP(L"Kits.TestName", L"Validate Engine Meta Data Basic Support")
            _TMP(L"Kits.TestId", L"ff458052-1946-47c4-959a-74a181ae4f09")
            _TMP(L"Kits.TestId2", L"0eab4be6-d2f9-4ab3-9312-26b9e8e4af87")
            _TMP(L"Kits.ExpectedRuntime", L"2")
            //Description
            _TMP(L"Kits.Description", L"Test will check for adpater engine meta data using ETW events")
            _TMP(L"Kits.IsInProc", L"true")
            _TMP(L"Kits.TimeoutInMinutes", L"4")
            //Job Constraints:
            //_TMP(L"Kits.Logo.Dependency", L"")
            //Job Files:
        END_TEST_METHOD();

        BEGIN_TEST_METHOD(ValidateEngineType3D)
            //For HLK
            _TMP(L"Kits.TestName", L"Validate 3D content runs on 3D Engine")
            _TMP(L"Kits.TestId", L"ff468052-1946-47c4-959a-74a191ae4f09")
            _TMP(L"Kits.TestId2", L"09c5ed78-26e3-4228-81ad-9500dabad4d2")
            _TMP(L"Kits.ExpectedRuntime", L"2")
            //Description
            _TMP(L"Kits.Description", L"Test will check dma packet usage is in line with engine meta data description using ETW events")
            _TMP(L"Kits.IsInProc", L"true")
            _TMP(L"Kits.TimeoutInMinutes", L"4")
            //Job Constraints:
            //_TMP(L"Kits.Logo.Dependency", L"")
        //Job Files:
            TEST_METHOD_PROPERTY(L"Kits.DeploymentItem", L"NTTEST\\windowstest\\graphics\\wddm\\bin\\SchBillboard.exe")
        END_TEST_METHOD();

        BEGIN_TEST_METHOD(ValidateEngineTypeDecodeEngine)
            //For HLK
            _TMP(L"Kits.TestName", L"Validate Decode content runs on Decode Engine")
            _TMP(L"Kits.TestId", L"ff478052-1946-47c4-959a-74a1a1ae4f09")
            _TMP(L"Kits.TestId2", L"e0197583-1999-45d3-871c-1c27aea4d1b7")
            _TMP(L"Kits.ExpectedRuntime", L"2")
            //Description
            _TMP(L"Kits.Description", L"Test will check dma packet usage is in line with engine meta data description using ETW events")
            _TMP(L"Kits.IsInProc", L"true")
            _TMP(L"Kits.TimeoutInMinutes", L"4")
            //Job Constraints:
            //_TMP(L"Kits.Logo.Dependency", L"")
            //Job Files:
            _TMP(L"Kits.DeploymentItem", L"NTTEST\\windowstest\\graphics\\d3d\\scenario\\DXVP.exe")
            _TMP(L"Kits.DeploymentItem", L"test\\wpd\\tests\\mtptest\\content\\TestFile.wmv")
        END_TEST_METHOD();

        BEGIN_TEST_METHOD(ValidateEngineTypeVideoProcessing)
            //For HLK
            _TMP(L"Kits.TestName", L"Validate Video Processing Content runs on Video Processing Engine")
            _TMP(L"Kits.TestId", L"ff488052-1946-47c4-959a-74a1b1ae4f09")
            _TMP(L"Kits.TestId2", L"2fdbe22a-345a-4777-993f-f2b4887d92fa")
            _TMP(L"Kits.ExpectedRuntime", L"2")
            //Description
            _TMP(L"Kits.Description", L"Test will check dma packet usage is in line with engine meta data description using ETW events")
            _TMP(L"Kits.IsInProc", L"true")
            _TMP(L"Kits.TimeoutInMinutes", L"4")
            //Job Constraints:
            //_TMP(L"Kits.Logo.Dependency", L"")
            //Job Files:
            _TMP(L"Kits.DeploymentItem", L"NTTEST\\windowstest\\graphics\\d3d\\scenario\\DXVP.exe")
        END_TEST_METHOD();
        // N/A yet
        // TEST_METHOD(ValidateEngineTypeEncodeEngine)
        // N/A yet
        //TEST_METHOD(ValidateEngineTypeSceneAssembly)
        BEGIN_TEST_METHOD(ValidateEngineTypeCopy)
          //For HLK
            _TMP(L"Kits.TestName", L"Validate Copy content runs on Copy Engine")
            _TMP(L"Kits.TestId", L"ff498052-1946-47c4-959a-74a1c1ae4f09")
            _TMP(L"Kits.TestId2", L"dd6ea8bf-138a-47a2-bd49-4ee15dc95a3d")
            _TMP(L"Kits.ExpectedRuntime", L"2")
            //Description
            _TMP(L"Kits.Description", L"Test will check dma packet usage is in line with engine meta data description using ETW events")
            _TMP(L"Kits.IsInProc", L"false")
            _TMP(L"Kits.TimeoutInMinutes", L"4")
            //Job Constraints:
            //_TMP(L"Kits.Logo.Dependency", L"")
            //Job Files:
            _TMP(L"Kits.DeploymentItem", L"NTTEST\\windowstest\\graphics\\d3d\\scenario\\DXVP.exe")
        END_TEST_METHOD();

        // N/A yet
        //TEST_METHOD(ValidateEngineTypeOverlay)
    private:

        void GetBufferSizes(XperfHelper* _pHelper)
        {
            _pHelper->GetNTkernelSessionBuffers(&m_iNTKernelBufferSize,&m_iNTKernelMinBuffers,&m_iNTKernelMaxBuffers);
            _pHelper->GetDxkernelSessionBuffers(&m_iDxkernelBufferSize,&m_iDxkernelMinBuffers,&m_iDxkernelMaxBuffers);
            _pHelper->GetOtherSessionBuffers(&m_iOtherBufferSize,&m_iOtherMinBuffers,&m_iOtherMaxBuffers);
            _pHelper->GetAdditionalSessionBuffers(&m_iAdditionalBufferSize,&m_iAdditionalMinBuffers,&m_iAdditionalMaxBuffers);
        }

        void GrowBuffers(XperfHelper* _pHelper)
        {
            // The BufferSize values must always be a power of 2. All others will grow
            // by 1/2.
            m_iNTKernelBufferSize *= 2;
            m_iNTKernelMinBuffers += (m_iNTKernelMinBuffers/2);
            m_iNTKernelMaxBuffers += (m_iNTKernelMaxBuffers/2);
            m_iDxkernelBufferSize *= 2;
            m_iDxkernelMinBuffers += (m_iDxkernelMinBuffers/2);
            m_iDxkernelMaxBuffers += (m_iDxkernelMaxBuffers/2);
            m_iOtherBufferSize *= 2;
            m_iOtherMinBuffers += (m_iOtherMinBuffers/2);
            m_iOtherMaxBuffers += (m_iOtherMaxBuffers/2);
            m_iAdditionalBufferSize *= 2;
            m_iAdditionalMinBuffers += (m_iAdditionalMinBuffers/2);
            m_iAdditionalMaxBuffers += (m_iAdditionalMaxBuffers/2);

            _pHelper->SetNTkernelSessionBuffers(m_iNTKernelBufferSize,m_iNTKernelMinBuffers,m_iNTKernelMaxBuffers);
            _pHelper->SetDxkernelSessionBuffers(m_iDxkernelBufferSize,m_iDxkernelMinBuffers,m_iDxkernelMaxBuffers);
            _pHelper->SetOtherSessionBuffers(m_iOtherBufferSize,m_iOtherMinBuffers,m_iOtherMaxBuffers);
            _pHelper->SetAdditionalSessionBuffers(m_iAdditionalBufferSize,m_iAdditionalMinBuffers,m_iAdditionalMaxBuffers);
        }

        //
        // Note that this routine will log ETW events with the EventWrite*() routines. In 
        // order to get GPUView to decode them in your trace, you'll need to install the manifest
        // like "wevtutil in etwLibEvents.man" and have the resource dll in the same location
        // from where GPUView is run. That file is etwLibEventsResource.dll. These two files
        // are built from .\testsrc\windowstest\common\etw.
        //
        void Run()
        {
            CProvider etwProvider;
            bool bError=false;
            m_bMSBDA = false;
            HRESULT hr = S_OK;
            XperfHelper* pHelper = NULL;

            SchedPerfCommon::SchCmdLine CmdLine;
            VERIFY_IS_TRUE( CmdLine.Valid() );
            m_pCmdLine = &CmdLine;

            // Now we're going to loop until we are able to successfully get an ETL
            // file that doesn't have dropped events.
#define NUMBER_OF_TRIES 5
            if( !m_pCmdLine->PostProcessETLOnly() )
            {
                pHelper = new XperfHelper(m_pCmdLine->Level(),m_pCmdLine->Echo(),m_pCmdLine->Buffers());
                if (pHelper)
                {
                    GetBufferSizes(pHelper);
                }

                // This will be used any time we call LaunchProcess to run a command line
                m_CMDLinelen = sizeof(m_wszCMDLine)/sizeof(m_wszCMDLine[0]);

                for(int iLoop=1;iLoop<NUMBER_OF_TRIES;iLoop++)
                {
                    // Now set the resulting etl file to our command line
                    pHelper->SetMergedFile(m_pCmdLine->ETLFile());

                    if( !m_pCmdLine->DisableEtw() )
                    {
                        VERIFY_SUCCEEDED(pHelper->Start());
                    }

                    // Use whatever string is interesting here. you'll see that string in the payload
                    // of the decoded event. The event name will be "Test-Etw LogMarker". The Start,
                    // stop and Info will be denoted in the opcode of the event.
                    EventWriteLogMarkerStart(1,L"StartTest");

                    Log::Comment(String().Format(L"%S Starting Test", __FUNCTION__));

                    switch(m_eWorkType)
                    {
                    case eWorkType_ValidateForm:
                        {
                            UINT uiTime = 2000; // seconds to run
                            m_ulSecondsToRun = 2;
                            if (m_pCmdLine->Time())
                            {
                                m_ulSecondsToRun = m_pCmdLine->Time();
                                uiTime=(m_ulSecondsToRun*1000); // Convert to milliseconds
                            }

                            Sleep(uiTime);
                        }
                        break;
                    case eWorkType_Validate3D:
                        {
                            // Lauch SchBillboard.exe for 10 seconds or the default time on the comamnd line
                            UINT uiTime = 10; // seconds to run
                            m_ulSecondsToRun = 10;
                            if (m_pCmdLine->Time())
                            {
                                m_ulSecondsToRun = m_pCmdLine->Time();
                                uiTime=(m_ulSecondsToRun);
                            }

                            // Launch schbillboard.exe time:N
                            VERIFY_SUCCEEDED(StringCchPrintf(m_wszCMDLine, m_CMDLinelen, 
                                                             L"schbillboard.exe time:%d",
                                                             uiTime));

                            VERIFY_SUCCEEDED(LaunchProcess(m_wszCMDLine));
                        }
                        break;
                    case eWorkType_ValidateDecodeEngine:
                        {
                            // Lauch DXVP.EXE for 20 seconds or the default time on the comamnd line
                            UINT uiTime = 20000; // milliseconds to run
                            m_ulSecondsToRun = 20;
                            if (m_pCmdLine->Time())
                            {
                                m_ulSecondsToRun = m_pCmdLine->Time();
                                uiTime=(m_ulSecondsToRun*1000); // Convert to milliseconds
                            }

                            VERIFY_SUCCEEDED(StringCchPrintf(m_wszCMDLine, m_CMDLinelen, L"dxvp.exe -i TestFile.wmv"));

                            // Launch dxvp.exe, but kill it on our timeout
                            VERIFY_SUCCEEDED(LaunchProcessTerminate(m_wszCMDLine,uiTime,true));
                        }
                        break;
                    case eWorkType_ValidateEncodeEngine:
                        break;
                    case eWorkType_ValidateVideoProcessing:
                        {
                            // Lauch DXVP.EXE for 20 seconds or the default time on the comamnd line
                            UINT uiTime = 20000; // milliseconds to run
                            m_ulSecondsToRun = 20;
                            if (m_pCmdLine->Time())
                            {
                                m_ulSecondsToRun = m_pCmdLine->Time();
                                uiTime=(m_ulSecondsToRun*1000); // Convert to milliseconds
                            }

                            VERIFY_SUCCEEDED(StringCchPrintf(m_wszCMDLine, m_CMDLinelen, L"dxvp.exe"));

                            // Launch dxvp.exe, but kill it on our timeout
                            VERIFY_SUCCEEDED(LaunchProcessTerminate(m_wszCMDLine,uiTime,true));
                        }
                        break;
                    case eWorkType_ValidateSceneAssembly:
                        break;
                    case eWorkType_ValidateCopy: // Same as eWorkType_ValidateVideoProcessing
                        {
                            // Lauch DXVP.EXE for 20 seconds or the default time on the comamnd line
                            UINT uiTime = 20000; // milliseconds to run
                            m_ulSecondsToRun = 20;
                            if (m_pCmdLine->Time())
                            {
                                m_ulSecondsToRun = m_pCmdLine->Time();
                                uiTime=(m_ulSecondsToRun*1000); // Convert to milliseconds
                            }

                            VERIFY_SUCCEEDED(StringCchPrintf(m_wszCMDLine, m_CMDLinelen, L"dxvp.exe"));

                            // Launch dxvp.exe, but kill it on our timeout
                            VERIFY_SUCCEEDED(LaunchProcessTerminate(m_wszCMDLine,uiTime,true));
                        }
                        break;
                    case eWorkType_ValidateOverlay:
                        break;
                    }

                    Log::Comment(String().Format(L"%S Ending Test", __FUNCTION__));

                    EventWriteLogMarkerStop(1, L"EndTest");

                    if( !m_pCmdLine->DisableEtw() )
                    {
                        hr = pHelper->Stop();
                        if( hr != S_OK )
                        {
                            if( iLoop >= NUMBER_OF_TRIES )
                            {
                                Log::Comment(String().Format(L"%S Unable to get valid ETL file after %d tries", 
                                                             __FUNCTION__,NUMBER_OF_TRIES));
                                // We've grown all we want too.
                                VERIFY_FAIL();
                            }

                            GrowBuffers(pHelper);
                            continue;
                        }
                        else
                        {
                            Log::Comment(String().Format(L"%S Successfully captured a valid ETL file after %d tries", 
                                                         __FUNCTION__,iLoop));
                        }
                    }
                    // If we get to this point, we've got an ETL file to process.
                    break;
                }
            }

        

            // Don't post process if the user only wanted to run the scenario.
            if( !m_pCmdLine->DisableEtw() )
            {
                // Now we need to create our helper class and validate the data.

                CDMAEngineHelper NodeMetaDataHelper(m_pCmdLine->ETLFile());

                // Set options on the dxetw helper.
                NodeMetaDataHelper.SetEcho(m_pCmdLine->Echo());
                NodeMetaDataHelper.SetPostProcessCSVOnly(m_pCmdLine->PostProcessCSVOnly());

                // Gather our data
                std::vector<CAdapterMetaData*>* pList;
                pList = NodeMetaDataHelper.GatherDMAEngine(m_pCmdLine->CSVFile(),
                                                              NULL); //L"-marker 1");
                if( pList )
                {
#ifdef WANT_DEBUG
                    // Output the types of engines that are available
                    DumpEngineTypes(pList);
#endif
                    switch(m_eWorkType)
                    {
                    case eWorkType_ValidateForm:
                        {
                            bError = ValidateForm(pList);
                        }
                        break;
                    case eWorkType_Validate3D:
                    case eWorkType_ValidateDecodeEngine:
                    case eWorkType_ValidateVideoProcessing:
                    case eWorkType_ValidateCopy:
                        {
                            bError = ValidateEngine(pList);
                        }
                        break;
                    case eWorkType_ValidateEncodeEngine:
                        break;
                    case eWorkType_ValidateSceneAssembly:
                        break;
                    case eWorkType_ValidateOverlay:
                        break;

                    }
                }
                else
                {
                    Log::Comment(String().Format(L"%S Data read from CSV file is valid", __FUNCTION__));
                    // We've grown all we want too.
                    VERIFY_FAIL();
                }


                VERIFY_IS_TRUE(bError==false);
            }
        }
#ifdef WANT_DEBUG
        void DumpEngineTypes(std::vector<CAdapterMetaData*>* _pList)
        {
            std::vector<CAdapterMetaData*>::iterator itrAdapter = _pList->begin();
            while(itrAdapter != _pList->end())
            {
                CAdapterMetaData* pAdapter = (*itrAdapter);
        
                std::vector<CDMAEngine*>::iterator itrNode = pAdapter->List()->begin();
                while( itrNode != pAdapter->List()->end() )
                {
                    CDMAEngine* pNodeMetaData = (*itrNode);
                    WCHAR* pName = NULL;

                    switch(pNodeMetaData->Type())
                    {
                    case DxgkEngineType_Other:
                        pName = L"DxgkEngineType_Other";
                        break;
                    case DxgkEngineType_3D:
                        pName = L"DxgkEngineType_3D";
                        break;
                    case DxgkEngineType_VideoDecode:
                        pName = L"DxgkEngineType_VideoDecode";
                        break;
                    case DxgkEngineType_VideoEncode:
                        pName = L"DxgkEngineType_VideoEncode";
                        break;
                    case DxgkEngineType_VideoProcessing:
                        pName = L"DxgkEngineType_VideoProcessing";
                        break;
                    case DxgkEngineType_SceneAssembly:
                        pName = L"DxgkEngineType_SceneAssembly";
                        break;
                    case DxgkEngineType_Copy:
                        pName = L"DxgkEngineType_Copy";
                        break;
                    case DxgkEngineType_Overlay:
                        pName = L"DxgkEngineType_Overlay";
                        break;
                    }

                    wprintf_s(L"Index %d is type %s\n", 
                              pNodeMetaData->Index(),
                              pName);

                    itrNode++;
                }
                itrAdapter++;
            }
        }
#endif
        bool ValidateEngine(std::vector<CAdapterMetaData*>* _pList)
        {
            bool bFoundEngineWithDataOnIt=false;

            // We should find work occurred on the 3d engine. We want to see that 
            // we're getting at least one DmaPacket per frame. 
            std::vector<CAdapterMetaData*>::iterator itrAdapter = _pList->begin();
            while(itrAdapter != _pList->end())
            {
                CAdapterMetaData* pAdapter = (*itrAdapter);

                // is this our active adapter? If so, walk the NodeMetaData and look for 3d engine usage
                if( pAdapter->Active() )
                {
                    std::vector<CDMAEngine*>::iterator itrNode = pAdapter->List()->begin();
                    while( itrNode != pAdapter->List()->end() )
                    {
                        CDMAEngine* pNodeMetaData = (*itrNode);

                        bool bFound=false;
                        if( eWorkType_Validate3D == m_eWorkType )
                        {
                            if( pNodeMetaData->Type() == DxgkEngineType_3D )
                            {
                                bFound = true;
                            }
                        }
                        else if( eWorkType_ValidateDecodeEngine == m_eWorkType )
                        {
                            if( ( pNodeMetaData->Type() == DxgkEngineType_3D ) || 
                                ( pNodeMetaData->Type() == DxgkEngineType_VideoDecode ) )
                            {
                                bFound = true;
                            }
                        }
                        else if( eWorkType_ValidateVideoProcessing == m_eWorkType )
                        {
                            if( ( pNodeMetaData->Type() == DxgkEngineType_3D ) || 
                                ( pNodeMetaData->Type() == DxgkEngineType_VideoProcessing ) )
                            {
                                bFound = true;
                            }
                        }
                        else if( eWorkType_ValidateCopy == m_eWorkType )
                        {
                            if( ( pNodeMetaData->Type() == DxgkEngineType_3D ) || 
                                ( pNodeMetaData->Type() == DxgkEngineType_Copy ) )
                            {
                                bFound = true;
                            }
                        }
                        else
                        {
                            PATH_TRAP(); // shouldn't have any other types.
                        }

                        // is this the 3d engine?
                        if( bFound )
                        {
                            // if we expect one packet per every other frame, we'll get
                            // (m_ulSecondsToRun * 60)/2
                            ULONG ulThresholdCount = (m_ulSecondsToRun * 60)/2;

                            // yep, do we see Standard packets on this engine that exceed our threshold?

                            if( pNodeMetaData->Standard() >= ulThresholdCount )
                            {
                                bFoundEngineWithDataOnIt=true;
                            }
                        }

                        itrNode++;
                    }
                }
        
                itrAdapter++;
            }

            if(!bFoundEngineWithDataOnIt)
            {
                PATH_TRAP();
                Log::Comment(String().Format(L"%S:%d No work found on engine during test", 
                                             __FUNCTION__,
                                             __LINE__));
            }


            return !bFoundEngineWithDataOnIt;
        }


        bool VerifyMetaData(CDMAEngine* pNodeMetaData,
                            CAdapterMetaData* pAdapter)
        {
            bool bError = false;

            // If the type is out of line, fail

            if( pNodeMetaData->Type() >= NUM_DXGK_ENGINE_TYPES )
            {
                bError=true;
                Log::Comment(String().Format(L"%S:%d Engine Type (%d) out of bounds for index %d", 
                                             __FUNCTION__,
                                             __LINE__,
                                             pNodeMetaData->Type(),
                                             pNodeMetaData->Index()));
            }

            // if they provide a friendly name, it must be less then DXGK_MAX_METADATA_NAME_LENGTH
            // characters in length

            if( pNodeMetaData->FriendlyName() )
            {
                size_t size=0;
                HRESULT hr = StringCchLength(pNodeMetaData->FriendlyName(),
                                             DXGK_MAX_METADATA_NAME_LENGTH-1,
                                             &size);
                if( S_OK != hr )
                {
                    bError=true;
                    // bad friendly name
                    Log::Comment(String().Format(L"%S:%d Friendly Name(%s) invalid for Index %d", 
                                                 __FUNCTION__,
                                                 __LINE__,
                                                 pNodeMetaData->FriendlyName(),
                                                 pNodeMetaData->Index()));
                }

                // Our first check is to make sure that the friendly names match what's expected
                // for standard types.

                Log::Comment(String().Format(L"%S:%d Info: (%I64d) Friendly Name(%s) on classification(%d) for Index %d", 
                                             __FUNCTION__,
                                             __LINE__,
                                             pAdapter->Handle(),
                                             pNodeMetaData->FriendlyName(),
                                             pNodeMetaData->Type(),
                                             pNodeMetaData->Index()));


                bool bCheck=true;
                WCHAR* pExpectedName=L"";
                WCHAR* pFriendlyName;
                switch(pNodeMetaData->Type())
                {
                case DxgkEngineType_3D:
                    {
                        pExpectedName = L"3D";

                        // Special case MSBDA
                        //
                        // The MSBDA driver will have multiple 3D engines and there will be a 
                        // friendly name associated with each one. it will be in the form of
                        // 3DNode** 

                        pFriendlyName = wcsstr(pNodeMetaData->FriendlyName(),L"3DNode");
                        if( NULL != pFriendlyName )
                        {
                            // we found MSBDA, mark this adapter as being MSBDA
                            m_bMSBDA = true;
                        }
                    }
                    break;
                case DxgkEngineType_VideoDecode:
                    pExpectedName = L"Video Decode";
                    break;
                case DxgkEngineType_VideoEncode:
                    pExpectedName = L"Video Encode";
                    break;
                case DxgkEngineType_VideoProcessing:
                    pExpectedName = L"Video Processing";
                    break;
                case DxgkEngineType_SceneAssembly:
                    pExpectedName = L"Scene Assembly";
                    break;
                case DxgkEngineType_Copy:
                    pExpectedName = L"Copy";
                    break;
                case DxgkEngineType_Overlay:
                    pExpectedName = L"Overlay";
                    break;
                default:
                    bCheck=false;
                    break;
                }

                // Do we have a standard name? if so, we'll validate that the friendly name
                // is what we expect.
                if( bCheck )
                {
                    // If it's not what we expect, we'll get NULL back
                    pFriendlyName = wcsstr(pNodeMetaData->FriendlyName(),pExpectedName);
                    if( NULL == pFriendlyName )
                    {
                        bError=true;
                        // bad friendly name
                        Log::Comment(String().Format(L"%S:%d Friendly Name(%s) on standard classification(%d) for Index %d", 
                                                     __FUNCTION__,
                                                     __LINE__,
                                                     pNodeMetaData->FriendlyName(),
                                                     pNodeMetaData->Type(),
                                                     pNodeMetaData->Index()));
                    }
                }

                // Also can only have a friendlyname if NodeMetaData exists.

                if( (size > 0) && !pNodeMetaData->Exist() )
                {
                    bError=true;
                    Log::Comment(String().Format(L"%S:%d No Exists flag with Friendly Name for Index %d", 
                                                 __FUNCTION__,
                                                 __LINE__,
                                                 pNodeMetaData->Index()));
                }
            }
            return bError;
        }

        bool ValidateForm(std::vector<CAdapterMetaData*>* _pList)
        {
            bool bError = false;
            UINT uiCountof3DEngines=0;
            // Walk each adapter in the list.....
        
            std::vector<CAdapterMetaData*>::iterator itrAdapter = _pList->begin();
            while(itrAdapter != _pList->end())
            {
                CAdapterMetaData* pAdapter = (*itrAdapter);
        
                std::vector<CDMAEngine*>::iterator itrNode = pAdapter->List()->begin();
                while( itrNode != pAdapter->List()->end() )
                {
                    CDMAEngine* pNodeMetaData = (*itrNode);
        
                    // Validate that all the elements of the NodeMetaData are within bounds.
                    bool bTmp=VerifyMetaData(pNodeMetaData,pAdapter);
                    if( bTmp )
                    {
                        bError = bTmp;
                    }
        
                    if( pNodeMetaData->Type() == DxgkEngineType_3D )
                    {
                        uiCountof3DEngines++;
                    }
        
                    itrNode++;
                }
        
                // if we don't have a 3d engine, we've got to fail
                if( pAdapter->Active() && (0 == uiCountof3DEngines)  )
                {
                    bError=true;
                    // No 3d Engine
                    Log::Comment(String().Format(L"%S:%d No 3D engine for Adapter %I64d", 
                                                 __FUNCTION__,
                                                 __LINE__,
                                                 pAdapter->Handle()));
                }

                // MSBDA can have multiple 3D engines. We're only going to police the count
                // if we're not MSBDA.

                if( !m_bMSBDA )
                {
                    if( pAdapter->Active() && (uiCountof3DEngines > 1)  )
                    {
                        bError=true;
                        // No 3d Engine
                        Log::Comment(String().Format(L"%S:%d %d 3D engines for Adapter %I64d", 
                                                     __FUNCTION__,
                                                     __LINE__,
                                                     uiCountof3DEngines,
                                                     pAdapter->Handle()));
                    }
                }
        
                // Walk the NodeMetaData a second time and validate that if the adapter
                // is active, all the nodes associated with this adapter have data
                itrNode = pAdapter->List()->begin();
                while( itrNode != pAdapter->List()->end() )
                {
                    CDMAEngine* pNodeMetaData = (*itrNode);
        
                    if( pAdapter->Active() )
                    {
                        if( !pNodeMetaData->Exist() )
                        {
                            bError=true;
                            Log::Comment(String().Format(L"%S:%d No Meta Data Exists for engine Index %d", 
                                                         __FUNCTION__,
                                                         __LINE__,
                                                         pNodeMetaData->Index()));
                        }
                    }
                    itrNode++;
                }
        
                itrAdapter++;
            }
            return bError;
        }



        //
        // Private variables for this class.
        //
        SchedPerfCommon::SchCmdLine* m_pCmdLine;

        eWorkType m_eWorkType;

        WCHAR m_wszCMDLine[2000];
        size_t m_CMDLinelen;

        ULONG m_ulSecondsToRun;
        bool m_bMSBDA;

        // Used for growing our buffer sizes if we run into a problem on the machine.
        int m_iNTKernelBufferSize;
        int m_iNTKernelMinBuffers;
        int m_iNTKernelMaxBuffers;
        int m_iDxkernelBufferSize;
        int m_iDxkernelMinBuffers;
        int m_iDxkernelMaxBuffers;
        int m_iOtherBufferSize;
        int m_iOtherMinBuffers;
        int m_iOtherMaxBuffers;
        int m_iAdditionalBufferSize;
        int m_iAdditionalMinBuffers;
        int m_iAdditionalMaxBuffers;
    };

    void NodeMetaData::ValidateEngineTypes()
    {
        m_eWorkType = eWorkType_ValidateForm;
        Run();
    }

    void NodeMetaData::ValidateEngineType3D()
    {
        m_eWorkType = eWorkType_Validate3D;
        Run();
    }

    void NodeMetaData::ValidateEngineTypeDecodeEngine()
    {
        m_eWorkType = eWorkType_ValidateDecodeEngine;
        Run();
    }

    void NodeMetaData::ValidateEngineTypeVideoProcessing()
    {
        m_eWorkType = eWorkType_ValidateVideoProcessing;
        Run();
    }

    void NodeMetaData::ValidateEngineTypeCopy()
    {
        m_eWorkType = eWorkType_ValidateCopy;
        Run();
    }



}  /* namespace WEX */
