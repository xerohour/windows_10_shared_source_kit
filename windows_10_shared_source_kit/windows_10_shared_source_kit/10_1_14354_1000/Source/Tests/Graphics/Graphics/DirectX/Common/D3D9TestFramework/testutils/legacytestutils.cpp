//
//  TstUtils.cpp
//
//  Implementation file for functions declared in TstUtils.h
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 1997, Microsoft Corporation.
//
//  See the file "TstUtils.h" for additional information.
//

#include "TestUtilsPCH.h"

#include "d3d.h"
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "TestUtils.h"
#include "D3DUtils.h"
#include <strsafe.h>


// ----------------------------------------------------------------------------

void SetVertex(D3DVERTEX *lpVrt,
               float x, float y, float z,
               float nx, float ny, float nz)
{
    // set homogeneous coordinates
    //    These describe the location of the vertex in "model" coordinates.
    lpVrt->x = D3DVAL(x);  
    lpVrt->y = D3DVAL(y);  
    lpVrt->z = D3DVAL(z);  

    // set normal coordinates
    //    These describe a vector determining which way the vertex is
    //    facing, used by the lighting module.
    lpVrt->nx = D3DVAL(nx);            
    lpVrt->ny = D3DVAL(ny);           
    lpVrt->nz = D3DVAL(nz);

    // set texture coordinates
    lpVrt->tu = D3DVAL(0.0);
    lpVrt->tv = D3DVAL(0.0);
}

// ----------------------------------------------------------------------------

void SetVertex(D3DTLVERTEX *lpVrt, float x, float y, float z)
{
    // set coordinates
    //    These describe the location of the vertex in screen coordinates.
    lpVrt->sx = D3DVAL(x);
    lpVrt->sy = D3DVAL(y);
    lpVrt->sz = D3DVAL(z);

    // protect against a divide be zero fault for rhw.
    if (z == 0.0f)
        z = 0.00001f;

    // set homogeneous W value
    lpVrt->rhw = D3DVAL(1.0/z);
//  lpVrt->rhw = D3DVAL(z);
}

// ----------------------------------------------------------------------------

void SetTexture(D3DTLVERTEX *lpVrt, float u, float v, int nColor)
{
    switch (nColor)
    {
        case 0:
            // Black
            lpVrt->color = RGBA_MAKE(0, 0, 0, 255);
            break;
        case 1:
            // Red
            lpVrt->color = RGBA_MAKE(255, 0, 0, 255);
            break;
        case 2:
            // Green
            lpVrt->color = RGBA_MAKE(0, 255, 0, 255);
            break;
        case 3:
            // Blue
            lpVrt->color = RGBA_MAKE(0, 0, 255, 255);
            break;
        case 4:
            // Yellow
            lpVrt->color = RGBA_MAKE(255, 255, 0, 255);
            break;
        case 5:
            // Magenta
            lpVrt->color = RGBA_MAKE(255, 0, 255, 255);
            break;
        case 6:
            // Teal
            lpVrt->color = RGBA_MAKE(0, 255, 255, 255);
            break;
        case 7:
            // White
            lpVrt->color = RGBA_MAKE(255, 255, 255, 255);
            break;
    }

    lpVrt->specular = RGB_MAKE(0, 0, 0);
    lpVrt->tu = D3DVAL(u);
    lpVrt->tv = D3DVAL(v);
}


// ----------------------------------------------------------------------------

long UseConformanceRasterizer(int bUseCRaster)
{
    HRESULT ret;

    if(bUseCRaster)
    {
        HKEY hKey;
        LPDWORD lpdwDisposition = NULL;

        // Create the registry key needed to use the conformance rasterizer.
        ret = RegCreateKeyEx(
            HKEY_LOCAL_MACHINE,                            // handle of an open key 
            "Software\\Microsoft\\Direct3D\\Conformance",  // address of subkey name 
            0,                                             // reserved, must be zero. 
            NULL,                                          // address of class string 
            REG_OPTION_VOLATILE,                           // special options flag -- REG_OPTION_VOLATILE keeps the key in memory. 
            KEY_ALL_ACCESS,                                // desired security access 
            NULL,                                          // address of key security structure 
            &hKey,                                         // address of buffer for opened handle  
            lpdwDisposition);                              // address of disposition value buffer 

        if (ret == ERROR_SUCCESS)
            RegCloseKey(hKey);
    }
    else // bUseCRaster == false
    {
        // Remove the registry key.
        ret = RegDeleteKey(HKEY_LOCAL_MACHINE,                       // handle of open key 
                     "Software\\Microsoft\\Direct3D\\Conformance");  // address of name of subkey to open 
    }

    return ret;
}

// ----------------------------------------------------------------------------

TCHAR * GetFogModeString(D3DFOGMODE fogMode)
{
    switch(fogMode)
    {
        case D3DFOG_NONE:
            return "D3DFOG_NONE";
        case D3DFOG_EXP:
            return "D3DFOG_EXP";
        case D3DFOG_EXP2:
            return "D3DFOG_EXP2";
        case D3DFOG_LINEAR:
            return "D3DFOG_LINEAR";
        default:
            return "Unknown Fog Type";
    }
}

// ----------------------------------------------------------------------------

HRESULT BltSurface(HWND hWnd, LPDIRECTDRAWSURFACE pSource,
                   LPDIRECTDRAWSURFACE pDest, bool bFullscreen)
{
    RECT            rSrc,rDst;
    HRESULT         Result;
    POINT           pt;
    DDSURFACEDESC   ddSurf;

    // Get the Width and Height from the source surface
    memset(&ddSurf,0,sizeof(DDSURFACEDESC));
    ddSurf.dwSize = sizeof(DDSURFACEDESC);
    Result = pSource->GetSurfaceDesc(&ddSurf);

    if (DD_OK != Result)
        return Result;

    // Setup the default rects
    SetRect(&rSrc, 0, 0, ddSurf.dwWidth, ddSurf.dwHeight);
    SetRect(&rDst, 0, 0, ddSurf.dwWidth, ddSurf.dwHeight);

    // Calculate the client area of the primary to blit to
    pt.x=pt.y=0;

    if (!bFullscreen)
        ClientToScreen(hWnd,&pt);

    rDst.top    += pt.y;
    rDst.left   += pt.x;
    rDst.bottom += pt.y;
    rDst.right  += pt.x;

    Result = pDest->Blt(&rDst, pSource, &rSrc, DDBLT_WAIT, NULL);

    return Result;
}

// ----------------------------------------------------------------------------

int GetStrictLevel(void)
{
    TCHAR    CmdLine[512];
    LPSTR   lpOption = NULL;
    int     nLevel;

    // Get the command line from windows
    strcpy(CmdLine, GetCommandLine());

    // Advance the command line to the options
    lpOption = strtok(CmdLine, " -");
    lpOption = strtok(NULL, " -");

    if (lpOption != NULL)
    {
        // Parse the command line, looking for 'L' or 'l'
        while (lpOption != NULL ) 
        {
            if ((*lpOption == 'L') || (*lpOption == 'l'))
            {
                // Found the option, now get the number
                if (lpOption[1] == '\0')
                {
                    lpOption = strtok(NULL, " -");

                    // No number set, return default
                    if (lpOption == NULL)
                        return -1;

                    nLevel = atoi(lpOption);
                }
                else
                {
                    nLevel = atoi(lpOption+1);
                }

                if (nLevel > 5)
                    return 5;
                else
                    return nLevel;
            }

            lpOption = strtok(NULL, " -");
        }
    }

    // Level not set
    return -1;
}

// ----------------------------------------------------------------------------

long DrawCircle(LPDIRECT3DDEVICE2 pDevice, LPD3DVECTOR pCenter, float Radius,
    int nDivisions, D3DCOLOR Color, D3DCOLOR Specular)
{
    int             i, NumVertices;
    float           Theta, Rads;
    LPD3DTLVERTEX   pVertex;

    NumVertices = nDivisions + 2;
    Theta = 360.0f / nDivisions;

    // malloc pVertex based on NumVertices
    pVertex = (LPD3DTLVERTEX) malloc(sizeof(D3DTLVERTEX) * NumVertices);

    if(!pVertex)
        return E_FAIL;
        
    ZeroMemory(pVertex, sizeof(D3DTLVERTEX) * NumVertices);

    // Initialize the center point
    (&pVertex[0])->sx = pCenter->x;
    (&pVertex[0])->sy = pCenter->y;
    (&pVertex[0])->sz = pCenter->z;
    (&pVertex[0])->rhw = D3DVAL(1.0);
    (&pVertex[0])->color = Color;
    (&pVertex[0])->specular = Specular;
    (&pVertex[0])->tu = D3DVAL(1.0);
    (&pVertex[0])->tv = D3DVAL(1.0);

    for (i=1; i<NumVertices; i++)
    {
        // cos takes a radian value, so convert.
        Rads = (Theta * (i-1) * pi)/180.0f;

        (&pVertex[i])->sx = (float)(pCenter->x + Radius * cos(Rads));
        (&pVertex[i])->sy = (float)(pCenter->y + Radius * sin(Rads));
        (&pVertex[i])->sz = pCenter->z;
        (&pVertex[i])->rhw = D3DVAL(1.0);
        (&pVertex[i])->color = Color;
        (&pVertex[i])->specular = Specular;
        (&pVertex[i])->tu = D3DVAL(1.0);
        (&pVertex[i])->tv = D3DVAL(1.0);
    }

    pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,D3DVT_TLVERTEX,(LPVOID)pVertex,NumVertices,0);

    if(pVertex)
        free(pVertex);

    return ERROR_SUCCESS;
}

// ----------------------------------------------------------------------------

long MakeCircle(D3DHVERTEX * pVertices, D3DVECTOR dvecCenter, float Radius, int nDivisions)
{
    int          i, nVertices;
    float        fTheta, fRads;

    nVertices = nDivisions + 2;
    fTheta = 360.0f / nDivisions;

    if(!pVertices)
        return E_INVALIDARG;

    // clear the memory
    memset(pVertices, 0, sizeof(D3DHVERTEX) * nVertices);

    // Initialize the center point
    (&pVertices[0])->dvHX = dvecCenter.x;
    (&pVertices[0])->dvHY = dvecCenter.y;
    (&pVertices[0])->dvHZ = dvecCenter.z;

    for (i = 1; i < nVertices; i++)
    {
        // cos takes a radian value, so convert.
        fRads = (fTheta * (i-1) * pi) / 180.0f;

        (&pVertices[i])->dvHX = (float)(dvecCenter.x + Radius * cos(fRads));
        (&pVertices[i])->dvHY = (float)(dvecCenter.y + Radius * sin(fRads));
        (&pVertices[i])->dvHZ = dvecCenter.z;
    }

    return NOERROR;
}

// ----------------------------------------------------------------------------

unsigned long GetRefCount(LPVOID pInterface)
{
    if(!pInterface)
        return 0;

    IUnknown * pUnk = (IUnknown *)pInterface;
    pUnk->AddRef();
    return pUnk->Release();
}

// ----------------------------------------------------------------------------

int fCompare(double fInput1, double fInput2, int nPrecision)
{
    double fdiff;    // difference between fInput1 and fInput2;
    double fmask;     // used to find the difference within the precision.

    // step 1, find the difference between the two numbers.
    fdiff = fInput1 - fInput2;
    
    // step 2, divide the difference by the precision, and get the integer
    // component.
    fmask = pow((double)10.0, (double)nPrecision);
    return (int)(fdiff * fmask);  // return <0 for less, 0 for equal, >0 for greater
}

// ----------------------------------------------------------------------------

int GetTexFmtDesc( const PIXELFORMAT * pDDPF, PTSTR ptcsDesc, int nChars )
{
    int    nLen;
    int    nBits, nABits, nRBits, nGBits, nBBits;
    bool   bAlpha;
    DWORD  dwFlags;

    int     nCharsLeftInBuffer = nChars;
    TCHAR * ptcsCurPos = ptcsDesc;

    //
    // let's first make sure we don't get any access violations!
    //

    if((!pDDPF) || (!ptcsDesc))
        return 0;

    //
    // get the number of bits and flags for this format
    //

    nBits   = (int)pDDPF->dwRGBBitCount;
    dwFlags = pDDPF->dwFlags;
    nLen    = nABits = nRBits = nGBits = nBBits = 0;
    bAlpha  = false;

    //
    // get ARGB info
    //

    // Is this a FourCC format?
    if(dwFlags & DDPF_FOURCC)
    {
        TCHAR   tcsFourCC[5] = {0};
        DWORD   dwFourCC = pDDPF->dwFourCC;

        tcsFourCC[0] = (TCHAR)(dwFourCC & 0xff);
        tcsFourCC[1] = (TCHAR)((dwFourCC >> 8) & 0xff);
        tcsFourCC[2] = (TCHAR)((dwFourCC >> 16) & 0xff);
        tcsFourCC[3] = (TCHAR)((dwFourCC >> 24) & 0xff);
        tcsFourCC[4] = _T('\0');

        _sntprintf_s(
            ptcsCurPos, nCharsLeftInBuffer + 1, _TRUNCATE, _T("FourCC \"%s\""), tcsFourCC);

        nLen = _tcslen(ptcsCurPos);
        nCharsLeftInBuffer -= nLen;
        ptcsCurPos += nLen;
    }
    else
    {
        // Does this format have alpha pixels?
        if(dwFlags & DDPF_ALPHAPIXELS)
        {
            ULONG m;

            // Count the number of Alpha bits
            for (m = pDDPF->dwRGBAlphaBitMask; !(m & 1); m >>= 1);
            for (nABits = 0; m & 1; nABits++, m >>= 1);

            bAlpha = true;
        }

        if((dwFlags & DDPF_PALETTEINDEXED8) || (dwFlags & DDPF_PALETTEINDEXED4) ||
            (dwFlags & DDPF_PALETTEINDEXED2) || (dwFlags & DDPF_PALETTEINDEXED1))
        {
           _tcsncat(ptcsCurPos, _T("Palettized "), nCharsLeftInBuffer);
            nLen = _tcslen(ptcsCurPos);
            nCharsLeftInBuffer -= nLen;
            ptcsCurPos += nLen;
        }
        else if(dwFlags & DDPF_RGB) // is this format an RGB format?
        {
            ULONG m;

            if(pDDPF->dwRBitMask)
            {
                for (m = pDDPF->dwRBitMask; !(m & 1); m >>= 1);
                for (nRBits = 0; m & 1; nRBits++, m >>= 1);
            }

            if(pDDPF->dwGBitMask)
            {
                for (m = pDDPF->dwGBitMask; !(m & 1); m >>= 1);
                for (nGBits = 0; m & 1; nGBits++, m >>= 1);
            }

            if(pDDPF->dwBBitMask)
            {
                for (m = pDDPF->dwBBitMask; !(m & 1); m >>= 1);
                for (nBBits = 0; m & 1; nBBits++, m >>= 1);
            }

            if(bAlpha)
            {
                _sntprintf_s(
                    ptcsCurPos, nCharsLeftInBuffer + 1, _TRUNCATE, _T("%d%d%d%d ARGB "),
                    nABits, nRBits, nGBits, nBBits);
            }
            else
            {
                _sntprintf_s(
                    ptcsCurPos, nCharsLeftInBuffer + 1, _TRUNCATE, _T("%d%d%d RGB "),
                    nRBits, nGBits, nBBits);
            }

            nLen = _tcslen(ptcsCurPos);
            nCharsLeftInBuffer -= nLen;
            ptcsCurPos += nLen;
        }

        _sntprintf_s(ptcsCurPos, nCharsLeftInBuffer + 1, _TRUNCATE, _T("%d-bit"), nBits);
        nLen = _tcslen(ptcsCurPos);
        nCharsLeftInBuffer -= nLen;
        ptcsCurPos += nLen;

        if(dwFlags & DDPF_LUMINANCE)
        {
            StringCchCat(
                ptcsCurPos,
                nCharsLeftInBuffer,
                _T(" Luminance"));

            nLen = _tcslen(ptcsCurPos);
            nCharsLeftInBuffer -= nLen;
            ptcsCurPos += nLen;
        }
    }

    return _tcslen(ptcsDesc);
}


// ============================================================================
// End of File
