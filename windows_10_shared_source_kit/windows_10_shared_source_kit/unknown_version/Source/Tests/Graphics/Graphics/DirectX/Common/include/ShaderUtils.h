//=======================================================================================
//
//  ShaderUtils.h
//
//  Shader Utilities
//
// --------------------------------------------------------------------------------------
//
//  Copyright (c) 1999 Microsoft Corporation
//  All Rights Reserved
//
//=======================================================================================

#ifndef _SHADERUTILS_H_
#define _SHADERUTILS_H_

#include <windows.h>

//
//	Shader Op Counting Utility
//

struct SHADER_OPCOUNT_INFO
{
	DWORD dwCount_DEF;
	DWORD dwCount_DCL;
	DWORD dwCount_Tex;
	DWORD dwCount_Arith;
	DWORD dwCount_FlowCtrl_S;
	DWORD dwCount_FlowCtrl_D;
	DWORD dwCount_Macro;
	DWORD dwCount_NOP;
	DWORD dwCount_Total;
};

#define FLOWCTRL_STATIC    0
#define FLOWCTRL_DYNAMIC   1

HRESULT GetShaderOpCount( DWORD* pdwShader, DWORD dwDWORDCount, SHADER_OPCOUNT_INFO* pOpCountInfo, bool bDebugSpewEnabled = false );


#endif //_SHADERUTILS_H_

//=======================================================================================
// EOFF
