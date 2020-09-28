#include "FillLine.h"
#include <d3dmshelper.h>
#include <math.h>
using namespace testfactor;

// redner target height and width used to create the image for comparison
#define RT_HEIGHT 8
#define RT_WIDTH 8
#define SKIP 9999	// this value indicating we should skip the interior point when we draw the line

const FLOAT unit = 1.f / 256.f; // one subpixel step

FillLine::FillLine() : CMultisampleTest_Fill() 
{ 
	// disable multi-sampling
	m_bMSEnable = false;
	// set the render target size to be in m x n pixels
	m_RTWidth = RT_HEIGHT;
	m_RTHeight = RT_WIDTH;
	blendState = NULL;
}

TEST_RESULT FillLine::SetupTestCase() {
	TEST_RESULT tRes = CMultisampleTest_Fill::SetupTestCase();

	// add alpha blending to check whether a pixel is lit only once
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(BlendState));
	BlendState.RenderTarget[0].BlendEnable = true;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_BLEND_FACTOR;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = GetDevice()->CreateBlendState(&BlendState, &blendState);
	if (hr != S_OK)
	{
		WriteToLog(_T("CreateBlendState failed, hr = %s"), D3DHResultToString(hr).c_str());
		tRes = RESULT_FAIL;
	}
	return tRes;
}

void FillLine::CreatePixelResults() {
	// result 0 - no pixel lit
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	// result 1 - upper left corner pixel lit
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[1].setPixelValue(0, 0, 0.25f);  // set the upper left corner lit. 1 = lit; 0 = not lit.
	// result 2 
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[2].setPixelValue(0, 0, 0.25f);
	pr[2].setPixelValue(1, 0, 0.25f);
	pr[2].setPixelValue(2, 1, 0.25f);
	pr[2].setPixelValue(3, 1, 0.25f);
	// result 3
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[3].setPixelValue(3, 4, 0.25f);
	// result 4
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[4].setPixelValue(1, 1, 0.25f);
	pr[4].setPixelValue(2, 2, 0.25f);
	pr[4].setPixelValue(3, 3, 0.25f);
	pr[4].setPixelValue(2, 4, 0.25f);
	// result 5
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[5].setPixelValue(0, 0, 0.25f);
	pr[5].setPixelValue(1, 0, 0.25f);
	// result 6
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[6].setPixelValue(1, 0, 0.25f);
	pr[6].setPixelValue(1, 1, 0.25f);
	pr[6].setPixelValue(0, 2, 0.25f);
	pr[6].setPixelValue(0, 3, 0.25f);
	pr[6].setPixelValue(0, 4, 0.25f);
	// result 7
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[7].setPixelValue(0, 0, 0.25f);
	pr[7].setPixelValue(1, 1, 0.25f);
	// result 8
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[8].setPixelValue(0, 0, 0.25f);
	pr[8].setPixelValue(1, 1, 0.25f);
	pr[8].setPixelValue(1, 2, 0.25f);
	pr[8].setPixelValue(0, 3, 0.25f);
	// result 9
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[9].setPixelValue(0, 0, 0.25f);
	pr[9].setPixelValue(0, 1, 0.25f);
	// result 10
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[10].setPixelValue(1, 0, 0.25f);
	pr[10].setPixelValue(2, 0, 0.25f);
	// result 11
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[11].setPixelValue(1, 0, 0.25f);
	pr[11].setPixelValue(0, 1, 0.25f);
	// result 12
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[12].setPixelValue(1, 0, 0.25f);
	pr[12].setPixelValue(2, 0, 0.25f);
	pr[12].setPixelValue(0, 1, 0.25f);
	pr[12].setPixelValue(3, 1, 0.25f);
	// result 13
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[13].setPixelValue(0, 0, 0.25f);
	pr[13].setPixelValue(1, 0, 0.25f);
	pr[13].setPixelValue(2, 0, 0.25f);
	// result 14
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[14].setPixelValue(0, 0, 0.25f);
	pr[14].setPixelValue(1, 0, 0.25f * 2);	// this pixel should be drawn twice
	// result 15
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[15].setPixelValue(0, 0, 0.25f);
	pr[15].setPixelValue(1, 0, 0.25f);
	pr[15].setPixelValue(2, 0, 0.25f);
	// result 16
	pr.push_back( PixelResult(RT_HEIGHT, RT_WIDTH) );
	pr[16].setPixelValue(0, 0, 0.25f * 2);  // set the upper left corner lit. 1 = lit; 0 = not lit.
}

// all the lines that are going to be tested
static FLOAT lines [] = 
{
	// format : startX, startY, endX, endY,  pixel result index,

	// test of edges, line intersects the edge //
	0.5f, 0.5f, 1.0f, 1.0f,		1,  
	0.5f, 0.5f, 1.0f, 0.0f,		1,
	0.5f, 0.5f, 0.0f, 0.0f,		1,
	0.5f, 0.5f, 0.0f, 1.0f,		1,

	// test of edges, exiting the four edges, with starting point on each edge //
	0.0f + 40 * unit, 0.5f - 40 * unit, 0.0f, 0.0f,		0,		// upper left edge, not lit
	0.0f + 41 * unit, 0.5f - 40 * unit, 0.0f, 0.0f,		1,		// upper left edge boundary test, lit
	
	0.5f + unit, 0.0f + unit, 1.0f, 0.0f,				0,		// upper right edge, not lit
	0.5f + unit, 0.0f + 2 * unit, 1.0f, 0.0f,			1,		// upper right edge boundary test, lit

	0.0f + 30 * unit, 0.5f + 30 * unit, 0.0f, 1.0f,		1,		// lower left edge, lit
	0.0f + 29 * unit, 0.5f + 30 * unit, 0.0f, 1.0f,		0,		// lower left edge boundary test, not lit

	0.5f + 3 * unit, 1.0f - 3 * unit, 1.0f, 1.0f,		1,		// lower right edge, lit
	0.5f + 3 * unit, 1.0f - 2 * unit, 1.0f, 1.0f,		0,		// lower right edge boundary, not lit

	// test some lines starting and ending on the same edge, none should be lit //
	0.0f + 3 * unit, 0.5f - 3 * unit, 0.0f + 30 * unit, 0.5f - 30 * unit,	0, // upper left
	0.0f + 3 * unit, 0.5f + 3 * unit, 0.0f + 30 * unit, 0.5f + 30 * unit,	0, // lower left
	0.5f + 10 * unit, 0.0f + 10 * unit, 0.5f + 50 * unit, 0.0f + 50 * unit, 0, // upper right
	1.0f - unit, 0.5f + unit, 1.0f - 2 * unit, 0.5f + 2 * unit,	0,					   // lower right


	// test of diamond corners and x-major and y-major lines //
	// right corner
	0.5f, 0.5f, 1.0f, 0.5f,		1,	// exiting the right corner, x-major, so right corner not included
	3.5f, 4.5f, 4.0f, 4.5f,		3,	// exiting the right corner, x-major, so right corner not included
	1.0f - unit, 0.5f + unit, 1.0f, 0.5f,		1,	// boundary test the right corner
	1.0f, 0.5f, 1.0f, 0.0f,		1,	// y-major line, right corner included
	1.0f, 0.5f, 1.0f, 1.0f,		1,	// y-major line, right corner included
	1.0f, 0.5f, 1.5f, 0.0f,		0,	// x-major, right corner not included, boundary case with slope = -1
	1.0f, 0.5f, 1.5f - unit, 0.0f,		1,		// boundary test, y-major line now, right corner included
	1.0f, 0.5f, 1.5f, 1.0f,		0,  // x-major, right corner not included, slope = 1
	1.0f, 0.5f, 1.5f - unit, 1.0f,		1,		// boundary test, y-major line now, right corner included		

	// left corner
	0.5f, 0.5f, 0.0f, 0.5f,		1,  // exiting the left corner, x-major
	0.0f, 0.0f, 0.0f, 1.0f,		0,	// y-major, left corner not included
	0.0f, 0.5f, 0.5f, 0.5f,		0,	// doesn't work in the reverse direction
	0.0f + unit, 0.5f, 0.0f, 0.5f,		1,		// one boundary test of the left corner

	// upper corner
	0.5f, 0.5f, 0.5f, 0.0f,		1,	// exiting the upper corner, y-major
	0.5f, 1.0f, 0.5f, 0.0f,		1,  // similar to previous
	0.0f, 0.0f, 0.0f, 1.0f,		0,  // x-major, upper corner also not included
	0.5f, 0.5f, 0.5f, 0.0f + unit,		0, // one boundary test

	// lower corner
	0.5f, 0.5f, 0.5f, 1.0f,		0,	// lower corner included, pixel not lit for y-major
	0.5f, 0.5f, 0.5f + unit, 1.0f,		1,	// one boundary test
	0.0f, 1.0f, 1.0f, 1.0f,		1,	// lower corner included for x-major


	// intersects two diamond area edges //
	0.0f, 0.0f, 1.0f, 1.0f,		1,
	0.0f + 30 * unit, 0.0f + 30 * unit, 1.0f - 30 * unit, 0.0f + 30 * unit,		1,
	0.0f + 30 * unit, 0.0f + 30 * unit, 0.0f + 30 * unit, 1.0f - 30 * unit,		1,

	// test of degenerating lines //
	1.0f, 1.0f, 1.0f, 1.0f,		0,  // degenerating line outside diamond
	2.5f, 2.5f, 2.5f, 2.5f,		0,	// degenerating line inside diamond

	// small line segments outisde the diamond and inside the diamond
	0.0f + 30 * unit, 0.0f + 30 * unit, 0.0f + 60 * unit, 0.0f + 60 * unit,		0,
	0.5f + 30 * unit, 0.0f + 29 * unit, 1.0f, 0.0f + 29 * unit,		0,
	0.0f + 2 * unit, 1.0f - 20 * unit, 0.0f + 10 * unit, 1.0f - 20 * unit,		0,
	1.0f - unit, 1.0f - unit, 1.0f, 1.0f,		0,
	0.5f, 0.5f, 0.5f + 20 * unit, 0.5f + 20 * unit,		0,

	// =============================================================================== //

	// inter-pixel lines //
	0.0f, 0.5f, 4.5f, 2.0f,		2,
	0.0f + 40 * unit, 0.0f + 40 * unit, 2.0f - unit, 1.0f - unit,	5,  // testing the end point
	1.5f, 1.0f, 0.0f, 5.5f,		6,
	0.0f, 0.5f, 2.0f, 2.5f,		7,
	0.5f, 1.0f, 2.5f, 3.0f,		7,
	2.0f, 0.5f, 0.0f, 2.5f,		11, 
	0.0f, 2.5f, 2.0f, 0.5f,		11,
	0.0f, 1.0f, 3.0f, 1.0f,		15,
		
};

// similar to one line case, also test the diamond exit rule for line strips, 
// including testing the different areas of the diamonds along the lines.
// interior points 
static FLOAT lineStrips [] = 
{
	// format : startX, startY, interiorX, interiorY, endX, endY,  pixel result index,

	// intra-pixel //
	0.0f, 0.0f, 0.5f, 0.0f, 1.0f, 0.5f,		0,
	0.0f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f,		0,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,		1,  // test interior point
	0.0f, 0.0f, 0.0f + unit, 0.0f + unit, 0.0f + unit * 2, 0.0f + unit * 2,		0,

	// inter-pixel //
	1.5f, 2.0f, 3.5f, 4.0f, 1.5f, 6.0f,		4,
	0.0f + 40 * unit, 0.0f + 40 * unit, 2.0f - 3 * unit, 0.0f + 4 * unit,  2.0f - unit, 1.0f - unit,	14,
	0.0f, 0.5f, 2.0f, 2.5f, 0.0f, 4.5f,		8,
	0.0f, 2.5f, 2.0f, 0.5f, 4.0f, 2.5f,		12,
	// test the interior point
	1.0f, 0.5f, 1.0f, 1.5f, 1.0f, 2.5f,		9,
	2.5f, 1.0f,	1.5f, 1.0f, 0.5f, 1.0f,		10,
	0.0f, 0.5f, 2.0f, 0.5f, 3.0f, 1.5f,		13,
	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,		16
};


void FillLine::InitTestParameters()
{
	m_RTFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_uSampleCount = 1;
	m_QualityLevelScenario = QUALITY_LEVEL_ZERO;

	// specify the resulting image
	CreatePixelResults();	// fill the vector with expected results

	// one line segment, one start point and one ending point
	// starting and ending point value sets
	CUserValueSet<FLOAT> *startXValueSet = new CUserValueSet<FLOAT>();
	CUserValueSet<FLOAT> *startYValueSet = new CUserValueSet<FLOAT>();

	CUserValueSet<FLOAT> *endXValueSet = new CUserValueSet<FLOAT>();
	CUserValueSet<FLOAT> *endYValueSet = new CUserValueSet<FLOAT>();

	// add values to the value sets
	for (int i = 0; i < ARRAY_SIZE(lines); i =  i + 5) 
	{
		startXValueSet->AddValue(lines[i]);
		startYValueSet->AddValue(lines[i+1]);
		endXValueSet->AddValue(lines[i+2]);
		endYValueSet->AddValue(lines[i+3]);
	}

	// set up the test factors
	rsx = AddParameter(_T( "StartX" ), &line.startX, startXValueSet);
	RFactor rsy = AddParameter(_T( "StartY" ), &line.startY, startYValueSet);
	
	CTestCaseParameter<FLOAT> *pInteriorX = AddParameter(_T( "InteriorX" ), &line.intX );
	pInteriorX->SetDefaultValue(SKIP);	// put value SKIP to indicate it is not a line strip
	CTestCaseParameter<FLOAT> *pInteriorY = AddParameter(_T( "InteriorY" ), &line.intY );
	pInteriorY->SetDefaultValue(SKIP);  // put value SKIP to indicate it is not a line strip

	RFactor rex = AddParameter(_T( "EndX" ), &line.endX, endXValueSet);
	RFactor rey = AddParameter(_T( "EndY" ), &line.endY, endYValueSet);

	// for line strips with one interior point, do the same thing
	// starting point value sets
	CUserValueSet<FLOAT> *sxVS = new CUserValueSet<FLOAT>();
	CUserValueSet<FLOAT> *syVS = new CUserValueSet<FLOAT>();
	// interior point value sets
	CUserValueSet<FLOAT> *intxVS = new CUserValueSet<FLOAT>();
	CUserValueSet<FLOAT> *intyVS = new CUserValueSet<FLOAT>();
	// ending point value sets
	CUserValueSet<FLOAT> *exVS = new CUserValueSet<FLOAT>();
	CUserValueSet<FLOAT> *eyVS = new CUserValueSet<FLOAT>();

	for (int i = 0; i < ARRAY_SIZE(lineStrips); i =  i + 7) 
	{
		sxVS->AddValue(lineStrips[i]);
		syVS->AddValue(lineStrips[i+1]);
		intxVS->AddValue(lineStrips[i+2]);
		intyVS->AddValue(lineStrips[i+3]);
		exVS->AddValue(lineStrips[i+4]);
		eyVS->AddValue(lineStrips[i+5]);
	}

	rsxLS = AddParameterValueSet(_T( "StartX" ), sxVS);
	RFactor rsyLS = AddParameterValueSet(_T( "StartY" ), syVS);
	RFactor rixLS = AddParameterValueSet(_T("InteriorX"), intxVS);
	RFactor riyLS = AddParameterValueSet(_T("InteriorY"), intyVS);
	RFactor rexLS = AddParameterValueSet(_T( "EndX" ), exVS);
	RFactor reyLS = AddParameterValueSet(_T( "EndY" ), eyVS); 

	// run case by case
	SetRootTestFactor( rsx % rsy % rex % rey +		// independent line tests
		rsxLS % rsyLS % rixLS % riyLS % rexLS % reyLS );	// line strip tests
}

// determine whether a line segment is a line strip or not
bool FillLine::isLineStrip(const LineSegment &ls) {
	if (ls.intX == SKIP)  // if the interior point value is default, then it is a line
		return false;
	else  // not default, line strip
		return true;
}

void FillLine::Draw() 
{
	HRESULT hr;
	float bfactor [] = { 0.25, 1, 1, 1 };	// set blend factor for the red channel
	GetEffectiveContext()->OMSetBlendState(blendState, bfactor, 0xffffffff);

	VERTEX *pDestVerts;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( hr = GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
		throw TRException("m_pVertexBuffer->Map() failed.", hr, RESULT_FAIL);
	pDestVerts = (VERTEX*) mappedRes.pData;

	pDestVerts[0].pos = D3DXVECTOR2(line.startX, line.startY);  // we always have one start point

	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );

	if ( !isLineStrip(line) ) 
	{	// only a line
		pDestVerts[1].pos = D3DXVECTOR2(line.endX, line.endY);
		GetEffectiveContext()->Unmap(m_pVertexBuffer,0);
		GetEffectiveContext()->Draw(2, 0);
	}
	else 
	{	// a line strip with one interior point
		pDestVerts[1].pos = D3DXVECTOR2(line.intX, line.intY);
		pDestVerts[2].pos = D3DXVECTOR2(line.endX, line.endY);
		GetEffectiveContext()->Unmap(m_pVertexBuffer,0);
		GetEffectiveContext()->Draw(3, 0);
	}

	GetEffectiveContext()->OMSetBlendState( NULL, 0, 0xffffffff );	// set the blend state back after drawing
}

TEST_RESULT FillLine::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		// background color is black
		FLOAT ClearColor[4] = { 0, 0, 0, 0 };
		GetEffectiveContext()->ClearRenderTargetView( m_pRTView, ClearColor );
		Draw();
		// Present 
		if (g_TestApp.m_bPresent) {
			// Copy result to swapchain texture 
			ID3D11Texture2D *pSwapChainTexture = NULL;
			IDXGISwapChain *pSwapChain = g_TestApp.GetSwapChain();
			if ( FAILED( hr = pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**) &pSwapChainTexture ) ) )
				throw TRException("[CMultisampleTest::Present] CreateTexture2DFromSwapChain failed.", hr, RESULT_FAIL);
			GetEffectiveContext()->CopyResource(pSwapChainTexture, m_pRTTexture2D);
			
			if ( FAILED( hr = ExecuteEffectiveContext() ) )
				throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

			// Present the result
			g_TestApp.Present( NULL, NULL, NULL, NULL, NULL, NULL );
			SAFE_RELEASE( pSwapChainTexture );
		}
		else if (g_TestApp.ViewResources()) {
			WindowsTest::CHandle<ID3D11Texture2D> pTexture;
			D3D11_TEXTURE2D_DESC Tex2DDesc = {};
			Tex2DDesc.Width = g_TestApp.m_RenderTargetOptions.SwapChainDesc.Width;
			Tex2DDesc.Height = g_TestApp.m_RenderTargetOptions.SwapChainDesc.Height;
			Tex2DDesc.Format = g_TestApp.m_RenderTargetOptions.SwapChainDesc.Format;
			Tex2DDesc.SampleDesc = g_TestApp.m_RenderTargetOptions.SwapChainDesc.SampleDesc;
			Tex2DDesc.MipLevels = 1;
			Tex2DDesc.ArraySize = 1;
			Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
			Tex2DDesc.BindFlags =
				((g_TestApp.m_RenderTargetOptions.SwapChainDesc.BufferUsage & DXGI_USAGE_RENDER_TARGET_OUTPUT) ? D3D11_BIND_RENDER_TARGET : 0) |
				((g_TestApp.m_RenderTargetOptions.SwapChainDesc.BufferUsage & DXGI_USAGE_SHADER_INPUT) ? D3D11_BIND_SHADER_RESOURCE : 0) |
				((g_TestApp.m_RenderTargetOptions.SwapChainDesc.BufferUsage & DXGI_USAGE_UNORDERED_ACCESS) ? D3D11_BIND_UNORDERED_ACCESS : 0);
			Tex2DDesc.MiscFlags = 0;
			Tex2DDesc.CPUAccessFlags = 0;
	
			if ( FAILED( hr = m_pDevice->CreateTexture2D( &Tex2DDesc, nullptr, &pTexture ) ) )
				throw TRException("[CMultisampleTest::Present] CreateTexture2D failed.", hr, RESULT_FAIL);	
			GetEffectiveContext()->CopyResource(pTexture, m_pRTTexture2D);
			
			if ( FAILED( hr = ExecuteEffectiveContext() ) )
				throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
			ShowResource(L"Multisample Present", pTexture);
		}
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[FillLine::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	tRes = Validate();  // verify whether the rendered image is the same as the one we expected

	return tRes;	
}


TEST_RESULT FillLine::Validate() 
{ 
	// read out the rendered buffer
	vector<D3DXVECTOR4> vBuffer(RT_WIDTH * RT_HEIGHT);
	MSHelper::Read(GetFramework(), GetFramework()->GetRTTexture(), (FLOAT*)&vBuffer[0], m_RTVFormat);

	int prIndex;
	if (isLineStrip(line)) {
		prIndex = (int) lineStrips[rsxLS.Get()->Value() * 7 + 6];  // read out the index for the pr vector from the data array
	}
	else {
		prIndex = (int) lines[rsx.Get()->Value() * 5 + 4];
	}

	PixelResult *expected = &pr[prIndex]; // the expected result

	TEST_RESULT tRes = RESULT_PASS;
	float diff; // the difference between expected and the actual value
	float maxdiff = 0.05f; // set the tolenrace to be big here
	// now compare the result
	for (int i = 0; i < RT_HEIGHT * RT_WIDTH; i++) 
	{
		// only look at the red component of the pixel assuming the other color components were all correct
		diff = vBuffer[i].x - expected->getPixelValue(i);
		if ( abs(diff) > maxdiff )  // if we are not within the tolerance
		{
			tRes = RESULT_FAIL;  // for each incorrect pixel result, log the information
			WriteToLog("At location x : %d, y : %d,", i % RT_WIDTH, i / RT_WIDTH); 
			if (expected->getPixelValue(i) == 0.25f) {
				WriteToLog("pixel should be lit once!");
			}
			if (expected->getPixelValue(i) == 0.5f) {
				WriteToLog("pixel should be lit twice!");
			}
			if (expected->getPixelValue(i) == 0.0f) {
				WriteToLog("pixel should not be lit!");
			}
		}
	}

	return tRes; 
}


void FillLine::CleanupTestCase() 
{
	SAFE_RELEASE( blendState );
	CMultisampleTest_Fill::CleanupTestCase();
}
