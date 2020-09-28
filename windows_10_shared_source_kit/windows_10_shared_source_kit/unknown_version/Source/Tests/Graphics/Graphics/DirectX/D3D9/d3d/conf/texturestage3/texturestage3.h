///////////////////////////////////////////////////////////
// FILE:        TextureStage3.h
// DESC:        TextureStageTest 3rd generation.
// AUTHOR:      Bobby Paulose 
//				Bill Kristiansen
// Created:		May 21, 2002.
///////////////////////////////////////////////////////////


#ifndef _TEXSTAGE_EFFICIENT_H_
#define _TEXSTAGE_EFFICIENT_H_

#include <d3d8.h>
#include "CD3DTest.h"
#include "CTextures.h"
#include "CBuffers.h"


//HACK///////HACK///////HACK/////
//#defines from <d3d9.h>/////////
#define D3DPMISCCAPS_PERSTAGECONSTANT   0x00008000L /* Device supports per-stage constants */
#define D3DTA_CONSTANT          0x00000006  // select texture stage constant
#define D3DTSS_CONSTANT       (enum _D3DTEXTURESTAGESTATETYPE)32 /* Per-stage constant D3DTA_CONSTANT */

///////////////some caps local to this test and not supported by d3d.
#define D3DTSCAPS_SPECULAR ((DWORD)0x1)
#define D3DTSCAPS_COMPLEMENT ((DWORD)0x2)
#define D3DTSCAPS_ALPHAREPLICATE ((DWORD)0x4)



//Mips levels for MIPS texture created
#define MIP_LEVELS 6
#define VOLUME_MIP_LEVELS 1
#define CUBE_MIP_LEVELS 1
//Volume depth for Volume texture created
#define VOLUME_DEPTH 4  //must be power of 2


//max texture stages in refrast or hardware
#define MAX_TEX_STAGES 8

//update this as typedef enum _D3DTEXTUREOP changes per dx version
#define D3DTOP_LAST_PLACEHOLDER (D3DTOP_LERP+1)

#define NUM_RENDER_STATES 4
#define NUM_TEXSTAGE_STATES 2 + 8 //(8 for new D3DTSS_CONSTANT in DX9)

#define DISPLAY_HEIGHT	128 //a power of 2
#define DISPLAY_WIDTH	128 //a power of 2

#define BUFFER_LENGTH 256
#define OPTION_BUFFER_LENGTH 1024

#define TRI_LIST_INDX_COUNT 6
#define TRI_STRIP_INDX_COUNT 4
#define TRI_FAN_INDX_COUNT 4

#define LARGE_TEXWIDTH		DISPLAY_WIDTH*2
#define LARGE_TEXHEIGHT		DISPLAY_HEIGHT*2
#define DEFAULT_TEXWIDTH	DISPLAY_WIDTH
#define DEFAULT_TEXHEIGHT	DISPLAY_HEIGHT
#define SMALL_TEXWIDTH		DISPLAY_WIDTH/2
#define SMALL_TEXHEIGHT		DISPLAY_HEIGHT/2

//This is an offset applied to TL gemoetry in order to align texture and gemetry 0.
#define OFFSET_FOR_TEXTURE	0.5f


enum PrimitiveID
{
	TRI_LIST_DST2,
	TRI_STRIP_DST2,
	TRI_FAN_DST2,
	TRI_STRIP_DST2_HalfDiffuse,
	TRI_STRIP_DST2_DiffuseAlphaGradient,
	TRI_STRIP_DST2_ZeroDiffuse,	
	TRI_LIST_DST2T2,
	TRI_STRIP_DST2T2,
	TRI_FAN_DST2T2,
	TRI_LIST_DST2T2half,
	TRI_STRIP_DST2T2half,
	TRI_FAN_DST2T2half,
	TRI_LIST_DST2T3T3_forVolCube,
	TRI_STRIP_DST2T3T3_forVolCube,
	TRI_FAN_DST2T3T3_forVolCube,
	TRI_LIST_DST2T3T3_forMipMap,
	TRI_STRIP_DST2T3T3_forMipMap,
	TRI_FAN_DST2T3T3_forMipMap,
};

enum TextureID
{
	TEX_NULL,
	TEX_PREVIOUS_STAGE,
	TEX_ONE_STAGE_ABOVE,
	TEX_TWO_STAGE_ABOVE,
	TEX_THREE_STAGE_ABOVE,
	TEX_FOUR_STAGE_ABOVE,
	TEX_FIVE_STAGE_ABOVE,
	TEX_SIX_STAGE_ABOVE,
	TEX_SEVEN_STAGE_ABOVE,
	TEX_CHECKER,
	TEX_CHECKER_HIGHFREQ,
	TEX_HALF_WHITE_HALF_ALPHA,
	TEX_00FFFFFF,
	TEX_GRADIENT_ALPHA,
	TEX_GRADIENT_ALPHA2,
	TEX_GRADIENT_BLACK_WHITE,
	TEX_GRADIENT_BLUE_WHITE,
	TEX_GRADIENT_FIRE_WHITE,
	TEX_GRADIENT_FOR_BUMP,
	TEX_GRADIENT_FOR_BUMP2,
	TEX_STRIPES,
	TEX_STRIPES_FOR_CONST,
	TEX_DECOR,
	TEX_DECOR2,
	TEX_STAGE0,
	TEX_STAGE1,
	TEX_STAGE2,
	TEX_STAGE3,
	TEX_STAGE4,
	TEX_STAGE5,
	TEX_STAGE6,
	TEX_STAGE7,
	TEX_STAGE_ALL,
	TEX_NEWTS,
	TEX_TEXT1,
	TEX_TEXT2,
	TEX_TEXT3,
	TEX_TEXT4,
	TEX_TEXT5,
	TEX_TEXT6,
	TEX_DX_LOGO,
	TEX_DX_LOGO2,
	TEX_DX_LOGO3,
	TEX_DX_LOGO4,
	TEX_DX_LOGO5,
	TEX_DX_LOGOBUMP,
	TEX_DX_LOGOBUMP2,
	TEX_COLORS_BLEND,
	TEX_WIN_LOGO,
	TEX_WIN_LOGO2,
	TEX_WIN_LOGO3,
	TEX_DOTS1,

	TEX_MIPS,
	TEX_VOLUME,
	TEX_CUBE,
	TEX_MIPSVOLUMECUBE_1,
	TEX_MIPSVOLUMECUBE_2,
	TEX_MIPSVOLUMECUBE_3,
	TEX_MIPSVOLUMECUBE_4,
	TEX_MIPSVOLUMECUBE_5,
	TEX_MIPSVOLUMECUBE_6,

	TEX_TEST,
	TEX_LAST_PLACEHOLDER_CIMAGE,
	TEX_FILE_TEST,
	TEX_LAST_PLACEHOLDER
};

//extern TCHAR *g_TexFileNames[TEX_LAST_PLACEHOLDER - TEX_LAST_PLACEHOLDER_CIMAGE];

enum CycleID
{
	CYC_DONT_CARE,
	CYC_PREVIOUS_STAGE_DEPENDENT,
	CYC_ONE_STAGE_ABOVE_DEPENDENT,
	CYC_TWO_STAGE_ABOVE_DEPENDENT,
	CYC_THREE_STAGE_ABOVE_DEPENDENT,
	CYC_FOUR_STAGE_ABOVE_DEPENDENT,
	CYC_FIVE_STAGE_ABOVE_DEPENDENT,
	CYC_SIX_STAGE_ABOVE_DEPENDENT,
	CYC_SEVEN_STAGE_ABOVE_DEPENDENT,
	CYC_ALL,
	CYC_RGB,
	CYC_ALPHA,
	CYC_BUMPDUDV,
	CYC_LUMINANCE,
	CYC_BUMPLUMINANCE,
	CYC_LAST_PLACEHOLDER_TO_FREE,
	//Framework format lists
	CYC_VOLUME,
	CYC_CUBE,
	//End Framework format lists
	CYC_VOLUME_RGB,
	CYC_CUBE_RGB,
	CYC_LAST_PLACEHOLDER

};


typedef struct _VERTEX_DST2
{
    float x, y, z, w;
    DWORD a, b;
    float u0, v0;
} VERTEX_DST2, *PVERTEX_DST2;
#define VERTEX_DST2_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0) )

typedef struct _VERTEX_DST2T2
{
    float x, y, z, w;
    DWORD a, b;
    float u0, v0;
    float u1, v1;
} VERTEX_DST2T2, *PVERTEX_DST2T2;
#define VERTEX_DST2T2_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) )

typedef struct _VERTEX_DST2T3T3
{
    float x, y, z, w;
    DWORD a, b;
    float u0, v0;
    float u1, v1, w1;
    float u2, v2, w2;
} VERTEX_DST2T3T3, *PVERTEX_DST2T3T3;
#define VERTEX_DST2T3T3_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2) )

struct TextureStageState
{
	D3DTEXTUREOP ColorOp;
	DWORD ColorArg1;
	DWORD ColorArg2;
	DWORD ColorArg0;

	D3DTEXTUREOP AlphaOp;
	DWORD AlphaArg1;
	DWORD AlphaArg2;
	DWORD AlphaArg0;

	DWORD ResultTo;

	TextureID TexID; CycleID CycID;
	DWORD TexCoordIndex;
	struct 
	{
		D3DTEXTURESTAGESTATETYPE TSS;
		DWORD Value;
	} OtherTexStageStates[NUM_TEXSTAGE_STATES];
};

template < int _NumStages > struct TSSTestCaseTemplate
{
	TCHAR *pDescription;
	PrimitiveID PrimID;
	bool markedToRun;
	struct
	{
		D3DRENDERSTATETYPE RS;
		DWORD Value;
	} RenderStates[NUM_RENDER_STATES];

	TextureStageState Stage[_NumStages];
	
	static int GetNumStages() { return _NumStages; }
};

template < class _TestCaseClass > struct TSSTestCaseDataTemplate
{
	typedef _TestCaseClass TestCaseClass;
	
	_TestCaseClass TestCase;
	UINT uNumCycles;
	
	static int GetNumStages() { return _TestCaseClass::GetNumStages(); }
};

typedef TSSTestCaseTemplate< 1 > TSStestCaseOneStage;
typedef TSSTestCaseTemplate< 2 > TSStestCaseTwoStage;
typedef TSSTestCaseTemplate< 3 > TSStestCaseThreeStage;
typedef TSSTestCaseTemplate< 4 > TSStestCaseFourStage;
typedef TSSTestCaseTemplate< 5 > TSStestCaseFiveStage;
typedef TSSTestCaseTemplate< 6 > TSStestCaseSixStage;
typedef TSSTestCaseTemplate< 7 > TSStestCaseSevenStage;
typedef TSSTestCaseTemplate< 8 > TSStestCaseEightStage;

typedef TSSTestCaseDataTemplate< TSStestCaseOneStage > TSStestCaseOneStageData;
typedef TSSTestCaseDataTemplate< TSStestCaseTwoStage > TSStestCaseTwoStageData;
typedef TSSTestCaseDataTemplate< TSStestCaseThreeStage > TSStestCaseThreeStageData;
typedef TSSTestCaseDataTemplate< TSStestCaseFourStage > TSStestCaseFourStageData;
typedef TSSTestCaseDataTemplate< TSStestCaseFiveStage > TSStestCaseFiveStageData;
typedef TSSTestCaseDataTemplate< TSStestCaseSixStage > TSStestCaseSixStageData;
typedef TSSTestCaseDataTemplate< TSStestCaseSevenStage > TSStestCaseSevenStageData;
typedef TSSTestCaseDataTemplate< TSStestCaseEightStage > TSStestCaseEightStageData;
 
typedef struct _FORMAT_CYCLE
{
	UINT uNumFormats;
	FORMAT *pFormats;
} FORMAT_CYCLE;


extern TSStestCaseOneStage g_TestCasesOneStages[];
extern const UINT NUM_ONE_STAGES_TEST_CASES;

extern TSStestCaseTwoStage g_TestCasesTwoStages[];
extern const UINT NUM_TWO_STAGES_TEST_CASES;

extern TSStestCaseThreeStage g_TestCasesThreeStages[];
extern const UINT NUM_THREE_STAGES_TEST_CASES;

extern TSStestCaseFourStage g_TestCasesFourStages[];
extern const UINT NUM_FOUR_STAGES_TEST_CASES;

extern TSStestCaseFiveStage g_TestCasesFiveStages[];
extern const UINT NUM_FIVE_STAGES_TEST_CASES;

extern TSStestCaseSixStage g_TestCasesSixStages[];
extern const UINT NUM_SIX_STAGES_TEST_CASES;

extern TSStestCaseSevenStage g_TestCasesSevenStages[];
extern const UINT NUM_SEVEN_STAGES_TEST_CASES;

extern TSStestCaseEightStage g_TestCasesEightStages[];
extern const UINT NUM_EIGHT_STAGES_TEST_CASES;

class CBaseTSSTest : public CD3DTest
{
public:
    CBaseTSSTest();
    ~CBaseTSSTest();
    virtual UINT TestInitialize(void);
    virtual bool TestTerminate(void);
    virtual void CommandLineHelp(void);
    virtual bool ExecuteTest(UINT);
    virtual void SceneRefresh(void);
    virtual void UpdateStatus();
    virtual void ProcessArgs(void);
	bool PreModeSetup(void);
	
	virtual bool Setup();
	virtual void Cleanup();
	
	bool ExecuteTSStestCase( UINT uTestCaseIndex, UINT uFormatCycle );
	bool CreateTexturesForThisTestCase( UINT uTestCaseIndex, UINT uFormatCycle );
		
protected:
	virtual int GetNumStages() = 0;
	virtual TCHAR *GetTestCaseDescription( int caseIndex ) = 0;
	virtual UINT GetTestCaseNumCycles( int caseIndex ) = 0;
	virtual PrimitiveID GetTestCasePrimID( int caseIndex ) = 0;
	virtual void GetTestCaseRenderStateValue( int caseIndex, int index, D3DRENDERSTATETYPE &rs, DWORD &value ) = 0;
	virtual const TextureStageState &GetTestCaseStage( int caseIndex, int stageIndex ) = 0;

	//Caps and Options processor.
	virtual bool ReadAndFilterCapsBasedOnOptions();
	//Texture Image generation.
	virtual bool GenerateTextureImages();
    //virtual void ReleaseTexturesImages();

	//Draw primitive
	virtual bool DrawPrimitiveID(PrimitiveID primID);
	
	UINT GetTestCaseIndexToRun(UINT uiTest, UINT *puFormatCycle);

protected:

	WORD m_IndxTriList[TRI_LIST_INDX_COUNT];
	WORD m_IndxTriStrip[TRI_STRIP_INDX_COUNT];
	WORD m_IndxTriFan[TRI_FAN_INDX_COUNT];

	VERTEX_DST2 m_4VrtxDST2[4];

	VERTEX_DST2 m_4VrtxDST2_HalfDiffuse[4];

	VERTEX_DST2 m_4VrtxDST2_ZeroDiffuse[4];

	VERTEX_DST2 m_4VrtxDST2_DiffuseAlphaGradient[4];

	VERTEX_DST2T2 m_4VrtxDST2T2[4];

	VERTEX_DST2T2 m_4VrtxDST2T2half[4];

	VERTEX_DST2T3T3 m_4VrtxDST2T3T3_forVolCube[4];

	VERTEX_DST2T3T3 m_4VrtxDST2T3T3_forMipMap[4];
	
	FORMAT *m_pStageFormat[8];

protected:

	LPVERTEXBUFFERS m_pVB4VrtxDST2, m_pVB4VrtxDST2_HalfDiffuse, m_pVB4VrtxDST2_ZeroDiffuse, 
		m_pVB4VrtxDST2_DiffuseAlphaGradient, m_pVB4VrtxDST2T2, m_pVB4VrtxDST2T2half, 
		m_pVB4VrtxDST2T3T3_forVolCube, m_pVB4VrtxDST2T3T3_forMipMap;

	bool bPointFilter;

	FORMAT_CYCLE m_VariousTexFormatCycles[CYC_LAST_PLACEHOLDER];

	//Textures and images for it.
	CImage      m_TexImages[TEX_LAST_PLACEHOLDER_CIMAGE];
    CnBaseTexture   *m_pTextures[MAX_TEX_STAGES];

	UINT m_uTotalTestCycles;
	
	UINT m_uNumTestCases;
	UINT m_uIndexTestCase;
	UINT m_uFormatCycle;

	//hardware caps flags.
	DWORD m_dwPrimitiveMiscCaps;
	DWORD m_dwRasterCaps;
	DWORD m_dwTextureCaps;
	DWORD m_dwTextureOpCaps;
	DWORD m_dwSrcAlphaBlendCaps;
	DWORD m_dwTextureStageCaps;
	WORD m_wMaxSimultaneousTextures;
};



template < class _TestCaseDataClass, typename _TestCaseDataClass::TestCaseClass *_Data, const UINT *_pMaxNumTestCases > class CTSSTestTemplate : public CBaseTSSTest
{
	typedef _TestCaseDataClass TestCaseDataClass;
	typedef typename TestCaseDataClass::TestCaseClass TestCaseClass;
	
protected:
	TestCaseDataClass *m_pTestCases;
	
public:
	int GetNumStages()
		{
			return TestCaseDataClass::GetNumStages();
		}
		
	virtual TCHAR *GetTestCaseDescription( int caseIndex )
		{
			return m_pTestCases[caseIndex].TestCase.pDescription;
		}
		
	virtual UINT GetTestCaseNumCycles( int caseIndex )
		{
			return m_pTestCases[caseIndex].uNumCycles;
		}
		
	virtual PrimitiveID GetTestCasePrimID( int caseIndex )
		{
			return m_pTestCases[caseIndex].TestCase.PrimID;
		}
		
	virtual void GetTestCaseRenderStateValue( int caseIndex, int index, D3DRENDERSTATETYPE &rs, DWORD &value )
		{
			rs = m_pTestCases[caseIndex].TestCase.RenderStates[index].RS;
			value = m_pTestCases[caseIndex].TestCase.RenderStates[index].Value;
		}
		
	virtual const TextureStageState &GetTestCaseStage( int caseIndex, int stageIndex )
		{
			return m_pTestCases[caseIndex].TestCase.Stage[stageIndex];
		}
	
	CTSSTestTemplate( LPCTSTR szCommandKey )
		{
			m_szTestName = "TextureStage";
			m_szCommandKey = szCommandKey;
			
			if( TestCaseDataClass::GetNumStages() == 7 && KeySet( _T( "WHQL" ) ) )
				m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81;
		}
		
	UINT TestInitialize()
		{
			//member var initialization
			m_pTestCases = NULL;
			m_uNumTestCases = m_uIndexTestCase = m_uFormatCycle = 0;

			//set a default test range
			SetTestRange(1,1);

			//fast skip!
			if( m_pSrcDevice->GetCaps()->wMaxTextureBlendStages < GetNumStages() )
			{
				WriteToLog( _T("Card doesn't support %d tex stages.\n"), GetNumStages() );
				return D3DTESTINIT_SKIPALL;
			}

			////////Create texture format lists/////////////////////////////////////////
			UINT rval = CBaseTSSTest::TestInitialize();
			if( D3DTESTINIT_RUN != rval ) return rval;
			
			//Create a list of test cases that'll be run. Note: we can potentially
			// modify test cases based on caps before it goes into this list.
			// Example 1) if card can't do output to TEMP we can change that to output to current.
			//	2) if card can't support so many textures(MaxSimulTex) we can set the rest of
			//		tex to NULL and change inp arg to CURRENT.
			m_pTestCases = new TestCaseDataClass[*_pMaxNumTestCases];
			
			if( NULL == m_pTestCases )
			{
				WriteToLog( _T("\n%i stage test case allocation failed!"), GetNumStages() );
				return D3DTESTINIT_ABORT;
			}

			//Which tests can we execute? Filter TestCases Based On Caps And User Options.
			//Caps: D3DPMISCCAPS_PERSTAGECONSTANT, D3DPMISCCAPS_TSSARGTEMP,
			//		 CubeMap, VolumeMap, TextureOpCaps, MaxSimultaneousTextures.
			//Also try modifying certain test cases to fit the card caps. 
			//		 D3DPMISCCAPS_TSSARGTEMP, MaxSimultaneousTextures
			m_uNumTestCases = FilterTestCasesBasedOnCapsAndOptions( m_pTestCases );
			if( 0 == m_uNumTestCases )
			{
				WriteToLog(_T("\nNo test case included to run!") );
				return D3DTESTINIT_SKIPALL;
			}

			//compute num test cycles based on format cycles for each test case.
			m_uTotalTestCycles = ComputeTestCycles( m_pTestCases, m_uNumTestCases );

			SetTestRange( 1, m_uTotalTestCycles );

			return D3DTESTINIT_RUN;
		}
	
	bool TestTerminate()
		{
			bool rval=true;

			//Base class TestTerminate() first.
			////////Destroy texture format lists////
			////////Destroy VertexBuffers created.
			rval = CBaseTSSTest::TestTerminate();

			if( m_pTestCases )	//only if we reached this far in TestInitialize.
			{
				//Destroy the textures created for the last test. The last test index
				// is now in m_uIndexTestCase.
				for( int i=0; i < GetNumStages(); i++ )
				{
					if( m_pTextures[i] )
					{
						//release this texture.
						RELEASE( m_pTextures[i] );
					}
				}

				//Delete the selected test cases array.
				delete[] m_pTestCases;
				m_pTestCases = NULL;
			}

			return rval;
		}
				
protected:
	UINT FilterTestCasesBasedOnCapsAndOptions( TestCaseDataClass *pTestCases )
		{
			UINT uNumTestCases = 0;

			//Read hardware Caps.
			//Filter caps with options specified for test run.
			if( !ReadAndFilterCapsBasedOnOptions() )
			{
				return 0;
			}

			for( UINT i = 0; i < *_pMaxNumTestCases; i++ )
			{
				//copy first
				pTestCases[uNumTestCases].uNumCycles = 0;
				pTestCases[uNumTestCases].TestCase = _Data[i];

				//Filter based on Caps.
				//In certain cases only modify test-case if caps don't allow execution.
				if( !FilterTestCaseBasedOnCaps_ModifyIfPossible( &( pTestCases[uNumTestCases].TestCase ) ) )
				{
					//This test case is selected to run.
					uNumTestCases++;
				}
			}

			return uNumTestCases;
		}

	bool FilterTestCaseBasedOnCaps_ModifyIfPossible( TestCaseClass *pTestCase )
		{
			UINT numTextures = 0;

			int i;
			for(i = 0; i < GetNumStages(); i++ )
			{
				//Skip////////////////////////////////

				// D3DPMISCCAPS_PERSTAGECONSTANT caps
				if( ( pTestCase->Stage[i].ColorArg1 == D3DTA_CONSTANT ) ||
					( pTestCase->Stage[i].ColorArg2 == D3DTA_CONSTANT ) ||
					( pTestCase->Stage[i].ColorArg0 == D3DTA_CONSTANT ) ||
					( pTestCase->Stage[i].AlphaArg1 == D3DTA_CONSTANT ) ||
					( pTestCase->Stage[i].AlphaArg2 == D3DTA_CONSTANT ) ||
					( pTestCase->Stage[i].AlphaArg0 == D3DTA_CONSTANT ) )
				{
					if( (m_dwVersion < 0x0900) || !( m_dwPrimitiveMiscCaps & D3DPMISCCAPS_PERSTAGECONSTANT ) )
					{
						//doesn't support input from per stage const.
						//Skip this test case.
						return true;
					}
				}

				//Skip////////////////////////////////

				// D3DPTEXTURECAPS_MIPMAP caps
				if( pTestCase->Stage[i].TexID == TEX_MIPS )
				{
					if( !( m_dwTextureCaps & D3DPTEXTURECAPS_MIPMAP ) )
					{
						//doesn't support mipmap textures.
						//Skip this test case.
						return true;
					}

				}

				//Skip////////////////////////////////

				// D3DPTEXTURECAPS_CUBEMAP caps
				if( pTestCase->Stage[i].TexID == TEX_CUBE )
				{
					if( !( m_dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP ) )
					{
						//doesn't support cubemap textures.
						//Skip this test case.
						return true;
					}
				}
			
				//Skip////////////////////////////////

				// D3DPTEXTURECAPS_VOLUMEMAP caps
				if( pTestCase->Stage[i].TexID == TEX_VOLUME )
				{
					if( !( m_dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP ) )
					{
						//doesn't support volume textures.
						//Skip this test case.
						return true;
					}
				}
			

				//Skip////////////////////////////////

				// dwTextureOpCaps 
				if( !( m_dwTextureOpCaps & (DWORD)( 1 << ( pTestCase->Stage[i].ColorOp - 1 ) ) ) )
				{
					//doesn't support color op.
					//Skip this test case.
					return true;
				}
				
				if( !( m_dwTextureOpCaps & (DWORD)( 1 << ( pTestCase->Stage[i].AlphaOp - 1 ) ) ) )
				{
					//doesn't support alpha op.
					//Skip this test case.
					return true;
				}


				//Modify ////////////////////////////////

				// D3DPRASTERCAPS_MIPMAPLODBIAS caps
				for( int j = 0; j < NUM_TEXSTAGE_STATES; j++ )
				{
					if( pTestCase->Stage[i].OtherTexStageStates[j].TSS == D3DTSS_MIPMAPLODBIAS )
					{
						if( !( m_dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS ) )
						{
							// MipMapLODBias not support, don't use it
							pTestCase->Stage[i].OtherTexStageStates[j].TSS = (enum _D3DTEXTURESTAGESTATETYPE)0;
						}
					}
				}


				//Modify////////////////////////////////
				// D3DTSCAPS_SPECULAR. This is a caps local to this test. It is added because 
				//	intel i81x hardware can't do D3DTA_SPECULAR.
				if( !( m_dwTextureStageCaps & D3DTSCAPS_SPECULAR ) )
				{
					//doesn't support input from specular.
					//Modify input to come from diffuse.
					if( pTestCase->Stage[i].ColorArg1 == D3DTA_SPECULAR ) pTestCase->Stage[i].ColorArg1 = D3DTA_DIFFUSE;
					if( pTestCase->Stage[i].ColorArg2 == D3DTA_SPECULAR ) pTestCase->Stage[i].ColorArg2 = D3DTA_DIFFUSE;
					if( pTestCase->Stage[i].ColorArg0 == D3DTA_SPECULAR ) pTestCase->Stage[i].ColorArg0 = D3DTA_DIFFUSE;
																		
					if( pTestCase->Stage[i].AlphaArg1 == D3DTA_SPECULAR ) pTestCase->Stage[i].AlphaArg1 = D3DTA_DIFFUSE;
					if( pTestCase->Stage[i].AlphaArg2 == D3DTA_SPECULAR ) pTestCase->Stage[i].AlphaArg2 = D3DTA_DIFFUSE;
					if( pTestCase->Stage[i].AlphaArg0 == D3DTA_SPECULAR ) pTestCase->Stage[i].AlphaArg0 = D3DTA_DIFFUSE;
				}

				//Modify////////////////////////////////
				// D3DTSCAPS_COMPLEMENT. This is a caps local to this test. It is added because 
				//	d3d doesn't have a comparable caps and only dx8 level hardware can do D3DTA_COMPLEMENT.
				if( !( m_dwTextureStageCaps & D3DTSCAPS_COMPLEMENT ) )
				{
					//doesn't support input modification complement.
					//Clear input modifier complement.
					if( pTestCase->Stage[i].ColorArg1 & D3DTA_COMPLEMENT ) pTestCase->Stage[i].ColorArg1 &= (~D3DTA_COMPLEMENT);
					if( pTestCase->Stage[i].ColorArg2 & D3DTA_COMPLEMENT ) pTestCase->Stage[i].ColorArg2 &= (~D3DTA_COMPLEMENT);
					if( pTestCase->Stage[i].ColorArg0 & D3DTA_COMPLEMENT ) pTestCase->Stage[i].ColorArg0 &= (~D3DTA_COMPLEMENT);
																		
					if( pTestCase->Stage[i].AlphaArg1 & D3DTA_COMPLEMENT ) pTestCase->Stage[i].AlphaArg1 &= (~D3DTA_COMPLEMENT);
					if( pTestCase->Stage[i].AlphaArg2 & D3DTA_COMPLEMENT ) pTestCase->Stage[i].AlphaArg2 &= (~D3DTA_COMPLEMENT);
					if( pTestCase->Stage[i].AlphaArg0 & D3DTA_COMPLEMENT ) pTestCase->Stage[i].AlphaArg0 &= (~D3DTA_COMPLEMENT);
				}

				//Modify////////////////////////////////
				// D3DTSCAPS_ALPHAREPLICATE. This is a caps local to this test. It is added because 
				//	d3d doesn't have a comparable caps and only dx8 level hardware can do D3DTA_ALPHAREPLICATE.
				if (!(m_dwTextureStageCaps & D3DTSCAPS_ALPHAREPLICATE))
				{
					//doesn't support input modification alpha-replicate.
					//Clear input modifier alpha-replicate.
					if( pTestCase->Stage[i].ColorArg1 & D3DTA_ALPHAREPLICATE ) pTestCase->Stage[i].ColorArg1 &= (~D3DTA_ALPHAREPLICATE);
					if( pTestCase->Stage[i].ColorArg2 & D3DTA_ALPHAREPLICATE ) pTestCase->Stage[i].ColorArg2 &= (~D3DTA_ALPHAREPLICATE);
					if( pTestCase->Stage[i].ColorArg0 & D3DTA_ALPHAREPLICATE ) pTestCase->Stage[i].ColorArg0 &= (~D3DTA_ALPHAREPLICATE);
																			
					if( pTestCase->Stage[i].AlphaArg1 & D3DTA_ALPHAREPLICATE ) pTestCase->Stage[i].AlphaArg1 &= (~D3DTA_ALPHAREPLICATE);
					if( pTestCase->Stage[i].AlphaArg2 & D3DTA_ALPHAREPLICATE ) pTestCase->Stage[i].AlphaArg2 &= (~D3DTA_ALPHAREPLICATE);
					if( pTestCase->Stage[i].AlphaArg0 & D3DTA_ALPHAREPLICATE ) pTestCase->Stage[i].AlphaArg0 &= (~D3DTA_ALPHAREPLICATE);
				}

				//Modify////////////////////////////////
				// D3DPMISCCAPS_TSSARGTEMP caps
				if( pTestCase->Stage[i].ResultTo == D3DTA_TEMP )
				{
					if( !( m_dwPrimitiveMiscCaps & D3DPMISCCAPS_TSSARGTEMP ) )
					{
						//doesn't support output to temp
						pTestCase->Stage[i].ResultTo = D3DTA_CURRENT;

						//Now check if the stage that follow have arg D3DTA_TEMP
						if( i < GetNumStages() - 1 )
						{
							for( int j = i + 1; j < GetNumStages(); j++ )
							{
								if( pTestCase->Stage[j].ColorArg1 == D3DTA_TEMP )
									pTestCase->Stage[j].ColorArg1 = D3DTA_CURRENT;
								if( pTestCase->Stage[j].ColorArg2 == D3DTA_TEMP )
									pTestCase->Stage[j].ColorArg2 = D3DTA_CURRENT;
								if( pTestCase->Stage[j].ColorArg0 == D3DTA_TEMP )
									pTestCase->Stage[j].ColorArg0 = D3DTA_CURRENT;

								if( pTestCase->Stage[j].AlphaArg1 == D3DTA_TEMP )
									pTestCase->Stage[j].AlphaArg1 = D3DTA_CURRENT;
								if( pTestCase->Stage[j].AlphaArg2 == D3DTA_TEMP )
									pTestCase->Stage[j].AlphaArg2 = D3DTA_CURRENT;
								if( pTestCase->Stage[j].AlphaArg0 == D3DTA_TEMP )
									pTestCase->Stage[j].AlphaArg0 = D3DTA_CURRENT;

							}
						}
					}
				}

				//////////////////////////////////

				//count Simultaneous Textures in this test case
				if( pTestCase->Stage[i].TexID != TEX_NULL )
					numTextures++;

				//////////////////////////////////

			} //for loop end

			//stage loop end/////////////////////////////////////////////////////////


			//Modify////////////////////////////////

			// dwMaxSimultaneousTextures caps
			if( m_wMaxSimultaneousTextures < numTextures )
			{
				//card can't support so many textures
				i = 0;
				while( (m_wMaxSimultaneousTextures < numTextures) && ( i < GetNumStages() ) )
				{
					if( pTestCase->Stage[i].TexID != TEX_NULL )
					{
						pTestCase->Stage[i].TexID = TEX_NULL;
						numTextures--;

						//change the current stage not to input from texture
						if( pTestCase->Stage[i].ColorArg1 == D3DTA_TEXTURE )
							pTestCase->Stage[i].ColorArg1 = D3DTA_CURRENT;
						if( pTestCase->Stage[i].ColorArg2 == D3DTA_TEXTURE )
							pTestCase->Stage[i].ColorArg2 = D3DTA_CURRENT;
						if( pTestCase->Stage[i].ColorArg0 == D3DTA_TEXTURE )
							pTestCase->Stage[i].ColorArg0 = D3DTA_CURRENT;

						if( pTestCase->Stage[i].AlphaArg1 == D3DTA_TEXTURE )
							pTestCase->Stage[i].AlphaArg1 = D3DTA_CURRENT;
						if( pTestCase->Stage[i].AlphaArg2 == D3DTA_TEXTURE )
							pTestCase->Stage[i].AlphaArg2 = D3DTA_CURRENT;
						if( pTestCase->Stage[i].AlphaArg0 == D3DTA_TEXTURE )
							pTestCase->Stage[i].AlphaArg0 = D3DTA_CURRENT;


						//change subsequent CYC_PREVIOUS_STAGE_DEPENDENT if any
						// BUGBUG: ??? i + 1, k ???
						int j, k;
						for(j = i + 1; j < GetNumStages(); j++ )
						{
							if( pTestCase->Stage[j].CycID == CYC_DONT_CARE )
							{
								continue;
							}
							else if( pTestCase->Stage[j].CycID == CYC_PREVIOUS_STAGE_DEPENDENT )
							{
								pTestCase->Stage[j].CycID = pTestCase->Stage[i].CycID;
								break;
							}
							else
							{
								//we found a non-dependent cycle.
								break;
							}
						}

						//change subsequent CYC_*THIS*_STAGE_ABOVE_DEPENDENT if any.
						//change subsequent TEX_*THIS*_STAGE_ABOVE if any.
						for( j = i + 1, k = 0; j < GetNumStages(); j++, k++ )
						{
							//change subsequent CYC_*THIS*_STAGE_ABOVE_DEPENDENT if any.
							if( pTestCase->Stage[j].CycID == ( CYC_ONE_STAGE_ABOVE_DEPENDENT + k ) )
							{
								pTestCase->Stage[j].CycID = pTestCase->Stage[i].CycID;
							}

							//change subsequent TEX_*THIS*_STAGE_ABOVE if any.
							if( pTestCase->Stage[j].TexID == (TEX_ONE_STAGE_ABOVE + k ) )
							{
								pTestCase->Stage[j].TexID = TEX_NULL;
								numTextures--;

								//change the current stage not to input from texture
								if( pTestCase->Stage[j].ColorArg1 == D3DTA_TEXTURE )
									pTestCase->Stage[j].ColorArg1 = D3DTA_CURRENT;
								if( pTestCase->Stage[j].ColorArg2 == D3DTA_TEXTURE )
									pTestCase->Stage[j].ColorArg2 = D3DTA_CURRENT;
								if( pTestCase->Stage[j].ColorArg0 == D3DTA_TEXTURE )
									pTestCase->Stage[j].ColorArg0 = D3DTA_CURRENT;

								if( pTestCase->Stage[j].AlphaArg1 == D3DTA_TEXTURE )
									pTestCase->Stage[j].AlphaArg1 = D3DTA_CURRENT;
								if( pTestCase->Stage[j].AlphaArg2 == D3DTA_TEXTURE )
									pTestCase->Stage[j].AlphaArg2 = D3DTA_CURRENT;
								if( pTestCase->Stage[j].AlphaArg0 == D3DTA_TEXTURE )
									pTestCase->Stage[j].AlphaArg0 = D3DTA_CURRENT;
							}
						}
					}

					i++;
				}
			}

			//////////////////////////////////

			//This test case cannot be filtered out.
			return false;
		}
		
	UINT ComputeTestCycles( TestCaseDataClass *pTestCases, UINT uNumTestCases )
		{
			UINT uTotalTestCycles=0;

			for (UINT i=0; i<uNumTestCases; i++)
			{
				UINT uThisTestCaseCycles=1;
				for( int j = 0; j < GetNumStages(); j++)
				{
					if( ( pTestCases[i].TestCase.Stage[j].TexID != TEX_NULL ) &&
						( pTestCases[i].TestCase.Stage[j].TexID != TEX_PREVIOUS_STAGE ) && 
						( pTestCases[i].TestCase.Stage[j].TexID != TEX_ONE_STAGE_ABOVE ) && 
						( pTestCases[i].TestCase.Stage[j].TexID != TEX_TWO_STAGE_ABOVE ) && 
						( pTestCases[i].TestCase.Stage[j].TexID != TEX_THREE_STAGE_ABOVE ) && 
						( pTestCases[i].TestCase.Stage[j].TexID != TEX_FOUR_STAGE_ABOVE ) && 
						( pTestCases[i].TestCase.Stage[j].TexID != TEX_FIVE_STAGE_ABOVE ) && 
						( pTestCases[i].TestCase.Stage[j].TexID != TEX_SIX_STAGE_ABOVE ) && 
						( pTestCases[i].TestCase.Stage[j].TexID != TEX_SEVEN_STAGE_ABOVE ) && 
						( pTestCases[i].TestCase.Stage[j].CycID != CYC_DONT_CARE )
						)
					{
						uThisTestCaseCycles = uThisTestCaseCycles * ( m_VariousTexFormatCycles[pTestCases[i].TestCase.Stage[j].CycID].uNumFormats );
					}
				}
				pTestCases[i].uNumCycles = uThisTestCaseCycles;

				uTotalTestCycles = uTotalTestCycles + pTestCases[i].uNumCycles;
			}

			return uTotalTestCycles;
		}
};	
	

typedef CTSSTestTemplate< TSStestCaseOneStageData, g_TestCasesOneStages, &NUM_ONE_STAGES_TEST_CASES > CTSSTestOneStage;
typedef CTSSTestTemplate< TSStestCaseTwoStageData, g_TestCasesTwoStages, &NUM_TWO_STAGES_TEST_CASES > CTSSTestTwoStage;
typedef CTSSTestTemplate< TSStestCaseThreeStageData, g_TestCasesThreeStages, &NUM_THREE_STAGES_TEST_CASES > CTSSTestThreeStage;
typedef CTSSTestTemplate< TSStestCaseFourStageData, g_TestCasesFourStages, &NUM_FOUR_STAGES_TEST_CASES > CTSSTestFourStage;
typedef CTSSTestTemplate< TSStestCaseFiveStageData, g_TestCasesFiveStages, &NUM_FIVE_STAGES_TEST_CASES > CTSSTestFiveStage;
typedef CTSSTestTemplate< TSStestCaseSixStageData, g_TestCasesSixStages, &NUM_SIX_STAGES_TEST_CASES > CTSSTestSixStage;
typedef CTSSTestTemplate< TSStestCaseSevenStageData, g_TestCasesSevenStages, &NUM_SEVEN_STAGES_TEST_CASES > CTSSTestSevenStage;
typedef CTSSTestTemplate< TSStestCaseEightStageData, g_TestCasesEightStages, &NUM_EIGHT_STAGES_TEST_CASES > CTSSTestEightStage;



#endif // _TEXSTAGE_EFFICIENT_H_
