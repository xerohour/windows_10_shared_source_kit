#pragma once

#include "RectScenarios.h"

typedef enum { eRedFractional, eGreenIntegral, eGreenOutofRange, eRedInf, eGreenNaN } eColorScenario;
typedef enum { eRTV, eUAV, eVPV, eVDV, eVIV } eViewType;

extern SRectScenarios RectScenariosMip0[];
extern SRectScenarios RectScenariosMip1[];

template< UINT T >
class CClearViewTest : public CD3D11Test
{
public:
	// Constructor/Destructor
	CClearViewTest(void)
	{
	}

	~CClearViewTest(void)
	{
	}

	// Test functions
    inline void InitTestParameters( );
	inline virtual TEST_RESULT Setup();
	inline virtual TEST_RESULT ExecuteTestCase();
	inline virtual void Cleanup();

protected:
	testfactor::RFactor GetFormatsAndMSAA(const DXGI_FORMAT* MSAACheckFormats, const UINT &TSampleCount, CTestCaseParameter<DXGI_FORMAT> *pFormatParam, CTestCaseParameter<UINT> *pMSAAQualityParam);
	float GetExpectedComponentValue( const float ClampedClearColorData[4], const float ClampedClearColorChrominanceData[4], const DXGI_FORMAT &resourceFormat, const DXGI_FORMAT &viewFormat, const DXGI_FORMAT &viewFormatChrominanceData, UINT pixelX, UINT pixelY, UINT component );
    void WriteColorFailure( const TCHAR * msg, int x, int y, const float ClampedColor[4], FLOAT_COLOR & ReformattedResult );
    void WriteYUVTransform( const TCHAR * msg, const float ClampedColor[4] );
	bool ConvertAndClampClearColorToFormat( const float ClearColor[4], const DXGI_FORMAT &colorConvertFormat, float ConvertedAndClampedClearColor[4] );
	std::vector<RECT> GetViewSpaceRects( const RECT* const pRects, const UINT &viewSpaceRectCount );
	std::vector<RECT> GetModifiedRects( const RECT* const pRects, const UINT &numRects, const float &xScale, const float &yScale );
	void WriteRects( const std::vector<RECT> &rects );
	void WriteRects( const RECT* pRects, const UINT &numRects );

protected:
    ATL::CComPtr< ID3D11VideoDevice >       m_pVideoDevice;
    ATL::CComPtr< ID3D11VideoContext >      m_pVideoContext;
    ATL::CComPtr< ID3D11VideoProcessorEnumerator >      m_pVideoEnum;
    D3D_RESOURCE_SCENARIO   m_ResourceScenario;
    UINT                    m_MultisampleQuality;
    UINT                    m_MipLevels;
    UINT                    m_ArraySize;
    DXGI_FORMAT             m_Format;
    UINT                    m_MipSlice;
    UINT                    m_FirstArraySlice;
    UINT                    m_ViewArraySize;
    ATL::CAutoPtr< FLOAT_COLOR > m_Scratch;
    ATL::CAutoPtr< TCHAR >       m_TextView;

    eRectScenario           m_RectScenario;
    eColorScenario          m_ColorScenario;
    eViewType               m_ViewType;
	bool					m_bSkipVideoViewTestCases;
	bool					m_bIsYUV;
	bool					m_bIsYUVSubsampled;
	bool					m_bVideoView;
	bool					m_bViewHasFormat;
	D3D11_RECT				m_rects[50];
};

DECLARE_NAMED_VALUES( eColorScenario );
DECLARE_NAMED_VALUES( eViewType );


template<UINT T>
inline void CClearViewTest<T>::Cleanup()
{
    m_pVideoDevice = 0;
    m_pVideoContext = 0;
    m_pVideoEnum = 0;
    m_Scratch.Free();
    m_TextView.Free();
}


class CTosser
{
public:
    CTosser( TEST_RESULT tr ) : m_tr (tr) {}
    TEST_RESULT m_tr;
};
