#pragma once

#include <vector>

#include <CDevice.h>

#define	TYPE_OFFSCREENPLAIN		0
#define TYPE_RENDERTARGET		1
#define TYPE_DEPTHSTENCIL		2
#define	TYPE_TEXTURE			3
#define TYPE_CUBETEXTURE		4
#define TYPE_VOLUMETEXTURE		5
#define TYPE_VERTEXBUFFER		6
#define TYPE_INDEXBUFFER		7
#define TYPE_MAXTYPE			8 //The number of resource type
#define TYPE_NONE       		0xf0f0f0f0

extern const TCHAR *TypeName[];

struct RESOURCE_POOL
{
	DWORD	dwPool;
	TCHAR	szPool[255];
	float	fMinDX;
	float	fMaxDX;
};

struct RESOURCE_FORMAT
{
	FMT		format;
	TCHAR	szFormat[255];
	float	fMinDX;
	float	fMaxDX;
};

struct RESOURCE_USAGE
{
	float	fMinDX;
	float	fMaxDX;
	DWORD	dwUsage;
	TCHAR	szUsage[255];
};



struct RESOURCE_TYPE
{
	float	fMinDX;
	float	fMaxDX;
	DWORD	dwType;
	DWORD	dwUsage;
	DWORD	dwRType;
	TCHAR	szType[255];
};

struct RESOURCE_SIZE
{
	UINT	uWidth;
	UINT	uHeight;
	bool	bSquare;
	bool	bMul4;
};

struct RESOURCE_CASE
{
	DWORD dwUsage;
	TCHAR szUsage[255];
	DWORD dwPool;
	TCHAR szPool[255];
	FMT  format;
	TCHAR szFormat[255];
	DWORD dwFVF;
	TCHAR szFVF[255];
	DWORD dwRType;

	UINT				uWidth;
	UINT				uHeight;
	UINT				uDepth;
	bool				bValid;
	UINT				uLevels;
	MULTISAMPLE_TYPE	multiSample;
	DWORD				multiSampleQuality;
	bool				bLockable;
	bool				bDiscardable;
	bool				bUseMipLevels;

	bool				bCanUpdateSurface;
	bool				bCanStretchRect;
	bool				bCanColorFill;
	bool				bCanRender;
    bool                bRequiredShareable;
};

struct XYZRHWDIFFUSEVERTEX 
{
	FLOAT     x;             
	FLOAT     y;
	FLOAT     z;
	FLOAT	  rhw;
	DWORD	  color;
};

class CEnumResource
{
private:
	LPD3D						m_pD3D;
	LPDEVICE					m_pDevice;
	CD3DTest					*m_pD3DTest;
	float						m_fDXVersion;
	DWORD						m_dwDeviceType;
	FMT							m_formatAdapter;

	UINT						m_nMultiSample;

    FMT                         m_formatCompareRT;

	//Command line overide test options
	DWORD						m_dwPoolToTest;
	DWORD						m_dwUsageToTest;
	DWORD						m_dwRTYPEToTest;

	std::vector <RESOURCE_CASE *>	m_listCases[TYPE_MAXTYPE];

	UINT							m_iCurrentCase[TYPE_MAXTYPE];

	//private methods
	bool	FormatNotInCurrentDX(UINT uFormat);
	bool	PoolNotInCurrentDX(UINT uPool);
	bool	UsageNotInCurrentDX(UINT uUsage);

	void	GenerateAllCases();

    bool    FindComapreRenderTargetFMT(FMT* fmt);

	void	GenerateRenderTargetCases();
	void	GenerateDepthStencilCases();
	void	GenerateOffscreenPlainCases();
	void	GenerateTextureCases();
	void	GenerateCubeTextureCases();
	void	GenerateVolumeTextureCases();
	void	GenerateVertexBufferCases();
	void	GenerateIndexBufferCases();

	void	GetCommandLineOptions();

public:
	CEnumResource();
	~CEnumResource();

	bool			Init(CD3DTest *pD3DTest);
	void			Clean();

	HRESULT			CreateResource(DWORD dwRTYPE, RESOURCE_CASE *pResCase, CObject **ppResource, HANDLE *pSharedHandle);
	RESOURCE_CASE*	GetCurrentCase(DWORD dwRTYPE);
	RESOURCE_CASE*	GetNextCase(DWORD dwRTYPE);
	RESOURCE_CASE*	GetNextCase(DWORD dwRTYPE, DWORD dwPoolRequested);
	RESOURCE_CASE*	GetNextCase(DWORD dwRTYPE, DWORD dwPoolRequested, UINT &uRequestedCase);
	void			DumpCaseDetails(DWORD dwRTYPE, RESOURCE_CASE *pResCase, TCHAR* szDesc);
    void            MakeCaseName(DWORD dwRTYPE, RESOURCE_CASE *pResCase, TCHAR* szName);
    void            GetTypeName(DWORD dwRType, TCHAR* szRType);

	FMT				GetAnotherValidFormatForUsage(DWORD dwRTYPE, DWORD dwUsage, FMT format);
	DWORD			GetAnotherValidUsageForFormat(DWORD dwRTYPE, FMT format, DWORD dwUsage, bool bAllowZero = true);
	DWORD			GetAnotherValidPool(DWORD dwRTYPE, DWORD dwPool, FMT format, DWORD dwUsage);
	TCHAR*			GetUsageString(DWORD dwUsage);
	TCHAR*			GetFormatString(FMT format);
	UINT			GetNbCasesForPool(DWORD dwRTYPE, DWORD dwPool);
	int				GetLastCaseForPool(DWORD dwRTYPE, DWORD dwPool);

	void			FindMatchingCaseForResType(DWORD dwSrcRTYPE, RESOURCE_CASE *pSrcResCase, DWORD dwDestRTYPE, RESOURCE_CASE *pDestResCase);

    FMT             GetCompareRTFormat() {return m_formatCompareRT;}

	void			ResetCurrentCase(DWORD dwRTYPE);

	bool			IsFormatDXTn(FMT format);
	bool			IsFormatFourCC(FMT format);
};
