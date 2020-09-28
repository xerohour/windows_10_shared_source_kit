// FILE:        specular.h
// DESC:        WLP specular conformance test header

#include "CD3D.h"
#include "CD3DTest.h"
#include "CShapes.h"
#include "CStateCascade.h"
#include "CTextures.h"

typedef struct _MyColor
{
    const CHAR *szName;
    D3DCOLORVALUE dcvColor;
} MYCOLOR, *PMYCOLOR;

#define GLE(x) (m_pD3D->HResultToString(x))

#define POWERS    3
#define BASEPOWER 8

#define KEY_LIGHT    "Light"
#define KEY_MATERIAL "Material"
#define KEY_SPECULAR "Specular"
#define KEY_POWER    "Power"

const UINT MAXBUFFER = 128;

const UINT NSTATUS = 5;

class CSpecular : public CD3DTest
{
    private:
        CHAR m_szStatus[NSTATUS][2][MAXBUFFER]; // text buffer (status)
        bool m_bTexture;
        CShapes m_Sphere;
        CStateCascade *m_pStates;
        CImage *m_pImage;
        CnTexture *m_pTexture;

    public:
        CSpecular();
        ~CSpecular();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual bool Setup(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uiTest);
        virtual VOID Cleanup(VOID);
        virtual VOID SceneRefresh(VOID);
        virtual VOID UpdateStatus(VOID);
        VOID ClearStatus(VOID);
        bool SetupMatrices(VOID);
};

