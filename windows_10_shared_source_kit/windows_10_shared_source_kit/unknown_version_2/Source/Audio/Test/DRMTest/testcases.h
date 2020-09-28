// Copyright (C) Microsoft. All rights reserved.
#ifndef _TEST_CASES_H
#define _TEST_CASES_H

// adapter.cpp
DWORD Test_Miniport_SetContentId();
DWORD Test_Authentication_Image();
DWORD Test_Authentication_Authenticate();
DWORD Test_Authentication_MinDrmLevel();
DWORD Test_Authentication_CertList();
DWORD Test_Authentication_FindAllDrivers();
DWORD Test_DrmForwardContentToFileObject();
DWORD Test_CheckDisableProtectedAudioDG(); 
DWORD Test_NoTestSignedDrivers();

#endif _TEST_CASES_H
