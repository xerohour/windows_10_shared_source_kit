// FILE:        multi.h
// DESC:        WLP multi texturing conformance test header

#include "CD3D.h"
#include "CD3DTest.h"
#include "CTextures.h"

typedef enum _BlendType
{
    MODULATE_RGB			= 0,
    MODULATE_ALPHA			= 1,
    ADD						= 2,
    BLEND_REGISTER			= 3,
    BLEND_DIFFUSE			= 4,
	MODULATE_ALPHA_ADDCOLOR	= 5
} BLENDTYPE;

typedef struct _BlendMode
{
    const CHAR *szName;
    BLENDTYPE eBlend;
} BLENDMODE, *PBLENDMODE;

typedef struct _MyColor
{
    const CHAR *szName;
    DWORD dwColor;
} MYCOLOR, *PMYCOLOR;

typedef struct _FlexVert
{
    D3DVALUE fPx;
    D3DVALUE fPy;
    D3DVALUE fPz;
    D3DVALUE fRHW;
    D3DCOLOR dcDiffuse;
	D3DCOLOR dcSpecular;
    D3DVALUE fTs;
    D3DVALUE fTt;
    D3DVALUE fTu;
    D3DVALUE fTv;
} FLEXVERT, *PFLEXVERT;

const DWORD FLEXFMT = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2);

const UINT MAXBUFFER = 128;

const UINT NSTATUS = 7;

#define GLE(x) (m_pD3D->HResultToString(x))

#define LOG(x) if (!x) { WriteToLog("(FAIL) %s: %s.\n", #x, GLE(GetLastError())); Fail(); }

#define WLP_ALPHA(x, y) (x | D3DRGBA(0.0f, 0.0f, 0.0f, y))

#define WLP_RED     D3DRGBA(1.0f, 0.0f, 0.0f, 0.0f)
#define WLP_GREEN   D3DRGBA(0.0f, 0.8f, 0.0f, 0.0f)
#define WLP_BLUE    D3DRGBA(0.0f, 0.0f, 1.0f, 0.0f)
#define WLP_GRAY    D3DRGBA(0.5f, 0.5f, 0.5f, 0.0f)
#define WLP_WHITE   D3DRGBA(1.0f, 1.0f, 1.0f, 0.0f)
#define WLP_YELLOW  D3DRGBA(1.0f, 1.0f, 0.0f, 0.0f)
#define WLP_CYAN    D3DRGBA(0.0f, 1.0f, 1.0f, 0.0f)
#define WLP_MAGENTA D3DRGBA(1.0f, 0.0f, 1.0f, 0.0f)

#define NALPHAS   5
#define NINDICES  5
#define NMODES    6
#define NPOWERS   9
#define NTEXTURES 3
#define NFOG	  2

class CMulti : public CD3DTest
{
    private:
        CHAR m_szStatus[NSTATUS][2][MAXBUFFER]; // text buffer (status)
        DWORD m_dwIndex;
        DWORD m_dwColors[4];
        float m_fAlpha;
        int m_nBlend;
        int m_nSeed;
        int m_nTextures;
        int m_nVariations;
        CImage *m_pImages[2];
        CnTexture *m_pTextures[2];

    public:
        CMulti();
        ~CMulti();
        virtual bool CapsCheck(VOID);
        virtual bool Setup(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uiTest);
        virtual VOID Cleanup(VOID);
        virtual VOID SceneRefresh(VOID);
        virtual bool ProcessFrame(VOID);
        virtual VOID UpdateStatus();
		virtual VOID CommandLineHelp(VOID);
        VOID ClearStatus();
        bool SetupMatrices(VOID);
		VOID SetupFog(bool bFogOn);
        VOID SetupModulateRGB(VOID);
        VOID SetupModulateAlpha(VOID);
		VOID SetupModulateAlphaAddColor(VOID);
        VOID SetupAdd(VOID);
        VOID SetupBlendFactor(VOID);
        VOID SetupBlendDiffuse(VOID);
        VOID ValidateDevice(VOID);
        DWORD Bits(DWORD dwSrc)
        {
            DWORD dwBits = (DWORD) 0, dwMask = dwSrc;

            if (0 == dwMask)
                return dwBits;
            else
            {
                while (0 == (dwMask & 1))
                    dwMask >>= 1;

                while (dwMask & 1)
                {
                    dwMask >>= 1;
                    dwBits++;
                }
            }

            return dwBits;
        }
};

