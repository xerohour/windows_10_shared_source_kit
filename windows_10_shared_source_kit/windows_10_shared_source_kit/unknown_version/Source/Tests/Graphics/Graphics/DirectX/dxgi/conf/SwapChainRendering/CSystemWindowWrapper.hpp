//////////////////////////////////////////////////////////////////////
// File:  CSystemWindowWrapper.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for an immersive window thread object.
//
// History:
// 14 Jan 2014  SEdmison  Created.
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
// Class:  CSystemWindowWrapper
//
// Purpose:
// Wrapper object for managing a Win32 window and a thread
// to launch it and pump messages for it.
////////////////////////////////////////////////////////////
class CSystemWindowWrapper
{
	//
	// Construction/destruction
	//

	public:
		// Default constructor.
		CSystemWindowWrapper();

		// Constructor that takes window width and height.
		CSystemWindowWrapper
		(
			UINT WindowWidth,
			UINT WindowHeight
		);

		// Constructor that takes window rectangle coordinates.
		CSystemWindowWrapper
		(
			LONG WindowLeft,
			LONG WindowTop,
			LONG WindowRight,
			LONG WindowBottom
		);


		// Destructor.
		~CSystemWindowWrapper();


	//
	// Public accessor methods
	//

	public:
		HWND GetSystemWindow();


		CSceneRenderer * GetSceneRenderer();

		void SetSceneRenderer
		(
			_In_ CSceneRenderer * pSceneRenderer
		);


		void SetLogger
		(
			_In_ ISimpleLog * pLogger
		);


	//
	// Public methods
	//

	public:
		HRESULT StartSystemWindowThread();

		HRESULT StopSystemWindowThread();


	//
	// Static methods
	//

	private:
		static LRESULT WINAPI StaticMsgProc
		(
			_In_ HWND hWindow,
			UINT Message,
			WPARAM wParam,
			LPARAM lParam
		);


		static DWORD WINAPI ThreadProc
		(
			_In_ void * pParam
		);


	//
	// Event handlers
	//

	private:
		LRESULT MsgProc
		(
			_In_ HWND hWindow,
			UINT Message,
			WPARAM wParam,
			LPARAM lParam
		);


		HRESULT OnWindowSizeChanged
		(
			UINT NewWidth,
			UINT NewHeight
		);


		HRESULT OnWindowClosed();


	//
	// Helper methods
	//

	private:
		HRESULT CreateSystemWindow();

		void DestroySystemWindow();


		HRESULT RecreateSizeDependentResources
		(
			UINT NewWidth,
			UINT NewHeight
		);


		HRESULT RegisterWindowClass();

		HRESULT UnregisterWindowClass();


		HRESULT RenderAndProcessWindowEvents();


	//
	// Member data
	//

	private:
		ISimpleLog * m_pLogger;

		LONG m_WindowLeft;
		LONG m_WindowTop;
		LONG m_WindowRight;
		LONG m_WindowBottom;

		ATOM m_WindowClassAtom;
		HWND m_hWindow;

		HANDLE m_hWindowThread;

		HANDLE m_hWindowLaunchedEvent;
		HANDLE m_hWindowClosedEvent;

		CSceneRenderer * m_pSceneRenderer;
};
