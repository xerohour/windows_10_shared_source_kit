//////////////////////////////////////////////////////////////////////////
//  IntArith.h
//  created:	2005/03/10
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"
	
class CShader5xTest_IArith : public CShader5xTest
{
public:
	CShader5xTest_IArith() : 
		CShader5xTest(DXGI_FORMAT_R32G32B32A32_UINT),
		m_pCurrentUAView(NULL)
	{
	}

protected:
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	virtual tstring GetInstruction() = 0;
	virtual void CalcResult() = 0;
	
	virtual TEST_RESULT CheckPixel( BYTE Pixel[4][4] );

	UINT32			m_uiResult[4];
	ID3D11UnorderedAccessView* m_pCurrentUAView;
	
};
	
class CShader5xTest_IArith_Binary : public CShader5xTest_IArith
{
public:
	virtual void InitTestParameters();

protected:
	TEST_RESULT SetupTestCase();

	UINT32			m_uiSrc0[4];
	UINT32			m_uiSrc1[4];
	SRC_MOD_TYPE	m_SrcMod[2];
};
class CShader5xTest_IArith_BinaryNoNeg : public CShader5xTest_IArith_Binary
{
public:
	virtual void InitTestParameters();
};
class CShader5xTest_IArith_Unary : public CShader5xTest_IArith
{
public:
	virtual void InitTestParameters();

protected:
	TEST_RESULT SetupTestCase();

	UINT32			m_uiSrc[4];
	SRC_MOD_TYPE	m_SrcMod;
};
class CShader5xTest_IArith_Trinary : public CShader5xTest_IArith
{
public:
	virtual void InitTestParameters();

protected:
	TEST_RESULT SetupTestCase();

	UINT32			m_uiSrc0[4];
	UINT32			m_uiSrc1[4];
	UINT32			m_uiSrc2[4];
	SRC_MOD_TYPE	m_SrcMod[3];
};
class CShader5xTest_IArith_TrinaryNoNeg : public CShader5xTest_IArith_Trinary
{
public:
	virtual void InitTestParameters();
};
	
class CShader5xTest_IArith_DualResultTest : public CShader5xTest_IArith
{
protected:
	virtual TEST_RESULT SetupTestCase() = 0;
	virtual void CalcResults( ) = 0;
	void CalcResult() {return;}
	virtual TEST_RESULT CheckPixel( BYTE Pixel[4][4] ) = 0;
	
	UINT32			m_uiSrc0[4];
	UINT32			m_uiSrc1[4];
	SRC_MOD_TYPE	m_SrcMod[2];
	bool			m_bNULLSrc[2];
	UINT32			m_uiResult0[4];
	UINT32			m_uiResult1[4];
};
class CShader5xTest_IArith_DivTest : public CShader5xTest_IArith_DualResultTest
{
protected:
	TEST_RESULT SetupTestCase();
	TEST_RESULT CheckPixel( BYTE Pixel[4][4] );
};
class CShader5xTest_IArith_MulTest : public CShader5xTest_IArith_DualResultTest
{
protected:
	TEST_RESULT SetupTestCase();
	TEST_RESULT CheckPixel( BYTE Pixel[4][4] );
};
	
class CShader5xTest_iadd : public CShader5xTest_IArith_Binary
{
protected:
	tstring GetInstruction();
	void CalcResult();
};

class CShader5xTest_imad : public CShader5xTest_IArith_Trinary
{
protected:
	tstring GetInstruction();
	void CalcResult();
};

class CShader5xTest_imax : public CShader5xTest_IArith_Binary
{
protected:
	tstring GetInstruction();
	void CalcResult();
};


class CShader5xTest_imin : public CShader5xTest_IArith_Binary
{
protected:
	tstring GetInstruction();
	void CalcResult();
};


class CShader5xTest_imul : public CShader5xTest_IArith_MulTest
{
public:
	void InitTestParameters();

protected:
	tstring GetInstruction();
	void CalcResults( );
};


class CShader5xTest_ineg : public CShader5xTest_IArith_Unary
{
protected:
	tstring GetInstruction();
	void CalcResult();
};


class CShader5xTest_udiv : public CShader5xTest_IArith_DivTest
{
public:
	void InitTestParameters();

protected:
	tstring GetInstruction();
	void CalcResults( );
};

class CShader5xTest_umad : public CShader5xTest_IArith_TrinaryNoNeg
{
protected:
	tstring GetInstruction();
	void CalcResult();
};

class CShader5xTest_umax : public CShader5xTest_IArith_BinaryNoNeg
{
protected:
	tstring GetInstruction();
	void CalcResult();
};


class CShader5xTest_umin : public CShader5xTest_IArith_BinaryNoNeg
{
protected:
	tstring GetInstruction();
	void CalcResult();
};


class CShader5xTest_umul : public CShader5xTest_IArith_MulTest
{
public:
	void InitTestParameters();

protected:
	tstring GetInstruction();
	void CalcResults( );
};

class CShader5xTest_IArith_StatusBit : public CShader5xTest_IArith_Trinary
{
public:
	void InitTestParameters();
	TEST_RESULT Setup();

protected:
	bool m_bTestStatusBit;
};

class CShader5xTest_uaddc : public CShader5xTest_IArith_StatusBit
{
protected:
	tstring GetInstruction();
	void CalcResult();
};

class CShader5xTest_usubb : public CShader5xTest_IArith_StatusBit
{
protected:
	tstring GetInstruction();
	void CalcResult();
};
