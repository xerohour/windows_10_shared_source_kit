#include "CCompDC.h"

extern "C" {
WINGDIAPI HBITMAP WINAPI CreateBitmapFromDxSurface(
                                                 __in HDC hdc, 
                                                 __in UINT uiWidth, 
                                                 __in UINT uiHeight, 
                                                 __in DWORD Format, 
                                                 __in HANDLE hDxSharedSurface
                                                      ); 
}

void RemoveMessage(HWND hWnd)
{
	MSG msg; 
	if(!PeekMessage(&msg, hWnd, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

CCompatibleBitmap::CCompatibleBitmap(HDC hDC, UINT Width, UINT Height, BOOL bDeviceBitmap):
	m_bValid(FALSE),
	m_hBitmap(NULL),
	m_pBitmapBits(NULL),
	m_Width(Width),
	m_Height(Height)
{
	ZeroMemory(&m_bmi, sizeof(m_bmi));
	m_bmi.bmiHeader.biSize = sizeof(m_bmi.bmiHeader); 
	m_bmi.bmiHeader.biWidth  = Width; 
	m_bmi.bmiHeader.biHeight = -(LONG)Height; 
	m_bmi.bmiHeader.biPlanes = 1; 
	m_bmi.bmiHeader.biBitCount = 32; 
	m_bmi.bmiHeader.biCompression = BI_RGB; 
	m_bmi.bmiHeader.biXPelsPerMeter = 96; 
	m_bmi.bmiHeader.biYPelsPerMeter = 96;

	if(NULL == hDC)
	{
		hDC = CreateCompatibleDC(NULL);
		if (hDC == NULL)
		{
			return;
		}

		HWND hScreenWnd = GetDesktopWindow();
		RECT ScreenRect;
		if( !GetWindowRect(hScreenWnd, &ScreenRect) )
		{
			return;
		}
		m_Width = ScreenRect.right - ScreenRect.left;
		m_Height = ScreenRect.bottom - ScreenRect.top;

		m_bmi.bmiHeader.biWidth  = m_Width; 
		m_bmi.bmiHeader.biHeight = -(LONG)m_Height;
	}

	if(!bDeviceBitmap)
	{
		m_hBitmap = CreateDIBSection(hDC, &m_bmi, DIB_RGB_COLORS, (VOID**)&m_pBitmapBits, NULL, 0);
		if (m_hBitmap == NULL)
		{
			return;
		}
	}
	else
	{
		m_hBitmap = CreateBitmapFromDxSurface(hDC, m_Width, m_Height, DXGI_FORMAT_B8G8R8A8_UNORM, NULL);
		if (m_hBitmap == NULL)
		{
			return;
		}
	}

	m_bValid = TRUE;
}
//-----------------------------------------------------------------------------
CCompatibleBitmap::~CCompatibleBitmap()
{
	if (m_hBitmap)
	{
		DeleteObject(m_hBitmap);
	}
}
BOOL CCompatibleBitmap::SaveBitmap(TCHAR* FileName)
{
	BOOL    bSaved = FALSE;
	DWORD   dwWrite;
	HANDLE  hFile;
	DWORD   dwSize = m_Width*m_Height*4;

	hFile = CreateFile((LPCTSTR)FileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE != hFile)
	{
		// Write bitmap headers.
		{
			PBITMAPINFOHEADER   pImageHeader;
			BITMAPV5HEADER      BitmapHeader;
			BITMAPFILEHEADER    FileHeader;


			// Initialize bitmap header.
			memset(&BitmapHeader, 0, sizeof(BITMAPV5HEADER));
			pImageHeader = (PBITMAPINFOHEADER) &m_bmi;
			memcpy(&BitmapHeader, pImageHeader, pImageHeader->biSize);
			BitmapHeader.bV5BitCount = 32; 
			BitmapHeader.bV5SizeImage = dwSize;
			BitmapHeader.bV5ClrUsed = 0;
			BitmapHeader.bV5ClrImportant = 0;
			//Assume OPTION_NO_TRANSLATE
			{
				BitmapHeader.bV5Size = sizeof(BITMAPINFOHEADER);
			}
	
			// Init & Write Bitmap file header.
			memset(&FileHeader, 0, sizeof(BITMAPFILEHEADER));
			FileHeader.bfType = BFT_BITMAP;
			FileHeader.bfSize = sizeof(BITMAPFILEHEADER) + BitmapHeader.bV5Size + dwSize;
			FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + BitmapHeader.bV5Size;
			bSaved = WriteFile(hFile, &FileHeader, sizeof(BITMAPFILEHEADER), &dwWrite, NULL);
			if(!bSaved)
			{
				return FALSE;
			}

			// Write Bitmap header.
			bSaved = WriteFile(hFile, &BitmapHeader, BitmapHeader.bV5Size, &dwWrite, NULL);
			if(!bSaved)
			{
				return FALSE;
			}
		}

		// Write bitmap bits to file.
		bSaved = WriteFile(hFile, m_pBitmapBits, dwSize, &dwWrite, NULL);
	
		// No longer need file handle.
		CloseHandle(hFile);
	}
	else
	{
		bSaved = FALSE;
	}
	return bSaved;
}

//-----------------------------------------------------------------------------
CCompatibleDC::CCompatibleDC(HWND hWnd, HDC hDC, BOOL bDeviceBitmap):
	m_hWnd(hWnd),
	m_bValid(FALSE),
	m_hDC(hDC),
	m_hScreenDC(NULL),
	m_hMemoryDC(NULL),
	m_hDeviceBitmapDC(NULL),
	m_pScreenBitmap(NULL),
	m_pMemoryBitmap(NULL),
	m_pDiffBitmap(NULL),
	m_pDeviceBitmap(NULL),
	m_pTemplateBits(NULL),
	m_bDeviceBitmap(bDeviceBitmap)
{
	m_iMaxError = 0;
	int Width = 0, Height = 0; 
	if(NULL == m_hDC)
	{
		//For non-window DC
		if(NULL == hWnd)
		{
			HWND hScreenWnd = GetDesktopWindow();
			RECT ScreenRect;

			if( !GetWindowRect(hScreenWnd, &ScreenRect) )
			{
				return;
			}
			Width = ScreenRect.right - ScreenRect.left;
			Height = ScreenRect.bottom - ScreenRect.top;
		}
		m_hDC = GetDC(hWnd);
		if (m_hDC == NULL)
		{
			return;
		}
		
	}
	else
	{
		//no need to maintain the winodw handle
		//since the DC comes from caller.
		m_hWnd = NULL;
	}
	// Get the size of the client area.
	if( Width <= 0 || Height <= 0)
	{
		RECT r;
		GetClientRect(hWnd, &r); 
		Width = (r.right - r.left);
		Height = (r.bottom - r.top);

		if(Width <= 0 || Height <= 0)
		{
			return;
		}
	}

	m_Width = Width;
	m_Height = Height;

	//allocate the memory for recording interior pixels in TextOut tests, 1 byte for 1 pixel to ease the implemetation
	m_pTemplateBits = (BYTE*)new char[m_Width*m_Height];
	if(NULL == m_pTemplateBits)
	{
		return;
	}
	ZeroMemory(m_pTemplateBits, m_Width*m_Height*sizeof(char));

	m_hScreenDC = CreateCompatibleDC(m_hDC);
	if( NULL == m_hScreenDC)
	{
		return;
	}
	m_pScreenBitmap = new CCompatibleBitmap(m_hScreenDC, m_Width, m_Height, FALSE);
	if(! m_pScreenBitmap->IsValid() || NULL == m_pScreenBitmap->GetBits())
	{
		return;
	}
	SelectObject(m_hScreenDC, m_pScreenBitmap->GetBitmap());

	m_hMemoryDC = CreateCompatibleDC(m_hDC);
	if( NULL == m_hMemoryDC)
	{
		return;
	}
	m_pMemoryBitmap = new CCompatibleBitmap(m_hMemoryDC, m_Width, m_Height, FALSE);
	if(! m_pMemoryBitmap->IsValid() || NULL == m_pMemoryBitmap->GetBits())
	{
		return;
	}
	SelectObject(m_hMemoryDC, m_pMemoryBitmap->GetBitmap());

	m_bmi = m_pMemoryBitmap->GetBMI();

	m_pDiffBitmap = new CCompatibleBitmap(m_hMemoryDC, m_Width, m_Height, FALSE);
	if(! m_pDiffBitmap->IsValid() || NULL == m_pDiffBitmap->GetBits())
	{
		return;
	}

	m_bValid = TRUE;

	if(bDeviceBitmap)
	{
		m_hDeviceBitmapDC = CreateCompatibleDC(m_hDC);
		if( NULL == m_hDeviceBitmapDC)
		{
			m_bDeviceBitmap = FALSE;
			return;
		}
		m_pDeviceBitmap = new CCompatibleBitmap(m_hDC, m_Width, m_Height, TRUE);
		if(! m_pDeviceBitmap->IsValid() || NULL == m_pDeviceBitmap->GetBitmap() )
		{
			m_bDeviceBitmap = FALSE;
			return;
		}
		SelectObject(m_hDeviceBitmapDC, m_pDeviceBitmap->GetBitmap());
	}
}
//-----------------------------------------------------------------------------
CCompatibleDC::~CCompatibleDC()
{
	if(NULL != m_pTemplateBits)
	{
		delete m_pTemplateBits;
	}
	if (m_pScreenBitmap)
	{
		delete m_pScreenBitmap;
	}
	if (m_pMemoryBitmap)
	{
		delete m_pMemoryBitmap;
	}
	if (m_pDiffBitmap)
	{
		delete m_pDiffBitmap;
	}
	if (m_pDeviceBitmap)
	{
		delete m_pDeviceBitmap;
	}
	if(m_hWnd && m_hDC)
	{
		ReleaseDC(m_hWnd, m_hDC);
	}
	if (m_hScreenDC)
	{
		DeleteDC(m_hScreenDC);
	}
	if (m_hMemoryDC)
	{
		DeleteDC(m_hMemoryDC);
	}
	if (m_hDeviceBitmapDC)
	{
		DeleteDC(m_hDeviceBitmapDC);
	}
}
//-----------------------------------------------------------------------------
BOOL CCompatibleDC::Initialize(HBRUSH hBrush)
{
	if (!m_bValid)
	{
		return FALSE;
	}

	//HBRUSH hBrush = CreateSolidBrush(0x00805060);
	if (hBrush == NULL)
	{
		return FALSE;
	}

	HBRUSH hOldBrush = (HBRUSH)SelectObject(m_hDC, hBrush);
	// Fill the background
	if (!BitBlt(m_hDC, 0, 0, m_Width, m_Height, NULL, 0, 0, PATCOPY))
	{
		return FALSE;
	}
	SelectObject(m_hDC, hOldBrush);
	hOldBrush = (HBRUSH)SelectObject(m_hMemoryDC, hBrush);
	if (!BitBlt(m_hMemoryDC, 0, 0, m_Width, m_Height, NULL, 0, 0, PATCOPY))
	{
		return FALSE;
	}
	SelectObject(m_hMemoryDC, hOldBrush);
	hOldBrush = (HBRUSH)SelectObject(m_hScreenDC, hBrush);
	if (!BitBlt(m_hScreenDC, 0, 0, m_Width, m_Height, NULL, 0, 0, PATCOPY))
	{
		return FALSE;
	}
	SelectObject(m_hScreenDC, hOldBrush);
	if(m_bDeviceBitmap)
	{
		hOldBrush = (HBRUSH)SelectObject(m_hDeviceBitmapDC, hBrush);
		if (!BitBlt(m_hDeviceBitmapDC, 0, 0, m_Width, m_Height, NULL, 0, 0, PATCOPY))
		{
			return FALSE;
		}
		SelectObject(m_hDeviceBitmapDC, hOldBrush);
	}
	GdiFlush();

	//Remove the message in the message loop in case of the overflow
	RemoveMessage(m_hWnd);

	return TRUE;
}
//-----------------------------------------------------------------------------
BOOL CCompatibleDC::GetInteriorPixels(COLORREF TextCol)
{
	UINT rText = (UINT)GetRValue(TextCol);
	UINT gText = (UINT)GetGValue(TextCol);
	UINT bText = (UINT)GetBValue(TextCol);

	//Check and mark the index for interior pixels for those with same text color in memory DC as the given one
	DWORD* pMemoryColors = (DWORD*) m_pMemoryBitmap->GetBits();
	if(NULL == pMemoryColors)
	{
		return FALSE;
	}

	DWORD MemoryValue;
	UINT col;
	for (UINT i=0; i < m_Height; i++)
	{
		for (UINT j=0; j < m_Width; j++)
		{
			MemoryValue = *(pMemoryColors + i*m_Width + j);

			*(m_pTemplateBits + i*m_Width + j) = 255;
			//Blue channel
			{	
				col = (UINT)((MemoryValue) & 0x000000ff);
				if( col != bText )
				{
					*(m_pTemplateBits + i*m_Width + j) = 0;
					continue;
				}
			}

			//Green channel
			{	
				col = (UINT)((MemoryValue) & 0x0000ff00)>>8;
				if( col != gText )
				{
					*(m_pTemplateBits + i*m_Width + j) = 0;
					continue;
				}
			}

			//Red channel
			{	
				col = (UINT)((MemoryValue) & 0x00ff0000)>>16;
				if( col != rText )
				{
					*(m_pTemplateBits + i*m_Width + j) = 0;
					continue;
				}
			}
		}	 	
	}

	UINT uTotal = 0;
	for (UINT i=0; i < m_Height; i++)
	{
		for (UINT j=0; j < m_Width; j++)
		{
			uTotal += *(m_pTemplateBits + i*m_Width + j)/255;
		}
	}
	//check the valid value set in the template memory
	if( uTotal < 1000 )
	{
		return FALSE;
	}

	return TRUE;
}
//-----------------------------------------------------------------------------
void CCompatibleDC::SetMaxErrorValue( int eValue )
{
	m_iMaxError = eValue;
}
BOOL CCompatibleDC::SyncBitmapDC(BOOL bUseDeviceBitmap)
{
	if (!m_bValid)
	{
		return FALSE;
	}

	GdiFlush();
	if(!bUseDeviceBitmap)
	{
		//HBITMAP hOldBitmap = (HBITMAP)SelectObject(m_hScreenDC, m_pScreenBitmap->GetBitmap());
		if (!BitBlt(m_hScreenDC, 0,0, m_Width, m_Height, m_hDC, 0, 0, SRCCOPY))
		{
			return FALSE;
		}
	}
	else
	{
		if (!BitBlt(m_hScreenDC, 0,0, m_Width, m_Height, m_hDeviceBitmapDC, 0, 0, SRCCOPY))
		{
			return FALSE;
		}
	}

	//if (!GetDIBits(m_hScreenDC, m_pScreenBitmap->GetBitmap(), 0, m_Height, m_pScreenBitmap->GetBits(), &m_bmi, DIB_RGB_COLORS))
	//{
	//	return FALSE;
	//}

	//if (!GetDIBits(m_hMemoryDC, m_pMemoryBitmap->GetBitmap(), 0, m_Height, m_pMemoryBitmap->GetBits(), &m_bmi, DIB_RGB_COLORS))
	//{
	//  	return FALSE;
	//}
	
	return TRUE;
}
//this is a simple version without saving details of errors
BOOL CCompatibleDC::CompareDC()
{
	UINT* pBitmapBits = (UINT*)m_pScreenBitmap->GetBits();
	UINT BitmapPitch = m_pScreenBitmap->GetPitch();
	UINT* pDiffBits = (UINT*)m_pMemoryBitmap->GetBits();
	UINT DiffPitch = m_pMemoryBitmap->GetPitch();
	BOOL bSuccess = TRUE;

	ErrorInfo.NumofColorError = 0;
	ErrorInfo.AveRedError = 0;
	ErrorInfo.AveGreenError = 0;
	ErrorInfo.AveBlueError = 0;
	ErrorInfo.MaxRedError = 0;
	ErrorInfo.MaxGreenError = 0;
	ErrorInfo.MaxBlueError = 0;

	for (UINT y=0; y < m_Height; y++)
	{
		UINT* pBitmapRow = (UINT*)((BYTE*)pBitmapBits + BitmapPitch * y);
		UINT* pThisRow = (UINT*)((BYTE*)pBitmapBits + m_Width * 4 * y);
		UINT* pDiffRow = (UINT*)((BYTE*)pDiffBits + DiffPitch * y);
		for (UINT x=0; x < m_Width; x++)
		{
			if ((pBitmapRow[x] & 0x00FFFFFF) != (pThisRow[x] & 0x00FFFFFF))
			{
				ErrorInfo.NumofColorError++;
				bSuccess = FALSE;
			}
		}
	}

	return bSuccess;
}
//this version is used for saving details if mis-matched pixels are encountered
BOOL CCompatibleDC::CompareDC(TCHAR* ScenarioName, BOOL bTolerance)
{
	if(NULL == m_pDiffBitmap->GetBits() || NULL == m_pMemoryBitmap->GetBits() || NULL == m_pScreenBitmap->GetBits())
	{
		return FALSE;
	}

	//Used for checking the precision problem with tracking the positon of mismatched pixels
	//ErrorIndexX indicates the latest position in the current row with a mismatched pixel
	//ErrorIndexY contains the results, 1 for mismatched pixel, on the very last row
	int ErrorIndexX = -1, *ErrorIndexY; 
	ErrorIndexY = new int[m_Width - 1];
	BOOL bPrecisionError = TRUE;

	//Compare the resulting contents between two DCs in the corresponding bitmaps pixel by pixel
	memset(&ErrorInfo, 0, sizeof (ErrorInfo));
	ErrorInfo.MaxPixelX = -1;
	ErrorInfo.MaxPixelY = -1;
	int NumofAlphaErrors = 0, AlphaDiff = 0;
	int numR=0, numG=0, numB=0;
	{
		DWORD* pMemoryColors = (DWORD*) m_pMemoryBitmap->GetBits();
		DWORD* pScreenColors = (DWORD*) m_pScreenBitmap->GetBits();
		DWORD* pDiffColors = (DWORD*) m_pDiffBitmap->GetBits();
		DWORD MemoryValue, ScreenValue;
		int Maxdiff = 0, diff, diffr, diffg, diffb, vAlpha1, vAlpha2;
		int col1, col2, temp;

		//ignore the pixels on the edges
		for (UINT i=1; i < m_Height-1; i++)
		{
			ErrorIndexX = -1;//no mismatched pixel found in the following row yet
			for (UINT j=1; j < m_Width-1; j++)
			{
				ScreenValue = *(pScreenColors + i*m_Width + j);
				MemoryValue = *(pMemoryColors + i*m_Width + j);

				vAlpha1 = ((ScreenValue) & 0xff000000)>>24;
				vAlpha2 = ((MemoryValue) & 0xff000000)>>24;

				temp = (vAlpha1-vAlpha2)*(vAlpha1-vAlpha2);
				if(temp > 0)
				{
					AlphaDiff += temp;
					NumofAlphaErrors++;
				}

				diff= 0;					
				//Blue channel
				{	
					col1 = (int)((ScreenValue) & 0x000000ff);
					col2 = (int)((MemoryValue) & 0x000000ff);
					diffb = abs(col1 - col2);
					diff += diffb*diffb;
				}

				//Green channel
				{	
					col1 = (int)((ScreenValue) & 0x0000ff00)>>8;
					col2 = (int)((MemoryValue) & 0x0000ff00)>>8;
					diffg = abs(col1 - col2);
					diff += diffg*diffg;
				}

				//Red channel
				{	
					col1 = (int)((ScreenValue) & 0x00ff0000)>>16;
					col2 = (int)((MemoryValue) & 0x00ff0000)>>16;
					diffr = abs(col1 - col2);
					diff += diffr*diffr;
				}

				if(diff > 0)
				{
					ErrorInfo.NumofColorError++;
					ErrorInfo.AveTotalError += diff;
					ErrorInfo.AveRedError += diffr;
					ErrorInfo.AveGreenError += diffg;
					ErrorInfo.AveBlueError += diffb;
					if(diffr > 0)
					{
						numR++;
						if(ErrorInfo.MaxRedError < diffr)
							ErrorInfo.MaxRedError = diffr;
					}
					if(diffg > 0)
					{
						numG++;
						if(ErrorInfo.MaxGreenError < diffg)
							ErrorInfo.MaxGreenError = diffg;
					}
					if(diffb > 0)
					{
						numB++;
						if(ErrorInfo.MaxBlueError < diffb)
							ErrorInfo.MaxBlueError = diffb;
					}
					if(Maxdiff < diff)
					{
						Maxdiff = diff;
						ErrorInfo.MaxPixelX = j;
						ErrorInfo.MaxPixelY = i;
						ErrorInfo.RedValue = (int)((MemoryValue) & 0x00ff0000)>>16;
						ErrorInfo.GreenValue = (int)((MemoryValue) & 0x0000ff00)>>8;
						ErrorInfo.BlueValue = (int)((MemoryValue) & 0x000000ff);
					}

					*(pDiffColors + i*m_Width + j) = 0x00FFFFFF;

					if(NULL != ErrorIndexY)
					{
						//check if the top and left pixels are also mismatched
						if(i > 1 && j > 1)
						{
							if((j-1) == ErrorIndexX && 1 == ErrorIndexY[j])
							{
								bPrecisionError = FALSE;
							}
						}

						ErrorIndexY[j] = 1; //error occurs on this one at current row
					}

					ErrorIndexX = j;
				}
				else
				{
					*(pDiffColors + i*m_Width + j) = 0;

					if(NULL != ErrorIndexY)
					{
						ErrorIndexY[j] = 0; //no error occur on this on at current row
					}
				}
			}
		}	 	
	}

	if(numR > 0)
		ErrorInfo.AveRedError /= numR;
	if(numG > 0)
		ErrorInfo.AveGreenError /= numG;
	if(numB > 0)
		ErrorInfo.AveBlueError /= numB;
	if(ErrorInfo.NumofColorError > 0)
	{
		ErrorInfo.AveTotalError /= ErrorInfo.NumofColorError;
		ErrorInfo.AveTotalError = (int)sqrt((float)ErrorInfo.AveTotalError);
	}

	int maxChannelError = max(ErrorInfo.MaxRedError, ErrorInfo.MaxGreenError);
	maxChannelError = max(maxChannelError, ErrorInfo.MaxBlueError);
	if(maxChannelError > m_iMaxError)
	{
		m_iMaxError = maxChannelError;

		//save bitmaps
		TCHAR FileName[256];
		StringCchPrintf(FileName, 256, _T("%s-Screen.bmp"), ScenarioName);
		if( !m_pScreenBitmap->SaveBitmap(FileName) )
		{
			return FALSE;
		}

		StringCchPrintf(FileName, 256, _T("%s-Memory.bmp"), ScenarioName);
		if( !m_pMemoryBitmap->SaveBitmap(FileName) )
		{
			return FALSE;
		}

		StringCchPrintf(FileName, 256, _T("%s-Diff.bmp"), ScenarioName);
		if( !m_pDiffBitmap->SaveBitmap(FileName) )
		{
			return FALSE;
		}
	}

	//Adding tolerance here
	if(bTolerance)
	{
		if( NULL != ScenarioName && 
			(NULL != StrStrI(ScenarioName, TEXT("TextoutAntialiased")) ||
			 NULL != StrStrI(ScenarioName, TEXT("AlphaBlend")) ) )
		{
			//Tolerance for AlphaBlend and transparent Antialiased Textout
			if( ErrorInfo.MaxRedError <= 2 &&
				ErrorInfo.MaxBlueError <= 2 &&
				ErrorInfo.MaxGreenError <= 2 )
			{
				ErrorInfo.NumofColorError = 0;
			}
		}
		if( NULL != ScenarioName && NULL != StrStrI(ScenarioName, TEXT("StretchBlt")) ) 
		{
			//Using extra Tolerance for StretchBlt
			if( bPrecisionError )
			{
				ErrorInfo.NumofColorError = 0;
			}
		}
	}

	if(NumofAlphaErrors > 0)
	{
		AlphaDiff /= NumofAlphaErrors;
	}

	if(NULL != ErrorIndexY)
	{
		delete ErrorIndexY;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//This version in used for ClearType Textout
BOOL CCompatibleDC::CompareDC(TCHAR* ScenarioName, BOOL bTolerance, COLORREF TextCol, UINT iTolerance)
{
	if(NULL == m_pDiffBitmap->GetBits() || NULL == m_pMemoryBitmap->GetBits() || NULL == m_pScreenBitmap->GetBits())
	{
		return FALSE;
	}

	//DWORD tColor = (DWORD) TextCol;
	//tColor = ((tColor) & 0x00ffffff);//only use the RGB value
	UINT rText = (UINT)GetRValue(TextCol);
	UINT gText = (UINT)GetGValue(TextCol);
	UINT bText = (UINT)GetBValue(TextCol);

	//Compare the resulting contents between two DCs in the corresponding bitmaps pixel by pixel
	memset(&ErrorInfo, 0, sizeof (ErrorInfo));
	ErrorInfo.MaxPixelX = -1;
	ErrorInfo.MaxPixelY = -1;
	int NumofAlphaErrors = 0, AlphaDiff = 0;
	int TotalMatchedPixelinMemDC= 0, TotalMatchedPixelinScreenDC= 0;
	int numR=0, numG=0, numB=0;
	{
		DWORD* pMemoryColors = (DWORD*) m_pMemoryBitmap->GetBits();
		DWORD* pScreenColors = (DWORD*) m_pScreenBitmap->GetBits();
		DWORD* pDiffColors = (DWORD*) m_pDiffBitmap->GetBits();
		DWORD MemoryValue, ScreenValue;
		int Maxdiff = 0, diff, diffr, diffg, diffb, vAlpha1, vAlpha2;
		UINT col1, col2, temp;
		BOOL bNoMismatchedInMem, bNoMismatchedInScreen;

		//ignore the pixels on the edges
		for (UINT i=1; i < m_Height-1; i++)
		{
			for (UINT j=1; j < m_Width-1; j++)
			{
				ScreenValue = *(pScreenColors + i*m_Width + j);
				MemoryValue = *(pMemoryColors + i*m_Width + j);

				vAlpha1 = ((ScreenValue) & 0xff000000)>>24;
				vAlpha2 = ((MemoryValue) & 0xff000000)>>24;

				temp = (vAlpha1-vAlpha2)*(vAlpha1-vAlpha2);
				if(temp > 0)
				{
					AlphaDiff += temp;
					NumofAlphaErrors++;
				}

				bNoMismatchedInScreen = TRUE;
				bNoMismatchedInMem = TRUE;
				diff= 0;

				int MaxColorErr = 0;
				//Blue channel
				{	
					col1 = (UINT)((ScreenValue) & 0x000000ff);
					col2 = (UINT)((MemoryValue) & 0x000000ff);
					diffb = abs((int)(col1 - col2));
					diff += diffb*diffb;
					bNoMismatchedInScreen &= (bText == col1);
					bNoMismatchedInMem &= (bText == col2);
				}

				//Green channel
				{	
					col1 = (UINT)((ScreenValue) & 0x0000ff00)>>8;
					col2 = (UINT)((MemoryValue) & 0x0000ff00)>>8;
					diffg = abs((int)(col1 - col2));
					diff += diffg*diffg;
					bNoMismatchedInScreen &= (gText == col1);
					bNoMismatchedInMem &= (gText == col2);
				}

				//Red channel
				{	
					col1 = (UINT)((ScreenValue) & 0x00ff0000)>>16;
					col2 = (UINT)((MemoryValue) & 0x00ff0000)>>16;
					diffr = abs((int)(col1 - col2));
					diff += diffr*diffr;
					bNoMismatchedInScreen &= (rText == col1);
					bNoMismatchedInMem &= (rText == col2);
				}

				MaxColorErr = max(diffb, diffg);
				MaxColorErr = max(diffr, MaxColorErr);

				//Firstly set the resulting difference image to be black for errors.
				//If there is no error at this pixel, white color will be set thereafter.
				*(pDiffColors + i*m_Width + j) = 0x00FFFFFF;

				//Only count the edge pixels and
				//the errors in interior pixels will be handled later.
				if(*(m_pTemplateBits + i*m_Width + j) < 255 && diff > 0)
				{
					ErrorInfo.NumofColorError++;
					ErrorInfo.AveTotalError += diff;
					ErrorInfo.AveRedError += diffr;
					ErrorInfo.AveGreenError += diffg;
					ErrorInfo.AveBlueError += diffb;
					if(diffr > 0)
					{
						numR++;
						if(ErrorInfo.MaxRedError < diffr)
							ErrorInfo.MaxRedError = diffr;
					}
					if(diffg > 0)
					{
						numG++;
						if(ErrorInfo.MaxGreenError < diffg)
							ErrorInfo.MaxGreenError = diffg;
					}
					if(diffb > 0)
					{
						numB++;
						if(ErrorInfo.MaxBlueError < diffb)
							ErrorInfo.MaxBlueError = diffb;
					}
					if(Maxdiff < MaxColorErr)
					{
						Maxdiff = MaxColorErr;
						ErrorInfo.MaxPixelX = j;
						ErrorInfo.MaxPixelY = i;
						ErrorInfo.RedValue = (int)((MemoryValue) & 0x00ff0000)>>16;
						ErrorInfo.GreenValue = (int)((MemoryValue) & 0x0000ff00)>>8;
						ErrorInfo.BlueValue = (int)((MemoryValue) & 0x000000ff);
					}
				}
				else if( diff > 0 )
				{
					//For errors in interior pixels
					ErrorInfo.NumofInteriorColorError++;
					if (ErrorInfo.MaxInteriorError < MaxColorErr)
					{
						ErrorInfo.MaxInteriorError = MaxColorErr;
					}
				}
				else
				{
					*(pDiffColors + i*m_Width + j) = 0;
				}

				if(bNoMismatchedInScreen)
				{
					TotalMatchedPixelinScreenDC++;
				}
				if(bNoMismatchedInMem)
				{
					TotalMatchedPixelinMemDC++;
				}
			}
		}	 	
	}

	if(numR > 0)
		ErrorInfo.AveRedError /= numR;
	if(numG > 0)
		ErrorInfo.AveGreenError /= numG;
	if(numB > 0)
		ErrorInfo.AveBlueError /= numB;
	if(ErrorInfo.NumofColorError > 0)
	{
		ErrorInfo.AveTotalError /= ErrorInfo.NumofColorError;
		ErrorInfo.AveTotalError = (int)sqrt((float)ErrorInfo.AveTotalError);
	}

	ErrorInfo.NumofMismatchedPixel = TotalMatchedPixelinMemDC - TotalMatchedPixelinScreenDC;
	ErrorInfo.NumofPixelwithTextCol = TotalMatchedPixelinMemDC;

	int maxChannelError = max(ErrorInfo.MaxRedError, ErrorInfo.MaxGreenError);
	maxChannelError = max(maxChannelError, ErrorInfo.MaxBlueError);
	if(maxChannelError > m_iMaxError)
	{
		m_iMaxError = maxChannelError;

		//save bitmaps
		TCHAR FileName[256];
		StringCchPrintf(FileName, 256, _T("%s-Screen.bmp"), ScenarioName);
		if( !m_pScreenBitmap->SaveBitmap(FileName) )
		{
			return FALSE;
		}

		StringCchPrintf(FileName, 256, _T("%s-Memory.bmp"), ScenarioName);
		if( !m_pMemoryBitmap->SaveBitmap(FileName) )
		{
			return FALSE;
		}

		StringCchPrintf(FileName, 256, _T("%s-Diff.bmp"), ScenarioName);
		if( !m_pDiffBitmap->SaveBitmap(FileName) )
		{
			return FALSE;
		}
	}

	//Adding tolerance here
	if(bTolerance)
	{
		if( ErrorInfo.MaxRedError <= (int)iTolerance &&    //Tolerance for transparent ClearType Textout using GammmaRump table
			ErrorInfo.MaxBlueError <= (int)iTolerance &&
			ErrorInfo.MaxGreenError <= (int)iTolerance )
		{
			ErrorInfo.NumofColorError = 0;
		}
	}

	if(NumofAlphaErrors > 0)
	{
		AlphaDiff /= NumofAlphaErrors;
	}

	return TRUE;
}

HGDIOBJ CCompatibleDC::Select(HGDIOBJ hObj)
{
	if(NULL == hObj)
	{
		return NULL;
	}
	HGDIOBJ OldObj = (HGDIOBJ)SelectObject(m_hDC, hObj);
	SelectObject(m_hMemoryDC, hObj);
	if(m_bDeviceBitmap)
		SelectObject(m_hDeviceBitmapDC, hObj);
	return OldObj;
}
COLORREF CCompatibleDC::SetTextCol(COLORREF crColor)
{
	if(NULL == crColor)
	{
		return NULL;
	}
	COLORREF OldCol = SetTextColor(m_hDC, crColor);
	SetTextColor(m_hMemoryDC, crColor);
	if(m_bDeviceBitmap)
		SetTextColor(m_hDeviceBitmapDC, crColor);
	return OldCol;
}
COLORREF CCompatibleDC::SetBKCol(COLORREF crColor)
{
	if(NULL == crColor)
	{
		return NULL;
	}
	COLORREF OldCol = SetBkColor(m_hDC, crColor);
	SetBkColor(m_hMemoryDC, crColor);
	if(m_bDeviceBitmap)
		SetBkColor(m_hDeviceBitmapDC, crColor);

	return OldCol;
}
int CCompatibleDC::SetBKMode(int mode)
{
	int OldMode = SetBkMode(m_hDC, mode);
	SetBkMode(m_hMemoryDC, mode);
	if(m_bDeviceBitmap)
		SetBkMode(m_hDeviceBitmapDC, mode);
	return OldMode;
}

BOOL CCompatibleDC::DoTextout()
{
	RECT r;
	GetClientRect(m_hWnd, &r);
	BOOL result; 

	TEXTMETRIC tm;
	GetTextMetrics(m_hDC, &tm);
	UINT NumLines = m_Height / tm.tmHeight;
	//NumLines = 3; //for debugging purpose

	//TextOut with Transparent mode and default font and no readback
	{
		TCHAR text[ ] = _T("Using DeviceBitmaps 0123456789");
		UINT y = abs(r.top);
		for (UINT n=0; n < NumLines/3; n++)
		{
			result = TextOut(m_hDC, r.left, y, text, ARRAYSIZE(text) - 1);
			if(!result)
			{
				return FALSE;
			}
			y += 3*tm.tmHeight;
		}
		if( !SyncBitmapDC(FALSE) )
		{
			return FALSE;
		}

		//SelectObject(m_hMemoryDC, m_pMemoryBitmap->GetBitmap());
		y = abs(r.top);
		for (UINT n=0; n < NumLines/3; n++)
		{
			result = TextOut(m_hMemoryDC, r.left, y, text, ARRAYSIZE(text) - 1);
			if(!result)
			{
				return FALSE;
			}
			y += 3*tm.tmHeight;
		}

		if(m_bDeviceBitmap)
		{
			y = abs(r.top);
			for (UINT n=0; n < NumLines/3; n++)
			{
				result = TextOut(m_hDeviceBitmapDC, r.left, y, text, ARRAYSIZE(text) - 1);
				if(!result)
				{
					return FALSE;
				}
				y += 3*tm.tmHeight;
			}
		}
	}

	return TRUE;
}

BOOL CCompatibleDC::DoBitBlt(HDC hTempDC, UINT BitmapSizeX, UINT BitmapSizeY, DWORD Rop, float factor)
{
	if(NULL == hTempDC)
	{
		return FALSE;
	}
	if(factor < 0)
	{
		factor = 0.0;
	}
	if(factor > 1)
	{
		factor = 1.0;
	}

	UINT x = (UINT)(m_Width*factor);
	UINT y = (UINT)(m_Height*factor);
	//BitBlt with different rops
	{
		int result;
		result = BitBlt(m_hDC, x, y, BitmapSizeX, BitmapSizeY, hTempDC, 0, 0, Rop);
		if(!result)
		{
			return FALSE;
		}
		if( !SyncBitmapDC(FALSE) )
		{
			return FALSE;
		}
		result = BitBlt(m_hMemoryDC, x, y, BitmapSizeX, BitmapSizeY, hTempDC, 0, 0, Rop);
		if(!result)
		{
			return FALSE;
		}
		if(m_bDeviceBitmap)
		{
			result = BitBlt(m_hDeviceBitmapDC, x, y, BitmapSizeX, BitmapSizeY, hTempDC, 0, 0, Rop);
			if(!result)
			{
				return FALSE;
			}
		}
	}
		
	return TRUE;
}
BOOL CCompatibleDC::DoColorFill(UINT BitmapSizeX, UINT BitmapSizeY, DWORD Rop, float factor)
{
	if(factor < 0)
	{
		factor = 0.0;
	}
	if(factor > 1)
	{
		factor = 1.0;
	}

	UINT x = (UINT)(m_Width*factor);
	UINT y = (UINT)(m_Height*factor);
	//BitBlt with different rops
	{
		int result;
		result = BitBlt(m_hDC, x, y, BitmapSizeX, BitmapSizeY, NULL, 0, 0, Rop);
		if(!result)
		{
			return FALSE;
		}
		if( !SyncBitmapDC(FALSE) )
		{
			return FALSE;
		}
		result = BitBlt(m_hMemoryDC, x, y, BitmapSizeX, BitmapSizeY, NULL, 0, 0, Rop);
		if(!result)
		{
			return FALSE;
		}
		if(m_bDeviceBitmap)
		{
			result = BitBlt(m_hDeviceBitmapDC, x, y, BitmapSizeX, BitmapSizeY, NULL, 0, 0, Rop);
			if(!result)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}
UINT CalculateBlending(UINT SCA, UINT SOA, UINT SCV, UINT DSV)
{
	float rVal, SourceAlpha;

	if(SCA > 255 || SCV > 255 || DSV > 255)
	{
		return 0;
	}

	if(SCA < 255)
	{
		rVal = (float)SCV * (float)SCA / 255.0f;
		SourceAlpha = (float)SOA * (float)SCA / 255.0f;
	}
	else
	{
		rVal = (float)SCV;
		SourceAlpha = (float)SOA;
	}
	if(SOA > 255) //Source Alpha is not set, so only use Source Constant Alpha
	{
		rVal += (1.0f - (float)SCA / 255.0f)*(float)DSV;
	}
	else
	{
		//using Source Alpha
		rVal += (1.0f - SourceAlpha / 255.0f)*(float)DSV;
	}

	//check if color saturation occurs. If so, truncate it to 255
	if(rVal > 255.0f)
	{
		rVal = 255.0f;
	}
	return (UINT)rVal;
}
BOOL CCompatibleDC::DoAlphaBlend(HDC hTempDC, UINT BitmapSizeX, UINT BitmapSizeY, BLENDFUNCTION Blend, BOOL bDoMath)
{
	if(NULL == hTempDC)
	{
		return FALSE;
	}
	//the sizes of source and destination must be same in this case
	if(m_Width != BitmapSizeX ||
	   m_Height != BitmapSizeY)
	{
		return FALSE;
	}

	int result;
	if(bDoMath)
	{
		//firstly copy the contents in the source, i.e. hTempDC, and destination, i.e. hDC, to hMemoryBitmap and hScreenBitmap, respectively
		if (!BitBlt(m_hScreenDC, 0,0, m_Width, m_Height, m_hDC, 0, 0, SRCCOPY))
		{
			return FALSE;
		}
		if (!BitBlt(m_hMemoryDC, 0,0, m_Width, m_Height, hTempDC, 0, 0, SRCCOPY))
		{
			return FALSE;
		}
		//GdiFlush();
		//then do the math for AlphaBlend and save the results in the MemoryDC
		DWORD* pMemoryColors = (DWORD*) m_pMemoryBitmap->GetBits();
		DWORD* pScreenColors = (DWORD*) m_pScreenBitmap->GetBits();
		DWORD DestniationValue, SourceValue, vCol;
		UINT DestinationCol, SourceCol, temp, SourceAlpha;

		//travel all pixels to achieve the alphablend and save the results in hMemoryDC
		for (UINT i=0; i < BitmapSizeY; i++)
		{
			for (UINT j=0; j < BitmapSizeX; j++)
			{
				DestniationValue = *(pScreenColors + i*m_Width + j);
				SourceValue = *(pMemoryColors + i*m_Width + j);

				SourceAlpha = (UINT)((SourceValue & 0xff000000) >> 24);
				vCol = ((DestniationValue) & 0x00ffffff);
				if(vCol > 0)//only apply for clipping regions
				{
					vCol = 0;
					//Red channel
					{	
						DestinationCol = (UINT)((DestniationValue) & 0x000000ff);
						SourceCol = (UINT)((SourceValue) & 0x000000ff);
						if(AC_SRC_ALPHA == Blend.AlphaFormat)
						{
							temp = CalculateBlending((UINT)Blend.SourceConstantAlpha, SourceAlpha, SourceCol, DestinationCol);
						}
						else
						{
							temp = CalculateBlending((UINT)Blend.SourceConstantAlpha, 256, SourceCol, DestinationCol);//non-perpixel alphablend
						}

						vCol |= (DWORD)temp & (0x000000ff);
					}

					//Green channel
					{	
						DestinationCol = (UINT)(((DestniationValue) & 0x0000ff00)>>8);
						SourceCol = (UINT)(((SourceValue) & 0x0000ff00)>>8);
						if(AC_SRC_ALPHA == Blend.AlphaFormat)
						{
							temp = CalculateBlending((UINT)Blend.SourceConstantAlpha, SourceAlpha, SourceCol, DestinationCol);
						}
						else
						{
							temp = CalculateBlending((UINT)Blend.SourceConstantAlpha, 256, SourceCol, DestinationCol);//non-perpixel alphablend
						}

						vCol |= ((DWORD)temp<<8) & (0x0000ff00);
					}

					//Blue channel
					{	
						DestinationCol = (UINT)(((DestniationValue) & 0x00ff0000)>>16);
						SourceCol = (UINT)(((SourceValue) & 0x00ff0000)>>16);
						if(AC_SRC_ALPHA == Blend.AlphaFormat)
						{
							temp = CalculateBlending((UINT)Blend.SourceConstantAlpha, SourceAlpha, SourceCol, DestinationCol);
						}
						else
						{
							temp = CalculateBlending((UINT)Blend.SourceConstantAlpha, 256, SourceCol, DestinationCol);//non-perpixel alphablend
						}

						vCol |= ((DWORD)temp<<16) & (0x00ff0000);
					}

					//Alpha channel
					{
						DestinationCol = (UINT)(((DestniationValue) & 0xff000000)>>24);
						SourceCol = (UINT)(((SourceValue) & 0xff000000)>>24);
						if(AC_SRC_ALPHA == Blend.AlphaFormat)
						{
							temp = CalculateBlending((UINT)Blend.SourceConstantAlpha, SourceAlpha, SourceCol, DestinationCol);
						}
						else
						{
							temp = CalculateBlending((UINT)Blend.SourceConstantAlpha, 256, SourceCol, DestinationCol);//non-perpixel alphablend
						}

						vCol |= ((DWORD)temp<<24) & (0xff000000);
					}
				}
				else
				{
					vCol = 0;
				}
				*(pMemoryColors + i*m_Width + j) = vCol;
			}
		}
	}
	else
	{
		result = AlphaBlend(m_hMemoryDC, 0, 0, m_Width, m_Height, hTempDC, 0, 0, BitmapSizeX, BitmapSizeY, Blend);
		if(!result)
		{
			return FALSE;
		}
	}
	//Do AlphaBlend with blend setting
	{
		result = AlphaBlend(m_hDC, 0, 0, m_Width, m_Height, hTempDC, 0, 0, BitmapSizeX, BitmapSizeY, Blend);
		if(!result)
		{
			return FALSE;
		}
		if( !SyncBitmapDC(FALSE) )
		{
			return FALSE;
		}
		if(m_bDeviceBitmap)
		{
			result = AlphaBlend(m_hDeviceBitmapDC, 0, 0, m_Width, m_Height, hTempDC, 0, 0, BitmapSizeX, BitmapSizeY, Blend);
			if(!result)
			{
				return FALSE;
			}
		}
	}
		
	return TRUE;
}

BOOL CCompatibleDC::DoTransparentBlt(HDC hTempDC, UINT Width, UINT Height, UINT ColorKey)
{
	if(NULL == hTempDC)
	{
		return FALSE;
    }
	//the sizes of source and destination must be same in this case
	if(m_Width != Width ||
		m_Height != Height)
	{
		return FALSE;
	}
	//Do TransparentBlt with color key 
	{
		int result;
		result = TransparentBlt(m_hDC, 0, 0, m_Width, m_Height, hTempDC, 0, 0, Width, Height, ColorKey);
		if(!result)
		{
			return FALSE;
		}
		if( !SyncBitmapDC(FALSE) )
		{
			return FALSE;
		}
		result = TransparentBlt(m_hMemoryDC, 0, 0, m_Width, m_Height, hTempDC, 0, 0, Width, Height, ColorKey);
		if(!result)
		{
			return FALSE;
		}
		if(m_bDeviceBitmap)
		{
			result = TransparentBlt(m_hDeviceBitmapDC, 0, 0, m_Width, m_Height, hTempDC, 0, 0, Width, Height, ColorKey);
			if(!result)
			{
				return FALSE;
			}
		}
	}
	
	return TRUE;
}

BOOL CCompatibleDC::DoStretchBlt(HDC hTempDC, UINT Width, UINT Height, float factor)
{
	if(NULL == hTempDC || 0.0f == factor)
	{
		return FALSE;
	}

	UINT DestLeft = 0;
	UINT DestTop = 0;
	//re-calculate the rectangles of destination and source to match the need for stretchblt
	if(factor < 0.0f)
	{
		DestLeft = Width;
		DestTop = Height;
		//factor *= -1.0f;
		if(factor > -1.0f)
			;//factor = -1.0f*factor - 1.0f;
		else
		{
			Width = (UINT)((float)m_Width/factor*(-1.0f));
			Height = (UINT)((float)m_Height/factor*(-1.0f));
			factor = -1.0f;
		}
	}
	if(factor > 1.0f)
	{
		Width = (UINT)((float)m_Width/factor);
		Height = (UINT)((float)m_Height/factor);
		factor = 1.0f;
	}
	
	//Do TransparentBlt with color key 
	{
		int result;
		result = StretchBlt(m_hDC,         // handle to destination DC
					DestLeft,           // x-coord of destination upper-left corner
					DestTop,           // y-coord of destination upper-left corner
					(int)(factor*m_Width),       // width of destination rectangle
					(int)(factor*m_Height),      // height of destination rectangle
					hTempDC,   // handle to source DC
					0,           // x-coord of source upper-left corner
					0,           // y-coord of source upper-left corner
					Width, // width of source rectangle
					Height,// height of source rectangle
					SRCCOPY      // raster operation code
					);
		if(!result)
		{
			return FALSE;
		}
		if( !SyncBitmapDC(FALSE) )
		{
			return FALSE;
		}
		result = StretchBlt(m_hMemoryDC,         // handle to destination DC
					DestLeft,           // x-coord of destination upper-left corner
					DestTop,           // y-coord of destination upper-left corner
					(int)(factor*m_Width),       // width of destination rectangle
					(int)(factor*m_Height),      // height of destination rectangle
					hTempDC,   // handle to source DC
					0,           // x-coord of source upper-left corner
					0,           // y-coord of source upper-left corner
					Width, // width of source rectangle
					Height,// height of source rectangle
					SRCCOPY      // raster operation code
					);
		if(!result)
		{
			return FALSE;
		}
		if(m_bDeviceBitmap)
		{
			result = StretchBlt(m_hDeviceBitmapDC,         // handle to destination DC
					DestLeft,           // x-coord of destination upper-left corner
					DestTop,           // y-coord of destination upper-left corner
					(int)(factor*m_Width),       // width of destination rectangle
					(int)(factor*m_Height),      // height of destination rectangle
					hTempDC,   // handle to source DC
					0,           // x-coord of source upper-left corner
					0,           // y-coord of source upper-left corner
					Width, // width of source rectangle
					Height,// height of source rectangle
					SRCCOPY      // raster operation code
					);
			if(!result)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}
BOOL DrawHVLine(HDC hDC, UINT Width, UINT Height)
{
	for(int row=30; row<(int)Width-30; row+=70)
	{
		for(int collum=(int)Height-30; collum>30; collum-=70)
		{
			MoveToEx(hDC, 20+row, Height-collum, NULL);
			LineTo(hDC, 20+row, Height-collum);
			LineTo(hDC, 20+row, 20+collum);
			LineTo(hDC, Width-row, 20+collum);
			LineTo(hDC, Width-row, Height-collum);
		}
	}

	return TRUE;
}
BOOL DrawDiagLine(HDC hDC, UINT Width, UINT Height)
{
	for(int row=30; row<(int)Width - 30; row+=70)
	{
		for(int collum=Height - 30; collum>30; collum-=70)
		{
			LineTo(hDC, row, collum);
			LineTo(hDC, collum, row);
		}
	}

	return TRUE;
}
BOOL DrawMixedLine(HDC hDC, UINT Width, UINT Height)
{
	for(int row=30; row<(int)Width - 30; row+=70)
	{
		//MoveTo(row, 500, NULL);
		for(int collum=(int)Height - 30; collum>30; collum-=70)
		{
			LineTo(hDC, 20+row, Height-collum);
			LineTo(hDC, 20+row, 20+collum);
			LineTo(hDC, row, collum);
			LineTo(hDC, collum, row);
		}
	}

	return TRUE;
}
BOOL CCompatibleDC::DoLineTo(UINT nTheme)
{
	//Lineto according to the scheme
	switch(nTheme)
	{
	case 1: //horizontal and vertical lines
		if(!DrawHVLine(m_hDC, m_Width, m_Height))
		{
			return FALSE;
		}
		if( !SyncBitmapDC(FALSE) )
		{
			return FALSE;
		}

		if(!DrawHVLine(m_hMemoryDC, m_Width, m_Height))
		{
			return FALSE;
		}

		if(m_bDeviceBitmap)
		{
			if(!DrawHVLine(m_hDeviceBitmapDC, m_Width, m_Height))
			{
				return FALSE;
			}
		}
		break;
	case 2: //diagnal lines
		if(!DrawDiagLine(m_hDC, m_Width, m_Height))
		{
			return FALSE;
		}
		if( !SyncBitmapDC(FALSE) )
		{
			return FALSE;
		}

		if(!DrawDiagLine(m_hMemoryDC, m_Width, m_Height))
		{
			return FALSE;
		}

		if(m_bDeviceBitmap)
		{
			if(!DrawDiagLine(m_hDeviceBitmapDC, m_Width, m_Height))
			{
				return FALSE;
			}
		}
		break;
	case 3: //mixed lines
		if(!DrawMixedLine(m_hDC, m_Width, m_Height))
		{
			return FALSE;
		}
		if( !SyncBitmapDC(FALSE) )
		{
			return FALSE;
		}

		if(!DrawMixedLine(m_hMemoryDC, m_Width, m_Height))
		{
			return FALSE;
		}

		if(m_bDeviceBitmap)
		{
			if(!DrawMixedLine(m_hDeviceBitmapDC, m_Width, m_Height))
			{
				return FALSE;
			}
		}
		break;
	default:
		break;
	}

	return TRUE;
}
BOOL DrawBezier(HDC hDC)
{
	POINT   OPoints[7] = {
					{ 10, 150 },
					{ 10, 10 },
					{ 280, 10 },
					{ 280, 150 },
					{ 280, 280 },
					{ 10, 280 },
					{ 10, 150 }
				};

	POINT   IPoints[7] = {
					{ 20, 120 },
					{ 20, 200 },
					{ 100, 100 },
					{ 100, 120 },
					{ 100, 100 },
					{ 20, 100 },
					{ 20, 120 }
				};

	BeginPath(hDC);
	{
		if(!PolyBezier(hDC, OPoints, 7))
			return FALSE;
		if(!PolyBezier(hDC, IPoints, 7))
			return FALSE;
	}
	EndPath(hDC);
	if(!StrokePath(hDC))
		return FALSE;

	return TRUE;
}
BOOL DrawEllipse(HDC hDC, UINT Width, UINT Height)
{
	BeginPath(hDC);
	{
		if(!Ellipse( hDC, 0, 0, Width, Height ))
			return FALSE;
		
		if(!MoveToEx(hDC, 0, 0, NULL))
			return FALSE;
		if(!LineTo( hDC, Width, Height))
			return FALSE;
		if(!MoveToEx( hDC, 0, Height, NULL))
			return FALSE;
		if(!LineTo( hDC, Width, 0 ))
			return FALSE;

		if(!Ellipse( hDC, Width/4, Height/4, 2*Width/3, 2*Height/3 ))
			return FALSE;
	}
	EndPath(hDC);
	if(!StrokePath(hDC))
		return FALSE;

	return TRUE;
}
BOOL CCompatibleDC::DoStrokePath(UINT nTheme)
{
	switch(nTheme)
	{
	case 1: //Curves
		//Do StrokePath 
		{
			if(!DrawBezier(m_hDC))
			{
				return FALSE;
			}
			if( !SyncBitmapDC(FALSE) )
			{
				return FALSE;
			}

			if(!DrawBezier(m_hMemoryDC))
			{
				return FALSE;
			}
			
			if(m_bDeviceBitmap)
			{
				if(!DrawBezier(m_hDeviceBitmapDC))
				{
					return FALSE;
				}
			}
		}
		break;
	case 2: //Ellipse and lines
		if(!DrawEllipse(m_hDC, m_Width, m_Height))
		{
			return FALSE;
		}
		if( !SyncBitmapDC(FALSE) )
		{
			return FALSE;
		}

		if(!DrawEllipse(m_hMemoryDC, m_Width, m_Height))
		{
			return FALSE;
		}

		if(m_bDeviceBitmap)
		{
			if(!DrawEllipse(m_hDeviceBitmapDC, m_Width, m_Height))
			{
				return FALSE;
			}
		}
		break;
	default:
		break;
	}

	return TRUE;
}
BOOL CCompatibleDC::DoMaskBlt(HDC hSrcDC, UINT BitmapSizeX, UINT BitmapSizeY, HBITMAP hMskBmp, DWORD fRop, DWORD bRop)
{
	//Generate the rop for MaskBlt
	DWORD mRop = MAKEROP4(fRop, bRop);

	//Always do MaskBlt in the center of destination
	int x = m_Width/2 - BitmapSizeX/2;
	int y = m_Height/2 - BitmapSizeY/2;

	//MaskBlt with different rops
	{
		int result;
		result = MaskBlt(m_hDC, x, y, BitmapSizeX, BitmapSizeY, hSrcDC, 0, 0, hMskBmp, 0, 0, mRop);
		if(!result)
		{
			return FALSE;
		}
		if( !SyncBitmapDC(FALSE) )
		{
			return FALSE;
		}
		result = MaskBlt(m_hMemoryDC, x, y, BitmapSizeX, BitmapSizeY, hSrcDC, 0, 0, hMskBmp, 0, 0, mRop);
		if(!result)
		{
			return FALSE;
		}
		if(m_bDeviceBitmap)
		{
			result = MaskBlt(m_hDeviceBitmapDC, x, y, BitmapSizeX, BitmapSizeY, hSrcDC, 0, 0, hMskBmp, 0, 0, mRop);
			if(!result)
			{
				return FALSE;
			}
		}
	}
		
	return TRUE;
}
BOOL CCompatibleDC::SelectRgn(HRGN hRgn, int fnMode)
{
	if(NULL == hRgn || RGN_COPY != fnMode)
		return FALSE;

	if(RGN_AND == fnMode ||
		RGN_COPY == fnMode ||
		RGN_DIFF == fnMode ||
		RGN_OR == fnMode ||
		RGN_XOR == fnMode)
	{
		BOOL bResult = TRUE;
		bResult = (ERROR != ExtSelectClipRgn(m_hDC, hRgn, fnMode));
		bResult &= (ERROR != ExtSelectClipRgn(m_hMemoryDC, hRgn, fnMode));
		if(m_bDeviceBitmap)
			bResult &= (ERROR != ExtSelectClipRgn(m_hDeviceBitmapDC, hRgn, fnMode));
		return bResult;
	}
	return FALSE;
}

BOOL CCompatibleDC::SelectRgn(HRGN hRgn)
{
	BOOL bResult = TRUE;
	bResult = (ERROR != SelectClipRgn(m_hDC, hRgn));
	bResult &= (ERROR != SelectClipRgn(m_hMemoryDC, hRgn));
	if(m_bDeviceBitmap)
		bResult &= (ERROR != SelectClipRgn(m_hDeviceBitmapDC, hRgn));
	return bResult;
}

BOOL CCompatibleDC::ExcludeRect(int nL, int nT, int nR, int nB)
{
	if(nL > nR || nT > nB)
		return FALSE;
	
	BOOL bResult = TRUE;
	bResult = (ERROR != ExcludeClipRect(m_hDC, nL, nT, nR, nB));
	bResult &= (ERROR != ExcludeClipRect(m_hMemoryDC, nL, nT, nR, nB));
	if(m_bDeviceBitmap)
		bResult &= (ERROR != ExcludeClipRect(m_hDeviceBitmapDC, nL, nT, nR, nB));
	return bResult;
}

BOOL CCompatibleDC::IntersectRect(int nL, int nT, int nR, int nB)
{
	if(nL > nR || nT > nB)
		return FALSE;
	
	BOOL bResult = TRUE;
	bResult = (ERROR != IntersectClipRect(m_hDC, nL, nT, nR, nB));
	bResult &= (ERROR != IntersectClipRect(m_hMemoryDC, nL, nT, nR, nB));
	if(m_bDeviceBitmap)
		bResult &= (ERROR != IntersectClipRect(m_hDeviceBitmapDC, nL, nT, nR, nB));
	return bResult;
}