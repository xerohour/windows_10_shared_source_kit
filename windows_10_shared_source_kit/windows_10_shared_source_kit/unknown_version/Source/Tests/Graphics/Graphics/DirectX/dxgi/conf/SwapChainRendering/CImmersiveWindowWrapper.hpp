//////////////////////////////////////////////////////////////////////
// File:  CImmersiveWindowWrapper.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for an immersive window thread object.
//
// History:
// 12 Dec 2013  SEdmison  Imported from
//                        windowstest\DWM\Common\DXScene\DXSceneImmersiveWindow.h.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Pragmas
//
/////////////////////////////////////////////////////////////////

#pragma once


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// WinRT headers
//

#include <corewindowp.h>
#include <windows.ui.core.corewindow.h>
#include <CoreWindow.h>
#include <Propsys.h>
#include <Propkey.h>
#include <shellapi.h>
#include <wrl\event.h>
#include <windowsstringp.h>


//
// Project headers
//

#include "RenderingTypes.hpp"
#include "GraphicsTAEFTest.hpp"


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Helper class interfaces
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Class:  CImmersiveWindowWrapper
//
// Purpose:
// Wrapper object for managing a core window and a thread
// to launch it and pump messages for it.
////////////////////////////////////////////////////////////
class CImmersiveWindowWrapper
{
	//
	// Construction/destruction
	//

	public:
		// Default constructor.
		CImmersiveWindowWrapper();

		// Constructor that takes window width and height.
		CImmersiveWindowWrapper
		(
			UINT WindowWidth,
			UINT WindowHeight
		);

		// Constructor that takes window rectangle origin and size.
		CImmersiveWindowWrapper
		(
			LONG WindowLeft,
			LONG WindowTop,
			UINT WindowWidth,
			UINT WindowHeight
		);


		// Destructor.
		~CImmersiveWindowWrapper();


	//
	// Public accessor methods
	//

	public:
		Windows::UI::Core::ICoreWindow * GetImmersiveWindow();


		void SetLogger
		(
			_In_ ISimpleLog * pLogger
		);


		CSceneRenderer * GetSceneRenderer();

		void SetSceneRenderer
		(
			_In_ CSceneRenderer * pSceneRenderer
		);


	//
	// Public methods
	//

	public:
		HRESULT StartImmersiveWindowThread();

		HRESULT StopImmersiveWindowThread();


	//
	// Static methods
	//

	private:
		static DWORD WINAPI ThreadProc
		(
			_In_ void * pParam
		);


	//
	// Event handlers
	//

	private:
		HRESULT OnWindowSizeChanged
		(
			_In_ Windows::UI::Core::ICoreWindow * sender,
			_In_ Windows::UI::Core::IWindowSizeChangedEventArgs * e
		);


		HRESULT OnWindowClosed
		(
			_In_ Windows::UI::Core::ICoreWindow * sender,
			_In_ Windows::UI::Core::ICoreWindowEventArgs * e
		);


	//
	// Helper methods
	//

	private:
		HRESULT CreateImmersiveWindow();

		void DestroyImmersiveWindow();


		HRESULT NotifyWindowResizeManagerOfLayoutCompletion();


		HRESULT RecreateSizeDependentResources
		(
			UINT NewWidth,
			UINT NewHeight
		);


		HRESULT RenderAndProcessWindowEvents();


		HRESULT SetAppUserModelID();


		HRESULT SetImmersiveWindowCursor();


	//
	// Member data
	//

	private:
		ISimpleLog * m_pLogger;

		LONG m_WindowLeft;
		LONG m_WindowTop;

		UINT m_WindowWidth;
		UINT m_WindowHeight;

		Microsoft::WRL::ComPtr< Windows::UI::Core::ICoreWindow > m_CoreWindow;
		Microsoft::WRL::ComPtr< Windows::UI::Core::ICoreDispatcher > m_CoreDispatcher;

		EventRegistrationToken m_WindowSizeChangedToken;
		EventRegistrationToken m_WindowClosedToken;

		HANDLE m_hWindowThread;

		HANDLE m_hWindowLaunchedEvent;
		HANDLE m_hWindowClosedEvent;

		CSceneRenderer * m_pSceneRenderer;
};
