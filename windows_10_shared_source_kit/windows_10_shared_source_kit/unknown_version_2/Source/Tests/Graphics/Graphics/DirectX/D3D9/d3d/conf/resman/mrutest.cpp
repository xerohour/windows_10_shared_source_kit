#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "ResMan.h"

CMRUTest::CMRUTest(PRIM_TYPES types)
{
	if (types == IS_TEXTURE)
	{
		m_Options.D3DOptions.fMinDXVersion = max(7.f, m_Options.D3DOptions.fMinDXVersion);
		m_szTestName   = "MRU Texture Test";
		m_szCommandKey = "mrutex";
		m_testType     = IS_TEXTURE;
		m_PrimCount    = 4;
	}
	else if (types == IS_CUBEMAP)
	{
		m_Options.D3DOptions.fMinDXVersion = max(7.f, m_Options.D3DOptions.fMinDXVersion);
		m_szTestName   = "MRU CubeMap Test";
		m_szCommandKey = "mrucube";
		m_testType     = IS_CUBEMAP;
		m_PrimCount    = 1;
	}
	else if (types == IS_VOLUME)
	{
		m_Options.D3DOptions.fMinDXVersion = max(8.f, m_Options.D3DOptions.fMinDXVersion);
		m_szTestName   = "MRU Volume Test";
		m_szCommandKey = "mruvol";
		m_testType     = IS_VOLUME;
		m_PrimCount    = 1;
	}
	else if (types == IS_NULL)
	{
		m_Options.D3DOptions.fMinDXVersion = max(8.f, m_Options.D3DOptions.fMinDXVersion);
		m_szTestName   = "MRU Mixed Test";
		m_szCommandKey = "mrumix";
		m_testType     = IS_NULL;
		m_PrimCount    = 1;
	}
	//m_Options.TestOptions.dwCaps |= TESTCAPS_SERIALEXECUTION;
}

//-----------------------------------------------------------------------------

UINT CMRUTest::TestInitialize(void)
{
	UINT   result;
	TCHAR  tcsBuffer[100];

	srand(10000);

	m_FixedEnd      = false;
	m_IsOutOfMemory = false;
	m_CountConstMemoryLeft     = 0;
	m_CountNegativeMemoryUsage = 0;
	m_EndIndex = NUMBER_OF_TESTS;
	m_ExtraCount = 0;

	SetTestRange(1, NUMBER_OF_TESTS);

	if ( KeySet("WHQL") || !KeySet(_T("D3DManage")) )
		return D3DTESTINIT_SKIPALL;

	result = ResMan::TestInitialize();
	if ( result != D3DTESTINIT_RUN )
		return result;

	m_IsHWProcessing = m_pSrcDevice->GetBehaviors() & DEVICECREATE_HWVP;

	switch (m_testType)
	{
		case IS_TEXTURE:
		{
			m_MinExhaustNum = 100;
			if (!m_UseTextures)
			{
				WriteToLog("CMRUTest: Textures are not supported.\n");
				return D3DTESTINIT_SKIPALL;
			}
			m_UseTextures  = true;
			m_UseCubeMaps  = false;
			m_UseVolMaps   = false;
			break;
		}
		case IS_CUBEMAP:
		{
			m_MinExhaustNum = 60;
			if (!m_UseCubeMaps)
			{
				WriteToLog("CMRUTest: Cube maps are not supported.\n");
				return D3DTESTINIT_SKIPALL;
			}
			m_UseTextures  = false;
			m_UseCubeMaps  = true;
			m_UseVolMaps   = false;
			break;
		}
		case IS_VOLUME:
		{
			m_MinExhaustNum = 60;
			if (!m_UseVolMaps)
			{
				WriteToLog("CMRUTest: Volumes are not supported.\n");
				return D3DTESTINIT_SKIPALL;
			}
			m_UseTextures  = false;
			m_UseCubeMaps  = false;
			m_UseVolMaps   = true;
			break;
		}
		case IS_NULL:
		{
			m_MinExhaustNum = m_IsHWProcessing ? 100 : 60;
			break;
		}
		default:
			WriteToLog("CMRUTest: invalid m_testType %d.\n", m_testType);
			return D3DTESTINIT_ABORT;

	}
	//get initial video memory
	m_GetMemPrev = GetAvailableTextureMem();
#if !_WIN64
	{
		const UINT VIDMEM_CLAMP_2GB = ((UINT)((((ULONGLONG)2) * 1024 * 1024 * 1024) - (64 * 1024)));
		if (m_GetMemPrev > VIDMEM_CLAMP_2GB)
			m_GetMemPrev = VIDMEM_CLAMP_2GB;
	}
#endif

	if (KeySet("showmem"))
		WriteToLog("Initial memory=%d.\n", m_GetMemPrev);
	//see if we should run the fixed number of cases
	int temp;
	TCHAR tempStr[30];
	sprintf(tempStr, "%sExhaust", m_szCommandKey);
	m_FixedEnd = ReadInteger(tempStr, &temp);
	if (m_FixedEnd)
	{
		m_EndIndex = (DWORD)temp;
		WriteToLog("CMRUTest: Memory is exhausted on test %d\n", m_EndIndex);
	}
	//see if we should output the data into the file
	if (KeySet("output"))
	{
		sprintf(tempStr, "%s%d.txt", m_szCommandKey, (int)(m_pD3D->GetVersion()));
        m_output = fopen(tempStr, "w");
		if (NULL == m_output || fprintf(m_output, "%s\n", "      ") != 7)
			return D3DTESTINIT_ABORT;
	}
	//see if we should use the data from the file
	else if (KeySet("input"))
	{
		sprintf(tempStr, "%s%d.txt", m_szCommandKey, (int)(m_pD3D->GetVersion()));
        m_input = fopen(tempStr, "r");
		m_FixedEnd = true;
		if (m_input)
		{
			if ( fscanf(m_input, "%d", &m_EndIndex) == 1)
				WriteToLog("CMRUTest: Memory is exhausted at test %d\n", m_EndIndex);
			else
			{
				WriteToLog("CMRUTest: Error reading the file.\n");
				return D3DTESTINIT_ABORT;
			}
		}
		else
		{
			WriteToLog("CMRUTest: Error opening the file for reading.\n");
			return D3DTESTINIT_ABORT;
		}
	}

	return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool CMRUTest::ExecuteTest(UINT iTest)
{
	msgString[0] = '\0';
	m_Message[0] = '\0';

	int  result, count_fail = 0, count_skip = 0;
	
	UINT uStartIndex = iTest;
	if (iTest != 1 && (iTest == m_pRangeList->uStart || (m_pConstraints != NULL && iTest == m_pConstraints->uStart)))
		uStartIndex = 1;

	for (UINT index = uStartIndex; index <= iTest; index++)
	{
		if (!m_FixedEnd && index > 1 && index <= m_EndIndex	&& m_CurrentOccupiedMemory > 300000)
		{
			m_GetMem = GetAvailableTextureMem();
#if !_WIN64
			{
				const UINT VIDMEM_CLAMP_2GB = ((UINT)((((ULONGLONG)2) * 1024 * 1024 * 1024) - (64 * 1024)));
				if (m_GetMem > VIDMEM_CLAMP_2GB)
					m_GetMem = VIDMEM_CLAMP_2GB;
			}
#endif

			if (m_GetMem == m_GetMemPrev)
			{
				m_CountConstMemoryLeft++;
				if (KeySet("showmem"))
					WriteToLog("%d: GetMemPrev=%u, GetMem=%u, UsedMem=%u.\n", index, m_GetMemPrev, m_GetMem, m_TotalUsedMem);
			}
			else if (m_GetMem >= m_GetMemPrev)
			{
				m_CountNegativeMemoryUsage++;
				if (KeySet("showmem"))
					WriteToLog("%d: GetMemPrev=%u, GetMem=%u, UsedMem=%u.\n", index, m_GetMemPrev, m_GetMem, m_TotalUsedMem);
			}
			else
				m_CountConstMemoryLeft = 0;
			m_GetMemPrev = m_GetMem;
			if ( (m_CountConstMemoryLeft >= 3 || m_CountNegativeMemoryUsage >= 2) && 
				index < m_EndIndex && index >= m_MinExhaustNum )
			{
				m_EndIndex = index;
				if (KeySet("showmem"))
					WriteToLog("%d: AvailableMem=%d, UsedMem=%d.\n", index, m_GetMem, m_TotalUsedMem);
				WriteToLog("Memory is exhausted at test %d\n", index);
			}
		}
		if ( m_IsOutOfMemory && index < m_EndIndex )
		{
			m_EndIndex = index;
			if (KeySet("showmem"))
				WriteToLog("%d: AvailableMem=%u, UsedMem=%u.\n", index, GetAvailableTextureMem(), m_TotalUsedMem);
			WriteToLog("Memory is exhausted at test %d\n", index);
		}

		//add additional resources to make sure that we exhausted video memory
		if ( index == m_EndIndex )
		{
			DWORD temp = m_TotalNumberOfSlices;
			m_ExtraCount = m_IsOutOfMemory ? 0 : m_PrimCount * (DWORD)(ceil(m_ResInfosList.size()/(m_PrimCount*10.f)));
			for( DWORD count = m_ExtraCount; count > 0; count-- )
			{
				LONG rand_number = 0;
				if (m_input)
				{
					if ( fscanf( m_input, "%d", &rand_number) != 1 || 
						rand_number > IS_VOLUME || rand_number < IS_NULL )
						return false;
				}
				else if (m_testType == IS_NULL)
				{
					bool go = false;
					while (!go)
					{
						if (m_IsHWProcessing)
							rand_number = rand()%8 + 1;
						else
							rand_number = rand()%3 + 6;
						switch (rand_number)
						{
							case IS_32INDEXBUFFER:
								if (m_Use32BitIB)  go = true; break;
							case IS_TEXTURE:
								if (m_UseTextures) go = true; break;
							case IS_CUBEMAP: 
								if (m_UseCubeMaps) go = true; break;
							case IS_VOLUME:
								if (m_UseVolMaps)  go = true; break;
							default:
								go = true; break;
						}
					}
				}
				else
					rand_number = m_testType;
				if (m_output)
					fprintf(m_output, "%d ", rand_number);
				int texIndex = m_ExtraCount-count+(index-1)*m_PrimCount;
				switch (rand_number)
				{
					case IS_16INDEXBUFFER: 
					case IS_32INDEXBUFFER:	result = AddIndexBufferToList(rand_number);  break;
					case IS_TLVERTEXBUFFER:	result = AddTLVertexBufferToList();          break;
					case IS_LVERTEXBUFFER:  result = AddLVertexBufferToList();           break;
					case IS_VERTEXBUFFER:	result = AddVertexBufferToList();            break;
					case IS_TEXTURE: 		result = AddTextureToList(texIndex);         break;
					case IS_CUBEMAP:		result = AddCubeMapToList(texIndex);         break;
					case IS_VOLUME:			result = AddVolTextureToList(texIndex);      break;
				}
				msgString[0] = '\0';
				m_Message[0] = '\0';
				temp           += m_TotalNumberOfSlices;
				m_TotalUsedMem += m_CurrentOccupiedMemory;
				
				if (index == iTest)
				{
					if ( result == FAIL )
						count_fail++;
					else if ( result == SKIP )
						count_skip++;
				}
				else
				{
					startResInfo = m_ResInfosList.end();
					startIBuffer = m_IBuffersList.end();
					startVBuffer = m_VBuffersList.end();
					startTexture = m_TexturesList.end();
					startCubeMap = m_CubesList.end();
					startVolTex  = m_VolumesList.end();
					--startResInfo;
					if ( (*startResInfo)->Type != IS_NULL )
					{
						--startVBuffer;
						(*startVBuffer)->PreLoad();
						switch ( (*startResInfo)->Type )
						{
							case IS_16INDEXBUFFER: 
							case IS_32INDEXBUFFER: --startIBuffer; (*startIBuffer)->PreLoad(); break;
							case IS_TEXTURE:       --startTexture; (*startTexture)->PreLoad(); break;
							case IS_CUBEMAP:       --startCubeMap; (*startCubeMap)->PreLoad(); break;
							case IS_VOLUME:        --startVolTex;  (*startVolTex)->PreLoad();  break;
						}
					}
				}
			}
			m_TotalNumberOfSlices = temp;
			//m_ExtraCount = m_ResInfosList.size()/m_PrimCount + 1;
			SkipTestRange(2*m_EndIndex + m_ExtraCount/m_PrimCount, NUMBER_OF_TESTS);
			if (KeySet("showmem"))
				WriteToLog("%3d: AvailableMem=%12u, UsedMem=%12u.\n", index, GetAvailableTextureMem(), m_TotalUsedMem);
		}
	
		//adding resources
		if ( index < m_EndIndex )
		{
			DWORD temp = m_TotalNumberOfSlices;
			LONG rand_number = 0;
			for( DWORD i = 0; i < m_PrimCount; i++ )
			{
				if (m_input)
				{
					if ( fscanf( m_input, "%d", &rand_number) != 1 || 
						rand_number > IS_VOLUME || rand_number < IS_NULL )
						return false;
				}
				else if (m_testType == IS_NULL)
				{
					bool go = false;
					while (!go)
					{
						if (m_IsHWProcessing)
							rand_number = rand()%8 + 1;
						else
							rand_number = rand()%3 + 6;
						switch (rand_number)
						{
							case IS_32INDEXBUFFER:
								if (m_Use32BitIB)  go = true; break;
							case IS_TEXTURE:
								if (m_UseTextures) go = true; break;
							case IS_CUBEMAP: 
								if (m_UseCubeMaps) go = true; break;
							case IS_VOLUME:
								if (m_UseVolMaps)  go = true; break;
							default:
								go = true; break;
						}
					}
				}
				else
					rand_number = m_testType;
				if (m_output)
					fprintf(m_output, "%d ", rand_number);
				int texIndex = (index-1)*m_PrimCount+i;
				
				switch (rand_number)
				{
					case IS_16INDEXBUFFER:    
					case IS_32INDEXBUFFER:  result = AddIndexBufferToList(rand_number); break;
					case IS_TLVERTEXBUFFER:	result = AddTLVertexBufferToList();         break;
					case IS_LVERTEXBUFFER:  result = AddLVertexBufferToList();          break;
					case IS_VERTEXBUFFER:	result = AddVertexBufferToList();           break;
					case IS_TEXTURE: 		result = AddTextureToList(texIndex);        break;
					case IS_CUBEMAP:		result = AddCubeMapToList(texIndex);        break;
					case IS_VOLUME:			result = AddVolTextureToList(texIndex);     break;
				}
				
				temp           += m_TotalNumberOfSlices;
				m_TotalUsedMem += m_CurrentOccupiedMemory;
				
				if (index == iTest)
				{
					if ( result == FAIL )
						count_fail++;
					else if ( result == SKIP )
						count_skip++;
				}
				else
				{
					msgString[0] = '\0';
					m_Message[0] = '\0';

					startResInfo = m_ResInfosList.end();
					startIBuffer = m_IBuffersList.end();
					startVBuffer = m_VBuffersList.end();
					startTexture = m_TexturesList.end();
					startCubeMap = m_CubesList.end();
					startVolTex  = m_VolumesList.end();
					--startResInfo;
					if ( (*startResInfo)->Type != IS_NULL )
					{
						--startVBuffer;
						(*startVBuffer)->PreLoad();
						switch ( (*startResInfo)->Type )
						{
							case IS_16INDEXBUFFER: 
							case IS_32INDEXBUFFER: --startIBuffer; (*startIBuffer)->PreLoad(); break;
							case IS_TEXTURE:       --startTexture; (*startTexture)->PreLoad(); break;
							case IS_CUBEMAP:       --startCubeMap; (*startCubeMap)->PreLoad(); break;
							case IS_VOLUME:        --startVolTex;  (*startVolTex)->PreLoad();  break;
						}
					}
				}
			}

			m_TotalNumberOfSlices = temp;
			if (KeySet("showmem") && !((index-1)%5))
				WriteToLog("%3d: AvailableMem=%12u, UsedMem=%12u.\n", index, GetAvailableTextureMem(), m_TotalUsedMem);
		}

		if (index <= m_EndIndex)
		{
			startResInfo = m_ResInfosList.end();
			startIBuffer = m_IBuffersList.end();
			startVBuffer = m_VBuffersList.end();
			startTexture = m_TexturesList.end();
			startCubeMap = m_CubesList.end();
			startVolTex  = m_VolumesList.end();
		}
		else
		{
			for (UINT i = 0; i < m_PrimCount; i++)
			{
				--startResInfo;
				if ( (*startResInfo)->Type != IS_NULL )
				{
					--startVBuffer;
					switch ( (*startResInfo)->Type )
					{
						case IS_16INDEXBUFFER: 
						case IS_32INDEXBUFFER: --startIBuffer; break;
						case IS_TEXTURE:       --startTexture; break;
						case IS_CUBEMAP:       --startCubeMap; break;
						case IS_VOLUME:        --startVolTex;  break;
					}
				}
			}
		}
	}
		
	BeginTestCase(msgString, iTest);

	if ( count_fail )
		Fail();
	
	HRESULT hr;
	
    hr = m_pSrcDevice->BeginScene();
	if (FAILED(hr))
	{
		WriteToLog("BeginScene() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	
	hr = DrawFunction(false);
    if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
	{
		m_pSrcDevice->EndScene();
        SkipTestRange(iTest, NUMBER_OF_TESTS);
        return false;
	}
	
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

bool CMRUTest::ProcessFrame(void)
{
	bool result = true;

	result = CheckListColors(false);

	if (result)
		Pass();
	else
		Fail();

	return result;
}

//-----------------------------------------------------------------------------

bool CMRUTest::TestTerminate(void)
{
	Cleanup();

	if (m_input)
		fclose( m_input);
	if (m_output)
	{
		int i = fseek( m_output, 0L, SEEK_SET );
		fprintf(m_output, "%d", m_EndIndex);
		fclose(m_output);
	}

	DestroyAllResInfosFromList();
	DestroyAllVertexBuffersFromList();
	DestroyAllIndexBuffersFromList();
	DestroyAllTexturesFromList();
	DestroyAllCubeMapsFromList();
	DestroyAllVolTexturesFromList();

	return true;
}

//-----------------------------------------------------------------------------

bool CMRUTest::Setup()
{
	ResMan::Setup();

	if (!KeySet("nofillmem"))
		m_TotalUsedMem = FillVideoMemory();
	else
		m_TotalUsedMem = 0;
	return true;
}

//-----------------------------------------------------------------------------

void CMRUTest::Cleanup()
{
	ResMan::Cleanup();

	DestroyAllDefaultTexList();
}
