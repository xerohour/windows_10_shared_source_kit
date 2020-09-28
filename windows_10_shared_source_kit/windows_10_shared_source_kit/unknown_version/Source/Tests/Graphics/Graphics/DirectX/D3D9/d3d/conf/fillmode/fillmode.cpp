// FILE:        FillMode.cpp
// COMMENTS:    This test checks that all fill mode
//				work correctly.
//
// AUTHOR: Anthony Clarke

#include "FillMode.h"
#include "assert.h"
#include "math.h"

CD3DWindowFramework TheApp;

CSolidFill SolidFill;
CLineFill LineFill;
CPointFill PointFill;

// Global variables
DWORD g_rgSnakeColors[NUM_COLORS] =
{    
    { 0xFFFF0000 }, //Red
    { 0xFF00FF00 }, //Green
    { 0xFF0000FF }, //Blue
};

CFillMode::CFillMode()
{	
    // Init Framework options
	SetTestType(TESTTYPE_CONF);
	m_pFramework->GetImageCmp()->SetMinPixels(0);

	// Force test to use relevant DirectX versions
	m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81 | DXVERSION_7 | DXVERSION_6;
}

CFillMode::~CFillMode()
{
}

void CFillMode::CommandLineHelp()
{
	//WriteCommandLineHelp("$y-RevRelease\t$wReleases the textures in queue order (FIFO)");
}

// NAME:        TestInitialize()
// DESC:        Initialized the test
// INPUT:       None
// OUTPUT:      D3DTESTINIT_RUN - The test initialized successfully
//              D3DTESTINIT_SKIPALL - If all tests have been eliminated from running
//              D3DTESTINIT_ABORT - The HW was incapable of running the test, or there was a fatal error
UINT CFillMode::TestInitialize()
{
	m_Flags.Replace(0);    
	m_fTenthWidth = (float)m_pCurrentMode->nBackBufferWidth / 10.0f;
	m_fTenthHeight = (float)m_pCurrentMode->nBackBufferHeight / 10.0f;
	m_nVarsPerType = DEFAULT_PRIMTYPE_VARS;
	m_nTotalVariations = m_nVarsPerType * NUM_PRIMTYPES;
	m_uCurrentPrimType = 0;
	m_fPointSize = 1;
	m_dwVertexCount = SNAKE_SIZE;
	
    // 
    // Perform caps checking
    //
	if (!DoCapsCheck())
		return D3DTESTINIT_ABORT;
    // Set test ranges 
    SetTestRange(1, m_nTotalVariations);

    // Set render states
    SetRenderState(D3DRS_LIGHTING, (DWORD)FALSE);  
	SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);
	return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        Displays one frame of the test
// INPUT:       The frame number to display
// OUTPUT:      true - The frame was displayed
//              false - otherwise
bool CFillMode::ExecuteTest(UINT uiTest)
{
	UINT uPrimType;
	TCHAR   szBuffer[MAX_PATH];
	bool bResult = true;
       
	uiTest--; // This makes the range 0 - (testrange-1)
	uPrimType = uiTest/m_nVarsPerType;
	uPrimType++;	
	//Half way through the vars/type change point size if
	//pointsprites are supported.
	if ( ((uPrimType*m_nVarsPerType) - uiTest) <= (m_nVarsPerType/2.0f) )
	{
		if ( m_Flags.Check(POINTSPRITE) )
		{
			//invalidate the current primtype.
			m_uCurrentPrimType = 0;
			SetPointSize();
        }
	}
    else
    {
        ClearPointSize();
    }
	//now make sure uPrimtype is in the range 1 to 6 inclusive.
	uPrimType = uPrimType % (NUM_PRIMTYPES+1);
	if (0 == uPrimType)
		uPrimType = 1;

	if (uPrimType != m_uCurrentPrimType)
	{
		//This is so we can reset the point size to 1
		//at the beginning of a primtype run.
		if ( m_uCurrentPrimType != 0)
			ClearPointSize();
		
		m_uCurrentPrimType = uPrimType;		
		switch (m_uCurrentPrimType)
		{
			case D3DPT_POINTLIST :
				_tcscpy( m_sztCurrentPrimType, _T("POINTLIST") );
				break;
			case D3DPT_LINELIST :
				_tcscpy( m_sztCurrentPrimType, _T("LINELIST") );
				break;
			case D3DPT_LINESTRIP :
				_tcscpy( m_sztCurrentPrimType, _T("LINESTRIP") );
				break;
			case D3DPT_TRIANGLELIST :
				_tcscpy( m_sztCurrentPrimType, _T("TRIANGLELIST") );
				break;
			case D3DPT_TRIANGLESTRIP :
				_tcscpy( m_sztCurrentPrimType, _T("TRIANGLESTRIP") );
				break;
			case D3DPT_TRIANGLEFAN :
				_tcscpy( m_sztCurrentPrimType, _T("D3DPT_TRIANGLEFAN") );
				break;
		}
	}
	sprintf(szBuffer, "Primitive Type: %s Point Size: %f", m_sztCurrentPrimType, m_fPointSize);
    BeginTestCase(szBuffer, uiTest+1);

	WriteStatus("$gLemniSnake Primitive Type", "%s", m_sztCurrentPrimType);
	WriteStatus("$gPoint Size", "%f", m_fPointSize);

	GenerateSnake(uiTest);

	if (BeginScene())
    {
		if (!RenderPrimitive((D3DPRIMITIVETYPE)m_uCurrentPrimType, D3DFVF_TLVERTEX, m_rgSnakeVertices, m_dwVertexCount, NULL, 0, NULL))
		{
			WriteToLog(_T("RenderPrimitive() %s failed.\n"), m_sztCurrentPrimType);
			bResult = false;
		}
		
		if ( m_uCurrentPrimType == D3DPT_TRIANGLEFAN && !m_Flags.Check(KILLSNAKE2) )
		{
			if (!RenderPrimitive((D3DPRIMITIVETYPE)m_uCurrentPrimType, D3DFVF_TLVERTEX, m_rgSnakeVertices2, m_dwVertexCount, NULL, 0, NULL))
			{
				WriteToLog(_T("RenderPrimitive() %s failed.\n"), m_sztCurrentPrimType);
				bResult = false;
			}
		}

		if (!EndScene())
        {
            WriteToLog(_T("(ERROR) EndScene() failed: (%08X)\n"), GetLastError());
            Fail();
        }
	}    

	if ( m_Flags.Check(INVALID) || NeedDx6TLVertWorkAround() )
	{
        WriteStatus("$gPoint Size", "N/A");
		SetCompareSrcRef(false);
		Pass();
	}
	else
		SetCompareSrcRef(true);

    return true;
}

// NAME:    NeedDx6TLVertWorkAround()
// DESC:
//This function is here due to manbug #51868. Vertex buffers that contain
//RHW FVF values cannot be clipped by the runtime in Dx6. This means that 
//Process Vertices cannot be called for vertex buffers of this type.
//In Dx7 ProcessVertices can be called on T&Lverts. For Dx8 ProcessVertices
//cannot be called for T&Lverts however by default the runtime will
//initialize the internal clip list the first time a vertex buffer is used.
//This clip list data is however never updated.
bool CFillMode::NeedDx6TLVertWorkAround(void)
{
    if (m_dwVersion < 0x0700)
    {
        if ( m_dwRPCurrentAPI == RP_DIPVB || m_dwRPCurrentAPI == RP_DPVB ||
             m_dwRPCurrentAPI == RP_DPVB_OPT || m_dwRPCurrentAPI == RP_DIPVB_OPT )
        {
            if (!AllSnakeInView() && !m_Flags.Check(GUARDBAND))
                return true;
        }
        
    }
    return false;
}

bool CFillMode::ProcessFrame(void)
{
    return CD3DTest::ProcessFrame();
}

// NAME:        TestTerminate()
// DESC:        terminate test
// INPUT:       none
// OUTPUT:      true
bool CFillMode::TestTerminate(VOID)
{
	return true;
}

// NAME:        DoCapsCheck()
// DESC:        Looks for any relevant caps
// INPUT:       none
// OUTPUT:      true
bool CFillMode::DoCapsCheck()
{
    CAPS SrcDesc;
    bool bPass = true;

	m_Flags.Clear(POINTSPRITE);
	if (m_dwVersion >= 0x0800)
	{
		m_pD3D->GetDeviceCaps(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), &SrcDesc);
	
		if (SrcDesc.MaxPointSize > 1.0f)	
			m_Flags.Set(POINTSPRITE);
	}
    if ( m_pSrcDevice->GetCaps()->dvGuardBandLeft != m_pSrcDevice->GetCaps()->dvGuardBandRight )
    {
        //We have guard band.
        m_Flags.Set(GUARDBAND);
    }
    return bPass;
}

inline float round_to_int( float f )
{
	return floorf( f + .5f );
}

// NAME:        GenerateSnake()
// DESC:        This is the guts of this test.
//				Generates the points that make up the
//				Snake in the correct format for the primitive
//				type along a path generated by the path class.
//				In this case a Lemniscate.
// INPUT:       Test number.
// OUTPUT:      none (snake array is filled)
void CFillMode::GenerateSnake (UINT uiTest)
{
	D3DXVECTOR3 vecStart, vecEnd, vecOrg, vecCenter;
	D3DXVECTOR3 vecX, vecY, vecZ, vecRes, vecTemp;
	UINT uiSnakeStepLength;
	int i, nStep;

	//Init vertex count to be the size of the entire snake array.
	m_dwVertexCount = SNAKE_SIZE;

	//Initialize color, z and rhw value for snake 
	for (i = 0; i < SNAKE_SIZE; ++i)
	{
		m_rgSnakeVertices[i].sz = 0.5;
		m_rgSnakeVertices[i].rhw = 1.0;
		m_rgSnakeVertices[i].color = g_rgSnakeColors[i%NUM_COLORS];
		m_rgSnakeVertices[i].specular = 0xFFFFFFFF;
	}
	vecCenter.x = m_fTenthWidth * 5.0f;
	vecCenter.y = m_fTenthHeight * 5.0f;
	vecCenter.z = 0.5f;
	m_Path.InitLemniscate(&vecCenter, LEMNISNAKE_STEPS, m_fTenthWidth * 6.0f, 2);

	switch ((D3DPRIMITIVETYPE)m_uCurrentPrimType)
	{
		case D3DPT_POINTLIST :
		case D3DPT_LINELIST :
		case D3DPT_TRIANGLESTRIP :
		case D3DPT_LINESTRIP :
		{				
			uiSnakeStepLength = SNAKE_SIZE/2;
			nStep = uiTest%m_nVarsPerType;
			nStep -= uiSnakeStepLength;
			for (i = 0; i < SNAKE_SIZE-1; i = i + 2)
			{				
				m_Path.GetStep(nStep, &vecOrg, &vecX, &vecY, &vecZ);
				D3DXVec3Subtract(&vecRes, &vecOrg, D3DXVec3Scale(&vecTemp, &vecX, m_fTenthHeight));
				//The cast to int means that these are all whole numbers
                //and hence precission errors will be avoided.
                m_rgSnakeVertices[i].sx = round_to_int( vecRes.x );
				m_rgSnakeVertices[i].sy = round_to_int( vecRes.y );
				D3DXVec3Add(&vecRes, &vecOrg, &vecTemp);
				m_rgSnakeVertices[i+1].sx = round_to_int( vecRes.x );
				m_rgSnakeVertices[i+1].sy = round_to_int( vecRes.y );
				nStep++;
			}
			break;
		}		
		case D3DPT_TRIANGLELIST :
		{
			uiSnakeStepLength = SNAKE_SIZE/3;
			nStep = uiTest%m_nVarsPerType;
			nStep -= uiSnakeStepLength;
			m_Path.GetStep(nStep, &vecOrg, &vecX, &vecY, &vecZ);
			nStep++;
			D3DXVec3Subtract(&vecRes, &vecOrg, D3DXVec3Scale(&vecTemp, &vecX, m_fTenthHeight));
			m_rgSnakeVertices[0].sx = round_to_int( vecRes.x );
			m_rgSnakeVertices[0].sy = round_to_int( vecRes.y );
			D3DXVec3Add(&vecRes, &vecOrg, &vecTemp);
			m_rgSnakeVertices[1].sx = round_to_int( vecRes.x );
			m_rgSnakeVertices[1].sy = round_to_int( vecRes.y );
			for (i = 2; i < ((uiSnakeStepLength-1)*3); i = i + 3)
			{	
				m_Path.GetStep(nStep, &vecOrg, &vecX, &vecY, &vecZ);
				nStep++;
				D3DXVec3Subtract(&vecRes, &vecOrg, D3DXVec3Scale(&vecTemp, &vecX, m_fTenthHeight));
				m_rgSnakeVertices[i].sx = round_to_int( vecRes.x );
				m_rgSnakeVertices[i].sy = round_to_int( vecRes.y );
				m_rgSnakeVertices[i+1].sx = round_to_int( vecRes.x );
				m_rgSnakeVertices[i+1].sy = round_to_int( vecRes.y );
				D3DXVec3Add(&vecRes, &vecOrg, &vecTemp);
				m_rgSnakeVertices[i+2].sx = round_to_int( vecRes.x );
				m_rgSnakeVertices[i+2].sy = round_to_int( vecRes.y );								
			}
			m_Path.GetStep(nStep, &vecOrg, &vecX, &vecY, &vecZ);
			D3DXVec3Subtract(&vecRes, &vecOrg, D3DXVec3Scale(&vecTemp, &vecX, m_fTenthHeight));
			m_rgSnakeVertices[i].sx = round_to_int( vecRes.x );
			m_rgSnakeVertices[i].sy = round_to_int( vecRes.y );
			m_dwVertexCount = i+1;
			break;
		}	    
		case D3DPT_TRIANGLEFAN :
		{	
			//Snake 1
			nStep = uiTest%m_nVarsPerType;
			m_Path.GetStep(nStep, &vecOrg, &vecX, &vecY, &vecZ);
			m_rgSnakeVertices[0].sx = round_to_int( vecOrg.x );
			m_rgSnakeVertices[0].sy = round_to_int( vecOrg.y );
			//Now use nStep for the fan.			 
			m_fTriFanTheta = (2.0f*pi)/float(SNAKE_SIZE-1);
			for (i = 1; i < SNAKE_SIZE; ++i)
			{				
				m_rgSnakeVertices[i].sx = round_to_int(m_rgSnakeVertices[0].sx + ( 2.0f * m_fTenthWidth * cos( float(nStep) * m_fTriFanTheta ) ) );
				m_rgSnakeVertices[i].sy = round_to_int(m_rgSnakeVertices[0].sy - ( 2.0f * m_fTenthWidth * sin( float(nStep) * m_fTriFanTheta ) ) );
				nStep++;
			}

			//The reason snake 2 exists is to provide more primitives
			//on the screen in wireframe mode to allow for greater precision error.
			//Snake 2 
			nStep = uiTest%m_nVarsPerType;
			//Start snake 2 3rd of the way down the path
			nStep += (LEMNISNAKE_STEPS/3); 
			m_Path.GetStep(nStep, &vecOrg, &vecX, &vecY, &vecZ);
			m_rgSnakeVertices2[0].sx = round_to_int( vecOrg.x );
			m_rgSnakeVertices2[0].sy = round_to_int( vecOrg.y );
			m_rgSnakeVertices2[0].sz = 0.5;
			m_rgSnakeVertices2[0].rhw = 1.0;
			m_rgSnakeVertices2[0].color = g_rgSnakeColors[i%NUM_COLORS];
			m_rgSnakeVertices2[0].specular = 0xFFFFFFFF;
			//Now use nStep for the fan.
			m_fTriFanTheta = (2.0f*pi)/float(SNAKE_SIZE-1);
			for (i = 1; i < SNAKE_SIZE; ++i)
			{					
				m_rgSnakeVertices2[i].sx = round_to_int( m_rgSnakeVertices2[0].sx + ( 2.0f * m_fTenthWidth * cos( float(nStep) * m_fTriFanTheta ) ) );
				m_rgSnakeVertices2[i].sy = round_to_int( m_rgSnakeVertices2[0].sy - ( 2.0f * m_fTenthWidth * sin( float(nStep) * m_fTriFanTheta ) ) );
				//Init stuff
				m_rgSnakeVertices2[i].sz = 0.5;
				m_rgSnakeVertices2[i].rhw = 1.0;
				m_rgSnakeVertices2[i].color = g_rgSnakeColors[i%NUM_COLORS];
				m_rgSnakeVertices2[i].specular = 0xFFFFFFFF;
				nStep++;
			}
			break;
		}
	}
	if (m_dwVertexCount > SNAKE_SIZE)
		assert(false);
	return;
}

// NAME:        SetPointSize()
// DESC:        Sets a points size for
//				those drivers that support 
//				point sprites
// INPUT:       none
// OUTPUT:      none
void CFillMode::SetPointSize()
{
	if ( m_Flags.Check(POINTSPRITE) )
	{
		m_fPointSize = MIN_MAXPOINTSIZE;
		SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&m_fPointSize) );
	}
	return;
}

// NAME:        ClearPointSize()
// DESC:        Clears point size. If the device
//				doesn't support point sprites then
//				rs will graciously fail.
// INPUT:       none
// OUTPUT:      none 
void CFillMode::ClearPointSize()
{
	m_fPointSize = 1.0f;
	SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&m_fPointSize) );
	return;
}

bool CFillMode::InsideDisplayWindow(float fsx, float fsy)
{    
    if ((fsy >= 0) && (fsy <= m_pCurrentMode->nBackBufferHeight) 
        && (fsx >= 0) && (fsx <= m_pCurrentMode->nBackBufferWidth))
         return true;
    else
         return false;
}

bool CFillMode::AllSnakeInView()
{
    D3DTLVERTEX *pSnakeArray;
    int i, nSnakeNum;
    float fHalfPointSize;
        
    for (nSnakeNum = 0; nSnakeNum < 2; ++nSnakeNum)
    {
        if (nSnakeNum == 1)
        {
            if (m_uCurrentPrimType == D3DPT_TRIANGLEFAN && !m_Flags.Check(KILLSNAKE2))
                pSnakeArray = m_rgSnakeVertices2;
            else
                break;
        }
        else
            pSnakeArray = m_rgSnakeVertices;

        //Now check that the snake is inside the view
        for (i = 0; i < SNAKE_SIZE; ++i)
        {
            if ( !InsideDisplayWindow(pSnakeArray[i].sx, pSnakeArray[i].sy) )
                return false;
            if (m_fPointSize > 1.0)
            {    
                fHalfPointSize = m_fPointSize / 2.0f;
                // Four corners of the sprite
                // Top left
                if ( !InsideDisplayWindow(pSnakeArray[i].sx - fHalfPointSize, 
                                          pSnakeArray[i].sy - fHalfPointSize) )
                    return false;
                // Bottom left
                if ( !InsideDisplayWindow(pSnakeArray[i].sx - fHalfPointSize, 
                                          pSnakeArray[i].sy + fHalfPointSize) )
                    return false;

                // Top right
                if ( !InsideDisplayWindow(pSnakeArray[i].sx + fHalfPointSize, 
                                          pSnakeArray[i].sy - fHalfPointSize) )
                    return false;

                // Bottom right
                if ( !InsideDisplayWindow(pSnakeArray[i].sx + fHalfPointSize, 
                                          pSnakeArray[i].sy + fHalfPointSize) )
                    return false;
            }
        }
    }
    return true;
}
