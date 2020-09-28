////////////////////////////////////////////////////////////////
// CommonD3DTestBase.h
//
// Classes and macros for quickly implementing a TAEF test using
// common D3D12 testing utilities.
////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include "ISimpleLogController.h"
#include "VariationData.h"
#include "D3D12DeviceManager.h"
#include "SettingProperties.h"
#include "SafeMacros.h"
#include "WideMacros.h"

////////////////////////////////////////////////////////////////
//
// MACROS
//
////////////////////////////////////////////////////////////////
#ifndef HMODULE_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;	// Quick reference to the top of the containing binary
#define HMODULE_THISCOMPONENT ((HMODULE)&__ImageBase)
#endif

#define LOG_ERROR(errorType, errorCode, errorMessage) \
if (CCommonD3DTestModuleBase::GetCommonD3DTestModule()->GetLogger()) \
	{ \
	CCommonD3DTestModuleBase::GetLogger()->LogError(errorType, (UINT)errorCode, errorMessage, __FILEW__, __LINE__, __FUNCTION__); \
	}

#define LOG_WIN32_ERROR(errorCode, errorMessage) \
	LOG_ERROR(ERROR_TYPE_WIN32, (UINT)errorCode, errorMessage)

#define LOG_HRESULT_ERROR(errorCode, errorMessage) \
	LOG_ERROR(ERROR_TYPE_HRESULT, (UINT)errorCode, errorMessage)

#define LOG_BOOL_ERROR(errorCode, errorMessage) \
	LOG_ERROR(ERROR_TYPE_BOOL, (UINT)errorCode, errorMessage)

#define LOG_GENERAL_ERROR(errorMessage) \
	LOG_ERROR(ERROR_TYPE_BOOL, (UINT)FALSE, errorMessage)

#define LOG_MESSAGE(messageType, message) \
if (CCommonD3DTestModuleBase::GetLogger()) \
	{ \
	CCommonD3DTestModuleBase::GetLogger()->LogMessage(messageType, message); \
	}

#define LOG_INFORMATIONAL_MESSAGE(message) \
	LOG_MESSAGE(MESSAGE_TYPE_INFORMATIONAL, message)

#define LOG_WARNING_MESSAGE(message) \
	LOG_MESSAGE(MESSAGE_TYPE_WARNING, message)

#define LOG_VERBOSE_MESSAGE(message) \
	LOG_MESSAGE(MESSAGE_TYPE_VERBOSE, message)

#define LOG_DEBUG_MESSAGE(message) \
	LOG_MESSAGE(MESSAGE_TYPE_DEBUG, message)


#define USE_COMMON_D3D_TEST_CLASS_SETUP() \
	TEST_CLASS_SETUP(PrivateTestClassSetup) \
	{ \
	return CommonTestClassSetup(); \
	}

#define USE_COMMON_D3D_TEST_CLASS_CLEANUP() \
	TEST_CLASS_CLEANUP(PrivateTestClassCleanup) \
	{ \
	return CommonTestClassCleanup(); \
	}

#define USE_COMMON_D3D_TEST_METHOD_SETUP() \
	TEST_METHOD_SETUP(PrivateTestMethodSetup) \
	{ \
	return CommonTestMethodSetup(); \
	}

#define USE_COMMON_D3D_TEST_METHOD_CLEANUP() \
	TEST_METHOD_CLEANUP(PrivateTestMethodCleanup) \
	{ \
	return CommonTestMethodCleanup(); \
	}

// Logs an error and continues execution on failing HRESULT
#define LOG_ERROR_IF_HRESULT_FAILED(statement) \
	CCommonD3DTestModuleBase::LogErrorIfHResultFailed(statement, L"Failure calling "L#statement, __FILEW__, __LINE__, __FUNCTION__) \

// Logs an error, fail the test method, and returns void on failing HRESULT
#define LOG_FAIL_AND_RETURN_IF_HRESULT_FAILED(statement) \
if (FAILED(CCommonD3DTestModuleBase::LogFailIfHResultFailed(statement, L"Failure calling "L#statement, __FILEW__, __LINE__, __FUNCTION__))) \
	{ \
	return; \
	} \

// Logs an error, fail the test method, and returns boolean false on failing HRESULT
#define LOG_FAIL_AND_RETURN_FALSE_IF_HRESULT_FAILED(statement) \
if (FAILED(CCommonD3DTestModuleBase::LogFailIfHResultFailed(statement, L"Failure calling "L#statement, __FILEW__, __LINE__, __FUNCTION__))) \
	{ \
	return false; \
	} \

// Logs an error, fail the test method, and returns void if null
#define LOG_FAIL_AND_RETURN_IF_NULL_POINTER(statement) \
if ((statement) == nullptr) \
	{ \
	CCommonD3DTestModuleBase::LogFailWithMessage(L"Failing as the following is null: "L#statement, __FILEW__, __LINE__, __FUNCTION__); \
	return; \
	} \

// Logs an error, fail the test method, and returns false if null
#define LOG_FAIL_AND_RETURN_FALSE_IF_NULL_POINTER(statement) \
if ((statement) == nullptr) \
	{ \
	CCommonD3DTestModuleBase::LogFailWithMessage(L"Failing as the following is null: "L#statement, __FILEW__, __LINE__, __FUNCTION__); \
	return false; \
	} \

// Logs an error and fails the test method
#define LOG_FAIL_WITH_MESSAGE(message) \
	CCommonD3DTestModuleBase::LogFailWithMessage(message, __FILEW__, __LINE__, __FUNCTION__); \

// Logs an error and returns boolean false if statement is false
#define LOG_FAIL_AND_RETURN_FALSE_IF_FALSE(statement) \
if (!CCommonD3DTestModuleBase::LogFailIfFalse(statement, L"Failure calling "L#statement, __FILEW__, __LINE__, __FUNCTION__)) \
	{ \
	return false; \
	}

// Logs an error and returns boolean false if statement is false
#define LOG_FAIL_AND_RETURN_IF_FALSE(statement) \
if (!CCommonD3DTestModuleBase::LogFailIfFalse(statement, L"Failure calling "L#statement, __FILEW__, __LINE__, __FUNCTION__)) \
	{ \
	return; \
	}

// Logs an error if statement is false
#define LOG_FAIL_IF_FALSE(statement) \
	CCommonD3DTestModuleBase::LogFailIfFalse(statement, L"Failure calling "L#statement, __FILEW__, __LINE__, __FUNCTION__)

////////////////////////////////////////////////////////////////
// USE_COMMON_D3D_FIXTURES()
//
// Use this macro to enable common fixtures of the CCommonD3DtestBase class
#define USE_COMMON_D3D_TEST_CLASS_FIXTURES() \
	USE_COMMON_D3D_TEST_CLASS_SETUP(); \
	USE_COMMON_D3D_TEST_CLASS_CLEANUP(); \
	USE_COMMON_D3D_TEST_METHOD_SETUP(); \
	USE_COMMON_D3D_TEST_METHOD_CLEANUP();

#define USE_COMMON_D3D_TEST_MODULE_CLASS(_ClassName) \
	MODULE_SETUP(CommonD3DTestModuleSetup) \
	{ \
	return CCommonD3DTestModuleBase::Create<_ClassName>(); \
	} \
	MODULE_CLEANUP(CommonD3DTestModuleCleanup) \
	{ \
	bool rval = true; \
	CCommonD3DTestModuleBase *pModuleObject = CCommonD3DTestModuleBase::GetCommonD3DTestModule(); \
if (pModuleObject) \
		{ \
		rval = _D3DTestModuleCleanup(pModuleObject); \
		} \
		return rval; \
	}


////////////////////////////////////////////////////////////////
//
// CLASSES
//
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// class CCommonD3DTestModuleBase
//
// Derive from this class to use a common test module object in a D3D TAEF test.
// Provides overridable Setup() and Cleanup() methods for the module.  
//
// The macro USE_COMMON_D3D_TEST_MODULE_CLASS(derived-class-name) must be used
// to take advantage of this class.
//
// Effectively replaces the need for TAEF macros MODULE_SETUP() AND MODULE_CLEANUP().
class CCommonD3DTestModuleBase
{
public:
	CCommonD3DTestModuleBase();
	virtual ~CCommonD3DTestModuleBase();

	//
	// OVERRIDABLE METHODS
	//

	////////////////////////////////////////////////////////////////
	// Setup()
	//
	// Override this method to perform additional setup tasks for the test module
	virtual bool Setup() { return true; }

	////////////////////////////////////////////////////////////////
	// Cleanup()
	//
	// Override this method to perform additional cleanup tasks for the test module
	virtual bool Cleanup() { return true; }

	////////////////////////////////////////////////////////////////
	// RegisterSettings()
	//
	// Override this method to register additional setting properties for the module
	virtual bool RegisterSettings(CPropertyManager *pPropertyManager) { return true; }

	//
	// ACCESSORS
	//

	////////////////////////////////////////////////////////////////
	// GetSettingsManager()
	// 
	// Returns pointer to the property manager used to process command line settings
	CPropertyManager *GetSettingsManager() { return m_pSettingsManager; }

	//
	// STATIC METHODS
	//

	////////////////////////////////////////////////////////////////
	// GetLogger()
	//
	// Returns a pointer to the logger created by the singleton CCommonD3DTestModule 
	// object
	static ISimpleLogController *GetLogger() { return GetCommonD3DTestModule()->m_pLogger; }

	////////////////////////////////////////////////////////////////
	// GetDeviceManager()
	// 
	// Returns pointer to the module's CD3D12DeviceManager object
	static CD3D12DeviceManager *GetDeviceManager() { return GetCommonD3DTestModule()->m_pDeviceManager; }

	////////////////////////////////////////////////////////////////
	// GetCommonD3DTestModule()
	//
	// Returns pointer to the singleton CCommonD3DTestModuleBase object
	static CCommonD3DTestModuleBase *GetCommonD3DTestModule() { return s_pSingleton; }

	////////////////////////////////////////////////////////////////
	// LogErrorIfHResultFailed()
	//
	// Logs an error for a failing HRESULT and returns the failing HRESULT
	static HRESULT LogErrorIfHResultFailed(HRESULT hr, PCWSTR pMessage, PCWSTR pFile, UINT line, PCSTR pFunction)
	{
		if (FAILED(hr))
		{
			if (GetLogger())
			{
				GetLogger()->LogError(ERROR_TYPE_HRESULT, (UINT)hr, pMessage, pFile, line, pFunction); \
			}
		}

		return hr;
	}

	////////////////////////////////////////////////////////////////
	// LogFailIfHResultFailed()
	//
	// Logs an error and sets the log result to TESTCASE_RESULT_FAILED
	// for a failing HRESULT and returns the failing HRESULT
	static HRESULT LogFailIfHResultFailed(HRESULT hr, PCWSTR pMessage, PCWSTR pFile, UINT line, PCSTR pFunction)
	{
		if (FAILED(hr))
		{
			if (GetLogger())
			{
				GetLogger()->LogError(ERROR_TYPE_HRESULT, (UINT)hr, pMessage, pFile, line, pFunction); \
					GetLogger()->SetTestCaseResult(TESTCASE_RESULT_FAILED, L"TestMethod method failure"); \
			}
		}

		return hr;
	}

	////////////////////////////////////////////////////////////////
	// LogFailIfNull()
	//
	// Logs an error and sets the log result to TESTCASE_RESULT_FAILED
	// when passed a NULL ptr
	static void LogFailIfNull(void* pointer, PCWSTR pMessage, PCWSTR pFile, UINT line, PCSTR pFunction)
	{
		if (pointer == nullptr)
		{
			if (GetLogger())
			{
				GetLogger()->LogError(ERROR_TYPE_BOOL, FALSE, pMessage, pFile, line, pFunction); \
					GetLogger()->SetTestCaseResult(TESTCASE_RESULT_FAILED, L"TestMethod method failure"); \
			}
		}
	}

	////////////////////////////////////////////////////////////////
	// LogFailWithMessage()
	//
	// Logs an error and sets the log result to TESTCASE_RESULT_FAILED
	// 
	static void LogFailWithMessage(PCWSTR pMessage, PCWSTR pFile, UINT line, PCSTR pFunction)
	{
		if (GetLogger())
		{
			GetLogger()->LogError(ERROR_TYPE_BOOL, FALSE, pMessage, pFile, line, pFunction); \
				GetLogger()->SetTestCaseResult(TESTCASE_RESULT_FAILED, L"TestMethod method failure"); \
		}
	}

	////////////////////////////////////////////////////////////////
	// LogFailIfFalse()
	//
	// Logs an error and sets the log result to TESTCASE_RESULT_FAILED
	// for a false input value and returns the value
	static bool LogFailIfFalse(bool value, PCWSTR pMessage, PCWSTR pFile, UINT line, PCSTR pFunction)
	{
		if (!value)
		{
			if (GetLogger())
			{
				GetLogger()->LogError(ERROR_TYPE_BOOL, (UINT)value, pMessage, pFile, line, pFunction); \
					GetLogger()->SetTestCaseResult(TESTCASE_RESULT_FAILED, L"TestMethod method failure"); \
			}
		}

		return value;
	}

	//
	// TEMPLATE METHODS
	//

	////////////////////////////////////////////////////////////////
	// Create()
	//
	// Called by the USE_COMMON_D3D_TEST_MODULE_CLASS() macro.  Do not call this
	// template method directly.
	template<class _DerivedClass>
	static bool Create()
	{
		assert(s_pSingleton == nullptr);

		if (s_pSingleton != nullptr)
		{
			return false;
		}

		// Allocate the test module class object
		s_pSingleton = new _DerivedClass();

		// Setup the module object
		return s_pSingleton->InternalSetup();
	}

	//
	// INTERNAL METHODS
	//
private:

	////////////////////////////////////////////////////////////////
	// InternalSetup()
	// 
	// Called by the USE_COMMON_D3D_TEST_MODULE_CLASS() macro.  Do not call this
	// method directly.
	bool InternalSetup();

	////////////////////////////////////////////////////////////////
	// InternalCleanup()
	// 
	// Called by the USE_COMMON_D3D_TEST_MODULE_CLASS() macro.  Do not call this
	// method directly.
	bool InternalCleanup();

	////////////////////////////////////////////////////////////////
	// InternalRegisterSettings()
	// 
	// Configures the settings CPropertyManager object for the test module.
	// Calls the overridable RegisterSettings() method to give the test author
	// the ability to register custom settings for the test.
	bool InternalRegisterSettings(CPropertyManager *pPropertyManager);

	////////////////////////////////////////////////////////////////
	// GatherAndLogCurrentModuleProperties()
	// 
	// Gathers and sends the current test module properties to the logger
	bool GatherAndLogCurrentModuleProperties();

	////////////////////////////////////////////////////////////////
	// GatherAndLogProcessProperties()
	// 
	// Gathers and sends the current and parent process properties to the logger
	bool GatherAndLogProcessProperties();

	////////////////////////////////////////////////////////////////
	// GatherAndLogSystemProperties()
	// 
	// Gathers and sends the current system properties to the logger
	bool GatherAndLogSystemProperties();

	////////////////////////////////////////////////////////////////
	// GatherAndLogDeviceProperties()
	// 
	// Gathers and sends the current graphics device properties to the logger
	bool GatherAndLogDeviceProperties();

	////////////////////////////////////////////////////////////////
	// LogModuleProperties()
	// 
	// Sends the specified module properties to the logger
	bool LogFileProperties(CPropertyManager *pFileProperties, const std::wstring &headerName);

	////////////////////////////////////////////////////////////////
	// LogProcessProperties()
	// 
	// Sends the specified process properties to the logger
	bool LogProcessProperties(CPropertyManager *pProcessProperties, const std::wstring &headerName);

	////////////////////////////////////////////////////////////////
	// LogSystemProperties()
	// 
	// Sends the specified system properties to the logger
	bool LogSystemProperties(CPropertyManager *pSystemProperties, const std::wstring &headerName);

	////////////////////////////////////////////////////////////////
	// LogDeviceProperties()
	// 
	// Sends the specified graphics device properties to the logger
	bool LogDeviceProperties(CPropertyManager *pDeviceProperties, const std::wstring &headerName);

	////////////////////////////////////////////////////////////////
	// LogPropertySectionHeader()
	// 
	// Sends a section header to the logger
	void LogPropertySectionHeader(const std::wstring &headerName);

private:
	static CCommonD3DTestModuleBase *s_pSingleton;
	CD3D12DeviceManager *m_pDeviceManager;

	CPropertyManager *m_pSettingsManager;
	CPropertyManager *m_pProcessProperties;
	CPropertyManager *m_pParentProcessProperties;
	CPropertyManager *m_pGeneralSystemProperties;
	CPropertyManager *m_pGraphicsDeviceProperties;
	ISimpleLogController *m_pLogger;

	friend bool __stdcall _D3DTestModuleCleanup(class CCommonD3DTestModuleBase *pTestModule);
};

////////////////////////////////////////////////////////////////
// class CCommonD3DTestBase
//
// Base class for common D3D12 testing
class CCommonD3DTestBase
{
public:
	CCommonD3DTestBase();
	virtual ~CCommonD3DTestBase();

public:
	//
	// FIXTURES
	//

	////////////////////////////////////////////////////////////////
	// CommonTestMethodSetup()
	//
	// When using common fixtures this method performs necessary setup
	// before each test method.
	//
	// After completing common tasks this method invokes 
	// the inheritable TestSetup() method for custom setup tasks.
	bool CommonTestMethodSetup();

	////////////////////////////////////////////////////////////////
	// CommonTestMethodCleanup()
	//
	// When using common fixtures this method performs necessary cleanup
	// after each test method.
	//
	// Before completing common cleanup tasks this method invokes 
	// the inheritable TestMethodCleanup() method for custom cleanup tasks.
	bool CommonTestMethodCleanup();

	////////////////////////////////////////////////////////////////
	// CommonTestClassSetup()
	//
	// When using common fixtures this method performs necessary setup
	// for each test class instance.
	//
	// After completing common tasks this method invokes 
	// the inheritable TestSetup() method for custom setup tasks.
	bool CommonTestClassSetup();

	////////////////////////////////////////////////////////////////
	// CommonTestClassCleanup()
	//
	// When using common fixtures this method performs necessary cleanup
	// for each test class instance.
	//
	// Before completing common cleanup tasks this method invokes 
	// the inheritable ClassCleanup() method for custom cleanup tasks.
	bool CommonTestClassCleanup();

	//
	// OVERRIDABLE METHODS
	//

	////////////////////////////////////////////////////////////////
	// InitialTestMethodSetup()
	// 
	// Override this method to perform initial (pre-common) setup tasks for each test method.
	// This is a good place to update the device manager properties.
	// 
	// NOTE: Avoid creating D3D resources in this method.  It is possible for this test
	// infrastructure to destroy the current device and create a new one after InitialTestMethodSetup().
	// Please use FinalTestMethodSetup() or OnDeviceCreated() instead.
	virtual bool InitialTestMethodSetup() { return true; }

	////////////////////////////////////////////////////////////////
	// FinalTestMethodSetup()
	// 
	// Override this method to perform final (post-common) setup tasks for each test method.
	virtual bool FinalTestMethodSetup() { return true; }

	////////////////////////////////////////////////////////////////
	// InitialTestMethodCleanup()
	// 
	// Override this method to perform initial (pre-common) cleanup tasks for each test method
	virtual bool InitialTestMethodCleanup() { return true; }

	////////////////////////////////////////////////////////////////
	// InitialTestClassSetup()
	// 
	// Override this method to perform initial (pre-common) setup tasks for the test class instance.
	// This method is called before any base setup tasks including logger creation and device manager
	// setup.
	//
	// NOTE: Avoid creating D3D resources in this method.  Please use OnDeviceCreated() instead.
	virtual bool InitialTestClassSetup() { return true; }

	////////////////////////////////////////////////////////////////
	// FinalTestClassSetup()
	// 
	// Override this method to perform final (post-common) setup tasks for the test class instance.
	// This is a good place to initialize the device manager properties.
	//
	// NOTE: Avoid creating D3D resources in this method.  Please use OnDeviceCreated() instead.
	virtual bool FinalTestClassSetup() { return true; }

	////////////////////////////////////////////////////////////////
	// InitialTestClassCleanup()
	// 
	// Override this method to perform initial (pre-common) cleanup tasks for the test class instance
	virtual bool InitialTestClassCleanup() { return true; }

	////////////////////////////////////////////////////////////////
	// OnDeviceCreated()
	//
	// Called when the device manager has created a new device.  Override this method
	// to create class-level D3D resources.
	//
	// This is the safest place to create D3D resources that persist across test cases.  Whenever
	// the D3D device is recycled it is important to cleanup stale interfaces and create new new ones
	// using the updated device.
	virtual bool OnDeviceCreated() { return true; }

	////////////////////////////////////////////////////////////////
	// OnDeviceInvalid()
	//
	// Called when the device manager device is no longer valid and is about to be
	// released.  Override this method to cleanup class-level D3D resources.
	//
	// This is the safest place to release D3D resources that persist across test cases.  Whenever
	// the D3D device is recycled it is important to cleanup stale interfaces and create new new ones
	// using the updated device.
	virtual bool OnDeviceInvalidated() { return true; }

	////////////////////////////////////////////////////////////////
	// RegisterSettings()
	//
	// Override this method to register additional setting properties for the test class
	virtual bool RegisterSettings(CPropertyManager *pPropertyManager) { return true; }


	//
	// ACCESSORS
	//
	ISimpleLogController *GetLogger() { return CCommonD3DTestModuleBase::GetCommonD3DTestModule()->GetLogger(); }
	CD3D12DeviceManager *GetDeviceManager() { return CCommonD3DTestModuleBase::GetDeviceManager(); }

	void SetVariationData(IVariationData *pVariationData)
	{
		m_pCommonVariationData = pVariationData;
		pVariationData->AddRef();
	}

	void GetVariationData(IVariationData **ppVariationData)
	{
		if (ppVariationData)
		{
			if (m_pCommonVariationData)
			{
				m_pCommonVariationData->AddRef();
			}

			*ppVariationData = m_pCommonVariationData;
		}
	}

	//
	// PRIVATE METHODS
	//
private:

	////////////////////////////////////////////////////////////////
	// InternalRegisterSettings()
	// 
	// Configures the settings CPropertyManager object for the test class.
	// Calls the overridable RegisterSettings() method to give the test author
	// the ability to register custom settings for the test class.
	bool InternalRegisterSettings(CPropertyManager *pPropertyManager);

private:
	IVariationData *m_pCommonVariationData;
	CPropertyManager *m_pSettingsManager;
	CPropertyManager *m_pGraphicsDeviceProperties;
};


////////////////////////////////////////////////////////////////
//
// GLOBAL FUNCTIONS
//
////////////////////////////////////////////////////////////////
extern bool __stdcall _D3DTestModuleCleanup(class CCommonD3DTestModuleBase *pTestModule);
