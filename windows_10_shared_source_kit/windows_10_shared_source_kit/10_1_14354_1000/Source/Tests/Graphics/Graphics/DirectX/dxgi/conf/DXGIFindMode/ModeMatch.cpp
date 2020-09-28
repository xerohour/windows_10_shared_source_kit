//
// Win32 headers
//

#include <windows.h>
#include <basetsd.h>


//
// Direct3D headers
//

#include <initguid.h>
#include <d3d10_1.h>
#include <dxgi.h>
#include <d3dx10.h>


//
// C runtime headers
//

#include <stdio.h>


//
// Framework headers
//

#include <WGFTestCore.h>
#include <WGFTestUI.h>
#include <DXGITest.h>


//
// Project headers
//

//#include "GetDeviceInfo9.h"
#include "D3D10RefUtil.h"

#include "DXGIValueSets.h"

#include "DXGIScanoutFormats.h"


//
// StrSafe.h has to come last, because it deprecates functions
// that it doesn't like.
//

#include <StrSafe.h>


#undef MSG_LEN
#define MSG_LEN     1024

////////////////////////////////////////////////////////////////////////////////
// Test class definitions

class CMyTest
:
	public CDXGITest,
	public clsD3D10Ref
{
	//
	// Construction/destruction
	//

	public:
		CMyTest()
		:
			pOutput(NULL),
			pDeviceUnknownInterface(NULL)
		{
		};

		virtual ~CMyTest();


	protected:
		int iFormatArrayIndex;
		int iDimensionArrayIndex;
		int iRefreshRateIndex;
		IDXGIOutput *pOutput;
		IUnknown *pDeviceUnknownInterface;

		TEST_RESULT  CreateObjects();
		virtual TEST_RESULT  ObtainOutput() = 0;


	public:
		static CONST DXGI_FORMAT TestedFormats[];
		static CONST UINT cuiNoOfTestedFormats;

		typedef struct _MODE_DIMENSIONS
		{
			UINT uiWidth;
			UINT uiHeight;
		} MODE_DIMENSIONS;

		static CONST MODE_DIMENSIONS TestedModeDimensions[];
		static CONST UINT cuiNoOfTestedModeDimensions;

		static CONST UINT TestedRefreshRates[];
		static CONST UINT cuiNoOfTestedRefreshRates;

		TEST_RESULT ExecuteTestCase();

		void InitTestParameters()
		{
			//
			// Call the base class implementation.
			//

			CDXGITest::InitTestParameters();


			tstring strPath = GetPath().ToString();

			AddParameter(_T( "Format" ), &iFormatArrayIndex, RangeValueSet(0, (int)(cuiNoOfTestedFormats - 1), 1));
			AddParameter(_T( "Dimensions" ), &iDimensionArrayIndex, RangeValueSet(0, (int)(cuiNoOfTestedModeDimensions - 1), 1));
			AddParameter(_T( "RefreshRate" ), &iRefreshRateIndex, RangeValueSet(0, (int)(cuiNoOfTestedRefreshRates - 1), 1));
		};

		bool VerifyClosestMatch(CONST DXGI_MODE_DESC *pRequestedMode, CONST DXGI_MODE_DESC *pReturnedMode);
		bool VerifyNoMatchingMode(CONST DXGI_MODE_DESC *pRequestedMode);
};


class clsModeMatchNoDevice
:
	public CMyTest
{
	//
	// Construction/destruction
	//

	public:
		clsModeMatchNoDevice() {};

		virtual ~clsModeMatchNoDevice();


	protected:
		virtual TEST_RESULT  ObtainOutput();
};


class clsModeMatchD3D10Device
:
	public CMyTest
{
	//
	// Construction/destruction
	//

	public:
		clsModeMatchD3D10Device() {};

		virtual ~clsModeMatchD3D10Device();


	protected:
		virtual TEST_RESULT  ObtainOutput();
};



//////////////////////////////////////////////////////////////////////////
// CMyTestApp - An instance of a single object derived from CWGFTestFramework
// is required by the framework.
//////////////////////////////////////////////////////////////////////////
class CMyTestApp
:
	public CDXGITestFramework
{
	//
	// Construction/destruction
	//

	public:
		CMyTestApp()
		{
		};


	//
	// Framework override methods
	//

	public:
		virtual TESTTYPE GetTestType() const
		{
			return TESTTYPE_CONF;
		};

	
		bool InitTestGroups()
		{
			// Register test classes with the group manager.

			if (!ADD_TEST(_T("ModeMatchNoDevice"), clsModeMatchNoDevice))
				return false;

			if (!ADD_TEST(_T("ModeMatchD3D10Device"), clsModeMatchD3D10Device))
				return false;

			return true;
		};

		virtual void InitOptionVariables()
		{
			// Call the base class implementation.
			CDXGITestFramework::InitOptionVariables();
		};
};


extern CMyTestApp g_Application;


////////////////////////////////////////////////////////////////////////////////
// Static variable definitions


CONST DXGI_FORMAT CMyTest::TestedFormats[] =
{
    DXGI_FORMAT_B8G8R8A8_TYPELESS,
    DXGI_FORMAT_B8G8R8X8_TYPELESS,
	DXGI_FORMAT_R8G8B8A8_TYPELESS,
	DXGI_FORMAT_R32G32B32A32_TYPELESS,
	DXGI_FORMAT_R32G32B32_TYPELESS,
	DXGI_FORMAT_R16G16B16A16_TYPELESS,
	DXGI_FORMAT_R10G10B10A2_TYPELESS,
};

CONST CMyTest::MODE_DIMENSIONS CMyTest::TestedModeDimensions[] =
{
	{800, 600},
	{1024, 769},
	{1152, 864},
	{1280, 720},
	{1280, 768},
	{1280, 800},
	{1280, 960},
	{1280, 1024},
	{1360, 768},
	{1600, 900},
	{1600, 1024},
	{1600, 1200},
	{1800, 1440}
};

CONST UINT CMyTest::TestedRefreshRates[] = {60, 70, 72, 75, 85, 100, 120};

CONST UINT CMyTest::cuiNoOfTestedFormats = sizeof(CMyTest::TestedFormats) / sizeof(CMyTest::TestedFormats[0]);
CONST UINT CMyTest::cuiNoOfTestedModeDimensions = (sizeof(CMyTest::TestedModeDimensions) / sizeof(CMyTest::TestedModeDimensions[0]));
CONST UINT CMyTest::cuiNoOfTestedRefreshRates = (sizeof(CMyTest::TestedRefreshRates) / sizeof(CMyTest::TestedRefreshRates[0]));



////////////////////////////////////////////////////////////////////////////////
// CMyTest (base class) Methods

CMyTest::~CMyTest()
{
	if (pOutput)
		pOutput -> Release();

	if (pDeviceUnknownInterface)
		pDeviceUnknownInterface -> Release();
}

TEST_RESULT  CMyTest::CreateObjects()
{
	TEST_RESULT     test_result = RESULT_PASS;   
	HRESULT         hr = S_OK;
	bool            bReturn = false;
	char            szMsg[MSG_LEN] = {0};    
	ID3D10Device   *pD3D10Device = NULL;


	if (pOutput)
	{
		pOutput -> Release();
		pOutput = NULL;
	}

	if (pDeviceUnknownInterface)
	{
		pDeviceUnknownInterface -> Release();
		pDeviceUnknownInterface = NULL;
	}

   	hr = g_Application.CreateDevice
    (
	    NULL,
	    g_Application.m_D3DOptions.DriverType,
	    NULL,
	    D3D10_CREATE_DEVICE_SINGLETHREADED,
	    m_TestFeatureLevel,
	    0,
	    &pD3D10Device
    );

	if
	(
		hr == DXGI_ERROR_UNSUPPORTED ||
		hr == E_NOINTERFACE ||
		hr == E_NOTIMPL
	)
	{
		test_result = RESULT_SKIP;
		StringCchPrintf( szMsg, sizeof(szMsg), "Driver does not support Direct3D 10.  Skipping this test group." );
		WriteToLog( szMsg );

		StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  SKIPPED: CreateDevice returned hr = 0x%08x\n", __FILE__, __LINE__, hr );
		DebugOut.Write( szMsg );
		WriteToLog( szMsg );
	}
	else if ( FAILED( hr ) )
	{
		test_result = RESULT_FAIL;
		StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  FAILED: CreateDevice returned hr = 0x%08x\n", __FILE__, __LINE__, hr );
		DebugOut.Write( szMsg );
		WriteToLog( szMsg );
	}
	else
	{
		pDeviceUnknownInterface = (IUnknown *)pD3D10Device;
	}

	if (pDeviceUnknownInterface)
	{
		IDXGIAdapter *pDeviceAdapter = NULL;

		{
			IDXGIDevice *pDXGIDeviceInterface = NULL;

			if (FAILED((hr = pDeviceUnknownInterface -> QueryInterface(__uuidof(IDXGIDevice), (void **)(&pDXGIDeviceInterface)))))
			{
				test_result = RESULT_FAIL;
				StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  FAILED: QueryInterface for IDXGIDevice failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
				DebugOut.Write( szMsg );
				WriteToLog( szMsg );
			}
			else
			{
				if (FAILED((hr = pDXGIDeviceInterface -> GetAdapter(&pDeviceAdapter))))
				{
					test_result = RESULT_FAIL;
					StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  FAILED: IDXGIDevice::GetAdapter failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
					DebugOut.Write( szMsg );
					WriteToLog( szMsg );
				}

				pDXGIDeviceInterface -> Release();
			}
		}

		if (pDeviceAdapter)
		{
			if (FAILED((hr = pDeviceAdapter -> EnumOutputs(0, &pOutput))))
			{
				test_result = RESULT_FAIL;
				StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  FAILED: IDXGIAdapter::EnumOutputs failed - hr = 0x%08x\n", __FILE__, __LINE__, hr );
				DebugOut.Write( szMsg );
				WriteToLog( szMsg );
			}

			pDeviceAdapter -> Release();
		}
	}

	return test_result;
}

TEST_RESULT CMyTest::ExecuteTestCase()
{
	TEST_RESULT     test_result = RESULT_FAIL;
	HRESULT         hresult = S_OK;

	test_result = ObtainOutput();

	if ( test_result == RESULT_PASS )
	{
		DXGI_MODE_DESC ModeDesc, ModeDescMatched;
		// Compose a description of the tested mode, based on the test case
		// parameters.

		//typedef struct DXGI_MODE_DESC
		//    {
		//    UINT Width;
		//    UINT Height;
		//    float RefreshRate;
		//    DXGI_FORMAT Format;
		//    } 	DXGI_MODE_DESC;

		ModeDesc.Width = (TestedModeDimensions[iDimensionArrayIndex]).uiWidth;
		ModeDesc.Height = (TestedModeDimensions[iDimensionArrayIndex]).uiHeight;
		(ModeDesc.RefreshRate).Numerator = (TestedRefreshRates[iRefreshRateIndex]);
		(ModeDesc.RefreshRate).Denominator = 1;
		ModeDesc.Format = TestedFormats[iFormatArrayIndex];
		ModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		ModeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		/*
		DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
		DXGI_MODE_SCALING Scaling;
		*/
		// 

		DebugOut.Write("About to call FindClosestMatchingMode.\n");

		if (SUCCEEDED((hresult = pOutput -> FindClosestMatchingMode(&ModeDesc, &ModeDescMatched, pDeviceUnknownInterface))))
		{
			test_result = VerifyClosestMatch(&ModeDesc, &ModeDescMatched) ? RESULT_PASS: RESULT_FAIL;
		}
		else
		{
			WriteToLog("FindClosestMatchingMode failed.\n");

			test_result = RESULT_PASS;

			if ( !pDeviceUnknownInterface || clsScanOutFormats::CheckScanOutFormat(ModeDesc.Format) )
			{
				// FindClosestMatchMode failed.  Verify that none of the output's
				// enumerated modes constitute valid matches.
				if ( !VerifyNoMatchingMode(&ModeDesc) )
				{
					test_result = RESULT_FAIL;
				}
			}
		}
	}

	return test_result;
}

template<class T> class CMyAutoPtr
{
public:
	CMyAutoPtr(T * pp):p(pp){}
	~CMyAutoPtr(){delete[] p;}
	T * p;
};

bool CMyTest::VerifyClosestMatch(CONST DXGI_MODE_DESC *pRequestedMode, CONST DXGI_MODE_DESC *pReturnedMode)
{
	HRESULT hresult;

	DebugOut.Write("Entering VerifyClosestMatch.\n");

	// Enumerate the modes supported by the output and return 'false' if one of these matches
	// the requested mode more closely than does the returned mode.

	// Determine the BPP count for the requested mode's format.

	const DXGIFormatInfo *pRequestedModeFormatInfo = NULL;
	if (!(pRequestedModeFormatInfo = CFormatInfoMap::Find(pRequestedMode -> Format)))
	{
		WriteToLog("CFormatInfoMap::Find failed to match format value %d.\n", (UINT)(pRequestedMode -> Format));
		return false;
	}
	UINT uiRequestedFormatBPP = (UINT)(pRequestedModeFormatInfo -> uBitsPerElement);

	// Determine the BPP count for the returned mode's format.

	const DXGIFormatInfo *pReturnedModeFormatInfo = NULL;
	if (!(pReturnedModeFormatInfo = CFormatInfoMap::Find(pReturnedMode -> Format)))
	{
		WriteToLog("CFormatInfoMap::Find failed to match format value %d.\n", (UINT)(pReturnedMode -> Format));
		return false;
	}
	UINT uiReturnedFormatBPP = (UINT)(pReturnedModeFormatInfo -> uBitsPerElement);

	WriteToLog("\nMode Comparison:\n");
	WriteToLog
	(
		"Requested Mode: %d x %d x %dbpp at %dHz (%s)\n",
		pRequestedMode -> Width,
		pRequestedMode -> Height,
		uiRequestedFormatBPP,
		(UINT)((float)((pRequestedMode -> RefreshRate).Numerator) /
		(float)((pRequestedMode -> RefreshRate).Denominator)),
		pRequestedModeFormatInfo -> szDesc
	);
	WriteToLog
	(
		"Returned Mode: %d x %d x %dbpp at %dHz (%s)\n",
		pReturnedMode -> Width,
		pReturnedMode -> Height,
		uiReturnedFormatBPP,
		(UINT)((float)((pReturnedMode -> RefreshRate).Numerator) /
		(float)((pReturnedMode -> RefreshRate).Denominator)),
		pReturnedModeFormatInfo -> szDesc
	);
	WriteToLog("Comparing enumerated modes:\n");

	// Confine the enumeration to modes whose formats have a BPP count greater than
	// or equal to that of the requested mode, since only these can potentially be deemed
	// better matches.

	UINT uiEnumModeFormatIndex;
	const UINT cuiNoOfFormats = (UINT)(CFormatInfoMap::GetDXGIFormatInfoTableSize());

	for (uiEnumModeFormatIndex = 0; uiEnumModeFormatIndex < cuiNoOfFormats; ++uiEnumModeFormatIndex)
	{
		const DXGIFormatInfo &CurrentFormat = CFormatInfoMap::GetDXGIFormatInfo(uiEnumModeFormatIndex);

		// If NULL was passed for the pConcernedDevice argument, confine the search to modes
		// whose formats match precisely that of the requested mode.

		if (pDeviceUnknownInterface || (CurrentFormat.Format == pRequestedMode -> Format))
		{
			UINT uiEnumeratedBPP = CurrentFormat.uBitsPerElement;

			if (uiEnumeratedBPP >= uiRequestedFormatBPP)
			{
				UINT NumModes;

				DebugOut.Write("About to call GetDisplayModeList (1st call).\n");


				if (FAILED((hresult = pOutput -> GetDisplayModeList(CurrentFormat.Format, 0, &NumModes, 0))))
					WriteToLog("Warning: GetDisplayModeList (1st call) failed.  Format: %s.\n", CurrentFormat.szDesc);
				else
				if (NumModes)
				{
					DebugOut.Write("About to instantiate CMyAutoPtr.\n");

					CMyAutoPtr<DXGI_MODE_DESC> pModes(new DXGI_MODE_DESC[NumModes]);

					if (!pModes.p)
						return false;

					DebugOut.Write("About to call GetDisplayModeList (2nd call).\n");

					if (FAILED((hresult = pOutput -> GetDisplayModeList(CurrentFormat.Format, 0, &NumModes, pModes.p))))
						WriteToLog("Warning: GetDisplayModeList (2nd call) failed.  Format: %s.\n", CurrentFormat.szDesc);
					else
					{
						// Scan enumerated modes.
						UINT uiModeOrdinal;

						for (uiModeOrdinal = 0; uiModeOrdinal < NumModes; ++uiModeOrdinal)
						{
							DXGI_MODE_DESC EnumeratedMode;

							EnumeratedMode = pModes.p[uiModeOrdinal];

							WriteToLog
							(
								"Enumerated Mode: %d x %d x %dbpp at %dHz (%s)\n",
								EnumeratedMode.Width,
								EnumeratedMode.Height,
								uiEnumeratedBPP,
								(UINT)((float)((EnumeratedMode.RefreshRate).Numerator) /
								(float)((EnumeratedMode.RefreshRate).Denominator)),
								CurrentFormat.szDesc
							);

							// First determine whether the enumerated mode is "at least as good as" the
							// requested mode.

							if ((EnumeratedMode.Width >= pRequestedMode -> Width) &&
								(EnumeratedMode.Height >= pRequestedMode -> Height) &&
								(((EnumeratedMode.RefreshRate).Numerator * (pRequestedMode -> RefreshRate).Denominator) >=
								((EnumeratedMode.RefreshRate).Denominator * (pRequestedMode -> RefreshRate).Numerator))
								)
							{
								// Report a failure if the enumerated mode has a smaller BPP than
								// the returned mode.

								if (uiEnumeratedBPP < uiReturnedFormatBPP)
								{
									WriteToLog("An enumerated mode matches BPP more closely.\n");
									return false;
								}

								// Report a failure if the enumerated mode as the same BPP and a
								// better match on dimensions.
								if ((uiEnumeratedBPP == uiReturnedFormatBPP) && (EnumeratedMode.Width < pReturnedMode -> Width))
								{
									WriteToLog("An enumerated mode matches dimensions more closely.\n");
									return false;
								}

								if ((uiEnumeratedBPP == uiReturnedFormatBPP) && (EnumeratedMode.Width == pReturnedMode -> Width) && (EnumeratedMode.Height < pReturnedMode -> Height))
								{
									WriteToLog("An enumerated mode matches dimensions more closely.\n");
									return false;
								}

								if ((uiEnumeratedBPP == uiReturnedFormatBPP) && (EnumeratedMode.Width == pReturnedMode -> Width) &&
									(EnumeratedMode.Height == pReturnedMode -> Height) &&
									(((EnumeratedMode.RefreshRate).Numerator * (pReturnedMode -> RefreshRate).Denominator) <
									((EnumeratedMode.RefreshRate).Denominator * (pReturnedMode -> RefreshRate).Numerator))
									)
								{
									WriteToLog("An enumerated mode matches refresh rate more closely.\n");
									return false;
								}
							}
						}
					}
				}
			}
		}
	}

	return true;
}

bool CMyTest::VerifyNoMatchingMode(CONST DXGI_MODE_DESC *pRequestedMode)
{
	// Verify that no mode enumerated by the output is equal to or better than the requested mode.

	HRESULT hresult;

	if (pOutput)
	{
		// Determine the BPP count for the requested mode's format.

		const DXGIFormatInfo *pRequestedModeFormatInfo = NULL;
		if (!(pRequestedModeFormatInfo = CFormatInfoMap::Find(pRequestedMode -> Format)))
		{
			WriteToLog("CFormatInfoMap::Find failed to match format value %d.\n", (UINT)(pRequestedMode -> Format));
			return false;
		}
		UINT uiRequestedFormatBPP = (UINT)(pRequestedModeFormatInfo -> uBitsPerElement);

		WriteToLog
		(
			"Requested Mode: %d x %d x %dbpp at %dHz (%s)\n",
			pRequestedMode -> Width,
			pRequestedMode -> Height,
			uiRequestedFormatBPP,
			(UINT)((float)((pRequestedMode -> RefreshRate).Numerator) /
			(float)((pRequestedMode -> RefreshRate).Denominator)),
			pRequestedModeFormatInfo -> szDesc
		);

		UINT uiEnumModeFormatIndex;
		const UINT cuiNoOfFormats = (UINT)(CFormatInfoMap::GetDXGIFormatInfoTableSize());

		for (uiEnumModeFormatIndex = 0; uiEnumModeFormatIndex < cuiNoOfFormats; ++uiEnumModeFormatIndex)
		{
			const DXGIFormatInfo &CurrentFormat = CFormatInfoMap::GetDXGIFormatInfo(uiEnumModeFormatIndex);

			// If NULL was passed for the pConcernedDevice argument, confine the search to modes
			// whose formats match precisely that of the requested mode.

			if (pDeviceUnknownInterface || (CurrentFormat.Format == pRequestedMode -> Format))
			{
				UINT uiEnumeratedBPP = CurrentFormat.uBitsPerElement;

				if (uiEnumeratedBPP >= uiRequestedFormatBPP)
				{
					UINT NumModes;

					DebugOut.Write("About to call GetDisplayModeList (1st call).\n");

					if (FAILED((hresult = pOutput -> GetDisplayModeList(CurrentFormat.Format, 0, &NumModes, 0))))
						WriteToLog("Warning: GetDisplayModeList (1st call) failed.  Format: %s.\n", CurrentFormat.szDesc);
					else
					if (NumModes)
					{
						DebugOut.Write("About to instantiate CMyAutoPtr.\n");

						CMyAutoPtr<DXGI_MODE_DESC> pModes(new DXGI_MODE_DESC[NumModes]);

						if (!pModes.p)
							return false;

						DebugOut.Write("About to call GetDisplayModeList (2nd call).\n");

						if (FAILED((hresult = pOutput -> GetDisplayModeList(CurrentFormat.Format, 0, &NumModes, pModes.p))))
							WriteToLog("Warning: GetDisplayModeList (2nd call) failed.  Format: %s.\n", CurrentFormat.szDesc);
						else
						{
							// Scan enumerated modes.
							UINT uiModeOrdinal;

							for (uiModeOrdinal = 0; uiModeOrdinal < NumModes; ++uiModeOrdinal)
							{
								DXGI_MODE_DESC EnumeratedMode;

								EnumeratedMode = pModes.p[uiModeOrdinal];

								// It has already been established that the BPP of the enumerated
								// mode matches or betters that of the requested mode.

								// If the enumerated mode is "at least as good as" the
								// requested mode in terms of resolution and refresh rate, return
								// false to indicate an unexpected match.

								if ((EnumeratedMode.Width >= pRequestedMode -> Width) &&
									(EnumeratedMode.Height >= pRequestedMode -> Height) &&
									(((EnumeratedMode.RefreshRate).Numerator * (pRequestedMode -> RefreshRate).Denominator) >=
									((EnumeratedMode.RefreshRate).Denominator * (pRequestedMode -> RefreshRate).Numerator))
									)
								{
									WriteToLog("Mode enumeration unexpectedly found a matching mode.\n");

									WriteToLog
									(
										"Enumerated Mode: %d x %d x %dbpp at %dHz (%s)\n",
										EnumeratedMode.Width,
										EnumeratedMode.Height,
										uiEnumeratedBPP,
										(UINT)((float)((EnumeratedMode.RefreshRate).Numerator) /
										(float)((EnumeratedMode.RefreshRate).Denominator)),
										CurrentFormat.szDesc
									);

									return false;
								}
							}
						}
					}
				}
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// clsModeMatchNoDevice Methods

TEST_RESULT  clsModeMatchNoDevice::ObtainOutput()
{
	TEST_RESULT     test_result = RESULT_FAIL;

	if ( pOutput && !pDeviceUnknownInterface )
	{
		return RESULT_PASS;
	}

	test_result = CreateObjects();

	if ( pDeviceUnknownInterface )
	{
		pDeviceUnknownInterface->Release();
		pDeviceUnknownInterface = NULL;
	}

	return test_result;
}

clsModeMatchNoDevice::~clsModeMatchNoDevice()
{
}

////////////////////////////////////////////////////////////////////////////////
// clsModeMatchD3D10Device Methods

TEST_RESULT  clsModeMatchD3D10Device::ObtainOutput()
{
	if (pOutput && pDeviceUnknownInterface)
	{
		return RESULT_PASS;
	}

	return CreateObjects();
}

clsModeMatchD3D10Device::~clsModeMatchD3D10Device()
{
}

//////////////////////////////////////////////////////////////////////////


CMyTestApp  g_Application;
