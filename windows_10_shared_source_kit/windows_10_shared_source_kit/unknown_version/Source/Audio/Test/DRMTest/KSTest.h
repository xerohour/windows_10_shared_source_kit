//+-------------------------------------------------------------------------
//
//
// Copyright (C) Microsoft. All rights reserved.
//
//  File:       KSTest.h
//
//--------------------------------------------------------------------------
#pragma once

// This is used to determine how long to wait in streaming cases
//   - For Non-RT cases this is not an issue, since streaming completes when
//     the IRP completes and an event gets set
//   - For RT cases, there are no IRPs to complete, and the data(in capture
//     case) gets transferred (the pointer get assigned) at the end of the
//     capture cycle.
#define STREAMING_TOLERANCE 1.2

#ifndef TEST_FREQUENCY
#define TEST_FREQUENCY 1000
#endif

// ----------------------------------------------------------------------------------
// CKsTest
// ----------------------------------------------------------------------------------
class CKsTest
{
public:
    // Constructor / destructor
    CKsTest();
    ~CKsTest();
    
    BOOL    Initialize(CPCMAudioFilter* pFilter);    

    BOOL    CreateRender(DWORD dwStreamTime);   // Creates the Render Pin    

    BOOL    DestroyRender();                    // Cleans up the DirectSound object

private:
    CPCMAudioFilter    *m_pFilter;
    DWORD               m_dwStreamTime; //StreamTime in secs
};
