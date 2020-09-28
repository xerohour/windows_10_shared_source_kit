#ifndef ___CSHADERVALIDATOR_H___
#define ___CSHADERVALIDATOR_H___


#include <CD3DTest.h>
#include <cd3dx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <cdevice.h>
#include "CInclude.h"


#define MAX_TEST_CASE_FROM_FILE_SIZE 1000000

struct VS_CRITERIA
{
    BOOL	bPassAssembler;
    BOOL	bRequireSWVP;
    INT		nVertexShaderVersion;
    //INT		nMaxVertexShaderConst;
    INT		nDynamicFlowControlDepth;
    INT		nNumTemps;
    INT		nStaticFlowControlDepth;
	INT		nMaxVertexShaderConst;// moved from above
    INT		nCaps;
    INT		nMaxVertexShader30InstructionSlots;
	FLOAT	fMinDXVersion;
};


struct PS_CRITERIA
{
    BOOL	bPassAssembler;
    BOOL	bRequireREF;
    INT		nPixelShaderVersion;
    INT		nDynamicFlowControlDepth;
    INT		nNumTemps;
    INT		nStaticFlowControlDepth;
    INT		nNumInstructionSlots;
    INT		nCaps;
    INT		nMaxPixelShader30InstructionSlots;
	FLOAT	fMinDXVersion;
};


#define CAPS_ARBITRARYSWIZZLE       1
#define CAPS_GRADIENTINSTRUCTIONS   2
#define CAPS_PREDICATION            4
#define CAPS_NODEPENDENTREADLIMIT   8
#define CAPS_NOTEXINSTRUCTIONLIMIT 16

class CShaderValidator : public CApiTest
{
public:

    CShaderValidator();

    virtual UINT TestInitialize();
    virtual bool ExecuteTest   ( UINT );
    virtual void UpdateStatus  ();
    virtual bool TestTerminate ();
	virtual void CommandLineHelp( );

	//virtual bool SetIndices(TCHAR *);

    CHAR  m_pszTestTemplateNameList[ 1000 ][ 256 ];
    UINT  m_puTestTemplateAccumulatedSizeList[ 1000 ];
    int   m_nTestTemplateCount;
    int   m_nCurrentTestTemplate;

protected:

    BOOL GetTestTemplateNameList();
    UINT GetTestCaseCount();
    BOOL GetNextTestCase();
    BOOL GetTestCase_VS();
    BOOL GetTestCase_PS();

    VS_CRITERIA m_VS_Criteria;
    PS_CRITERIA m_PS_Criteria;

    LPD3DXEFFECT         m_pEffect;
    LPD3DXBUFFER         m_pShaderBuf;
    LPD3DXBUFFER         m_pErrorBuf_Effect;
    LPD3DXBUFFER         m_pErrorBuf_Shader;
    CIncludeFromResource * m_pIncludeHandler_Resource;
    CIncludeFromFile     * m_pIncludeHandler_File;

    LPCSTR  m_pTestTemplateDesc;
    CHAR    m_szTestCaseDesc_ShaderVersion[ 1024 ];
    CHAR    m_szTestCaseDesc_Desc[ 1024 ];
    LPCSTR  m_pShader;
    BOOL    m_bVSTest;

    CHAR    m_szTestCaseFromFile[ MAX_TEST_CASE_FROM_FILE_SIZE ];

    UINT    m_uCurrentTestCase;

    BOOL    m_bLoadFromFile;
    BOOL    m_bIsREF;
    BOOL    m_bIsSWVP;
    BOOL    m_bIsMixedVP;

    LPDIRECT3DDEVICE9 m_pDev;

    D3DCAPS9 m_Caps;
};


#endif