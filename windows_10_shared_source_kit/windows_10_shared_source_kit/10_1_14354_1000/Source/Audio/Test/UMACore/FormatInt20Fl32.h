// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatInt20Fl32Create();
DWORD Tc_APOFormatInt20Fl32Process();

class CBaseFormatTest;
class CAPOFormatInt20Fl32Test : public CBaseFormatTest
{
public:
    CAPOFormatInt20Fl32Test()
    {
    };
    virtual ~CAPOFormatInt20Fl32Test()
    {
    };
    DWORD APOFormatProcess();
};
