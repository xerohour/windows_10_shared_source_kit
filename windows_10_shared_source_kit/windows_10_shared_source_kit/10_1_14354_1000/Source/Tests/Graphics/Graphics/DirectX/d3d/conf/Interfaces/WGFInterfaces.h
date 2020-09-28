//////////////////////////////////////////////////////////////////////////
//  WGFInterfaces.h
//  created:	2009/04/17
//
//  Description: D3D11 Interfaces provides a way for the developer to 
//		change the behavior of a shader at runtime.  Different variations 
//		of a function can be written in separate classes and then
//		embedded in the shader.  At runtime, the application can pick 
//		which implementation the shader uses and reduce the overall 
//		need for managing huge numbers of shaders in a library.
//////////////////////////////////////////////////////////////////////////

#pragma once

// Includes
#include "D3D11Test.h"

#define ZeroStructMem(x) ZeroMemory(&(x), sizeof(x))

// This is used rather than D3D11_SHADER_MAX_INTERFACES because
// of windows 7 bug 679965 (which prevents all 256 interface slots from being used)
#define MAX_INTERFACE_SLOTS 253

//------------------------------------------------------------------------------------------------
// CInterfacesTest

class CInterfacesTest : public CD3D11Test
{
protected:
	ID3D11ClassLinkage * m_pClassLinkage;

	CInterfacesTest() 
		: m_numClassInstances(1)
		, m_pClassLinkage(NULL)
	{ 
	}

	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	virtual void CleanupTestCase();
	virtual void Cleanup();

protected:
	UINT m_numClassInstances;

public:
	struct CClassInstanceData
	{
		LPCSTR m_ClassTypeName;
		UINT m_ConstantBufferOffset;
		UINT m_ConstantVectorOffset;
		UINT m_TextureOffset;
		UINT m_SamplerOffset;
	};
};


//------------------------------------------------------------------------------------------------
// CInterfacesTestApp

class CInterfacesTestApp : public CD3D11TestFramework
{
public:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	virtual bool InitTestGroups();
	virtual void InitOptionVariables();

	bool m_bLogShaderText;
};

// Global test app instance
extern CInterfacesTestApp  g_TestApp;
