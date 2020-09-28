// Copyright (C) Microsoft. All rights reserved.
#ifndef _UTIL_H
#define _UTIL_H

///////////////////////////////////////////////////////////////////////////////
// Defines
#define TEST_FREQUENCY      1000

///////////////////////////////////////////////////////////////////////////////
// Macros
#define BOOL_STRING(f) \
    (f ? _T("TRUE") : _T("FALSE"))

#define COPYRIGHT(s)            s.CopyProtect
#define ORIGINAL(s)             s.Reserved
#define DIGITAL(s)              s.DigitalOutputDisable

#define COPYRIGHT_STRING(s)     BOOL_STRING(s->CopyProtect)
#define ORIGINAL_STRING(s)      BOOL_STRING(s->Reserved)
#define DIGITAL_STRING(s)       BOOL_STRING(s->DigitalOutputDisable)

///////////////////////////////////////////////////////////////////////////////
// typedefs
#define MAX_FAILURES    30
typedef struct
{
    BOOL                        ParseCatalogFile;
    DWORD                       MinDRMLevel;
    BOOL                        RequireSignedCatalog;
    BOOL                        CheckDriverImages;
    AUTHSTRUCT                  DriverFailures[MAX_FAILURES];
    DWORD                       NumFailures;
    DWORD                       MaxFailures;
    BOOL                        AllowTestCertificate;
    LPTSTR                      DriverFileCache;
} AUTHPARAM, *PAUTHPARAM;

///////////////////////////////////////////////////////////////////////////////
// Functions
BOOL IsUsb
(
    LPTSTR                      szPnpId
);

///////////////////////////////////////////////////////////////////////////////
// Globals
extern DRMRIGHTS                gDrmRights[];

#endif
