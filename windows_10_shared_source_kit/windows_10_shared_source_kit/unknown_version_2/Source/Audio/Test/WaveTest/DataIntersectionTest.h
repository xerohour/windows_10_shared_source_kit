// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  DataIntersectionTest.h
//
// Abstract:
//
//  Header for CDataIntersectionTest class
//



// -------------------------------------------------------------------------------

class CDataIntersectionTest
{

private:
    CPCMAudioPin   *m_pPCMAudioPin;
    DWORD           m_dwBufferMS;
    BOOL            m_bIntersect;               // member tracks DataIntersection    
    bool            m_fIsWaveRTPin;             // true if the current pin is WaveRT
    bool            m_fIsStdStrmPin;            // true if the current pin supports KSINTERFACE_STANDARD_STREAMING
    bool            m_fIsStdLoopedStrmPin;      // true if the current pin supports KSINTERFACE_STANDARD_LOOPED_STREAMING

private:
    BOOL    CreateIntersection(WAVEFORMATEX* pWfx);

public:
    CDataIntersectionTest()
    : m_pPCMAudioPin(NULL)
    , m_dwBufferMS(0)
    , m_bIntersect(FALSE)  
    , m_fIsWaveRTPin(false)
    , m_fIsStdStrmPin(false)
    , m_fIsStdLoopedStrmPin(false)
    {};
    
    virtual ~CDataIntersectionTest()    {};

    BOOL    Initialize(CPCMAudioPin* pKsPin, bool fIsWaveRTPin, bool fIsStdStrmPin, bool fIsStdLoopedStrmPin);
    BOOL    CreateRender_Intersection(WAVEFORMATEX* pWfx); // Checks for intersection on render pin
    BOOL    CreateCapture_Intersection(WAVEFORMATEX* pWfx); //Checks for intersection on record pin
    BOOL    Render();
    BOOL    DestroyRender();
    BOOL    Capture();
    BOOL    DestroyCapture();

    BOOL    DoesIntersect()     { return m_bIntersect; }
}; 
