// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatInt16Fl32Create();
DWORD Tc_APOFormatInt16Fl32Process();

class CBaseFormatTest;
class CAPOFormatInt16Fl32Test : public CBaseFormatTest
{
public:
    CAPOFormatInt16Fl32Test()
    {
    };
    virtual ~CAPOFormatInt16Fl32Test()
    {
    };
    DWORD APOFormatProcess();
};
