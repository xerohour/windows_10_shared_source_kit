//
// DXGIBufferCompare.h
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

#pragma once

#define COMPARE_CACHE_SIZE 65536

class CBufferCompare
{
public:	
	// Member types
	enum TYPE
	{
		TYPE_UNKNOWN = 0,
		TYPE_FLOAT,
		TYPE_INT8,
		TYPE_INT16,
		TYPE_INT32,
		TYPE_UINT8,
		TYPE_UINT16,
		TYPE_UINT32
	};
	
	struct ElementDesc
	{
		DXGI_FORMAT Format;
		UINT uElementSize;		// Size of element in bytes
		UINT uNumComponents;	// Number of components in chunk
		UINT uNumChunks;		// Number of chunks (not necessarily the number of components)
		UINT uChunkBitSize;		// Size of chunk of data in bits (not necessarily the size of a component)
		TYPE compareType;		// Type to expand components to
		UINT uCompChunkIndex[4]; // Index of chunk that component is in
		UINT uCompShift[4];		// Number of bits from right of chunk to the LSB of component
		UINT uCompBitSize[4];	// Size of components in bits
	};

public:
	CBufferCompare();
	virtual ~CBufferCompare();
	
	void SetFloat32ToleranceULP( int nULP ) { m_uFloat32TolULP = nULP; }
	void SetFloat16ToleranceULP( int nULP ) { m_uFloat16TolULP = nULP; }
	void SetFloat10ToleranceULP( int nULP ) { m_uFloat10TolULP = nULP; }
	
	// SetFloat32Tolerance
	//	Tolerance values used when comparing floats >= 32 bits in precision.
	void SetFloat32Tolerance( float fTolRel, float fTolAbs ) { m_fFloat32TolRel = fTolRel; m_fFloat32TolAbs = fTolAbs; }
	
	// SetFloat16Tolerance
	//	Tolerance values used when comparing floats >= 16 bits and < 32 bits in precision.
	void SetFloat16Tolerance( float fTolRel, float fTolAbs ) { m_fFloat16TolRel = fTolRel; m_fFloat16TolAbs = fTolAbs; }
	
	// SetFloat10Tolerance
	//	Tolerance values used when comparing floats >= 10 bits and < 16 bits in precision.
	void SetFloat10Tolerance( float fTolRel, float fTolAbs ) { m_fFloat10TolRel = fTolRel; m_fFloat10TolAbs = fTolAbs; }
	
	// SetInt32Tolerance
	//	Tolerance values used when comparing integer values > 16 bits and <= 32 bits in size.
	void SetInt32Tolerance( float fTolPct, UINT32 uTolAbs ) { m_fInt32TolPct = fTolPct; m_uInt32TolAbs = uTolAbs; }
	
	// SetInt16Tolerance
	//	Tolerance values used when comparing integer values > 8 bits and <= 16 bits in size.
	void SetInt16Tolerance( float fTolPct, UINT32 uTolAbs ) { m_fInt16TolPct = fTolPct; m_uInt16TolAbs = uTolAbs; }
	
	// SetInt8Tolerance
	//	Tolerance values used when comparing integer values >= 1 bits and <= 8 bits in size.
	void SetInt8Tolerance( float fTolPct, UINT32 uTolAbs ) { m_fInt8TolPct = fTolPct; m_uInt8TolAbs = uTolAbs; }
	
	// SetPerElementTolAdjustment
	//	The per-element tolerance adjustment is compared to the average percent
	//	error (error / error_max) for each component of the element.  The default
	//	value of this is 1.0.  A value below 1.0 indicates a tighter overall tolerance
	//	at the component level and a value above 1.0 indicates a looser overall tolerance.
	void SetPerElementTolAdjustment( float fPerElementTolAdjustment ) { m_fPerElementTolAdjustment = fPerElementTolAdjustment; }
	
	UINT Compare( 
		void *pSrcData, 
		void *pRefData, 
		DXGI_FORMAT Format, 
		UINT uElementOffset, 
		UINT uElementStride,
		UINT uWidth,
		UINT uHeight = 1,
		UINT uSrcRowPitch = 0,
		UINT uRefRowPitch = 0,
		UINT uDepth = 1, 
		UINT uSrcDepthPitch = 0,
		UINT uRefDepthPitch = 0
		);
		
	// Compares elements of a source buffer to a reference buffer and returns the
	// number of failed comparisons.
	
	static bool DXGIFormatToElementDesc( DXGI_FORMAT Format, ElementDesc *desc );

protected:
	// Helper Functions
	bool D3D10FormatToElementDesc( DXGI_FORMAT Format, ElementDesc *desc ) const;
	

private:
	// Floating point metrics
	// For floating point comparisons max_error = max( rel_error, abs_error )
	float m_fFloat32TolRel;				// Relative tolerance for float32 comparisons (rel_error = m_fFloat32TolRel * 2 ^ ref_exponent)
	float m_fFloat32TolAbs;				// Absolute tolerance for float32 comparisons (abs_error = m_fFloat32TolAbs)
	float m_fFloat16TolRel;				// Relative tolerance for float16 comparisons (rel_error = m_fFloat16TolRel * 2 ^ ref_exponent)
	float m_fFloat16TolAbs;				// Absolute tolerance for float16 comparisons (abs_error = m_fFloat16TolAbs)
	float m_fFloat10TolRel;				// Relative tolerance for Float10 comparisons (rel_error = m_fFloat10TolRel * 2 ^ ref_exponent)
	float m_fFloat10TolAbs;				// Absolute tolerance for Float10 comparisons (abs_error = m_fFloat10TolAbs)
	UINT32 m_uFloat32TolULP;				// Floating point error represented in ULP (units last place).
	UINT32 m_uFloat16TolULP;				// Floating point error represented in ULP (units last place).
	UINT32 m_uFloat10TolULP;				// Floating point error represented in ULP (units last place).
	
	// Integer metrics
	// For integer comparisons max_error = max( rel_error, abs_error )
	float m_fInt32TolPct;				// Relative tolerance (rel_error = ref_value * ( 1 - m_fTolPct ))
	UINT32 m_uInt32TolAbs;				// Absolute tolerance (abs_error = m_uTolAbs)
	float m_fInt16TolPct;				// Relative tolerance (rel_error = ref_value * ( 1 - m_fTolPct ))
	UINT32 m_uInt16TolAbs;				// Absolute tolerance (abs_error = m_uTolAbs)
	float m_fInt8TolPct;				// Relative tolerance (rel_error = ref_value * ( 1 - m_fTolPct ))
	UINT32 m_uInt8TolAbs;				// Absolute tolerance (abs_error = m_uTolAbs)

	float m_fPerElementTolAdjustment;
		
	INT8 *m_pCache;
};



