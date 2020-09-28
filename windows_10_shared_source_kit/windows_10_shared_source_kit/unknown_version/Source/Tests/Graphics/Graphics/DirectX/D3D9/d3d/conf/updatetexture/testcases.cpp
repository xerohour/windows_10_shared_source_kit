#include "TestCases.h"

DIMENSION_ADJUST_RECORD DimensionAdjustments[] =
{
	{0, 0}
	//{8, 16}
};
CONST UINT cuiNoOfDimensionAdjustments = (sizeof(DimensionAdjustments) / sizeof(DimensionAdjustments[0]));

const RESTYPE CRESOURCETYPE[]=
{
	{D3DRTYPE_TEXTURE, TEXT("MIPMAP")},
	{D3DRTYPE_CUBETEXTURE, TEXT("CUBEMAP")}
		//D3DRTYPE_VOLUMETEXTURE ,TEXT("VOLUMETEXTURE")
};
const UINT TOTALTYPES=sizeof (CRESOURCETYPE)/sizeof (RESTYPE);


// The mask field is added because "X channel is not important, and shouldn't be compared."
const PIXELFORMATENTRY FORMATS[]=
{
        {D3DFMT_A8R8G8B8,    "A8R8G8B8"			,0xffffffff,	4},
        {D3DFMT_X8R8G8B8,    "X8R8G8B8"			,0x00ffffff,	4},
        {D3DFMT_R3G3B2,      "R3G3B2"			,0x000000ff,	1},
		{D3DFMT_R5G6B5,      "R5G6B5"			,0x0000ffff,	2},
        {D3DFMT_X1R5G5B5,    "X1R5G5B5"			,0x00007fff,	2},
        {D3DFMT_A1R5G5B5,    "A1R5G5B5"			,0x0000ffff,	2},
        {D3DFMT_A4R4G4B4,    "A4R4G4B4"			,0x0000ffff,	2},
        {D3DFMT_A8R3G3B2,    "A8R3G3B2"			,0x0000ffff,	2},
        {D3DFMT_UYVY,        "UYVY"				,0xffffffff,	2},
        {D3DFMT_YUY2,        "YUY2"				,0xffffffff,	2},
        {D3DFMT_A8P8,	     "A8P8"				,0x0000ffff,	2},
        {D3DFMT_A8L8,	     "A8L8"				,0x0000ffff,	2},
        {D3DFMT_A8,			 "A8"				,0x000000ff,	1},
        {D3DFMT_P8  ,        "P8"				,0x000000ff,	1},
        {D3DFMT_L8  ,	     "L8"				,0x000000ff,	1},
        {D3DFMT_V8U8,	     "D3DFMT_V8U8"		,0x0000ffff,	2},
        {D3DFMT_L6V5U5,	     "D3DFMT_L6V5U5"	,0x0000ffff,	2},
        {D3DFMT_X8L8V8U8,    "D3DFMT_X8L8V8U8"	,0xffffffff,	4},
        {D3DFMT_Q8W8V8U8,    "D3DFMT_Q8W8V8U8"	,0xffffffff,	4},
        {D3DFMT_V16U16,	     "D3DFMT_V16U16"	,0xffffffff,	4},
        //{D3DFMT_W11V11U10,   "D3DFMT_W11V11U10"	,0x3fffffff,	4},//not sure about this mask value
        {D3DFMT_DXT1,        "DXT1"				,0x000000ff,	0},//mask/bpt value doesn't matter
        {D3DFMT_DXT2,        "DXT2"				,0x000000ff,	0},//mask/bpt value doesn't matter
        {D3DFMT_DXT3,        "DXT3"				,0x000000ff,	0},//mask/bpt value doesn't matter
        {D3DFMT_DXT4,        "DXT4"				,0x000000ff,	0},//mask/bpt value doesn't matter
		{D3DFMT_DXT5,        "DXT5"				,0x000000ff,	0},//mask/bpt value doesn't matter

		// DX9-specific formats:
		{D3DFMT_A8B8G8R8,    "D3DFMT_A8B8G8R8"	,0xffffffff,	4},
		{D3DFMT_X8B8G8R8,    "D3DFMT_X8B8G8R8"	,0x00ffffff,	4},
		//{D3DFMT_A8X8V8U8,    "D3DFMT_A8X8V8U8"	,0xff00ffff,	4},
		//{D3DFMT_L8X8V8U8,    "D3DFMT_L8X8V8U8"	,0xff00ffff,	4},
		{D3DFMT_L16,         "D3DFMT_L16"		,0x0000ffff,	2},
		//{D3DFMT_A16B16G16R16},
		//{D3DFMT_Q16W16V16U16},
		//{D3DFMT_CxV8U8},
		{D3DFMT_R16F,        "D3DFMT_R16F"      ,0x0000ffff,	2},
		{D3DFMT_G16R16F,     "D3DFMT_G16R16F"   ,0xffffffff,	4},
		//{D3DFMT_A16B16G16R16F},
		{D3DFMT_R32F,        "D3DFMT_R32F"      ,0xffffffff,	4}
		//{D3DFMT_G32R32F},
		//{D3DFMT_A32B32G32R32F}
};	
const UINT TOTALFORMATS=sizeof (FORMATS)/sizeof (PIXELFORMATENTRY);

UINT GetBytesPerTexel(D3DFORMAT d3dfmt)
{
	UINT uiFormatRecordIndex;

	for (uiFormatRecordIndex = 0; uiFormatRecordIndex < TOTALFORMATS; ++uiFormatRecordIndex)
	{
		if ((FORMATS[uiFormatRecordIndex]).Format == d3dfmt)
			return (FORMATS[uiFormatRecordIndex]).uiBytesPerTexel;
	}

	return 0;
}

const D3DPOOL	POOLS[]=
{
	D3DPOOL_DEFAULT,
	D3DPOOL_SYSTEMMEM,
	D3DPOOL_MANAGED
};

const UINT TOTALPOOLS=sizeof (POOLS)/sizeof (D3DPOOL);


//
// CMULTIRECTS is used for multi-lock tests
//
const RECT CMULTIRECTS[]=
{
	{16,16,48,56}, // aligned
	{25,26,57,39}, // not aligned
	{14,35,15,36}, // point
	{0,46,12,49} , // boundary
	{(GENERICTEXSIZE>>1)-13,(GENERICTEXSIZE>>1)-15,(GENERICTEXSIZE>>1)+7,(GENERICTEXSIZE>>1)+9}, // middle
	{GENERICTEXSIZE-25,GENERICTEXSIZE-37,GENERICTEXSIZE-1,GENERICTEXSIZE}, // boundary
	{0,GENERICTEXSIZE-40,1,GENERICTEXSIZE} // boundary
};

// AndrewLu on 05/01/2002
// For UYVY formats, the rectangles must be "2x2-aligned".
const RECT CUYVYMULTIRECTS[] =
{
	{16,16,48,56}, // aligned
	{24,26,56,38}, // not aligned
	{14,34,16,36}, // "point" (2x2 tile)
	{0,46,12,48} , // boundary
	{(GENERICTEXSIZE>>1)-12,(GENERICTEXSIZE>>1)-14,(GENERICTEXSIZE>>1)+6,(GENERICTEXSIZE>>1)+8}, // middle
	{GENERICTEXSIZE-24,GENERICTEXSIZE-36,GENERICTEXSIZE-2,GENERICTEXSIZE}, // boundary
	{0,GENERICTEXSIZE-40,2,GENERICTEXSIZE} // boundary
};

const RECT CDXTnMULTIRECTS[]=
{
	{16,16,48,56}, 
	{24,28,52,36}, 
	{12,32,16,36}, 
	{0,40,12,48} , // boundary
	{(GENERICTEXSIZE>>1)-12,(GENERICTEXSIZE>>1)-16,(GENERICTEXSIZE>>1)+8,(GENERICTEXSIZE>>1)+8}, // middle
	{GENERICTEXSIZE-24,GENERICTEXSIZE-36,GENERICTEXSIZE,GENERICTEXSIZE}, // boundary
	{0,GENERICTEXSIZE-40,8,GENERICTEXSIZE} // boundary
};



const UINT TOTALRECTS=sizeof (CMULTIRECTS)/sizeof (RECT);


//
// CMULTIBOXES is used for multi-lock tests
//
const D3DBOX CMULTIBOXES[]=
{
	{16,16,48,56,8,40}, // aligned
	{25,26,57,39,4,7}, // not aligned
	{14,35,15,36,16,17}, // point
	{0,46,12,49,0,4} , // boundary
	{(GENERICTEXSIZE>>1)-13,(GENERICTEXSIZE>>1)-15,(GENERICTEXSIZE>>1)+7,(GENERICTEXSIZE>>1)+9,(GENERICTEXSIZE>>1)-2,(GENERICTEXSIZE>>1)+3}, // middle
	{GENERICTEXSIZE-25,GENERICTEXSIZE-37,GENERICTEXSIZE-1,GENERICTEXSIZE,GENERICTEXSIZE-1,GENERICTEXSIZE}, // boundary
	{0,GENERICTEXSIZE-40,1,GENERICTEXSIZE,1,GENERICTEXSIZE} // boundary

};

const D3DBOX CDXTnMULTIBOXES[]=
{
	{16,16,48,56,8,40}, 
	{20,28,56,36,4,15}, 
	{12,32,16,36,16,23},
	{0,44,12,48,0,4} , 
	{(GENERICTEXSIZE>>1)-12,(GENERICTEXSIZE>>1)-12,(GENERICTEXSIZE>>1)+8,(GENERICTEXSIZE>>1)+8,(GENERICTEXSIZE>>1)-4,(GENERICTEXSIZE>>1)+4}, // middle
	{GENERICTEXSIZE-24,GENERICTEXSIZE-36,GENERICTEXSIZE,GENERICTEXSIZE,GENERICTEXSIZE-4,GENERICTEXSIZE}, // boundary
	{0,GENERICTEXSIZE-40,4,GENERICTEXSIZE,8,GENERICTEXSIZE} // boundary

};


const UINT TOTALBOXES=sizeof (CMULTIBOXES)/sizeof (D3DBOX);



#if FAST_TEST

const TESTCASE tc[] =
{	//szTitle
    //uDestSizeSrc,uSrcLevels,
    //uDestSize,uDestLevels
    //(*PreUpdateFunc()),(*VerifyFunc()),bExpectSucceed;

    {TEXT("Lock whole src tex, unlock it, UpdateTexture"),
    GENERICTEXSIZE, GENERICTEXLEVEL,
    GENERICTEXSIZE, GENERICTEXLEVEL,
    &(UpdateTex::Case1),&(UpdateTex::CheckEntireTexOK),TRUE,FALSE},

    {TEXT("Lock (<=6) sub rects on src tex, unlock it, UpdateTexture"),
    GENERICTEXSIZE, GENERICTEXLEVEL,
    GENERICTEXSIZE, GENERICTEXLEVEL,
    &(UpdateTex::Case2),&(UpdateTex::CheckSubTexOK),TRUE,FALSE},	


    {TEXT("AddDirtyRect() the  entire src tex, UpdateTexture"),
    GENERICTEXSIZE, GENERICTEXLEVEL,
    GENERICTEXSIZE, GENERICTEXLEVEL,
    &(UpdateTex::Case4),&(UpdateTex::CheckEntireTexOK),TRUE,FALSE},

    {TEXT("AddDirtyRect() (<=6) sub rects dirty, UpdateTexture"),
    GENERICTEXSIZE, GENERICTEXLEVEL,
    GENERICTEXSIZE, GENERICTEXLEVEL,
    &(UpdateTex::Case5),&(UpdateTex::CheckSubTexOK),TRUE,FALSE},


    {TEXT("Lock src tex with D3DLOCK_NO_DIRTY_UPDATE, Unlock it, UpdateTexture"),
    GENERICTEXSIZE, GENERICTEXLEVEL,
    GENERICTEXSIZE, GENERICTEXLEVEL,
    &(UpdateTex::Case7),&(UpdateTex::CheckEntireTexFail),TRUE,TRUE},

    {TEXT("Lock src tex with D3DLOCK_READONLY, Unlock it, UpdateTexture"),
    GENERICTEXSIZE, GENERICTEXLEVEL,
    GENERICTEXSIZE, GENERICTEXLEVEL,
    &(UpdateTex::Case7_2),&(UpdateTex::CheckEntireTexFail),TRUE,TRUE},

    {TEXT("Update immediately after another UpdateTexture "),
    GENERICTEXSIZE, GENERICTEXLEVEL,
    GENERICTEXSIZE, GENERICTEXLEVEL,
    &(UpdateTex::Case8),&(UpdateTex::CheckEntireTexFail),TRUE,TRUE},

    {TEXT("LockSubLevels without the top level "),
    GENERICTEXSIZE, GENERICTEXLEVEL,
    GENERICTEXSIZE, GENERICTEXLEVEL,
    &(UpdateTex::Case9),&(UpdateTex::CheckEntireTexFail),TRUE,TRUE},

    //
    // This is a tweak to use Case4 to AddDirtyXXX(NULL) here,
    // because the source texture is clean, and need to become
    // dirty to make the UpdateTexture works. It also means
    // if AddDirtyXXX has a problem, this could mistakenly reported
    //	error
    //
    //{TEXT("Verify texture of different levels can be updated "),
    //GENERICTEXSIZE,GENERICTEXLEVEL,
    //GENERICTEXSIZE>>3,GENERICTEXLEVEL-3,
    //&(UpdateTex::Case4),&(UpdateTex::CheckEntireTexOK),TRUE,FALSE}
};
#else // FAST_TEST

const TESTCASE tc[]=
{	//szTitle
	//uDestSizeSrc,uSrcLevels,
	//uDestSize,uDestLevels
	//(*PreUpdateFunc()),(*VerifyFunc()),bExpectSucceed;

	{TEXT("Lock whole src tex, unlock it, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case1),&(UpdateTex::CheckEntireTexOK),TRUE,FALSE},


	{TEXT("Lock (<=6) sub rects on src tex, unlock it, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case2),&(UpdateTex::CheckSubTexOK),TRUE,FALSE},	

	{TEXT("Lock (>6) sub rects on src tex, unlock it, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case3),&(UpdateTex::CheckManySubTexOK),TRUE,FALSE},

	{TEXT("AddDirtyRect() the  entire src tex, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case4),&(UpdateTex::CheckEntireTexOK),TRUE,FALSE},

	{TEXT("AddDirtyRect() (<=6) sub rects dirty, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case5),&(UpdateTex::CheckSubTexOK),TRUE,FALSE},

	{TEXT("AddDirtyRect() (>6) sub rects dirty, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case6),&(UpdateTex::CheckManySubTexOK),TRUE,FALSE},

	// AndrewLu; 09/03/2002; Removal of CopyRects test cases
	// Notes:
	// The following test cases all involved "dirtying" a system memory
	// source texture with CopyRects operations before calling UpdateTexture.
	// The CopyRects method has been removed from the DX9 interfaces and the substitute
	// method (UpdateSurface) can perform only system memory to video memory copies,
	// so these test cases cannot be adapted for DX9.

	/*
	{TEXT("CopyRects (<=6) from sys-mem tex -> src tex, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case13),&(UpdateTex::CheckSubTexOK),TRUE},

	{TEXT("CopyRects (<=6) from managed-mem tex -> src tex, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case13_2),&(UpdateTex::CheckSubTexOK),TRUE},

	{TEXT("CopyRects (<=6) from video-mem tex -> src tex, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case13_3),&(UpdateTex::CheckSubTexOK),TRUE},



	{TEXT("CopyRects (>6) from sys-mem tex -> src tex, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case14),&(UpdateTex::CheckManySubTexOK),TRUE},

	{TEXT("CopyRects (>6) from managed-mem tex -> src tex, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case14_2),&(UpdateTex::CheckManySubTexOK),TRUE},

	{TEXT("CopyRects (>6) from video-mem tex -> src tex, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case14_3),&(UpdateTex::CheckManySubTexOK),TRUE},
	*/

	{TEXT("Lock src tex with D3DLOCK_NO_DIRTY_UPDATE, Unlock it, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case7),&(UpdateTex::CheckEntireTexFail),TRUE,TRUE},

	{TEXT("Lock src tex with D3DLOCK_READONLY, Unlock it, UpdateTexture"),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case7_2),&(UpdateTex::CheckEntireTexFail),TRUE,TRUE},



	{TEXT("Update immediately after another UpdateTexture "),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case8),&(UpdateTex::CheckEntireTexFail),TRUE,TRUE},

	{TEXT("LockSubLevels without the top level "),
	 GENERICTEXSIZE,0,
	 GENERICTEXSIZE,0,
	 &(UpdateTex::Case9),&(UpdateTex::CheckEntireTexFail),TRUE,TRUE},


	 //
	 // This is a tweak to use Case4 to AddDirtyXXX(NULL) here,
	 // because the source texture is clean, and need to become
	 // dirty to make the UpdateTexture works. It also means
	 // if AddDirtyXXX has a problem, this could mistakenly reported
	 //	error
	 //
	{TEXT("Verify texture of different levels can be updated "),
	 GENERICTEXSIZE,GENERICTEXLEVEL,
	 GENERICTEXSIZE>>3,GENERICTEXLEVEL-3,
	 &(UpdateTex::Case4),&(UpdateTex::CheckEntireTexOK),TRUE,FALSE}

	 /*
	{TEXT("Verify failure when bottom level texture's size doesn't match "),
	 GENERICTEXSIZE,GENERICTEXLEVEL,
	 GENERICTEXSIZE>>3,GENERICTEXLEVEL-4,
	 &(UpdateTex::DoNothing),&(UpdateTex::DoNothing),FALSE,TRUE},
	 */


	 /*
	{TEXT("src tex: Nonsquare, Dest tex: square "),
	 GENERICTEXSIZE,GENERICTEXLEVEL,
	 GENERICTEXSIZE,GENERICTEXLEVEL,
	 &(UpdateTex::Case11),&(UpdateTex::DoNothing),FALSE,TRUE},
	 */

	 /*
	{TEXT("src tex: Square, dest tex: Nonsquare "),
	 GENERICTEXSIZE,GENERICTEXLEVEL,
	 GENERICTEXSIZE,GENERICTEXLEVEL,
	 &(UpdateTex::Case10),&(UpdateTex::DoNothing),FALSE,TRUE},
	 */

	/*
	{TEXT("src tex: Nonsquare, dest tex: Nonsquare"),
	 GENERICTEXSIZE,GENERICTEXLEVEL,
	 GENERICTEXSIZE,GENERICTEXLEVEL,
	 &(UpdateTex::Case12),&(UpdateTex::CheckEntireTexOK),TRUE,TRUE},
	 */
};
#endif //FAST_TEST
const UINT TOTALCASES= sizeof (tc)/sizeof (TESTCASE);
