// Copyright(C) Microsoft. All rights reserved.//

// APIGenericAPO.h
//
// Description:
//
//  Definition of CGenericAPOTest class
//

#pragma once


//********************************************************************************
// <type> Class </type>
// <name> CGenericAPOTest </name>
// <summary> Implementation of helper class for the APO gneric interfaces </summary>
// <interfaces> CProcessorTestBaseAuto </interfaces> 
//*********************************************************************************
class CGenericAPOTest : public CProcessorTestBaseAuto
{
    IAudioProcessingObjectPtr   m_apoObject;
    IAudioProcessingObjectRTPtr m_apoObjectRT;

public:
    CGenericAPOTest() {}

    //
    // Verify that the required interfaces are supported
    // by all system APO's
    DWORD CheckInterfacesSupported();

    //
    // Verify that APO can't be locked with invalid connection information
    DWORD CheckLockForProcess();

    //
    // Validate basic interfaces
    DWORD ValidateBasicInterface();

	//
	// Validate that at LockForProcess the APO's are checking for buffer size
	DWORD LockForProcessValidateBufferSize();
private:
    //
    // Handles initialization for various APO's
    //
    HRESULT InitializeAPO(KNOWN_APO_TYPES type, AUDIO_FRAME_FORMAT* pinFormats, AUDIO_FRAME_FORMAT* poutFormat);
};

