//////////////////////////////////////////////////////////////////////
// File:  DXGIPresentFlags.h
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for DXGI present flag tests.
//
// History:
// ?? ??? ????  AndrewLu  Created.
// 05 Oct 2007  SEdmison  Rewrote, cleaned up, and added feature level
//                        support.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#pragma once

#include <d3d10_1.h>
#include <d3dx10.h>

#include <DXGIPresent.h>

//
// Project headers
//

#include "TestApp.h"


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////
class CFSSequential
:
	public CPresentFS
{
	//
	// Construction/destruction
	//

	public:
		CFSSequential();

		virtual ~CFSSequential();


	//
	// Framework override methods
	//

	public:
		virtual void InitTestParameters();
        virtual RFactor IntitTestRFactors();

};

class CFSDiscard
:
	public CPresentFS
{
	//
	// Construction/destruction
	//

	public:
		CFSDiscard();

		virtual ~CFSDiscard();


	//
	// Framework override methods
	//

	public:
		virtual void InitTestParameters();
        virtual RFactor IntitTestRFactors();
};

class CWinSequential
:
	public CPresentWinConvertStretch
{
	//
	// Construction/destruction
	//

	public:
		CWinSequential();

		virtual ~CWinSequential();


	//
	// Framework override methods
	//

	public:
		void InitTestParametersImpl();
};

class CWinDiscard
:
	public CPresentWinConvertStretch
{
	//
	// Construction/destruction
	//

	public:
		CWinDiscard();

		virtual ~CWinDiscard();


	//
	// Framework override methods
	//

	public:
		void InitTestParametersImpl();
};

