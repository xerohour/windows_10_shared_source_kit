#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "ResMan.h"

//-----------------------------------------------------------------------------

CDiscardBytesTest::CDiscardBytesTest()
{
	m_szTestName = "DiscardBytes Test";
	m_szCommandKey = "db";
	m_PrimCount = 5;
	m_pBackBuffer = NULL;
}

//-----------------------------------------------------------------------------

UINT CDiscardBytesTest::TestInitialize(void)
{
	HRESULT hr;
	srand(10000);

	SetTestRange(1, 9);

	UINT result = ResMan::TestInitialize();
	if ( result != D3DTESTINIT_RUN )
		return result;

	if ( !QueryPerformanceFrequency((LARGE_INTEGER*)&llFreq) )
	{
		WriteToLog("CDiscardBytesTest - QueryPerformanceFrequency failed!\n");
		return D3DTESTINIT_SKIPALL;
	}
	else
	{
		if(FAILED(hr = m_pSrcDevice->GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &m_pBackBuffer)))
		{
			WriteToLog("CDiscardBytesTest - GetBackBuffer failed hr=%s(%x).\n", m_pD3D->HResultToString(hr) ,hr);
			return D3DTESTINIT_ABORT;
		}
	}

	CheckDiscardBytesTestCaps();

	if (KeySet("output"))
	{
		if (m_dwVersion >= 0x0900)
            m_output = fopen("db9.txt", "w");
		else if (m_dwVersion >= 0x0800)
            m_output = fopen("db8.txt", "w");
		else if (m_dwVersion >= 0x0700)
            m_output = fopen("db7.txt", "w");
//		else
//            m_output = fopen("db6.txt", "w");
		if (!m_output)
			return D3DTESTINIT_ABORT;
	}
	else if (KeySet("input"))
	{
		if (m_dwVersion >= 0x0900)
            m_input = fopen("db9.txt", "r");
		else if (m_dwVersion >= 0x0800)
            m_input = fopen("db8.txt", "r");
		else if (m_dwVersion >= 0x0700)
            m_input = fopen("db7.txt", "r");
//		else
//            m_input = fopen("db6.txt", "r");
		if (!m_input)
			return D3DTESTINIT_ABORT;
	}

	return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool CDiscardBytesTest::ExecuteTest(UINT iTest)
{
	int result;
	
	startVBuffer = m_VBuffersList.begin();
	startResInfo = m_ResInfosList.begin();

	if (iTest <= 3)
	{
		startTexture = m_TexturesList.begin();
		strcpy(m_Message, m_Messages[0]);//sprintf(m_Message, "%s", m_Messages[0]);
		m_TotalNumberOfSlices = m_NumberOfSlices[0];
	}
	else if (iTest <= 6)
	{
		startCubeMap = m_CubesList.begin();
		for (int i = 0; i < m_PrimCount; i++)
		{
			startVBuffer++;
			startResInfo++;
		}
		strcpy(m_Message, m_Messages[1]);//sprintf(m_Message, "%s", m_Messages[1]);
		m_TotalNumberOfSlices = m_NumberOfSlices[1];
	}
	else
	{
		for (int i = 0; i < 2*m_PrimCount; i++)
		{
			startVBuffer++;
			startResInfo++;
		}
		startVolTex  = m_VolumesList.begin();
		strcpy(m_Message, m_Messages[2]);
		m_TotalNumberOfSlices = m_NumberOfSlices[2];
	}
		
	HRESULT hr;
	
	BeginTestCase(msgString, iTest);

	if (m_uPrevMaxMipLevel != 0)
	{
		m_uPrevMaxMipLevel = 0;
		SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
	}
	
    hr = m_pSrcDevice->BeginScene();
	if (FAILED(hr))
	{
		WriteToLog("BeginScene() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}

	DrawFunction(false);
    
	hr = m_pSrcDevice->EndScene();
	if (FAILED(hr))
	{
		WriteToLog("EndScene() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

bool CDiscardBytesTest::ProcessFrame(void)
{
	bool result = true;

	result = CheckListColors(false);
	
	if (result)
		Pass();
	else
		Fail();

	LOCKED_RECT lock;
	m_pBackBuffer->LockRect(&lock, NULL, 0);
	m_pBackBuffer->UnlockRect();
	QueryPerformanceCounter((LARGE_INTEGER*)&llStart);
	return result;
}

//-----------------------------------------------------------------------------

void CDiscardBytesTest::UpdateStatus(void)
{
	QueryPerformanceCounter((LARGE_INTEGER*)&llEnd);
	LONGLONG time = QPC_DIFFERENCE(llEnd, llStart);
	switch (m_uTestNumber)
	{
		case 1: case 4: case 7:
			WriteToLog("Initial rendering time:\n");
			break;
		case 2: case 5: case 8:
			WriteToLog("Second rendering time:\n");
			break;
		case 3: case 6: case 9:
			WriteToLog("Rendering time after calling %s\n", (m_dwVersion >= 0x0800 ? "DiscardBytes(): " : "EvictManagedTextures(): ") );
			break;
	}
	WriteToLog(" %d\n", time*1000000/llFreq);
	if (m_uTestNumber == 2 || m_uTestNumber == 5 || m_uTestNumber == 8 )
	{
		m_pSrcDevice->EvictManagedResources();
		LOCKED_RECT lock;
		m_pBackBuffer->LockRect(&lock, NULL, 0);
		m_pBackBuffer->UnlockRect();
	}

    ListTest::UpdateStatus();
}

//-----------------------------------------------------------------------------

void CDiscardBytesTest::CheckDiscardBytesTestCaps()
{
	if ( !m_UseTextures )
	{
		WriteToLog("DiscardBytesTest: textures are skipped.\n");
		SkipTestRange(1, 3);
	}
	if ( !m_UseCubeMaps )
	{
		WriteToLog("DiscardBytesTest: cube maps are not supported.\n");
		SkipTestRange(4, 6);
	}
	if ( !m_UseVolMaps )
	{
		WriteToLog("DiscardBytesTest: volume textures are not supported.\n");
		SkipTestRange(7, 9);
	}
}

//-----------------------------------------------------------------------------

bool CDiscardBytesTest::TestTerminate()
{
	Cleanup();

	if (m_input)
		fclose( m_input);
	if (m_output)
		fclose(m_output);

	RELEASE(m_pBackBuffer);

	return true;
}

//-----------------------------------------------------------------------------

bool CDiscardBytesTest::Setup()
{
	ResMan::Setup();

	if (!FindMaxTexSizes())
		return false;
	
	DWORD temp; 

	if ( m_UseTextures )
	{
		temp = 0;
		for (int i = 0; i < m_PrimCount; i++)
		{
			AddTextureToList(i);
			msgString[0] = '\0';
			temp += m_TotalNumberOfSlices;
		}
		strcpy(m_Messages[0], m_Message);//sprintf(m_Messages[0], "%s", m_Message);
		m_Message[0] = '\0';
		m_NumberOfSlices[0] = temp;
	}
	else
	{
		for (int i = 0; i < m_PrimCount; i++)
		{
			m_VBuffersList.insert(m_VBuffersList.end(), (CVertexBuffer *) NULL);
			m_ResInfosList.insert(m_ResInfosList.end(), (RESINFO *) NULL);
		}
	}

	if ( m_UseCubeMaps )
	{
		temp = 0;
		for (int i = 0; i < m_PrimCount; i++)
		{
		
			AddCubeMapToList(i+m_PrimCount);
			msgString[0] = '\0';
			temp += m_TotalNumberOfSlices;
		}
		strcpy(m_Messages[1], m_Message);//sprintf(m_Messages[1], "%s", m_Message);
		m_Message[0] = '\0';
		m_NumberOfSlices[1] = temp;
	}
	else
	{
		for (int i = 0; i < m_PrimCount; i++)
		{
			m_VBuffersList.insert(m_VBuffersList.end(), (CVertexBuffer *) NULL);
			m_ResInfosList.insert(m_ResInfosList.end(), (RESINFO *) NULL);
		}
	}

	if ( m_UseVolMaps )
	{
		temp = 0;
		for (int i = 0; i < m_PrimCount; i++)
		{
			AddVolTextureToList(i+2*m_PrimCount);
			msgString[0] = '\0';
			temp += m_TotalNumberOfSlices;
		}
		strcpy(m_Messages[2], m_Message);//sprintf(m_Messages[2], "%s", m_Message);
		m_Message[0] = '\0';
		m_NumberOfSlices[2] = temp;
	}
	else
	{
		for (int i = 0; i < m_PrimCount; i++)
		{
			m_VBuffersList.insert(m_VBuffersList.end(), (CVertexBuffer *) NULL);
			m_ResInfosList.insert(m_ResInfosList.end(), (RESINFO *) NULL);
		}
	}

	return true;
}

//-----------------------------------------------------------------------------

void CDiscardBytesTest::Cleanup()
{
	ResMan::Cleanup();

    DestroyAllDefaultTexList();
	DestroyAllTexturesFromList();
	DestroyAllCubeMapsFromList();
	DestroyAllVolTexturesFromList();
	DestroyAllVertexBuffersFromList();
	DestroyAllResInfosFromList();
}