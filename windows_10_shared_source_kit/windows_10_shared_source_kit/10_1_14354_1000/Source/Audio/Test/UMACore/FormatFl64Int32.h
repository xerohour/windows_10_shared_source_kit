// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatFl64Int32Create();
DWORD Tc_APOFormatFl64Int32Process();

class CBaseFormatTest;
class CAPOFormatFl64Int32Test : public CBaseFormatTest
{
public:
    CAPOFormatFl64Int32Test()
    {
    };
    virtual ~CAPOFormatFl64Int32Test()
    {
    };
    DWORD APOFormatProcess();
};
