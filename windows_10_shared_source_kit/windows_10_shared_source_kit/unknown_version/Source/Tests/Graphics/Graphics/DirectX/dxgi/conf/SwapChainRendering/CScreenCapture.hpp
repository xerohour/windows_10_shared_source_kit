//////////////////////////////////////////////////////////////////////
// File:  CScreenCapture.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a simple screen-capture library.
//
// History:
// 25 Aug 2015  SEdmison  Imported from windows\test\DWM\Common\Utilities.
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
// Win32 headers
//

#include <Windows.h>
#include <WinCodec.h>


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

enum IMAGE_CONTAINER_FORMAT
{
    IMAGE_CONTAINER_FORMAT_BMP,
    IMAGE_CONTAINER_FORMAT_PNG,
    IMAGE_CONTAINER_FORMAT_ICO,
    IMAGE_CONTAINER_FORMAT_JPG,
    IMAGE_CONTAINER_FORMAT_TIFF,
    IMAGE_CONTAINER_FORMAT_GIF,
    IMAGE_CONTAINER_FORMAT_WMP,
};


class CScreenCapture
{
	//
	// Construction/destruction
	//

	public:
		// Constructor
		CScreenCapture();

		// Destructor
		~CScreenCapture();

	private:
		// Copy constructor; not implemented
		CScreenCapture
		(
			const CScreenCapture &
		);

		// Assignment operator; not implemented
		CScreenCapture & operator=
		(
			const CScreenCapture &
		);


	//
	// Initialization/uninitialization
	//

	public:
		HRESULT Initialize();

		HRESULT Uninitialize();


	//
	// Public methods
	//

	public:
		HRESULT Capture();

		HRESULT GetPixel
		(
			LONG X,
			LONG Y,
			_Out_ COLORREF * pColor
		);

		HRESULT Save
		(
			PCWSTR ImageContainerFilename,
			IMAGE_CONTAINER_FORMAT ImageContainerFormat
		);


	//
	// Member data
	//

	private:
		//
		// Windows Imaging Codecs state
		//

		IWICImagingFactory * m_pImagingFactory;
		IWICBitmap * m_pBitmap;


		//
		// Screen coordinates
		//

		RECT m_VirtualScreenRect;
};
