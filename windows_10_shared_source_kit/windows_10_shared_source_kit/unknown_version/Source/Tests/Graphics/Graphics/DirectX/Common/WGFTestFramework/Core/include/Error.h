//////////////////////////////////////////////////////////////////////////
//  Error.h
//  created:	2004/10/01
//
//  purpose: Handles error output.
//////////////////////////////////////////////////////////////////////////

#pragma once

extern void WGFTESTAPI WriteErrorA( const CHAR *szFormat, ... );
extern void WGFTESTAPI WriteErrorW( const WCHAR *szFormat, ... );


#if defined(UNICODE)
#define WriteError WriteErrorW
#else
#define WriteError WriteErrorA
#endif
