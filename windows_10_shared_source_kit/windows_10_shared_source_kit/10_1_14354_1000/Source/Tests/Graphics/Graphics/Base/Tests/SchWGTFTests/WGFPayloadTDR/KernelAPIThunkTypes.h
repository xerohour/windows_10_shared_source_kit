/******************************Module*Header*******************************
* Module Name: WGFPayloadTDR.h
*
* D3D11 Graphics and Compute TDR workload test
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#pragma once

// These are for command line options. They are used like:
//
//  -graphics:dma_buffer
// 
enum GRAPHICS_TYPE
{
    GRAPHICS_PREEMPTION_NONE                = D3DKMDT_GRAPHICS_PREEMPTION_NONE,
    GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY = D3DKMDT_GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY,
    GRAPHICS_PREEMPTION_PRIMITIVE_BOUNDARY  = D3DKMDT_GRAPHICS_PREEMPTION_PRIMITIVE_BOUNDARY,
    GRAPHICS_PREEMPTION_TRIANGLE_BOUNDARY   = D3DKMDT_GRAPHICS_PREEMPTION_TRIANGLE_BOUNDARY,
    GRAPHICS_PREEMPTION_PIXEL_BOUNDARY      = D3DKMDT_GRAPHICS_PREEMPTION_PIXEL_BOUNDARY,
    GRAPHICS_PREEMPTION_SHADER_BOUNDARY     = D3DKMDT_GRAPHICS_PREEMPTION_SHADER_BOUNDARY,
};

// These are for command line options. They are used like:
//
//  -compute:group
// 
enum COMPUTE_TYPE
{
    COMPUTE_PREEMPTION_NONE                  = D3DKMDT_COMPUTE_PREEMPTION_NONE,
    COMPUTE_PREEMPTION_DMA_BUFFER_BOUNDARY   = D3DKMDT_COMPUTE_PREEMPTION_DMA_BUFFER_BOUNDARY,
    COMPUTE_PREEMPTION_DISPATCH_BOUNDARY     = D3DKMDT_COMPUTE_PREEMPTION_DISPATCH_BOUNDARY,
    COMPUTE_PREEMPTION_THREAD_GROUP_BOUNDARY = D3DKMDT_COMPUTE_PREEMPTION_THREAD_GROUP_BOUNDARY,
    COMPUTE_PREEMPTION_THREAD_BOUNDARY       = D3DKMDT_COMPUTE_PREEMPTION_THREAD_BOUNDARY,
    COMPUTE_PREEMPTION_SHADER_BOUNDARY       = D3DKMDT_COMPUTE_PREEMPTION_SHADER_BOUNDARY,
};

#ifdef DEFINE_TYPES

DECLARE_NAMED_VALUES( GRAPHICS_TYPE );

BEGIN_NAMED_VALUES( GRAPHICS_TYPE )
    NAMED_VALUE( _T( "none" ),       GRAPHICS_PREEMPTION_NONE )
    NAMED_VALUE( _T( "dma_buffer" ), GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY )
    NAMED_VALUE( _T( "primitive" ),  GRAPHICS_PREEMPTION_PRIMITIVE_BOUNDARY )
    NAMED_VALUE( _T( "triangle" ),   GRAPHICS_PREEMPTION_TRIANGLE_BOUNDARY )
    NAMED_VALUE( _T( "pixel" ),      GRAPHICS_PREEMPTION_PIXEL_BOUNDARY )
    NAMED_VALUE( _T( "shader" ),     GRAPHICS_PREEMPTION_SHADER_BOUNDARY )

END_NAMED_VALUES( GRAPHICS_TYPE )


DECLARE_NAMED_VALUES( COMPUTE_TYPE );

BEGIN_NAMED_VALUES( COMPUTE_TYPE )
    NAMED_VALUE( _T( "none" ),         COMPUTE_PREEMPTION_NONE )
    NAMED_VALUE( _T( "dma_buffer" ),   COMPUTE_PREEMPTION_DMA_BUFFER_BOUNDARY )
    NAMED_VALUE( _T( "dispatch" ),     COMPUTE_PREEMPTION_DISPATCH_BOUNDARY )
    NAMED_VALUE( _T( "thread_group" ), COMPUTE_PREEMPTION_THREAD_GROUP_BOUNDARY )
    NAMED_VALUE( _T( "thread" ),       COMPUTE_PREEMPTION_THREAD_BOUNDARY )
    NAMED_VALUE( _T( "shader" ),       COMPUTE_PREEMPTION_SHADER_BOUNDARY )

END_NAMED_VALUES( COMPUTE_TYPE )

#endif

// For faults, we have a collection that we can use
/*
const UINT ProbeAndLock = (1 << 0);
const UINT SplitPoint = (1 << 1);
const UINT SwizzlingAperture = (1 << 3);
const UINT PagingPathLockSubRange = (1 << 4);
const UINT PagingPathLockMinRange = (1 << 5);
const UINT ComplexLock = (1 << 7);
const UINT FailVARotation = (1 << 8);
const UINT NoWriteCombined = (1 << 9);
const UINT NoPrePatching = (1 << 10);
const UINT AlwaysRepatch = (1 << 11);
const UINT DisablePreparationBreak = (1 << 12);
const UINT FailUMVAMapping = (1 << 13);
const UINT NeverDiscardOffered = (1 << 14);
const UINT ForceDiscardOffered = (1 << 15);
*/

enum DXGKERNEL_FAULT_TYPE
{
    ProbeAndLock = (1 << 0),
    SplitPoint = (1 << 1),
    SwizzlingAperture = (1 << 3),
    PagingPathLockSubRange = (1 << 4),
    PagingPathLockMinRange = (1 << 5),
    ComplexLock = (1 << 7),
    FailVARotation = (1 << 8),
    NoWriteCombined = (1 << 9),
    NoPrePatching = (1 << 10),
    AlwaysRepatch = (1 << 11),
    DisablePreparationBreak = (1 << 12),
    FailUMVAMapping = (1 << 13),
    NeverDiscardOffered = (1 << 14),
    ForceDiscardOffered = (1 << 15),
};

#ifdef DEFINE_TYPES

DECLARE_NAMED_VALUES( DXGKERNEL_FAULT_TYPE );

BEGIN_NAMED_VALUES( DXGKERNEL_FAULT_TYPE )
    NAMED_VALUE( _T( "ProbeAndLock" ),            ProbeAndLock )
    NAMED_VALUE( _T( "SplitPoint" ),              SplitPoint )
    NAMED_VALUE( _T( "SwizzlingAperture" ),       SwizzlingAperture )
    NAMED_VALUE( _T( "PagingPathLockSubRange" ),  PagingPathLockSubRange )
    NAMED_VALUE( _T( "PagingPathLockMinRange" ),  PagingPathLockMinRange )
    NAMED_VALUE( _T( "ComplexLock" ),             ComplexLock )
    NAMED_VALUE( _T( "FailVARotation" ),          FailVARotation )
    NAMED_VALUE( _T( "NoWriteCombined" ),         NoWriteCombined )
    NAMED_VALUE( _T( "NoPrePatching" ),           NoPrePatching )
    NAMED_VALUE( _T( "AlwaysRepatch" ),           AlwaysRepatch )
    NAMED_VALUE( _T( "DisablePreparationBreak" ), DisablePreparationBreak )
    NAMED_VALUE( _T( "FailUMVAMapping" ),         FailUMVAMapping )
    NAMED_VALUE( _T( "NeverDiscardOffered" ),     NeverDiscardOffered )
    NAMED_VALUE( _T( "ForceDiscardOffered" ),     ForceDiscardOffered )

END_NAMED_VALUES( DXGKERNEL_FAULT_TYPE )

#endif