#include "DXGISurfUtils.h"
#include "DXGIValueSets.h"

#include <d3d10.h>

DXGI_MAPPED_RECT CTestDXGISurfaceUtils::dxgilr;
UINT CTestDXGISurfaceUtils::uiBitsPerTexel;
UINT CTestDXGISurfaceUtils::uiBytesPerTexel;

CTestDXGISurfaceUtils::CTestDXGISurfaceUtils()
{
}

CTestDXGISurfaceUtils::~CTestDXGISurfaceUtils()
{
}

void CTestDXGISurfaceUtils::InitializeSubrect(RECT rect, BYTE byteTwoBitVal, UINT uiModifiedBitsOffset)
{
}

bool CTestDXGISurfaceUtils::InitializeLockableSurface(IDXGISurface *pInitSurface)
{
	bool bReturn = false;

	if (pInitSurface)
	{
		HRESULT hresult;

		// Determine the surface properties.

		DXGI_SURFACE_DESC dxgisd;

		// Call GetDesc to obtain surface dimensions and format.

		// DebugOut.Write("Attempting GetDesc.\n");

		dxgisd.Width = 32;
		dxgisd.Height = 32;
		dxgisd.Format = DXGI_FORMAT_R10G10B10A2_UNORM;

		/*
		if (FAILED((hresult = pInitSurface -> GetDesc(&dxgisd))))
		{
			DebugOut.Write("GetDesc failed.\n");
			GetFramework()->WriteToLog("GetSurfaceDesc failed in InitializeLockableSurface.\n");
		}
		else
		*/
			if ((!(dxgisd.Width)) || (!(dxgisd.Height)))
				GetFramework()->WriteToLog("Test error: GetSurfaceDesc reported zero-dimension surface.\n");
			else
			{
				// Attempt to access the framework's record for the format.

				const DXGIFormatInfo *pBBFormatInfo = NULL;

				if (!(pBBFormatInfo = CFormatInfoMap::Find(dxgisd.Format)))
					GetFramework()->WriteToLog("Test error in InitializeLockableSurface: No framework record found for surface format.\n");
				else
					if (!(uiBitsPerTexel = pBBFormatInfo -> uBitsPerElement))
						GetFramework()->WriteToLog("Test error in InitializeLockableSurface: Framework reports zero bits per texel for surface format.\n");
					else
						if ((DWORD)uiBitsPerTexel & 7)
							GetFramework()->WriteToLog("Test error in InitializeLockableSurface: Function requires that each texel be represented by an integral number of bytes.\n");
						else
						{
							ID3D10Texture2D *pTexture2D = NULL;

							// Attempt to retrieve a ID3D10Texture2D interface from the surface.

							if
							(
								FAILED
								(
									(
										hresult =
										pInitSurface ->
										QueryInterface
										(
											__uuidof(ID3D10Texture2D),
											(LPVOID *)(&pTexture2D)
										)
									)
								)
							)
							{
								GetFramework()->WriteToLog("Failed to retrieve ID3D10Texture2D interface in InitializeLockableSurface.\n");
								DebugOut.Write("QI failed.\n");
							}

							/*
							else
							{
								D3D10_MAPPED_TEXTURE2D pMappedTex2D;

								if (FAILED((hresult = pTexture2D -> Map(0, D3D10_MAP_WRITE, 0, &pMappedTex2D))))
								{
									GetFramework()->WriteToLog("Map failed on back buffer resource in InitializeLockableSurface.\n");
									DebugOut.Write("Map failed.\n");
								}
								else
									pTexture2D -> Unmap(0);

								pTexture2D -> Release();

								return true;
							}
							*/

							else
							{
								UINT uiBytesPerRow, uiBufferSizeInBytes;
								BYTE *pBuffer = NULL;

								uiBytesPerTexel = uiBitsPerTexel / 8;
								uiBytesPerRow = dxgisd.Width * uiBytesPerTexel;
								uiBufferSizeInBytes = uiBytesPerRow * dxgisd.Height;

								if (!(pBuffer = new BYTE[uiBufferSizeInBytes]))
								{
									GetFramework()->WriteToLog("Failed to allocate buffer in InitializeLockableSurface.\n");
									DebugOut.Write("new failed.\n");
								}
								else
								{
									ID3D10Device *pDevice = NULL;
									memset((void *)pBuffer, 0xFF, uiBufferSizeInBytes);

									/*
									if
									(
										FAILED
										(
											(
												hresult =
												pTexture2D -> UpdateSubresource
												(
													0,
													NULL,
													(void *)pBuffer,
													uiBytesPerRow,
													uiBufferSizeInBytes
												)
											)
										)
									)
									{
										GetFramework()->WriteToLog("UpdateSubresource failed in InitializeLockableSurface.\n");
										DebugOut.Write("UpdateSubresource failed.\n");
									}
									else
										bReturn = true;
									*/

									pTexture2D -> GetDevice( &pDevice );
									pDevice-> UpdateSubresource
									(
										pTexture2D,
										0,
										NULL,
										(void *)pBuffer,
										uiBytesPerRow,
										uiBufferSizeInBytes
									);
									pDevice->Release();

									bReturn = true;

									delete [] pBuffer;
								}

								pTexture2D -> Release();
							}

							/*
							else
							// (If LockRect were available ...)
							{
								// Clear the surface.

								UINT uiBytesPerRow = dxgisd.Width * uiBytesPerTexel;
								UINT uiRowsRemaining = dxgisd.Height;
								BYTE *pCurrentRow = dxgilr.pBits;

								do
								{
									memset((void *)pCurrentRow, 0xFFFF0000, uiBytesPerRow);
									pCurrentRow += dxgilr.Pitch;
								}
								while (--uiRowsRemaining);

								pInitSurface -> UnlockRect();

								return true;
							}
							*/
						}
			}
	}

	return bReturn;
}

/*
{
	// Attempt surface lock.

	DXGI_MAPPED_RECT dxgilr;

	if (FAILED((hresult = pBB -> LockRect(&dxgilr, NULL, 0))))
		GetFramework()->WriteToLog("LockRect failed in ExecuteTestCase.\n");
	else
	{
		// Modify the back buffer.

		UINT uiRowsRemaining = 32;
		UINT uiNoOfBytesPerRow = (UINT)((DWORD)32 << 2);
		BYTE *pRowStart = dxgilr.pBits;
		BYTE bVal = 0;

		do
		{
			UINT uiBytesRemaining = uiNoOfBytesPerRow;
			BYTE *pModifyByte = pRowStart;

			do
			{
				*(pModifyByte++) = bVal++;
			}
			while (--uiBytesRemaining);

			pRowStart += dxgilr.Pitch;
		}
		while (--uiRowsRemaining);

		pBB -> UnlockRect();
	}

	return true;
}
*/
