uteVPxDXVATest(23));
}

void CMediaEngineTests::VP8PlaybackTest24(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(24));
}


void CMediaEngineTests::VP8QualityTest70(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(70));
}

void CMediaEngineTests::VP8QualityTest71(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(71));
}

void CMediaEngineTests::VP8QualityTest72(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(72));
}

void CMediaEngineTests::VP8QualityTest73(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(73));
}

void CMediaEngineTests::VP8QualityTest74(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(74));
}

void CMediaEngineTests::VP9_10bitPlaybackTest90(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(90));
}


void CMediaEngineTests::VP9_10bitQualityTest91(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(91));
}

void CMediaEngineTests::VP9_10bitPlaybackTest92(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(92));
}


void CMediaEngineTests::VP9_10bitQualityTest93(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(93));
}

void CMediaEngineTests::VP8FuzzTest100(void)
{
    ExecuteVPxDXVATest(100);
}

void CMediaEngineTests::VP9FuzzTest101(void)
{
    ExecuteVPxDXVATest(101);
}

void CMediaEngineTests::VP9FuzzTest102(void)
{
    ExecuteVPxDXVATest(102);
}

void CMediaEngineTests::VP9FuzzTest103(void)
{
    ExecuteVPxDXVATest(103);
}

void CMediaEngineTests::VP9FuzzTest104(void)
{
    ExecuteVPxDXVATest(104);
}

void CMediaEngineTests::VP9FuzzTest105(void)
{
    ExecuteVPxDXVATest(105);
}

void CMediaEngineTests::CustomXML(void)
{
    VERIFY_SUCCEEDED(RunCustomXML());
}



static  const TCHAR StoreMFTRegKey[] = TEXT("SOFTWARE\\Microsoft\\Windows Media Foundation\\StoreMFT");

void AllowVPxDecoderForClassicApps()
{
    HKEY    hkey;
    DWORD dwKeyValue = 1;

    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, StoreMFTRegKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL) == ERROR_SUCCESS)
    {
        Log::Comment(String().Format(L"EnableForwin32App reg key = 1", dwKeyValue));

        RegSetValueEx(hkey, TEXT("EnableForwin32App"), 0, REG_DWORD, (BYTE *)&dwKeyValue, sizeof(dwKeyValue));
        RegCloseKey(hkey);
    }
    else
    {
        Log::Comment(String().Format(L"failed to create reg key %s for VP9 decoder creation", StoreMFTRegKey));
    }
}

bool CMediaEngineTests::ClassSetup()
{
    VERIFY_SUCCEEDED(MFStartup(MF_VERSION));
    AllowVPxDecoderForClassicApps();
    return true;
}


bool CMediaEngineTests::ClassCleanup()
{
    VERIFY_SUCCEEDED(MFShutdown());
    return true;
}

#ifdef BUILD_MINWIN

HRESULT CMediaEngineTests::ExecuteTest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;
    CString stringSubType;
    GUID subType = GUID_NULL;



    do
    {
        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_PLAYBACKCONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetString(L"InputType/MF_MT_SUBTYPE", stringSubType);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        if((stringSubType == "MFVideoFormat_MJPG") || (stringSubType == "MEDIASUBTYPE_MP4V") || (stringSubType == "MEDIASUBTYPE_H263"))
        {
            subType = GetSubTypeFromString(stringSubType);
            hr = VerifyHardwareDecoderPresent(subType);
            if (FAILED(hr))
            {
                break;
            }
            else if (hr == S_FALSE)
            {
                // No HW decoder present, skip test
                break;
            }
        }

        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Config", stringConfig);
        if (FAILED(hr) || stringConfig.IsEmpty())
        {
            // Use default config
            stringConfig = L"-DCOMP -DXVA";
        }
        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty() || !stringTestType.CompareNoCase(L"Playback"))
        {
            // Use default config
            stringTestType = L"Render";
        }

        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);

    } while (FALSE);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped, WEX::Common::String().Format(L"No Hardware decoder for %s found, skipping test", stringSubType));
    }

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}

HRESULT CMediaEngineTests::ExecuteTest()
{
    HRESULT hr = S_OK;

    //SetWexLogger();

    //Call the methods of to initialize and execute a particular test case until done.
    WEX::Common::String stringConfig;
    TestData::TryGetValue(L"Config", stringConfig);

    WEX::Common::String stringInput;
    TestData::TryGetValue(L"Input", stringInput);

    WEX::Common::String stringOutput;
    TestData::TryGetValue(L"Output", stringOutput);

    WEX::Common::String stringTestType;
    TestData::TryGetValue(L"TestType", stringTestType);

    WEX::Common::String stringTestID;
    TestData::TryGetValue(L"ID", stringTestID);

    WEX::Common::String stringTestTitle;
    TestData::TryGetValue(L"Title", stringTestTitle);

    WEX::Common::String stringDLLName;
    TestData::TryGetValue(L"DLL", stringDLLName);

    hr = MediaEngineTestEntryPointMobile(stringInput, stringOutput, stringConfig, stringTestType,
        _ttoi(stringTestID), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped);
    }

    return hr;
}

HRESULT CMediaEngineTests::ExecuteVisValTest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";
    CString stringSubType;

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;
    GUID subType = GUID_NULL;


    do
    {
        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_VISVALCONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetString(L"InputType/MF_MT_SUBTYPE", stringSubType);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        if((stringSubType == "MFVideoFormat_MJPG") || (stringSubType == "MEDIASUBTYPE_MP4V") || (stringSubType == "MEDIASUBTYPE_H263"))
        {
            subType = GetSubTypeFromString(stringSubType);
            hr = VerifyHardwareDecoderPresent(subType);
            if (FAILED(hr))
            {
                break;
            }
            else if (hr == S_FALSE)
            {
                // No HW decoder present, skip test
                break;
            }
        }

        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Config", stringConfig);
        if (FAILED(hr) || stringConfig.IsEmpty())
        {
            // Use default config
            stringConfig = L"-frameserver -DXVA -frames 200 -frameinterval 15 -visval";
        }
        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty() || !stringTestType.CompareNoCase(L"Playback"))
        {
            // Use default config
            stringTestType = L"Render";
        }

        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);

    } while (FALSE);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped, WEX::Common::String().Format(L"No Hardware decoder for %s found, skipping test", stringSubType));
    }

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}

HRESULT CMediaEngineTests::ExecuteOneCoreTest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;

    do
    {
        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_ONECORETESTSCONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty())
        {
            // Use default config. Assume frame server test if nothing is given
            stringConfig = L"-frameserver -frames 20 ";
        }
        else if (!stringTestType.CompareNoCase(L"Quality"))
        {
            stringConfig = L"-frameserver -frames 20 ";
        }
        else if (!stringTestType.CompareNoCase(L"Full_Quality"))
        {
            CString stringNumFrames;
            hr = pTestConfig->GetString(L"NumFrames", stringNumFrames);
            if (FAILED(hr) || stringNumFrames.IsEmpty())
            {
                stringConfig = L"-frameserver -NoDumpFrame  ";
            }
            else
            {
                stringConfig = L"-frameserver -NoDumpFrame -frames "+ stringNumFrames;
            }           
        }


        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);

        if (hr == S_FALSE)
        {
            WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped);
        }

    } while (FALSE);

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}


HRESULT CMediaEngineTests::ExecuteDXVATest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;

    do
    {
        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_DXVATESTSCONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty())
        {
            // Use default config. Assume plabyack test if nothing is given
            stringTestType = L"Render";

            stringConfig = L"-DCOMP -DXVA";
        }
        else if (!stringTestType.CompareNoCase(L"Playback"))
        {
            stringConfig = L"-DCOMP -DXVA";
        }
        else if (!stringTestType.CompareNoCase(L"Quality"))
        {
            stringConfig = L"-frameserver -DXVA -frames 200  -frameinterval 15 -visval";
        }


        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);

        if (hr == S_FALSE)
        {
            WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped);
        }

    } while (FALSE);

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}

#endif


HRESULT CMediaEngineTests::ExecuteVPxDXVATest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";
    CString stringInputType;

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;
    GUID guid = D3D11_DECODER_PROFILE_VP8_VLD;


    do
    {

        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_VPxDXVATESTSCONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"InputType", stringInputType);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        if (!stringInputType.CompareNoCase(L"VP8"))
        {
            guid = D3D11_DECODER_PROFILE_VP8_VLD;
        }
        else if (!stringInputType.CompareNoCase(L"VP9_PROFILE0"))
        {
            guid = D3D11_DECODER_PROFILE_VP9_VLD_PROFILE0;
        }
        else if (!stringInputType.CompareNoCase(L"VP9_PROFILE2"))
        {
            guid = D3D11_DECODER_PROFILE_VP9_VLD_10BIT_PROFILE2;
        }

        hr = VerifyDXVASupported(guid);
        if (FAILED(hr))
        {
            break;
        }
        else if (hr == S_FALSE)
        {
            // No HW HEVC decoder present, skip test
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty())
        {
            // Use default config. Assume plabyack test if nothing is given
            stringTestType = L"Render";

            stringConfig = L"-DCOMP -DXVA";
        }
        else if (!stringTestType.CompareNoCase(L"Playback"))
        {
            stringConfig = L"-DCOMP -DXVA";
        }
        else if (!stringTestType.CompareNoCase(L"Quality"))
        {
            stringConfig = L"-frameserver -DXVA -frames 200  -frameinterval 15 -visval";
        }


        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);


    } while (FALSE);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped, WEX::Common::String().Format(L"No Hardware VPx decoder found, skipping test"));
    }

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}



HRESULT CMediaEngineTests::ExecutePipelineTest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringKID = L"";
    CString stringContentKey = L"";
    CString stringDLLName = L"";

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;

    GUID clsid = GUID_NULL;

    do
    {
        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_MEDIAPIPELINECONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetGUID(L"Component_CLSID", &clsid);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        if (clsid == CLSID_MSH265DecoderMFT)
        {
            hr = VerifyDXVASupported(D3D11_DECODER_PROFILE_HEVC_VLD_MAIN);
            if (FAILED(hr))
            {
                WEX::Logging::Log::Comment(WEX::Common::String().Format(L"VerifyDXVASupported for D3D11_DECODER_PROFILE_HEVC_VLD_MAIN Error: 0x%x", hr));
                break;
            }
            else if (hr == S_FALSE)
            {
                // No HW decoder present, skip test
                break;
            }
        }

        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Config", stringConfig);
        if (FAILED(hr) || stringConfig.IsEmpty())
        {
            // Use default config
            stringConfig = L"-DCOMP -DXVA";
        }
        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty())
        {
            // Use default config
            stringTestType = L"Render";
        }

        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        if (!stringTestType.CompareNoCase(L"DRM"))
        {
            hr = pTestConfig->GetString(L"KID", stringKID);
            if (FAILED(hr))
            {
                hr = E_FAIL;
                break;
            }

            hr = pTestConfig->GetString(L"ContentKey", stringContentKey);
            if (FAILED(hr))
            {
                hr = E_FAIL;
                break;
            }
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

#ifndef BUILD_MINWIN

        hr = MediaEngineTestEntryPoint(stringInput, stringOutput, stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, stringKID, stringContentKey, WEX_LOG);
#else
        hr = MediaEngineTestEntryPointMobile(stringInput, stringOutput, stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, stringKID, stringContentKey, WEX_LOG);
#endif

    } while (FALSE);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped, WEX::Common::String().Format(L"No H265 DXVA decoder found, skipping test"));
    }

    // Mark the test as blocked if it is a DRM test and it failed due to issues with internet access.
    if (hr == WININET_E_NAME_NOT_RESOLVED && !stringTestType.CompareNoCase(L"DRM"))
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Blocked, WEX::Common::String().Format(L"DRM tests require internet access."));
        hr = S_FALSE;
    }

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}

HRESULT InitializePlaybackTestTable(CXmlRootConfig** ppXMLRootConfig, DWORD dwTestConfig)
{
    HRESULT hr = S_OK;
    HGLOBAL hResource = NULL;
    CXmlRootConfig* pConfig = NULL;

    do
    {
        if (ppXMLRootConfig == NULL)
        {
            hr = E_POINTER;
            break;
        }

        pConfig = new CXmlRootConfig();
        if (pConfig == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        HMODULE hMod = NULL;

        if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (TCHAR*)&InitializePlaybackTestTable, &hMod) == FALSE)
        {
            hr = E_FAIL;
            break;
        }

        HRSRC hRsrc = FindResource(hMod, MAKEINTRESOURCE(dwTestConfig), L"Config");
        if (hRsrc == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hResource = LoadResource(hMod, hRsrc);
        if (hResource == NULL)
        {
            hr = E_FAIL;
            break;
        }

        char* pszXML = (char*)LockResource(hResource);
        if (pszXML == NULL)
        {
            hr = E_FAIL;
            break;
        }

        DWORD dwXML = SizeofResource(hMod, hRsrc);
        if (dwXML == 0)
        {
            hr = E_FAIL;
            break;
        }

        int nConvertedLength = MultiByteToWideChar(CP_ACP, 0, pszXML, dwXML, NULL, NULL);

        CComBSTR bstrXML(nConvertedLength);

        if (MultiByteToWideChar(CP_ACP, 0, pszXML, dwXML, bstrXML, nConvertedLength) == 0)
        {
            hr = E_FAIL;
            break;
        }

        hr = pConfig->Init(bstrXML);
        if (FAILED(hr))
        {
            break;
        }

        *ppXMLRootConfig = pConfig;
    } while (false);

    if (hResource != NULL)
    {
        FreeResource(hResource);
    }

    if (FAILED(hr))
    {
        SAFEDELETES(pConfig);
    }

    return hr;
}

HRESULT CMediaEngineTests::GetGUIDsToTest(
    const DWORD dwTestID,
    GUID pGUIDsToTest[],
    WCHAR* pppszHWURLs[],
    const DWORD dwMaxGUIDsToTest,
    DWORD *pdwNumGUIDs)
{
    HRESULT hr = S_OK;

    do
    {
        if (pdwNumGUIDs == NULL)
        {
            hr = E_POINTER;
            break;
        }

        (*pdwNumGUIDs) = 1;
    } while (false);

    return hr;
}


HRESULT CMediaEngineTests::RunActualTest(const DWORD dwTestID, const GUID* pGUID, const WCHAR* pszHWURL)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";
    CString stringKID = L"";
    CString stringContentKey = L"";

    CXmlConfig* pTestConfig = NULL;

    do
    {
        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestID);

        pTestConfig = m_pXMLConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Category", stringTestType);
        
        if (FAILED(hr) || stringTestType.IsEmpty())
        {
            // Use default config. Assume plabyack test if nothing is given
            stringTestType = L"Render";

            stringConfig = L"-DCOMP -DXVA";
        }
        else
        {
            hr = pTestConfig->GetString(L"Config", stringConfig);

            if (!stringTestType.CompareNoCase(L"Playback"))
            {
                if (FAILED(hr) || stringConfig.IsEmpty())
                {
                    stringConfig = L"-DCOMP -DXVA";
                }
                
            }
            else if (!stringTestType.CompareNoCase(L"Quality"))
            {
                if (FAILED(hr) || stringConfig.IsEmpty())
                {
                    stringConfig = L"-frameserver -DXVA -frames 200  -frameinterval 15 -visval";
                }
            }

#ifndef BUILD_MINWIN
            else if (!stringTestType.CompareNoCase(L"DRM"))
            {
                hr = pTestConfig->GetString(L"KID", stringKID);
                if (FAILED(hr))
                {
                    hr = E_FAIL;
                    break;
                }

                hr = pTestConfig->GetString(L"ContentKey", stringContentKey);
                if (FAILED(hr))
                {
                    hr = E_FAIL;
                    break;
                }
            }
#endif
        }

        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

#ifndef BUILD_MINWIN
        hr = MediaEngineTestEntryPoint(stringInput, stringOutput, stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, stringKID, stringContentKey, WEX_LOG);        
#else
        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, stringKID, stringContentKey, WEX_LOG);
#endif

    } while (FALSE);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped);
    }

    SAFE_DELETE(pTestConfig);

    return hr;
}

HRESULT CMediaEngineTests::RunActualChildTest(
    const DWORD     dwInstance,
    const WCHAR*    pwszContextName,
    const DWORD     dwContextSize)
{
    HRESULT hr = S_OK;
    return hr;
}


HRESULT CMediaEngineTests::ExecuteHEVCDXVATest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";
    CString stringInputType;

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;
    GUID guid = D3D11_DECODER_PROFILE_HEVC_VLD_MAIN;


    do
    {

        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_HEVCDXVATESTSCONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"InputType", stringInputType);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        if (!stringInputType.CompareNoCase(L"HEVC10"))
        {
            guid = D3D11_DECODER_PROFILE_HEVC_VLD_MAIN10;
        }

        hr = VerifyDXVASupported(guid);
        if (FAILED(hr))
        {
            break;
        }
        else if (hr == S_FALSE)
        {
            // No HW HEVC decoder present, skip test
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty())
        {
            // Use default config. Assume plabyack test if nothing is given
            stringTestType = L"Render";

            stringConfig = L"-DCOMP -DXVA";
        }
        else if (!stringTestType.CompareNoCase(L"Playback"))
        {
            stringConfig = L"-DCOMP -DXVA";
        }
        else if (!stringTestType.CompareNoCase(L"Quality"))
        {
            stringConfig = L"-frameserver -DXVA -frames 200  -frameinterval 15 -visval";
        }


        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);

    } while (FALSE);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped, WEX::Common::String().Format(L"No Hardware HEVC decoder found, skipping test"));
    }

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}
#include <windows.h>
#include <ntverp.h>
#include "resource.h"
#include <d3dframework.rc>

#define VER_FILETYPE                VFT_APP
#define VER_FILESUBTYPE             VFT_UNKNOWN
#define VER_FILEDESCRIPTION_STR     "Direct3D conformance test"
#define VER_INTERNALNAME_STR        "updatesurface"
#define VER_ORIGINALFILENAME_STR    "UpdateSurface.exe"


/////////////////////////////////////////////////////////////////////////////
//
// DIBFILETYPE
//

IDR_DIBFILETYPE1        DIBFILETYPE DISCARDABLE "tex.bmp"
IDR_DIBFILETYPE2        DIBFILETYPE DISCARDABLE "lake.bmp"

#include <common.ver>// FILE:        sprite.cpp
// DESC:        point sprite class methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:    for issues regarding point sprites, see .\sprite.doc

#include <math.h>
#include "sprite.h"

#ifndef RGB_MAKE
#define RGB_MAKE(r, g, b) ((D3DCOLOR) (((r) << 16) | ((g) << 8) | (b)))
#endif

CD3DWindowFramework   App;
CSpriteAttenuateFVFL  SpriteAttenuateFVFL;
CSpriteAttenuateFVFLS SpriteAttenuateFVFLS;
CSpriteScaleFVFL      SpriteScaleFVFL;
CSpriteScaleFVFLS     SpriteScaleFVFLS;
CSpriteScaleFVFTL     SpriteScaleFVFTL;
CSpriteScaleFVFTLS    SpriteScaleFVFTLS;
CSpriteWrapTrans      WrapTrans;
CSpriteUClip          SpriteUClip;
CSpriteVClip          SpriteVClip;
CSpriteCull           SpriteCull;
CSpriteFill           SpriteFill;
CSpriteBatch          SpriteBatch;
CSpriteCap            SpriteCaps;
CSpriteMipMap         SpriteMipMap;
CSpriteMipMapMulti    SpriteMipMapMulti;

// NAME:        CSprite()
// DESC:        constructor for sprite class
// INPUT:       none
// OUTPUT:      none

CSprite::CSprite(VOID)
{   
    SetTestType(TESTTYPE_CONF);

    sprintf(m_szFormat, "NA");
    ClearStatus();

    m_fFixed = 0.0f;

    m_dvAngle = 55.0f*pi/180.0f;
    m_dvFar = 90.0f;
    m_dvNear = 1.0f;

    m_At.x = 0.0f;
    m_At.y = 0.0f;
    m_At.z = 1.0f;

    m_From.x = 0.0f;
    m_From.y = 0.0f;
    m_From.z = -1.0f;

    m_Up.x = 0.0f;
    m_Up.y = 1.0f;
    m_Up.z = 0.0f;

    m_dwMinLevelStd = (DWORD) 0;
    m_dwMinLevelYUV = (DWORD) 0;
    m_dwMinLevel = (DWORD) 0;
    m_dwMaxLevel = (DWORD) 0;

    m_iNumStagesSet = 1;

	// Initialize image compare settings required for point sprite test
    m_pFramework->GetImageCmp()->SetMinPixels(0);
	m_pFramework->GetImageCmp()->SetImgDeltaWght(0.92f);

    m_Options.ModeOptions.CubeFmt = FMT_ANY;
    m_Options.ModeOptions.VolumeFmt = FMT_ANY;
    m_Options.ModeOptions.ZBufferFmt = FMT_ANY;
    m_Options.ModeOptions.dwTexInclude = (PF_RGB | PF_DXTN | PF_YUV | PF_LUMINANCE | PF_ALPHA);
    m_Options.ModeOptions.dwTexExclude = (DDPF_PALETTEINDEXED1 |
                                          DDPF_PALETTEINDEXED2 |
                                          DDPF_PALETTEINDEXED4 |
                                          DDPF_PALETTEINDEXED8 |
                                          DDPF_PALETTEINDEXEDTO8);

    m_Options.D3DOptions.bZBuffer = true;
    m_Options.D3DOptions.fMinDXVersion = 8.0f;
}

// NAME:        ~CSprite()
// DESC:        destructor for sprite class
// INPUT:       none
// OUTPUT:      none

CSprite::~CSprite(VOID)
{
}


// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSprite::CommandLineHelp(void)
{
    WriteCommandLineHelp("$yextents: $wdraw viewport extents $c($wOn$c/Off)");
    WriteCommandLineHelp("$yflat: $wflat shade $c($wOn$c/Off)");
    WriteCommandLineHelp("$ymipmap: $wmipmap $c($wOn$c/Off)");
    WriteCommandLineHelp("$ynotex: $wno textures $c($wYes$c/No)");
    WriteCommandLineHelp("$yfixed: $wfixed max point size $c(default=0)");
    WriteCommandLineHelp("$ystages: $wmax simultaneous textures $c(default=caps)");
    WriteCommandLineHelp("$yvariations: $wvariations per texture $c(default=%d)", NVDEF);
}


// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSprite::TestInitialize(VOID)
{
    DWORD dwWidth = (DWORD) MAXSIZE, dwHeight = (DWORD) MAXSIZE;
    UINT i, j;

    SetTestRange((UINT) 1, (UINT) 1);    

    m_Rect.top = 0;
    m_Rect.left = 0;
    m_Rect.bottom = m_pCurrentMode->nBackBufferHeight;
    m_Rect.right = m_pCurrentMode->nBackBufferWidth;

    m_dcBackground = RGB_MAKE(255, 255, 255);
    
    m_iFormat = (INT) NOFORMAT;    

    m_nVariationsPerFormat = (UINT) 0;    

    for (i = 0; i < D3DDP_MAXTEXCOORD; i++)
    {
        m_rgpTexture[i] = NULL;

        for (j = 0; j < MIPLEVELS; j++)
            m_rgpImage[i][j] = NULL;
    }

    m_Sprite.Ka.f = 1.0f;
    m_Sprite.Kb.f = 0.0f;
    m_Sprite.Kc.f = 0.0f;
    m_Sprite.PointSize.f = 1.0f;
    m_Sprite.PointSizeMax.f = 1.0f;
    m_Sprite.PointSizeMin.f = 1.0f;

    ProcessArgs();

    CAPS RefCaps, SrcCaps; // raw device caps
    m_pD3D->GetDeviceCaps(m_pRefDevice->GetAdapterID(), m_pRefDevice->GetDevType(), &RefCaps);
    m_pD3D->GetDeviceCaps(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), &SrcCaps);

    DEVICEDESC *pRefDesc, *pSrcDesc; // device description after vertex processing behavior applied
    pRefDesc = m_pRefDevice->GetCaps();
    pSrcDesc = m_pSrcDevice->GetCaps();

    if ((m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_SCISSORTEST) &&
        (m_pRefDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_SCISSORTEST))
        m_Flags.Set(CF_SCISSOR);

    if (SrcCaps.MaxPointSize <= 1.0f)
    {
        WriteToLog("Device does NOT support point size (testing emulation).\n");
        m_Flags.Set(CF_EMULATING);

        // point sprites are not required to be supported but cap must be set to 1.0f

        if (SrcCaps.MaxPointSize < 1.0f)
        {
            if (m_pSrcDevice->GetDDI() <= 0x007)
            {
                WriteToLog("Runtime failure: MaxPointSize should = 1.0f for this hardware.\n");
                return D3DTESTINIT_ABORT;
            }
            else 
            {
                WriteToLog("Drivers must set MaxPointSize = 1.0f to indicate no support.\n");
                return D3DTESTINIT_ABORT;
            }
        }

        SrcCaps.MaxPointSize = pSrcDesc->dvMaxPointSize;
    }

    if (SrcCaps.MaxPointSize < RefCaps.MaxPointSize)
        m_Sprite.PointSizeMax.f = SrcCaps.MaxPointSize;
    else
        m_Sprite.PointSizeMax.f = RefCaps.MaxPointSize;    

    // allow fixed max point size if specified in (1,max]

    if (m_fFixed > 1.0f)
        m_Sprite.PointSizeMax.f = (m_Sprite.PointSizeMax.f > m_fFixed) ? m_fFixed : m_Sprite.PointSizeMax.f;

    if (m_Flags.Check(CF_VERTEXPOINTSIZE))
    {
        if (!(m_pSrcDevice->GetCaps()->dwFVFCaps & D3DFVFCAPS_PSIZE))
        {
            WriteToLog("Src device does not support point size in vertex data.\n");
            m_Flags.Clear(CF_VERTEXPOINTSIZE);
        }

        if (!(m_pRefDevice->GetCaps()->dwFVFCaps & D3DFVFCAPS_PSIZE))
        {
            WriteToLog("Ref device does not support point size in vertex data.\n");
            m_Flags.Clear(CF_VERTEXPOINTSIZE);
        }
    }
    
    // caps check tests that this value is actually correct ie > MIN_MAXPOINTSIZE

    if (m_Sprite.PointSizeMax.f <= 1.0f)
    {
        // if we have no point size then abort the test

        WriteToLog("Device does NOT support point sprites.\n");

        // sprites are not required to be supported but the cap must be set to 1.0f

        if (!(m_pSrcDevice->GetBehaviors() & D3DCREATE_SOFTWARE_VERTEXPROCESSING))
        {
            if (m_Sprite.PointSizeMax.f < 1.0f)
            {
                if (m_pSrcDevice->GetDDI() <= 0x007)
                {
                    WriteToLog("Runtime failure: MaxPointSize should = 1.0f for this hardware.\n");
                    return D3DTESTINIT_ABORT;
                }
                else
                {
                    WriteToLog("Drivers must set MaxPointSize = 1.0f to indicate no support.\n");
                    return D3DTESTINIT_ABORT;
                }
            }
            else
            {
                WriteToLog("Drivers for TNL hw are allowed to set MaxPointSize = 1.0f.\n");
                return D3DTESTINIT_SKIPALL;
            }
        }
        else
        {
            if (m_Sprite.PointSizeMax.f <= 1.0f)
            {
                WriteToLog("Runtime failure: Point sprites should be emulated for this hardware.\n");
                return D3DTESTINIT_ABORT;
            }
        }
    }

    if (!(m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT) && m_Flags.Check(CF_MIPMAP))
    {
        WriteToLog("Src device does not support mip mapping.\n");
        return D3DTESTINIT_SKIPALL;
    }

    // find max point that can be contained within the port

    m_dvPointSizeMax = (D3DVALUE) ((m_pCurrentMode->nBackBufferWidth > m_pCurrentMode->nBackBufferHeight) ? m_pCurrentMode->nBackBufferHeight : m_pCurrentMode->nBackBufferWidth);
    m_dvPointSizeMax = (m_Sprite.PointSizeMax.f < m_dvPointSizeMax) ? m_Sprite.PointSizeMax.f : m_dvPointSizeMax;

    DWORD pdwColors[4] =
    {
        0xFFFF0000,
        0xFF00FF00,
        0xFF0000FF,
        0xFFFFFFFF
    };    

    m_rgpImage[0][0] = new CImage;

    if (!m_rgpImage[0][0])
    {
        WriteToLog("Unable to create image 0.\n");
        return D3DTESTINIT_ABORT;
    }

    // load gradient color for top mip level (stage 0)

    if (!m_rgpImage[0][0]->LoadGradient(dwWidth, dwHeight, pdwColors))
    {
        WriteToLog("Unable to load image.\n");
        return D3DTESTINIT_ABORT;
    }
    
    for (i = 1; i < m_iNumStagesSet; i++)
    {
        DWORD pdwStripes[D3DDP_MAXTEXCOORD];
        
        m_rgpImage[i][0] = new CImage;

        if (!m_rgpImage[i][0])
        {
            WriteToLog("Unable to create image 0.\n");
            return D3DTESTINIT_ABORT;
        }
    
        // load vertical stripe for top mip level (stage > 0)

        memset(pdwStripes, 0xffffffff, sizeof(pdwStripes));
        pdwStripes[i] = pdwColors[i%4];
        
        if (!m_rgpImage[i][0]->LoadStripes(dwWidth, dwHeight, m_iNumStagesSet, pdwStripes, true ) )
        {
            WriteToLog("Unable to load image %d.\n", i);
            return D3DTESTINIT_ABORT;
        }
    }

    if (m_Flags.Check(CF_MIPMAP))
    {
        DWORD dwMipWidth, dwMipHeight;
        DWORD pdwStageStripes[D3DDP_MAXTEXCOORD];
        DWORD pdwStripes[MIPLEVELS] =
        {
            { 0xFF000000 },
            { 0xFFFFFF00 },
            { 0xFFFF00FF },
            { 0xFFFF0000 },
            { 0xFF00FF00 },
            { 0xFF0000FF },
            { 0xFF00FFFF },
            { 0xFFF0000F },
            { 0xFF0F00F0 }
        };

        // check mipmap level caps

        DWORD dwMinTextureWidth = m_pSrcDevice->GetCaps()->dwMinTextureWidth;
        DWORD dwMaxTextureWidth = m_pSrcDevice->GetCaps()->dwMaxTextureWidth;
        DWORD dwMinTextureHeight = m_pSrcDevice->GetCaps()->dwMinTextureHeight;
        DWORD dwMaxTextureHeight = m_pSrcDevice->GetCaps()->dwMaxTextureHeight;

        // equalize texture width and height

        if (dwMaxTextureWidth > dwMaxTextureHeight)
            dwMaxTextureWidth = dwMaxTextureHeight;
        else
            dwMaxTextureHeight = dwMaxTextureWidth;

        if (dwMinTextureWidth < dwMinTextureHeight)
            dwMinTextureWidth = dwMinTextureHeight;
        else
            dwMinTextureHeight = dwMinTextureWidth;

        // find appropriate max and min mip levels

        for (m_dwMaxLevel =         0; (MAXSIZE >> m_dwMaxLevel) > dwMaxTextureWidth;  m_dwMaxLevel++);
        for (m_dwMinLevel = MIPLEVELS; (MAXSIZE >> m_dwMinLevel) < dwMinTextureHeight; m_dwMinLevel--);

        m_dwMinLevelStd = m_dwMinLevel;
        m_dwMinLevelYUV = m_dwMinLevel;

        // Matrox drivers fail YUV mipmap texture creation when width is less than 8 texels

        while ((MAXSIZE >> m_dwMinLevelYUV) < 8)
            m_dwMinLevelYUV--;

        WriteToLog("(INFO) Max texture size supported (up to %dx%d): %dx%d.\n",
                   MAXSIZE, MAXSIZE, MAXSIZE >> m_dwMaxLevel, MAXSIZE >> m_dwMaxLevel);

        WriteToLog("(INFO) Min texture size supported (down to 1x1): %dx%d.\n",
                   MAXSIZE >> m_dwMinLevel, MAXSIZE >> m_dwMinLevel);

        for (i = 0; i < m_iNumStagesSet; i++)
        {
            dwMipWidth = MAXSIZE >> m_dwMaxLevel;
            dwMipHeight = MAXSIZE >> m_dwMaxLevel;

            for (j = 1; j <= m_dwMinLevel; j++)
            {
                m_rgpImage[i][j] = new CImage;

                if (!m_rgpImage[i][j])
                {
                    WriteToLog("Unable to create image %d.\n", i);
                    return D3DTESTINIT_ABORT;
                }

                dwMipWidth = (1 > (dwMipWidth >> 1)) ? 1 : dwMipWidth >> 1;
                dwMipHeight = (1 > (dwMipHeight >> 1)) ? 1 : dwMipHeight >> 1;

                if (0 == i) // first stage
                {
                    // load solid color in each mip level (stage 0)

                    if (!m_rgpImage[i][j]->LoadStripes(dwMipWidth, dwMipHeight, 1, &pdwStripes[j], false))
                    {
                        WriteToLog("Unable to load image %d.\n", j);
                        return D3DTESTINIT_ABORT;
                    }
                }
                else
                {
                    memset(pdwStageStripes, 0xffffffff, sizeof(pdwStageStripes));
                    pdwStageStripes[i] = pdwStripes[(D3DDP_MAXTEXCOORD - i) % MIPLEVELS];                    

                    // load horizontal stripe in each mip level (stage > 0)

                    if (!m_rgpImage[i][j]->LoadStripes(dwMipWidth, dwMipHeight, m_iNumStagesSet, pdwStageStripes, false))
                    {
                        WriteToLog("Unable to load image %d.\n", i);
                        return D3DTESTINIT_ABORT;
                    }
                }
            }
        }
    }

#ifdef FAST_TEST
    m_uCommonTextureFormats = 1;
#endif

    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSprite::ExecuteTest(UINT uTest)
{
    bool bFound = false, bValid = true;
    int iFormat, iStage;

    iFormat = (uTest - 1) / m_nVariationsPerFormat;
    iFormat = iFormat % (m_uCommonTextureFormats + 1);

    if (iFormat < m_uCommonTextureFormats)
    {
        if (!m_Flags.Check(CF_TEXTURE) && (m_Flags.Saved() & CF_TEXTURE))
            m_Flags.Set(CF_TEXTURE); // restore if necessary

        bFound = true;
    }
    else if (iF