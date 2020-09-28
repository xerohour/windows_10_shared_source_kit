//////////////////////////////////////////////////////////////////////////
//  ConstantBuffers.h
//  created:	2006/06/29
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"

#define NUM_CONSTANT_BUFFER_SLOTS		15u
#define	MAX_CB_ELEMS					4091u
#define	LIMIT_TEST_VALUE				39.f

class CShader5xTest_ConstantBuffers : public CShader5xTest
{
public:
	CShader5xTest_ConstantBuffers() :
		m_pRTBuffer( NULL ),
		m_pRTCopy( NULL ),
		m_pRTV( NULL ),
		m_pRTUAV( NULL ),
		m_pVB( NULL ),
		m_pVS( NULL ),
		m_pVSPass( NULL ),
		m_pGS( NULL ),
		m_pPS( NULL ),
		m_pPSPass( NULL ),
		m_pLayout( NULL ),
		m_pBuffer14( NULL ),
		m_pSpecialFloats(&m_fInfP)
	{ 
		UINT32 uInfP = 0x7f800000;
		UINT32 uInfN = 0xff800000;
		UINT32 uNaN = 0x7f800001;
		UINT32 uDenorm = 0x00000001;

		m_fInfP = *( (float*) &uInfP );
		m_fInfN = *( (float*) &uInfN );
		m_fNaN = *( (float*) &uNaN );
		m_fDenorm = *( (float*) &uDenorm );
	}

	void				InitTestParameters();
	
	TEST_RESULT			SetupTestCase();
	TEST_RESULT			ExecuteTestCase();
	void				CleanupTestCase();

	TEST_RESULT			Setup();
	void				Cleanup();
	
	TEST_RESULT         BuildShaders();
	bool                WithinOneULP( float fReference, float fOutput );
	bool                IsDenorm( float fValue );
	bool                IsNaN( float fValue );
	UINT32              GetTestPosition( UINT32 testPoint, UINT32 cbIndex ) const;
	TEST_RESULT         BindConstantBuffers();

protected:
	tstring				m_szSlotContents;
	bool				m_bDynamic;
	bool				m_bSpecials;
	bool				m_bUnsized;
	bool				m_bMaxElems;

	ID3D11Buffer		*m_pBuffers[ NUM_CONSTANT_BUFFER_SLOTS - 1 ];
	ID3D11Buffer		*m_pBuffer14;
	ID3D11Buffer			*m_pRTBuffer;
	ID3D11Buffer			*m_pRTCopy;
	ID3D11Buffer				*m_pVB;
	ID3D11RenderTargetView	*m_pRTV;
	ID3D11UnorderedAccessView	*m_pRTUAV;
	ID3D11VertexShader		*m_pVS;
	ID3D11VertexShader		*m_pVSPass;
	ID3D11GeometryShader		*m_pGS;
	ID3D11PixelShader		*m_pPS;
	ID3D11PixelShader		*m_pPSPass;
	ID3D11InputLayout		*m_pLayout;

	float               m_fInfP;
	float               m_fInfN;
	float               m_fNaN;
	float               m_fDenorm;
	const float*        m_pSpecialFloats;

	bool				m_bWasDebug;
	
	float				m_matWorld[4][4];

	UINT32 m_uConstantBufferSizeInConstants;
	UINT32 m_uBufferWidthInBytes;
	UINT32 m_uBufferOffsetInBytes;
	UINT32 m_uNumConstantsInBytes;

	struct CB_VERTEX
	{
		float  position[4];
		UINT32  positionTL[2];
	};
};
