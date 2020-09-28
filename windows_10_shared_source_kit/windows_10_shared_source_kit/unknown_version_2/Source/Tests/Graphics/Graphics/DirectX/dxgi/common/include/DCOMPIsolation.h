//****************************************************************************
// File: DCOMPIsolation.h
//
// Purpose: Isolates all DCOMP calls to a separate lib so our test code can be
// compiled for pre-Win8 OSes.   Dynalinks to DCOMP interfaces so binaries
// will actually load on pre-win8 OSes.
//
// Presenting through DCOMP is referred to as "Windowless" presentation, but
// DCOMP still ultimately needs a window for final composition.
//
// Copyright (c) Microsoft Corporation
//****************************************************************************

#pragma once


#define TEMP NTDDI_VERSION
#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WIN8
#include <dcomp.h>
#undef NTDDI_VERSION
#define NTDDI_VERSION TEMP

#include <ISimpleLog.h>


#if !defined(PASTER2)
#define PASTER2(x,y) x ## y
#endif

#if !defined(PASTER)
#define PASTER(x,y) PASTER2(x,y)
#endif

#if !defined(UNICODIFY)
#define UNICODIFY( x ) PASTER(L,x)
#endif

#if !defined(__FILEW__)
#define __FILEW__ UNICODIFY(__FILE__)
#endif

#if !defined(SAFE_RELEASE)
#define SAFE_RELEASE(x) \
	if ((x) != nullptr) \
	{ \
		(x)->Release(); \
		(x) = nullptr; \
	}
#endif


class CDCompIsolation;
class CDCompDeviceWrapper;
class CDCompTargetWrapper;


class CDCompIsolation
{
	//
	// Friend declarations
	//

	friend class CDCompDeviceWrapper;
	friend class CDCompTargetWrapper;


	//
	// Construction/destruction
	//

	public:
		CDCompIsolation();

		~CDCompIsolation();


	//
	// Member accessors
	//

	public:
		void SetLogger
		(
			__in_opt ISimpleLog * pLogger
		);


	//
	// DirectComposition manipulation methods
	//

	public:
		HRESULT Initialize();

		void Cleanup();


		HRESULT CreateDevice
		(
			__out CDCompDeviceWrapper ** pDeviceWrapper
		);


		HRESULT CreateSurfaceHandle
		(
			DWORD DesiredAccess,
			__in_opt SECURITY_ATTRIBUTES * pSecurityAttributes,
			__out HANDLE * phCompositionSurface
		);


		// Method forwarder for old name until I can get around
		// to removing the references to the old name.
		void DestroyDCompDeviceAndVisual()
		{
			Cleanup();
		};


		HRESULT SetTargetWindow
		(
			__in CDCompDeviceWrapper * pDeviceWrapper,
			__in HWND hTargetWindow
		);

		HRESULT SetTargetWindow
		(
			__in HWND hTargetWindow
		);


		HRESULT SetSourceSwapChain
		(
			__in CDCompTargetWrapper * pTargetWrapper,
			__in IDXGISwapChain * pSourceSwapChain,
			bool bScaleToOutputWindow
		);

		HRESULT SetSourceSwapChain
		(
			__in IDXGISwapChain * pSourceSwapChain,
			bool bScaleToOutputWindow
		);


	//
	// Higher-level do-it-all-in-one-call methods
	//

	public:
		HRESULT CreateDCompResources
		(
			__in IDXGISwapChain * pSwapChain,
			__in HWND hTargetWindow,
			bool bScaleToOutputWindow = true
		);

		// Method forwarder for old name until I can get around
		// to removing the references to the old name.
		HRESULT CreateAndAssociateDCOMPVisual
		(
			__in IDXGISwapChain * pSwapChain,
			__in HWND hTargetWindow
		)
		{
			return CreateDCompResources
			(
				pSwapChain,
				hTargetWindow
			);
		};


	//
	// Helper methods
	//

	private:
		void LogError
		(
			ERROR_TYPE ErrorType,
			UINT ErrorCode,
			__in const WCHAR * SourceFileName,
			ULONG SourceFileLine,
			__in const CHAR * SourceFunction,
			__in const WCHAR * ErrorDescription
		);

		void LogMessage
		(
			__in const WCHAR * szFormat,
			...
		);


	//
	// Member data
	//

	private:
		// DirectComposition module handle
		HMODULE m_hDCompLibrary;

		// DirectComposition wrapper objects
		CDCompDeviceWrapper * m_pDefaultDeviceWrapper;
		CDCompTargetWrapper * m_pDefaultTargetWrapper;

		// Logger interface
		ISimpleLog * m_pLogger;
};


class CDCompDeviceWrapper
{
	//
	// Construction/destruction
	//

	public:
		CDCompDeviceWrapper
		(
			__in CDCompIsolation & Parent
		);

		~CDCompDeviceWrapper();


	//
	// Member accessors
	//

	public:
		IDCompositionDevice * GetDevice() const;

		CDCompIsolation * GetParent() const;


	//
	// Public methods
	//

	public:
		HRESULT Initialize();

		void Cleanup();


		HRESULT CreateTargetForWindow
		(
			__in HWND hWindow,
			__out CDCompTargetWrapper ** ppTargetWrapper
		);

		HRESULT CreateTargetForWindow
		(
			__in HWND hWindow,
			BOOL bTopmost,
			__out CDCompTargetWrapper ** ppTargetWrapper
		);


	//
	// Member data
	//

	private:
		// Parent DCompIsolation object
		CDCompIsolation * m_pParent;

		// DirectComposition module handle
		HMODULE m_hDCompLibrary;

		// DirectComposition device
		IDCompositionDevice * m_pDCompDevice;
};


class CDCompTargetWrapper
{
	//
	// Construction/destruction
	//

	public:
		CDCompTargetWrapper
		(
			__in CDCompDeviceWrapper & ParentDevice
		);

		~CDCompTargetWrapper();


	//
	// Member accessors
	//

	public:
		//IDCompositionVisual * GetRootVisual() const;

		IDCompositionTarget * GetTarget() const;

		HWND GetTargetWindow() const;


	//
	// Public methods
	//

	public:
		HRESULT Initialize
		(
			__in HWND hTargetWindow,
			BOOL bTopmost
		);

		void Cleanup();

		HRESULT SetSourceSwapChain
		(
			__in IDXGISwapChain * pSwapChain,
			bool bScaleToOutputWindow
		);


	//
	// Member data
	//

	private:
		// DirectComposition wrapper objects
		CDCompDeviceWrapper * m_pParentDevice;

		// DirectComposition objects
		IDCompositionTarget * m_pDCompTarget;
		IDCompositionVisual * m_pDCompRootVisual;

		// Target window handle
		HWND m_hTargetWindow;
};
