#include "VTVolume.h"

#if FAST_TEST
const UINT VTGENERICTEXSIZE=32;
const UINT VTGENERICTEXLEVEL=2;
#else
const UINT VTGENERICTEXSIZE=32;
const UINT VTGENERICTEXLEVEL=6;
#endif

// VTCMULTIBOXES is used for multi-lock tests
//
const BOX VTCMULTIBOXES[]=
{
	{0,0,4,14,0,24}, // aligned
	{6,8,8,25,4,7}, // not aligned
	{7,7,8,8,6,7}, // point
	{8,2,10,12,0,4} , 
	{(VTGENERICTEXSIZE>>1)-6,(VTGENERICTEXSIZE>>1)-5,(VTGENERICTEXSIZE>>1)+7,(VTGENERICTEXSIZE>>1)+9,(VTGENERICTEXSIZE>>1)-2,(VTGENERICTEXSIZE>>1)+3}, // middle
	{VTGENERICTEXSIZE-15,VTGENERICTEXSIZE-17,VTGENERICTEXSIZE-1,VTGENERICTEXSIZE,1,VTGENERICTEXSIZE}, // boundary
	{0,VTGENERICTEXSIZE-24,1,VTGENERICTEXSIZE,1,VTGENERICTEXSIZE} // boundary

};

const BOX VTCDXTnMULTIBOXES[]=
{
	{0,0,4,16,0,24}, 
	{4,4,8,16,8,12}, 
	{12,12,16,20,16,20},
	{0,4,12,16,12,16} , 
	{(VTGENERICTEXSIZE>>1)-12,(VTGENERICTEXSIZE>>1)-12,(VTGENERICTEXSIZE>>1)+8,(VTGENERICTEXSIZE>>1)+8,(VTGENERICTEXSIZE>>1)-4,(VTGENERICTEXSIZE>>1)+8}, // middle
	{VTGENERICTEXSIZE-24,VTGENERICTEXSIZE-28,VTGENERICTEXSIZE-4,VTGENERICTEXSIZE,VTGENERICTEXSIZE-4,VTGENERICTEXSIZE}, // boundary
	{0,VTGENERICTEXSIZE-24,4,VTGENERICTEXSIZE,8,VTGENERICTEXSIZE} // boundary

};


const UINT VTTOTALBOXES=sizeof (VTCMULTIBOXES)/sizeof (BOX);

#if FAST_TEST
const VTTESTCASE tc[] =
{	//szTitle
    //uDestSizeSrc,uSrcLevels,
    //uDestSize,uDestLevels
    //(*PreUpdateFunc()),bExpectSucceed;

    {TEXT("Lock whole src tex, unlock it, UpdateTexture"),
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    &(CMipVolTest::Case1),TRUE},

    {TEXT("Lock (<=6) sub boxes on src tex, unlock it, UpdateTexture"),
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    &(CMipVolTest::Case2),TRUE},

    {TEXT("AddDirtyRect() the  entire src tex, UpdateTexture"),
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    &(CMipVolTest::Case4),TRUE},

    {TEXT("AddDirtyRect() (<=6) sub boxes dirty, UpdateTexture"),
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    &(CMipVolTest::Case5),TRUE},


    {TEXT("Lock src tex with D3DLOCK_NO_DIRTY_UPDATE, Unlock it, UpdateTexture"),
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    &(CMipVolTest::Case7),TRUE},

    {TEXT("Lock src tex with D3DLOCK_READONLY, Unlock it, UpdateTexture"),
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    &(CMipVolTest::Case7_2),TRUE},

    {TEXT("Update immediately after another UpdateTexture "),
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    &(CMipVolTest::Case8),TRUE},

    {TEXT("LockSubLevels without the top level "),
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    VTGENERICTEXSIZE, VTGENERICTEXLEVEL,
    &(CMipVolTest::Case9),TRUE},
};
#else // FAST_TEST

const VTTESTCASE tc[]=
{	//szTitle
	//uDestSizeSrc,uSrcLevels,
	//uDestSize,uDestLevels
	//(*PreUpdateFunc()),bExpectSucceed;

	{TEXT("Lock whole src tex, unlock it, UpdateTexture"),
	 VTGENERICTEXSIZE,0,
	 VTGENERICTEXSIZE,0,
	 &(CMipVolTest::Case1),TRUE},



	{TEXT("Lock (<=6) sub boxes on src tex, unlock it, UpdateTexture"),
	 VTGENERICTEXSIZE,0,
	 VTGENERICTEXSIZE,0,
	 &(CMipVolTest::Case2),TRUE},

	{TEXT("Lock (>6) sub boxes on src tex, unlock it, UpdateTexture"),
	 VTGENERICTEXSIZE,0,
	 VTGENERICTEXSIZE,0,
	 &(CMipVolTest::Case3),TRUE},

	{TEXT("AddDirtyRect() the  entire src tex, UpdateTexture"),
	 VTGENERICTEXSIZE,0,
	 VTGENERICTEXSIZE,0,
	 &(CMipVolTest::Case4),TRUE},

	{TEXT("AddDirtyRect() (<=6) sub boxes dirty, UpdateTexture"),
	 VTGENERICTEXSIZE,0,
	 VTGENERICTEXSIZE,0,
	 &(CMipVolTest::Case5),TRUE},

	{TEXT("AddDirtyRect() (>6) sub boxes dirty, UpdateTexture"),
	 VTGENERICTEXSIZE,0,
	 VTGENERICTEXSIZE,0,
	 &(CMipVolTest::Case6),TRUE},

	{TEXT("Lock src tex with D3DLOCK_NO_DIRTY_UPDATE, Unlock it, UpdateTexture"),
	 VTGENERICTEXSIZE,0,
	 VTGENERICTEXSIZE,0,
	 &(CMipVolTest::Case7),TRUE},

	{TEXT("Lock src tex with D3DLOCK_READONLY, Unlock it, UpdateTexture"),
	 VTGENERICTEXSIZE,0,
	 VTGENERICTEXSIZE,0,
	 &(CMipVolTest::Case7_2),TRUE},



	{TEXT("Update immediately after another UpdateTexture "),
	 VTGENERICTEXSIZE,0,
	 VTGENERICTEXSIZE,0,
	 &(CMipVolTest::Case8),TRUE},

	{TEXT("LockSubLevels without the top level "),
	 VTGENERICTEXSIZE,0,
	 VTGENERICTEXSIZE,0,
	 &(CMipVolTest::Case9),TRUE},

	 /*
	{TEXT("Verify failure when bottom level texture's size doesn't match "),
	 VTGENERICTEXSIZE,VTGENERICTEXLEVEL,
	 VTGENERICTEXSIZE>>3,VTGENERICTEXLEVEL-4,
	 &(CMipVolTest::DoNothing),FALSE},
	 */
/*

	{TEXT("Nonsquare vs square case"),
	 VTGENERICTEXSIZE,VTGENERICTEXLEVEL,
	 VTGENERICTEXSIZE,VTGENERICTEXLEVEL,
	 &(CMipVolTest::Case10),FALSE},

	{TEXT("Square vs Nonsquare case"),
	 VTGENERICTEXSIZE,VTGENERICTEXLEVEL,
	 VTGENERICTEXSIZE,VTGENERICTEXLEVEL,
	 &(CMipVolTest::Case11),FALSE},

	{TEXT("Nonsquare vs Nonsquare case"),
	 VTGENERICTEXSIZE,VTGENERICTEXLEVEL,
	 VTGENERICTEXSIZE,VTGENERICTEXLEVEL,
	 &(CMipVolTest::Case12),TRUE},
*/
};
#endif //FAST_TEST
const UINT VTTOTALCASES= sizeof (tc)/sizeof (VTTESTCASE);

