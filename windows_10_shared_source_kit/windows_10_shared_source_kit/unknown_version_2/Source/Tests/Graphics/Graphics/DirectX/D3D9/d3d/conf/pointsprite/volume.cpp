// FILE:        VClip.cpp
// DESC:        volume clip class methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:

#include <math.h>
#include "sprite.h"

// NAME:        CSpriteVClip()
// DESC:        constructor for sprite volume clip class
// INPUT:       none
// OUTPUT:      none

CSpriteVClip::CSpriteVClip(VOID)
{
    m_szTestName="View volume clip";
    m_szCommandKey="VClip";

    m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        ~CSpriteVClip()
// DESC:        destructor for sprite volume clip class
// INPUT:       none
// OUTPUT:      none

CSpriteVClip::~CSpriteVClip(VOID)
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSpriteVClip::CommandLineHelp(void)
{
    CSprite::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteVClip::TestInitialize(VOID)
{
    D3DXVECTOR3 Origin, Temp;
    UINT uResult;

    m_Group.Set(CF_VERTEXPOINTSIZE);
    uResult = CSprite::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    m_nVariationsPerFormat = 2*NUM_ROT_ANGLES;
    SetTestRange((UINT) 1, (UINT) m_nVariationsPerFormat);
    Clear(m_dcBackground);
    SetViewport(CLIPBORDER,
                CLIPBORDER,
                m_pCurrentMode->nBackBufferWidth - (CLIPBORDER*2),
                m_pCurrentMode->nBackBufferHeight - (CLIPBORDER*2));

    SetPositionWorld(TOPLEFT_INSIDE, &Origin.x, &Origin.y);
    SetPositionWorld(BOTRIGHT_INSIDE, &Temp.x, &Temp.y);
    Origin.x = (Temp.x - Origin.x)/2.0f;
    Origin.y = (Temp.y - Origin.y)/2.0f;
    Origin.z = (m_dvFar - m_dvNear)/2.0f;

    // make sure path is clipped in z

    m_Path.Lemniscate(&Origin, 1.5f*Origin.z, 0.0f, SNAKE_STEPS); //As z is the largest radius.
    Origin.x = (m_Port.Right() - m_Port.Left())/2.0f;
    Origin.y = (m_Port.Bottom() - m_Port.Top())/2.0f;
    Origin.z = 0.5;

    // path will never be clipped in z but will be clipped in x,y

    m_PathTL.Lemniscate(&Origin, 1.5f*Origin.x, 0.0f, SNAKE_STEPS); //As x is the largest radius.
    Origin.x = m_Port.Left() - ((m_Port.Right() - m_Port.Left())/2.0f);
    Origin.y = (m_Port.Bottom() - m_Port.Top())/4.0f;
    Origin.z = -0.5;
    Temp.x = m_Port.Right() + ((m_Port.Right() - m_Port.Left())/2.0f);
    Temp.y = Origin.y;
    Temp.z = 1.5;

    // both final paths will sometimes be clipped in x,y,z

    m_Top.Line(&Origin, &Temp, SNAKE_STEPS);
    Origin.y = 3*(m_Port.Bottom() - m_Port.Top())/4.0f;
    Temp.y = Origin.y;
    m_Bottom.Line(&Origin, &Temp, SNAKE_STEPS);

    // set point size for devices that don't support point size in vertices
    m_Sprite.PointSize.f = VERT_SIZE;
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSpriteVClip::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    D3DXVECTOR3 Origin, Temp;
    DWORD dwBot, dwTop;
    DWORD dwPre, dwRaw;
    FLOAT fAngle;
    MYD3DLVERTEX Verts[NUM_VERTS];
    MYD3DLSVERTEX VertsWithSize[NUM_VERTS];
    MYD3DTLSVERTEX VertsTLWithSize[NUM_VERTS], TopVertsTLWithSize[NUM_VERTS], BotVertsTLWithSize[NUM_VERTS];
    MYD3DTLVERTEXNOTEX TopVertsTLnoTC[NUM_VERTS], BotVertsTLnoTC[NUM_VERTS], VertsTLnoTC[NUM_VERTS];
    VOID *pvBot, *pvTop;
    VOID *pvPre, *pvRaw;

    if (!CSprite::ExecuteTest(uTest))
        return false;    
    
    fAngle = pi/180.0f * (360.0f/NUM_ROT_ANGLES);
    fAngle *= uTest%(NUM_ROT_ANGLES);    
    m_Path.SetCurrentRotXYZ(0.0f, fAngle, 0.0f);
    m_PathTL.SetCurrentRotXYZ(fAngle, 0.0f, 0.0f);
    m_nSnakeStart = uTest%SNAKE_STEPS;

    if (m_Flags.Check(CF_VERTEXPOINTSIZE))
    {
        pvTop = TopVertsTLWithSize;
        pvBot = BotVertsTLWithSize;
        pvPre = VertsTLWithSize;
        pvRaw = VertsWithSize;
        dwTop = dwBot = dwPre = MYD3DFVF_TLSVERTEX;
        dwRaw = MYD3DFVF_LSVERTEX;

        for (int i = 0; i < NUM_VERTS; i++)
        {
            m_Path.GetStep(m_nSnakeStart+i, &Origin, &Temp, &Temp, &Temp);
            VertsWithSize[i].dvX = Origin.x;
            VertsWithSize[i].dvY = Origin.y;
            VertsWithSize[i].dvZ = Origin.z;
            VertsWithSize[i].dvS = i*2 > VERT_SIZE ? VERT_SIZE : i*2;
            VertsWithSize[i].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
            VertsWithSize[i].dcSpecular = m_dcBackground;
            VertsWithSize[i].dvTU = 0.0f;
            VertsWithSize[i].dvTV = 0.0f;

            m_PathTL.GetStep(m_nSnakeStart+i, &Origin, &Temp, &Temp, &Temp);
            VertsTLWithSize[i].dvSX = Origin.x;
            VertsTLWithSize[i].dvSY = Origin.y;
            VertsTLWithSize[i].dvSZ = 0.5;
            VertsTLWithSize[i].dvRHW = 1.0f;
            VertsTLWithSize[i].dvS = i*2 > VERT_SIZE ? VERT_SIZE : i*2;
            VertsTLWithSize[i].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
            VertsTLWithSize[i].dcSpecular = m_dcBackground;
            VertsTLWithSize[i].dvTU = 0.0f;
            VertsTLWithSize[i].dvTV = 0.0f;

            m_Top.GetStep(m_nSnakeStart+i, &Origin, &Temp, &Temp, &Temp);
            TopVertsTLWithSize[i].dvSX = Origin.x;
            TopVertsTLWithSize[i].dvSY = Origin.y;
            TopVertsTLWithSize[i].dvSZ = Origin.z;
            TopVertsTLWithSize[i].dvRHW = 1.0f;
            TopVertsTLWithSize[i].dvS = i*2 > VERT_SIZE ? VERT_SIZE : i*2;
            TopVertsTLWithSize[i].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
            TopVertsTLWithSize[i].dcSpecular = m_dcBackground;

            m_Bottom.GetStep(SNAKE_STEPS/2.0f - (m_nSnakeStart+i), &Origin, &Temp, &Temp, &Temp);
            BotVertsTLWithSize[i].dvSX = Origin.x;
            BotVertsTLWithSize[i].dvSY = Origin.y;
            BotVertsTLWithSize[i].dvSZ = Origin.z;
            BotVertsTLWithSize[i].dvRHW = 1.0f;
            BotVertsTLWithSize[i].dvS = i*2 > VERT_SIZE ? VERT_SIZE : i*2;
            BotVertsTLWithSize[i].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
            BotVertsTLWithSize[i].dcSpecular = m_dcBackground;
        }
    }
    else
    {
        pvTop = TopVertsTLnoTC;
        pvBot = BotVertsTLnoTC;
        pvPre = VertsTLnoTC;
        pvRaw = Verts;
        dwTop = dwBot = dwPre = MYD3DFVF_TLVERTEX_NOTEX;
        dwRaw = MYD3DFVF_LVERTEX;        

        for (int i = 0; i < NUM_VERTS; i++)
        {
            m_Path.GetStep(m_nSnakeStart+i, &Origin, &Temp, &Temp, &Temp);
            Verts[i].dvX = Origin.x;
            Verts[i].dvY = Origin.y;
            Verts[i].dvZ = Origin.z;
            Verts[i].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
            Verts[i].dcSpecular = m_dcBackground;
            Verts[i].dvTU = 0.0f;
            Verts[i].dvTV = 0.0f;

            m_PathTL.GetStep(m_nSnakeStart+i, &Origin, &Temp, &Temp, &Temp);
            VertsTLnoTC[i].dvSX = Origin.x;
            VertsTLnoTC[i].dvSY = Origin.y;
            VertsTLnoTC[i].dvSZ = 0.5;
            VertsTLnoTC[i].dvRHW = 1.0f;            
            VertsTLnoTC[i].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
            VertsTLnoTC[i].dcSpecular = m_dcBackground;

            m_Top.GetStep(m_nSnakeStart+i, &Origin, &Temp, &Temp, &Temp);
            TopVertsTLnoTC[i].dvSX = Origin.x;
            TopVertsTLnoTC[i].dvSY = Origin.y;
            TopVertsTLnoTC[i].dvSZ = Origin.z;
            TopVertsTLnoTC[i].dvRHW = 1.0f;            
            TopVertsTLnoTC[i].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
            TopVertsTLnoTC[i].dcSpecular = m_dcBackground;
            m_Bottom.GetStep(SNAKE_STEPS/2.0f - (m_nSnakeStart+i), &Origin, &Temp, &Temp, &Temp);
            BotVertsTLnoTC[i].dvSX = Origin.x;
            BotVertsTLnoTC[i].dvSY = Origin.y;
            BotVertsTLnoTC[i].dvSZ = Origin.z;
            BotVertsTLnoTC[i].dvRHW = 1.0f;            
            BotVertsTLnoTC[i].dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(255,0,0,255);
            BotVertsTLnoTC[i].dcSpecular = m_dcBackground;
        }
    }    
      
    sprintf(m_szBuffer, "Volume Clip Test : Format %s", m_szFormat);
    BeginTestCase(m_szBuffer, uTest);

    if (BeginScene())
    {
        if (NULL != pvRaw)
        {
            if (!RenderPrimitive(D3DPT_POINTLIST, dwRaw, pvRaw, (DWORD) NUM_VERTS, NULL, 0, NULL))
            {
                WriteToLog("RenderPrimitive() failed.\n");
                bResult = false;
            }
        }        

        // Reason for order of TL vertices is to cover situations where
        // FVF converted by runtime remains constant and z clipping changes
        //  -clip primitives
        //  -don't clip primitives
        //  -clip primitives
        // see manbug #53249

        if (NULL != pvPre)
        {
            if (!RenderPrimitive(D3DPT_POINTLIST, dwPre, pvPre, (DWORD) NUM_VERTS, NULL, 0, NULL))
            {
                WriteToLog("RenderPrimitive() failed.\n");
                bResult = false;
            }
        }

        if (NULL != pvTop)
        {
            if (!RenderPrimitive(D3DPT_POINTLIST, dwTop, pvTop, (DWORD) NUM_VERTS, NULL, 0, NULL))
            {
                WriteToLog("RenderPrimitive() failed.\n");
                bResult = false;
            }
        }

        if (NULL != pvBot)
        {
            if (!RenderPrimitive(D3DPT_POINTLIST, dwBot, pvBot, (DWORD) NUM_VERTS, NULL, 0, NULL))
            {
                WriteToLog("RenderPrimitive() failed.\n");
                bResult = false;
            }
        }

        if (!EndScene())
        {
            WriteToLog("EndScene() failed.\n");
            bResult = false;
        }
    }
    else
    {
        WriteToLog("BeginScene() failed.\n");
        bResult = false;
    }

    return bResult;
}
