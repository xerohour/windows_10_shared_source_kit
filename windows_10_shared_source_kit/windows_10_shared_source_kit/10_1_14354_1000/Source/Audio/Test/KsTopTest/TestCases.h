// -----------------------------------------------------------------------------
// File:    TestCases.h
//
// Desc:    This file contains the definition of the classes
//          that handle the testing of the KS Topology Device API.
//          There are two main classes that do the actual testing:
//          1. CNodeReq (checks for node requirements)
//          2. CPropDescReq (checks for property description requiremenst)
//
//          Both these classes inheret from CKSTopoDevLoop, which is a
//          class that contains code for looping Filters; Filters, Nodes;
//          Filters, Nodes, Connections and running a specific function to 
//          act on the objects.
//
//          The CNodeReq and CPropDescReq have interface functions that 
//          usually just call one of the looping functions and pass as an 
//          argument a function pointer to a static private member function.
//          In order to be able to see the member variables, a 'this'
//          pointer is passed to the private static functions.
//
//          IMPORTANT: These classes are NOT THREAD-SAFE on one object!!
//                     They can be run in parallel with different objects.
//
//
// Copyright (C) Microsoft Corporation. All rights reserved.
// -----------------------------------------------------------------------------

#pragma once


// These are defined constants which at the time of writing this test
// are used to limit values in sanity checks. If in the future these 
// limits change, updating them here should be enough.
const UINT  MAX_PROPERTY_DESCRIPTION_SIZE = 102400; // 100 KB
const UINT  MINIMUM_SAMPLE_FREQUENCY = 1;           //   1 Hz i.e not 0
const UINT  MAXIMUM_SAMPLE_FREQUENCY = 384000;      // 384 KHz
const UINT  MINIMUM_BITS_PER_SAMPLE = 8;            //   8 bits
const UINT  MAXIMUM_BITS_PER_SAMPLE = 32;           //  32 bits
const UINT  MAXIMUM_NUMBER_OF_CHANNELS = 2048;      // 2048 channels at most
const UINT  MAX_BUFFER_SIZE = 300;                  // Maximum buffer size
                                                    // for error messages

typedef bool (*FnCheck)(void *);

class CKSTopoDevLoop
{
public:
    CKSTopoDevLoop();
    HRESULT IsUSBAudio10Device(bool &fUSBAudio10);
    DWORD WINAPI LoopFilters(CDevice *, FnCheck, void *);
    DWORD WINAPI LoopFiltersNodes(CDevice *, FnCheck, void *);
    DWORD WINAPI LoopFiltersNodesProps(CDevice *, FnCheck, void *);
    DWORD WINAPI LoopFiltersPins(CDevice *, FnCheck, void *);

protected:
    CFilter    *m_pFilter;
    CNode      *m_pNode;
    CPropDesc  *m_pPropDesc;
    CPinFactory *m_pPinFactory;
    UINT        m_iFilter, m_iNode, m_iPropDesc, m_iPin;
};