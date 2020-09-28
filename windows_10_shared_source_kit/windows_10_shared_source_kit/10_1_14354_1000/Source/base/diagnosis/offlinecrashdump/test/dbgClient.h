/*++

    Copyright (C) Microsoft. All rights reserved.

Module Name:
   dbgclient.h

Environment:
   User Mode

Author:
   radutta 
--*/
#pragma once
#include <dbgeng.h>

class DbgClient
{
    DbgClient();
    DbgClient(const DbgClient &);

public:

    static BOOL Initialize(__in PCWSTR pszDmp, __in_opt PCWSTR pszSymPath);
    static IDebugSymbols3 *GetSymbols();
    static IDebugDataSpaces2 *GetDataSpaces();

private:

    static IDebugSymbols3 *s_pSymbols;
    static IDebugDataSpaces2 *s_pSpaces;
};
