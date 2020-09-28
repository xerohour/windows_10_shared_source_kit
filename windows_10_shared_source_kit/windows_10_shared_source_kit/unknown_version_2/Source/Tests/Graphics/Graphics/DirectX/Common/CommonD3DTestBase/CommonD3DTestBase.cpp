////////////////////////////////////////////////////////////////
// CommonD3DTestBase.cpp


#include <atlbase.h>
#include <windows.h>
#include <string>
#include <fstream>
#include <initguid.h> // Needed for IID_ISimpleLog
#include <tlhelp32.h>

#include "CommonD3DTestBase.h"
#include "TAEFLog.hpp"
#include "WexTestClass.h"
#include "SystemProperties.hpp"

//
// HELPER FUNCTIONS
//
static bool ApplySettingsFromRuntimeParameters(CPropertyManager *pPropertyManager)
{
	// Fetch the list of registered properties
	UINT numProperties = pPropertyManager->GetPropertyCount();
	std::vector<CProperty *> properties;
	properties.resize(numProperties);
	pPropertyManager->GetProperties(properties.data(), numProperties);

	// For each registered property...
	for (UINT index = 0; index < numProperties; ++index)
	{
		CProperty *pProperty = properties[index];

		assert(pProperty);

		if (pProperty)
		{
			HRESULT hr = S_OK;
			WEX::Common::String str;
			const tstring &propertyName = pProperty->GetName();
			const std::wstring propertyNameW = ToStringW(propertyName);
			const WCHAR *pNameW = propertyNameW.c_str();
			hr = WEX::TestExecution::RuntimeParameters::TryGetValue(pNameW, str);
			if (SUCCEEDED(hr))
			{
				tstring tstr = ToString(std::wstring((const wchar_t *)str));
				pProperty->SetValueString(tstr);
			}
		}
	}

	return true;
}



////////////////////////////////////////////////////////////////
//
// class CCommonD3DTestModuleBase
//
////////////////////////////////////////////////////////////////
CCommonD3DTestModuleBase *CCommonD3DTestModuleBase::s_pSingleton = nullptr;

CCommonD3DTestModuleBase::CCommonD3DTestModuleBase() :
m_pDeviceManager(nullptr),
m_pSettingsManager(nullptr),
m_pProcessProperties(nullptr),
m_pParentProcessProperties(nullptr),
m_pGeneralSystemProperties(nullptr),
m_pGraphicsDeviceProperties(nullptr),
m_pLogger(nullptr)
{

}

CCommonD3DTestModuleBase::~CCommonD3DTestModuleBase()
{

}

bool CCommonD3DTestModuleBase::InternalSetup()
{
	HRESULT hr = S_OK;
	ATL::CComPtr<CTAEFLog> pLogger;
	hr = CTAEFLog::CreateInstance(IID_ISimpleLogController, reinterpret_cast<void **>(&pLogger));
	if (FAILED(hr))
	{
		std::wstring errorString = L"Failure creating CTAEFLog with hresult error " + ToStringW(hr);
		WEX::Logging::Log::Error(errorString.c_str());
		return false;
	}

	m_pLogger = pLogger;
	m_pLogger->AddRef();

	//
	// Configure the settings manager
	//

	m_pSettingsManager = new(std::nothrow) CPropertyManager();
	if (nullptr == m_pSettingsManager)
	{
		LOG_HRESULT_ERROR(E_OUTOFMEMORY, L"Out of memory allocating CPropertyManager object");
		return false;
	}

	//
	// Create device manager
	//
	m_pDeviceManager = new(std::nothrow) CD3D12DeviceManager();

	if (nullptr == m_pDeviceManager)
	{
		LOG_HRESULT_ERROR(E_OUTOFMEMORY, L"Out of memory allocating CD3D12DeviceManager object");
		return false;
	}

	// Setup device manager
	hr = m_pDeviceManager->Setup(m_pLogger);
	if (FAILED(hr))
	{
		LOG_HRESULT_ERROR(hr, L"Failure setting up device manager");
		return false;
	}

	if (!InternalRegisterSettings(m_pSettingsManager))
	{
		return false;
	}

	if (!ApplySettingsFromRuntimeParameters(m_pSettingsManager))
	{
		return false;
	}

	//
	// Gather and log test and process properties
	// Note this property manager also includes information about the test dll
	//

	m_pProcessProperties = new(std::nothrow) CPropertyManager();
	if (nullptr == m_pProcessProperties)
	{
		LOG_HRESULT_ERROR(E_OUTOFMEMORY, L"Out of memory allocating CPropertyManager object");
		return false;
	}

	m_pParentProcessProperties = new(std::nothrow) CPropertyManager();
	if (nullptr == m_pProcessProperties)
	{
		LOG_HRESULT_ERROR(E_OUTOFMEMORY, L"Out of memory allocating CPropertyManager object");
		return false;
	}

	if (!GatherAndLogCurrentModuleProperties())
	{
		LOG_BOOL_ERROR(false, L"GatherAndLogCurrentModuleProperties failed");
		return false;
	}

	if (!GatherAndLogProcessProperties())
	{
		LOG_BOOL_ERROR(false, L"GatherAndLogProcessProperties failed");
		return false;
	}

	//
	// Gather and log machine properties
	//

	m_pGeneralSystemProperties = new(std::nothrow) CPropertyManager();
	if (nullptr == m_pGeneralSystemProperties)
	{
		LOG_HRESULT_ERROR(E_OUTOFMEMORY, L"Out of memory allocating CPropertyManager object");
		return false;
	}

	if (!GatherAndLogSystemProperties())
	{
		LOG_BOOL_ERROR(false, L"GatherAndLogSystemProperties failed");
		return false;
	}

	//
	// Gather and log device properties
	//

	m_pGraphicsDeviceProperties = new(std::nothrow) CPropertyManager();
	if (nullptr == m_pGraphicsDeviceProperties)
	{
		LOG_HRESULT_ERROR(E_OUTOFMEMORY, L"Out of memory allocating CPropertyManager object");
		return false;
	}

	if (!GatherAndLogDeviceProperties())
	{
		LOG_BOOL_ERROR(false, L"GatherAndLogDeviceProperties failed");
		return false;
	}

	return Setup();
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestModuleBase::InternalCleanup()
////////////////////////////////////////////////////////////////
bool CCommonD3DTestModuleBase::InternalCleanup()
{
	bool rval = Cleanup();

	SAFE_RELEASE(m_pLogger);
	SAFE_DELETE(m_pDeviceManager);
	SAFE_DELETE(m_pSettingsManager);
	SAFE_DELETE(m_pProcessProperties);
	SAFE_DELETE(m_pParentProcessProperties);
	SAFE_DELETE(m_pGeneralSystemProperties);
	SAFE_DELETE(m_pGraphicsDeviceProperties);
	return rval;
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestModuleBase::InternalRegisterSettings()
////////////////////////////////////////////////////////////////
bool CCommonD3DTestModuleBase::InternalRegisterSettings(CPropertyManager *pPropertyManager)
{
	HRESULT hr = S_OK;

	// Allow the device manager to register properties with the settings manager
	hr = m_pDeviceManager->RegisterSettings(m_pSettingsManager);
	if (FAILED(hr))
	{
		LOG_HRESULT_ERROR(hr, L"CD3D12DeviceManager::RegisterSettings() failed");
		return false;
	}

	return RegisterSettings(pPropertyManager);
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestModuleBase::GatherAndLogCurrentModuleProperties()
////////////////////////////////////////////////////////////////
bool CCommonD3DTestModuleBase::GatherAndLogCurrentModuleProperties()
{
	// Local variables
	bool bSuccess = false;
	WCHAR fileName[MAX_PATH + 1] = { 0 };
	size_t fileNameLength = 0;

	//
	// Gather file properties
	//

	// Get the current module's name
	if (0 == GetModuleFileNameW(HMODULE_THISCOMPONENT, fileName, _countof(fileName)))
	{
		LOG_WIN32_ERROR(GetLastError(), L"GetModuleFileNameW failed");
		goto Cleanup;
	}

	// Get the length of the file name
	fileNameLength = wcsnlen_s(fileName, sizeof(fileName));

	// Check that the string is not empty
	if (fileNameLength == 0)
	{
		LOG_WARNING_MESSAGE(L"The current module's name is empty. Logged information will be related to the executable file of the current process and not the current module.");
	}

	// Check that the string is null terminated
	if (fileNameLength >= sizeof(fileName))
	{
		LOG_BOOL_ERROR(false, L"The current module's name wasn't null terminated.");
		goto Cleanup;
	}

	if (!CSystemProperties::GatherFileProperties(m_pProcessProperties, GetLogger(), tstring(fileName)))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GatherFileProperties failed");
		goto Cleanup;
	}

	//
	// Log file properties
	//

	if (!LogFileProperties(m_pProcessProperties, L"Current Test Module Information"))
	{
		LOG_BOOL_ERROR(false, L"LogFileProperties failed");
		goto Cleanup;
	}

	// If we got here, we passed
	bSuccess = true;

Cleanup:
	return bSuccess;
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestModuleBase::GatherAndLogSystemProperties()
////////////////////////////////////////////////////////////////
bool CCommonD3DTestModuleBase::GatherAndLogProcessProperties()
{
	// Local variables
	const DWORD currentPId = GetCurrentProcessId();
	DWORD parentPId = 0;
	HANDLE hSnapshot = nullptr;
	PROCESSENTRY32 pe32;
	bool bSuccess = false;

	//
	// Gather system properties
	//

	// Get the parent PID of our current process
	// Note we do this because we need the info for TE.exe

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		LOG_BOOL_ERROR(false, L"CreateToolhelp32Snapshot failed");
		return false;
	}

	ZeroMemory(&pe32, sizeof(pe32));
	pe32.dwSize = sizeof(pe32);
	if (!Process32First(hSnapshot, &pe32))
	{
		LOG_BOOL_ERROR(false, L"Process32First failed");
		return false;
	}

	// Find the parent process ID
	do
	{
		if (pe32.th32ProcessID == currentPId)
		{
			parentPId = pe32.th32ParentProcessID;
			break;
		}
	} while (Process32Next(hSnapshot, &pe32));

	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hSnapshot);
		hSnapshot = nullptr;
	}

	if (0 == parentPId)
	{
		LOG_BOOL_ERROR(false, L"Unable to find the parent process ID");
		return false;
	}

	// Log process properties for both the parent process and the current process...
	for (UINT i = 0; i < 2; i++)
	{
		// ...determine out PID, log header, and property bag to use (i = 0: current process, i = 1: parent process)
		const UINT currentPIdToLog = (i == 0) ? currentPId : parentPId;
		const tstring currentHeaderToLog = (i == 0) ? L"Current Process Information" : L"Parent Process Information";
		CPropertyManager *pCurrentProcessPropertiesToLog = (i == 0) ? m_pProcessProperties : m_pParentProcessProperties;

		// Get the properties from the process we are currently logging
		if (!CSystemProperties::GatherProcessProperties(pCurrentProcessPropertiesToLog, GetLogger(), currentPIdToLog))
		{
			LOG_BOOL_ERROR(false, L"CSystemProperties::GatherProcessProperties failed");
			goto Cleanup;
		}

		// Log the properties
		if (!LogProcessProperties(pCurrentProcessPropertiesToLog, currentHeaderToLog))
		{
			LOG_BOOL_ERROR(false, L"LogSystemProperties failed");
			goto Cleanup;
		}
	}

	// If we got here, we passed
	bSuccess = true;

Cleanup:
	return bSuccess;
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestModuleBase::GatherAndLogSystemProperties()
////////////////////////////////////////////////////////////////
bool CCommonD3DTestModuleBase::GatherAndLogSystemProperties()
{
	// Local variables
	bool bSuccess = false;

	//
	// Gather system properties
	//

	if (!CSystemProperties::GatherGeneralProperties(m_pGeneralSystemProperties, GetLogger()))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GatherGeneralProperties failed");
		goto Cleanup;
	}

	//
	// Log system properties
	//

	if (!LogSystemProperties(m_pGeneralSystemProperties, L"System Information"))
	{
		LOG_BOOL_ERROR(false, L"LogSystemProperties failed");
		goto Cleanup;
	}

	// If we got here, we passed
	bSuccess = true;

Cleanup:
	return bSuccess;
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestModuleBase::GatherAndLogDeviceProperties()
////////////////////////////////////////////////////////////////
bool CCommonD3DTestModuleBase::GatherAndLogDeviceProperties()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool bSuccess = false;
	IDXGIDevice *pDXGIDevice = nullptr;
	ID3D12Device *pD3D12Device = nullptr;

	//
	// Create a temporary device.
	// This device is only used for logging.
	//

	// Temporarily setup the device manager
	assert(m_pDeviceManager);
	m_pDeviceManager->GetD3D12Device(&pD3D12Device);
	assert(nullptr == pD3D12Device);

	// Create the D3D12 device
	hr = m_pDeviceManager->RefreshGraphics();
	if (FAILED(hr))
	{
		LOG_HRESULT_ERROR(hr, L"CD3D12DeviceManager::RefreshGraphics() failed");
		goto Cleanup;
	}

	//
	// Gather device properties
	//

	// Get the device from the temp device manager
	hr = m_pDeviceManager->GetDXGIDevice(&pDXGIDevice);
	if (FAILED(hr))
	{
		LOG_HRESULT_ERROR(hr, L"CD3D12DeviceManager::GetDXGIDevice() failed");
		goto Cleanup;
	}

	if (!CSystemProperties::GatherGraphicsDeviceProperties(m_pGraphicsDeviceProperties, GetLogger(), pDXGIDevice))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GatherGraphicsDeviceProperties failed");
		goto Cleanup;
	}

	//
	// Log device properties
	//

	if (!LogDeviceProperties(m_pGraphicsDeviceProperties, L"Graphics Device Information"))
	{
		LOG_BOOL_ERROR(false, L"LogDeviceProperties failed");
		goto Cleanup;
	}

	// If we got here, we passed
	bSuccess = true;

Cleanup:
	SAFE_RELEASE(pD3D12Device);
	SAFE_RELEASE(pDXGIDevice);
	m_pDeviceManager->ReleaseGraphics();

	return bSuccess;
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestModuleBase::LogFileProperties()
////////////////////////////////////////////////////////////////
bool CCommonD3DTestModuleBase::LogFileProperties(CPropertyManager *pFileProperties, const std::wstring &headerName)
{
	// Local variables
	bool bSuccess = false;
	tstring propertyValue;

	//
	// Log module properties
	//

	LogPropertySectionHeader(headerName);

	// Name
	if (!CSystemProperties::GetPropertyValue(pFileProperties, GetLogger(), PropertyNames::File::g_Name, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("Name: ") + propertyValue).c_str()));

	// Version
	if (!CSystemProperties::GetPropertyValue(pFileProperties, GetLogger(), PropertyNames::File::g_Version, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("Version: ") + propertyValue).c_str()));

	// Build type
	if (!CSystemProperties::GetPropertyValue(pFileProperties, GetLogger(), PropertyNames::File::g_BuildType, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("Build Type: ") + propertyValue).c_str()));

	// If we got here, we passed
	bSuccess = true;

Cleanup:
	return bSuccess;
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestModuleBase::LogProcessProperties()
////////////////////////////////////////////////////////////////
bool CCommonD3DTestModuleBase::LogProcessProperties(CPropertyManager *pProcessProperties, const std::wstring &headerName)
{
	// Local variables
	bool bSuccess = false;
	tstring propertyValue;

	//
	// Log process properties
	//

	LogPropertySectionHeader(headerName);

	// Executable path
	if (!CSystemProperties::GetPropertyValue(pProcessProperties, GetLogger(), PropertyNames::Process::g_ExecutablePath, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("Executable: ") + propertyValue).c_str()));

	// Command line
	if (!CSystemProperties::GetPropertyValue(pProcessProperties, GetLogger(), PropertyNames::Process::g_CommandLine, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("Command line: ") + propertyValue).c_str()));

	// Date and time
	if (!CSystemProperties::GetPropertyValue(pProcessProperties, GetLogger(), PropertyNames::Process::g_CreationDateLocalString, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("Start Time: ") + propertyValue).c_str()));

	// PID
	if (!CSystemProperties::GetPropertyValue(pProcessProperties, GetLogger(), PropertyNames::Process::g_ProcessId, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("Process ID: ") + propertyValue).c_str()));

	// Session ID
	if (!CSystemProperties::GetPropertyValue(pProcessProperties, GetLogger(), PropertyNames::Process::g_SessionId, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("Session ID: ") + propertyValue).c_str()));

	// If we got here, we passed
	bSuccess = true;

Cleanup:
	return bSuccess;
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestModuleBase::LogSystemProperties()
////////////////////////////////////////////////////////////////
bool CCommonD3DTestModuleBase::LogSystemProperties(CPropertyManager *pSystemProperties, const std::wstring &headerName)
{
	// Local variables
	bool bSuccess = false;
	tstring propertyValue = L"Not Set";
	tstring osArchitecture = L"Not Set";
	tstring osString = L"Not Set";
	tstring testAndOSArchitectureString = L"Not Set";
	tstring processorString = L"Not Set";
	tstring memoryString = L"Not Set";
	tstring systemModelString = L"Not Set";

	//
	// Log system properties
	//

	// Log the header
	LogPropertySectionHeader(headerName);

	// Machine vendor
	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_MachineVendor, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("Machine Vendor: ") + propertyValue).c_str()));

	// Machine model string
	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_MachineModelName, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	systemModelString = _T("Machine Model: ") + propertyValue + _T(" (Asset: ");

	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_MachineAssetTag, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	systemModelString += propertyValue + _T(", Serial: ");

	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_MachineSerialNumber, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	systemModelString += propertyValue + _T(")");

	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(systemModelString).c_str());

	// Machine name
	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_MachineName, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("Machine Name: ") + propertyValue).c_str()));

	// OS string
	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_OSProductName, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	osString = _T("OS: ") + propertyValue;

	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_OSArchitecture, osArchitecture))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	osString += _T(" ") + osArchitecture;

	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_OSBuild, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	osString += _T(" (") + propertyValue + _T(")");

	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(osString).c_str());

	// Test/OS architecture
	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_IsWoW64Process, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	if (_T("1") == propertyValue)
	{
		testAndOSArchitectureString = _T("WOW64(") + osArchitecture + _T(")");
	}
	else
	{
		testAndOSArchitectureString = osArchitecture;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("OS Architecture: ") + testAndOSArchitectureString).c_str()));

	// Language
	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_OSLanguage, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("OS Language: ") + propertyValue).c_str()));

	// Processor string
	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_ProcessorName, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	processorString = _T("Processor: ") + propertyValue;

	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_ProcessorCoreCount, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	processorString += _T(" (Cores: ") + propertyValue + _T(")");

	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(processorString).c_str());

	// Memory string
	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_SystemMemoryTotal, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	memoryString = _T("System Memory: ") + propertyValue + _T("MB total, ");

	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_SystemMemoryAvailable, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	memoryString += propertyValue + _T("MB available");

	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(memoryString).c_str());

	// User
	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_UserName, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("User: ") + propertyValue).c_str()));

	// User DPI
	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_UserDPI, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("User DPI: ") + propertyValue).c_str()));

	// System DPI
	if (!CSystemProperties::GetPropertyValue(pSystemProperties, GetLogger(), PropertyNames::General::g_SystemDPI, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(tstring(_T("System DPI: ") + propertyValue).c_str()));

	// If we got here, we passed
	bSuccess = true;

Cleanup:
	return bSuccess;
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestModuleBase::LogDeviceProperties()
////////////////////////////////////////////////////////////////
bool CCommonD3DTestModuleBase::LogDeviceProperties(CPropertyManager *pDeviceProperties, const std::wstring &headerName)
{
	// Local variables
	bool bSuccess = false;
	tstring propertyValue = L"Not Set";
	tstring descriptionString = L"Not Set";
	tstring memoryString = L"Not Set";

	//
	// Log device properties
	//

	// Log the header
	LogPropertySectionHeader(headerName);

	// Graphics device description
	if (!CSystemProperties::GetPropertyValue(pDeviceProperties, GetLogger(), PropertyNames::GraphicsDevice::g_Description, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	descriptionString = _T("Graphics Device: ") + propertyValue + _T(" (PnPID: ");

	if (!CSystemProperties::GetPropertyValue(pDeviceProperties, GetLogger(), PropertyNames::GraphicsDevice::g_PnPId, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	descriptionString += propertyValue + _T(")");
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(descriptionString).c_str());

	// Device memory
	if (!CSystemProperties::GetPropertyValue(pDeviceProperties, GetLogger(), PropertyNames::GraphicsDevice::g_DedicatedVideoMemoryMB, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	memoryString = _T("Graphics Device Memory: ") + propertyValue + _T(" MB device dedicated, ");

	if (!CSystemProperties::GetPropertyValue(pDeviceProperties, GetLogger(), PropertyNames::GraphicsDevice::g_DedicatedSystemMemoryMB, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	memoryString += propertyValue + _T(" MB system dedicated, ");

	if (!CSystemProperties::GetPropertyValue(pDeviceProperties, GetLogger(), PropertyNames::GraphicsDevice::g_DedicatedSystemMemoryMB, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	memoryString += propertyValue + _T(" MB system shared");
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(memoryString).c_str());

	// Device type
	if (!CSystemProperties::GetPropertyValue(pDeviceProperties, GetLogger(), PropertyNames::GraphicsDevice::g_Type, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(_T("Graphics Device Type: " + propertyValue)).c_str());

	// Driver version
	if (!CSystemProperties::GetPropertyValue(pDeviceProperties, GetLogger(), PropertyNames::GraphicsDevice::g_UMDDriverVersion, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(_T("Driver: ") + propertyValue).c_str());

	// Driver model
	if (!CSystemProperties::GetPropertyValue(pDeviceProperties, GetLogger(), PropertyNames::GraphicsDevice::g_KMTDriverModelString, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(_T("Driver Model: ") + propertyValue).c_str());

	// Driver binaries
	if (!CSystemProperties::GetPropertyValue(pDeviceProperties, GetLogger(), PropertyNames::GraphicsDevice::g_UMDDriverBinaries, propertyValue))
	{
		LOG_BOOL_ERROR(false, L"CSystemProperties::GetPropertyValue failed.");
		goto Cleanup;
	}
	LOG_INFORMATIONAL_MESSAGE(tstr2wstr(_T("Driver Binaries: ") + propertyValue).c_str());

	// If we got here, we passed
	bSuccess = true;

Cleanup:
	return bSuccess;
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestModuleBase::LogPropertySectionHeader()
////////////////////////////////////////////////////////////////
void CCommonD3DTestModuleBase::LogPropertySectionHeader(const std::wstring &headerName)
{
	std::wstring characterSeparatorString = L"";
	const std::wstring unnamedSectionHeader = L"Unnamed section";
	const std::wstring *pSectionName = &headerName;

	if (headerName.empty())
	{
		pSectionName = &unnamedSectionHeader;
	}

	for (UINT i = 0; i < pSectionName->size(); i++)
	{
		characterSeparatorString += L"-";
	}

	LOG_INFORMATIONAL_MESSAGE(characterSeparatorString.c_str());
	LOG_INFORMATIONAL_MESSAGE(pSectionName->c_str());
	LOG_INFORMATIONAL_MESSAGE(characterSeparatorString.c_str());
}

////////////////////////////////////////////////////////////////
//
// class CCommonD3DTestBase
//
////////////////////////////////////////////////////////////////
CCommonD3DTestBase::CCommonD3DTestBase() :
m_pCommonVariationData(nullptr),
m_pSettingsManager(nullptr),
m_pGraphicsDeviceProperties(nullptr)
{
}

CCommonD3DTestBase::~CCommonD3DTestBase()
{

}


////////////////////////////////////////////////////////////////
// CCommonD3DTestBase::CommonTestMethodSetup()
bool CCommonD3DTestBase::CommonTestMethodSetup()
{
	HRESULT hr = S_OK;
	CD3D12DeviceManager *pDeviceManager = CCommonD3DTestModuleBase::GetDeviceManager();

	assert(nullptr != pDeviceManager);

	// Refresh the current VariationCore parameter values
	if (m_pCommonVariationData)
	{
		hr = m_pCommonVariationData->RefreshParameterValues();

		if (FAILED(hr))
		{
			LOG_HRESULT_ERROR(hr, L"CVariationData::RefreshParameterValues() failed");
			return false;
		}
	}

	// Invoke initial test method setup
	if (!InitialTestMethodSetup())
	{
		return false;
	}

	if (pDeviceManager->IsRefreshNeeded())
	{
		// Device is about to be recycled so cleanup any existing D3D resources
		if (!OnDeviceInvalidated())
		{
			LOG_BOOL_ERROR(false, L"OnDeviceInvalidated() failed");
			return false;
		}
	}

	// Refresh the device manager
	hr = pDeviceManager->RefreshGraphics();
	if (FAILED(hr))
	{
		LOG_HRESULT_ERROR(hr, L"CD3D12DeviceManager::RefreshGraphics() failed");
		return false;
	}

	// Gather properties for the new device if necessary
	if (S_OK == hr)
	{
		IDXGIDevice *pDevice = nullptr;
		hr = pDeviceManager->GetDXGIDevice(&pDevice);
		if (FAILED(hr))
		{
			LOG_HRESULT_ERROR(hr, L"CD3D12DeviceManager::GetDXGIDevice() failed");
			return false;
		}

		// Clear existing graphics device properties and gather the new graphics device properties
		m_pGraphicsDeviceProperties->ClearProperties();
		if (!CSystemProperties::GatherGraphicsDeviceProperties(m_pGraphicsDeviceProperties, GetLogger(), pDevice))
		{
			LOG_BOOL_ERROR(false, L"CSystemProperties::GatherGraphicsDeviceProperties() failed");
			SAFE_RELEASE(pDevice);
			return false;
		}

		SAFE_RELEASE(pDevice);
	}

	if (S_OK == hr)
	{
		if (!OnDeviceCreated())
		{
			LOG_BOOL_ERROR(false, L"OnDeviceCreated() failed");
		}
	}

	// Perform final test method setup tasks
	return FinalTestMethodSetup();
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestBase::CommonTestMethodCleanup()
bool CCommonD3DTestBase::CommonTestMethodCleanup()
{
	return InitialTestMethodCleanup();
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestBase::CommonTestClassSetup()
bool CCommonD3DTestBase::CommonTestClassSetup()
{
	// Create the Logger object
	HRESULT hr = S_OK;

	if (!InitialTestClassSetup())
	{
		return false;
	}

	// Create the settings manager
	m_pSettingsManager = new(std::nothrow) CPropertyManager;
	if (nullptr == m_pSettingsManager)
	{
		LOG_BOOL_ERROR(FALSE, L"Out of memory allocating settings manager's CPropertyManager object");
		return false;
	}

	// Create the system properties manager
	m_pGraphicsDeviceProperties = new(std::nothrow) CPropertyManager;
	if (nullptr == m_pGraphicsDeviceProperties)
	{
		LOG_BOOL_ERROR(FALSE, L"Out of memory allocating graphics device properties' CPropertyManager object");
		return false;
	}

	// Register settings
	if (!InternalRegisterSettings(m_pSettingsManager))
	{
		LOG_BOOL_ERROR(FALSE, L"Error registering settings");
		return false;
	}

	//
	// Apply settings from runtime parameters
	//
	if (!ApplySettingsFromRuntimeParameters(m_pSettingsManager))
	{
		LOG_BOOL_ERROR(FALSE, L"Error applying settings from runtime parameters");
		return false;
	}

	//
	// Setup the device manager
	//
	CD3D12DeviceManager *pDeviceManager = CCommonD3DTestModuleBase::GetDeviceManager();
	assert(nullptr != pDeviceManager);
	pDeviceManager->Setup(GetLogger());

	// Perform additional class setup
	return FinalTestClassSetup();
}

////////////////////////////////////////////////////////////////
// CCommonD3DTestBase::CommonClassCleanup()
bool CCommonD3DTestBase::CommonTestClassCleanup()
{
	CD3D12DeviceManager *pDeviceManager = CCommonD3DTestModuleBase::GetDeviceManager();
	assert(nullptr != pDeviceManager);

	// First perform custom test class cleanup
	bool rval = InitialTestClassCleanup();

	// Cleanup device interfaces
	if (!OnDeviceInvalidated())
	{
		rval = false;
	}

	// Cleanup the device manager
	pDeviceManager->ReleaseGraphics();

	// Common cleanup tasks
	SAFE_RELEASE(m_pCommonVariationData);
	SAFE_DELETE(m_pSettingsManager);
	SAFE_DELETE(m_pGraphicsDeviceProperties);

	return rval;
}


////////////////////////////////////////////////////////////////
// CCommonD3DTestBase::InternalRegisterSettings()
////////////////////////////////////////////////////////////////
bool CCommonD3DTestBase::InternalRegisterSettings(CPropertyManager *pPropertyManager)
{
	return RegisterSettings(pPropertyManager);
}

////////////////////////////////////////////////////////////////
//
// GLOBAL FUNCTIONS
//
////////////////////////////////////////////////////////////////
bool __stdcall _D3DTestModuleCleanup(class CCommonD3DTestModuleBase *pTestModule)
{
	bool rval = pTestModule->InternalCleanup();
	SAFE_DELETE(pTestModule);

	return rval;
}
