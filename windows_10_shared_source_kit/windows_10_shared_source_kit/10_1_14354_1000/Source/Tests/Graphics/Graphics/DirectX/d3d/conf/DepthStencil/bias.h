//////////////////////////////////////////////////////////////////////////
//  bias.h
//  created:	2006/11/10
//
//  purpose:    conformance for depth bias in rasterizer state
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "depth.h"

class CBiasTest : public CDepthTest
{
public:
	CBiasTest();

private:
	void InitTestParameters();
	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	void Cleanup();

	void FillVBBias(const std::vector<float> &vData, std::vector<float> &vMaxDepthSlope);
	D3D11_RASTERIZER_DESC m_RSDesc;
};

