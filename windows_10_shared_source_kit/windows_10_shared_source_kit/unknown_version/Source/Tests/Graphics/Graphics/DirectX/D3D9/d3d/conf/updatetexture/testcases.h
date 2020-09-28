#ifndef _TESTS
#define _TESTS
#include "CUpdateTex.h"

typedef
struct _DIMENSION_ADJUST_RECORD
{
	UINT uiWidthAdjustment;
	UINT uiHeightAdjustment;
}
DIMENSION_ADJUST_RECORD;
extern DIMENSION_ADJUST_RECORD DimensionAdjustments[];
extern CONST UINT cuiNoOfDimensionAdjustments;

typedef struct _CUBERECT
{
	D3DCUBEMAP_FACES	Face;
	RECT				Rect;
} CUBERECT,*PCUBERECT;

typedef struct _RESTPYE
{
	D3DRESOURCETYPE Type;
	LPCTSTR Name;
} RESTYPE, *PRESTYPE;




typedef struct _PIXELFORMATENTRY
{
	D3DFORMAT				Format;
	LPCTSTR					szDesc;
	DWORD					Mask;
	UINT					uiBytesPerTexel;
} PIXELFORMATENTRY,*PPIXELFORMATENTRY;

extern UINT GetBytesPerTexel(D3DFORMAT d3dfmt);

extern const RESTYPE CRESOURCETYPE[];
extern const UINT TOTALTYPES;

extern const PIXELFORMATENTRY FORMATS[];
extern const UINT TOTALFORMATS;

extern const D3DPOOL	POOLS[];

extern const UINT TOTALPOOLS;

extern const RECT CMULTIRECTS[];
extern const RECT CUYVYMULTIRECTS[];
extern const RECT CDXTnMULTIRECTS[];
extern const UINT TOTALRECTS;

extern const CUBERECT CMULTICUBERECTS[];
extern const CUBERECT CDXTnMULTICUBERECTS[];
extern const UINT TOTALCUBERECTS;

extern const D3DBOX CMULTIBOXES[];
extern const D3DBOX CDXTnMULTIBOXES[];
extern const UINT TOTALBOXES;


extern const TESTCASE tc[];
extern const UINT TOTALCASES;

#endif