#pragma once


//
// Win32 headers
//

#include <windows.h>
#include <dxgipresent.h>

class CSEWinSequential
:
	public CPresentWinConvertStretch
{
	//
	// Construction/destruction
	//

	public:
		CSEWinSequential();

		virtual ~CSEWinSequential();


	//
	// Framework override methods
	//

	public:
		virtual void InitTestParametersImpl();
        virtual TEST_RESULT ExecuteTestCase();
        bool m_ValidateNoExtraBuffers;


};

class CFSWinTransitionSequential
:
	public CSEWinSequential
{
	//
	// Construction/destruction
	//

	public:
		CFSWinTransitionSequential();

		virtual ~CFSWinTransitionSequential();


	//
	// Framework override methods
	//

	public:
        virtual TEST_RESULT ExecuteTestCase();
        void InitTestParametersImpl();
        virtual HRESULT TransitionAfterPresent();
        virtual HRESULT TransitionBeforePresent();
        virtual TEST_RESULT Setup();

    public:
        bool m_HoldReferenceToPrimary;
        bool m_ResizeBuffers;



};

class CSEFSSequential
:
	public CPresentFS
{
	//
	// Construction/destruction
	//

	public:
		CSEFSSequential();

		virtual ~CSEFSSequential();


	//
	// Framework override methods
	//

	public:
		virtual void InitTestParameters();
        virtual TEST_RESULT ExecuteTestCase();
        virtual RFactor IntitTestRFactors();
        bool m_ValidateNoExtraBuffers;
};