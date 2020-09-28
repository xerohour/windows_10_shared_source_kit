#pragma once

#include "Arithmetic.h"
#include "Precision.h"

class CPSInputPrecisionTest : public CPrecisionTest
{
public:
	enum ERegisterType
	{
		ERT_POSITION,
		ERT_TEXCOORD,
		ERT_INT,
	};
	struct Vertex10
	{
		float pos[2];
		float texcoord[4];
		int intData;
	};
	struct Vertex9
	{
		float pos[2];
		float texcoord[4];
		float ignoredData;
	};

public:
	CPSInputPrecisionTest() {}

	virtual void			InitTestParameters( );

	virtual bool			CreateResources( );

	virtual TEST_RESULT		SetupTestCase( );
	virtual TEST_RESULT		ExecuteTestCase( );
	virtual void			CleanupTestCase( );
	virtual TEST_RESULT		ValidateResult( );

protected:
	ERegisterType			m_RegisterType;

	static const UINT		m_RTWidth = 2048;
	static const UINT		m_RTHeight = 2;
};

//---------------------------------------------------------------

class CVSInputPrecisionTest : public CArithmeticPrecisionTest
{
public:
	struct Vertex
	{
		float pos[2];
		union
		{
			float fTexcoord[4];
			int   iTexcoord[4];
			UINT  uTexcoord[4];
		};
	};

	CVSInputPrecisionTest() : m_CreatedValueType( VT_Float ) {}

	virtual void			InitTestParameters();
	virtual DXGI_FORMAT		GetRenderTargetFormat();
	virtual bool			CreateResources();
	virtual bool			CreateShaders();

	virtual TEST_RESULT		SetupTestCase();
	virtual TEST_RESULT		ExecuteTestCase();
	virtual TEST_RESULT		ValidateResult();

	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor() { testfactor::RFactor rEmpty; return rEmpty; }

	char* GetTypeName( CArithmeticPrecisionTest::EValueType type );

	DXGI_FORMAT GetVertexBufferFormat();

protected:
	EValueType					m_ValueType;
	EValueType					m_CreatedValueType;
};

