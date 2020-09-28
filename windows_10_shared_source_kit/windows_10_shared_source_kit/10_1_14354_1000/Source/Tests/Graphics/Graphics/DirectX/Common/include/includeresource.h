//=======================================================================================
//
//  IncludeResource.h
//
// --------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation
//  All Rights Reserved
//
//=======================================================================================

#pragma once


///////////////////////////////////////////////////////////////////////////////
// CIncludeResource
// 
// Provides shader compiler with data from resource files in the executable image to handle #include directives
// 
///////////////////////////////////////////////////////////////////////////////
class CIncludeResource : public ID3D10Include
{
public:
	STDMETHOD(Open)(D3D10_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
	STDMETHOD(Close)(LPCVOID pData);
};