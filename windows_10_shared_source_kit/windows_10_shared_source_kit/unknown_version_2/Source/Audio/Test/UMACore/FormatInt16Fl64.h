// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatInt16Fl64Create();
DWORD Tc_APOFormatInt16Fl64Process();

class CBaseFormatTest;
class CAPOFormatInt16Fl64Test : public CBaseFormatTest
{
public:
    CAPOFormatInt16Fl64Test()
    {
    };
    virtual ~CAPOFormatInt16Fl64Test()
    {
    };
    DWORD APOFormatProcess();
};
