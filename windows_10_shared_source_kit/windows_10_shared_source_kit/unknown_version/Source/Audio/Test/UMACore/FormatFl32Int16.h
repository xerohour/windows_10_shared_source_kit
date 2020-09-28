// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatFl32Int16Create();
DWORD Tc_APOFormatFl32Int16Process();

class CBaseFormatTest;
class CAPOFormatFl32Int16Test : public CBaseFormatTest
{
public:
    CAPOFormatFl32Int16Test()
    {
    };
    virtual ~CAPOFormatFl32Int16Test()
    {
    };

    DWORD APOFormatProcess();
};
