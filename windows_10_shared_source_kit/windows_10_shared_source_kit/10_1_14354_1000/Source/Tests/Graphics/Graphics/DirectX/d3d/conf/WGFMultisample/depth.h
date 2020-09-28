//////////////////////////////////////////////////////////////////////////
//  Depth.h
//  created:	2006/08/03
//
//  purpose:    test multisample depth
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "multisample.h"
#include <D3DDepthHelper.h>

enum TEST_TYPE {
	NORMAL,
	COMBO_MSOFF, //	cases while toggling multisample
	COMBO_ODEPTH // cases while toggling writing odepth
};

class CMultisampleTest_Depth : public CMultisampleTest
{
public:
	CMultisampleTest_Depth();
	virtual void InitTestParameters();

protected:
	enum TEST_LENGTH {
		SMALL,   // short and long are taken...
		MEDIUM,
		LARGE
	};

	BOOL IsPixelBlack();
	void SetDepth( float );
	virtual TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();

	void SetSample(UINT index);

	ID3D11PixelShader		*m_pPSDepth;
	ID3D11Texture2D   		*m_pDSTex2D;
	ID3D11DepthStencilView	*m_pDSView;
	ID3D11DepthStencilState	*m_pDSState;
	ID3D11DepthStencilState  *m_pDSCompState;
	ID3D11RasterizerState	*m_pRSMSOff;

	bool				 m_bMSEnable;
	TEST_LENGTH			 m_TestLength;
	TEST_TYPE			 m_TestType;
	UINT				 m_uCurrentSample;
};
	

//
// 
//
class CMultisampleTest_DepthHelper : public CMultisampleTest_Depth
{
public:
	CMultisampleTest_DepthHelper();
	virtual void InitTestParameters();

protected:
	void CreateDSTex();
	virtual TEST_RESULT ExecuteTestCase();
	void BeginQuery();
	UINT64 EndQuery();


	// query
	ID3D11Query				*m_pQuery;
	UINT				m_uFirstSlice;
	UINT				m_uArraySize;

};