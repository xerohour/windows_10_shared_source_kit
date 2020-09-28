// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatInt24Fl32Create();
DWORD Tc_APOFormatInt24Fl32Process();

class CBaseFormatTest;
class CAPOFormatInt24Fl32Test : public CBaseFormatTest
{
public:
    CAPOFormatInt24Fl32Test()
    {
    };
    virtual ~CAPOFormatInt24Fl32Test()
    {
    };
    DWORD APOFormatProcess();
};
