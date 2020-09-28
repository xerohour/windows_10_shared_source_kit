//////////////////////////////////////////////////////////////////////
// File:  D3DWrappers.h
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Type definitions, macros, and helper classes for adapting Direct3D
// 10 code to compile and run on Direct3D 11.
//
// History:
// 08 Feb 2008  SEdmison   Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Build flags
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

#include <windows.h>
#include <basetsd.h>
#include <dwmapi.h>


//
// Direct3D headers
//

#include <dxgi.h>
#include <d3d11.h>
#include <d3d10_1.h>
#include <d3dx10.h>


//
// STL headers
//

#include <vector>
#include <algorithm>


//
// Project headers
//

#include "WGFTestCore.h"
#include "WGFTestUI.h"


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

//
// Direct3D version-specific type wrappers
//

typedef D3D10_DEVICE_POINTER D3D_DEVICE_POINTER;

typedef D3D10_MAPPED_TEXTURE2D D3D_MAPPED_TEXTURE2D;
typedef D3D10_SHADER_RESOURCE_VIEW_DESC D3D_SHADER_RESOURCE_VIEW_DESC;
typedef D3D10_SRV_DIMENSION D3D_SRV_DIMENSION;
typedef D3D10_TEXTURE2D_DESC D3D_TEXTURE2D_DESC;
typedef D3D10_USAGE D3D_USAGE;

typedef ID3D10Buffer ID3DBuffer;
typedef ID3D10Device ID3DDevice;
typedef ID3D10RenderTargetView ID3DRenderTargetView;
typedef ID3D10Resource ID3DResource;
typedef ID3D10ShaderResourceView ID3DShaderResourceView;
typedef ID3D10Texture2D ID3DTexture2D;

#define IID_ID3DDevice IID_ID3D10Device
#define IID_ID3DResource IID_ID3D10Resource
