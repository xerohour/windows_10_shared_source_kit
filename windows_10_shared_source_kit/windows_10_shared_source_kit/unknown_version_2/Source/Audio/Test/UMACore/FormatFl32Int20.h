// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatFl32Int20Create();
DWORD Tc_APOFormatFl32Int20Process();

class CBaseFormatTest;
class CAPOFormatFl32Int20Test : public CBaseFormatTest
{
public:
    CAPOFormatFl32Int20Test()
    {
    };
    virtual ~CAPOFormatFl32Int20Test()
    {
    };
    DWORD APOFormatProcess();
};
