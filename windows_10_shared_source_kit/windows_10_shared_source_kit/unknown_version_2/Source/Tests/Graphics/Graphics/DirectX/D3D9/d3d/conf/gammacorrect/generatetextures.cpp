 CShader5xTest_ld_base
{
public:
	CShader5xTest_ld() : CShader5xTest_ld_base( _T("ld") ) {}
};

//-----------------------------------------------------------------------------

class CShader5xTest_ld_s : public CShader5xTest_ld_base
{
public:
	CShader5xTest_ld_s() : CShader5xTest_ld_base(_T("ld_s")){}
};

//-----------------------------------------------------------------------------
class CShader5xTest_ldRawOrStruct : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_ldRawOrStruct(const TCHAR* Instruction) : CShader5xTest_ResourceAccess(Instruction){}
	TEST_RESULT	SetupTestCase();
	bool CheckResult( void * pResult, UINT uRowPitch );
	AssemblyHelper::MaskFlag	m_uOffsetSelectComp;
	AssemblyHelper::MaskFlag	m_uAddressSelectComp;

};

//-----------------------------------------------------------------------------
class CShader5xTest_ld_raw_base : public CShader5xTest_ldRawOrStruct
{
public:
	CShader5xTest_ld_raw_base(const TCHAR* Instruction) : CShader5xTest_ldRawOrStruct(Instruction){}
	void InitTestParameters();
	tstring GetShaderCode(AssemblyHelper* pAsmHelper);
};

// TODO: ld_raw and ld_structured can also be used on thread group shared memory, but this will be tested by WGF11Compute.
//-----------------------------------------------------------------------------
class CShader5xTest_ld_raw : public CShader5xTest_ld_raw_base
{
public:
	CShader5xTest_ld_raw() : CShader5xTest_ld_raw_base(_T("ld_raw")){}
};

//-----------------------------------------------------------------------------
class CShader5xTest_ld_raw_s : public CShader5xTest_ld_raw_base
{
public:
	CShader5xTest_ld_raw_s() : CShader5xTest_ld_raw_base(_T("ld_raw_s")){}
};

class CShader5xTest_ld_structured_base : public CShader5xTest_ldRawOrStruct
{
public:
	CShader5xTest_ld_structured_base(const TCHAR* Instruction) : CShader5xTest_ldRawOrStruct(Instruction){}
	void		InitTestParameters();
	tstring GetShaderCode(AssemblyHelper* pAsmHelper);
};

//-----------------------------------------------------------------------------
class CShader5xTest_ld_structured : public CShader5xTest_ld_structured_base
{
public:
	CShader5xTest_ld_structured() : CShader5xTest_ld_structured_base(_T("ld_structured")){}
};

//-----------------------------------------------------------------------------
class CShader5xTest_ld_structured_s : public CShader5xTest_ld_structured_base
{
public:
	CShader5xTest_ld_structured_s() : CShader5xTest_ld_structured_base(_T("ld_structured_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample() : CShader5xTest_ResourceAccess(_T("sample")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_s : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_s() : CShader5xTest_ResourceAccess(_T("sample_cl_s")){}
	Ignore(Clamp);
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_cl : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_cl() : CShader5xTest_ResourceAccess(_T("sample_cl_s")){}
	Ignore(ResidencyFeedback);
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_cl_s : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_cl_s() : CShader5xTest_ResourceAccess(_T("sample_cl_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_min_lod : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_min_lod() : CShader5xTest_ResourceAccess( _T("sample"), true ) {}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_d_base : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_d_base(const TCHAR* Instruction, bool bTestResourceMinLOD) : CShader5xTest_ResourceAccess(Instruction, bTestResourceMinLOD){}
	TEST_RESULT	InitResource();
	virtual bool UseClamp() const { return true; }
	virtual bool UseResidencyFeedback() const { return true; }
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_d : public CShader5xTest_sample_d_base
{
public:
	CShader5xTest_sample_d() : CShader5xTest_sample_d_base( _T("sample_d"), true ) {}
	virtual bool UseClamp() const { return true; }
	virtual bool UseResidencyFeedback() const { return true; }
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_d_s : public CShader5xTest_sample_d_base
{
public:
	CShader5xTest_sample_d_s() : CShader5xTest_sample_d_base( _T("sample_d_cl_s"), true ) {}
	Ignore(Clamp);
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_d_cl : public CShader5xTest_sample_d_base
{
public:
	CShader5xTest_sample_d_cl() : CShader5xTest_sample_d_base( _T("sample_d_cl_s"), true ) {}
	Ignore(ResidencyFeedback);
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_d_cl_s : public CShader5xTest_sample_d_base
{
public:
	CShader5xTest_sample_d_cl_s() : CShader5xTest_sample_d_base( _T("sample_d_cl_s"), true ) {}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_l_base : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_l_base(const TCHAR* Instruction, bool bTestResourceMinLOD) : CShader5xTest_ResourceAccess( Instruction, bTestResourceMinLOD ) {}
	TEST_RESULT	InitResource();
	virtual bool UseClamp() const { return true; }
	virtual bool UseResidencyFeedback() const { return true; }
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_l : public CShader5xTest_sample_l_base
{
public:
	CShader5xTest_sample_l() : CShader5xTest_sample_l_base( _T("sample_l"), true ) {}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_l_s : public CShader5xTest_sample_l_base
{
public:
	CShader5xTest_sample_l_s() : CShader5xTest_sample_l_base( _T("sample_l_s"), true ) {}
	Ignore(Clamp);
};

//-----------------------------------------------------------------------------

class CShader5xTest_Comparison : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_Comparison( const TCHAR* Instruction, bool bTestPerResourceMinLOD = false );

	void	InitTestParameters( );
	bool	CheckResult( void *pResult, UINT uRowPitch );
	virtual bool UseClamp() const { return true; }
	virtual bool UseResidencyFeedback() const { return true; }
};

//-----------------------------------------------------------------------------
class CShader5xTest_sample_c : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c() : CShader5xTest_Comparison(_T("sample_c")){}
};

//-----------------------------------------------------------------------------
class CShader5xTest_sample_c_s : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_s() : CShader5xTest_Comparison(_T("sample_c_cl_s")){}
	Ignore(Clamp);
};

//-----------------------------------------------------------------------------
class CShader5xTest_sample_c_cl : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_cl() : CShader5xTest_Comparison(_T("sample_c_cl_s")){}
	Ignore(ResidencyFeedback);
};

//-----------------------------------------------------------------------------
class CShader5xTest_sample_c_cl_s : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_cl_s() : CShader5xTest_Comparison(_T("sample_c_cl_s")){}
};

//-----------------------------------------------------------------------------
class CShader5xTest_sample_c_min_lod : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_min_lod() : CShader5xTest_Comparison( _T("sample_c"), true ) {}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_c_lz : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_lz() : CShader5xTest_Comparison(_T("sample_c_lz")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_c_lz_s : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_lz_s() : CShader5xTest_Comparison(_T("sample_c_lz_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_c_lz_min_lod : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_lz_min_lod() : CShader5xTest_Comparison( _T("sample_c_lz"), true ) {}
	TEST_RESULT	InitResource();

protected:
	virtual bool TestingPerResourceMinLOD();
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_b_base : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_b_base(const TCHAR* Instruction) : CShader5xTest_ResourceAccess(Instruction)
	{
		m_ShaderStage = D3D_SHADER_STAGE_PIXEL;
		m_Resource = RESOURCE_TEXTURE2D_ARRAY;
		m_bIsBound = true;
	}

	void		InitTestParameters( );
	TEST_RESULT InitResource( );
	virtual bool UseClamp() const { return true; }
	virtual bool UseResidencyFeedback() const { return true; }
	bool		CheckResult( void *pResult, UINT uRowPitch );

protected:
	float		*m_pResourceData;
	float        	m_fMinLOD;
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_b : public CShader5xTest_sample_b_base
{
public:
	CShader5xTest_sample_b() : CShader5xTest_sample_b_base(_T("sample_b")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_b_s : public CShader5xTest_sample_b_base
{
public:
	CShader5xTest_sample_b_s() : CShader5xTest_sample_b_base(_T("sample_b_cl_s")){}
	Ignore(Clamp);
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_b_cl : public CShader5xTest_sample_b_base
{
public:
	CShader5xTest_sample_b_cl() : CShader5xTest_sample_b_base(_T("sample_b_cl_s")){}
	Ignore(ResidencyFeedback)
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_b_cl_s : public CShader5xTest_sample_b_base
{
public:
	CShader5xTest_sample_b_cl_s() : CShader5xTest_sample_b_base(_T("sample_b_cl_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_base : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_gather4_base(const TCHAR* Instruction);

	void			InitTestParameters( );
	bool			CheckResult( void *pResult, UINT uRowPitch );
	TEST_RESULT		Setup( );
	TEST_RESULT		SetupTestCase( );

	virtual TEST_RESULT InitResource();

protected:
	void			GetQuad( UINT x, UINT y, UINT elementStride, UINT uRowPitch, float *pQuad );
	float           m_fResourceMinLOD;
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4 : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4() : CShader5xTest_gather4_base(_T("gather4")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_s : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_s() : CShader5xTest_gather4_base(_T("gather4_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_po : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_po() : CShader5xTest_gather4_base(_T("gather4_po")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_po_s : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_po_s() : CShader5xTest_gather4_base(_T("gather4_po_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_c : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_c() : CShader5xTest_gather4_base(_T("gather4_c")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_c_s : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_c_s() : CShader5xTest_gather4_base(_T("gather4_c_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_po_c : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_po_c() : CShader5xTest_gather4_base(_T("gather4_po_c")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_po_c_s : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_po_c_s() : CShader5xTest_gather4_base(_T("gather4_po_c_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_lod : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_lod();

	void			InitTestParameters( );
	bool			CheckResult( void *pResult, UINT uRowPitch );
	TEST_RESULT		SetupInputBuffer( );
	TEST_RESULT		Setup( );

protected:
	float			m_fScale;
};

#undef Ignore// FILE:        pshader.h
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
		bool PreModeS