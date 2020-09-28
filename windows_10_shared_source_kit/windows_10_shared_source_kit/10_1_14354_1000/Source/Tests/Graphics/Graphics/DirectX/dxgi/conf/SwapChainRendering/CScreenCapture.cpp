//////////////////////////////////////////////////////////////////////
// File:  CScreenCapture.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a simple screen-capture library.
//
// History:
// 25 Aug 2015  SEdmison  Imported from windows\test\DWM\Common\Utilities.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "CScreenCapture.hpp"
#include "HelperMacros.h"


//
// C runtime headers
//

#include <assert.h>


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CScreenCapture
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CScreenCapture::CScreenCapture()
:
	m_pImagingFactory( nullptr ),
	m_pBitmap( nullptr )
{
	ZeroMemory( &m_VirtualScreenRect, sizeof( m_VirtualScreenRect ) );
};


///////////////////////////////////////////////////////
// Function:  ~CScreenCapture
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CScreenCapture::~CScreenCapture()
{
	assert( m_pBitmap == nullptr );
	assert( m_pImagingFactory == nullptr );
};


////////////////////////////////////////////////////////////
//
// Initialization/uninitialization
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  Initialize
//
// Purpose:
// Creates resources.
///////////////////////////////////////////////////////
HRESULT CScreenCapture::Initialize()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Initialize members and state.
	//

	ZeroMemory( &m_VirtualScreenRect, sizeof( m_VirtualScreenRect ) );


	//
	// Create a WIC factory.
	//

	HRESULT hCreateWICFactoryResult = CoCreateInstance
	(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(PVOID *) &m_pImagingFactory
	);

	if ( FAILED( hCreateWICFactoryResult ) )
	{
		assert( m_pImagingFactory == nullptr );
		m_pImagingFactory = nullptr;

		goto Cleanup;
	};


	//
	// If we made it this far, then we're good to go.
	//

	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  Uninitialize
//
// Purpose:
// Destroys and releases resources.
///////////////////////////////////////////////////////
HRESULT CScreenCapture::Uninitialize()
{
	SAFE_RELEASE( m_pBitmap );
	SAFE_RELEASE( m_pImagingFactory );

	return S_OK;
};


////////////////////////////////////////////////////////////
//
// Public method implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  Capture
//
// Purpose:
// Captures the screen.
///////////////////////////////////////////////////////
HRESULT CScreenCapture::Capture()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	HDC hScreenDC = nullptr;
	HDC hCompatibleDC = nullptr;


	//
	// Release any bitmap we might be holding from a
	// prior call to this method.
	//

	SAFE_RELEASE( m_pBitmap );


	//
	// Hide the cursor.
	//

	ShowCursor( FALSE );


	//
	// Get the screen device context.
	//

	hScreenDC = GetDC( nullptr );

	if ( hScreenDC == nullptr )
	{
		hOverallResult = E_FAIL;
		goto Cleanup;
	};


	//
	// Create a memory device context compatible with the
	// screen device context.
	//

	hCompatibleDC = CreateCompatibleDC( hScreenDC );

	if ( hCompatibleDC == nullptr )
	{
		hOverallResult = E_FAIL;
		goto Cleanup;
	};


	//
	// Determine the extents of the virtual screen.
	//

	LONG ScreenLeft = (LONG) GetSystemMetrics( SM_XVIRTUALSCREEN );
	LONG ScreenTop = (LONG) GetSystemMetrics( SM_YVIRTUALSCREEN );
	LONG ScreenWidth = (LONG) GetSystemMetrics( SM_CXVIRTUALSCREEN );
	LONG ScreenHeight = (LONG) GetSystemMetrics( SM_CYVIRTUALSCREEN );


	//
	// Create a bitmap compatible with the screen device context.
	//

	HBITMAP hBitmap = CreateCompatibleBitmap
	(
		hScreenDC,
		ScreenWidth,
		ScreenHeight
	);

	if ( hBitmap == nullptr )
	{
		hOverallResult = E_FAIL;
		goto Cleanup;
	};


	//
	// Select the bitmap object into the specified device context.
	//


	HGDIOBJ hOriginallySelectedObject = SelectObject
	(
		hCompatibleDC,
		hBitmap
	);


	//
	// Copy pixels into the bitmap.
	//

	BOOL BitBlitResult = BitBlt
	(
		hCompatibleDC,
		0,
		0,
		ScreenWidth,
		ScreenHeight,
		hScreenDC,
		ScreenLeft,
		ScreenTop,
		SRCCOPY
	);

	DWORD BitBlitError = GetLastError();


	//
	// Re-select the originally-selected object.
	//

	if ( hOriginallySelectedObject != nullptr )
	{
		SelectObject
		(
			hCompatibleDC,
			hOriginallySelectedObject
		);
	};


	//
	// Handle errors from BitBlt.
	//

	if ( BitBlitResult != TRUE )
	{
		hOverallResult = HRESULT_FROM_WIN32( BitBlitError );
		goto Cleanup;
	};


	//
	// Create a WIC bitmap from the HBITMAP.
	//

	assert( m_pImagingFactory != nullptr );
	HRESULT hCreateBitmapResult = m_pImagingFactory->CreateBitmapFromHBITMAP
	(
		hBitmap,
		nullptr,
		WICBitmapIgnoreAlpha,
		&m_pBitmap
	);

	if ( FAILED( hCreateBitmapResult ) )
	{
		hOverallResult = hCreateBitmapResult;
		goto Cleanup;
	};

	assert( m_pBitmap != nullptr );


	//
	// Save the virtual screen coordinates.
	//

	m_VirtualScreenRect.left = ScreenLeft;
	m_VirtualScreenRect.top = ScreenTop;
	m_VirtualScreenRect.right = ScreenLeft + ScreenWidth;
	m_VirtualScreenRect.bottom = ScreenTop + ScreenHeight;


	//
	// If we made it this far, then we're good to go.
	//

	hOverallResult = S_OK;


Cleanup:
	//
	// Show the cursor.
	//

	ShowCursor( TRUE );


	if ( FAILED( hOverallResult ) )
	{
		SAFE_RELEASE( m_pBitmap );

		ZeroMemory( &m_VirtualScreenRect, sizeof( m_VirtualScreenRect ) );
	};

	if ( hCompatibleDC != nullptr )
	{
		DeleteDC( hCompatibleDC );
		hCompatibleDC = nullptr;
	};

	if ( hScreenDC != nullptr )
	{
		DeleteDC( hScreenDC );
		hScreenDC = nullptr;
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  GetPixel
//
// Purpose:
// Retrieves the pixel at the specified location from
// the most recent screen capture.
///////////////////////////////////////////////////////
HRESULT CScreenCapture::GetPixel
(
	LONG X,
	LONG Y,
	_Out_ COLORREF * pColor
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	IWICBitmapLock * pBitmapLock = nullptr;


	//
	// Validate and initialize parameters.
	//

	if ( pColor == nullptr )
	{
		hOverallResult = E_POINTER;
		goto Cleanup;
	};


	if
	(
		X < m_VirtualScreenRect.left ||
		X >= m_VirtualScreenRect.right ||
		Y < m_VirtualScreenRect.top ||
		Y >= m_VirtualScreenRect.bottom
	)
	{
		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Validate members and state.
	//

	// Confirm that there is a captured bitmap.
	if ( m_pBitmap == nullptr )
	{
		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Translate from the screen coordinates to bitmap coordinates.
	//

	UINT RelativeX = X - m_VirtualScreenRect.left;
	UINT RelativeY = Y - m_VirtualScreenRect.top;


	//
	// Lock the region that we're sampling from the bitmap.
	//

	WICRect LockRect;
	LockRect.X = RelativeX;
	LockRect.Y = RelativeY;
	LockRect.Width = 1;
	LockRect.Height = 1;
	HRESULT hLockBitmapResult = m_pBitmap->Lock
	(
		&LockRect,
		WICBitmapLockRead,
		&pBitmapLock
	);

	if ( FAILED( hLockBitmapResult ) )
	{
		hOverallResult = hLockBitmapResult;
		goto Cleanup;
	};


	//
	// Get a pointer to the color data.
	//

	UINT BufferSize = 0;
	BYTE * pData = nullptr;
	HRESULT hGetDataPointerResult = pBitmapLock->GetDataPointer
	(
		&BufferSize,
		&pData
	);

	if ( FAILED( hGetDataPointerResult ) )
	{
		hOverallResult = hGetDataPointerResult;
		goto Cleanup;
	};

	if ( pData == nullptr )
	{
		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( BufferSize < 3 )
	{
		hOverallResult = E_NOT_SUFFICIENT_BUFFER;
		goto Cleanup;
	};


	//
	// Make a COLORREF from the color data to fill our out parameter.
	//

#pragma warning( suppress:  26014 )
	*pColor = RGB( pData[ 2 ], pData[ 1 ], pData[ 0 ] );


	//
	// If we made it this far, then we're good to go.
	//

	hOverallResult = S_OK;


Cleanup:
	SAFE_RELEASE( pBitmapLock );


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  Save
//
// Purpose:
// Saves a screen capture to a file.
///////////////////////////////////////////////////////
HRESULT CScreenCapture::Save
(
	PCWSTR ImageContainerFilename,
	IMAGE_CONTAINER_FORMAT ImageContainerFormat
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	GUID WICImageContainerFormat;
	ZeroMemory( &WICImageContainerFormat, sizeof( WICImageContainerFormat ) );

	UINT BitmapWidth = 0;
	UINT BitmapHeight = 0;

	WICRect rect;
	IWICStream * pTargetStream = nullptr;
	IWICBitmapEncoder * pBitmapEncoder = nullptr;
	IWICBitmapFrameEncode * pTargetFrame = nullptr;


	//
	// Validate parameters.
	//

	if
	(
		ImageContainerFilename == nullptr ||
		ImageContainerFilename[ 0 ] == L'\0'
	)
	{
		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	switch ( ImageContainerFormat )
	{
		case IMAGE_CONTAINER_FORMAT_BMP:
		{
			WICImageContainerFormat = GUID_ContainerFormatBmp;
		} break;

		case IMAGE_CONTAINER_FORMAT_PNG:
		{
			WICImageContainerFormat = GUID_ContainerFormatPng;
		} break;

		case IMAGE_CONTAINER_FORMAT_ICO:
		{
			WICImageContainerFormat = GUID_ContainerFormatIco;
		} break;

		case IMAGE_CONTAINER_FORMAT_JPG:
		{
			WICImageContainerFormat = GUID_ContainerFormatJpeg;
		} break;

		case IMAGE_CONTAINER_FORMAT_TIFF:
		{
			WICImageContainerFormat = GUID_ContainerFormatTiff;
		} break;

		case IMAGE_CONTAINER_FORMAT_GIF:
		{
			WICImageContainerFormat = GUID_ContainerFormatGif;
		} break;

		case IMAGE_CONTAINER_FORMAT_WMP:
		{
			WICImageContainerFormat = GUID_ContainerFormatWmp;
		} break;

		default:
		{
			hOverallResult = E_INVALIDARG;
			goto Cleanup;
		};
	};


	//
	// Validate members and state.
	//

	if ( m_pBitmap == nullptr )
	{
		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Create a WIC Stream.
	//

	HRESULT hCreateStreamResult = m_pImagingFactory->CreateStream( &pTargetStream );

	if ( FAILED( hCreateStreamResult ) )
	{
		hOverallResult = hCreateStreamResult;
		goto Cleanup;
	};


	//
	// Initialize the stream.
	//

	HRESULT hInitializeStreamResult = pTargetStream->InitializeFromFilename
	(
		ImageContainerFilename,
		GENERIC_WRITE
	);

	if ( FAILED( hInitializeStreamResult ) )
	{
		hOverallResult = hInitializeStreamResult;
		goto Cleanup;
	};


	//
	// Create a bitmap encoder.
	//

	HRESULT hCreateEncoderResult = m_pImagingFactory->CreateEncoder
	(
		WICImageContainerFormat,
		nullptr,
		&pBitmapEncoder
	);

	if ( FAILED( hCreateEncoderResult ) )
	{
		hOverallResult = hCreateEncoderResult;
		goto Cleanup;
	};


	//
	// Initialize the bitmap encoder.
	//

	HRESULT hInitializeEncoderResult = pBitmapEncoder->Initialize
	(
		pTargetStream,
		WICBitmapEncoderNoCache
	);

	if ( FAILED( hInitializeEncoderResult ) )
	{
		hOverallResult = hInitializeEncoderResult;
		goto Cleanup;
	};


	//
	// Have the encoder create a new frame.
	//

	HRESULT hCreateFrameResult = pBitmapEncoder->CreateNewFrame
	(
		&pTargetFrame,
		nullptr
	);

	if ( FAILED( hCreateFrameResult ) )
	{
		hOverallResult = hCreateFrameResult;
		goto Cleanup;
	};


	//
	// Initialize the new frame.
	//

	HRESULT hInitializeFrameResult = pTargetFrame->Initialize( nullptr );

	if ( FAILED( hInitializeFrameResult ) )
	{
		hOverallResult = hInitializeFrameResult;
		goto Cleanup;
	};


	//
	// Get the size of the bitmap, and set the size of the new
	// frame to match.
	//

	HRESULT hGetBitmapSizeResult = m_pBitmap->GetSize
	(
		&BitmapWidth,
		&BitmapHeight
	);

	if ( FAILED( hGetBitmapSizeResult ) )
	{
		hOverallResult = hGetBitmapSizeResult;
		goto Cleanup;
	};


	HRESULT hSetFrameSizeResult = pTargetFrame->SetSize
	(
		BitmapWidth,
		BitmapHeight
	);

	if ( FAILED( hSetFrameSizeResult ) )
	{
		hOverallResult = hSetFrameSizeResult;
		goto Cleanup;
	};


	//
	// Get the pixel format of the bitmap, and set the format
	// of the new frame to match.
	//

	WICPixelFormatGUID BitmapPixelFormat;
	HRESULT hGetBitmapPixelFormatResult = m_pBitmap->GetPixelFormat( &BitmapPixelFormat );

	if ( FAILED( hGetBitmapPixelFormatResult ) )
	{
		hOverallResult = hGetBitmapPixelFormatResult;
		goto Cleanup;
	};


	HRESULT hSetFramePixelFormatResult = pTargetFrame->SetPixelFormat( &BitmapPixelFormat );

	if ( FAILED( hSetFramePixelFormatResult ) )
	{
		hOverallResult = hSetFramePixelFormatResult;
		goto Cleanup;
	};


	//
	// Copy all the bits from the source bitmap into the new frame.
	//

	rect.X = 0;
	rect.Y = 0;
	rect.Width = BitmapWidth;
	rect.Height = BitmapHeight;

	HRESULT hWriteSourceResult = pTargetFrame->WriteSource
	(
		m_pBitmap,
		&rect
	);

	if ( FAILED( hWriteSourceResult ) )
	{
		hOverallResult = hWriteSourceResult;
		goto Cleanup;
	};


	//
	// Commit the frame.
	//

	HRESULT hCommitFrameResult = pTargetFrame->Commit();

	if ( FAILED( hCommitFrameResult ) )
	{
		hOverallResult = hCommitFrameResult;
		goto Cleanup;
	};


	//
	// Have the bitmap encoder commit to the output stream.
	//

	HRESULT hCommitBitmapResult = pBitmapEncoder->Commit();

	if ( FAILED( hCommitBitmapResult ) )
	{
		hOverallResult = hCommitBitmapResult;
		goto Cleanup;
	};


	//
	// If we made it this far, then we're good to go.
	//

	hOverallResult = S_OK;


Cleanup:
	SAFE_RELEASE( pTargetFrame );
	SAFE_RELEASE( pBitmapEncoder );
	SAFE_RELEASE( pTargetStream );


	return hOverallResult;
};
