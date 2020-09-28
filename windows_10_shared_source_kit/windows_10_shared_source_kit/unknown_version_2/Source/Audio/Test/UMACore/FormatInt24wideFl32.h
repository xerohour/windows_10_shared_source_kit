// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatInt24wideFl32Create();
DWORD Tc_APOFormatInt24wideFl32Process();

class CBaseFormatTest;
class CAPOFormatInt24wideFl32Test : public CBaseFormatTest
{
public:
    CAPOFormatInt24wideFl32Test()
    {
    };
    virtual ~CAPOFormatInt24wideFl32Test()
    {
    };
    DWORD APOFormatProcess();
};
