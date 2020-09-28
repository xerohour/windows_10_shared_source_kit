// DXGIFloatTypes.h
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

#pragma once

class CDXGIFloat16
{
public:
	CDXGIFloat16() {}
	CDXGIFloat16( float f );
	CDXGIFloat16( CDXGIFloat16 const &f ) { m_uValue = (UINT16) f; }
	CDXGIFloat16( UINT16 v ) { m_uValue = v; }
	
	enum
	{
		FRAC_BITS = 10,
		EXP_BITS = 5,
		SIGN_BIT = 15,
		SIGN_BIT_MASK = (1 << SIGN_BIT),
		FRAC_MASK = ( 1 << FRAC_BITS ) - 1,
		EXP_MASK = ( (1 << EXP_BITS) - 1 ) << FRAC_BITS,
		EXP_BIAS = ( 1 << (EXP_BITS - 1) ) - 1,
		EXP_MAX = EXP_BIAS,
		EXP_MIN = -EXP_BIAS + 1,
		MAX_UNBIASED_NORM = ( (EXP_MAX+127) << 23 ) | 0x7FE000,
		MIN_UNBIASED_NORM = ( EXP_MIN + 127 ) << 23,
		BIAS_DIFF = ( EXP_BIAS - 127 ) << 23,
		FRAC_BITS_DIFF = 23 - FRAC_BITS,

		NAN_VALUE = 0x7fff,
		POS_INFINITY = 0x7c00,
		NEG_INFINITY = 0xfc00,
		MAX_POS_VALUE = 0x7bff,
		MIN_POS_VALUE_NORM = 0x0400,
		MIN_POS_VALUE_DENORM = 0x0001,
	};
	
	operator float() const;
	operator UINT16() const { return m_uValue; }

	bool IsNan() const;
	bool IsFinite() const;
	
private:
	UINT16 m_uValue;
};


class CDXGIFloat11
{
public:
	CDXGIFloat11() {}
	CDXGIFloat11( float f );
	CDXGIFloat11( CDXGIFloat11 const &f ) : m_dwValue( f.m_dwValue ) {}
	CDXGIFloat11( DWORD dwValue ) : m_dwValue( dwValue ) {}
	
	enum
	{
		FRAC_BITS = 8,
		EXP_BITS = 3,
		FRAC_MASK = ( 1 << FRAC_BITS ) - 1,
		EXP_MAX = ( 1 << EXP_BITS ) - 1,
		EXP_MASK = EXP_MAX << FRAC_BITS,
		EXP_BIAS = EXP_MAX >> 1,
		EXP_MAX_UNBIASED_NORM = EXP_MAX - EXP_BIAS,
		EXP_MIN_UNBIASED_NORM = 1 - EXP_BIAS,
		MAX_VALUE = EXP_MASK | FRAC_MASK,
	};
	
	operator float() const;
	operator DWORD() const { return m_dwValue; }
	
private:
	DWORD m_dwValue;
	
	friend class CDXGIFloat11;
};


class CDXGIFloat10
{
public:
	CDXGIFloat10() {}
	CDXGIFloat10( float f );
	CDXGIFloat10( CDXGIFloat10 const &f ) : m_dwValue( f.m_dwValue ) {}
	CDXGIFloat10( DWORD dwValue ) : m_dwValue( dwValue ) {}
	
	enum
	{
		FRAC_BITS = 7,
		EXP_BITS = 3,
		FRAC_MASK = ( 1 << FRAC_BITS ) - 1,
		EXP_MAX = ( 1 << EXP_BITS ) - 1,
		EXP_MASK = EXP_MAX << FRAC_BITS,
		EXP_BIAS = EXP_MAX >> 1,
		EXP_MAX_UNBIASED_NORM = EXP_MAX - EXP_BIAS,
		EXP_MIN_UNBIASED_NORM = 1 - EXP_BIAS,
		MAX_VALUE = EXP_MASK | FRAC_MASK,
	};
	
	operator float() const;
	operator DWORD() const { return m_dwValue; }
	
private:
	DWORD m_dwValue;
	
	friend class CDXGIFloat10;
};
