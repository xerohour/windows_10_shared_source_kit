// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatUint8Fl32Create();
DWORD Tc_APOFormatUint8Fl32Process();

class CBaseFormatTest;
class CAPOFormatUint8Fl32Test : public CBaseFormatTest
{
public:
    CAPOFormatUint8Fl32Test()
    {
    };
    virtual ~CAPOFormatUint8Fl32Test()
    {
    };
    DWORD APOFormatProcess();
};
