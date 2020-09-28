// FILE:        sizes.h
// DESC:        WLP texture size conformance test header

#include "CD3D.h"
#include "CD3DTest.h"
#include "CTextures.h"

#define GLE(x) (m_pD3D->HResultToString(x))

#define LOG(x) if (!x) { WriteToLog("(FAIL) %s: %s", #x, GLE(GetLastError())); Fail(); }

#define WLP_RED     D3DRGBA(1.0, 0.0, 0.0, 1.0)
#define WLP_GREEN   D3DRGBA(0.0, 0.8, 0.0, 1.0)
#define WLP_BLUE    D3DRGBA(0.0, 0.0, 1.0, 1.0)
#define WLP_GRAY    D3DRGBA(0.5, 0.5, 0.5, 1.0)
#define WLP_WHITE   D3DRGBA(1.0, 1.0, 1.0, 1.0)
#define WLP_YELLOW  D3DRGBA(1.0, 1.0, 0.0, 1.0)
#define WLP_CYAN    D3DRGBA(0.0, 1.0, 1.0, 1.0)
#define WLP_MAGENTA D3DRGBA(1.0, 0.0, 1.0, 1.0)

typedef union
{
    DWORD dwValue;
    float fValue;
} DWORDFLOAT; 

typedef struct _MyColor
{
    const CHAR *szName;
    DWORD dwColor;
} MYCOLOR, *PMYCOLOR;

class CSizes : public CD3DTest
{
    public:
        CHAR m_szDesc[256];        // description
        INT m_nSeed;               // random number seed
        DWORD m_dwMaxAspectRatio;  // max aspect ratio
        DWORD m_dwMaxWidthPow2;    // max texture width power of 2
        DWORD m_dwMaxHeightPow2;   // max texture height power of 2
        DWORD m_dwWidth;           // texture width
        DWORD m_dwHeight;          // texture height
        CImage *m_pImage;          // image class
        CnTexture *m_pTexture;     // texture class

    public:
        CSizes();
        ~CSizes();
        virtual bool CapsCheck(VOID);
        virtual bool Setup(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        virtual VOID Cleanup(VOID);
        virtual VOID SceneRefresh(VOID);
        bool SetupMatrices(VOID);
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
        DWORD MaxPowerOf2(DWORD dwSrc)
        {
            DWORD dwMax = (DWORD) 31;

            while ((dwMax > 0) && !((1 << dwMax) & dwSrc))
                dwMax--;

            return dwMax;
        }
};
