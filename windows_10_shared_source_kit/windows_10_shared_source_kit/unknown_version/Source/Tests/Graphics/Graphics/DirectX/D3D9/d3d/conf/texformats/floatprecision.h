/*==========================================================================;
 *
 *  Copyright (C) 1994-2004 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	FloatPrecision.h
 *
 ***************************************************************************/

#pragma once


#include "CD3DTest.h"
#include "CTextures.h"
#include "texformats.h"
#include "assert.h"

//-----------------------------------------------------------------------------
// Floating point precision tests
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

struct FloatPrecisionParam
{
	float value;
	float epsilon;
	
	FloatPrecisionParam( int sign, int exp, float mant, int numfracbits )
	{
		assert( exp <= 127 && exp >= -126 );
		assert( numfracbits > 0 && numfracbits <= 23 );
		
		*( (DWORD *) &value ) = 
			( sign << 31 ) | 
			( ( 127 + exp ) << 23 ) |
			( *( (DWORD *) &mant ) & 0x007fffff );
			
		*( (DWORD *) &epsilon ) = 
			( ( 127 + exp - numfracbits ) << 23 );
	}
	
	FloatPrecisionParam( const FloatPrecisionParam &fpp ) : value( fpp.value ), epsilon( fpp.epsilon ) {}
	FloatPrecisionParam( float fpp_value, float fpp_epsilon ) : value( fpp_value ), epsilon( fpp_epsilon ) {}	
	FloatPrecisionParam( DWORD fpp_dw_value, float fpp_epsilon ) : epsilon( fpp_epsilon )
	{
		*( (DWORD *) &value ) = fpp_dw_value;
	}	
};


//-----------------------------------------------------------------------------
// Float texture precision class definition
//-----------------------------------------------------------------------------

class CFloatTexPrecisionTest : public CD3DTest
{
public:
	CFloatTexPrecisionTest();
	virtual ~CFloatTexPrecisionTest();
	
	// CD3DTest overloads
	virtual bool CapsCheck();
	virtual bool Setup();
	virtual void Cleanup();
	virtual void UpdateStatus();
	virtual void UpdateDetails();
	virtual bool ExecuteTest( UINT uTestNumber );
	
	virtual bool ProcessFrame();
	
	virtual int GetPSVersionMajor() const = 0;
	
protected:
	bool BuildPixelShader( const char *lpzSource, CnPixelShader **ppPixelShader );
	
protected:
	FORMAT*		m_pTexFormat;
	
	CnPixelShader *m_pPixelShader;
	const FloatPrecisionParam *m_pPrecisionTable;
	
	TLVERT m_pVertList[6];
};



class CFloatTexPrecisionTest20 : public CFloatTexPrecisionTest
{
public:
	CFloatTexPrecisionTest20();
	virtual int GetPSVersionMajor() const { return 2; }
	UINT TestInitialize();
};

class CFloatTexPrecisionTest30 : public CFloatTexPrecisionTest
{
public:
	CFloatTexPrecisionTest30();
	virtual int GetPSVersionMajor() const { return 3; }
	UINT TestInitialize();
};


class CFloatTexBadValuesTest20 : public CFloatTexPrecisionTest
{
public:
	CFloatTexBadValuesTest20();
	UINT TestInitialize();
	int GetPSVersionMajor() const { return 2; }
	virtual bool ProcessFrame();
};

class CFloatTexBadValuesTest30 : public CFloatTexPrecisionTest
{
public:
	CFloatTexBadValuesTest30();
	int GetPSVersionMajor() const { return 3; }
	UINT TestInitialize();
	virtual bool ProcessFrame();
};



class CFloatTexRTPrecisionTest : public CFloatTexPrecisionTest
{
public:
	CFloatTexRTPrecisionTest();
	virtual ~CFloatTexRTPrecisionTest();
	
	// CD3DTest overloads
	virtual bool Setup();
	virtual void Cleanup();
	bool ExecuteTest( UINT uTestNumber );	
	virtual bool ProcessFrame();
	
	virtual int GetPSVersionMajor() const = 0;
	
protected:
	CnTexture *m_pTex;
	CnTexture *m_pSysMemTex;
	int m_nTestIndex;
};


class CFloatTexRTPrecisionTest20 : public CFloatTexRTPrecisionTest
{
public:
	CFloatTexRTPrecisionTest20();
	int GetPSVersionMajor() const { return 2; }
	UINT TestInitialize();
};

class CFloatTexRTPrecisionTest30 : public CFloatTexRTPrecisionTest
{
public:
	CFloatTexRTPrecisionTest30();
	int GetPSVersionMajor() const { return 3; }
	UINT TestInitialize();
};


