// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatFl32Int24Create();
DWORD Tc_APOFormatFl32Int24Process();

class CBaseFormatTest;
class CAPOFormatFl32Int24Test : public CBaseFormatTest
{
public:
    CAPOFormatFl32Int24Test()
    {
    };
    virtual ~CAPOFormatFl32Int24Test()
    {
    };
    DWORD APOFormatProcess();
};
