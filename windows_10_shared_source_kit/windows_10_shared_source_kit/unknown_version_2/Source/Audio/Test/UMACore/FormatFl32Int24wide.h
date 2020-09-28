// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOFormatFl32Int24wideCreate();
DWORD Tc_APOFormatFl32Int24wideProcess();

class CBaseFormatTest;
class CAPOFormatFl32Int24wideTest : public CBaseFormatTest
{
public:
    CAPOFormatFl32Int24wideTest()
    {
    };
    virtual ~CAPOFormatFl32Int24wideTest()
    {
    };
    DWORD APOFormatProcess();
};
