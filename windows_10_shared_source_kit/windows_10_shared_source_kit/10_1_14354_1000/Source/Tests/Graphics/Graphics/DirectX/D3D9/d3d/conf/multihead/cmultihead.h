////////////////////////////////////////////////////////////////////////////////
// File:  CMultiHead.h
//
// Purpose:
// Tests for D3D multi-head device functionality.
//
// History:
// 21 Nov 2001  BruceN    Created.
// 07 Apr 2006  SEdmison  
////////////////////////////////////////////////////////////////////////////////

#ifndef __CMULTIHEAD_H__
#define __CMULTIHEAD_H__


///////////////////////////////////////////////////////////////////////////
//
// Includes
//
///////////////////////////////////////////////////////////////////////////

#include <CD3DTest.h>
#include "Utility.h"


///////////////////////////////////////////////////////////////////////////
//
// Interfaces
//
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Class:  CMultiHead
//
// Purpose:
// Base class for multi-head test groups.
//////////////////////////////////////////////////////////////////////

class CMultiHead
:
	public CD3DTest
{
	public:
		CMultiHead();
		template<class T>
		bool verifyEQ( const T &a, const T &b, const TCHAR *ptcsFmt, ... )
		{
			if ( a == b)
				return true;

			va_list	argList;
			va_start(argList, ptcsFmt);
			writeToLog(ptcsFmt, argList);
			Fail();
			return false;
		}
		template<class T>
		bool verifyNE( const T &a, const T &b, const TCHAR *ptcsFmt, ... )
		{
			if ( a != b)
				return true;

			va_list	argList;
			va_start(argList, ptcsFmt);
			writeToLog(ptcsFmt, argList);
			Fail();
			return false;
		}
		virtual bool	CapsCheck();
		virtual UINT    TestInitialize(void);
		virtual HRESULT CreatePresentParams(TESTMODE *pMode, PRESENT_PARAMETERS **ppParams);
		bool CompareMode(PDISPLAYMODE pmode1, PDISPLAYMODE pmode2);
		bool IsModeSupportedByAllAdaptersInGroup(PDISPLAYMODE pMode, CDevice *pDevice);
		virtual bool FilterResolution(PDISPLAYMODE pMode);

	protected:
		UINT m_nParams;
		UINT m_iSwapChain;
		std::vector< UINT > m_Adapters;
		virtual bool CleanupTest(void);
		virtual bool GetDepthStencil( UINT iSwapChain, LPSURFACES *pDepthStencil );


	private:
		void writeToLog(const TCHAR *ptcsFmt, va_list &argList)
		{
			TCHAR *sTemp = GetVarArgString(ptcsFmt, argList);
			va_end(argList);
			m_pFramework->WriteToLog(1, sTemp);
			SAFEDELETEA(sTemp);
		}
		CPtrList<CWindow>* m_MyWindows;
};


#endif //__CMULTIHEAD_H__
