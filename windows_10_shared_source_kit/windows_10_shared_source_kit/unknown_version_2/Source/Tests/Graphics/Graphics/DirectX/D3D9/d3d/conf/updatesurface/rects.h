#ifndef MY_RECTS_INCLUDED
#define MY_RECTS_INCLUDED

#include <windows.h>

typedef
struct _SOURCE_DEST_RECORD
{
	RECT rectSource;
	POINT pointDest;
}
SOURCE_DEST_RECORD;

extern SOURCE_DEST_RECORD SourceDestPairs[];
extern CONST UINT cuiNoOfSourceDestPairs;

#endif
