// Includes
#include <fstream>
#include <sstream>
#include "create.h"
#include <d3d9.h>

typedef std::basic_stringstream< TCHAR > tstringstream;
static const int DXVersion_10_1 = 0x1010;
static const int DXVersion_11_0 = 0x1100;

static tstring TrimWhiteSpace(const tstring& ref)
{
    tstring retVal = ref;
    retVal.erase(retVal.find_last_not_of(_T(" \t\n\r"))+1);
    retVal.erase(0, retVal.find_first_not_of(_T(" \t\n\r")));
    return retVal;
}

static tstring TrimPnpIdSubsysRev(tstring fullPnpId)
{
    size_t midPos = fullPnpId.find(_T("&SUBSYS"), 0);
    if (midPos == tstring::npos)
        return fullPnpId;
    return fullPnpId.substr(0, midPos);
}

static D3D_FEATURE_LEVEL ConvertToCommonFeatureLevel(D3D10_FEATURE_LEVEL1 featureLevel)
{
    switch (featureLevel)
    {
    default:
    case 0:
        return (D3D_FEATURE_LEVEL)0;
    case D3D10_FEATURE_LEVEL_9_1:
        return D3D_FEATURE_LEVEL_9_1;
    case D3D10_FEATURE_LEVEL_9_2:
        return D3D_FEATURE_LEVEL_9_2;
    case D3D10_FEATURE_LEVEL_9_3:
        return D3D_FEATURE_LEVEL_9_3;
    }
}

static tstring GetFeatureLevelString(D3D_FEATURE_LEVEL featureLevel)
{
    switch (featureLevel)
    {
    default:
    case 0:
        return ToString(0);
    case D3D_FEATURE_LEVEL_9_1:
        return ToString(1);
    case D3D_FEATURE_LEVEL_9_2:
        return ToString(2);
    case D3D_FEATURE_LEVEL_9_3:
        return ToString(3);
    }
}

static void WriteFeatureLevelToFile(std::ostream& outFile, D3D_FEATURE_LEVEL featureLevel)
{
    outFile << _T("featurelevel: ");
    outFile << GetFeatureLevelString(featureLevel);
    outFile << std::endl;
}

static void WriteMachineNameToFile(std::ostream& outFile, tstring machineName)
{
    outFile << _T("machine: ") << machineName << std::endl;
}

static void WriteDisplayNameToFile(std::ostream& outFile, tstring displayName)
{
    outFile << _T("display: ") << displayName << std::endl;
}

static void WriteDriverVersionToFile(std::ostream& outFile, tstring driverVersion)
{
    outFile << _T("driver: ") << driverVersion << std::endl;
}

static void WritePnPIDToFile(std::ostream& outFile, tstring pnpId)
{
    outFile << _T("pnpid: ") << pnpId << std::endl;
}

////////////////////////

CCreateTest::CCreateTest()
{
}

/////

void CCreateTest::InitTestParameters()
{
    CTestCaseParameter<UINT>* pAdapterNumParam = AddParameter<UINT>( _T( "Adapter" ), &m_AdapterNum );
    testfactor::RFactor rAdapterNum
        = AddParameterValue<UINT>(pAdapterNumParam, 0)
        ;

    SetRootTestFactor(rAdapterNum);
}

/////

TEST_RESULT CCreateTest::Setup()
{

    ZeroMemory(&m_AdapterDesc, sizeof(m_AdapterDesc));
    m_uMaxCreatedLevel[DXVersion_10_1] = (D3D_FEATURE_LEVEL)(0);
    m_uMaxCreatedLevel[DXVersion_11_0] = (D3D_FEATURE_LEVEL)(0);

    return RESULT_PASS;
}

static tstring ReadFile(tstring fileName)
{
    std::ifstream inFile( fileName.c_str(), std::ios_base::_Nocreate | std::ios::binary );

    inFile.seekg (0, std::ios::end);
    size_t fileSize = (size_t) inFile.tellg();
    inFile.seekg (0, std::ios::beg);

    char *buf = new char[fileSize];
    inFile.read( buf, fileSize );
    inFile.close();

    tstring bufStr( buf );
    delete [] buf;

    return bufStr;
}

static tstring ReadResourceFile(tstring rscName)
{
    std::string retStr = "";
    HRSRC rsc = FindResource( NULL, rscName.c_str(), RT_RCDATA );
    if (rsc)
    {
        const DWORD fileSize = SizeofResource( NULL, rsc );
        HGLOBAL glob = LoadResource( NULL, rsc );
        if (glob)
        {
            void* pData = LockResource( glob );
            if (pData)
            {
                char *buf = new char[fileSize+1];
                memcpy( buf, pData, fileSize );
                buf[fileSize] = 0;
                retStr = tstring( buf );
                delete [] buf;
            }
        }
    }
    return retStr;
}

/////

TEST_RESULT CCreateTest::ExecuteTestCase()
{
    TEST_RESULT tRes = RESULT_PASS;
    HRESULT hr = S_OK;

    ZeroMemory(&m_UserModeDriverVersion, sizeof(m_UserModeDriverVersion));

    // create devices of each feature level
    {
        IDXGIFactory* pDXGIFactory = NULL;
        IDXGIAdapter* pDXGIAdapter = NULL;

        hr = CreateDXGIFactory(IID_IDXGIFactory, (void**)&pDXGIFactory);
        if (FAILED(hr) || !pDXGIFactory)
        {
            WriteToLog("Failed to get DXGI Factory");
            tRes = RESULT_FAIL;
            goto testDone;
        }
        else
        {
            hr = pDXGIFactory->EnumAdapters(0, &pDXGIAdapter);
            if (FAILED(hr) || !pDXGIAdapter)
            {
                WriteToLog("Failed to get DXGI Adapter");
                tRes = RESULT_FAIL;
                goto testDone;
            }
            else
            {
                hr = pDXGIAdapter->GetDesc(&m_AdapterDesc);
                if (FAILED(hr))
                {
                    WriteToLog("Failed on pDXGIAdapter->GetDesc, hr = %s", D3DHResultToString(hr).c_str() );
                    tRes = RESULT_FAIL;
                    goto testDone;
                }

                IDirect3D9Ex * pD3D9 = NULL;
                hr = Direct3DCreate9Ex( D3D_SDK_VERSION, &pD3D9 );

                if ( FAILED(hr) || pD3D9 == NULL )
                {
                    WriteToLog("Failed on Direct3DCreate9Ex" );
                    tRes = RESULT_FAIL;
                    goto testDone;
                }
                else
                {
                    D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
                    ZeroMemory( &AdapterIdentifier, sizeof( AdapterIdentifier ) );

                    hr = pD3D9->GetAdapterIdentifier( 0, 0, &AdapterIdentifier );
                    if ( FAILED(hr) )
                    {
                        WriteToLog("Failed on pD3D9->GetAdapterIdentifier" );
                        tRes = RESULT_FAIL;
                    }
                    else
                    {
                        m_UserModeDriverVersion = AdapterIdentifier.DriverVersion;
                    }

                    SAFE_RELEASE(pD3D9);

                }
            }
        }

        // D3D 10.1
        {
            D3D10_FEATURE_LEVEL1 featureLevels[] = 
            {
                D3D10_FEATURE_LEVEL_9_3,
                D3D10_FEATURE_LEVEL_9_2,
                D3D10_FEATURE_LEVEL_9_1,
            };

            for (int n = 0; n < ARRAYSIZE(featureLevels); ++n)
            {
                ID3D10Device1 *pDevice = NULL;
                D3D10_FEATURE_LEVEL1 currFeatureLevel = featureLevels[n];
                hr = D3D10CreateDevice1(
                    pDXGIAdapter, 
                    D3D10_DRIVER_TYPE_HARDWARE,
                    NULL,
                    0,
                    currFeatureLevel,
                    D3D10_1_SDK_VERSION,
                    &pDevice );

                if (SUCCEEDED(hr))
                    m_uMaxCreatedLevel[DXVersion_10_1] = max(m_uMaxCreatedLevel[DXVersion_10_1], ConvertToCommonFeatureLevel(currFeatureLevel));
                else if (hr != E_NOINTERFACE)
                {
                    // E_NOINTERFACE is the only acceptable return code for unsupported feature level
                    tRes = RESULT_FAIL;
                    WriteToLog("D3D10CreateDevice1 failed on FL %s, returning hr=%s", 
                        ToString(currFeatureLevel).c_str(),
                        D3DHResultToString(hr).c_str()
                        );
                }


                SAFE_RELEASE(pDevice);
            }
        }

        // D3D 11.0
        {
            D3D_FEATURE_LEVEL featureLevels[] = 
            {
                D3D_FEATURE_LEVEL_9_2,
                D3D_FEATURE_LEVEL_9_3,
                D3D_FEATURE_LEVEL_9_1,
            };

            // try using whole array
            {
                ID3D11Device *pDevice = NULL;
                D3D_FEATURE_LEVEL currFeatureLevel = D3D_FEATURE_LEVEL_9_1;
                hr = D3D11CreateDevice(
                    pDXGIAdapter, 
                    D3D_DRIVER_TYPE_UNKNOWN,
                    NULL,
                    0,
                    featureLevels,
                    ARRAYSIZE(featureLevels),
                    D3D11_SDK_VERSION,
                    &pDevice,
                    &currFeatureLevel,
                    NULL );

                if (SUCCEEDED(hr))
                    m_uMaxCreatedLevel[DXVersion_11_0] = max(m_uMaxCreatedLevel[DXVersion_11_0], currFeatureLevel);
                else if ( hr != DXGI_ERROR_UNSUPPORTED || 0 < m_uMaxCreatedLevel[DXVersion_10_1] )
                {
                    // DXGI_ERROR_UNSUPPORTED is the only acceptable return code for unsupported feature level
                    // but if we already got a device from 10.1, then we know at least one feature level is supported
                    tRes = RESULT_FAIL;
                    WriteToLog("D3D11CreateDevice failed on array of {9.3, 9.2, 9.1}, returning hr=%s", 
                        D3DHResultToString(hr).c_str()
                        );
                }

                SAFE_RELEASE(pDevice);
            }

            for (int n = 0; n < ARRAYSIZE(featureLevels); ++n)
            {
                ID3D11Device *pDevice = NULL;
                D3D_FEATURE_LEVEL currFeatureLevel = featureLevels[n];
                hr = D3D11CreateDevice(
                    pDXGIAdapter, 
                    D3D_DRIVER_TYPE_UNKNOWN,
                    NULL,
                    0,
                    &currFeatureLevel,
                    1,
                    D3D11_SDK_VERSION,
                    &pDevice,
                    NULL,
                    NULL );

                if (SUCCEEDED(hr))
                    m_uMaxCreatedLevel[DXVersion_11_0] = max(m_uMaxCreatedLevel[DXVersion_11_0], currFeatureLevel);
                else if (hr != DXGI_ERROR_UNSUPPORTED)
                {
                    // DXGI_ERROR_UNSUPPORTED is the only acceptable return code for unsupported feature level
                    tRes = RESULT_FAIL;
                    WriteToLog("D3D11CreateDevice failed on FL %s, returning hr=%s", 
                        ToString(currFeatureLevel).c_str(),
                        D3DHResultToString(hr).c_str()
                        );
                }

                SAFE_RELEASE(pDevice);

                if ( SUCCEEDED( hr ) )
                {
                    //now create the device again on the same adapter:
                    hr = D3D11CreateDevice(
                        pDXGIAdapter, 
                        D3D_DRIVER_TYPE_UNKNOWN,
                        NULL,
                        0,
                        &currFeatureLevel,
                        1,
                        D3D11_SDK_VERSION,
                        &pDevice,
                        NULL,
                        NULL );
                    if ( FAILED( hr ) )
                    {
                        WriteToLog("D3D11 refused to create same device twice on same adapter...");
                        tRes = RESULT_FAIL;
                    }

                    //now create a device on the adapter retrieved from the previous...

                    WindowsTest::CHandle<IDXGIDevice>  pDXGIDevice;
                    WindowsTest::CHandle<IDXGIAdapter> pRetrievedDXGIAdapter;
                    if ( SUCCEEDED( hr ) )
                    {
                        pDevice->QueryInterface( 
                            __uuidof( pDXGIDevice ),
                            reinterpret_cast<void**>(&pDXGIDevice));

                        if ( pDXGIDevice )
                        {
                            pDXGIDevice->GetAdapter( & pRetrievedDXGIAdapter );
                        }
                    }

                    SAFE_RELEASE(pDevice);

                    if ( SUCCEEDED( hr ) && pRetrievedDXGIAdapter )
                    {
                        hr = D3D11CreateDevice(
                            pRetrievedDXGIAdapter, 
                            D3D_DRIVER_TYPE_UNKNOWN,
                            NULL,
                            0,
                            &currFeatureLevel,
                            1,
                            D3D11_SDK_VERSION,
                            &pDevice,
                            NULL,
                            NULL );
                        if ( FAILED( hr ) )
                        {
                            WriteToLog("D3D11 refused to create device on retrieved adapter...");
                            tRes = RESULT_FAIL;
                        }
                    }

                    SAFE_RELEASE(pDevice);
                }
            }
        }

        if (m_uMaxCreatedLevel[DXVersion_10_1] != m_uMaxCreatedLevel[DXVersion_11_0])
        {
            const D3D_FEATURE_LEVEL fl_10_1 = m_uMaxCreatedLevel[DXVersion_10_1];
            const D3D_FEATURE_LEVEL fl_11_0 = m_uMaxCreatedLevel[DXVersion_11_0];
            tRes = RESULT_FAIL;
            WriteToLog("D3D 10.1 and 11.0 disagree on max feature level possible. D3D10.1 = %s, D3D11.0 = %s", 
                ToString(fl_10_1).c_str(),
                ToString(fl_11_0).c_str()
                );
        }

        SAFE_RELEASE(pDXGIAdapter);
        SAFE_RELEASE(pDXGIFactory);
    }

    WriteToLog("Maximum feature levels reported: D3D10.1 = %s, D3D11.0 = %s", 
        ToString(m_uMaxCreatedLevel[DXVersion_10_1]).c_str(),
        ToString(m_uMaxCreatedLevel[DXVersion_11_0]).c_str()
        );

    // verify feature level against master list
    {
        tstring currentDate;
        {
            TCHAR szDate[128];
            int retVal = GetDateFormat(
                LOCALE_USER_DEFAULT, // the locale for which the formatting is being done
                0,                   // date format (long, short, ...) not valid when format string used
                NULL,                // the date to be formatted (here current system date)
                _T("yyyy'-'MM'-'dd"),    // style of date format
                szDate,              // output buffer
                128                  // size of output buffer
                );	
            currentDate = szDate;
        }
        tstring machineName;
        {
            TCHAR tcsMachineName[MAX_COMPUTERNAME_LENGTH + 1] = {0};
            ULONG nChars = MAX_COMPUTERNAME_LENGTH + 1;
            GetComputerName(tcsMachineName, &nChars);
            machineName = tcsMachineName;
            std::replace(machineName.begin(), machineName.end(), ' ', '_');
        }
        tstring adapterName;
        {
            TCHAR sDisplay[256] = {0};
            size_t size = wcslen( m_AdapterDesc.Description );
            if( size )
            {
                size++;
                CopyString( sDisplay, m_AdapterDesc.Description, size );
            }
            adapterName = sDisplay;
            if (adapterName == _T("RDPDD Chained DD"))
            {
                WriteToLog("Cannot collect results over remote desktop, unknown adapter");
                if (tRes == RESULT_PASS)
                    tRes = RESULT_SKIP;
            }
            adapterName = TrimWhiteSpace(adapterName);
            std::replace(adapterName.begin(), adapterName.end(), ' ', '_');
        }
        tstring driverVersion;
        {
            driverVersion = _T("");
            driverVersion += ToString( HIWORD(m_UserModeDriverVersion.HighPart) );
            driverVersion += _T(".");
            driverVersion += ToString( LOWORD(m_UserModeDriverVersion.HighPart) );
            driverVersion += _T(".");
            driverVersion += ToString( HIWORD(m_UserModeDriverVersion.LowPart) );
            driverVersion += _T(".");
            driverVersion += ToString( LOWORD(m_UserModeDriverVersion.LowPart) );
        }
        tstring pnpId;
        {
            TCHAR szBuffer[50];
            _snprintf(szBuffer, 49, _T("VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_%02X"), m_AdapterDesc.VendorId, m_AdapterDesc.DeviceId, m_AdapterDesc.SubSysId, m_AdapterDesc.Revision);
            szBuffer[49] = 0;
            pnpId = szBuffer;
        }
        WriteToLog("PnPID: %s", pnpId.c_str());
        WriteToLog("Adapter Name: %s", adapterName.c_str());
        WriteToLog("Driver Version: %s", driverVersion.c_str());
        if (m_uMaxCreatedLevel[DXVersion_10_1] > 0)
        {
            if (false)
            {
                // log to single file for machine
                tstring logFilePath = g_TestApp.m_FeatureLevelLogPath;
                logFilePath += _T("\\machines\\");
                logFilePath += currentDate;
                logFilePath += _T("_");
                logFilePath += machineName;
                logFilePath += _T(".txt");

                tstringstream outStreamCard;
                WriteDisplayNameToFile(outStreamCard, adapterName);
                WriteFeatureLevelToFile(outStreamCard, m_uMaxCreatedLevel[DXVersion_10_1]);
                WriteDriverVersionToFile(outStreamCard, driverVersion);
                WritePnPIDToFile(outStreamCard, pnpId);
                outStreamCard << std::endl;

                // replace existing file
                std::ofstream outFileCard(logFilePath.c_str(), std::ios_base::out);
                if (!outFileCard)
                {
                    WriteToLog("Could not open feature level log file '%s'", logFilePath.c_str() );
                }
                else
                {
                    outFileCard << outStreamCard.str() << std::endl;
                    outFileCard.close();
                }
            }
            if (g_TestApp.m_FeatureLevelLogPath.size() > 0)
            {
                // log to aggregation file
                tstring logFilePath = g_TestApp.m_FeatureLevelLogPath;
                logFilePath += _T("\\aggregate");
                logFilePath += _T("_");
                logFilePath += currentDate;
                logFilePath += _T(".txt");

                tstringstream outStreamAllCards;
                WriteDisplayNameToFile(outStreamAllCards, adapterName);
                WriteFeatureLevelToFile(outStreamAllCards, m_uMaxCreatedLevel[DXVersion_10_1]);
                WriteDriverVersionToFile(outStreamAllCards, driverVersion);
                WritePnPIDToFile(outStreamAllCards, pnpId);
                WriteMachineNameToFile(outStreamAllCards, machineName);

                // append existing file
                std::ofstream outFileAllCards(logFilePath.c_str(), std::ios_base::app);
                if (!outFileAllCards)
                {
                    WriteToLog("Could not open feature level log file '%s'", logFilePath.c_str() );
                }
                else
                {
                    outFileAllCards << outStreamAllCards.str() << std::endl;
                    outFileAllCards.close();
                }
            }

            DeviceInfo currDevice;
            currDevice.pnpId = pnpId;
            currDevice.adapterName = adapterName;
            currDevice.featureLevel = GetFeatureLevelString(m_uMaxCreatedLevel[DXVersion_10_1]);
            currDevice.driverVersion = driverVersion;
            currDevice.machineName = machineName;

			DeviceMap_t masterDeviceInfo;

			if( g_TestApp.GetMinimumFeatureLevelAsUInt() != 0 )
			{
				// Ensure the feature level supported by the device is equal to or better than the minimum required feature level
				if( (UINT) m_uMaxCreatedLevel[DXVersion_10_1] < g_TestApp.GetMinimumFeatureLevelAsUInt() )
				{
					tstring minFeatureLevelString = GetFeatureLevelString(g_TestApp.GetMinimumFeatureLevel());
					WriteToLog( _T( "Device Feature Level of 9.%s is less than the requested minimum feature level of 9.%s" ), currDevice.featureLevel.c_str(), minFeatureLevelString.c_str() );
					tRes = RESULT_FAIL;
				}
				else
				{
					tRes = RESULT_PASS;
				}

				goto testDone;
			}
			else
			{
				tstring masterLogFileContents;
				// we could optionally load the master file from a specified path, but it's more consistent to always load the embedded resource
				masterLogFileContents = ReadResourceFile( _T("master.txt") );

				if (!ReadDeviceInfoFromMasterFile(masterLogFileContents, masterDeviceInfo))
				{
					tRes = RESULT_FAIL;
					WriteToLog("Failure reading Master list");
					goto testDone;
				}
			}

            DeviceMap_t::iterator masterIt = masterDeviceInfo.find(TrimPnpIdSubsysRev(pnpId));
            if (masterIt != masterDeviceInfo.end())
            {
                // if card is in master list, check that we match the feature level
                DeviceInfo& masterDevice = masterIt->second;
                if (currDevice.featureLevel != masterDevice.featureLevel)
                {
                    tRes = RESULT_FAIL;
                    WriteToLog("Feature Level does not match master list. Current = 9.%s, Master = 9.%s.", 
                        currDevice.featureLevel.c_str(), masterDevice.featureLevel.c_str() );

                    if (g_TestApp.m_FeatureLevelLogPath.size() > 0)
                    {
                        tstring conflictLogFilePath = g_TestApp.m_FeatureLevelLogPath;
                        conflictLogFilePath += _T("\\conflict");
                        conflictLogFilePath += _T("_");
                        conflictLogFilePath += currentDate;
                        conflictLogFilePath += _T(".txt");
                        if (!WriteDeviceInfoToApprovalFile(conflictLogFilePath, currDevice))
                        {
                            tRes = RESULT_FAIL;
                            WriteToLog("Error adding card to conflict file");
                        }
                    }
                }
            }
            else
            {
				// If the HW is not found in the master list, assume FL9.3
				WriteToLog("Device ID not found in master list, assuming Feature Level 9.3");
				if( (UINT) m_uMaxCreatedLevel[DXVersion_10_1] < D3D_FEATURE_LEVEL_9_3 )
				{
					WriteToLog( _T( "Device Feature Level of 9.%s is less than the assumed minimum feature level of 9.3" ), currDevice.featureLevel.c_str() );
                    tRes = RESULT_FAIL;
				}

                if (g_TestApp.m_FeatureLevelLogPath.size() > 0)
                {
                    tstring approveLogFilePath = g_TestApp.m_FeatureLevelLogPath;
                    approveLogFilePath += _T("\\approve");
                    approveLogFilePath += _T("_");
                    approveLogFilePath += currentDate;
                    approveLogFilePath += _T(".txt");
                    DeviceMap_t approveDeviceInfo;
                    if (!ReadDeviceInfoFromMasterFile(ReadFile(approveLogFilePath), approveDeviceInfo))
                    {
                        WriteToLog("Failure reading Approval list");
                        tRes = RESULT_FAIL;
                        goto testDone;
                    }
                    DeviceMap_t::iterator approveIt = approveDeviceInfo.find(TrimPnpIdSubsysRev(pnpId));
                    if (approveIt == approveDeviceInfo.end())
                    {
                        // update approval list with this card info
                        WriteToLog("Adding card to approval list");
                        if (!WriteDeviceInfoToApprovalFile(approveLogFilePath, currDevice))
                        {
                            tRes = RESULT_FAIL;
                            WriteToLog("Error adding card to approval file");
                        }
                    }
                }
            }
        } // RESULT_PASS
    }

testDone:
    return tRes;
}

bool CCreateTest::ReadDeviceInfoFromMasterFile(tstring fileContents, DeviceMap_t& deviceMap)
{
    bool bRes = true;
    std::istringstream masterFile( fileContents );
    if (!masterFile)
    {
        WriteToLog("Could not open master log file" );
    }
    else
    {
        // format: featurelevel[TAB]deviceName[TAB]driverVersion[TAB]machineName[NEWLINE]
        while(masterFile)
        {
            tstring currLine;
            std::getline(masterFile, currLine);
            DeviceInfo currDevice;
            size_t tab1 = currLine.find(_T("\t"), 0);
            currDevice.featureLevel = currLine.substr(0, tab1);
            if (tab1 != tstring::npos)
            {
                size_t tab2 = currLine.find(_T("\t"), tab1 + 1);
                currDevice.pnpId = currLine.substr(tab1 + 1, tab2 - tab1 - 1);
                if (tab2 != tstring::npos)
                {
                    size_t tab3 = currLine.find(_T("\t"), tab2 + 1);
                    currDevice.adapterName = currLine.substr(tab2 + 1, tab3 - tab2 - 1);
                    if (tab3 != tstring::npos)
                    {
                        size_t tab4 = currLine.find(_T("\t"), tab3 + 1);
                        currDevice.driverVersion = currLine.substr(tab3 + 1, tab4 - tab3 - 1);
                        if (tab4 != tstring::npos)
                        {
                            currDevice.machineName = currLine.substr(tab4 + 1, tstring::npos);
                        }
                    }
                    // need at least a pnpid and feature level to make use of this entry
                    DeviceMap_t::iterator existingIt = deviceMap.find(TrimPnpIdSubsysRev(currDevice.pnpId));
                    if (existingIt != deviceMap.end())
                    {
                        WriteToLog("Duplicate entry in master log for device id '%s'", currDevice.pnpId.c_str() );
                        bRes = false;
                    }
                    deviceMap[TrimPnpIdSubsysRev(currDevice.pnpId)] = currDevice;
                }
            }
        }
    }
    return bRes;
}

bool CCreateTest::WriteDeviceInfoToApprovalFile(tstring fileName, DeviceInfo newDevice)
{
    bool bRes = true;
    tstring deviceString = _T("");
    deviceString += newDevice.featureLevel;
    deviceString += _T("\t");
    deviceString += newDevice.pnpId;
    deviceString += _T("\t");
    deviceString += newDevice.adapterName;
    deviceString += _T("\t");
    deviceString += newDevice.driverVersion;
    deviceString += _T("\t");
    deviceString += newDevice.machineName;

    std::ofstream approvalFile(fileName.c_str(), std::ios_base::app);
    if (!approvalFile)
    {
        WriteToLog("Could not open approval log file '%s'", fileName.c_str() );
        bRes = false;
    }
    else
    {
        approvalFile << deviceString << std::endl;

        approvalFile.close();
    }
    return bRes;
}

/////

void CCreateTest::CleanupTestCase()
{
}

/////

void CCreateTest::Cleanup()
{
}


