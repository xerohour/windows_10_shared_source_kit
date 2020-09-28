// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatInt32Fl64Create();
DWORD Tc_APOFormatInt32Fl64Process();

class CBaseFormatTest;
class CAPOFormatInt32Fl64Test : public CBaseFormatTest
{
public:
    CAPOFormatInt32Fl64Test()
    {
    };
    virtual ~CAPOFormatInt32Fl64Test()
    {
    };
    DWORD APOFormatProcess();
};
