/*
 *  CMultiHeadConf.h
 *
 *  Test DxD3D Multi-head functionality
 *
 *  Author: Bruce Nelson    (brucen)    11/21/2001
 */

#ifndef __CMULTIHEADCONF_H__
#define __CMULTIHEADCONF_H__

#include "CMultiHead.h"

enum TEST_RESULT
{
	RESULT_PASS,
	RESULT_CONTINUE,
	RESULT_SKIP,
	RESULT_ABORT,
	RESULT_FAIL
};


// normal configuration swapeffect flip, autodepthstencil
class CMultiHeadConf
:
	public CMultiHead
{
	public:
		CMultiHeadConf();

		virtual UINT    TestInitialize();
		virtual bool    ExecuteTest(UINT);

	private:
		int m_nSides;
		int m_nVertices;
		float m_fStep;
		bool m_bDifferent;
		bool m_bPresent;
		bool m_bDrawPrimitive;
		std::vector<D3DXVECTOR2> m_shape1, m_shape2;

		TEST_RESULT DoMultiHeadTest();
};

// swapeffect copy
class CMultiHeadConfCopy
:
	public CMultiHeadConf
{
	public:
		CMultiHeadConfCopy()
		{
			m_szTestName = _T("Multi-Head Conformance Copy Test");
			m_szCommandKey = _T("CMultiHeadConfCopy");
			m_Options.D3DOptions.dwSwapEffect = SWAPEFFECT_COPY;
		}
};

// max backbuffer count (1 or 3)
class CMultiHeadConfBBCount
:
	public CMultiHeadConf
{
	public:
		CMultiHeadConfBBCount()
		{
			m_szTestName = _T("Multi-Head Conformance Backbuffer Count Test");
			m_szCommandKey = _T("CMultiHeadConfBBCount");
		}
		virtual HRESULT CreatePresentParams(TESTMODE *pMode, PRESENT_PARAMETERS **ppParams);
};


// backbuffer lockable
class CMultiHeadConfBBLock
:
	public CMultiHeadConf
{
	public:
		CMultiHeadConfBBLock()
		{
			m_szTestName = _T("Multi-Head Conformance Backbuffer Lockable Test");
			m_szCommandKey = _T("CMultiHeadConfBBLock");
		}
		virtual HRESULT CreatePresentParams(TESTMODE *pMode, PRESENT_PARAMETERS **ppParams);
};

// no z-buffer
class CMultiHeadConfNoZ
:
	public CMultiHeadConf
{
	public:
		CMultiHeadConfNoZ()
		{
    		m_szTestName = _T("Multi-Head Conformance No Z Test");
    		m_szCommandKey = _T("CMultiHeadConfNoZ");
			m_Options.D3DOptions.bZBuffer = false;
		}
};

// diff Presentation Intervals
class CMultiHeadConfDiffIntvl
:
	public CMultiHeadConf
{
	public:
		CMultiHeadConfDiffIntvl()
		{
			m_szTestName = _T("Multi-Head Conformance Presentation Interval Test");
			m_szCommandKey = _T("CMultiHeadConfDiffIntvl");
		}
		virtual HRESULT CreatePresentParams(TESTMODE *pMode, PRESENT_PARAMETERS **ppParams);
};

// diff Refresh Rates
class CMultiHeadConfDiffRefrsh
:
	public CMultiHeadConf
{
	public:
		CMultiHeadConfDiffRefrsh()
		{
			m_szTestName = _T("Multi-Head Conformance Refresh Rate Test");
			m_szCommandKey = _T("CMultiHeadConfDiffRefrsh");
		}
		virtual bool DrawStats(int nHeight, int nWidth, CDevice *pDevice);
		virtual HRESULT CreatePresentParams(TESTMODE *pMode, PRESENT_PARAMETERS **ppParams);
};

// no z-buffer with different mode configurations on each head
class CMultiHeadConfNoZDiffModes
:
	public CMultiHeadConfNoZ
{
	public:
		CMultiHeadConfNoZDiffModes()
		{
    		m_szTestName = _T("Multi-Head Conformance No Z Different Modes Test");
    		m_szCommandKey = _T("CMultiHeadConfNoZDiffModes");
		}
		virtual bool DrawStats(int nHeight, int nWidth, CDevice *pDevice);
		virtual HRESULT CreatePresentParams(TESTMODE *pMode, PRESENT_PARAMETERS **ppParams);
};

// no implicit z-buffer but will create one for each swapchain
class CMultiHeadConfNoZDiffModesDB
:
	public CMultiHeadConfNoZDiffModes
{
	public:
		CMultiHeadConfNoZDiffModesDB()
		{
    		m_szTestName = _T("Multi-Head Conformance No Z Different Modes Depth Stencil Test");
    		m_szCommandKey = _T("CMultiHeadConfNoZDiffModesDB");
		}
	protected:
		virtual bool GetDepthStencil( UINT iSwapChain, LPSURFACES *pDepthStencil );
};

// test cursor positioning
class CMultiHeadCursor
:
	public CMultiHead
{
	public:
		CMultiHeadCursor();

		virtual UINT    TestInitialize(void);
		virtual bool    ExecuteTest(UINT);
};

#endif //__CMULTIHEADCONF_H__
