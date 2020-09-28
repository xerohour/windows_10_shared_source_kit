// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatFl32Uint8Create();
DWORD Tc_APOFormatFl32Uint8Process();

class CBaseFormatTest;
class CAPOFormatFl32Uint8Test : public CBaseFormatTest
{
public:
    CAPOFormatFl32Uint8Test()
    {
    };
    virtual ~CAPOFormatFl32Uint8Test()
    {
    };

    DWORD APOFormatProcess();
};
