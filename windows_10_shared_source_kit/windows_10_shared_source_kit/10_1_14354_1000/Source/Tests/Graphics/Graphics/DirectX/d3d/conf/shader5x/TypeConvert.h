//////////////////////////////////////////////////////////////////////////
//  TypeConvert.h
//  created:	2005/03/10
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"

class CShader5xTest_TypeConvert : public CShader5xTest
{
public:
	CShader5xTest_TypeConvert();

protected:
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	TEST_RESULT SetupTestCase();
	TEST_RESULT CheckPixel( BYTE Pixel[4][4] );

	virtual void InitTestParameters() = 0;
	virtual tstring GetInstruction() = 0;
	virtual tstring	GetCompare() = 0;
	virtual void CalcResult() = 0;

	bool			m_bResultIsFloat;
	
	ID3D11PixelShader* m_pPassThroughPS;
	ID3D11HullShader* m_pPassThroughHS;
	ID3D11DomainShader* m_pPassThroughDS;

	UINT32			m_uiSrc[4];
	UINT32			m_uiResult[4];
};
	
class CShader5xTest_ftoi : public CShader5xTest_TypeConvert
{
protected:
	void InitTestParameters();
	tstring GetInstruction();
	tstring	GetCompare();
	void CalcResult();

	SRC_MOD_TYPE	m_SrcMod;
};

class CShader5xTest_ftou : public CShader5xTest_TypeConvert
{
protected:
	void InitTestParameters();
	tstring GetInstruction();
	tstring	GetCompare();
	void CalcResult();
};

class CShader5xTest_itof : public CShader5xTest_TypeConvert
{
protected:
	void InitTestParameters();
	tstring GetInstruction();
	tstring	GetCompare();
	void CalcResult();
	
	SRC_MOD_TYPE	m_SrcMod;
};

class CShader5xTest_utof : public CShader5xTest_TypeConvert
{
protected:
	void InitTestParameters();
	tstring GetInstruction();
	tstring	GetCompare();
	void CalcResult();
};

class CShader5xTest_f32tof16 : public CShader5xTest_TypeConvert
{
protected:
	TEST_RESULT Setup();
	void InitTestParameters();
	tstring GetInstruction();
	tstring	GetCompare();
	void CalcResult();

	SRC_MOD_TYPE	m_SrcMod;
};

class CShader5xTest_f16tof32 : public CShader5xTest_TypeConvert
{
protected:
	TEST_RESULT Setup();	
	void InitTestParameters();
	tstring GetInstruction();
	tstring	GetCompare();
	void CalcResult();

	SRC_MOD_TYPE	m_SrcMod;
};
