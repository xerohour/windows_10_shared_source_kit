/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  Content: Graphics device - Resource format helper
 *
 ***************************************************************************/

#include "FormatDesc.hpp"

#define R D3D10FCN_R
#define G D3D10FCN_G
#define B D3D10FCN_B
#define A D3D10FCN_A
#define D D3D10FCN_D
#define S D3D10FCN_S
#define X D3D10FCN_X

#define _TYPELESS   D3D10FCI_TYPELESS
#define _FLOAT      D3D10FCI_FLOAT
#define _SNORM      D3D10FCI_SNORM
#define _UNORM      D3D10FCI_UNORM
#define _SINT       D3D10FCI_SINT
#define _UINT       D3D10FCI_UINT
#define _UNORM_SRGB D3D10FCI_UNORM_SRGB

// --------------------------------------------------------------------------------------------------------------------------------
// Format Cast Sets
DXGI_FORMAT D3D10FCS_UNKNOWN[] =
{
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R32G32B32A32[] =
{
    DXGI_FORMAT_R32G32B32A32_TYPELESS,
    DXGI_FORMAT_R32G32B32A32_FLOAT,
    DXGI_FORMAT_R32G32B32A32_UINT,
    DXGI_FORMAT_R32G32B32A32_SINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R32G32B32[] =
{
    DXGI_FORMAT_R32G32B32_TYPELESS,
    DXGI_FORMAT_R32G32B32_FLOAT,
    DXGI_FORMAT_R32G32B32_UINT,
    DXGI_FORMAT_R32G32B32_SINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R16G16B16A16[] =
{
    DXGI_FORMAT_R16G16B16A16_TYPELESS,
    DXGI_FORMAT_R16G16B16A16_FLOAT,
    DXGI_FORMAT_R16G16B16A16_UNORM,
    DXGI_FORMAT_R16G16B16A16_UINT,
    DXGI_FORMAT_R16G16B16A16_SNORM,
    DXGI_FORMAT_R16G16B16A16_SINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R32G32[] =
{
    DXGI_FORMAT_R32G32_TYPELESS,
    DXGI_FORMAT_R32G32_FLOAT,
    DXGI_FORMAT_R32G32_UINT,
    DXGI_FORMAT_R32G32_SINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R32G8X24[] =
{
    DXGI_FORMAT_R32G8X24_TYPELESS,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R10G10B10A2[] =
{
    DXGI_FORMAT_R10G10B10A2_TYPELESS,
    DXGI_FORMAT_R10G10B10A2_UNORM,
    DXGI_FORMAT_R10G10B10A2_UINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R11G11B10[] =
{
    DXGI_FORMAT_R11G11B10_FLOAT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R8G8B8A8[] =
{
    DXGI_FORMAT_R8G8B8A8_TYPELESS,
    DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
    DXGI_FORMAT_R8G8B8A8_UINT,
    DXGI_FORMAT_R8G8B8A8_SNORM,
    DXGI_FORMAT_R8G8B8A8_SINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R16G16[] =
{
    DXGI_FORMAT_R16G16_TYPELESS,
    DXGI_FORMAT_R16G16_FLOAT,
    DXGI_FORMAT_R16G16_UNORM,
    DXGI_FORMAT_R16G16_UINT,
    DXGI_FORMAT_R16G16_SNORM,
    DXGI_FORMAT_R16G16_SINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R32[] =
{
    DXGI_FORMAT_R32_TYPELESS,
    DXGI_FORMAT_D32_FLOAT,
    DXGI_FORMAT_R32_FLOAT,
    DXGI_FORMAT_R32_UINT,
    DXGI_FORMAT_R32_SINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R24G8[] =
{
    DXGI_FORMAT_R24G8_TYPELESS,
    DXGI_FORMAT_D24_UNORM_S8_UINT,
    DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
    DXGI_FORMAT_X24_TYPELESS_G8_UINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R8G8[] =
{
    DXGI_FORMAT_R8G8_TYPELESS,
    DXGI_FORMAT_R8G8_UNORM,
    DXGI_FORMAT_R8G8_UINT,
    DXGI_FORMAT_R8G8_SNORM,
    DXGI_FORMAT_R8G8_SINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R16[] =
{
    DXGI_FORMAT_R16_TYPELESS,
    DXGI_FORMAT_R16_FLOAT,
    DXGI_FORMAT_D16_UNORM,
    DXGI_FORMAT_R16_UNORM,
    DXGI_FORMAT_R16_UINT,
    DXGI_FORMAT_R16_SNORM,
    DXGI_FORMAT_R16_SINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R8[] =
{
    DXGI_FORMAT_R8_TYPELESS,
    DXGI_FORMAT_R8_UNORM,
    DXGI_FORMAT_R8_UINT,
    DXGI_FORMAT_R8_SNORM,
    DXGI_FORMAT_R8_SINT,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};
DXGI_FORMAT D3D10FCS_A8[] =
{
    DXGI_FORMAT_A8_UNORM,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R1[] =
{
    DXGI_FORMAT_R1_UNORM,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R9G9B9E5[] =
{
    DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_R8G8_B8G8[] =
{
    DXGI_FORMAT_R8G8_B8G8_UNORM,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_G8R8_G8B8[] =
{
    DXGI_FORMAT_G8R8_G8B8_UNORM,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_BC1[] =
{
    DXGI_FORMAT_BC1_TYPELESS,
    DXGI_FORMAT_BC1_UNORM,
    DXGI_FORMAT_BC1_UNORM_SRGB,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_BC2[] =
{
    DXGI_FORMAT_BC2_TYPELESS,
    DXGI_FORMAT_BC2_UNORM,
    DXGI_FORMAT_BC2_UNORM_SRGB,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_BC3[] =
{
    DXGI_FORMAT_BC3_TYPELESS,
    DXGI_FORMAT_BC3_UNORM,
    DXGI_FORMAT_BC3_UNORM_SRGB,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_BC4[] =
{
    DXGI_FORMAT_BC4_TYPELESS,
    DXGI_FORMAT_BC4_UNORM,
    DXGI_FORMAT_BC4_SNORM,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_BC5[] =
{
    DXGI_FORMAT_BC5_TYPELESS,
    DXGI_FORMAT_BC5_UNORM,
    DXGI_FORMAT_BC5_SNORM,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_B5G6R5[] =
{
    DXGI_FORMAT_B5G6R5_UNORM,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_B5G5R5A1[] =
{
    DXGI_FORMAT_B5G5R5A1_UNORM,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_B8G8R8A8[] =
{
    DXGI_FORMAT_B8G8R8A8_UNORM,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

DXGI_FORMAT D3D10FCS_B8G8R8X8[] =
{
    DXGI_FORMAT_B8G8R8X8_UNORM,
    DXGI_FORMAT_UNKNOWN // not part of cast set, just the "null terminator"
};

// ----------------------------------------------------------------------------
// As much information about D3D10's interpretation of DXGI Resource Formats should be encoded in this
// table, and everyone should query the information from here, be it for
// specs or for code.
// ----------------------------------------------------------------------------

const CD3D10FormatHelper_Test::FORMAT_DETAIL CD3D10FormatHelper_Test::s_FormatDetail[] =
{
//      DXGI_FORMAT                              ParentFormat                       pFormatCastSet          BitsPerComponent[4], BitsPerElement, WidthAlignment, HeightAlignment, DepthAlignment,   Layout,             TypeLevel,              ComponentName[4],ComponentInterpretation[4],                          Buffer          InputAssemblerVertexBuffer        InputAssemblerIndexBuffer StreamOutputBuffer                 Texture1D               Texture2D               Texture3D       TextureCube     ld                  sample        sample (Comparison filter)  sample (MONO 1-bit filter)      MipMap          MipMapAutoGen   RenderTarget             BlendableRenderTarget  DepthStencilTarget                  CPULockable     MultisampleRenderTarget  MultisampleLoad          MultisampleResolve  DisplayScanOut  CastWithinBitLayout  bDX9VertexOrIndexFormat bDX9TextureFormat
    {DXGI_FORMAT_UNKNOWN                     ,   DXGI_FORMAT_UNKNOWN,               D3D10FCS_UNKNOWN,       {0,0,0,0},           0,              1,              1,               1,                D3D10FL_CUSTOM,     D3D10FTL_NO_TYPE,       X,X,X,X,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_REQ,     D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_NA,              D3D10R_NA,      D3D10R_NA,      D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_NA,      D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_NA,           TRUE,                   FALSE,              },
    {DXGI_FORMAT_R32G32B32A32_TYPELESS       ,   DXGI_FORMAT_R32G32B32A32_TYPELESS, D3D10FCS_R32G32B32A32,  {32,32,32,32},       128,            1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,G,B,A,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R32G32B32A32_FLOAT      ,   DXGI_FORMAT_R32G32B32A32_TYPELESS, D3D10FCS_R32G32B32A32,  {32,32,32,32},       128,            1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _FLOAT, _FLOAT, _FLOAT, _FLOAT,                      D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_REQ,                        D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_OPT,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          TRUE,                   FALSE,              },
    {    DXGI_FORMAT_R32G32B32A32_UINT       ,   DXGI_FORMAT_R32G32B32A32_TYPELESS, D3D10FCS_R32G32B32A32,  {32,32,32,32},       128,            1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _UINT, _UINT, _UINT, _UINT,                          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_REQ,                        D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R32G32B32A32_SINT       ,   DXGI_FORMAT_R32G32B32A32_TYPELESS, D3D10FCS_R32G32B32A32,  {32,32,32,32},       128,            1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _SINT, _SINT, _SINT, _SINT,                          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_REQ,                        D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_R32G32B32_TYPELESS          ,   DXGI_FORMAT_R32G32B32_TYPELESS,    D3D10FCS_R32G32B32,     {32,32,32,0},        96,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,G,B,X,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R32G32B32_FLOAT         ,   DXGI_FORMAT_R32G32B32_TYPELESS,    D3D10FCS_R32G32B32,     {32,32,32,0},        96,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,X,         _FLOAT, _FLOAT, _FLOAT, _TYPELESS,                   D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_REQ,                        D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_OPT,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_OPT,     D3D10R_OPT,              D3D10R_OPT,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          TRUE,                   FALSE,              },
    {    DXGI_FORMAT_R32G32B32_UINT          ,   DXGI_FORMAT_R32G32B32_TYPELESS,    D3D10FCS_R32G32B32,     {32,32,32,0},        96,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,X,         _UINT, _UINT, _UINT, _TYPELESS,                      D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_REQ,                        D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_OPT,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R32G32B32_SINT          ,   DXGI_FORMAT_R32G32B32_TYPELESS,    D3D10FCS_R32G32B32,     {32,32,32,0},        96,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,X,         _SINT, _SINT, _SINT, _TYPELESS,                      D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_REQ,                        D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_OPT,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_R16G16B16A16_TYPELESS       ,   DXGI_FORMAT_R16G16B16A16_TYPELESS, D3D10FCS_R16G16B16A16,  {16,16,16,16},       64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,G,B,A,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {   DXGI_FORMAT_R16G16B16A16_FLOAT       ,   DXGI_FORMAT_R16G16B16A16_TYPELESS, D3D10FCS_R16G16B16A16,  {16,16,16,16},       64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _FLOAT, _FLOAT, _FLOAT, _FLOAT,                      D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_REQ,     D3D10R_REQ,          TRUE,                   FALSE,              },
    {    DXGI_FORMAT_R16G16B16A16_UNORM      ,   DXGI_FORMAT_R16G16B16A16_TYPELESS, D3D10FCS_R16G16B16A16,  {16,16,16,16},       64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _UNORM, _UNORM, _UNORM, _UNORM,                      D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_OPT,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          TRUE,                   TRUE,               },
    {    DXGI_FORMAT_R16G16B16A16_UINT       ,   DXGI_FORMAT_R16G16B16A16_TYPELESS, D3D10FCS_R16G16B16A16,  {16,16,16,16},       64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _UINT, _UINT, _UINT, _UINT,                          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R16G16B16A16_SNORM      ,   DXGI_FORMAT_R16G16B16A16_TYPELESS, D3D10FCS_R16G16B16A16,  {16,16,16,16},       64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _SNORM, _SNORM, _SNORM, _SNORM,                      D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          TRUE,                   FALSE,              },
    {    DXGI_FORMAT_R16G16B16A16_SINT       ,   DXGI_FORMAT_R16G16B16A16_TYPELESS, D3D10FCS_R16G16B16A16,  {16,16,16,16},       64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _SINT, _SINT, _SINT, _SINT,                          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_R32G32_TYPELESS             ,   DXGI_FORMAT_R32G32_TYPELESS,       D3D10FCS_R32G32,        {32,32,0,0},         64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,G,X,X,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R32G32_FLOAT            ,   DXGI_FORMAT_R32G32_TYPELESS,       D3D10FCS_R32G32,        {32,32,0,0},         64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,X,X,         _FLOAT, _FLOAT, _TYPELESS, _TYPELESS,                D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_REQ,                        D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_OPT,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          TRUE,                   FALSE,              },
    {    DXGI_FORMAT_R32G32_UINT             ,   DXGI_FORMAT_R32G32_TYPELESS,       D3D10FCS_R32G32,        {32,32,0,0},         64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,X,X,         _UINT, _UINT, _TYPELESS, _TYPELESS,                  D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_REQ,                        D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R32G32_SINT             ,   DXGI_FORMAT_R32G32_TYPELESS,       D3D10FCS_R32G32,        {32,32,0,0},         64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,X,X,         _SINT, _SINT, _TYPELESS, _TYPELESS,                  D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_REQ,                        D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_R32G8X24_TYPELESS           ,   DXGI_FORMAT_R32G8X24_TYPELESS,     D3D10FCS_R32G8X24,      {32,8,24,0},         64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,G,X,X,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_NA,      D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_D32_FLOAT_S8X24_UINT    ,   DXGI_FORMAT_R32G8X24_TYPELESS,     D3D10FCS_R32G8X24,      {32,8,24,0},         64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     D,S,X,X,         _FLOAT,_UINT,_TYPELESS,_TYPELESS,                    D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_NA,      D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_REQ,                         D3D10R_REQ,     D3D10R_OPT,              D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,   DXGI_FORMAT_R32G8X24_TYPELESS,     D3D10FCS_R32G8X24,      {32,8,24,0},         64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _FLOAT,_TYPELESS,_TYPELESS,_TYPELESS,                D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_NA,      D3D10R_REQ,     D3D10R_REQ,         D3D10R_OPT,   D3D10R_REQ,                 D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT ,   DXGI_FORMAT_R32G8X24_TYPELESS,     D3D10FCS_R32G8X24,      {32,8,24,0},         64,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     X,G,X,X,         _TYPELESS,_UINT,_TYPELESS,_TYPELESS,                 D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_NA,      D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_R10G10B10A2_TYPELESS        ,   DXGI_FORMAT_R10G10B10A2_TYPELESS,  D3D10FCS_R10G10B10A2,   {10,10,10,2},        32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,G,B,A,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R10G10B10A2_UNORM       ,   DXGI_FORMAT_R10G10B10A2_TYPELESS,  D3D10FCS_R10G10B10A2,   {10,10,10,2},        32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _UNORM, _UNORM, _UNORM, _UNORM,                      D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_REQ,     D3D10R_REQ,          FALSE,                  TRUE,               },
    {    DXGI_FORMAT_R10G10B10A2_UINT        ,   DXGI_FORMAT_R10G10B10A2_TYPELESS,  D3D10FCS_R10G10B10A2,   {10,10,10,2},        32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _UINT, _UINT, _UINT, _UINT,                          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_R11G11B10_FLOAT             ,   DXGI_FORMAT_R11G11B10_FLOAT,       D3D10FCS_R11G11B10,     {11,11,10,0},        32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,X,         _FLOAT, _FLOAT, _FLOAT, _TYPELESS,                   D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_NA,           FALSE,                  FALSE,              },
    {DXGI_FORMAT_R8G8B8A8_TYPELESS           ,   DXGI_FORMAT_R8G8B8A8_TYPELESS,     D3D10FCS_R8G8B8A8,      {8,8,8,8},           32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,G,B,A,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R8G8B8A8_UNORM          ,   DXGI_FORMAT_R8G8B8A8_TYPELESS,     D3D10FCS_R8G8B8A8,      {8,8,8,8},           32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _UNORM, _UNORM, _UNORM, _UNORM,                      D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_REQ,     D3D10R_REQ,          TRUE,                   TRUE,               },
    {    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB     ,   DXGI_FORMAT_R8G8B8A8_TYPELESS,     D3D10FCS_R8G8B8A8,      {8,8,8,8},           32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _UNORM_SRGB, _UNORM_SRGB, _UNORM_SRGB, _UNORM_SRGB,  D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_REQ,     D3D10R_REQ,          FALSE,                  TRUE,               },
    {    DXGI_FORMAT_R8G8B8A8_UINT           ,   DXGI_FORMAT_R8G8B8A8_TYPELESS,     D3D10FCS_R8G8B8A8,      {8,8,8,8},           32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _UINT, _UINT, _UINT, _UINT,                          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R8G8B8A8_SNORM          ,   DXGI_FORMAT_R8G8B8A8_TYPELESS,     D3D10FCS_R8G8B8A8,      {8,8,8,8},           32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _SNORM, _SNORM, _SNORM, _SNORM,                      D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R8G8B8A8_SINT           ,   DXGI_FORMAT_R8G8B8A8_TYPELESS,     D3D10FCS_R8G8B8A8,      {8,8,8,8},           32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,B,A,         _SINT, _SINT, _SINT, _SINT,                          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_R16G16_TYPELESS             ,   DXGI_FORMAT_R16G16_TYPELESS,       D3D10FCS_R16G16,        {16,16,0,0},         32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,G,X,X,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R16G16_FLOAT            ,   DXGI_FORMAT_R16G16_TYPELESS,       D3D10FCS_R16G16,        {16,16,0,0},         32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,X,X,         _FLOAT, _FLOAT, _TYPELESS, _TYPELESS,                D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          TRUE,                   TRUE,               },
    {    DXGI_FORMAT_R16G16_UNORM            ,   DXGI_FORMAT_R16G16_TYPELESS,       D3D10FCS_R16G16,        {16,16,0,0},         32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,X,X,         _UNORM, _UNORM, _TYPELESS, _TYPELESS,                D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_OPT,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          TRUE,                   TRUE,               },
    {    DXGI_FORMAT_R16G16_UINT             ,   DXGI_FORMAT_R16G16_TYPELESS,       D3D10FCS_R16G16,        {16,16,0,0},         32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,X,X,         _UINT, _UINT, _TYPELESS, _TYPELESS,                  D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R16G16_SNORM            ,   DXGI_FORMAT_R16G16_TYPELESS,       D3D10FCS_R16G16,        {16,16,0,0},         32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,X,X,         _SNORM, _SNORM, _TYPELESS, _TYPELESS,                D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          TRUE,                   TRUE,               },
    {    DXGI_FORMAT_R16G16_SINT             ,   DXGI_FORMAT_R16G16_TYPELESS,       D3D10FCS_R16G16,        {16,16,0,0},         32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,X,X,         _SINT, _SINT, _TYPELESS, _TYPELESS,                  D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_R32_TYPELESS                ,   DXGI_FORMAT_R32_TYPELESS,          D3D10FCS_R32,           {32,0,0,0},          32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,X,X,X,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_D32_FLOAT               ,   DXGI_FORMAT_R32_TYPELESS,          D3D10FCS_R32,           {32,0,0,0},          32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     D,X,X,X,         _FLOAT, _TYPELESS, _TYPELESS, _TYPELESS,             D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_NA,      D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_REQ,                         D3D10R_REQ,     D3D10R_OPT,              D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  TRUE,               },
    {    DXGI_FORMAT_R32_FLOAT               ,   DXGI_FORMAT_R32_TYPELESS,          D3D10FCS_R32,           {32,0,0,0},          32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _FLOAT, _TYPELESS, _TYPELESS, _TYPELESS,             D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_REQ,                        D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_OPT,   D3D10R_REQ,                 D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          TRUE,                   TRUE,               },
    {    DXGI_FORMAT_R32_UINT                ,   DXGI_FORMAT_R32_TYPELESS,          D3D10FCS_R32,           {32,0,0,0},          32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _UINT, _TYPELESS, _TYPELESS, _TYPELESS,              D3D10R_REQ,     D3D10R_REQ,                       D3D10R_REQ,               D3D10R_REQ,                        D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R32_SINT                ,   DXGI_FORMAT_R32_TYPELESS,          D3D10FCS_R32,           {32,0,0,0},          32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _SINT, _TYPELESS, _TYPELESS, _TYPELESS,              D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_REQ,                        D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_R24G8_TYPELESS              ,   DXGI_FORMAT_R24G8_TYPELESS,        D3D10FCS_R24G8,         {24,8,0,0},          32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,G,X,X,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_NA,      D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_D24_UNORM_S8_UINT       ,   DXGI_FORMAT_R24G8_TYPELESS,        D3D10FCS_R24G8,         {24,8,0,0},          32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     D,S,X,X,         _UNORM,_UINT,_TYPELESS,_TYPELESS,                    D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_NA,      D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_REQ,                         D3D10R_REQ,     D3D10R_OPT,              D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  TRUE,               },
    {    DXGI_FORMAT_R24_UNORM_X8_TYPELESS   ,   DXGI_FORMAT_R24G8_TYPELESS,        D3D10FCS_R24G8,         {24,8,0,0},          32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _UNORM,_TYPELESS,_TYPELESS,_TYPELESS,                D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_NA,      D3D10R_REQ,     D3D10R_REQ,         D3D10R_OPT,   D3D10R_REQ,                 D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_X24_TYPELESS_G8_UINT    ,   DXGI_FORMAT_R24G8_TYPELESS,        D3D10FCS_R24G8,         {24,8,0,0},          32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     X,G,X,X,         _TYPELESS,_UINT,_TYPELESS,_TYPELESS,                 D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_NA,      D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_R8G8_TYPELESS               ,   DXGI_FORMAT_R8G8_TYPELESS,         D3D10FCS_R8G8,          {8,8,0,0},           16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,G,X,X,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R8G8_UNORM              ,   DXGI_FORMAT_R8G8_TYPELESS,         D3D10FCS_R8G8,          {8,8,0,0},           16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,X,X,         _UNORM, _UNORM, _TYPELESS, _TYPELESS,                D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R8G8_UINT               ,   DXGI_FORMAT_R8G8_TYPELESS,         D3D10FCS_R8G8,          {8,8,0,0},           16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,X,X,         _UINT, _UINT, _TYPELESS, _TYPELESS,                  D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R8G8_SNORM              ,   DXGI_FORMAT_R8G8_TYPELESS,         D3D10FCS_R8G8,          {8,8,0,0},           16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,X,X,         _SNORM, _SNORM, _TYPELESS, _TYPELESS,                D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          FALSE,                  TRUE,               },
    {    DXGI_FORMAT_R8G8_SINT               ,   DXGI_FORMAT_R8G8_TYPELESS,         D3D10FCS_R8G8,          {8,8,0,0},           16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,G,X,X,         _SINT, _SINT, _TYPELESS, _TYPELESS,                  D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_R16_TYPELESS                ,   DXGI_FORMAT_R16_TYPELESS,          D3D10FCS_R16,           {16,0,0,0},          16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,X,X,X,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R16_FLOAT               ,   DXGI_FORMAT_R16_TYPELESS,          D3D10FCS_R16,           {16,0,0,0},          16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _FLOAT, _TYPELESS, _TYPELESS, _TYPELESS,             D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_D16_UNORM               ,   DXGI_FORMAT_R16_TYPELESS,          D3D10FCS_R16,           {16,0,0,0},          16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     D,X,X,X,         _UNORM, _TYPELESS, _TYPELESS, _TYPELESS,             D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_NA,      D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_REQ,                         D3D10R_REQ,     D3D10R_OPT,              D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  TRUE,               },
    {    DXGI_FORMAT_R16_UNORM               ,   DXGI_FORMAT_R16_TYPELESS,          D3D10FCS_R16,           {16,0,0,0},          16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _UNORM, _TYPELESS, _TYPELESS, _TYPELESS,             D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_REQ,                 D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_OPT,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R16_UINT                ,   DXGI_FORMAT_R16_TYPELESS,          D3D10FCS_R16,           {16,0,0,0},          16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _UINT, _TYPELESS, _TYPELESS, _TYPELESS,              D3D10R_REQ,     D3D10R_REQ,                       D3D10R_REQ,               D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R16_SNORM               ,   DXGI_FORMAT_R16_TYPELESS,          D3D10FCS_R16,           {16,0,0,0},          16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _SNORM, _TYPELESS, _TYPELESS, _TYPELESS,             D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R16_SINT                ,   DXGI_FORMAT_R16_TYPELESS,          D3D10FCS_R16,           {16,0,0,0},          16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _SINT, _TYPELESS, _TYPELESS, _TYPELESS,              D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_R8_TYPELESS                 ,   DXGI_FORMAT_R8_TYPELESS,           D3D10FCS_R8,            {8,0,0,0},           8,              1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_PARTIAL_TYPE,  R,X,X,X,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R8_UNORM                ,   DXGI_FORMAT_R8_TYPELESS,           D3D10FCS_R8,            {8,0,0,0},           8,              1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _UNORM, _TYPELESS, _TYPELESS, _TYPELESS,             D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R8_UINT                 ,   DXGI_FORMAT_R8_TYPELESS,           D3D10FCS_R8,            {8,0,0,0},           8,              1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _UINT, _TYPELESS, _TYPELESS, _TYPELESS,              D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R8_SNORM                ,   DXGI_FORMAT_R8_TYPELESS,           D3D10FCS_R8,            {8,0,0,0},           8,              1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _SNORM, _TYPELESS, _TYPELESS, _TYPELESS,             D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {    DXGI_FORMAT_R8_SINT                 ,   DXGI_FORMAT_R8_TYPELESS,           D3D10FCS_R8,            {8,0,0,0},           8,              1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _SINT, _TYPELESS, _TYPELESS, _TYPELESS,              D3D10R_REQ,     D3D10R_REQ,                       D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_REQ,              D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_A8_UNORM                    ,   DXGI_FORMAT_A8_UNORM,              D3D10FCS_A8,            {0,0,0,8},           8,              1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     X,X,X,A,         _TYPELESS, _TYPELESS, _TYPELESS, _UNORM,             D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_OPT,              D3D10R_OPT,              D3D10R_REQ,         D3D10R_NA,      D3D10R_NA,           FALSE,                  TRUE,               },
    {DXGI_FORMAT_R1_UNORM                    ,   DXGI_FORMAT_R1_UNORM,              D3D10FCS_R1,            {1,0,0,0},           1,              8,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     R,X,X,X,         _UNORM, _TYPELESS, _TYPELESS, _TYPELESS,             D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_NA,      D3D10R_NA,      D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_REQ,                     D3D10R_NA,      D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_NA,           FALSE,                  FALSE,              },
    {DXGI_FORMAT_R9G9B9E5_SHAREDEXP          ,   DXGI_FORMAT_R9G9B9E5_SHAREDEXP,    D3D10FCS_R9G9B9E5,      {0,0,0,0},           32,             1,              1,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,G,B,X,         _FLOAT, _FLOAT, _FLOAT, _FLOAT,                      D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_NA,           FALSE,                  FALSE,              },
    {DXGI_FORMAT_R8G8_B8G8_UNORM             ,   DXGI_FORMAT_R8G8_B8G8_UNORM,       D3D10FCS_R8G8_B8G8,     {0,0,0,0},           16,             2,              1,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,G,B,X,         _UNORM, _UNORM, _UNORM, _TYPELESS,                   D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_NA,           FALSE,                  TRUE,               },
    {DXGI_FORMAT_G8R8_G8B8_UNORM             ,   DXGI_FORMAT_G8R8_G8B8_UNORM,       D3D10FCS_G8R8_G8B8,     {0,0,0,0},           16,             2,              1,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,G,B,X,         _UNORM, _UNORM, _UNORM, _TYPELESS,                   D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_REQ,             D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_NA,           FALSE,                  TRUE,               },
    {DXGI_FORMAT_BC1_TYPELESS                ,   DXGI_FORMAT_BC1_TYPELESS,          D3D10FCS_BC1,           {0,0,0,0},           4,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_PARTIAL_TYPE,  R,G,B,A,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  TRUE,               },
    {DXGI_FORMAT_BC1_UNORM                   ,   DXGI_FORMAT_BC1_TYPELESS,          D3D10FCS_BC1,           {0,0,0,0},           4,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,G,B,A,         _UNORM, _UNORM, _UNORM, _UNORM,                      D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  TRUE,               },
    {DXGI_FORMAT_BC1_UNORM_SRGB              ,   DXGI_FORMAT_BC1_TYPELESS,          D3D10FCS_BC1,           {0,0,0,0},           4,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,G,B,A,         _UNORM_SRGB, _UNORM_SRGB, _UNORM_SRGB, _UNORM,       D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  TRUE,               },
    {DXGI_FORMAT_BC2_TYPELESS                ,   DXGI_FORMAT_BC2_TYPELESS,          D3D10FCS_BC2,           {0,0,0,0},           8,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_PARTIAL_TYPE,  R,G,B,A,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  TRUE,               },
    {DXGI_FORMAT_BC2_UNORM                   ,   DXGI_FORMAT_BC2_TYPELESS,          D3D10FCS_BC2,           {0,0,0,0},           8,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,G,B,A,         _UNORM, _UNORM, _UNORM, _UNORM,                      D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  TRUE,               },
    {DXGI_FORMAT_BC2_UNORM_SRGB              ,   DXGI_FORMAT_BC2_TYPELESS,          D3D10FCS_BC2,           {0,0,0,0},           8,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,G,B,A,         _UNORM_SRGB, _UNORM_SRGB, _UNORM_SRGB, _UNORM,       D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  TRUE,               },
    {DXGI_FORMAT_BC3_TYPELESS                ,   DXGI_FORMAT_BC3_TYPELESS,          D3D10FCS_BC3,           {0,0,0,0},           8,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_PARTIAL_TYPE,  R,G,B,A,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_BC3_UNORM                   ,   DXGI_FORMAT_BC3_TYPELESS,          D3D10FCS_BC3,           {0,0,0,0},           8,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,G,B,A,         _UNORM, _UNORM, _UNORM, _UNORM,                      D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  TRUE,               },
    {DXGI_FORMAT_BC3_UNORM_SRGB              ,   DXGI_FORMAT_BC3_TYPELESS,          D3D10FCS_BC3,           {0,0,0,0},           8,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,G,B,A,         _UNORM_SRGB, _UNORM_SRGB, _UNORM_SRGB, _UNORM,       D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  TRUE,               },
    {DXGI_FORMAT_BC4_TYPELESS                ,   DXGI_FORMAT_BC4_TYPELESS,          D3D10FCS_BC4,           {0,0,0,0},           4,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_PARTIAL_TYPE,  R,X,X,X,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_BC4_UNORM                   ,   DXGI_FORMAT_BC4_TYPELESS,          D3D10FCS_BC4,           {0,0,0,0},           4,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,X,X,X,         _UNORM, _TYPELESS, _TYPELESS, _TYPELESS,             D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_BC4_SNORM                   ,   DXGI_FORMAT_BC4_TYPELESS,          D3D10FCS_BC4,           {0,0,0,0},           4,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,X,X,X,         _SNORM, _TYPELESS, _TYPELESS, _TYPELESS,             D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_BC5_TYPELESS                ,   DXGI_FORMAT_BC5_TYPELESS,          D3D10FCS_BC5,           {0,0,0,0},           8,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_PARTIAL_TYPE,  R,G,X,X,         _TYPELESS, _TYPELESS, _TYPELESS, _TYPELESS,          D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_NA,          D3D10R_NA,    D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_BC5_UNORM                   ,   DXGI_FORMAT_BC5_TYPELESS,          D3D10FCS_BC5,           {0,0,0,0},           8,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,G,X,X,         _UNORM, _UNORM, _TYPELESS, _TYPELESS,                D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_BC5_SNORM                   ,   DXGI_FORMAT_BC5_TYPELESS,          D3D10FCS_BC5,           {0,0,0,0},           8,              4,              4,               1,                D3D10FL_CUSTOM,     D3D10FTL_FULL_TYPE,     R,G,X,X,         _SNORM, _SNORM, _TYPELESS, _TYPELESS,                D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_REQ,     D3D10R_REQ,     D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_REQ,     D3D10R_NA,      D3D10R_NA,               D3D10R_NA,             D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_REQ,          FALSE,                  FALSE,              },
    {DXGI_FORMAT_B5G6R5_UNORM                ,   DXGI_FORMAT_B5G6R5_UNORM,          D3D10FCS_B5G6R5,        {5,6,5,0},           16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     B,G,R,X,         _UNORM, _UNORM, _UNORM, _TYPELESS,                   D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_NA,      D3D10R_NA,      D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_NA,      D3D10R_NA,      D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_NA,           FALSE,                  TRUE,               },
    {DXGI_FORMAT_B5G5R5A1_UNORM              ,   DXGI_FORMAT_B5G5R5A1_UNORM,        D3D10FCS_B5G5R5A1,      {5,5,5,1},           16,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     B,G,R,A,         _UNORM, _UNORM, _UNORM, _UNORM,                      D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_NA,      D3D10R_NA,      D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_NA,      D3D10R_NA,      D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_NA,           FALSE,                  TRUE,               },
    {DXGI_FORMAT_B8G8R8A8_UNORM              ,   DXGI_FORMAT_B8G8R8A8_UNORM,        D3D10FCS_B8G8R8A8,      {8,8,8,8},           32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     B,G,R,A,         _UNORM, _UNORM, _UNORM, _UNORM,                      D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_NA,      D3D10R_NA,      D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_NA,      D3D10R_NA,      D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_NA,           FALSE,                  TRUE,               },
    {DXGI_FORMAT_B8G8R8X8_UNORM              ,   DXGI_FORMAT_B8G8R8X8_UNORM,        D3D10FCS_B8G8R8X8,      {8,8,8,8},           32,             1,              1,               1,                D3D10FL_STANDARD,   D3D10FTL_FULL_TYPE,     B,G,R,X,         _UNORM, _UNORM, _UNORM, _TYPELESS,                   D3D10R_NA,      D3D10R_NA,                        D3D10R_NA,                D3D10R_NA,                         D3D10R_NA,              D3D10R_REQ,             D3D10R_NA,      D3D10R_NA,      D3D10R_REQ,         D3D10R_REQ,   D3D10R_NA,                  D3D10R_NA,                      D3D10R_NA,      D3D10R_NA,      D3D10R_REQ,              D3D10R_REQ,            D3D10R_NA,                          D3D10R_REQ,     D3D10R_NA,               D3D10R_NA,               D3D10R_NA,          D3D10R_NA,      D3D10R_NA,           FALSE,                  TRUE,               },
};

const UINT CD3D10FormatHelper_Test::s_NumFormats = (sizeof(CD3D10FormatHelper_Test::s_FormatDetail)/sizeof(CD3D10FormatHelper_Test::FORMAT_DETAIL));

LPCSTR CD3D10FormatHelper_Test::s_FormatNames[] =  // separate from above structure so it can be compiled out of the runtime.
{
//   Name
    {"UNKNOWN",                      },
    {"R32G32B32A32_TYPELESS",        },
    {    "R32G32B32A32_FLOAT",       },
    {    "R32G32B32A32_UINT",        },
    {    "R32G32B32A32_SINT",        },
    {"R32G32B32_TYPELESS",           },
    {    "R32G32B32_FLOAT",          },
    {    "R32G32B32_UINT",           },
    {    "R32G32B32_SINT",           },
    {"R16G16B16A16_TYPELESS",        },
    {    "R16G16B16A16_FLOAT",       },
    {    "R16G16B16A16_UNORM",       },
    {    "R16G16B16A16_UINT",        },
    {    "R16G16B16A16_SNORM",       },
    {    "R16G16B16A16_SINT",        },
    {"R32G32_TYPELESS",              },
    {    "R32G32_FLOAT",             },
    {    "R32G32_UINT",              },
    {    "R32G32_SINT",              },
    {"R32G8X24_TYPELESS",            },
    {    "D32_FLOAT_S8X24_UINT",     },
    {    "R32_FLOAT_X8X24_TYPELESS", },
    {    "X32_TYPELESS_G8X24_UINT",  },
    {"R10G10B10A2_TYPELESS",         },
    {    "R10G10B10A2_UNORM",        },
    {    "R10G10B10A2_UINT",         },
    {"R11G11B10_FLOAT",              },
    {"R8G8B8A8_TYPELESS",            },
    {    "R8G8B8A8_UNORM",           },
    {    "R8G8B8A8_UNORM_SRGB",      },
    {    "R8G8B8A8_UINT",            },
    {    "R8G8B8A8_SNORM",           },
    {    "R8G8B8A8_SINT",            },
    {"R16G16_TYPELESS",              },
    {    "R16G16_FLOAT",             },
    {    "R16G16_UNORM",             },
    {    "R16G16_UINT",              },
    {    "R16G16_SNORM",             },
    {    "R16G16_SINT",              },
    {"R32_TYPELESS",                 },
    {    "D32_FLOAT",                },
    {    "R32_FLOAT",                },
    {    "R32_UINT",                 },
    {    "R32_SINT",                 },
    {"R24G8_TYPELESS",               },
    {    "D24_UNORM_S8_UINT",        },
    {    "R24_UNORM_X8_TYPELESS",    },
    {    "X24_TYPELESS_G8_UINT",     },
    {"R8G8_TYPELESS",                },
    {    "R8G8_UNORM",               },
    {    "R8G8_UINT",                },
    {    "R8G8_SNORM",               },
    {    "R8G8_SINT",                },
    {"R16_TYPELESS",                 },
    {    "R16_FLOAT",                },
    {    "D16_UNORM",                },
    {    "R16_UNORM",                },
    {    "R16_UINT",                 },
    {    "R16_SNORM",                },
    {    "R16_SINT",                 },
    {"R8_TYPELESS",                  },
    {    "R8_UNORM",                 },
    {    "R8_UINT",                  },
    {    "R8_SNORM",                 },
    {    "R8_SINT",                  },
    {"A8_UNORM",                     },
    {"R1_UNORM",                     },
    {"R9G9B9E5_SHAREDEXP",           },
    {"R8G8_B8G8_UNORM",              },
    {"G8R8_G8B8_UNORM",              },
    {"BC1_TYPELESS",                 },
    {    "BC1_UNORM",                },
    {    "BC1_UNORM_SRGB",           },
    {"BC2_TYPELESS",                 },
    {    "BC2_UNORM",                },
    {    "BC2_UNORM_SRGB",           },
    {"BC3_TYPELESS",                 },
    {    "BC3_UNORM",                },
    {    "BC3_UNORM_SRGB",           },
    {"BC4_TYPELESS",                 },
    {    "BC4_UNORM",                },
    {    "BC4_SNORM",                },
    {"BC5_TYPELESS",                 },
    {    "BC5_UNORM",                },
    {    "BC5_SNORM",                },
    {"DXGI_FORMAT_B5G6R5_UNORM",     },
    {"DXGI_FORMAT_B5G5R5A1_UNORM",   },
    {"DXGI_FORMAT_B8G8R8A8_UNORM",   },
    {"DXGI_FORMAT_B8G8R8X8_UNORM",   },
};


//---------------------------------------------------------------------------------------------------------------------------------
// GetTableSize
SIZE_T CD3D10FormatHelper_Test::GetTableSize()
{
    return s_NumFormats;
}

//---------------------------------------------------------------------------------------------------------------------------------
// GetFormat
DXGI_FORMAT CD3D10FormatHelper_Test::GetFormat(SIZE_T Index)
{
    if( Index < s_NumFormats )
    {
        return s_FormatDetail[Index].DXGIFormat;
    }

    return (DXGI_FORMAT)-1;
}

//---------------------------------------------------------------------------------------------------------------------------------
// GetTableIndex
UINT CD3D10FormatHelper_Test::GetTableIndex(DXGI_FORMAT  Format)
{
    if( (UINT)Format < s_NumFormats )
    {
        if( s_FormatDetail[(UINT)Format].DXGIFormat == Format )
        {
            return Format;
        }
    }

    return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------
// FormatExists
bool CD3D10FormatHelper_Test::FormatExists(DXGI_FORMAT Format)
{
    return (GetTableIndex(Format) != -1);
}

//---------------------------------------------------------------------------------------------------------------------------------
// APIFormatExists
bool CD3D10FormatHelper_Test::APIFormatExists(DXGI_FORMAT Format)
{
    switch (Format)
    {
    case (DXGI_FORMAT_B5G6R5_UNORM):
    case (DXGI_FORMAT_B5G5R5A1_UNORM):
    case (DXGI_FORMAT_B8G8R8A8_UNORM):
    case (DXGI_FORMAT_B8G8R8X8_UNORM):
        // app-invisible:
        return false;

    default:
        return CD3D10FormatHelper_Test::FormatExists(Format);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------
// GetTableIndexThrow
UINT CD3D10FormatHelper_Test::GetTableIndexThrow(DXGI_FORMAT  Format)
{
    UINT Index = GetTableIndex( Format );
    if( -1 == Index )
    {
        throw E_FAIL;
    }
    return Index;
}

//---------------------------------------------------------------------------------------------------------------------------------
// GetTableIndexNoThrow
UINT CD3D10FormatHelper_Test::GetTableIndexNoThrow(DXGI_FORMAT  Format)
{
    UINT Index = GetTableIndex( Format );
    assert( -1 != Index ); // Needs to be validated externally.
    return Index;
}

//---------------------------------------------------------------------------------------------------------------------------------
// GetBitsPerStencil
UINT CD3D10FormatHelper_Test::GetBitsPerStencil(DXGI_FORMAT  Format)
{
    const UINT Index = GetTableIndexThrow( Format );
    if( (s_FormatDetail[Index].TypeLevel != D3D10FTL_PARTIAL_TYPE) &&
        (s_FormatDetail[Index].TypeLevel != D3D10FTL_FULL_TYPE) )
    {
        return 0;
    }
    for( UINT comp = 0; comp < 4; comp++ )
    {
        D3D10_FORMAT_COMPONENT_NAME name;
        switch(comp)
        {
        case 0: name = s_FormatDetail[Index].ComponentName0; break;
        case 1: name = s_FormatDetail[Index].ComponentName1; break;
        case 2: name = s_FormatDetail[Index].ComponentName2; break;
        case 3: name = s_FormatDetail[Index].ComponentName3; break;
        }
        if( name == D3D10FCN_S )
        {
            return s_FormatDetail[Index].BitsPerComponent[comp];
        }
    }
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------
// GetFormatReturnTypes
void    CD3D10FormatHelper_Test::GetFormatReturnTypes(
        DXGI_FORMAT                            Format,
        D3D10_FORMAT_COMPONENT_INTERPRETATION* pInterpretations  ) // return array with 4 entries
{
    const UINT Index = GetTableIndexThrow(Format);
//    pInterpretations[D3D10_SB_4_COMPONENT_R] = s_FormatDetail[Index].ComponentInterpretation0;
//    pInterpretations[D3D10_SB_4_COMPONENT_G] = s_FormatDetail[Index].ComponentInterpretation1;
//    pInterpretations[D3D10_SB_4_COMPONENT_B] = s_FormatDetail[Index].ComponentInterpretation2;
//    pInterpretations[D3D10_SB_4_COMPONENT_A] = s_FormatDetail[Index].ComponentInterpretation3;
}

//---------------------------------------------------------------------------------------------------------------------------------
// GetNumComponentsInFormat
UINT CD3D10FormatHelper_Test::GetNumComponentsInFormat( DXGI_FORMAT  Format )
{
    UINT n = 0;
    const UINT Index = GetTableIndexThrow(Format);
    for( UINT comp = 0; comp < 4; comp++ )
    {
        D3D10_FORMAT_COMPONENT_NAME name;
        switch(comp)
        {
        case 0: name = s_FormatDetail[Index].ComponentName0; break;
        case 1: name = s_FormatDetail[Index].ComponentName1; break;
        case 2: name = s_FormatDetail[Index].ComponentName2; break;
        case 3: name = s_FormatDetail[Index].ComponentName3; break;
        }
        if( name != D3D10FCN_X )
        {
            n++;
        }
    }
    return n;
}

//---------------------------------------------------------------------------------------------------------------------------------
// GetNumComponentsInFormat
UINT CD3D10FormatHelper_Test::Sequential2AbsoluteComponentIndex( DXGI_FORMAT  Format, UINT SequentialComponentIndex)
{
    UINT n = 0;
    const UINT Index = GetTableIndexThrow(Format);
    for( UINT comp = 0; comp < 4; comp++ )
    {
        D3D10_FORMAT_COMPONENT_NAME name;
        switch(comp)
        {
        case 0: name = s_FormatDetail[Index].ComponentName0; break;
        case 1: name = s_FormatDetail[Index].ComponentName1; break;
        case 2: name = s_FormatDetail[Index].ComponentName2; break;
        case 3: name = s_FormatDetail[Index].ComponentName3; break;
        }
        if( name != D3D10FCN_X )
        {
            if (SequentialComponentIndex == n)
            {
                return comp;
            }
            n++;
        }
    }
    return UINT(-1);
}

//---------------------------------------------------------------------------------------------------------------------------------
UINT CD3D10FormatHelper_Test::GetWidthAlignment(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].WidthAlignment;
}

UINT CD3D10FormatHelper_Test::GetHeightAlignment(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].HeightAlignment;
}

UINT CD3D10FormatHelper_Test::GetDepthAlignment(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].DepthAlignment;
}

//---------------------------------------------------------------------------------------------------------------------------------
// GetFormatDetail
const CD3D10FormatHelper_Test::FORMAT_DETAIL* CD3D10FormatHelper_Test::GetFormatDetail( DXGI_FORMAT  Format )
{
    UINT n = 0;
    const UINT Index = GetTableIndex(Format);
    if( -1 == Index )
    {
        return NULL;
    }
    return &s_FormatDetail[ Index ];
}

//---------------------------------------------------------------------------------------------------------------------------------
// CheckFormatCastability
bool CD3D10FormatHelper_Test::CheckFormatCastability( DXGI_FORMAT a, DXGI_FORMAT b ) // check for casting from a to b or vice versa
{
    const FORMAT_DETAIL* aDetail = GetFormatDetail(a);
    const FORMAT_DETAIL* bDetail = GetFormatDetail(b);
    if( !aDetail || !bDetail )
    {
        return false;
    }
    return (aDetail->ParentFormat == bDetail->ParentFormat);
}

//---------------------------------------------------------------------------------------------------------------------------------
// GetName
LPCSTR CD3D10FormatHelper_Test::GetName(DXGI_FORMAT Format)
{
    return s_FormatNames[GetTableIndexNoThrow( Format )];
}
//---------------------------------------------------------------------------------------------------------------------------------
// GetParentFormat
DXGI_FORMAT CD3D10FormatHelper_Test::GetParentFormat(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].ParentFormat;
}
//---------------------------------------------------------------------------------------------------------------------------------
// GetFormatCastSet
DXGI_FORMAT* CD3D10FormatHelper_Test::GetFormatCastSet(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].pFormatCastSet;
}
//---------------------------------------------------------------------------------------------------------------------------------
// GetLayout
D3D10_FORMAT_LAYOUT CD3D10FormatHelper_Test::GetLayout(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].Layout;
}
//---------------------------------------------------------------------------------------------------------------------------------
// GetTypeLevel
D3D10_FORMAT_TYPE_LEVEL CD3D10FormatHelper_Test::GetTypeLevel(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].TypeLevel;
}
//---------------------------------------------------------------------------------------------------------------------------------
// GetBitsPerElementThrow
UINT CD3D10FormatHelper_Test::GetBitsPerElementThrow(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexThrow( Format )].BitsPerElement;
}
//---------------------------------------------------------------------------------------------------------------------------------
// GetBitsPerElement
UINT CD3D10FormatHelper_Test::GetBitsPerElement(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].BitsPerElement;
}
//---------------------------------------------------------------------------------------------------------------------------------
// GetComponentName
D3D10_FORMAT_COMPONENT_NAME CD3D10FormatHelper_Test::GetComponentName(DXGI_FORMAT Format, UINT AbsoluteComponentIndex)
{
    D3D10_FORMAT_COMPONENT_NAME name;
    switch( AbsoluteComponentIndex )
    {
    case 0: name = s_FormatDetail[GetTableIndexThrow( Format )].ComponentName0; break;
    case 1: name = s_FormatDetail[GetTableIndexThrow( Format )].ComponentName1; break;
    case 2: name = s_FormatDetail[GetTableIndexThrow( Format )].ComponentName2; break;
    case 3: name = s_FormatDetail[GetTableIndexThrow( Format )].ComponentName3; break;
    default: throw E_FAIL;
    }
    return name;
}
//---------------------------------------------------------------------------------------------------------------------------------
// GetBitsPerComponent
UINT CD3D10FormatHelper_Test::GetBitsPerComponent(DXGI_FORMAT Format, UINT AbsoluteComponentIndex)
{
    if( AbsoluteComponentIndex > 3 )
    {
        throw E_FAIL;
    }
    return s_FormatDetail[GetTableIndexThrow( Format )].BitsPerComponent[AbsoluteComponentIndex];
}
//---------------------------------------------------------------------------------------------------------------------------------
// GetFormatComponentInterpretation
D3D10_FORMAT_COMPONENT_INTERPRETATION CD3D10FormatHelper_Test::GetFormatComponentInterpretation(DXGI_FORMAT Format, UINT AbsoluteComponentIndex)
{
    D3D10_FORMAT_COMPONENT_INTERPRETATION interp;
    switch( AbsoluteComponentIndex )
    {
    case 0: interp = s_FormatDetail[GetTableIndexThrow( Format )].ComponentInterpretation0; break;
    case 1: interp = s_FormatDetail[GetTableIndexThrow( Format )].ComponentInterpretation1; break;
    case 2: interp = s_FormatDetail[GetTableIndexThrow( Format )].ComponentInterpretation2; break;
    case 3: interp = s_FormatDetail[GetTableIndexThrow( Format )].ComponentInterpretation3; break;
    default: throw E_FAIL;
    }
    return interp;
}
//---------------------------------------------------------------------------------------------------------------------------------
// BufferSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::BufferSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].Buffer;
}
//---------------------------------------------------------------------------------------------------------------------------------
// IAVertexBufferSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::IAVertexBufferSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].InputAssemblerVertexBuffer;
}
//---------------------------------------------------------------------------------------------------------------------------------
// IAIndexBufferSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::IAIndexBufferSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].InputAssemblerIndexBuffer;
}
//---------------------------------------------------------------------------------------------------------------------------------
// SOBufferSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::SOBufferSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].StreamOutputBuffer;
}
//---------------------------------------------------------------------------------------------------------------------------------
// Texture1DSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::Texture1DSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].Texture1D;
}
//---------------------------------------------------------------------------------------------------------------------------------
// Texture2DSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::Texture2DSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].Texture2D;
}
//---------------------------------------------------------------------------------------------------------------------------------
// Texture3DSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::Texture3DSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].Texture3D;
}
//---------------------------------------------------------------------------------------------------------------------------------
// TextureCubeSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::TextureCubeSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].TextureCube;
}
//---------------------------------------------------------------------------------------------------------------------------------
// ldSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::ldSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].ld;
}
//---------------------------------------------------------------------------------------------------------------------------------
// sampleSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::sampleSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].sample;
}
//---------------------------------------------------------------------------------------------------------------------------------
// sample_cSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::sample_cSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].sample_c;
}
//---------------------------------------------------------------------------------------------------------------------------------
// sample_monoSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::sample_monoSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].sample_mono;
}
//---------------------------------------------------------------------------------------------------------------------------------
// MipMapSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::MipMapSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].MipMap;
}
//---------------------------------------------------------------------------------------------------------------------------------
// MipMapAutogenSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::MipMapAutogenSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].MipMapAutoGen;
}
//---------------------------------------------------------------------------------------------------------------------------------
// RenderTargetSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::RenderTargetSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].RenderTarget;
}
//---------------------------------------------------------------------------------------------------------------------------------
// BlendableRenderTargetSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::BlendableRenderTargetSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].BlendableRenderTarget;
}
//---------------------------------------------------------------------------------------------------------------------------------
// DepthStencilTargetSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::DepthStencilTargetSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].DepthStencilTarget;
}
//---------------------------------------------------------------------------------------------------------------------------------
// CPULockable
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::CPULockable(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].CPULockable;
}
//---------------------------------------------------------------------------------------------------------------------------------
// MultisampleRenderTargetSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::MultisampleRenderTargetSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].MultisampleRenderTarget;
}
//---------------------------------------------------------------------------------------------------------------------------------
// MultisampleLoadSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::MultisampleLoadSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].MultisampleLoad;
}
//---------------------------------------------------------------------------------------------------------------------------------
// MultisampleResolveSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::MultisampleResolveSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].MultisampleResolve;
}
//---------------------------------------------------------------------------------------------------------------------------------
// DisplayScanOutSupport
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::DisplayScanOutSupport(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].DisplayScanOut;
}
//---------------------------------------------------------------------------------------------------------------------------------
// CastWithinBitLayout
D3D10_REQUIREMENTS CD3D10FormatHelper_Test::CastWithinBitLayout(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexNoThrow( Format )].CastWithinBitLayout;
}
//---------------------------------------------------------------------------------------------------------------------------------
// DX9VertexOrIndexFormat
BOOL CD3D10FormatHelper_Test::DX9VertexOrIndexFormat(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexThrow( Format )].bDX9VertexOrIndexFormat;
}
//---------------------------------------------------------------------------------------------------------------------------------
// DX9TextureFormat
BOOL CD3D10FormatHelper_Test::DX9TextureFormat(DXGI_FORMAT Format)
{
    return s_FormatDetail[GetTableIndexThrow( Format )].bDX9TextureFormat;
}

// End of file
