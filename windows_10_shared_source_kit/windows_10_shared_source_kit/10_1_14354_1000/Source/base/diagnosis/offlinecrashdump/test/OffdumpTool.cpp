/*++

    Copyright (C) Microsoft. All rights reserved.

Module Name:
   offdumptool.cpp

Environment:
   User Mode

Author:
   Rajib Dutta (radutta )
--*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <winioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strsafe.h>
#include <wchar.h>
#include "common.h"
#include "DiskUtil.h"
#include "DumpUtil.h"
#include "apreg64.h"


VOID
Usage()
{
    wprintf(
            L"offlinedump test tool.\n"
            L"Background and Assumptions:\n"
            L"    This utility:\n"
            L"          - Takes input that conforms to the Microsoft Offline Dump Specification\n"
            L"          - Must be run as an administrator\n"
            L"          - Denotes System under Test (SUT) as the System that crashed\n"
            L"          - Denotes Debug System (DS) as system running the Windows debugger\n"
            L"          \n"



            L"    The System Under Test (SUT):\n"
            L"          - Has a disk partition suitable for storing an offline crash\n"
            L"          - May be distinct from the Debug System (DS)\n"
            L"          - May be a different system architecture from the Debug System\n"
            L"          \n"
            L"    To expedite debugging, two distinct systems are recommended\n"
            L"    This is based on assumption, relative to SUT:\n"
            L"          - The Debug System has greater stability\n"
            L"          - The Debug System has higher performance\n"
            L"          \n"


            L"    The file extension has the following conventions:\n"
            L"          .RAW (Raw Partition saved as a File)\n"
            L"               [Output or Input to Tool]\n"
            L"          .DMP (Windows Dump File loadable by Windows Debugger)\n"
            L"               [Output from Tool]\n"
            L"               Windows Phone: [Input to Tool]\n"
            L"          .BIN (Silicon Vendor Specific BIN File)\n"
            L"               [Output from Tool]\n"
            L"          .DDR (DDR memory sections File)\n"
            L"               [Input to Tool]\n"
            L"          \n"
            L"          \n"

            L"BASIC Usage Commands:\n"
            L"     /help\n"
            L"          This help text\n"
            L"          \n"

            L"     /parsepart\n"
            L"          This will Parse the Raw_Dump Partition and create:\n"
            L"              .DMP = Windows Dump file \n"
            L"              .BIN = Silicon vendor specific bins\n"
            L"          Example: offlinedumptool /parsepart\n"
            L"          Note: Must run command on System under Test (SUT)\n"
            L"          Output file: raw_dump.DMP, <possibly others>\n"
            L"          Exact output filenames encoded in headers of Offline Specification\n"
            L"          \n"

            L"     /dumprawpart <DUMPFILE.RAW>\n"
            L"          This will dump the raw dump partition to a file. \n"
            L"          Example: offlinedumptool /dumprawpart dumpfile.raw\n"
            L"          Note: Must run command on System under Test (SUT)\n"
            L"          Output .RAW file is further processed using the /parsedump option\n"
            L"          \n"

            L"     /parsedump <DUMPFILE.RAW> \n"
            L"          This will open the Raw_Dump Partition file as raw dump and parse the headers. \n"
            L"          Example: offlinedumptool /parsedump DUMPFILE.RAW \n"
            L"          Note: .BIN file can be processed using this utility on Debug System\n"
            L"          Exact output filenames encoded in headers of Offline Specification\n"
            L"          Output file: raw_dump.DMP\n"
            L"          option /noapreg will not attempt to process APREG section\n"
            L"          \n"

            L"     /parsewpdump <WP_DUMPFILE.DMP>\n"
            L"          It will extract WP silicon vendor sections from WP dump secondary data and save as .bins \n"
            L"          Example: offlinedumptool /parsewpdump memory.dmp\n"
            L"          Note: Windows Phone Specific Option, all other options are supported exclusively for Windows\n"
            L"                Must run command on .DMP file from Windows Phone System under Test (SUT)\n"
            L"                The input .DMP file is generated from another Windows Phone tool\n"
            L"          Output file(s): <section>.BIN ...\n"
            L"          Exact output filenames encoded in headers of Offline Specification\n"
            L"          \n"
            
            L"     OPTIONAL ADD ON COMMANDS :-\n"
            L"     /noapreg \n"
            L"          Does not attempt to find APREG in the DDR sections\n"
            L"          \n"
            L"     /apreg64 <address in Hex> \n"
            L"          Assumes the APREG format to of 64 bit and attempts to find the MSM_DUMP_TABLE \n"
            L"          at that location.\n"
            L"          \n"


            L"RECOMMENDED Usage Commands:\n"
            L"          For example on Windows Systems Under Test (SUT), option A or B:\n"
            L"          A)  Raw Partition -- > Raw File -- > .DMP, .BIN(s)\n"
            L"              -----------------------------------------------------------\n"
            L"              /dumprawpart (on SUT)            /parsedump, windbg (on DS)\n"
            L"          \n"
            L"              1) 'offlinedumptool /dumprawpart dump.raw' (on SUT)\n"
            L"                 output is user specified 'dump.raw' file\n"
            L"              2) 'offlinedumptool /parsedump dump.raw' (on DS)\n"
            L"                 output is .DMP file (windbg loadable)\n"
            L"                 output is .BIN file(s) (as specified in .RAW file)\n"
            L"              3) 'windbg -z crash.dmp' (on DS)\n"

            L"          \n"
            L"          B)  Raw Partition       ----- >      .DMP, .BIN(s)\n"
            L"              --------------------------------------------------\n"
            L"              /parsepart (on SUT)              windbg (on DS)\n"
            L"          \n"
            L"              1) 'offlinedumptool /parsepart' (on SUT)\n"
            L"                 output is .DMP file (windbg loadable)\n"
            L"                 output is .BIN file(s) (as specified in .RAW file)\n"
            L"              2) 'windbg -z crash.dmp' (on DS)\n"

            L"          \n"


            L"          For Windows Phone Systems Under Test (SUT):\n"
            L"          A)  .DMP File (generated on SUT)      -- > .BIN(s)\n"
            L"              -----------------------------------------------\n"
            L"              /parsewpdump wp_dumpfile.dmp, windbg (on DS)\n"
            L"          \n"
            L"              1) 'offlinedumptool /parsewpdump wp_dumpfile.DMP' (on DS)\n"
            L"                 output is .BIN file(s) (as specified in .DMP file)\n"
            L"              2) 'windbg -z wp_dumpfile.dmp' (on DS)\n"
            L"          \n"


    );
}



DWORD GetFileVersionString (
    __in LPTSTR pFileName, 
    __in LPTSTR StringBuffer,
    __in UINT   BufferLengthInCharacters
    )
{
    BOOL   Status            = TRUE;
    DWORD  RetVal            = 0;
    DWORD  Size              = 0;
    DWORD  Handle            = 0;
    PVOID  pData             = NULL;
    PDWORD pTranslation      = NULL;
    PVOID  pRequestedData    = NULL;
    UINT   RequestedDataSize = 0;
    DWORD  DefaultLang       = 0x409;

    #define GfvsKeyLength 80
    
    WCHAR Key[GfvsKeyLength];

    RtlZeroMemory (Key, sizeof(WCHAR) * GfvsKeyLength);

    //
    // Get the size of the File Version Info
    //
    Size = GetFileVersionInfoSize(pFileName, &Handle);

    if (0 == Size)
    {
        RetVal = GetLastError();
        return RetVal;
    }

    //
    // Allocate memory for the File Version Info
    //
    pData = calloc(1, Size);

    if (NULL == pData)
    {
        RetVal = ERROR_NOT_ENOUGH_MEMORY;  
        goto Exit;
    }        

    //
    // Get the File Version Info
    //
    Status = GetFileVersionInfo (
        pFileName,
        Handle,
        Size,
        pData
        );

    if (!Status)
    {
        RetVal = GetLastError();
        goto Exit;
    }

    //
    // Get language details
    //
    Status = VerQueryValue (
        pData,
        L"\\VarFileInfo\\Translation",
        (PVOID *) & pTranslation,
        & RequestedDataSize
        );

    if (!Status)
    {        
        RetVal = GetLastError();

        //
        // No VarFileInfo translation, assuming default language.
        //
        pTranslation = &DefaultLang;
        RequestedDataSize = sizeof (DWORD);

        //
        // Ignore previous error
        //        
        RetVal = 0;
    }

    //
    // Create the query string
    //
    RetVal = StringCchPrintf (
        Key,
        GfvsKeyLength,
        L"\\StringFileInfo\\%04x%04x\\FileVersion",
        LOWORD(*pTranslation), 
        HIWORD(*pTranslation)
        );

    if (0 != RetVal)
    {
        goto Exit;
    }

    //
    // Query the FileVersion
    //
    Status = VerQueryValue(
        pData,
        Key,
        & pRequestedData,
        & RequestedDataSize
        );

    if (!Status)
    {
        RetVal = GetLastError();
        goto Exit;
    }        

    //
    // Check to see that we have enough room
    //
    if (RequestedDataSize > sizeof(WCHAR) * BufferLengthInCharacters)
    {
        RetVal = ERROR_BUFFER_OVERFLOW;
        goto Exit;
    }

    //
    // Copy the requested data
    //
    RetVal = StringCchPrintf (
        StringBuffer, 
        BufferLengthInCharacters, 
        (LPWSTR) pRequestedData
        );

Exit:
    if (pData)
    {
        free (pData);
    }
    return RetVal;
}

HRESULT
ParseCommandLine(
    __in_ecount(argc) WCHAR const * const argv[],
    DWORD argc,
    __inout PCOMMAND_LINE_ARGS arguments
    )
{
    HRESULT result = ERROR_SUCCESS;
    DWORD i;

    for(i=1; i<argc; ++i) {
        if(argv[i][0]=='/' || argv[i][0]=='-') {
            const WCHAR *arg = &argv[i][1];
            if(_wcsicmp(arg, L"?") == 0 || _wcsicmp(arg, L"help") == 0 ) {
                arguments->DisplayUsage = TRUE;
            }
            else if(_wcsicmp(arg, L"parsedump") == 0) {
                if(i + 1 < argc){
                     arguments->ParseDump = TRUE;
                     arguments->FileName = (PWSTR )&argv[i+1][0];
                     i++;
                }
            }
            else if (_wcsicmp(arg, L"noapreg") == 0) {
                arguments->DoNotParseAPREG = TRUE;
            }
            else if (_wcsicmp(arg, L"device") == 0) {                
                if(wcscmp(argv[i+1], L"8994") == 0) {
                    arguments->sharedIMEM = TRUE;
                }
                else if(wcscmp(argv[i+1], L"8916") == 0) {
                    arguments->sharedIMEM = FALSE;
                }
                
            }
            else if (_wcsicmp(arg, L"apreg64") == 0)  {
                arguments->apreg64 = TRUE;
                 if(i + 1 < argc){
                     arguments->apreg64TableAddr.QuadPart = _wcstoui64(argv[i+1], NULL, 16);                 
                     i++;
                }                
            }
            else if(_wcsicmp(arg, L"parsepart") == 0) {
                arguments->PasePartition = TRUE;
                arguments->DumpDDR = TRUE;
            }
            else if(_wcsicmp(arg, L"dumpddr") == 0) {
                arguments->DumpDDR = TRUE;
            }
            else if(_wcsicmp(arg, L"checkdebug") == 0) {
                arguments->CheckDebugPolicy = TRUE;
            }
            else if(_wcsicmp(arg, L"checkevent") == 0) {
                arguments->CheckOfflineDumpEvent = TRUE;
            }
            else if(_wcsicmp(arg, L"checkeventUI") == 0) {
                arguments->CheckOfflineDumpEventUI = TRUE;
            } 
            else if(_wcsicmp(arg, L"checkdump") == 0) {
                arguments->CheckValidDump = TRUE;
            }
            else if(_wcsicmp(arg, L"wipedump") == 0) {
                arguments->WipeRawDumpHeader = TRUE;
            }  
            else if(_wcsicmp(arg, L"createpart") == 0) {
                arguments->CreatePart = TRUE;
            }
            else if(_wcsicmp(arg, L"dumprawpart") == 0) {
                if(i + 1 < argc){
                     arguments->DumpRawPartition = TRUE;
                     arguments->FileName = (PWSTR )&argv[i+1][0];
                     i++;
                }
            }
            else if(_wcsicmp(arg, L"ParseWPdump") == 0) {
                if(i + 1 < argc){
                     arguments->ParseWPdump = TRUE;
                     arguments->FileName = (PWSTR )&argv[i+1][0];
                     i++;
                }
            }  
            else if(_wcsicmp(arg, L"ddr2dmp") == 0) {
                arguments->GenDMPFormDDR = TRUE;
            } 
            else if (_wcsicmp(arg, L"ddr") == 0) {
                if (i + 2 < argc) {
                    arguments->ddr[arguments->DDRCount].DDRBase.QuadPart = _wcstoui64(argv[i + 1], NULL, 16);
                    arguments->ddr[arguments->DDRCount].DDRFileName = (PWSTR)&argv[i + 2][0];
                    arguments->ddr[arguments->DDRCount].AlreadyMerged = FALSE;
                    arguments->DDRCount++;
                    i += 2;
                }
            }
            else {
                wprintf(L"Unexpected command line argument '%s'.\n", argv[i]);
                result = E_INVALIDARG;
                goto Exit;
                }
            }
    }

Exit:

    return result;
}

VOID
DumpGUID(
    _In_ GUID*  Guid
    )
{
        LogLibInfoPrintf(L"\t\t{%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X}\r\n",
                Guid->Data1, Guid->Data2, Guid->Data3,
                Guid->Data4[0], Guid->Data4[1], Guid->Data4[2],
                Guid->Data4[3], Guid->Data4[4], Guid->Data4[5],
                Guid->Data4[6], Guid->Data4[7]);
}

int __cdecl
wmain (
    DWORD argc,
    __in_ecount(argc) WCHAR const * const argv[]
    )
{
    HANDLE hVol = NULL;
    LPWSTR logName = L"OfflineDumpTool.wtl";
    HRESULT result = ERROR_SUCCESS;
    DMP_CONTEXT context;
    NTSTATUS status = STATUS_SUCCESS;
    LPWSTR mergedfile = L"merged.bin";
    LARGE_INTEGER filesize;
    WCHAR ProgramFileVersion [MAX_PATH];
    COMMAND_LINE_ARGS CommandLineArgs = {0};
    DWORD FileVersion = 0;
    filesize.QuadPart = 0;

    //
    // Init the Context 
    //
    context.RawDumpHeader = NULL;
    context.DDRMemoryMap = NULL;
    context.DumpDDR = FALSE;
    context.DumpHeader = NULL;
    context.ApReg = NULL;
    context.HasValidAP_REG = TRUE;
    context.IsAPREGRequested = TRUE;
    context.isAPREG64 = FALSE;
    context.KdDebuggerDataBlock = NULL;
    context.hDisk = INVALID_HANDLE_VALUE;
    context.diskoffset.QuadPart = 0;
    context.RawDumpPartitionLength .QuadPart = 0 ;
    context.DedicatedDumpHandle = INVALID_HANDLE_VALUE;


    LogLibCreateLog(logName);
    
#if 0
    LogLibInfoPrintf(L"\n====PRIVATE BUILD. PLEASE DO NOT DISTRIBUTE====\n");
#endif
    
    FileVersion = GetFileVersionString(
                    NAME_EXECUTABLE,
                    ProgramFileVersion,
                    MAX_PATH
                    );
    if( FileVersion == 0)
    {
        LogLibInfoPrintf(L"\n====Offline Dump Tool Version = %s====\n", ProgramFileVersion);
    }
    else
    {
        LogLibInfoPrintf(L"Version unavailable");
    }

    LogLibLogMachineInfo();
   
    // Start the COM interface.
    //
    result = CoInitializeEx( NULL, COINIT_MULTITHREADED );
    result = CoInitializeSecurity(NULL,
                              -1,
                              NULL,
                              NULL,
                              RPC_C_AUTHN_LEVEL_CONNECT,
                              RPC_C_IMP_LEVEL_IMPERSONATE,
                              NULL,
                              0,
                              NULL);

    if (FAILED(result))  {
        LogLibInfoPrintf(L"Failed to coinitialize");
    }
    
    result = ParseCommandLine(argv, argc, &CommandLineArgs);
    if(result != ERROR_SUCCESS) {
        goto Exit;
    }    

    if(CommandLineArgs.DisplayUsage == TRUE || argc == 1 ) {
        Usage();
        goto Exit;
    }
    
    if (CommandLineArgs.sharedIMEM == TRUE) {
        LogLibStartTest(L"Shared IMEM is different than IMEM BASE\n");
        g_Shared_IMEM = TRUE;
    }
    
    if (CommandLineArgs.apreg64 == TRUE) {
        LogLibStartTest(L"64 bit APREG format is chosen\n");
        context.isAPREG64 = TRUE;
        context.APRegAddress.QuadPart = CommandLineArgs.apreg64TableAddr.QuadPart;
        wprintf(L"context.APRegAddress.QuadPart: 0x%I64x\n", context.APRegAddress.QuadPart);       
    }
    
    if( CommandLineArgs.DoNotParseAPREG == TRUE) {
        LogLibStartTest(L"APREG update is not requested \n");
        context.IsAPREGRequested = FALSE;
        context.isAPREG64 = FALSE;            
    }  
    
    if (CommandLineArgs.CheckDebugPolicy == TRUE) {
          LogLibStartTest(L"Checking if the Device Debug policy is enabled or not\n");
          if( CheckDebugPolicyEnabled())  {
               LogLibInfoPrintf(L"The machine debug policy is enabled");
          }
          else {
               LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__, 
                L"Error: Debug Policy is not enabled. This device cannot use the offline ram dump feature");
          }
          LogLibEndTest(L"Checking if the Device Debug policy is enabled or not\n");
    }
    if (CommandLineArgs.CheckOfflineDumpEvent == TRUE) {
          LogLibStartTest(L"Check Abnormal Reset Event\n");
          if( QueryAbnromalResetEvents())  {
               LogLibInfoPrintf(L"there is an abnormal reset event");
          }
          else {
               LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__, 
                L"Error: not Found the Abnormal Reset Event");
          }
          LogLibEndTest(L"Check Abnormal Reset Event\n");
    }
    else if (CommandLineArgs.CheckOfflineDumpEventUI == TRUE) {
          if( QueryAbnromalResetEvents())  {
               MessageBox(NULL, 
                          L"Oh! Your machine just hit a hard hang!\r\n Please run offline dump collection script on the desk to collect the dump then send a mail to platform support alias!",
                          NULL,
                          MB_OK);
            }
    }
    else if (CommandLineArgs.CheckValidDump == TRUE) {
          if( ExistValidOfflineDump(&context))  {
               MessageBox(NULL, 
                          L"Oh! Your machine just hit a hard hang!\r\n Please run offline dump collection script on the desk to collect the dump then send a mail to platform support alias!",
                          NULL,
                          MB_OK);
            }
    }
    else if (CommandLineArgs.WipeRawDumpHeader == TRUE) {
          if(!WipeRawDumpHeader(&context))  {
              result = 1; 
            }
    }
    else if( CommandLineArgs.ParseDump == TRUE) {        
        LogLibStartTest(L"Read and Parse offline Dump from File\n");
        if(CommandLineArgs.DumpDDR == TRUE) {
            context.DumpDDR = TRUE;
            LogLibInfoPrintf(L"Will Dump DDR sections to files");
         }
        LogLibInfoPrintf(L"Read from File.\n");
        status = ExtractRawDumpFileToFiles(&context, CommandLineArgs.FileName);
        if (!NT_SUCCESS(status)) {
            LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__, 
                L"Error: ReadDumpFromFile Failed : %d\n",
                GetLastError());
        }
        LogLibEndTest(L"Read and Parse offline Dump from File\n");
    }
     else if( CommandLineArgs.PasePartition == TRUE) {
         LogLibStartTest(L"Extract Raw_Dump Partition To Windows Dump and Silicon Bin Files\n");
         if(CommandLineArgs.DumpDDR == TRUE) {
            context.DumpDDR = TRUE;
            LogLibInfoPrintf(L"Will Dump DDR sections to files");
         }
         status = ExtractRawDumpPartitionToFiles(&context);
          if(!NT_SUCCESS(status)) {
            result = E_FAIL;
            LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__, 
                L"Error: ExtractRawDumpPartitionToFiles Failed : %d\n",
                GetLastError());
            }
          LogLibEndTest(L"Extract Raw_Dump Partition To Windows Dump and Silicon Bin Files\n");
    } 
     else if( CommandLineArgs.CreatePart == TRUE) {
        LogLibStartTest(L"Create Raw_Dump Partition\n");
        hVol = CreateGPTRawDumpPartition();
        if(INVALID_HANDLE_VALUE == hVol) {
            result = E_FAIL;
            LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__, 
                L"Error: Create Raw Dump Parition Failed : %d\n",
                GetLastError());
        }
        LogLibEndTest(L"Create Raw_Dump Partition\n");
    }
    else if( CommandLineArgs.ParseWPdump == TRUE) {
        LogLibStartTest(L"Parse Windows Phone Dump File to Extract SV bins \n");
        result = ExtractSecondaryDataFromDumpFile(CommandLineArgs.FileName);
        if(FAILED(result)) {
            result = E_FAIL;
            LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__, 
                L"Error: Parse Windows Phone Dump File and Extract SV bins  Failed : %d\n",
                GetLastError());
        }
        LogLibEndTest(L"Parse Windows Phone Dump File to Extract SV bins \n");

        LogLibStartTest(L"Parse Windows Phone Dump File to reconstruct DDR bins \n");
        result = ReconstructDDRSectionsFromDumpFile(CommandLineArgs.FileName);
        if(FAILED(result)) {
            result = E_FAIL;
            LogLibErrorPrintf(
                result,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__, 
                L"Error: Parse Windows Phone Dump File and reconstruct DDR bins  Failed : %d\n",
                GetLastError());
        }
        LogLibEndTest(L"Parse Windows Phone Dump File to reconstruct DDR bins bins \n");
    }
    else if( CommandLineArgs.GenDMPFormDDR == TRUE) {
        LogLibStartTest(L"Try Carve Windows Dump File from DDR Sections \n");     
        if(!MergeDDRFiles(&context, &CommandLineArgs, mergedfile)) {
            LogLibErrorPrintf(
                E_FAIL,
                __LINE__,
                WIDEN(__FUNCTION__),
                __WFILE__, 
                L"Error:  Failed to merged Files\n");
       } 
       else
       {
           // now need process thie dump file now.
            result = ExtractWindowsDumpFromDDR(&context, mergedfile);
            if(FAILED(result)) {
                result = E_FAIL;
                LogLibErrorPrintf(
                    result,
                    __LINE__,
                    WIDEN(__FUNCTION__),
                    __WFILE__, 
                    L"Error:  Carving the Windows Dump file from the Merged DDR Sections  Failed : %d\n",
                    GetLastError());
            }
       }
       
       LogLibEndTest(L"Try Carve Windows Dump File from DDR Sections \n");
    }

    else if( CommandLineArgs.DumpRawPartition == TRUE) {
        LogLibStartTest(L"Directly extracting Raw_Dump Partition to a single file\n");
        status = FindDumpPartition(&context);
        if (status != ERROR_SUCCESS){
            // The script that wraps this tool expects the tool to return postive value for failure.
            result = 1;    
            LogLibErrorPrintf(
                                E_FAIL,
                                __LINE__,
                                WIDEN(__FUNCTION__),
                                __WFILE__, 
                                L"Error: Locating the RAW_Dump Partition  failed : %d\n",
                                GetLastError());
        }
        else {
            if(!ReadRawDumpPartitionToFile(&context,
                                    CommandLineArgs.FileName )) {
                // The script that wraps this tool expects the tool to return postive value for failure.
                result = 2;
                LogLibErrorPrintf(
                                E_FAIL,
                                __LINE__,
                                WIDEN(__FUNCTION__),
                                __WFILE__, 
                                L"Error: Extracting from the RAW_Dump partition failed : %d\n",
                                GetLastError());
            }
         }
         LogLibEndTest(L"Directly extract Raw_Dump Partition to single file\n");
    }

Exit:
    
    CoUninitialize();

    if(context.RawDumpHeader != NULL){
        free(context.RawDumpHeader);
    }
    
    if(context.DDRMemoryMap != NULL){
        free(context.DDRMemoryMap);
    }
    
    if(context.DumpHeader != NULL){
        free(context.DumpHeader);
    }
    
    if(context.ApReg != NULL){
        free(context.ApReg);
    }   
    
    if(context.KdDebuggerDataBlock != NULL){
        free(context.KdDebuggerDataBlock);
    }  

    if (context.DedicatedDumpHandle != INVALID_HANDLE_VALUE){
        CloseHandle(context.DedicatedDumpHandle);
        context.DedicatedDumpHandle = INVALID_HANDLE_VALUE;
    }    

    LogLibCloseLog();
    
    return result;
}



