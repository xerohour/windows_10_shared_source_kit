// FILE:        pshader.h
// DESC:        pixel shader class header for pixel shader conformance tests
// AUTHOR:      
// Modified:	Ver2.0 added  Bobby Paulose

#ifndef _PSHADER_H_
#define _PSHADER_H_

#include "CD3D.h"
#include "CD3DTest.h"
#include "CBuffers.h"
#include "CFlags.h"
#include "CTextures.h"
#include <vector>

#define GLE(x) (m_pD3D->HResultToString(x))
#define PSVERSION(a, b) ((DWORD)D3DPS_VERSION((a), (b)))
#define VSVERSION(a, b) ((DWORD)D3DVS_VERSION((a), (b)))
#define MAGIC_SHADER PSVERSION(255,255)		//Pixel shader version 255.255
#define LINE_LENGTH 80						//Shader line length
#define BUFFER_LENGTH 256					//Buffer size used for shader reads and str buffer
#define MAX_CODES 1024*4					//Max shader codes
#define DEFAULT_TEXSIZE 64					//Default texture size used.
#define LARGE_TEXSIZE 256					//Large textures
#define XLARGE_TEXSIZE 512					//Extra large textures
#define DEFAULT_BUMPMATIXSTAGE	1			//This is the stage that the bumpenv matricies are set on.
#define DEFAULT_PROJSTAGE		0			//This is the stage that the D3DTTFF_PROJECTED flag is set on for the q coord.
#define DEFAULT_VSSTREAM		1			//This is the default data stream used for programmable vertex shaders
#define VOLUME_DEPTH    4					//The default depth for a volume texture
#define MIP_LEVELS	4						//Miplevels for the mipmapped texture
#define MAXTEXSTAGE 8						//Maximum number of texture stages
#define MAXTEXSAMPLER 16					//Maximum number of texture samplers

//Possible texture format flags
#define TEXFORMAT	    0x01
#define BUMPFORMAT		0x02
#define CUBEFORMAT		0x04
#define VOLUMEFORMAT	0x08

//Possible shader flags
#define SH_INVALID          0x01    //This means that the shader is an invalid param test.
#define SH_INVALIDFORMAT    0x02    //Invalid texture format combination for this shader
									//And will be run as an invalid shader in normal test runs.
//Possible flags
#define HWTNL			0x00000001  //Set if the device supports Hardware vertex processing.
#define VBHARDWARE		0x00000002  //Set if vertex buffers should be created in hardware.
#define VOLUMECAP       0x00000004  //Set if the device can do volume textures.
#define PROJECTED       0x00000008  //Set if D3DTTFF_PROJECTED can be used
#define NOCHECKVER      0x00000010  //Set if not testing for supported versions beyond this kit.
#define HWVERTSHADER    0x00000020  //Set if vertex shaders are supported in hardware.
#define SETDEFSTAGES    0x00000040  //Set if vertex shader should set defaults for all texture stages that exist.
									//Default position is also set.
#define SETDEFMAXSTAGES 0x00000080  //Set if vertex shader should set defaults for all possible texture stages.
									//Default position is also set.
#define SETSTAGES       0x00000100  //Set if only texture co-ordinates should be set for stages that exist.

const float DEFAULT_Du = 0.04f;	
const float DEFAULT_Dv = 0.04f;
const float DEFAULT_LSCALE = 0.7f;
const float DEFAULT_LOFFSET = 0.2f;

//This is an offset applied to TL gemoetry in order to align texture and gemetry 0.
const float OFFSET_FOR_TEXTURE = 0.5f;

enum ShaderType 
{
	SHADE_NOTYPE,
	SHADE_PLAIN,
	SHADE_PLAIN_NEEDMIPMAP,
	SHADE_BUMP,
	SHADE_NEEDZ,
	SHADE_PROJ,
	SHADE_LBUMP,
	SHADE_NOLORA    //No luminance or Alpha formats should be tested.
};

enum VertexShaderType
{
	VST_RENDERPRIM,
	VST_FVF,
	VST_PROG
};

#ifndef D3DRGBA
	#define D3DRGBA(r, g, b, a) \
		((D3DCOLOR) \
			(   (((long)((a) * 255)) << 24) | (((long)((r) * 255)) << 16) \
			|   (((long)((g) * 255)) << 8) | (long)((b) * 255) \
			))
#endif


#define BUMPRGBA(r, g, b, a) \
		((D3DCOLOR) \
			(   (((long)((a) & 0x0FF)) << 24) | (((long)((r) & 0x0FF)) << 16) \
			|   (((long)((g) & 0x0FF)) << 8) | (long)((b) & 0x0FF) \
			))

struct MYTLVERTEX 
{
	float x, y, z, w;
	DWORD a, b;
	float u, v;
};
#define MYTLVERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0))

struct MYTLVERTEXTEX2 
{
	float x, y, z, w;
	DWORD a, b;
	float u0, v0;
	float u1, v1;
};
#define MYTLVERTEXTEX2_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1))

struct MYTLVERTEXTEX4 
{
	float x, y, z, w;
	DWORD a, b;
	float u0, v0;
	float u1, v1;
	float u2, v2;
	float u3, v3;
};
#define MYTLVERTEXTEX4_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3))

struct MYTLVERTEXTEX6 
{
	float x, y, z, w;
	DWORD a, b;
	float u0, v0;
	float u1, v1;
	float u2, v2;
	float u3, v3;
	float u4, v4;
	float u5, v5;
};
#define MYTLVERTEXTEX6_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX6 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3) | D3DFVF_TEXCOORDSIZE2(4) | D3DFVF_TEXCOORDSIZE2(5))

struct MYTLVERTEXTEX8 
{
	float x, y, z, w;
	DWORD a, b;
	float u0, v0;
	float u1, v1;
	float u2, v2;
	float u3, v3;
	float u4, v4;
	float u5, v5;
	float u6, v6;
	float u7, v7;
};
#define MYTLVERTEXTEX8_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX8 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3) | D3DFVF_TEXCOORDSIZE2(4) | D3DFVF_TEXCOORDSIZE2(5) | D3DFVF_TEXCOORDSIZE2(6) | D3DFVF_TEXCOORDSIZE2(7))

struct MYCUBETLVERTEX
{
	float x, y, z, w;
	DWORD a, b;
	float u0, v0, w0;
	float u1, v1, w1;
	float u2, v2, w2; 
	float u3, v3, w3; 
};
#define MYCUBETLVERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2) | D3DFVF_TEXCOORDSIZE3(3))

typedef struct _MYCUBETLVERTEX2
{
	float x, y, z, w;
	DWORD a, b;
	float u0, v0, w0;
	float u1, v1, w1;
	float u2, v2, w2; 
} MYCUBETLVERTEX2, * PMYCUBETLVERTEX2;
#define MYCUBETLVERTEX2_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2))

typedef struct _MYCUBETLVERTEX3
{
	float x, y, z, w;
	DWORD a, b;
	float u0, v0;
	float u1, v1, w1, q1; 
	float u2, v2, w2, q2; 
	float u3, v3, w3, q3; 
} MYCUBETLVERTEX3, * PMYCUBETLVERTEX3;
#define MYCUBETLVERTEX3_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE4(1) | D3DFVF_TEXCOORDSIZE4(2) | D3DFVF_TEXCOORDSIZE4(3))

typedef struct _MYTLVERTEX3
{
	float x, y, z, w;
	DWORD a, b;
	float u0, v0, w0, q0;
	float u1, v1, w1; 
	float u2, v2, w2; 
	float u3, v3, w3;
	float u4, v4; 
	float u5, v5, w5; 
} MYTLVERTEX3, * PMYTLVERTEX3;
#define MYTLVERTEX3_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX6 | D3DFVF_TEXCOORDSIZE4(0) | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2) | D3DFVF_TEXCOORDSIZE3(3) | D3DFVF_TEXCOORDSIZE2(4) | D3DFVF_TEXCOORDSIZE3(5))

struct MYTLVERTEXPERSP
{
	float x, y, z, w;
	DWORD a, b;
	float u, v, n, m;
};
#define MYTLVERTEXPERSP_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE4(0))

typedef struct _MYVOLTLVERTEX
{
	float x, y, z, w;
	DWORD a, b;
	float u0, v0, w0;
} MYVOLTLVERTEX, * PMYVOLTLVERTEX;
#define MYVOLTLVERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0) )


const UINT DEFAULT_BUMPS = 6;  // default number of bumps
typedef std::vector<CnBaseTexture **> TEXTUREVECTOR;

typedef struct
{
	TCHAR *pName;                   // pixel shader name
	ShaderType Type;	 	        // pixel shader type this is used how to setup TSS and what ever else is needed.
	TCHAR *pCode;                   // pixel shader code (ASCII)
	DWORD dwHandle;                 // pixel shader handle
	DWORD dwVSHandle;               // vertex shader handle
	std::vector<float> vConstants;  // constant vector
	DWORD dwConstantsStart;         // constant start
	DWORD dwRequired;               // base pixel shader version
	CnVertexBuffer *pVBs;           // vertex buffers
	DWORD dwVertexFormat;           // vertex format
	DWORD dwVertexFormatSize;       // vertex format size
	TEXTUREVECTOR vTextures;        // texture vector
	UINT rgnCoordsForStage[D3DDP_MAXTEXCOORD]; //Specifies the texture coordinates to use at stage/index of the array
	void *pVertices;
	bool bAlpha;                   // alpha blend flag
	bool bWrapAndMip;			   // flag to tell if this shader should be checked for wrap and mip query
	bool bSetup;                   // setup flag
	UINT uBumpStage;				//Stage to have bump matrix set on.
	VertexShaderType vstPrimType;	//Pipe used to render vertices.
	FMT d3dfTexFormat, d3dfBumpFormat;    //These are the formats that the shaders textures are using
	FMT d3dfCubeFormat, d3dfVolFormat;    //These are the formats that the shaders textures are using
	BYTE byTexFormatsUsed;
	CFlags Flags;                    //Used for any flags that need to be set on a per shader basis
} MYSHADER, * PMYSHADER;

typedef std::vector<MYSHADER *> SHADERVECTOR;

class CPixelShader : public CD3DTest
{
	public:
		CPixelShader();
		virtual UINT TestInitialize(void);
		virtual bool TestTerminate(void);
		virtual void CommandLineHelp(void);
		virtual bool ExecuteTest(UINT);
		virtual void SceneRefresh(void);
		virtual void ProcessArgs(void);
		bool PreModeSetup(void);
		
	protected:
		bool DoCapsCheck();

		//Texture generation for shaders.
		virtual bool GenerateTextureData();
		virtual bool LoadTextures();
		virtual void ReleaseTextures();
		virtual void ReleaseTextureData();
		
		virtual HRESULT PopulateShaderInfo() = 0;

		void AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices);
		void AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, DWORD dwRequired);
		void AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, bool bAlpha);
		void AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, ShaderType Type, DWORD dwRequired);
		void AddShaderDef(TCHAR *pName, char *pCode, DWORD dwFVF, void *pVertices, bool bAlpha, ShaderType Type, DWORD dwRequired);		

		void AddTextureToLastShaderDef(CnBaseTexture **ppTexture);
		void AddTextureToLastShaderDef(CnBaseTexture **ppTexture, UINT uiStage);
		void SetLastShaderWrapAndMip(void);
		void SetCoordsForLastPSStages(DWORD dwStage, DWORD dwCoordNum);
		void SetLastShaderTexFormats(BYTE byFormats);
		void SetLastShaderFlags(DWORD dwFlags);
		void SetConstantsStart(DWORD dwStart);
		void AddConstantsToLastShaderDef(float a, float b, float c, float d);
		void AddConstantsToLastShaderDef(float f[4]);
		void AddConstantsToLastShaderDef(float *f, DWORD dwCount);
		void AddBumpStageForLastShaderDef(UINT uStageNum);

		bool GenerateBrickTexture(CImage *pImage);
		bool GenerateBumpMap(CImage* pImage);

		DWORD m_dwMinPSVersion, m_dwRequestedVersion;
		int m_nTexFormat, m_nBumpFormat, m_nCubeFormat, m_nVolFormat;
		UINT m_uPSCommonBumpFormats, m_uPSCommonTexFormats, m_uPSCommonCubeFormats;
		UINT m_uPSCommonVolFormats, m_uTotalFormats, m_uNumShaderVersions;
		bool SetTextureFormat(BYTE byFormat = 0);		
		void CleanUpReg(void);
		void ReCreateShaderAndBuffers(MYSHADER * pShader);

	private:
		bool SetupBasicScene();
		bool CreateLoadedVertexBuffers(MYSHADER * pShader);
		bool m_bSupports255, m_bSpecificVerRequest;
		DWORD m_dwTested255;
		UINT m_uiLastTest, m_uiShaderNotRendered;
		bool m_bFormatChanged;    //Has any format changed since last time textures were loaded
		bool m_bShaderAdded;	//This is a flag to say if the last shader was added or not 
								//It prevents the need for if statements around all the AddShaderDef calls.
		bool CreateShaderVersionArray();
		void ResetAllShaderTexFormats();
		void ResetShaderTexFormats(MYSHADER * pShader);
		bool NeedPalette(UINT uNumFormats, FORMAT *pFormats);
		static FLOAT fBumpDu(FLOAT u, FLOAT v);
		static FLOAT fBumpDv(FLOAT u, FLOAT v);
		static FLOAT fBumpLuminance(FLOAT u, FLOAT v);		
		static FLOAT fSin(FLOAT u, FLOAT v);
		void DeleteShaderMemory(MYSHADER * pShader);

	protected:
		bool            m_bLastForward;
		SHADERVECTOR	m_vShaders;
		DWORD			m_dwSupported;
		DWORD			m_dwCurrentShader;
		DWORD			m_dwCurrentVersion;
		BYTE			m_byFormats;			
		std::vector<DWORD> m_vShaderVersions;
		CFlags			m_Flags;
		int m_nDisplaySizePow2;

		FORMAT *m_pTexFormats, *m_pBumpFormats;

		//Textures for shaders.
		CImage      m_DXLogoImage;
		CImage      m_CheckerImage;
		CImage      m_ColorCheckerImage;
		CImage      m_BrickImage;

		CnTexture   *m_pDXLogoTex;
		CnTexture   *m_pCheckerTex;
		CnTexture   *m_pColorCheckerTex;
		CnTexture   *m_pBrickTex;
};

class CPSOps : public CPixelShader
{
	public:
		CPSOps();
		UINT TestInitialize(void);

	protected:

		bool GenerateTextureData();
		bool LoadTextures();
		void ReleaseTextures();
	
		HRESULT PopulateShaderInfo();

		bool		m_bBump;

		CImage      m_Checker2Image;
		CImage      m_DP3Image;
		CImage      m_GradientImage;
		CImage      m_StripesImage;
		CImage      m_SineBumpImage;

		CnTexture   *m_pChecker2Tex;        
		CnTexture   *m_pDP3Tex;        
		CnTexture   *m_pGradientTex;		
		CnTexture   *m_pStripesTex;		
		CnTexture   *m_pSineBumpTex;
};

class CPSOps_Bump : public CPSOps
{
	public:
		CPSOps_Bump();

	protected:
		HRESULT PopulateShaderInfo();
};

class CPSRegister : public CPixelShader
{
	public:
		CPSRegister();
		UINT TestInitialize(void);

	protected:
		HRESULT PopulateShaderInfo();
};

class CPSSrcMod : public CPixelShader
{
	public:
		CPSSrcMod();
		UINT TestInitialize(void);

	protected:

   		bool GenerateTextureData();
		bool LoadTextures();
		void ReleaseTextures();
	
		HRESULT PopulateShaderInfo();


		CImage m_DP3Image;
		CImage m_GradientImage;

		CnTexture *m_pDP3Tex;
		CnTexture *m_pGradientTex;
};

class CPSDestMod : public CPixelShader
{
	public:
		CPSDestMod();
		UINT TestInitialize(void);

	protected:
		HRESULT PopulateShaderInfo();
};

class CPSAddress : public CPixelShader
{
	public:

		CPSAddress();
		UINT TestInitialize(void);

	protected:
               
		bool GenerateTextureData();
		bool LoadTextures();
		void ReleaseTextures();
		void ReleaseTextureData();
	
		HRESULT PopulateShaderInfo();
		HRESULT PopulateShaderInfo_Bump();
		HRESULT PopulateShaderInfo_Cube();
		HRESULT PopulateShaderInfo_Vol();
		HRESULT PopulateShaderInfo_BumpCube();

		bool		m_bBump;
		bool		m_bCube;
		bool		m_bVol;

		CImage      m_StripesVecBx2Image;
		CImage		m_VecSomeBx2Image;
		CImage		m_StripesImage;
		CImage		m_SineBumpImage;
		CImage		m_AddStripesImage;
		CImage      m_GradientImage;

		CImage      *m_pCubeImage[6];

		CImage      *m_pMipImage[MIP_LEVELS];
		
		CImage      *m_pVolImage[VOLUME_DEPTH];

		CnTexture	    *m_pStripesVecBx2;
		CnTexture	    *m_pVecSomeBx2;        
		CnTexture		*m_pStripesTex;
		CnTexture		*m_pSineBumpTex;		
		CnTexture		*m_pAddStripesTex;		
		CnTexture       *m_pGradientTex;		
		CnTexture       *m_pMipsTex;
		CnCubeTexture	*m_pCubeTex;
		CnVolTexture    *m_pVolTex;
};

class CPSAddress_Bump : public CPSAddress
{
	public:
		CPSAddress_Bump();
};

class CPSAddress_Cube : public CPSAddress
{
	public:
		CPSAddress_Cube();
};

class CPSAddress_Vol : public CPSAddress
{
	public:
		CPSAddress_Vol();
};

class CPSAddress_BumpCube : public CPSAddress
{
	public:
		CPSAddress_BumpCube();
};

class CReadShader : public CPixelShader
{
	public:
		CReadShader();
		~CReadShader();

		UINT TestInitialize(void);
		bool ExecuteTest(UINT);
		bool TestTerminate(void);
		void CommandLineHelp(void);		
		void ProcessArgs(void);
	
	protected:
		bool GenerateTextureData();
		bool LoadTextures();
		void ReleaseTextures();
	
		HRESULT PopulateShaderInfo();

	private:		

		HRESULT AddShader(TCHAR *sztType, TCHAR *sztName, TCHAR *sztCodes, DWORD dwVersion);
		TCHAR m_sztFilename[BUFFER_LENGTH];

		CImage	m_StripesImage;
		CImage	m_SineBumpImage;
		CImage	m_GradientImage;
        
		CnTexture	*m_pStripesTex;
		CnTexture	*m_pSineBumpTex;
		CnTexture	*m_pGradientTex;
};

class CPSVertShader : public CPSAddress
{
	public:
		CPSVertShader();
		UINT TestInitialize(void);
		void ProcessArgs(void);
		void CommandLineHelp(void);
	private:
		void ChangeVertexShaderType();
		bool GetVertexShaderCode(MYSHADER * pShader, TCHAR *sztCode, DWORD dwSize, DWORD *rgdwDeclarFromFVF);
		HRESULT DeclaratorFromFVF(DWORD dwFVF, DWORD Declarator[MAX_FVF_DECL_SIZE]);
};

class CPSVertShader_Bump : public CPSVertShader
{
	public:
		CPSVertShader_Bump();
};

class CPSVertShader_Cube : public CPSVertShader
{
	public:
		CPSVertShader_Cube();
};

class CPSVertShader_Vol : public CPSVertShader
{
	public:
		CPSVertShader_Vol();
};

class CPSVertShader_BumpCube : public CPSVertShader
{
	public:
		CPSVertShader_BumpCube();
};

class CPSLimits : public CPixelShader
{
	public:
		CPSLimits();
		UINT TestInitialize(void);

	protected:

		bool GenerateTextureData();
		bool LoadTextures();
		void ReleaseTextures();
	
		HRESULT PopulateShaderInfo();


		CImage      m_GradientImage;
		CImage      m_Checker2Image;
		CImage      m_StripesImage;

		CnTexture   *m_pGradientTex;		
		CnTexture   *m_pChecker2Tex;
		CnTexture   *m_pStripesTex;
};

class CPSLegacy : public CPixelShader
{
	public:
		CPSLegacy();
		UINT TestInitialize(void);
		bool ExecuteTest(UINT uiTest);

	protected:

		bool GenerateTextureData();
		bool LoadTextures();
		void ReleaseTextures();
	
		HRESULT PopulateShaderInfo();

		CImage      m_Checker2Image;
		CImage      m_GradientImage;

		CnTexture	*m_pChecker2Tex;
		CnTexture   *m_pGradientTex;		
};


#endif // _PSHADER_H_
