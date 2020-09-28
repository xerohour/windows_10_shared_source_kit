// FILE:        attenfvfl.cpp
// DESC:        attenuate class methods for point sprite conformance test
// AUTHOR:      Anthony W. Clarke
// COMMENTS:

#include <math.h>
#include "sprite.h"

const UINT NSPAWN = 24;

// NAME:        CSpriteAttenuateFVFL()
// DESC:        constructor for sprite attenuate class (D3DFVF_LVERTEX)
// INPUT:       none
// OUTPUT:      none

CSpriteAttenuateFVFL::CSpriteAttenuateFVFL(VOID)
{
    m_szTestName="Point sprite attenuation/scale (D3DFVF_LVERTEX)";
    m_szCommandKey="AttenuateFVFL";

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81;

    m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        ~CSpriteAttenuateFVFL()
// DESC:        destructor for sprite attenuate class (D3DFVF_LVERTEX)
// INPUT:       none
// OUTPUT:      none

CSpriteAttenuateFVFL::~CSpriteAttenuateFVFL(VOID)
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSpriteAttenuateFVFL::CommandLineHelp(void)
{
    CSprite::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteAttenuateFVFL::TestInitialize(VOID)
{
    double dTemp, dPointSizeComp;
    D3DXVECTOR3 V;
    UINT nAttempts, uResult = CSprite::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    m_LineOfSight.x = 0.0f;
    m_LineOfSight.y = 0.0f;
    m_LineOfSight.z = 0.0f;

    memset(m_Coefficients, 0, NCSETS*sizeof(COEFFDATA));

    // used for SetRenderState(D3DRS_POINTSIZE, m_Sprite.PointSize.dw), constant for this test group

    m_Sprite.PointSize.f = m_Sprite.PointSizeMax.f*1.5f;

    V.x = m_At.x - m_From.x;
    V.y = m_At.y - m_From.y;
    V.z = m_At.z - m_From.z;
    dTemp = sqrt((double) (V.x*V.x + V.y*V.y + V.z*V.z));
    m_LineOfSight.x = (D3DVALUE) (V.x/dTemp);
    m_LineOfSight.y = (D3DVALUE) (V.y/dTemp);
    m_LineOfSight.z = (D3DVALUE) (V.z/dTemp);

    // point sprite scale formula:
    //   Ss = Vh*Si*sqrt(1/(A + B*De + C*(De*De))
    // Si is per vertex or per render state
    
    // constant:
    //   a = ((Si*Vh)/Ss)**2
    // point sprite at max size when De = 1

    dPointSizeComp = ((m_Sprite.PointSize.f*m_pCurrentMode->nBackBufferHeight)/m_Sprite.PointSizeMax.f);
    dPointSizeComp *= dPointSizeComp;
    m_Coefficients[0].Ka.f =  dPointSizeComp;
    m_Coefficients[0].Kb.f = 0.0f;
    m_Coefficients[0].Kc.f = 0.0f;

    // (NB added to help with AdjustVertex() as distance will not change the point size in this case)

    m_Sprite.Ka.f = m_Coefficients[0].Ka.f;
    m_Sprite.Kb.f = m_Coefficients[0].Kb.f;
    m_Sprite.Kc.f = m_Coefficients[0].Kc.f;

    for (nAttempts = 0; nAttempts < MAX_ATTEMPTS; nAttempts++)
    {
        if (AdjustVertexAttenuated(&V, m_Sprite.PointSize.f))
            break;

        m_Coefficients[0].Ka.f += COFACTINC;
        m_Sprite.Ka.f = m_Coefficients[0].Ka.f;
    }

    if (MAX_ATTEMPTS == nAttempts)
    {
        WriteToLog("Unable to find a valid point size! Using %f\n", m_Sprite.PointSize.f);
        return D3DTESTINIT_ABORT;
    }
        
    // linear:
    //   b = ((Si*Vh)/Ss)**2
    // point sprite at max size when De = 1;

    m_Coefficients[1].Ka.f = 0.0f;
    m_Coefficients[1].Kb.f = dPointSizeComp;
    m_Coefficients[1].Kc.f = 0.0f;    
    
    // quadratic:
    //   c = ((Si*Vh)/Ss)**2
    // point sprite at max size when De = m_dvFar - m_dvNear / 5.0f
    // (i.e. 1/5 of the way down the view frustrum)

    dTemp = (m_dvFar - m_dvNear) / 5.0f;
    dTemp *= dTemp;
    m_Coefficients[2].Ka.f = 0.0f;
    m_Coefficients[2].Kb.f = 0.0f;
    m_Coefficients[2].Kc.f = dPointSizeComp * 1.0f/(dTemp);   

    // full:
    // a = m_Coefficients[0].Ka.f, b = m_Coefficients[1].Kb.f, c = m_Coefficients[0].Kc.f

    m_Coefficients[3].Ka.f = m_Coefficients[0].Ka.f;
    m_Coefficients[3].Kb.f = m_Coefficients[1].Kb.f;
    m_Coefficients[3].Kc.f = m_Coefficients[2].Kc.f;
    m_nVariationsPerFormat *= NCSETS;
    SetTestRange((UINT) 1, (UINT) (m_uCommonTextureFormats*m_nVariationsPerFormat));
    SetRenderState(D3DRS_POINTSCALEENABLE, (DWORD) TRUE);
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSpriteAttenuateFVFL::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    D3DVALUE dvDistance, dvT;
    UINT uSet, uLocations;
   
    uLocations = m_nVariationsPerFormat/NCSETS;
    dvT = (D3DVALUE) ((uTest - 1) % uLocations) / (D3DVALUE) (uLocations - 1);
    dvDistance = dvT*((m_dvFar + 1.0f ) - 2.0f*EPSILON); // pick a range that culls front and back
    uSet = (UINT) ((uTest - 1) / uLocations) % NCSETS;

    m_Sprite.Ka.f = m_Coefficients[uSet].Ka.f;
    m_Sprite.Kb.f = m_Coefficients[uSet].Kb.f;
    m_Sprite.Kc.f = m_Coefficients[uSet].Kc.f;
     
    if (!CSprite::ExecuteTest(uTest))
        return false;

    sprintf(m_szBuffer, "(Ka,Kb,Kc)=(%f,%f,%f), Dist %f, Format %s",
            m_Sprite.Ka.f, m_Sprite.Kb.f, m_Sprite.Kc.f, dvDistance, m_szFormat);
    BeginTestCase(m_szBuffer, uTest);

    if (BeginScene())
    {
        DWORD dwFormat = MYD3DFVF_LVERTEX;
        MYD3DLVERTEX Vertex;

        Vertex.dvX = m_From.x + dvDistance*m_LineOfSight.x;
        Vertex.dvY = m_From.y + dvDistance*m_LineOfSight.y;
        Vertex.dvZ = m_From.z + EPSILON + dvDistance*m_LineOfSight.z;
        Vertex.dcColor = m_Flags.Check(CF_TEXTURE) ? m_dcBackground : D3DCOLOR_RGBA(0,255,0,255);
        Vertex.dcSpecular = m_dcBackground;
        Vertex.dvTU = 0.5f;
        Vertex.dvTV = 0.5f;

        if (!AdjustVertexAttenuated((D3DXVECTOR3 *) &Vertex, m_Sprite.PointSize.f))
        {
            if (!EndScene())
                WriteToLog("Unable to find a valid point size!\n");

            return false;
        }

        if (!RenderPrimitive(D3DPT_POINTLIST, dwFormat, &Vertex, (DWORD) 1, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() failed.\n");
            bResult = false;
        }

        // spawn more points if this is a particularly sensitive scenario (i.e. mip map and multi-texture)

        if (m_Flags.Check(CF_SPAWN))
        {
            DWORD dwSpawn;
            FLOAT fSize;

            // point sprite scale formula:
            //   Ss = Vh*Si*sqrt(1/(A + B*De + C*(De*De))
            // Si is per vertex or per render state

            fSize = m_Sprite.PointSize.f*m_pCurrentMode->nBackBufferHeight;
            fSize *= (FLOAT) sqrt(1.0/(m_Sprite.Ka.f + dvDistance*(m_Sprite.Kb.f + dvDistance*m_Sprite.Kc.f)));

            if (fSize > 32.0f)
                dwSpawn = (DWORD) 0;
            else if (fSize > 16.0f)
                dwSpawn = (DWORD) 8;
            else
                dwSpawn = (DWORD) NSPAWN;

            if (dwSpawn > 0)
            {
                D3DXVECTOR3 Right;
                FLOAT fScale = 0.667f * dvDistance * tan(m_dvAngle/2.0f);

                FLOAT fSign[NSPAWN][2] =
                {
                    { -1.0f, -1.0f },
                    {  0.0f, -1.0f },
                    {  1.0f, -1.0f },
                    { -1.0f,  0.0f },
                    {  1.0f,  0.0f },
                    { -1.0f,  1.0f },
                    {  0.0f,  1.0f },
                    {  1.0f,  1.0f },

                    { -0.5f, -0.5f },
                    {  0.0f, -0.5f },
                    {  0.5f, -0.5f },
                    { -0.5f,  0.0f },
                    {  0.5f,  0.0f },
                    { -0.5f,  0.5f },
                    {  0.0f,  0.5f },
					{  0.5f,  0.5f },

                    { -0.25f, -0.25f },
                    {  0.0f,  -0.25f },
                    {  0.25f, -0.25f },
                    { -0.25f,  0.0f  },
                    {  0.25f,  0.0f  },
                    { -0.25f,  0.25f },
                    {  0.0f,   0.25f },
                    {  0.25f,  0.25f },
                };
                MYD3DLVERTEX pSpawn[NSPAWN];

                D3DXVec3Cross(&Right, &m_LineOfSight, &m_Up);
                D3DXVec3Normalize(&Right, &Right);

                for (UINT i = 0; i < dwSpawn; i++)
                {
                    memcpy(&pSpawn[i], &Vertex, sizeof(MYD3DLVERTEX));
                    pSpawn[i].dvX = Vertex.dvX + fScale*fSign[i][0]*m_Up.x + fScale*fSign[i][1]*Right.x;
                    pSpawn[i].dvY = Vertex.dvY + fScale*fSign[i][0]*m_Up.y + fScale*fSign[i][1]*Right.y;
                    pSpawn[i].dvZ = Vertex.dvZ + fScale*fSign[i][0]*m_Up.z + fScale*fSign[i][1]*Right.z;
                }

                if (!RenderPrimitive(D3DPT_POINTLIST, dwFormat, pSpawn, dwSpawn, NULL, 0, NULL))
                {
                    WriteToLog("RenderPrimitive() failed.\n");
                    bResult = false;
                }
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
