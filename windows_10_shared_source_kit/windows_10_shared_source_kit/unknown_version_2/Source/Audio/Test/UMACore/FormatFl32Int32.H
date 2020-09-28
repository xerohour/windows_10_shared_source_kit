// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatFl32Int32Create();
DWORD Tc_APOFormatFl32Int32Process();

class CBaseFormatTest;
class CAPOFormatFl32Int32Test : public CBaseFormatTest
{
public:
    CAPOFormatFl32Int32Test()
    {
    };
    virtual ~CAPOFormatFl32Int32Test()
    {
    };
    DWORD APOFormatProcess();
};
