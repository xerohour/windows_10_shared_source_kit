// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatFl64Int16Create();
DWORD Tc_APOFormatFl64Int16Process();

class CBaseFormatTest;
class CAPOFormatFl64Int16Test : public CBaseFormatTest
{
public:
    CAPOFormatFl64Int16Test()
    {
    };
    virtual ~CAPOFormatFl64Int16Test()
    {
    };

    DWORD APOFormatProcess();
};
