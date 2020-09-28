#pragma once


#if !defined(PASTER2)
#define PASTER2(x,y) x ## y
#endif

#if !defined(PASTER)
#define PASTER(x,y) PASTER2(x,y)
#endif

#if !defined(UNICODIFY)
#define UNICODIFY( x ) PASTER(L,x)
#endif

#if !defined(__FILEW__)
#define __FILEW__ UNICODIFY(__FILE__)
#endif

#define StdPointSet std::set<POINT,bool(*)(POINT,POINT)>

// Includes
#include "ResourceAccess.h"
#include "RectScenarios.h"

#define USE_GUARD_RECT_SCENARIO 0

#if USE_GUARD_RECT_SCENARIO
#include "d3d11partner.h" // Needed for GuardRect private APIs
#endif

// Failure Categories
using namespace WindowsTest;
using namespace WindowsTest::Graphics::Direct3D;

////////////////////////

class CDiscardTest : public CResourceAccessTest
{
public:
	// Constructor/Destructor
	CDiscardTest(void)
		: m_pDevice1(NULL)
#if USE_GUARD_RECT_SCENARIO
		, m_pPartnerDevice(NULL)
#endif
		, m_pEffectiveContext1(NULL)
		, m_pResourceToDiscard(NULL)
		, m_pDummyResourceNoDiscard(NULL)
		, m_pStagingResource(NULL)
		, m_pSubresourceData(NULL)
		, m_bTestUsesView(false)
#if USE_GUARD_RECT_SCENARIO
		, m_bTestUsesGuardRect(false)
		, m_bInGuardRectangleSupport(false)
#endif
		, m_uMipLevels(RA_MIPLEVELS)
	{
		GetFramework()->m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;
	}

	~CDiscardTest(void)
	{
	}

	// Test functions
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();
	virtual void Cleanup();

	virtual bool Discard() = 0;				// This is where the test calls any discard APIs it needs
	virtual bool VerifyResults() const = 0;	// This is where the test checks the recorded DDI information to see if the test passed

protected:
	void GenerateCommonResourceParameterFactors();
	void GenerateCommonResourceFactorCombinations();
	bool GetArraySizeAndMipLevelsFromResource( ID3D11Resource *pResource, UINT &arraySize, UINT &mipLevels ) const;
	bool GetAllSubresources( std::vector<UINT> &subresources, ID3D11Resource *pResource ) const;
	void SnapTexelToMipDimensions( POINT &texel, const UINT &mipWidth, const UINT &mipHeight ) const;
	virtual bool CreateSubresourceData();
	virtual TEST_RESULT CreateResources();
	virtual bool CreateViews();
	virtual bool BufferViewHasElementOffset() const { return false; };	// No view by default
	virtual bool SRViewIncludesLowestLOD() const { return true; };		// Yes by default
	virtual bool UpdateSubresourceData();
	virtual bool VerifySubresourceData( ID3D11Resource *pResourceToVerify, const std::vector<UINT> &subresourcesToVerify ) const;

protected:

	//
	// Member variables
	//

	ID3D11Device1 *m_pDevice1;
#if USE_GUARD_RECT_SCENARIO
	ID3D11PartnerDevice *m_pPartnerDevice;					// Used for GuardRects
#endif
	ID3D11DeviceContext1 *m_pEffectiveContext1;
	ID3D11Resource *m_pResourceToDiscard;					// This is the resource to be discarded
	ID3D11Resource *m_pDummyResourceNoDiscard;				// This is a dummy resource to ensure the runtime doesn't discard the wrong resource
	ID3D11Resource *m_pStagingResource;						// This is the resource to map
	BYTE m_ResourceBitPattern;
	BYTE *m_pSubresourceData;
	bool m_bTestUsesView;									// If the test uses a view, set this flag
#if USE_GUARD_RECT_SCENARIO
	bool m_bTestUsesGuardRect;								// If the test uses a guard rect, set this flag
	bool m_bInGuardRectangleSupport;	
#endif

	//
	// Test factors
	//

	std::vector<D3D11_SUBRESOURCE_DATA> m_subresourceData;
	D3D11_USAGE m_Usage;
	UINT m_CPUAccessFlag;									// D3D11_CPU_ACCESS_FLAG
	D3D11_RESOURCE_MISC_FLAG m_ResourceMiscFlag;
	UINT m_uSampleCount;
	UINT m_uSampleQuality;
	UINT m_uMipLevels;
	bool m_bDoubleDiscard;									// The flag to indicate if the discard API should be called twice in a row
	bool m_bInitResourceData;								// The flag to indicate if the resource should be created with initial data or if it should be mapped and copied

#if USE_GUARD_RECT_SCENARIO
	eRectScenario m_guardRectScenario;
#endif
	// Factors
	testfactor::RFactor m_rResourceScenarioBuffer, 
		m_rResourceScenarioTex1D, m_rResourceScenarioTex1DArray,
		m_rResourceScenarioTex2D, m_rResourceScenarioTex2DArray, m_rResourceScenarioTex2DMS, m_rResourceScenarioTex2DMSArray,
		m_rResourceScenarioTex3D,
		m_rResourceScenarioTexCube;

	testfactor::RFactor m_rGuardRectScenario;
	testfactor::RFactor m_rResourceFormats, m_rResourceDSFormats, m_rResourceDepthFormats, m_rResourceStencilFormats, m_rResourceBCFormats, m_rResourceVideoFormats;

	testfactor::RFactor m_rResourceUsageDefault, m_rResourceUsageDynamic;

	testfactor::RFactor m_rCPUAccessFlagNone, m_rCPUAccessFlagWrite, m_rCPUAccessFlags;	// D3D11_CPU_ACCESS_READ is not used here because it can only be done for staging resources

	testfactor::RFactor m_rDoubleDiscard;

	testfactor::RFactor m_rResourceArraySize, m_rResourceArraySize_1, m_rResourceArraySize_2, m_rResourceArraySize_6;
	testfactor::RFactor m_rResourceNumMipLevels, m_rResourceNumMipLevels_0, m_rResourceNumMipLevels_1, m_rResourceNumMipLevels_2, m_rResourceNumMipLevels_3;
	testfactor::RFactor m_rResourceBindFlagDS, m_rResourceBindFlagSR, m_rResourceBindFlagRT, m_rResourceBindFlagUA, m_rResourceBindFlagCB, m_rResourceBindFlagSRRT;
	testfactor::RFactor m_rResourceWidth, m_rResourceWidth_1, m_rResourceWidth_16, m_rResourceWidth_64;
	testfactor::RFactor m_rResourceHeight, m_rResourceHeight_1, m_rResourceHeight_16, m_rResourceHeight_64;
	testfactor::RFactor m_rResourceDepth, m_rResourceDepth_1, m_rResourceDepth_16, m_rResourceDepth_64;

	testfactor::RFactor m_rResourceSampleCount_1, m_rResourceSampleCount_2;
	testfactor::RFactor m_rResourceSampleQuality_0;

	testfactor::RFactor m_rResourceMiscFlag_TexCube;

	testfactor::RFactor m_rInitResourceData, m_rInitResourceData_True, m_rInitResourceData_False;

	// Common test factor combinations
	testfactor::RFactor m_rBindFlags_SRRTUA;
	testfactor::RFactor m_rBindFlags_SRRT;

	testfactor::RFactor m_rDefaultUsageCPU_Buf;
	testfactor::RFactor m_rDefaultUsageCPUAll_Buf;
	testfactor::RFactor m_rDefaultUsageMipCPU_Tex;
	testfactor::RFactor m_rDefaultUsageArrayMipCPU_Tex;
	testfactor::RFactor m_rDefaultUsageArrayCPU_Tex;
	testfactor::RFactor m_rDefaultUsageCPU_Tex;
	testfactor::RFactor m_rDefaultUsageMipCPU_Cube;

	testfactor::RFactor m_rDynamicUsageCPU_Buf;
	testfactor::RFactor m_rDynamicUsageArrayMipCPU_Tex;
};

class CDiscardResourceTest : public CDiscardTest
{
public:
	// Constructor/Destructor
	CDiscardResourceTest(void)
		: CDiscardTest()
	{
#if USE_GUARD_RECT_SCENARIO
		// Need to indicate we use a guard rect
		m_bTestUsesGuardRect = true;
#endif
	}

	~CDiscardResourceTest(void)
	{
	}

	void InitTestParameters();
	bool Discard();
	bool VerifyResults() const;

protected:
	bool GetTexelsToCheck( StdPointSet &texelsToCheck, const UINT &mipSlice ) const;
	virtual bool VerifySubresourceDataAgainstGuardRect( ID3D11Resource *pResourceToVerify, const std::vector<UINT> &subresourcesToVerify ) const;
	RECT m_guardRect;
	bool m_bGuardRectWasSet;
};

class CDiscardViewTest : public CDiscardTest
{
public:
	// Constructor/Destructor
	CDiscardViewTest(void)
		: CDiscardTest()
		, m_pResourceViewToDiscard(NULL)
		, m_pDummyResourceViewNoDiscard(NULL)
	{
		// All view tests need to indicate a view is being used
		m_bTestUsesView = true;
	}

	~CDiscardViewTest(void)
	{
	}

	virtual bool Discard();
	virtual bool CreateViews() = 0;
	virtual bool VerifyResults() const;
	virtual void CleanupTestCase();

protected:
	void GenerateCommonViewParameterFactors();
	void GenerateCommonViewFactorCombinations();
	virtual bool GetSubresourcesInsideView( std::vector<UINT> &subresourcesInsideView, ID3D11Resource *pResource ) const;
	virtual bool GetSubresourcesOutsideView( std::vector<UINT> &subresourcesOutsideView, ID3D11Resource *pResource ) const;

	// Some views can partially cover a single subresource: Buffers, RTV Tex3D, etc...these need to be verified that everything outside the view is intact
	// Default code here is to handle buffer's and texture 3d's. This should be overridden on a per test basis
	virtual bool GetSubresourcesPartiallyCoveredByView( std::vector<UINT> &subresourcesPartiallyCoveredByView, ID3D11Resource *pResource ) const;
	virtual bool VerifyPartialSubresourceData( ID3D11Resource *pResourceToVerify, const std::vector<UINT> &subresourcesPartiallyCoveredByView ) const;

protected:

	//
	// Member variables
	//

	ID3D11View *m_pResourceViewToDiscard;			// This is the resource to be discarded
	ID3D11View *m_pDummyResourceViewNoDiscard;		// This is a dummy resource to ensure the runtime doesn't discard the wrong resource
	bool m_bBindView;								// The flag to indicate if the resource view should be bound to the pipeline
	UINT m_uViewMipSlice;
	UINT m_uViewFirstArraySlice;
	UINT m_uViewArraySize;

	UINT m_uViewElementOffset;	// Same as "FirstElement"
	UINT m_uViewElementWidth;	// Same as "NumElements"

	UINT m_uViewFirstWSlice;
	UINT m_uViewWSize;

	UINT m_uViewMostDetailedMip;
	UINT m_uViewMipLevels;

	//
	// Factors
	//

	testfactor::RFactor m_rMipSlice0;
	testfactor::RFactor m_rMipSlice1;
	testfactor::RFactor m_rMipSlice2;
	testfactor::RFactor m_rMipSlice01;
	testfactor::RFactor m_rMipSlice012;

	testfactor::RFactor m_rFirstArraySlice0;
	testfactor::RFactor m_rFirstArraySlice1;
	testfactor::RFactor m_rFirstArraySlice01;

	testfactor::RFactor m_rArraySize1;
	testfactor::RFactor m_rArraySize2;
	testfactor::RFactor m_rArraySize3;
	testfactor::RFactor m_rArraySize12;	// Array size 1 and 2

	testfactor::RFactor m_rMostDetailedMip0;
	testfactor::RFactor m_rMostDetailedMip1;
	testfactor::RFactor m_rMostDetailedMip2;
	testfactor::RFactor m_rMostDetailedMip012;

	testfactor::RFactor m_rMipLevels1;
	testfactor::RFactor m_rMipLevels2;
	testfactor::RFactor m_rMipLevels3;
	testfactor::RFactor m_rMipLevels123;

	testfactor::RFactor m_rElementOffset0;
	testfactor::RFactor m_rElementOffset1;
	testfactor::RFactor m_rElementOffset01;

	testfactor::RFactor m_rElementWidth1;
	testfactor::RFactor m_rElementWidth15;
	testfactor::RFactor m_rElementWidth16;
	testfactor::RFactor m_rElementWidth63;
	testfactor::RFactor m_rElementWidth64;

	testfactor::RFactor m_rViewFirstWSlice_0;
	testfactor::RFactor m_rViewFirstWSlice_1;
	testfactor::RFactor m_rViewFirstWSlice_01;

	testfactor::RFactor m_rViewWSize_1;
	testfactor::RFactor m_rViewWSize_3;
	testfactor::RFactor m_rViewWSize_6;
	testfactor::RFactor m_rViewWSize_8;
	testfactor::RFactor m_rViewWSize_15;
	testfactor::RFactor m_rViewWSize_16;

	testfactor::RFactor m_rResourceViewMipSlice_1;	
	testfactor::RFactor m_rResourceViewMipSlice_3;
	testfactor::RFactor m_rResourceViewMipSlice;

	testfactor::RFactor m_rResourceViewArraySlice_1;
	testfactor::RFactor m_rResourceViewArraySlice_2;
	testfactor::RFactor m_rResourceViewArraySlice_6;
	testfactor::RFactor m_rResourceViewArraySlice;

	testfactor::RFactor m_ResourceViewMostDetailedMipMipLevels_1_0_1;
	testfactor::RFactor m_ResourceViewMostDetailedMipMipLevels_3_1_1;
	testfactor::RFactor m_ResourceViewMostDetailedMipMipLevels_3_2_1;
	testfactor::RFactor m_ResourceViewMostDetailedMipMipLevels_3_0_123;
	testfactor::RFactor m_ResourceViewMostDetailedMipMipLevels;

	testfactor::RFactor m_rResourceViewWidthElementOffsetElementWidth, m_rResourceViewWidth64ElementOffsetElementWidth;
	testfactor::RFactor m_rResourceViewDepthFirstWSliceWSizeMipSlice;
};

class CDiscardView1Test : public CDiscardViewTest
{
public:
	// Constructor/Destructor
	CDiscardView1Test(void)
		: CDiscardViewTest()
	{
	}

	~CDiscardView1Test(void)
	{
	}

	virtual bool Discard();
	virtual bool VerifyResults() const;

protected:
	void GenerateCommonView1ParameterFactors();
	virtual bool GetTexelsToCheck( StdPointSet &texelsToCheck ) const;
	virtual bool VerifySubresourceDataAgainstRects( ID3D11Resource *pResourceToVerify, const std::vector<UINT> &subresourcesToVerify ) const;
	void WriteRectScenarioToLog( const SRectScenarios &rectScenario ) const;

	//
	// Member variables
	//

	SRectScenarios *m_pRectList;
	eRectScenario m_rectScenario;

	//
	// Factors
	//

	testfactor::RFactor m_rfRectScenario;
};

class CDiscardDSView1Test : public CDiscardView1Test
{
public:
	void InitTestParameters();
	bool CreateViews();

	// DSV's never have partially covered subresources
	bool GetSubresourcesPartiallyCoveredByView( std::vector<UINT> &subresourcesPartiallyCoveredByView, ID3D11Resource *pResource ) const { subresourcesPartiallyCoveredByView.clear(); return true; };
	bool VerifyPartialSubresourceData( ID3D11Resource *pResourceToVerify, const std::vector<UINT> &subresourcesPartiallyCoveredByView ) const { return true; };

protected:
	D3D11_DSV_FLAG m_DSVFlags;

	testfactor::RFactor m_rResourceViewDSVFlags, m_rResourceViewDSVFlags_None, m_rResourceViewDSVFlags_RODepth, m_rResourceViewDSVFlags_ROStencil;
};

class CDiscardRTView1Test : public CDiscardView1Test
{
public:
	void InitTestParameters();
	bool CreateViews();
};

class CDiscardDSViewTest : public CDiscardViewTest
{
public:
	void InitTestParameters();
	bool CreateViews();

	// DSV's never have partially covered subresources
	bool GetSubresourcesPartiallyCoveredByView( std::vector<UINT> &subresourcesPartiallyCoveredByView, ID3D11Resource *pResource ) const { subresourcesPartiallyCoveredByView.clear(); return true; };
	bool VerifyPartialSubresourceData( ID3D11Resource *pResourceToVerify, const std::vector<UINT> &subresourcesPartiallyCoveredByView ) const { return true; };

protected:
	D3D11_DSV_FLAG m_DSVFlags;

	testfactor::RFactor m_rResourceViewDSVFlags, m_rResourceViewDSVFlags_None, m_rResourceViewDSVFlags_RODepth, m_rResourceViewDSVFlags_ROStencil;
};

class CDiscardRTViewTest : public CDiscardViewTest
{
public:
	void InitTestParameters();
	bool CreateViews();
};

class CDiscardSRViewTest : public CDiscardViewTest
{
public:
	void InitTestParameters();
	bool CreateViews();
	bool SRViewIncludesLowestLOD() const { return ((m_uViewMipLevels == -1) || ((m_uViewMostDetailedMip + m_uViewMipLevels) == m_uMipLevels)); };

	// SRV's use different variable 
	bool GetSubresourcesInsideView( std::vector<UINT> &subresourcesInsideView, ID3D11Resource *pResource ) const;

	// The default implementation looks at buffers and texture3Ds. However, SRVs can only partially cover a subresource on buffers
	bool GetSubresourcesPartiallyCoveredByView( std::vector<UINT> &subresourcesPartiallyCoveredByView, ID3D11Resource *pResource ) const;
};

class CDiscardUAViewTest : public CDiscardViewTest
{
public:
	void InitTestParameters();
	bool CreateViews();
	bool BufferViewHasElementOffset() const { return m_uViewElementOffset > 0; }; 
};