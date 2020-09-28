// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatInt32Fl32Create();
DWORD Tc_APOFormatInt32Fl32Process();

class CBaseFormatTest;
class CAPOFormatInt32Fl32Test : public CBaseFormatTest
{
public:
    CAPOFormatInt32Fl32Test()
    {
    };
    virtual ~CAPOFormatInt32Fl32Test()
    {
    };
    DWORD APOFormatProcess();
};
